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

#define ICON_PRIORITY	INDICATOR_PRIORITY_NON_FIXED_7
#define MODULE_NAME		"FM_Radio"

static int register_fm_radio_module(void *data);
static int unregister_fm_radio_module(void);

Indicator_Icon_Object fm_radio = {
	.type = INDICATOR_IMG_ICON,
	.name = MODULE_NAME,
	.priority = ICON_PRIORITY,
	.always_top = EINA_FALSE,
	.exist_in_view = EINA_FALSE,
	.txt_obj = {0,},
	.img_obj = {0,},
	.obj_exist = EINA_FALSE,
	.fixed = EINA_FALSE,
	.init = register_fm_radio_module,
	.fini = unregister_fm_radio_module
};

static char *icon_path[] = {
	"Background_playing/B03_Backgroundplaying_FMradio.png",
	NULL
};

static void show_image_icon(void *data)
{
	fm_radio.img_obj.data = icon_path[0];
	indicator_util_icon_show(&fm_radio);
}

static void hide_image_icon(void)
{
	indicator_util_icon_hide(&fm_radio);
}

static void indicator_fm_radio_change_cb(keynode_t *node, void *data)
{
	int status;
	int ret;

	retif(data == NULL, , "Invalid parameter!");

	ret = vconf_get_int(VCONFKEY_RADIO_STATE, &status);
	if (ret == OK) {
		INFO("FM_RADIO state: %d", status);
		if (status == VCONFKEY_RADIO_PLAY)
			show_image_icon(data);
		else
			hide_image_icon();
	}
	return;
}

static int register_fm_radio_module(void *data)
{
	int ret;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONFKEY_RADIO_STATE,
				       indicator_fm_radio_change_cb, data);
	if (ret != OK)
		ERR("Failed to register callback!");
	indicator_fm_radio_change_cb(NULL, data);

	return ret;
}

static int unregister_fm_radio_module(void)
{
	int ret;

	ret = vconf_ignore_key_changed(VCONFKEY_RADIO_STATE,
				       indicator_fm_radio_change_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	hide_image_icon();

	return OK;
}
