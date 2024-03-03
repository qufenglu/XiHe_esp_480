#ifndef __PJB_GT911_H
/*
*
* MIT License 28/07/2023  - P J Bone
*
*/

#define __GT911_H

#include <stdint.h>
#include <stdbool.h>
#include <lvgl.h>


#ifdef __cplusplus
extern "C" {
#endif

#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

#define GT911_I2C_SLAVE_ADDR   		0x5D

#define GT911_ID_LEN   		4

/* Register Map of GT911 */
#define GT911_CONF_VER					0x8047
#define GT911_RES_X_MAX_L			  	0x8048
#define GT911_RES_X_MAX_H			  	0x8049
#define GT911_RES_Y_MAX_L			 	0x804A
#define GT911_RES_Y_MAX_H			 	0x804B
#define GT911_CONF_END					0x80FE
#define GT911_CONF_CHKSUM				0x80FF
#define GT911_UPDATE_CONF			 	0x8100
#define GT911_PRODUCT_ID1             	0x8140
#define GT911_PRODUCT_ID2             	0x8141
#define GT911_PRODUCT_ID3             	0x8142
#define GT911_PRODUCT_ID4             	0x8143
#define GT911_FIRMWARE_VER_L          	0x8144
#define GT911_FIRMWARE_VER_H          	0x8145
#define GT911_X_COORD_RES_L           	0x8146
#define GT911_X_COORD_RES_H           	0x8147
#define GT911_Y_COORD_RES_L           	0x8148
#define GT911_Y_COORD_RES_H           	0x8149
#define GT911_VENDOR_ID               	0x814A

#define GT911_STATUS_REG              	0x814E
#define GT911_STATUS_BUF        		0x80
#define GT911_STATUS_LARGE      		0x40
#define GT911_STATUS_PROX_VALID 		0x20
#define GT911_STATUS_HAVEKEY    		0x10
#define GT911_STATUS_TPT_MASK    		0x0F

#define GT911_TRACK_ID1               	0x814F
#define GT911_TPT1_X_COORD_L           	0x8150
#define GT911_TPT1_X_COORD_H           	0x8151
#define GT911_TPT1_Y_COORD_L           	0x8152
#define GT911_TPT1_Y_COORD_H           	0x8153
#define GT911_TPT1_X_SIZE_L            	0x8154
#define GT911_TPT1_X_SIZE_H            	0x8155

typedef struct {
    char pid[GT911_ID_LEN];
    uint16_t max_x;
    uint16_t max_y;
    uint8_t i2c_dev_addr;
    bool initialised;
} gt911_status_t;

void gt911_init(uint8_t dev_addr);
void gt911_read(lv_indev_drv_t *drv, lv_indev_data_t *data);

#ifdef __cplusplus
}
#endif
#endif /* __GT911_H */

