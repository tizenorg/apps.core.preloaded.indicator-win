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
#define MODULE_NAME		"wifi_direct"

extern void show_trnsfr_icon(void *data);
extern void hide_trnsfr_icon(void);

static int register_wifi_direct_module(void *data);
static int unregister_wifi_direct_module(void);

Indicator_Icon_Object wifi_direct = {
	.type = INDICATOR_IMG_ICON,
	.name = MODULE_NAME,
	.priority = ICON_PRIORITY,
	.always_top = EINA_TRUE,
	.exist_in_view = EINA_FALSE,
	.txt_obj = {0,},
	.img_obj = {0,},
	.obj_exist = EINA_FALSE,
	.fixed = EINA_FALSE,
	.init = register_wifi_direct_module,
	.fini = unregister_wifi_direct_module,
};

enum {
	WIFI_DIRECT_ACTIVATE = 0,
	WIFI_DIRECT_CONNECTED,
	WIFI_DIRECT_DISCOVERING,
	WIFI_DIRECT_GROUP_OWNER,
	WIFI_DIRECT_MAX,
};

#define WIFI_D_ICON_NOT_CONNECTED \
	"Bluetooth_NFC_GPS/B03_Wi-fi_direct_On_not_connected.png"

#define WIFI_D_ICON_CONNECTED \
	"Bluetooth_NFC_GPS/B03_Wi-fi_direct_On_connected.png"

static const char *icon_path[WIFI_DIRECT_MAX] = {
	[WIFI_DIRECT_ACTIVATE] = WIFI_D_ICON_NOT_CONNECTED,
	[WIFI_DIRECT_CONNECTED] = WIFI_D_ICON_CONNECTED,
	[WIFI_DIRECT_DISCOVERING] = NULL,
	[WIFI_DIRECT_GROUP_OWNER] = WIFI_D_ICON_CONNECTED,
};


static void show_icon(void *data, int i)
{
	if (i < WIFI_DIRECT_ACTIVATE || i >= WIFI_DIRECT_MAX)
		i = WIFI_DIRECT_ACTIVATE;

	wifi_direct.img_obj.data = icon_path[i];
	indicator_util_icon_show(&wifi_direct);
}

static void hide_icon(void)
{
	indicator_util_icon_hide(&wifi_direct);
}

static void indicator_wifi_direct_change_cb(keynode_t *node, void *data)
{
	int status;
	int ret;

	retif(data == NULL, , "Invalid parameter!");

	ret = vconf_get_int(VCONFKEY_WIFI_DIRECT_STATE, &status);

	if (ret == OK) {
		INFO("wifi_direct STATUS: %d", status);

		switch (status) {
		case VCONFKEY_WIFI_DIRECT_ACTIVATED:
			show_icon(data, WIFI_DIRECT_ACTIVATE);
			break;
		case VCONFKEY_WIFI_DIRECT_CONNECTED:
			show_icon(data, WIFI_DIRECT_CONNECTED);
			break;
		case VCONFKEY_WIFI_DIRECT_DISCOVERING:
			break;
		case VCONFKEY_WIFI_DIRECT_GROUP_OWNER:
			show_icon(data, WIFI_DIRECT_GROUP_OWNER);
			break;
		default:
			hide_icon();
			break;
		}
		return;
	}

	hide_icon();
	return;
}

static int register_wifi_direct_module(void *data)
{
	int ret;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONFKEY_WIFI_DIRECT_STATE,
				       indicator_wifi_direct_change_cb, data);
	if (ret != OK)
		ERR("Failed to register callback! : %s",
			VCONFKEY_WIFI_DIRECT_STATE);

	indicator_wifi_direct_change_cb(NULL, data);

	return ret;
}

static int unregister_wifi_direct_module(void)
{
	int ret;

	ret = vconf_ignore_key_changed(VCONFKEY_WIFI_DIRECT_STATE,
				       indicator_wifi_direct_change_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	hide_icon();

	return OK;
}
