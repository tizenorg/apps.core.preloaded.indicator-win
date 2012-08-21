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

#include <status-monitor.h>

#include "indicator_ui.h"
#include "common.h"

#define MSG_TIMEOUT 3.0

static Ecore_Timer *msg_timer;

static void inline _hide_message(Evas_Object *layout)
{
	if (layout)
		elm_object_signal_emit(layout,
			"message.hide", "indicator.prog");
}

static void inline _show_message(Evas_Object *layout)
{
	if (layout)
		elm_object_signal_emit(layout,
			"message.show", "indicator.prog");
}

static Eina_Bool _msg_timeout_cb(void *data)
{
	struct appdata *ad = NULL;

	msg_timer = NULL;

	if (!data)
		return ECORE_CALLBACK_CANCEL;

	ad = data;

	_hide_message(ad->layout_main);

	return ECORE_CALLBACK_CANCEL;
}

static void __message_callback(const char *message, void *data)
{
	struct appdata *ad = NULL;
	char *text = NULL;
	int w = 0;

	if (!data)
		return;

	ad = data;

	if (!ad->layout_main)
		return;

	if (msg_timer)
		ecore_timer_del(msg_timer);

	_hide_message(ad->layout_main);

	if (!message)
		return;

	text = evas_textblock_text_utf8_to_markup(NULL, message);
	if (!text)
		return;

	elm_object_part_text_set(ad->layout_main, "message.text", text);
	free(text);

	_show_message(ad->layout_main);

	msg_timer = ecore_timer_add(MSG_TIMEOUT, _msg_timeout_cb, data);
}

int indicator_message_init(void *data)
{
	int ret = 0;

	ret = status_monitor_message_cb_set(__message_callback, data);

	return ret;
}

int indicator_message_fini(void)
{
	int ret = 0;

	ret = status_monitor_message_cb_unset();

	return ret;
}


