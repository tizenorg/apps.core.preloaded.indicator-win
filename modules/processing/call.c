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
#include <Ecore_X.h>
#include "common.h"
#include "indicator.h"
#include "indicator_ui.h"
#include "modules.h"
#include "indicator_icon_util.h"

#define ICON_PRIORITY	INDICATOR_PRIORITY_NON_FIXED_3
#define MODULE_NAME		"call"

static int register_call_module(void *data);
static int unregister_call_module(void);

Indicator_Icon_Object call = {
	.type = INDICATOR_IMG_ICON,
	.name = MODULE_NAME,
	.priority = ICON_PRIORITY,
	.always_top = EINA_TRUE,
	.exist_in_view = EINA_FALSE,
	.txt_obj = {0,},
	.img_obj = {0,},
	.obj_exist = EINA_FALSE,
	.fixed = EINA_FALSE,
	.init = register_call_module,
	.fini = unregister_call_module
};

static const char *icon_path[] = {
	"Call/B03_Background_playing_call.png",
	NULL
};

static Ecore_Event_Handler *evt_hl;

static void show_image_icon(void *data)
{
	call.img_obj.data = icon_path[0];
	indicator_util_icon_show(&call);
}

static void hide_image_icon(void)
{
	indicator_util_icon_hide(&call);
}

static void indicator_call_change_cb(keynode_t *node, void *data)
{
	int status = 0;
	int ret;

	retif(data == NULL, , "Invalid parameter!");

	ret = vconf_get_int(VCONFKEY_CALL_STATE, &status);
	if (ret == FAIL) {
		ERR("Failed to get VCONFKEY_CALL_STATE!");
		return;
	}

	INFO("Call state = %d", status);
	switch (status) {
	case VCONFKEY_CALL_VOICE_CONNECTING:
	case VCONFKEY_CALL_VIDEO_CONNECTING:
		show_image_icon(data);
		indicator_util_icon_animation_set(&call, ICON_ANI_BLINK);
		break;
	case VCONFKEY_CALL_VOICE_ACTIVE:
	case VCONFKEY_CALL_VIDEO_ACTIVE:
		show_image_icon(data);
		indicator_util_icon_animation_set(&call, ICON_ANI_NONE);
		break;
	case VCONFKEY_CALL_OFF:
		INFO("Call off");
		indicator_util_icon_animation_set(&call, ICON_ANI_NONE);
		hide_image_icon();
		break;
	default:
		ERR("Invalid value %d", status);
		break;
	}
}

static int register_call_module(void *data)
{
	int ret;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONFKEY_CALL_STATE,
				       indicator_call_change_cb, data);
	if (ret != OK)
		ERR("Failed to register callback!");

	indicator_call_change_cb(NULL, data);

	return ret;
}

static int unregister_call_module(void)
{
	int ret;

	ret = vconf_ignore_key_changed(VCONFKEY_CALL_STATE,
				       indicator_call_change_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	if (evt_hl)
		ecore_event_handler_del(evt_hl);
	hide_image_icon();

	return OK;
}
