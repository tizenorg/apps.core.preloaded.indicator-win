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


#ifndef __INDICATOR_H__
#define __INDICATOR_H__

#include <Elementary.h>
#include <Eina.h>

#define VCONF_INDICATOR_HOME_PRESSED "memory/private/"PACKAGE_NAME"/home_pressed"


#ifndef VCONFKEY_INDICATOR_STARTED
#define VCONFKEY_INDICATOR_STARTED "memory/private/"PACKAGE_NAME"/started"
#endif /* VCONFKEY_INDICATOR_STARTED */


enum {
	/* Value is allocated from Left side in status bar */
	INDICATOR_PRIORITY_FIXED_MIN = 0,
	INDICATOR_PRIORITY_FIXED5 = INDICATOR_PRIORITY_FIXED_MIN,/* Home */
	INDICATOR_PRIORITY_FIXED4,		/* Network,Wi-Fi,Wimax */
	INDICATOR_PRIORITY_FIXED3,		/* RSSI, Flight Mode, Roaming */
	INDICATOR_PRIORITY_FIXED2,		/* Battery, Power, Connected */
	INDICATOR_PRIORITY_FIXED1,		/* Clock */
	INDICATOR_PRIORITY_FIXED_MAX = INDICATOR_PRIORITY_FIXED1,
	INDICATOR_PRIORITY_NON_FIXED_MIN,
	/* 3rd app Event */
	INDICATOR_PRIORITY_NON_FIXED_10 = INDICATOR_PRIORITY_NON_FIXED_MIN,
	INDICATOR_PRIORITY_NON_FIXED_9,	/* Rotation Lock */
	INDICATOR_PRIORITY_NON_FIXED_8, /* USB */
	INDICATOR_PRIORITY_NON_FIXED_7, /* Music,FM Radio,Voice Recorder */
	INDICATOR_PRIORITY_NON_FIXED_6, /* Data Transffering,Downloading,
					 * Uploading,sync,call divert,DB update
					 */
	INDICATOR_PRIORITY_NON_FIXED_5, /* Bluetooth,NFC,GPS */
	INDICATOR_PRIORITY_NON_FIXED_4, /* Email, Message, IM */
	INDICATOR_PRIORITY_NON_FIXED_3, /* Call, SOS, Missed Call */
	INDICATOR_PRIORITY_NON_FIXED_2, /* Alarm */
	INDICATOR_PRIORITY_NON_FIXED_1, /* Silent / Vibration */
	INDICATOR_PRIORITY_NON_FIXED_MAX = INDICATOR_PRIORITY_NON_FIXED_1
};

enum indicator_icon_type {
	INDICATOR_IMG_ICON = 0,
	INDICATOR_TXT_ICON,
	INDICATOR_TXT_WITH_IMG_ICON
};

enum indicator_icon_ani {
	ICON_ANI_NONE = 0,
	ICON_ANI_BLINK,
	ICON_ANI_ROTATE,
	ICON_ANI_METRONOME,
	ICON_ANI_MAX
};

typedef struct _Text_Icon_Info {
	char *data;
	Evas_Object *obj;
	int width;
} Text_Icon_Info;

typedef struct _Img_Icon_Info {
	const char *data;
	Evas_Object *obj;
	int width;
} Img_Icon_Info;

typedef struct Indicator_Icon {
	char *name;
	enum indicator_icon_type type;
	int priority;
	Eina_Bool always_top;	/* only for SOS messsage */

	/* func */
	int (*init) (void *);
	int (*fini) (void);
	int (*hib_enter) (void);
	int (*hib_leave) (void *);
	int (*lang_changed) (void *);
	int (*region_changed) (void *);

	/* do not modify this area */
	/* internal data */

	char data[1024];
	void *ad;
	Eina_Bool obj_exist;
	Text_Icon_Info txt_obj;
	Img_Icon_Info img_obj;
	Eina_Bool wish_to_show;
	Eina_Bool exist_in_view;
	Eina_Bool fixed;
	enum indicator_icon_ani ani;
} Indicator_Icon_Object;

#endif /*__INDICATOR_H__*/
