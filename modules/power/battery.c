/*
 * Copyright 2012  Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.tizenopensource.org/license
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <vconf.h>
#include <appcore-efl.h>
#include "common.h"
#include "indicator.h"
#include "indicator_ui.h"
#include "modules.h"
#include "indicator_icon_util.h"

#define ICON_PRIORITY	INDICATOR_PRIORITY_FIXED2
#define MODULE_NAME		"battery"
#define TIMER_INTERVAL	0.7
#define BATTERY_IMAGEWIDTH	49
#define BATTERY_TEXTWIDTH	62
#define BATTERY_VALUE_FONT_SIZE	20
#define BATTERY_PERCENT_FONT_SIZE	20
#define BATTERY_PERCENT_FONT_STYLE "Bold"

static int register_battery_module(void *data);
static int unregister_battery_module(void);

Indicator_Icon_Object battery = {
	.type = INDICATOR_IMG_ICON,
	.name = MODULE_NAME,
	.priority = ICON_PRIORITY,
	.fixed = EINA_TRUE,
	.always_top = EINA_FALSE,
	.exist_in_view = EINA_FALSE,
	.txt_obj = {0,},
	.img_obj = {0,},
	.obj_exist = EINA_FALSE,
	.init = register_battery_module,
	.fini = unregister_battery_module
};

enum {
	BATTERY_LEVEL_6,
	BATTERY_LEVEL_20,
};

enum {
	LEVEL_MIN = 0,
	LEVEL_0 = LEVEL_MIN,	/* 1  ~   4% */
	LEVEL_1,				/* 5  ~  14% */
	LEVEL_2,				/* 15 ~  24% */
	LEVEL_3,				/* 25 ~  39% */
	LEVEL_4,				/* 40 ~  59% */
	LEVEL_5,				/* 60 ~  79% */
	LEVEL_6,				/* 80 ~ 100% */
	LEVEL_MAX = LEVEL_6,
	LEVEL_NUM,
};

static const char *icon_path[LEVEL_NUM] = {
	[LEVEL_0] = "Power/battery_6/B03_Power_battery_00.png",
	[LEVEL_1] = "Power/battery_6/B03_Power_battery_01.png",
	[LEVEL_2] = "Power/battery_6/B03_Power_battery_02.png",
	[LEVEL_3] = "Power/battery_6/B03_Power_battery_03.png",
	[LEVEL_4] = "Power/battery_6/B03_Power_battery_04.png",
	[LEVEL_5] = "Power/battery_6/B03_Power_battery_05.png",
	[LEVEL_6] = "Power/battery_6/B03_Power_battery_06.png",
};

static const char *charging_icon_path[LEVEL_NUM] = {
	[LEVEL_0] = "Power/battery_6/B03_Power_connected_00.png",
	[LEVEL_1] = "Power/battery_6/B03_Power_connected_01.png",
	[LEVEL_2] = "Power/battery_6/B03_Power_connected_02.png",
	[LEVEL_3] = "Power/battery_6/B03_Power_connected_03.png",
	[LEVEL_4] = "Power/battery_6/B03_Power_connected_04.png",
	[LEVEL_5] = "Power/battery_6/B03_Power_connected_05.png",
	[LEVEL_6] = "Power/battery_6/B03_Power_connected_06.png",
};

enum {
	FUEL_GAUGE_LV_MIN = 0,
	FUEL_GAUGE_LV_0 = FUEL_GAUGE_LV_MIN,
	FUEL_GAUGE_LV_1,
	FUEL_GAUGE_LV_2,
	FUEL_GAUGE_LV_3,
	FUEL_GAUGE_LV_4,
	FUEL_GAUGE_LV_5,
	FUEL_GAUGE_LV_6,
	FUEL_GAUGE_LV_7,
	FUEL_GAUGE_LV_8,
	FUEL_GAUGE_LV_9,
	FUEL_GAUGE_LV_10,
	FUEL_GAUGE_LV_11,
	FUEL_GAUGE_LV_12,
	FUEL_GAUGE_LV_13,
	FUEL_GAUGE_LV_14,
	FUEL_GAUGE_LV_15,
	FUEL_GAUGE_LV_16,
	FUEL_GAUGE_LV_17,
	FUEL_GAUGE_LV_18,
	FUEL_GAUGE_LV_19,
	FUEL_GAUGE_LV_20,
	FUEL_GAUGE_LV_MAX = FUEL_GAUGE_LV_20,
	FUEL_GAUGE_LV_NUM,
};

static const char *fuel_guage_icon_path[FUEL_GAUGE_LV_NUM] = {
	[FUEL_GAUGE_LV_0] = "Power/battery_20/B03_Power_battery_00.png",
	[FUEL_GAUGE_LV_1] = "Power/battery_20/B03_Power_battery_01.png",
	[FUEL_GAUGE_LV_2] = "Power/battery_20/B03_Power_battery_02.png",
	[FUEL_GAUGE_LV_3] = "Power/battery_20/B03_Power_battery_03.png",
	[FUEL_GAUGE_LV_4] = "Power/battery_20/B03_Power_battery_04.png",
	[FUEL_GAUGE_LV_5] = "Power/battery_20/B03_Power_battery_05.png",
	[FUEL_GAUGE_LV_6] = "Power/battery_20/B03_Power_battery_06.png",
	[FUEL_GAUGE_LV_7] = "Power/battery_20/B03_Power_battery_07.png",
	[FUEL_GAUGE_LV_8] = "Power/battery_20/B03_Power_battery_08.png",
	[FUEL_GAUGE_LV_9] = "Power/battery_20/B03_Power_battery_09.png",
	[FUEL_GAUGE_LV_10] = "Power/battery_20/B03_Power_battery_10.png",
	[FUEL_GAUGE_LV_11] = "Power/battery_20/B03_Power_battery_11.png",
	[FUEL_GAUGE_LV_12] = "Power/battery_20/B03_Power_battery_12.png",
	[FUEL_GAUGE_LV_13] = "Power/battery_20/B03_Power_battery_13.png",
	[FUEL_GAUGE_LV_14] = "Power/battery_20/B03_Power_battery_14.png",
	[FUEL_GAUGE_LV_15] = "Power/battery_20/B03_Power_battery_15.png",
	[FUEL_GAUGE_LV_16] = "Power/battery_20/B03_Power_battery_16.png",
	[FUEL_GAUGE_LV_17] = "Power/battery_20/B03_Power_battery_17.png",
	[FUEL_GAUGE_LV_18] = "Power/battery_20/B03_Power_battery_18.png",
	[FUEL_GAUGE_LV_19] = "Power/battery_20/B03_Power_battery_19.png",
	[FUEL_GAUGE_LV_20] = "Power/battery_20/B03_Power_battery_20.png",
};

static const char *fuel_guage_charging_icon_path[FUEL_GAUGE_LV_NUM] = {
	[FUEL_GAUGE_LV_0] = "Power/battery_20/B03_Power_charging_00.png",
	[FUEL_GAUGE_LV_1] = "Power/battery_20/B03_Power_charging_01.png",
	[FUEL_GAUGE_LV_2] = "Power/battery_20/B03_Power_charging_02.png",
	[FUEL_GAUGE_LV_3] = "Power/battery_20/B03_Power_charging_03.png",
	[FUEL_GAUGE_LV_4] = "Power/battery_20/B03_Power_charging_04.png",
	[FUEL_GAUGE_LV_5] = "Power/battery_20/B03_Power_charging_05.png",
	[FUEL_GAUGE_LV_6] = "Power/battery_20/B03_Power_charging_06.png",
	[FUEL_GAUGE_LV_7] = "Power/battery_20/B03_Power_charging_07.png",
	[FUEL_GAUGE_LV_8] = "Power/battery_20/B03_Power_charging_08.png",
	[FUEL_GAUGE_LV_9] = "Power/battery_20/B03_Power_charging_09.png",
	[FUEL_GAUGE_LV_10] = "Power/battery_20/B03_Power_charging_10.png",
	[FUEL_GAUGE_LV_11] = "Power/battery_20/B03_Power_charging_11.png",
	[FUEL_GAUGE_LV_12] = "Power/battery_20/B03_Power_charging_12.png",
	[FUEL_GAUGE_LV_13] = "Power/battery_20/B03_Power_charging_13.png",
	[FUEL_GAUGE_LV_14] = "Power/battery_20/B03_Power_charging_14.png",
	[FUEL_GAUGE_LV_15] = "Power/battery_20/B03_Power_charging_15.png",
	[FUEL_GAUGE_LV_16] = "Power/battery_20/B03_Power_charging_16.png",
	[FUEL_GAUGE_LV_17] = "Power/battery_20/B03_Power_charging_17.png",
	[FUEL_GAUGE_LV_18] = "Power/battery_20/B03_Power_charging_18.png",
	[FUEL_GAUGE_LV_19] = "Power/battery_20/B03_Power_charging_19.png",
	[FUEL_GAUGE_LV_20] = "Power/battery_20/B03_Power_charging_20.png",
};

struct battery_level_info {
	int current_level;
	int min_level;
	int max_level;
	const char **icon_path;
	const char **charing_icon_path;
};

static struct battery_level_info _level;
static Ecore_Timer *timer;
static int battery_level_type = BATTERY_LEVEL_20;
static Eina_Bool battery_percentage_on = EINA_FALSE;
static Eina_Bool battery_charging = EINA_FALSE;

static void indicator_battery_level_init(void)
{
	/* Currently, kernel not support level 6, So We use only level 20 */
	battery_level_type = BATTERY_LEVEL_20;
	_level.min_level = FUEL_GAUGE_LV_MIN;
	_level.current_level = FUEL_GAUGE_LV_MIN;
	_level.max_level = FUEL_GAUGE_LV_MAX;
	_level.icon_path = fuel_guage_icon_path;
	_level.charing_icon_path = fuel_guage_charging_icon_path;
}

static void delete_timer(void)
{
	if (timer != NULL) {
		ecore_timer_del(timer);
		timer = NULL;
	}
}

static int level_check(int capacity)
{
	if (battery_level_type == BATTERY_LEVEL_20) {
		if (capacity < 0)
			_level.current_level = FUEL_GAUGE_LV_MIN;
		else if (capacity > 100)
			_level.current_level = FUEL_GAUGE_LV_MAX;
		else if (capacity >= 98)
			_level.current_level = FUEL_GAUGE_LV_20;
		else if (capacity >= 93)
			_level.current_level = FUEL_GAUGE_LV_19;
		else if (capacity >= 88)
			_level.current_level = FUEL_GAUGE_LV_18;
		else if (capacity >= 83)
			_level.current_level = FUEL_GAUGE_LV_17;
		else if (capacity >= 78)
			_level.current_level = FUEL_GAUGE_LV_16;
		else if (capacity >= 73)
			_level.current_level = FUEL_GAUGE_LV_15;
		else if (capacity >= 68)
			_level.current_level = FUEL_GAUGE_LV_14;
		else if (capacity >= 63)
			_level.current_level = FUEL_GAUGE_LV_13;
		else if (capacity >= 58)
			_level.current_level = FUEL_GAUGE_LV_12;
		else if (capacity >= 53)
			_level.current_level = FUEL_GAUGE_LV_11;
		else if (capacity >= 48)
			_level.current_level = FUEL_GAUGE_LV_10;
		else if (capacity >= 43)
			_level.current_level = FUEL_GAUGE_LV_9;
		else if (capacity >= 38)
			_level.current_level = FUEL_GAUGE_LV_8;
		else if (capacity >= 33)
			_level.current_level = FUEL_GAUGE_LV_7;
		else if (capacity >= 28)
			_level.current_level = FUEL_GAUGE_LV_6;
		else if (capacity >= 23)
			_level.current_level = FUEL_GAUGE_LV_5;
		else if (capacity >= 18)
			_level.current_level = FUEL_GAUGE_LV_4;
		else if (capacity >= 13)
			_level.current_level = FUEL_GAUGE_LV_3;
		else if (capacity >= 8)
			_level.current_level = FUEL_GAUGE_LV_2;
		else if (capacity >= 3)
			_level.current_level = FUEL_GAUGE_LV_1;
		else if (capacity >= 1)
			_level.current_level = FUEL_GAUGE_LV_0;
	} else {
		if (capacity < 0)
			_level.current_level = LEVEL_MIN;
		else if (capacity > 100)
			_level.current_level = LEVEL_MAX;
		else if (capacity >= 80)
			_level.current_level = LEVEL_6;
		else if (capacity >= 60)
			_level.current_level = LEVEL_5;
		else if (capacity >= 40)
			_level.current_level = LEVEL_4;
		else if (capacity >= 25)
			_level.current_level = LEVEL_3;
		else if (capacity >= 15)
			_level.current_level = LEVEL_2;
		else if (capacity >= 5)
			_level.current_level = LEVEL_1;
		else
			_level.current_level = LEVEL_0;
		return 1;
	}

	return 0;
}

static void show_battery_icon(void)
{
	indicator_util_icon_show(&battery);
}

static void indicator_battery_text_set(void *data, int value)
{
	Eina_Strbuf *temp_buf = NULL;
	Eina_Strbuf *percent_buf = NULL;
	char *temp_str = NULL;

	retif(data == NULL, , "Invalid parameter!");

	battery.type = INDICATOR_TXT_WITH_IMG_ICON;
	temp_buf = eina_strbuf_new();
	percent_buf = eina_strbuf_new();

			/* Set Label for percentage value */
	eina_strbuf_append_printf(temp_buf, "%d", value);
	temp_str = eina_strbuf_string_steal(temp_buf);
	eina_strbuf_append_printf(percent_buf, "%s",
				  indicator_util_icon_label_set
				  (temp_str, NULL, NULL,
				   BATTERY_VALUE_FONT_SIZE,
				   data));
	free(temp_str);

	/* Set Label for pecentage symbol */
	eina_strbuf_append_printf(temp_buf, "%%");
	temp_str = eina_strbuf_string_steal(temp_buf);
	eina_strbuf_append_printf(percent_buf, "%s",
				  indicator_util_icon_label_set
				  (temp_str, NULL,
				   BATTERY_PERCENT_FONT_STYLE,
				   BATTERY_PERCENT_FONT_SIZE,
				   data));
	free(temp_str);

	if (battery.txt_obj.data != NULL)
		free(battery.txt_obj.data);

	battery.txt_obj.data = eina_strbuf_string_steal(percent_buf);

	if (temp_buf != NULL)
		eina_strbuf_free(temp_buf);

	if (percent_buf != NULL)
		eina_strbuf_free(percent_buf);
}

static Eina_Bool indicator_battery_charging_ani_cb(void *data)
{
	static int i = -1;

	retif(data == NULL, FAIL, "Invalid parameter!");

	if (_level.current_level == _level.max_level) {
		i = _level.max_level;
		battery.img_obj.data = _level.charing_icon_path[i];
		show_battery_icon();
		timer = NULL;
		return TIMER_STOP;
	}

	if (i >= _level.max_level || i < 0)
		i = _level.current_level;
	else
		i++;

	battery.img_obj.data = _level.charing_icon_path[i];
	show_battery_icon();

	return TIMER_CONTINUE;
}

static int indicator_change_battery_image_level(void *data, int level)
{
	retif(data == NULL, FAIL, "Invalid parameter!");

	/* Set arg for display image only or text with image */
	battery.img_obj.data = _level.icon_path[level];
	show_battery_icon();
	return OK;
}

static void indicator_battery_check_percentage_option(void *data)
{
	int ret = FAIL;
	int status = 0;

	retif(data == NULL, , "Invalid parameter!");

	/* Check Display Percentage option(Text) */
	ret = vconf_get_bool(VCONFKEY_SETAPPL_BATTERY_PERCENTAGE_BOOL, &status);

	if (ret == OK) {
		/* Hide Battery Icon and change Padding between all icons */
		if (status) {
			battery_percentage_on = EINA_TRUE;
			battery.txt_obj.width = BATTERY_TEXTWIDTH;
			battery.img_obj.width = BATTERY_IMAGEWIDTH;
			battery.type = INDICATOR_TXT_WITH_IMG_ICON;
		} else {
			battery_percentage_on = EINA_FALSE;
			battery.img_obj.width = BATTERY_IMAGEWIDTH;
			battery.type = INDICATOR_IMG_ICON;
		}
	}
}

static void indicator_battery_check_charging(void *data)
{
	int ret = -1;
	int status = 0;

	ret = vconf_get_int(VCONFKEY_SYSMAN_BATTERY_CHARGE_NOW, &status);

	if (ret == OK) {
		INFO("Battery Status: %d", status);
		if (status == 1)
			battery_charging = EINA_TRUE;
		else
			battery_charging = EINA_FALSE;
	} else
		ERR("Fail to get [VCONFKEY_SYSMAN_BATTERY_CHARGE_NOW:%d]", ret);
}

static void indicator_battery_update_display(void *data)
{
	int battery_capa = 0;
	int ret;

	retif(data == NULL, , "Invalid parameter!");

	ret = vconf_get_int(VCONFKEY_SYSMAN_BATTERY_CAPACITY, &battery_capa);

	if (ret != OK) {
		ERR("Fail to get [VCONFKEY_SYSMAN_BATTERY_CAPACITY:%d]", ret);
		return;
	}

	INFO("Battery Capacity: %d", battery_capa);

	/* Check Percentage option */
	if (battery_percentage_on == EINA_TRUE)
		indicator_battery_text_set(data, battery_capa);

	/* Check Battery Level */
	level_check(battery_capa);
	DBG("Battery Capacity: %d", battery_capa);

	/* Check Charging. If Battery Status is not charging,
	 * we check battery level
	 */
	if (battery_charging == EINA_TRUE) {
		if (!timer) {
			indicator_util_icon_animation_set(&battery,
						ICON_ANI_NONE);
			timer = ecore_timer_add(TIMER_INTERVAL,
					indicator_battery_charging_ani_cb,
					data);
		}
	} else {
		delete_timer();
		indicator_util_icon_animation_set(&battery, ICON_ANI_NONE);
		indicator_change_battery_image_level(data,
				_level.current_level);
	}
}

static void indicator_battery_charging_cb(keynode_t *node, void *data)
{
	indicator_battery_check_charging(data);
	indicator_battery_update_display(data);
}

static void indicator_battery_percentage_option_cb(keynode_t *node, void *data)
{
	indicator_battery_check_percentage_option(data);
	indicator_util_icon_hide(&battery);
	indicator_battery_update_display(data);
}

static void indicator_battery_change_cb(keynode_t *node, void *data)
{
	indicator_battery_update_display(data);
}

static int register_battery_module(void *data)
{
	int r = 0, ret = -1;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONFKEY_SYSMAN_BATTERY_CAPACITY,
				       indicator_battery_change_cb, data);
	if (ret != OK) {
		ERR("Failed to register callback!");
		r = ret;
	}

	ret = vconf_notify_key_changed(VCONFKEY_SYSMAN_BATTERY_STATUS_LOW,
				       indicator_battery_change_cb, data);
	if (ret != OK) {
		ERR("Failed to register callback!");
		r = r | ret;
	}

	ret = vconf_notify_key_changed(VCONFKEY_SYSMAN_BATTERY_CHARGE_NOW,
				       indicator_battery_charging_cb, data);
	if (ret != OK) {
		ERR("Failed to register callback!");
		r = r | ret;
	}

	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_BATTERY_PERCENTAGE_BOOL,
			       indicator_battery_percentage_option_cb, data);
	if (ret != OK) {
		ERR("Failed to register callback!");
		r = r | ret;
	}

	indicator_battery_level_init();
	indicator_battery_check_percentage_option(data);
	indicator_battery_check_charging(data);
	indicator_battery_update_display(data);

	return r;
}

static int unregister_battery_module(void)
{
	int ret;

	ret = vconf_ignore_key_changed(VCONFKEY_SYSMAN_BATTERY_CAPACITY,
				       indicator_battery_change_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	ret = vconf_ignore_key_changed(VCONFKEY_SYSMAN_BATTERY_STATUS_LOW,
				       indicator_battery_change_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	ret = vconf_ignore_key_changed(VCONFKEY_SYSMAN_BATTERY_CHARGE_NOW,
				       indicator_battery_charging_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_BATTERY_PERCENTAGE_BOOL,
				       indicator_battery_percentage_option_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	delete_timer();
	indicator_util_icon_hide(&battery);

	if (battery.txt_obj.data != NULL)
		free(battery.txt_obj.data);

	return OK;
}
