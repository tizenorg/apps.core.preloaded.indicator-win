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

#include <stdio.h>
#include <stdlib.h>
#include <vconf.h>
#include <heynoti.h>
#include <runtime_info.h>
#include <Ecore_X.h>
#include "common.h"
#include "indicator.h"
#include "indicator_ui.h"
#include "indicator_gui.h"
#include "indicator_icon_util.h"
#include "modules.h"

#define SETTING_TIME_CHANGED_NOTI	"setting_time_changed"
#define SYSTEM_RESUME				"system_wakeup"

#define TIME_FONT_SIZE		26
#define TIME_FONT_COLOR		127, 104, 79, 255

#define AMPM_FONT_SIZE		20
#define AMPM_FONT_COLOR		148, 125, 99, 255
#define LABEL_STRING		"<font_size=%d>%s" \
				"</font_size></font>"

enum {
	INDICATOR_CLOCK_MODE_12H = 0,
	INDICATOR_CLOCK_MODE_24H,
	INDICATOR_CLOCK_MODE_MAX
};

static int notifd;
static int clock_mode = INDICATOR_CLOCK_MODE_12H;
static Ecore_Timer *timer;

static int register_clock_module(void *data);
static int unregister_clock_module(void);
static int hib_enter_clock_module(void);
static int hib_leave_clock_module(void *data);
static int language_changed_cb(void *data);
static int region_changed_cb(void *data);

Indicator_Icon_Object sysclock = {
	.type = INDICATOR_TXT_ICON,
	.name = "clock",
	.priority = INDICATOR_PRIORITY_FIXED1,
	.always_top = EINA_FALSE,
	.txt_obj = {0,},
	.img_obj = {0,},
	.obj_exist = EINA_FALSE,
	.exist_in_view = EINA_FALSE,
	.init = register_clock_module,
	.fini = unregister_clock_module,
	.hib_enter = hib_enter_clock_module,
	.hib_leave = hib_leave_clock_module,
	.lang_changed = NULL,
	.region_changed = region_changed_cb,
};

static void indicator_clock_changed_cb(void *data)
{
	struct appdata *ad = (struct appdata *)data;
	char time_str[32];
	char time_buf[128], ampm_buf[128];
	char buf[256];
	struct tm *ts = NULL;
	time_t ctime;
	int len;

	retif(data == NULL, , "Invalid parameter!");

	/* Set time */
	ctime = time(NULL);
	ts = localtime(&ctime);
	if (ts == NULL)
		return;

	if (timer != NULL) {
		ecore_timer_del(timer);
		timer = NULL;
	}

	memset(time_str, 0x00, sizeof(time_str));
	memset(time_buf, 0x00, sizeof(time_buf));
	memset(ampm_buf, 0x00, sizeof(ampm_buf));
	memset(buf, 0x00, sizeof(buf));

	timer =
	    ecore_timer_add(60 - ts->tm_sec, (void *)indicator_clock_changed_cb,
			    data);
	if (clock_mode == INDICATOR_CLOCK_MODE_12H) {
		if (ts->tm_hour >= 0 && ts->tm_hour < 12)
			snprintf(ampm_buf, sizeof(ampm_buf),
				 LABEL_STRING, AMPM_FONT_SIZE,
				 _("IDS_COM_POP_AM_M_ABB"));
		else
			snprintf(ampm_buf, sizeof(ampm_buf),
				 LABEL_STRING, AMPM_FONT_SIZE,
				 _("IDS_COM_POP_PM_M_ABB"));
		strftime(time_str, sizeof(time_str), "%I:%M", ts);
	} else {
		strftime(time_str, sizeof(time_str), "%H:%M", ts);
	}

	snprintf(time_buf, sizeof(time_buf), LABEL_STRING,
		 TIME_FONT_SIZE, time_str);
	len = snprintf(buf, sizeof(buf), "%s%s", time_buf, ampm_buf);
	if (len < 0) {
		ERR("Unexpected ERROR!");
		return;
	}

	INFO("[CLOCK MODULE] Timer Status : %d Time: %s", timer, buf);

	edje_object_part_text_set(elm_layout_edje_get(ad->layout_main),
				  "elm.text.clock", buf);
	return;
}

static void indicator_clock_format_changed_cb(keynode_t *node, void *data)
{
	retif(data == NULL, , "Invalid parameter!");

	int r = -1;

	bool is_24hour_enabled = false;

	INFO("[Enter] indicator_clock_format_changed_cb");

	r = runtime_info_get_value_bool(
			RUNTIME_INFO_KEY_24HOUR_CLOCK_FORMAT_ENABLED, &is_24hour_enabled);

	/* Check Time format. If timeformat have invalid value, Set to 12H */
	if( r==RUNTIME_INFO_ERROR_NONE&&is_24hour_enabled==true)
	{
		clock_mode = INDICATOR_CLOCK_MODE_24H;
	}
	else
	{
		clock_mode = INDICATOR_CLOCK_MODE_12H;
	}

	indicator_clock_changed_cb(data);
}

static int language_changed_cb(void *data)
{
	indicator_clock_changed_cb(data);
	return OK;
}

static int region_changed_cb(void *data)
{
	indicator_clock_format_changed_cb(NULL, data);
	return OK;
}

static int register_clock_module(void *data)
{
	int r = 0, ret = -1;

	retif(data == NULL, FAIL, "Invalid parameter!");

	notifd = heynoti_init();

	ret =
	    heynoti_subscribe(notifd, SETTING_TIME_CHANGED_NOTI,
			      indicator_clock_changed_cb, data);
	if (ret != OK) {
		ERR("Fail: register SETTING_TIME_CHANGED_NOTI");
		r = ret;
	}

	ret =
	    heynoti_subscribe(notifd, SYSTEM_RESUME, indicator_clock_changed_cb,
			      data);
	if (ret != OK) {
		ERR("Fail: register SYSTEM_RESUME");
		r = r | ret;
	}

	ret = heynoti_attach_handler(notifd);
	if (ret != OK) {
		ERR("Failed to attach heynoti handler!");
		r = r | ret;
	}

	ret = vconf_notify_key_changed(VCONFKEY_REGIONFORMAT_TIME1224,
				       indicator_clock_format_changed_cb, data);
	if (ret != OK) {
		ERR("Fail: register VCONFKEY_REGIONFORMAT_TIME1224");
		r = r | ret;
	}

	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_TIMEZONE_INT,
				       indicator_clock_format_changed_cb, data);
	if (ret != OK) {
		ERR("Fail: register VCONFKEY_SETAPPL_TIMEZONE_INT");
		r = r | ret;
	}

	indicator_clock_format_changed_cb(NULL, data);

	return r;
}

static int unregister_clock_module(void)
{
	int ret;

	heynoti_unsubscribe(notifd, SETTING_TIME_CHANGED_NOTI,
			    indicator_clock_changed_cb);

	heynoti_unsubscribe(notifd, SYSTEM_RESUME, indicator_clock_changed_cb);

	heynoti_close(notifd);
	notifd = 0;

	ret = vconf_ignore_key_changed(VCONFKEY_REGIONFORMAT_TIME1224,
				       indicator_clock_format_changed_cb);
	if (ret != OK)
		ERR("Fail: unregister VCONFKEY_REGIONFORMAT_TIME1224");

	ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_TIMEZONE_INT,
				       indicator_clock_format_changed_cb);
	if (ret != OK)
		ERR("Fail: unregister VCONFKEY_SETAPPL_TIMEZONE_INT");

	if (timer != NULL) {
		ecore_timer_del(timer);
		timer = NULL;
	}
	return OK;
}

static int hib_enter_clock_module(void)
{
	int ret;

	ret = vconf_ignore_key_changed(VCONFKEY_REGIONFORMAT_TIME1224,
				       indicator_clock_format_changed_cb);
	if (ret != OK)
		ERR("Fail: unregister VCONFKEY_REGIONFORMAT_TIME1224");

	if (timer != NULL) {
		ecore_timer_del(timer);
		timer = NULL;
	}

	return OK;
}

static int hib_leave_clock_module(void *data)
{
	int ret;

	retif(data == NULL, FAIL, "Invalid parameter!");

	ret = vconf_notify_key_changed(VCONFKEY_REGIONFORMAT_TIME1224,
				       indicator_clock_format_changed_cb, data);
	retif(ret != OK, FAIL, "Failed to register callback!");

	indicator_clock_format_changed_cb(NULL, data);
	return OK;
}
