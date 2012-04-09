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

#define ICON_PRIORITY	INDICATOR_PRIORITY_NON_FIXED_6
#define MODULE_NAME		"transfer"
#define TIMER_INTERVAL	0.03

static int register_trnsfr_module(void *data);
static int unregister_trnsfr_module(void);

Indicator_Icon_Object trnsfr = {
	.type = INDICATOR_IMG_ICON,
	.name = MODULE_NAME,
	.priority = ICON_PRIORITY,
	.always_top = EINA_FALSE,
	.exist_in_view = EINA_FALSE,
	.txt_obj = {0,},
	.img_obj = {0,},
	.obj_exist = EINA_FALSE,
	.fixed = EINA_FALSE,
	.init = register_trnsfr_module,
	.fini = unregister_trnsfr_module
};

static const char *icon_path[] = {
	"Processing/B03_Processing_01.png",
	"Processing/B03_Processing_02.png",
	"Processing/B03_Processing_03.png",
	"Processing/B03_Processing_04.png",
	"Processing/B03_Processing_05.png",
	"Processing/B03_Processing_06.png",
	"Processing/B03_Processing_07.png",
	"Processing/B03_Processing_08.png",
	"Processing/B03_Processing_09.png",
	"Processing/B03_Processing_10.png",
	"Processing/B03_Processing_11.png",
	"Processing/B03_Processing_12.png",
	"Processing/B03_Processing_13.png",
	"Processing/B03_Processing_14.png",
	"Processing/B03_Processing_15.png",
	"Processing/B03_Processing_16.png",
	"Processing/B03_Processing_17.png",
	"Processing/B03_Processing_18.png",
	"Processing/B03_Processing_19.png",
	"Processing/B03_Processing_20.png",
	"Processing/B03_Processing_21.png",
	"Processing/B03_Processing_22.png",
	"Processing/B03_Processing_23.png",
	"Processing/B03_Processing_24.png",
	"Processing/B03_Processing_25.png",
	"Processing/B03_Processing_26.png",
	"Processing/B03_Processing_27.png",
	"Processing/B03_Processing_28.png",
	"Processing/B03_Processing_29.png",
	"Processing/B03_Processing_30.png",
	NULL
};

static Ecore_Timer *timer = NULL;
static unsigned int process_req_cnt = 0;

static void delete_timer(void)
{
	if (timer != NULL) {
		ecore_timer_del(timer);
		timer = NULL;
	}
}

static void show_image_icon(void *data, int i)
{
	trnsfr.img_obj.data = icon_path[i];
	indicator_util_icon_show(&trnsfr);
}

static void hide_image_icon(void)
{
	indicator_util_icon_hide(&trnsfr);
}

static Eina_Bool indicator_trnsfr_transfer_cb(void *data)
{
	static int i = 0;

	retif(data == NULL, ECORE_CALLBACK_CANCEL, "Invalid parameter!");

	show_image_icon(data, i);
	i = (++i % 30) ? i : 0;

	return ECORE_CALLBACK_RENEW;
}

void show_trnsfr_icon(void *data)
{
	if (process_req_cnt <= 0)
		timer = ecore_timer_add(TIMER_INTERVAL,
			indicator_trnsfr_transfer_cb, data);

	process_req_cnt++;
}

void hide_trnsfr_icon(void)
{
	process_req_cnt--;

	if (process_req_cnt <= 0) {
		process_req_cnt = 0;
		delete_timer();
		hide_image_icon();
	}
}

static int register_trnsfr_module(void *data)
{
	return OK;
}

static int unregister_trnsfr_module(void)
{
	delete_timer();
	hide_image_icon();
	process_req_cnt = 0;

	return OK;
}
