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
#include "indicator_icon_util.h"
#include "modules.h"
#include "indicator_ui.h"

#define ICON_PRIORITY	INDICATOR_PRIORITY_NON_FIXED_5
#define MODULE_NAME		"bluetooth"
#define TIMER_INTERVAL	0.3

extern void show_trnsfr_icon(void *data);
extern void hide_trnsfr_icon(void);

static int register_bluetooth_module(void *data);
static int unregister_bluetooth_module(void);
static int hib_enter_bluetooth_module(void);
static int hib_leave_bluetooth_module(void *data);

Indicator_Icon_Object bluetooth = {
	.type = INDICATOR_IMG_ICON,
	.name = MODULE_NAME,
	.priority = ICON_PRIORITY,
	.always_top = EINA_FALSE,
	.exist_in_view = EINA_FALSE,
	.txt_obj = {0,},
	.img_obj = {0,},
	.obj_exist = EINA_FALSE,
	.fixed = EINA_FALSE,
	.init = register_bluetooth_module,
	.fini = unregister_bluetooth_module,
	.hib_enter = hib_enter_bluetooth_module,
	.hib_leave = hib_leave_bluetooth_module
};

enum {
	LEVEL_MIN = 0,
	LEVEL_BT_NOT_CONNECTED = LEVEL_MIN,
	LEVEL_BT_CONNECTED,
	LEVEL_BT_HEADSET,
	LEVEL_MAX
};

static const char *icon_path[LEVEL_MAX] = {
	[LEVEL_BT_NOT_CONNECTED] =
		"Bluetooth_NFC_GPS/B03_BT_On_Notconnected.png",
	[LEVEL_BT_CONNECTED] = "Bluetooth_NFC_GPS/B03_BT_On_Connected.png",
	[LEVEL_BT_HEADSET] =
		"Bluetooth_NFC_GPS/B03_BT_On_Connected_headset.png",
};

static Ecore_Timer *timer;
static Eina_Bool bt_transferring = EINA_FALSE;

static void delete_timer(void)
{
	if (timer != NULL) {
		ecore_timer_del(timer);
		timer = NULL;
	}
}

static void show_image_icon(void *data, int i)
{
	if (i < LEVEL_MIN || i >= LEVEL_MAX)
		i = LEVEL_MIN;

	bluetooth.img_obj.data = icon_path[i];
	indicator_util_icon_show(&bluetooth);
}

static void hide_image_icon(void)
{
	indicator_util_icon_hide(&bluetooth);
}

static Eina_Bool indicator_bluetooth_multidev_cb(void *data)
{
	static int i = 0;

	retif(data == NULL, ECORE_CALLBACK_CANCEL, "Invalid parameter!");

	show_image_icon(data, LEVEL_BT_CONNECTED + i);
	i = (++i % 2) ? i : 0;

	return ECORE_CALLBACK_RENEW;
}

#define NO_DEVICE			(0x00)
#define HEADSET_CONNECTED	(0x01)
#define DEVICE_CONNECTED	(0x02)
#define DATA_TRANSFER		(0x04)

static void show_bluetooth_icon(void *data, int status)
{
	INFO("Bluetooth status = %d", status);

	if (status == NO_DEVICE) {
		show_image_icon(data, LEVEL_BT_NOT_CONNECTED);
		return;
	}
	/* check transfer icon first */
	if (status & DATA_TRANSFER) {
		if(bt_transferring != EINA_TRUE) {
			show_trnsfr_icon(data);
			bt_transferring	= EINA_TRUE;
		}
		return;
	}
	/* check multiple device */
	if ((status & HEADSET_CONNECTED) && (status & DEVICE_CONNECTED)) {
		INFO("BT_MULTI_DEVICE_CONNECTED");
		timer = ecore_timer_add(TIMER_INTERVAL,
					indicator_bluetooth_multidev_cb, data);
		return;
	}

	if (status & HEADSET_CONNECTED) {
		INFO("BT_HEADSET_CONNECTED");
		show_image_icon(data, LEVEL_BT_HEADSET);
	} else if (status & DEVICE_CONNECTED) {
		INFO("BT_DEVICE_CONNECTED");
		show_image_icon(data, LEVEL_BT_CONNECTED);
	}
	return;
}

static void indicator_bluetooth_change_cb(keynode_t *node, void *data)
{
	int status, dev;
	int ret;
	int result = NO_DEVICE;

	retif(data == NULL, , "Invalid parameter!");

	ret = vconf_get_int(VCONFKEY_BT_STATUS, &status);
	if (ret == OK) {
		INFO("BT STATUS: %d", status);
		/* Check transferring Icon animation */
		if (!(status & VCONFKEY_BT_STATUS_TRANSFER)) {
			if (bt_transferring == EINA_TRUE) {
				hide_trnsfr_icon();
				bt_transferring = EINA_FALSE;
			}
		}

		if (status == VCONFKEY_BT_STATUS_OFF) {
			hide_image_icon();
			return;
		} else if (status & VCONFKEY_BT_STATUS_TRANSFER) {
			INFO("BT TRASFER!");
			result = (result | DATA_TRANSFER);
			show_bluetooth_icon(data, result);
			return;
		}
	}

	ret = vconf_get_int(VCONFKEY_BT_DEVICE, &dev);
	if (ret == OK) {
		INFO("BT DEVICE: %d", dev);

		if (dev == VCONFKEY_BT_DEVICE_NONE) {
			show_bluetooth_icon(data, NO_DEVICE);
			return;
		}
		if ((dev & VCONFKEY_BT_DEVICE_HEADSET_CONNECTED) ||
		    (dev & VCONFKEY_BT_DEVICE_A2DP_HEADSET_CONNECTED)) {
			result = (result | HEADSET_CONNECTED);
			INFO("BT_HEADSET_CONNECTED(%x)", result);
		}
		if (((dev & VCONFKEY_BT_DEVICE_SAP_CONNECTED)) ||
		    (dev & VCONFKEY_BT_DEVICE_PBAP_CONNECTED)) {
			result = (result | DEVICE_CONNECTED);
			INFO("BT_DEVICE_CONNECTED(%x)", result);
		}
		show_bluetooth_icon(data, result);
	}
	return;
}

static int register_bluetooth_module(void *data)
{
	int r = 0, ret = -1;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONFKEY_BT_STATUS,
				       indicator_bluetooth_change_cb, data);
	if (ret != OK) {
		ERR("Failed to register callback!");
		r = ret;
	}

	ret = vconf_notify_key_changed(VCONFKEY_BT_DEVICE,
				       indicator_bluetooth_change_cb, data);
	if (ret != OK) {
		ERR("Failed to register callback!");
		r = r | ret;
	}

	indicator_bluetooth_change_cb(NULL, data);

	return r;
}

static int unregister_bluetooth_module(void)
{
	int ret;

	ret = vconf_ignore_key_changed(VCONFKEY_BT_STATUS,
				       indicator_bluetooth_change_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	ret = vconf_ignore_key_changed(VCONFKEY_BT_DEVICE,
				       indicator_bluetooth_change_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	delete_timer();

	if (bt_transferring == EINA_TRUE) {
		hide_trnsfr_icon();
		bt_transferring = EINA_FALSE;
	}

	hide_image_icon();

	return OK;
}

static int hib_enter_bluetooth_module(void)
{
	int ret;

	ret = vconf_ignore_key_changed(VCONFKEY_BT_STATUS,
				       indicator_bluetooth_change_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	return OK;
}

static int hib_leave_bluetooth_module(void *data)
{
	int ret;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONFKEY_BT_STATUS,
				       indicator_bluetooth_change_cb, data);
	retif(ret != OK, FAIL, "Failed to register callback!");

	indicator_bluetooth_change_cb(NULL, data);
	return OK;
}
