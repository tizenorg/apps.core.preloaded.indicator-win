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
#include "indicator_ui.h"
#include "modules.h"
#include "indicator_icon_util.h"

#define ICON_PRIORITY	INDICATOR_PRIORITY_NON_FIXED_2
#define MODULE_NAME		"alarm"

static int register_alarm_module(void *data);
static int unregister_alarm_module(void);
static int hib_enter_alarm_module(void);
static int hib_leave_alarm_module(void *data);

Indicator_Icon_Object useralarm = {
	.type = INDICATOR_IMG_ICON,
	.name = MODULE_NAME,
	.priority = ICON_PRIORITY,
	.always_top = EINA_FALSE,
	.exist_in_view = EINA_FALSE,
	.txt_obj = {0,},
	.img_obj = {0,},
	.obj_exist = EINA_FALSE,
	.fixed = EINA_FALSE,
	.init = register_alarm_module,
	.fini = unregister_alarm_module,
	.hib_enter = hib_enter_alarm_module,
	.hib_leave = hib_leave_alarm_module
};

static char *icon_path[] = {
	"Alarm/B03_Alarm.png",
	NULL
};

static void show_image_icon(void *data)
{
	useralarm.img_obj.data = icon_path[0];
	indicator_util_icon_show(&useralarm);
}

static void hide_image_icon(void)
{
	indicator_util_icon_hide(&useralarm);
}

static void indicator_alarm_change_cb(keynode_t *node, void *data)
{
	int status = 0;
	int ret;

	retif(data == NULL, , "Invalid parameter!");

	ret = vconf_get_int(VCONFKEY_ALARM_STATE, &status);
	if (ret == OK) {
		if (status > 0) {
			INFO("ALARM COUNT: %d", status);
			show_image_icon(data);
			return;
		}
		INFO("ALARM COUNT: %d", status);
		hide_image_icon();
		return;
	}
	ERR("Failed to get alarm count!");
	return;
}

static int register_alarm_module(void *data)
{
	int ret;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONFKEY_ALARM_STATE,
				       indicator_alarm_change_cb, data);
	if (ret != OK)
		ERR("Failed to register callback!");

	indicator_alarm_change_cb(NULL, data);

	return ret;
}

static int unregister_alarm_module(void)
{
	int ret;

	ret = vconf_ignore_key_changed(VCONFKEY_ALARM_STATE,
				       indicator_alarm_change_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	hide_image_icon();

	return OK;
}

static int hib_enter_alarm_module(void)
{
	int ret;

	ret = vconf_ignore_key_changed(VCONFKEY_ALARM_STATE,
				       indicator_alarm_change_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	return OK;
}

static int hib_leave_alarm_module(void *data)
{
	int ret;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONFKEY_ALARM_STATE,
				       indicator_alarm_change_cb, data);
	retif(ret != OK, FAIL, "Failed to register callback!");

	indicator_alarm_change_cb(NULL, data);
	return OK;
}
