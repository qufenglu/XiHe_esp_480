/*
* Copyright 2024 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"


void setup_scr_PlayWin(lv_ui *ui)
{
	//Write codes PlayWin
	ui->PlayWin = lv_obj_create(NULL);
	lv_obj_set_size(ui->PlayWin, 480, 272);

	//Write style for PlayWin, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->PlayWin, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes PlayWin_img_play
	ui->PlayWin_img_play = lv_img_create(ui->PlayWin);
	lv_obj_add_flag(ui->PlayWin_img_play, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_pivot(ui->PlayWin_img_play, 50,50);
	lv_img_set_angle(ui->PlayWin_img_play, 0);
	lv_obj_set_pos(ui->PlayWin_img_play, 0, 0);
	lv_obj_set_size(ui->PlayWin_img_play, 480, 272);

	//Write style for PlayWin_img_play, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->PlayWin_img_play, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes PlayWin_btn_exit
	ui->PlayWin_btn_exit = lv_btn_create(ui->PlayWin);
	ui->PlayWin_btn_exit_label = lv_label_create(ui->PlayWin_btn_exit);
	lv_label_set_text(ui->PlayWin_btn_exit_label, "X");
	lv_label_set_long_mode(ui->PlayWin_btn_exit_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->PlayWin_btn_exit_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->PlayWin_btn_exit, 0, LV_STATE_DEFAULT);
	lv_obj_set_pos(ui->PlayWin_btn_exit, 455, 0);
	lv_obj_set_size(ui->PlayWin_btn_exit, 25, 25);

	//Write style for PlayWin_btn_exit, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->PlayWin_btn_exit, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->PlayWin_btn_exit, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->PlayWin_btn_exit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->PlayWin_btn_exit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->PlayWin_btn_exit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->PlayWin_btn_exit, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->PlayWin_btn_exit, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->PlayWin_btn_exit, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes PlayWin_label_status
	ui->PlayWin_label_status = lv_label_create(ui->PlayWin);
	lv_label_set_text(ui->PlayWin_label_status, "loading...");
	lv_label_set_long_mode(ui->PlayWin_label_status, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->PlayWin_label_status, 190, 126);
	lv_obj_set_size(ui->PlayWin_label_status, 100, 20);

	//Write style for PlayWin_label_status, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->PlayWin_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->PlayWin_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->PlayWin_label_status, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->PlayWin_label_status, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->PlayWin_label_status, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->PlayWin_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->PlayWin_label_status, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->PlayWin_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->PlayWin_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->PlayWin_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->PlayWin_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->PlayWin_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->PlayWin_label_status, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Update current screen layout.
	lv_obj_update_layout(ui->PlayWin);

	
}
