/*
* Copyright 2024 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

typedef struct
{
  
	lv_obj_t *LoginWin;
	bool LoginWin_del;
	lv_obj_t *LoginWin_img_1;
	lv_obj_t *LoginWin_ta_ip;
	lv_obj_t *LoginWin_ta_port;
	lv_obj_t *LoginWin_labe_ip;
	lv_obj_t *LoginWin_label_port;
	lv_obj_t *LoginWin_btn_connet;
	lv_obj_t *LoginWin_btn_connet_label;
	lv_obj_t *LoginWin_label_2;
	lv_obj_t *PlayWin;
	bool PlayWin_del;
	lv_obj_t *PlayWin_img_play;
	lv_obj_t *PlayWin_btn_exit;
	lv_obj_t *PlayWin_btn_exit_label;
	lv_obj_t *PlayWin_label_status;
}lv_ui;

void ui_init_style(lv_style_t * style);
void init_scr_del_flag(lv_ui *ui);
void setup_ui(lv_ui *ui);
extern lv_ui guider_ui;

void setup_scr_LoginWin(lv_ui *ui);
void setup_scr_PlayWin(lv_ui *ui);
LV_IMG_DECLARE(_aurora_480x272);
LV_IMG_DECLARE(__480x272);

LV_FONT_DECLARE(lv_font_montserratMedium_12)
LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_subset4_70)


#ifdef __cplusplus
}
#endif
#endif
