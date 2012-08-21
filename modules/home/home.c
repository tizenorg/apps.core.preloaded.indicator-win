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

#define ICON_PRIORITY	INDICATOR_PRIORITY_FIXED5
#define MODULE_NAME		"home"

static int register_home_module(void *data);
static int unregister_home_module(void);

Indicator_Icon_Object home = {
	.type = INDICATOR_IMG_ICON,
	.name = MODULE_NAME,
	.priority = ICON_PRIORITY,
	.always_top = EINA_FALSE,
	.txt_obj = {0,},
	.img_obj = {0,},
	.obj_exist = EINA_FALSE,
	.fixed = EINA_TRUE,
	.exist_in_view = EINA_FALSE,
	.init = register_home_module,
	.fini = unregister_home_module,
};

static const char *icon_path[] = {
	"Home/B03_Home.png",
	"Home/B03_Home_press.png",
	NULL
};

static void change_home_icon_cb(keynode_t *node, void *data)
{
	int status = 0;
	int ret = -1;

	retif(data == NULL, , "Invalid parameter!");

	ret = vconf_get_int(VCONF_INDICATOR_HOME_PRESSED, &status);

	if (ret == 0) {
		if (status == 1) {
			INFO("change_home_icon_cb : Home Button Pressed!");
			home.img_obj.data = icon_path[1];
			indicator_util_icon_show(&home);
		} else {
			INFO("change_home_icon_cb : Home Button Released!");
			home.img_obj.data = icon_path[0];
			indicator_util_icon_show(&home);
		}
	}
}
static int register_home_module(void *data)
{
	int r = 0, ret = -1;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONF_INDICATOR_HOME_PRESSED,
				       change_home_icon_cb, data);
	if (ret != 0) {
		ERR("Failed to register callback!");
		r = ret;
	}

	home.img_obj.data = icon_path[0];
	indicator_util_icon_show(&home);
	return 0;
}

static int unregister_home_module(void)
{
	int ret = -1;

	ret = vconf_ignore_key_changed(VCONF_INDICATOR_HOME_PRESSED,
				       change_home_icon_cb);
	if (ret != 0)
		ERR("Failed to unregister callback!");

	indicator_util_icon_hide(&home);
	return 0;
}
