/*                                                                                                                                                                                                                                 
 * Copyright (c) 2000 - 2012 Samsung Electronics Co., Ltd. All Rights Reserved. 
 *                                                                                 
 * This file is part of indicator  
 *
 * Written by Jeonghoon Park <jh1979.park@samsung.com> Youngjoo Park <yjoo93.park@samsung.com>
 *                                                                                 
 * PROPRIETARY/CONFIDENTIAL                                                        
 *                                                                                 
 * This software is the confidential and proprietary information of                
 * SAMSUNG ELECTRONICS ("Confidential Information").                               
 * You shall not disclose such Confidential Information and shall use it only   
 * in accordance with the terms of the license agreement you entered into          
 * with SAMSUNG ELECTRONICS.                                                       
 * SAMSUNG make no representations or warranties about the suitability of          
 * the software, either express or implied, including but not limited to           
 * the implied warranties of merchantability, fitness for a particular purpose, 
 * or non-infringement. SAMSUNG shall not be liable for any damages suffered by 
 * licensee as a result of using, modifying or distributing this software or    
 * its derivatives.                                                                
 *                                                                                 
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

#define ICON_PRIORITY	INDICATOR_PRIORITY_FIXED4
#define MODULE_NAME		"mobile_hotspot"

static int register_mobile_hotspot_module(void *data);
static int unregister_mobile_hotspot_module(void);

Indicator_Icon_Object mobile_hotspot = {
	.type = INDICATOR_IMG_ICON,
	.name = MODULE_NAME,
	.priority = ICON_PRIORITY,
	.always_top = EINA_FALSE,
	.exist_in_view = EINA_FALSE,
	.txt_obj = {0,},
	.img_obj = {0,},
	.obj_exist = EINA_FALSE,
	.fixed = EINA_TRUE,
	.init = register_mobile_hotspot_module,
	.fini = unregister_mobile_hotspot_module
};

enum {
	MOBILEAP_ON_NOT_CONNECTED = 0,
	MOBILEAP_CONNECTED_MIN = MOBILEAP_ON_NOT_CONNECTED,
	MOBILEAP_CONNECTED1,
	MOBILEAP_CONNECTED2,
	MOBILEAP_CONNECTED3,
	MOBILEAP_CONNECTED4,
	MOBILEAP_CONNECTED5,
	MOBILEAP_CONNECTED6,
	MOBILEAP_CONNECTED7,
	MOBILEAP_CONNECTED8,
	MOBILEAP_CONNECTED9,
	MOBILEAP_MAX,
};

static const char *icon_path[MOBILEAP_MAX] = {
	[MOBILEAP_ON_NOT_CONNECTED] =
			"Connection/B03_MobileAP_on_not_connected.png",
	[MOBILEAP_CONNECTED1] = "Connection/B03_MobileAP_connected_01.png",
	[MOBILEAP_CONNECTED2] = "Connection/B03_MobileAP_connected_02.png",
	[MOBILEAP_CONNECTED3] = "Connection/B03_MobileAP_connected_03.png",
	[MOBILEAP_CONNECTED4] = "Connection/B03_MobileAP_connected_04.png",
	[MOBILEAP_CONNECTED5] = "Connection/B03_MobileAP_connected_05.png",
	[MOBILEAP_CONNECTED6] = "Connection/B03_MobileAP_connected_06.png",
	[MOBILEAP_CONNECTED7] = "Connection/B03_MobileAP_connected_07.png",
	[MOBILEAP_CONNECTED8] = "Connection/B03_MobileAP_connected_08.png",
	[MOBILEAP_CONNECTED9] = "Connection/B03_MobileAP_connected_09.png",
	/* "Connection/B03_MobileAP_connected.png", */
};

static void indicator_mobile_hotspot_change_cb(keynode_t *node, void *data)
{
	int status;
	int ret;

	retif(data == NULL, , "Invalid parameter!");
	ret = vconf_get_int(VCONFKEY_MOBILE_HOTSPOT_MODE, &status);
	if (ret == OK) {
		INFO("mobile_hotspot status: %d", status);
		if (status != VCONFKEY_MOBILE_HOTSPOT_MODE_NONE) {
			int icon_index = 0;
			int connected_device = 0;
			mobile_hotspot.img_obj.data =
				icon_path[MOBILEAP_ON_NOT_CONNECTED];

			ret = vconf_get_int(
				VCONFKEY_MOBILE_HOTSPOT_CONNECTED_DEVICE,
				&connected_device);

			icon_index = MOBILEAP_CONNECTED_MIN + connected_device;
			if (icon_index < MOBILEAP_ON_NOT_CONNECTED) {
				ERR("unable to handle %d connected devices ",
					connected_device);
				icon_index = MOBILEAP_ON_NOT_CONNECTED;
			} else if (icon_index >= MOBILEAP_MAX) {
				ERR("unable to handle %d connected devices ",
					connected_device);
				icon_index = MOBILEAP_CONNECTED9;
			}

			if (ret == OK)
				mobile_hotspot.img_obj.data =
					icon_path[icon_index];

			indicator_util_icon_show(&mobile_hotspot);
			return;
		} else {
			indicator_util_icon_hide(&mobile_hotspot);
			return;
		}
	}

	indicator_util_icon_hide(&mobile_hotspot);
	return;
}

static int register_mobile_hotspot_module(void *data)
{
	int r = 0, ret = -1;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONFKEY_MOBILE_HOTSPOT_MODE,
			       indicator_mobile_hotspot_change_cb, data);
	if (ret != OK) {
		ERR("Failed to register callback!");
		r = r | ret;
	}

	ret = vconf_notify_key_changed(VCONFKEY_MOBILE_HOTSPOT_CONNECTED_DEVICE,
			       indicator_mobile_hotspot_change_cb, data);
	if (ret != OK) {
		ERR("Failed to register callback!");
		r = r | ret;
	}

	indicator_mobile_hotspot_change_cb(NULL, data);

	return r;
}

static int unregister_mobile_hotspot_module(void)
{
	int ret;

	ret = vconf_ignore_key_changed(VCONFKEY_MOBILE_HOTSPOT_MODE,
				       indicator_mobile_hotspot_change_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	ret = vconf_ignore_key_changed(VCONFKEY_MOBILE_HOTSPOT_CONNECTED_DEVICE,
				       indicator_mobile_hotspot_change_cb);
	if (ret != OK)
		ERR("Failed to register callback!");

	indicator_util_icon_hide(&mobile_hotspot);

	return OK;
}
