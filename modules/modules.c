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


#include "modules.h"

/* Home */
extern Indicator_Icon_Object home;

/* Network */
extern Indicator_Icon_Object rssi;
extern Indicator_Icon_Object roaming;

/* Connection */
extern Indicator_Icon_Object usb;
extern Indicator_Icon_Object wifi;
extern Indicator_Icon_Object mobile_hotspot;
extern Indicator_Icon_Object conn;

/* Processing */
extern Indicator_Icon_Object trnsfr;
extern Indicator_Icon_Object call;
extern Indicator_Icon_Object call_divert;
extern Indicator_Icon_Object active_sync;

/* Information */
extern Indicator_Icon_Object mmc;
extern Indicator_Icon_Object noti;

/* message */
/* extern Indicator_Icon_Object message; */

/* voice mail*/
/* extern Indicator_Icon_Object voice_mail; */

/*
extern Indicator_Icon_Object missed_call;
extern Indicator_Icon_Object im;
*/

/* alarm */
extern Indicator_Icon_Object useralarm;

/* playing */
extern Indicator_Icon_Object fm_radio;
extern Indicator_Icon_Object mp3_play;
extern Indicator_Icon_Object voice_recorder;

/* Setting */
extern Indicator_Icon_Object silent;
extern Indicator_Icon_Object bluetooth;
extern Indicator_Icon_Object rotate;
extern Indicator_Icon_Object gps;
extern Indicator_Icon_Object nfc;
extern Indicator_Icon_Object wifi_direct;

/* Clock */
extern Indicator_Icon_Object sysclock;

/* Power */
extern Indicator_Icon_Object battery;

static Indicator_Icon_Object *modules[] = {
	/* Home */
	&home,
	/* Network */
	&rssi,
	&roaming,
	/* Connection */
	&usb,
	&wifi,
	&mobile_hotspot,
	&conn,
	/* Processing */
	&trnsfr,
	&call,
	&call_divert,

	/* 
	   &active_sync, 
	 */

	/* Information */
	/* &message, */
	/* &voice_mail, */
	&mmc,
	&noti,
	&useralarm,
	&fm_radio,
	&mp3_play,
	&voice_recorder,

	/* Setting */
	&silent,
	&bluetooth,
	&rotate,
	&gps,
	&nfc,
	&wifi_direct,

	/* Clock */
	&sysclock,
	/* Power */
	&battery,
/* Add your module object here */
	NULL
};

void indicator_init_modules(void *data)
{
	int i;
	/* add items to list */
	for (i = 0; modules[i]; i++) {
		indicator_icon_list_insert(modules[i]);
		modules[i]->ad = data;
		if (modules[i]->init)
			modules[i]->init(data);
	}
}

void indicator_fini_modules(void *data)
{
	int i;
	/* add items to list */
	for (i = 0; modules[i]; i++) {
		if (modules[i]->fini)
			modules[i]->fini();
	}
	/* delete modules */
	indicator_icon_list_free();
}

void indicator_hib_enter_modules(void *data)
{
	int i;
	/* add items to list */
	for (i = 0; modules[i]; i++) {
		if (modules[i]->hib_enter)
			modules[i]->hib_enter();
	}
}

void indicator_hib_leave_modules(void *data)
{
	int i;
	/* add items to list */
	for (i = 0; modules[i]; i++) {
		if (modules[i]->hib_leave)
			modules[i]->hib_leave(data);
	}
}

void indicator_lang_changed_modules(void *data)
{
	int i;
	/* add items to list */
	for (i = 0; modules[i]; i++) {
		if (modules[i]->lang_changed)
			modules[i]->lang_changed(data);
	}
}

void indicator_region_changed_modules(void *data)
{
	int i;
	/* add items to list */
	for (i = 0; modules[i]; i++) {
		if (modules[i]->region_changed)
			modules[i]->region_changed(data);
	}
}
