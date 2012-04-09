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
#include "indicator_ui.h"
#include "modules.h"
#include "indicator_icon_util.h"

#define ICON_PRIORITY	INDICATOR_PRIORITY_NON_FIXED_1
#define MODULE_NAME		"silent"

static int register_silent_module(void *data);
static int unregister_silent_module(void);
static int hib_enter_silent_module(void);
static int hib_leave_silent_module(void *data);

Indicator_Icon_Object silent = {
	.type = INDICATOR_IMG_ICON,
	.name = MODULE_NAME,
	.priority = ICON_PRIORITY,
	.always_top = EINA_FALSE,
	.exist_in_view = EINA_FALSE,	
	.txt_obj = {0,},
	.img_obj = {0,},
	.obj_exist = EINA_FALSE,
	.fixed = EINA_FALSE,
	.init = register_silent_module,
	.fini = unregister_silent_module,
	.hib_enter = hib_enter_silent_module,
	.hib_leave = hib_leave_silent_module
};

enum {
	PROFILE_SOUND_VIBRATION,
	PROFILE_MUTE,
	PROFILE_VIBRATION,
	PROFILE_NUM,
};

static const char *icon_path[PROFILE_NUM] = {
	[PROFILE_SOUND_VIBRATION] = "Profile/B03_Profile_Sound_Vibration.png",
	[PROFILE_MUTE] = "Profile/B03_Profile_Mute.png",
	[PROFILE_VIBRATION] = "Profile/B03_Profile_Vibration.png",
};

static void indicator_silent_change_cb(keynode_t *node, void *data)
{
	int sound_status = 0;
	int vib_status = 0;
	int ret;
	const char *selected_image = NULL;

	retif(data == NULL, , "Invalid parameter!");

	ret = vconf_get_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL, &sound_status);
	ret =
	    vconf_get_bool(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL, &vib_status);

	if (ret == OK) {
		INFO("CURRENT Sound Status: %d vib_status: %d", sound_status,
		     vib_status);

		if (sound_status == TRUE) {
			if (vib_status == TRUE)
				/* Sound and Vibration Mode */
				selected_image =
					icon_path[PROFILE_SOUND_VIBRATION];
		} else {
			if (vib_status != TRUE)
				/* Mute Mode */
				selected_image = icon_path[PROFILE_MUTE];
			else
				/* Vibration Only Mode */
				selected_image = icon_path[PROFILE_VIBRATION];
		}

		if (selected_image != NULL) {
			silent.img_obj.data = selected_image;
			indicator_util_icon_show(&silent);
			return;
		}

		/* If path is not set to 'selected_image', Hide All Image */
		indicator_util_icon_hide(&silent);
		return;
	}

	ERR("Failed to get current profile!");
	return;
}

static int register_silent_module(void *data)
{
	int ret;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL,
				       indicator_silent_change_cb, data);
	if (ret != OK)
		ERR("Fail: register VCONFKEY_SETAPPL_SOUND_STATUS_BOOL");

	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL,
				       indicator_silent_change_cb, data);
	if (ret != OK)
		ERR("Fail: register VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL");

	indicator_silent_change_cb(NULL, data);

	return ret;
}

static int unregister_silent_module(void)
{
	int ret;

	ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL,
				       indicator_silent_change_cb);
	if (ret != OK)
		ERR("Fail: ignore VCONFKEY_SETAPPL_SOUND_STATUS_BOOL");

	ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL,
				       indicator_silent_change_cb);
	if (ret != OK)
		ERR("Fail: ignore VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL");

	indicator_util_icon_hide(&silent);

	return OK;
}

static int hib_enter_silent_module(void)
{
	int ret;

	ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL,
				       indicator_silent_change_cb);
	if (ret != OK)
		ERR("H_Fail: ignore VCONFKEY_SETAPPL_SOUND_STATUS_BOOL");

	ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL,
				       indicator_silent_change_cb);
	if (ret != OK)
		ERR("H_Fail: ignore VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL");

	return OK;
}

static int hib_leave_silent_module(void *data)
{
	int ret;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL,
				       indicator_silent_change_cb, data);
	if (ret != OK)
		ERR("H_Fail: register VCONFKEY_SETAPPL_SOUND_STATUS_BOOL");

	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL,
				       indicator_silent_change_cb, data);
	if (ret != OK)
		ERR("H_Fail: register VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL");

	indicator_silent_change_cb(NULL, data);

	return OK;
}
