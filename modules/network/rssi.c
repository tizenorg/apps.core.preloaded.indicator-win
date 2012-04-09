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

#define ICON_PRIORITY	INDICATOR_PRIORITY_FIXED3
#define MODULE_NAME		"RSSI"
#define RSSI_WIDTH		38

#define ICON_NOSIM		_("IDS_COM_BODY_NO_SIM")
#define ICON_SEARCH		_("IDS_COM_BODY_SEARCHING")
#define ICON_NOSVC		_("IDS_CALL_POP_NOSERVICE")

static int register_rssi_module(void *data);
static int unregister_rssi_module(void);
static int hib_enter_rssi_module(void);
static int hib_leave_rssi_module(void *data);
static int language_changed_cb(void *data);

Indicator_Icon_Object rssi = {
	.type = INDICATOR_IMG_ICON,
	.name = MODULE_NAME,
	.priority = ICON_PRIORITY,
	.always_top = EINA_FALSE,
	.exist_in_view = EINA_FALSE,
	.txt_obj = {0,},
	.img_obj = {0,},
	.obj_exist = EINA_FALSE,
	.fixed = EINA_TRUE,
	.init = register_rssi_module,
	.fini = unregister_rssi_module,
	.hib_enter = hib_enter_rssi_module,
	.hib_leave = hib_leave_rssi_module,
	.lang_changed = language_changed_cb
};

enum {
	LEVEL_RSSI_MIN = 0,
	LEVEL_RSSI_0 = LEVEL_RSSI_MIN,
	LEVEL_RSSI_1,
	LEVEL_RSSI_2,
	LEVEL_RSSI_3,
	LEVEL_RSSI_4,
	LEVEL_RSSI_5,
	LEVEL_RSSI_6,
	LEVEL_RSSI_MAX = LEVEL_RSSI_6,
	LEVEL_FLIGHT,
	LEVEL_NOSIM,
	LEVEL_SEARCH,
	LEVEL_NOSVC,
	LEVEL_MAX,
};

static const char *icon_path[LEVEL_MAX] = {
	[LEVEL_RSSI_0] = "RSSI/B03_RSSI_Sim_00.png",
	[LEVEL_RSSI_1] = "RSSI/B03_RSSI_Sim_01.png",
	[LEVEL_RSSI_2] = "RSSI/B03_RSSI_Sim_02.png",
	[LEVEL_RSSI_3] = "RSSI/B03_RSSI_Sim_03.png",
	[LEVEL_RSSI_4] = "RSSI/B03_RSSI_Sim_04.png",
	[LEVEL_RSSI_5] = "RSSI/B03_RSSI_Sim_05.png",
	/* Currently We don't have RSSI LEVEL 6 Image */
	[LEVEL_RSSI_6] = "RSSI/B03_RSSI_Sim_05.png",
	[LEVEL_FLIGHT] = "RSSI/B03_RSSI_Flightmode.png",
	[LEVEL_NOSIM] = "RSSI/B03_RSSI_NoSim.png",
	[LEVEL_SEARCH] = "RSSI/B03_RSSI_Searching.png",
	[LEVEL_NOSVC] = "RSSI/B03_RSSI_NoService.png",
};

static int level_check(int *level)
{
	if (*level < LEVEL_RSSI_MIN) {
		*level = LEVEL_RSSI_MIN;
		return -1;
	} else if (*level > LEVEL_RSSI_MAX) {
		*level = LEVEL_RSSI_MAX;
		return 1;
	}
	return 0;
}

static void show_image_icon(void *data, int i)
{
	if (i < LEVEL_RSSI_MIN)
		i = LEVEL_RSSI_MIN;
	else if (i >= LEVEL_MAX)
		i = LEVEL_NOSVC;

	if (rssi.type == INDICATOR_TXT_ICON) {
		indicator_util_icon_hide(&rssi);
		rssi.type = INDICATOR_IMG_ICON;
	}
	rssi.img_obj.width = RSSI_WIDTH;
	rssi.img_obj.data = icon_path[i];
	indicator_util_icon_show(&rssi);
}

static void show_text_icon(void *data, const char *txt)
{
	char *ret_str = NULL;
	if (rssi.type == INDICATOR_IMG_ICON) {
		rssi.type = INDICATOR_TXT_ICON;
		indicator_util_icon_hide(&rssi);
	}

	ret_str = indicator_util_icon_label_set(txt, NULL, NULL, 14, data);

	if (rssi.txt_obj.data != NULL)
		free(rssi.txt_obj.data);

	retif(ret_str == NULL, , "Fail to Set label string!");
	rssi.txt_obj.data = ret_str;
	indicator_util_icon_show(&rssi);
}

static void hide_icon(void)
{
	indicator_util_icon_hide(&rssi);
}

static void indicator_rssi_change_cb(keynode_t *node, void *data)
{
	int status = 0;
	int ret;

	retif(data == NULL, , "Invalid parameter!");

	/* First, check Flight mode */
	ret = vconf_get_bool(VCONFKEY_SETAPPL_FLIGHT_MODE_BOOL, &status);
	if (ret == OK && status == TRUE) {
		INFO("RSSI Status: Flight Mode");
		show_image_icon(data, LEVEL_FLIGHT);
		return;
	}

	/* Second, check NOSIM mode */
	ret = vconf_get_int(VCONFKEY_TELEPHONY_SIM_SLOT, &status);
	if (ret == OK && status != VCONFKEY_TELEPHONY_SIM_INSERTED) {
		INFO("RSSI Status: No SIM Mode");
		show_image_icon(data, LEVEL_NOSIM);
		return;
	}

	/* Third, check service type */
	ret = vconf_get_int(VCONFKEY_TELEPHONY_SVCTYPE, &status);
	if (ret == OK) {
		if (status == VCONFKEY_TELEPHONY_SVCTYPE_NOSVC) {
			INFO("RSSI Status: No Service");
			show_image_icon(data, LEVEL_NOSVC);
			return;
		}
		if (status == VCONFKEY_TELEPHONY_SVCTYPE_SEARCH) {
			INFO("RSSI Status: Searching Service");
			show_image_icon(data, LEVEL_SEARCH);
			return;
		}
	}

	/* Fourth, get RSSI level */
	ret = vconf_get_int(VCONFKEY_TELEPHONY_RSSI, &status);
	if (ret == OK) {
		INFO("RSSI Level: %d", status);
		level_check(&status);
		show_image_icon(data, status);
		return;
	}


	ERR("Failed to get rssi status! Set as No Service.");
	show_image_icon(data, LEVEL_NOSVC);
	return;
}

static int language_changed_cb(void *data)
{
	indicator_rssi_change_cb(NULL, data);
	return OK;
}

static int register_rssi_module(void *data)
{
	int r = 0, ret = -1;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONFKEY_TELEPHONY_RSSI,
				       indicator_rssi_change_cb, data);
	if (ret != OK) {
		ERR("Failed to register callback!");
		r = ret;
	}

	ret = vconf_notify_key_changed(VCONFKEY_TELEPHONY_SIM_SLOT,
				       indicator_rssi_change_cb, data);
	if (ret != OK) {
		ERR("Failed to register callback!");
		r = r | ret;
	}

	ret = vconf_notify_key_changed(VCONFKEY_TELEPHONY_SVCTYPE,
				       indicator_rssi_change_cb, data);
	if (ret != OK) {
		ERR("Failed to register callback!");
		r = r | ret;
	}

	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_FLIGHT_MODE_BOOL,
				       indicator_rssi_change_cb, data);
	if (ret != OK) {
		ERR("Failed to register callback!");
		r = r | ret;
	}

	indicator_rssi_change_cb(NULL, data);

	return r;
}

static int unregister_rssi_module(void)
{
	int ret;

	ret = vconf_ignore_key_changed(VCONFKEY_TELEPHONY_RSSI,
				       indicator_rssi_change_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	ret = vconf_ignore_key_changed(VCONFKEY_TELEPHONY_SIM_SLOT,
				       indicator_rssi_change_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	ret = vconf_ignore_key_changed(VCONFKEY_TELEPHONY_SVCTYPE,
				       indicator_rssi_change_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_FLIGHT_MODE_BOOL,
				       indicator_rssi_change_cb);
	if (ret != OK)
		ERR("Failed to unregister callback!");

	hide_icon();

	if (rssi.txt_obj.data != NULL)
		free(rssi.txt_obj.data);

	return OK;
}

static int hib_enter_rssi_module(void)
{
	int ret;

	ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_FLIGHT_MODE_BOOL,
				       indicator_rssi_change_cb);
	if (ret != OK) {
		ERR("Failed to unregister callback!");
	}

	return OK;
}

static int hib_leave_rssi_module(void *data)
{
	int ret;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_FLIGHT_MODE_BOOL,
				       indicator_rssi_change_cb, data);
	retif(ret != OK, FAIL, "Failed to register callback!");

	indicator_rssi_change_cb(NULL, data);
	return OK;
}
