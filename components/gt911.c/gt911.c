/*
*
* MIT License 28/07/2023  - P J Bone
*
*/

#include <esp_log.h>
#include <esp_err.h>
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "gt911.h"

#define TAG "GT911"

gt911_status_t gt911_status;

esp_err_t gt911_i2c_read(uint8_t dev_addr, uint16_t reg_addr, uint8_t *data_buf, uint8_t len) {

	uint8_t buf[2];

	buf[0] = (uint8_t)((reg_addr & 0xFF00) >> 8);
	buf[1] = (uint8_t)(reg_addr & 0xFF);

	return i2c_master_write_read_device(dev_addr, GT911_I2C_SLAVE_ADDR, buf, 2, data_buf, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

esp_err_t gt911_i2c_write8(uint8_t dev_addr, uint16_t reg_addr, uint8_t data) {

	uint8_t buf[3];

	buf[0] = (uint8_t)((reg_addr & 0xFF00) >> 8);
	buf[1] = (uint8_t)(reg_addr & 0xFF);
	buf[2] = data;

	return i2c_master_write_to_device(dev_addr, GT911_I2C_SLAVE_ADDR, buf, sizeof(buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

static void gt911_set_config( uint8_t dev_addr ) {

	uint8_t data_buf;

	data_buf = 0x20;											// Change our bits
	gt911_i2c_write8(dev_addr, GT911_RES_X_MAX_L, data_buf);
	data_buf = 0x03;
	gt911_i2c_write8(dev_addr, GT911_RES_X_MAX_H, data_buf);
	data_buf = 0xE0;
	gt911_i2c_write8(dev_addr, GT911_RES_Y_MAX_L, data_buf);
	data_buf = 0x01;
	gt911_i2c_write8(dev_addr, GT911_RES_Y_MAX_H, data_buf);

	//    wr_ptr=buf;													// Calculate new checksum
	uint8_t csum=0;
	for (uint16_t i=GT911_CONF_VER; i<=GT911_CONF_END; i++) {
		gt911_i2c_read(dev_addr, i, &data_buf, 1);
		csum += data_buf;
	}
	csum = (~csum) + 1;

	gt911_i2c_write8(dev_addr, GT911_CONF_CHKSUM, csum);		// Write checksum and update configuration
	data_buf = 0x01;
	gt911_i2c_write8(dev_addr, GT911_UPDATE_CONF, data_buf);
	gt911_i2c_read(dev_addr, GT911_CONF_CHKSUM, &data_buf, 1);
	ESP_LOGI(TAG, "CCS: 0x%2X, RCS: 0x%2X", csum, data_buf);
}

static esp_err_t i2c_master_init(void) {

	int i2c_master_port = 0;

	i2c_config_t conf = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = 19,
		.scl_io_num = 20,
		.sda_pullup_en = GPIO_PULLUP_DISABLE,
		.scl_pullup_en = GPIO_PULLUP_DISABLE,
		.master.clk_speed = 400000,
    };

    i2c_param_config(i2c_master_port, &conf);
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void gt911_init(uint8_t dev_addr) {

	static lv_indev_drv_t	indev_drv;

	ESP_LOGI(TAG, "Init I2C");
	ESP_ERROR_CHECK(i2c_master_init());

	if(!gt911_status.initialised) {
		gt911_status.i2c_dev_addr = dev_addr;
		uint8_t data_buf;
		esp_err_t ret;

		ESP_LOGI(TAG, "Checking for GT911 Touch Controller");
		if ((ret = gt911_i2c_read(dev_addr, GT911_PRODUCT_ID1, &data_buf, 1) != ESP_OK)) {
			ESP_LOGE(TAG, "Error reading from device: %s",
			esp_err_to_name(ret));    // Only show error the first time
            return;
		}
		ESP_LOGI(TAG, "Set GT911 Configuration");
		gt911_set_config( dev_addr );
		// Read 4 bytes for Product ID in ASCII
		for (int i = 0; i < GT911_ID_LEN; i++) {
			gt911_i2c_read(dev_addr, (GT911_PRODUCT_ID1 + i), (uint8_t *)&(gt911_status.pid[i]), 1);
		}
		ESP_LOGI(TAG, "\tProduct ID: %s", gt911_status.pid);

		gt911_i2c_read(dev_addr, GT911_VENDOR_ID, &data_buf, 1);
		ESP_LOGI(TAG, "\tVendor ID: 0x%02x", data_buf);

		gt911_i2c_read(dev_addr, GT911_X_COORD_RES_L, &data_buf, 1);
		gt911_status.max_x = data_buf;
		gt911_i2c_read(dev_addr, GT911_X_COORD_RES_H, &data_buf, 1);
		gt911_status.max_x |= ((uint16_t)data_buf << 8);
		ESP_LOGI(TAG, "\tX Resolution: %d", gt911_status.max_x);

		gt911_i2c_read(dev_addr, GT911_Y_COORD_RES_L, &data_buf, 1);
		gt911_status.max_y = data_buf;
		gt911_i2c_read(dev_addr, GT911_Y_COORD_RES_H, &data_buf, 1);
		gt911_status.max_y |= ((uint16_t)data_buf << 8);
		ESP_LOGI(TAG, "\tY Resolution: %d", gt911_status.max_y);
		gt911_status.initialised = true;

		lv_indev_drv_init(&indev_drv);	/*Basic initialisation of pointer interface for GUI is attached to USB Mouse/Touch and VNC*/
		indev_drv.type = LV_INDEV_TYPE_POINTER;
		indev_drv.read_cb = &gt911_read;         /*This function will be called periodically (by the library) to get the mouse position and state*/
		indev_drv.user_data = NULL;
		lv_indev_drv_register(&indev_drv);
	}
}

void gt911_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {

	static int16_t last_x = 0;  // 12bit pixel value
	static int16_t last_y = 0;  // 12bit pixel value
	static uint8_t last_state = LV_INDEV_STATE_REL;
	uint8_t data_buf, status_reg;

	gt911_i2c_read(gt911_status.i2c_dev_addr, GT911_STATUS_REG, &status_reg, 1);
	//    ESP_LOGI(TAG, "\tstatus: 0x%02x", status_reg);
	//    touch_pnt_cnt = status_reg & 0x0F;
	if((status_reg & GT911_STATUS_BUF) && ((status_reg & GT911_STATUS_TPT_MASK) < 6 )) {
		gt911_i2c_read(gt911_status.i2c_dev_addr, GT911_TPT1_X_COORD_L, &data_buf, 1);
		last_x = data_buf;
		gt911_i2c_read(gt911_status.i2c_dev_addr, GT911_TPT1_X_COORD_H, &data_buf, 1);
		last_x |= ((uint16_t)data_buf << 8);

		gt911_i2c_read(gt911_status.i2c_dev_addr, GT911_TPT1_Y_COORD_L, &data_buf, 1);
		last_y = data_buf;
		gt911_i2c_read(gt911_status.i2c_dev_addr, GT911_TPT1_Y_COORD_H, &data_buf, 1);
		last_y |= ((uint16_t)data_buf << 8);
		//Reset Status Reg Value
		gt911_i2c_write8(gt911_status.i2c_dev_addr, GT911_STATUS_REG, 0x00);
		last_state = LV_INDEV_STATE_PR;
		//ESP_LOGI(TAG, "X=%u Y=%u PR=%u", last_x, last_y, last_state);
    } else last_state = LV_INDEV_STATE_REL;
	data->point.x = (480.0f / 800.0f) * last_x;
	data->point.y = (272.0f/ 480.0f) * last_y;
	data->state = last_state;
    //ESP_LOGI(TAG, "X=%u Y=%u", data->point.x, data->point.y);
    //ESP_LOGV(TAG, "X=%u Y=%u", data->point.x, data->point.y);
}
