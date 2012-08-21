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
#include "common.h"
#include "indicator.h"
#include "indicator_icon_util.h"
#include "modules.h"
#include "indicator_ui.h"

#define ICON_PRIORITY	INDICATOR_PRIORITY_NON_FIXED_5
#define MODULE_NAME		"gps"
#define TIMER_INTERVAL	0.3

static int register_gps_module(void *data);
static int unregister_gps_module(void);
static int hib_enter_gps_module(void);
static int hib_leave_gps_module(void *data);

Indicator_Icon_Object gps = {
	.type = INDICATOR_IMG_ICON,
	.name = MODULE_NAME,
	.priority = ICON_PRIORITY,
	.always_top = EINA_TRUE,
	.exist_in_view = EINA_FALSE,
	.txt_obj = {0,},
	.img_obj = {0,},
	.obj_exist = EINA_FALSE,
	.fixed = EINA_FALSE,
	.init = register_gps_module,
	.fini = unregister_gps_module,
	.hib_enter = hib_enter_gps_module,
	.hib_leave = hib_leave_gps_module
};

enum {
	LEVEL_MIN = 0,
	LEVEL_GPS_ON = LEVEL_MIN,
	LEVEL_GPS_SEARCHING,
	LEVEL_MAX
};

static const char *icon_path[LEVEL_MAX] = {
	[LEVEL_GPS_ON] = "Bluetooth_NFC_GPS/B03_GPS_On.png",
	[LEVEL_GPS_SEARCHING] = "Bluetooth_NFC_GPS/B03_GPS_Searching.png",
};

static void show_image_icon(void *data, int i)
{
	if (i < LEVEL_MIN)
		i = LEVEL_MIN;
	else if (i >= LEVEL_MAX)
		i = LEVEL_GPS_SEARCHING;

	gps.img_obj.data = icon_path[i];
	indicator_util_icon_show(&gps);
}

static void hide_image_icon(void)
{
	indicator_util_icon_hide(&gps);
}

static int indicator_gps_state_get(void)
{
	int ret = 0;
	int status = VCONFKEY_LOCATION_POSITION_OFF;

	ret = vconf_get_int(VCONFKEY_LOCATION_POSITION_STATE, &status);
	if (ret < 0)
		ERR("fail to get [%s]", VCONFKEY_LOCATION_POSITION_STATE);

	INFO("GPS STATUS: %d", status);

	return status;
}

static void indicator_gps_state_icon_set(int status, void *data)
{
	INFO("GPS STATUS: %d", status);

	switch (status) {
	case VCONFKEY_LOCATION_POSITION_OFF:
		hide_image_icon();
		break;
	case VCONFKEY_LOCATION_POSITION_CONNECTED:
		show_image_icon(data, LEVEL_GPS_ON);
		indicator_util_icon_animation_set(&gps, ICON_ANI_NONE);
		break;
	case VCONFKEY_LOCATION_POSITION_SEARCHING:
		show_image_icon(data, LEVEL_GPS_SEARCHING);
		indicator_util_icon_animation_set(&gps, ICON_ANI_BLINK);
		break;
	default:
		hide_image_icon();
		ERR("Invalid value!");
		break;
	}

	return;
}

static void indicator_gps_change_cb(keynode_t *node, void *data)
{
	int status = VCONFKEY_LOCATION_POSITION_OFF;

	retif(data == NULL, , "Invalid parameter!");
	retif(node == NULL, , "node is NULL");

	status = vconf_keynode_get_int(node);
	if (status < 0) {
 		ERR("fail to get value from node");
		status = VCONFKEY_LOCATION_POSITION_OFF;
	}

	indicator_gps_state_icon_set(status, data);

	return;
}

static int register_gps_module(void *data)
{
	int ret;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONFKEY_LOCATION_POSITION_STATE,
				       indicator_gps_change_cb, data);
	if (ret != OK)
		ERR("Failed to register callback!");

	indicator_gps_state_icon_set(indicator_gps_state_get(), data);

	return ret;
}

static int unregister_gps_module(void)
{
	int ret;

	ret = vconf_ignore_key_changed(VCONFKEY_LOCATION_POSITION_STATE,
				       indicator_gps_change_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	hide_image_icon();

	return OK;
}

static int hib_enter_gps_module(void)
{
	int ret;

	ret = vconf_ignore_key_changed(VCONFKEY_LOCATION_POSITION_STATE,
				       indicator_gps_change_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	return OK;
}

static int hib_leave_gps_module(void *data)
{
	int ret;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONFKEY_LOCATION_POSITION_STATE,
				       indicator_gps_change_cb, data);
	retif(ret != OK, FAIL, "Failed to register callback!");

	indicator_gps_state_icon_set(indicator_gps_state_get(), data);

	return OK;
}

