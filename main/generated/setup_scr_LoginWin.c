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


void setup_scr_LoginWin(lv_ui *ui)
{
	//Write codes LoginWin
	ui->LoginWin = lv_obj_create(NULL);
	lv_obj_set_size(ui->LoginWin, 480, 272);

	//Write style for LoginWin, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->LoginWin, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes LoginWin_img_1
	ui->LoginWin_img_1 = lv_img_create(ui->LoginWin);
	lv_obj_add_flag(ui->LoginWin_img_1, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->LoginWin_img_1, &_aurora_480x272);
	lv_img_set_pivot(ui->LoginWin_img_1, 50,50);
	lv_img_set_angle(ui->LoginWin_img_1, 0);
	lv_obj_set_pos(ui->LoginWin_img_1, 0, 0);
	lv_obj_set_size(ui->LoginWin_img_1, 480, 272);

	//Write style for LoginWin_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_opa(ui->LoginWin_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes LoginWin_ta_ip
	ui->LoginWin_ta_ip = lv_textarea_create(ui->LoginWin);
	lv_textarea_set_text(ui->LoginWin_ta_ip, "192.168.12.1");
	lv_textarea_set_password_bullet(ui->LoginWin_ta_ip, "*");
	lv_textarea_set_password_mode(ui->LoginWin_ta_ip, false);
	lv_textarea_set_one_line(ui->LoginWin_ta_ip, true);
	lv_obj_set_pos(ui->LoginWin_ta_ip, 189, 120);
	lv_obj_set_size(ui->LoginWin_ta_ip, 120, 25);

	//Write style for LoginWin_ta_ip, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->LoginWin_ta_ip, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->LoginWin_ta_ip, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->LoginWin_ta_ip, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->LoginWin_ta_ip, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->LoginWin_ta_ip, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->LoginWin_ta_ip, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->LoginWin_ta_ip, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->LoginWin_ta_ip, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->LoginWin_ta_ip, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->LoginWin_ta_ip, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->LoginWin_ta_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->LoginWin_ta_ip, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->LoginWin_ta_ip, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->LoginWin_ta_ip, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->LoginWin_ta_ip, 4, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for LoginWin_ta_ip, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->LoginWin_ta_ip, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->LoginWin_ta_ip, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->LoginWin_ta_ip, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

	//Write codes LoginWin_ta_port
	ui->LoginWin_ta_port = lv_textarea_create(ui->LoginWin);
	lv_textarea_set_text(ui->LoginWin_ta_port, "7777");
	lv_textarea_set_password_bullet(ui->LoginWin_ta_port, "*");
	lv_textarea_set_password_mode(ui->LoginWin_ta_port, false);
	lv_textarea_set_one_line(ui->LoginWin_ta_port, true);
	lv_obj_set_pos(ui->LoginWin_ta_port, 189.5, 158);
	lv_obj_set_size(ui->LoginWin_ta_port, 120, 25);

	//Write style for LoginWin_ta_port, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->LoginWin_ta_port, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->LoginWin_ta_port, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->LoginWin_ta_port, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->LoginWin_ta_port, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->LoginWin_ta_port, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->LoginWin_ta_port, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->LoginWin_ta_port, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->LoginWin_ta_port, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->LoginWin_ta_port, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->LoginWin_ta_port, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->LoginWin_ta_port, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->LoginWin_ta_port, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->LoginWin_ta_port, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->LoginWin_ta_port, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->LoginWin_ta_port, 4, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for LoginWin_ta_port, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->LoginWin_ta_port, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->LoginWin_ta_port, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->LoginWin_ta_port, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
	lv_obj_set_scrollbar_mode(ui->LoginWin_ta_port, LV_SCROLLBAR_MODE_OFF);

	//Write codes LoginWin_labe_ip
	ui->LoginWin_labe_ip = lv_label_create(ui->LoginWin);
	lv_label_set_text(ui->LoginWin_labe_ip, "ip:");
	lv_label_set_long_mode(ui->LoginWin_labe_ip, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->LoginWin_labe_ip, 163, 126);
	lv_obj_set_size(ui->LoginWin_labe_ip, 25, 15);

	//Write style for LoginWin_labe_ip, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->LoginWin_labe_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->LoginWin_labe_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->LoginWin_labe_ip, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->LoginWin_labe_ip, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->LoginWin_labe_ip, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->LoginWin_labe_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->LoginWin_labe_ip, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->LoginWin_labe_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->LoginWin_labe_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->LoginWin_labe_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->LoginWin_labe_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->LoginWin_labe_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->LoginWin_labe_ip, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes LoginWin_label_port
	ui->LoginWin_label_port = lv_label_create(ui->LoginWin);
	lv_label_set_text(ui->LoginWin_label_port, "port:");
	lv_label_set_long_mode(ui->LoginWin_label_port, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->LoginWin_label_port, 148, 163);
	lv_obj_set_size(ui->LoginWin_label_port, 40, 15);
	lv_obj_set_scrollbar_mode(ui->LoginWin_ta_ip, LV_SCROLLBAR_MODE_OFF);

	//Write style for LoginWin_label_port, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->LoginWin_label_port, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->LoginWin_label_port, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->LoginWin_label_port, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->LoginWin_label_port, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->LoginWin_label_port, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->LoginWin_label_port, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->LoginWin_label_port, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->LoginWin_label_port, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->LoginWin_label_port, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->LoginWin_label_port, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->LoginWin_label_port, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->LoginWin_label_port, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->LoginWin_label_port, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes LoginWin_btn_connet
	ui->LoginWin_btn_connet = lv_btn_create(ui->LoginWin);
	ui->LoginWin_btn_connet_label = lv_label_create(ui->LoginWin_btn_connet);
	lv_label_set_text(ui->LoginWin_btn_connet_label, "connect");
	lv_label_set_long_mode(ui->LoginWin_btn_connet_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->LoginWin_btn_connet_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->LoginWin_btn_connet, 0, LV_STATE_DEFAULT);
	lv_obj_set_pos(ui->LoginWin_btn_connet, 206, 190);
	lv_obj_set_size(ui->LoginWin_btn_connet, 80, 26);

	//Write style for LoginWin_btn_connet, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->LoginWin_btn_connet, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->LoginWin_btn_connet, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->LoginWin_btn_connet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->LoginWin_btn_connet, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->LoginWin_btn_connet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->LoginWin_btn_connet, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->LoginWin_btn_connet, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->LoginWin_btn_connet, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes LoginWin_label_2
	ui->LoginWin_label_2 = lv_label_create(ui->LoginWin);
	lv_label_set_text(ui->LoginWin_label_2, "XIHE");
	lv_label_set_long_mode(ui->LoginWin_label_2, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->LoginWin_label_2, 177, 42);
	lv_obj_set_size(ui->LoginWin_label_2, 126, 66);

	//Write style for LoginWin_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->LoginWin_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->LoginWin_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->LoginWin_label_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->LoginWin_label_2, &lv_font_subset4_70, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->LoginWin_label_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->LoginWin_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->LoginWin_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->LoginWin_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->LoginWin_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->LoginWin_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->LoginWin_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->LoginWin_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->LoginWin_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Update current screen layout.
	lv_obj_update_layout(ui->LoginWin);

	
}
