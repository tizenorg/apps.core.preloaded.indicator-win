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

#define ICON_PRIORITY	INDICATOR_PRIORITY_NON_FIXED_6
#define MODULE_NAME		"mmc"
#define TIMER_INTERVAL	0.3

static int register_mmc_module(void *data);
static int unregister_mmc_module(void);

Indicator_Icon_Object mmc = {
	.name = MODULE_NAME,
	.priority = ICON_PRIORITY,
	.always_top = EINA_TRUE,
	.exist_in_view = EINA_FALSE,
	.txt_obj = {0,},
	.img_obj = {0,},
	.obj_exist = EINA_FALSE,
	.fixed = EINA_FALSE,
	.init = register_mmc_module,
	.fini = unregister_mmc_module
};

static const char *icon_path[] = {
	"Background_playing/B03_Memorycard.png",
	NULL
};

static void indicator_mmc_change_cb(keynode_t *node, void *data)
{
	int status = 0;
	int ret;

	retif(data == NULL, , "Invalid parameter!");

	ret = vconf_get_int(VCONFKEY_FILEMANAGER_DB_STATUS, &status);
	if (ret == FAIL) {
		ERR("Failed to get VCONFKEY_MMC_STATE!");
		return;
	}

	switch (status) {
	case VCONFKEY_FILEMANAGER_DB_UPDATING:
		INFO("MMC loading");
		mmc.img_obj.data = icon_path[0];
		indicator_util_icon_show(&mmc);
		indicator_util_icon_animation_set(&mmc, ICON_ANI_BLINK);
		break;

	case VCONFKEY_FILEMANAGER_DB_UPDATED:
	default:
		indicator_util_icon_hide(&mmc);
		break;
	}
}

static int register_mmc_module(void *data)
{
	int ret;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONFKEY_FILEMANAGER_DB_STATUS,
				       indicator_mmc_change_cb, data);
	if (ret != OK)
		ERR("Failed to register mmcback!");

	indicator_mmc_change_cb(NULL, data);

	return ret;
}

static int unregister_mmc_module(void)
{
	int ret;

	ret = vconf_ignore_key_changed(VCONFKEY_FILEMANAGER_DB_STATUS,
				       indicator_mmc_change_cb);
	if (ret != OK)
		ERR("Failed to unregister mmcback!");

	indicator_util_icon_hide(&mmc);

	return OK;
}
