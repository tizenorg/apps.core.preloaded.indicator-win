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
extern Indicator_Icon_Object sos;
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

/* Smart stay */
extern Indicator_Icon_Object smart_stay;

static Indicator_Icon_Object *modules[] = {
	/* Home */
	&home,
	/* Network */
	&rssi,
	&roaming,
	/* Connection */
	&sos,
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
	/*Smart Stay*/
    &smart_stay,
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
