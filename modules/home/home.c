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
			INFO("change_home_icon_cb : Home Button Pressed!");
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
