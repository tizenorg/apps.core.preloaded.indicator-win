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

#define ICON_PRIORITY	INDICATOR_PRIORITY_NON_FIXED_7
#define MODULE_NAME		"VOICE_RECORDER"

static int register_voice_recorder_module(void *data);
static int unregister_voice_recorder_module(void);

Indicator_Icon_Object voice_recorder = {
	.type = INDICATOR_IMG_ICON,
	.name = MODULE_NAME,
	.priority = ICON_PRIORITY,
	.always_top = EINA_FALSE,
	.exist_in_view = EINA_FALSE,
	.txt_obj = {0,},
	.img_obj = {0,},
	.obj_exist = EINA_FALSE,
	.fixed = EINA_FALSE,
	.init = register_voice_recorder_module,
	.fini = unregister_voice_recorder_module
};

enum {
	VOICE_RECORDER_RECORDING,
	VOICE_RECORDER_PAUSED
};

static char *icon_path[] = {
	"Background_playing/B03_Backgroundplaying_voicerecorder_Recording.png",
	"Background_playing/B03_Backgroundplaying_voicerecorder_paused.png",
	NULL
};

static void show_image_icon(void *data, int status)
{
	voice_recorder.type = INDICATOR_IMG_ICON;
	voice_recorder.img_obj.data = icon_path[status];
	indicator_util_icon_show(&voice_recorder);
}

static void hide_image_icon(void)
{
	indicator_util_icon_hide(&voice_recorder);
}

static void indicator_voice_recorder_change_cb(keynode_t *node, void *data)
{
	int status;
	int ret;

	retif(data == NULL, , "Invalid parameter!");

	ret = vconf_get_int(VCONFKEY_VOICERECORDER_STATE, &status);
	if (ret == OK) {
		INFO("VOICE RECORDER state: %d", status);
		switch (status) {
		case VCONFKEY_VOICERECORDER_RECORDING:
			show_image_icon(data, VOICE_RECORDER_RECORDING);
			break;
		case VCONFKEY_VOICERECORDER_PAUSED:
			show_image_icon(data, VOICE_RECORDER_PAUSED);
			break;
		default:
			hide_image_icon();
			break;
		}
	}
	return;
}

static int register_voice_recorder_module(void *data)
{
	int ret;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONFKEY_VOICERECORDER_STATE,
				       indicator_voice_recorder_change_cb,
				       data);
	if (ret != OK)
		ERR("Failed to register callback!");

	indicator_voice_recorder_change_cb(NULL, data);

	return ret;
}

static int unregister_voice_recorder_module(void)
{
	int ret;

	ret = vconf_ignore_key_changed(VCONFKEY_VOICERECORDER_STATE,
				       indicator_voice_recorder_change_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	hide_image_icon();

	return OK;
}
