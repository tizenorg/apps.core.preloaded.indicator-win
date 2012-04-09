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

#define ICON_PRIORITY	INDICATOR_PRIORITY_NON_FIXED_5
#define MODULE_NAME		"nfc"

static int register_nfc_module(void *data);
static int unregister_nfc_module(void);
static int hib_enter_nfc_module(void);
static int hib_leave_nfc_module(void *data);

Indicator_Icon_Object nfc = {
	.type = INDICATOR_IMG_ICON,
	.name = MODULE_NAME,
	.priority = ICON_PRIORITY,
	.always_top = EINA_TRUE,
	.exist_in_view = EINA_FALSE,
	.txt_obj = {0,},
	.img_obj = {0,},
	.obj_exist = EINA_FALSE,
	.fixed = EINA_FALSE,
	.init = register_nfc_module,
	.fini = unregister_nfc_module,
	.hib_enter = hib_enter_nfc_module,
	.hib_leave = hib_leave_nfc_module
};

enum {
	NFC_ON = 0,
	NFC_NUM,
};


static const char *icon_path[NFC_NUM] = {
	[NFC_ON] = "Bluetooth_NFC_GPS/B03_NFC_On.png",
};

static void show_image_icon(void *data, int i)
{
	if (i < NFC_ON || i >= NFC_NUM)
		i = NFC_ON;

	nfc.img_obj.data = icon_path[i];
	indicator_util_icon_show(&nfc);
}

static void hide_image_icon(void)
{
	indicator_util_icon_hide(&nfc);
}

static void indicator_nfc_change_cb(keynode_t *node, void *data)
{
	int status;
	int ret;

	retif(data == NULL, , "Invalid parameter!");

	ret = vconf_get_bool(VCONFKEY_NFC_STATE, &status);
	if (ret == OK) {
		INFO("NFC STATUS: %d", status);

		if (status == 1) {
			/* Show NFC Icon */
			show_image_icon(data, NFC_ON);
			return;
		}
	}

	hide_image_icon();
	return;
}

static int register_nfc_module(void *data)
{
	int ret;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONFKEY_NFC_STATE,
				       indicator_nfc_change_cb, data);
	if (ret != OK)
		ERR("Failed to register callback!");

	indicator_nfc_change_cb(NULL, data);

	return ret;
}

static int unregister_nfc_module(void)
{
	int ret;

	ret = vconf_ignore_key_changed(VCONFKEY_NFC_STATE,
				       indicator_nfc_change_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	hide_image_icon();

	return OK;
}

static int hib_enter_nfc_module(void)
{
	int ret;

	ret = vconf_ignore_key_changed(VCONFKEY_NFC_STATE,
				       indicator_nfc_change_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	return OK;
}

static int hib_leave_nfc_module(void *data)
{
	int ret;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONFKEY_NFC_STATE,
				       indicator_nfc_change_cb, data);
	retif(ret != OK, FAIL, "Failed to register callback!");

	indicator_nfc_change_cb(NULL, data);
	return OK;
}
