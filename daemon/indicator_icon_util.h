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

#ifndef __INDICATOR_ICON_UTIL_H__
#define __INDICATOR_ICON_UTIL_H__

#include <Elementary.h>
#include "indicator.h"

enum {
	ICON_STATE_HIDDEN = 0,
	ICON_STATE_SHOWN
};

enum indicator_view_mode {
	INDICATOR_KEEP_VIEW = -1,
	INDICATOR_FULL_VIEW,
	INDICATOR_CLOCK_VIEW
};

extern int indicator_util_layout_add(void *data);
extern int indicator_util_layout_del(void *data);

extern void indicator_util_icon_show(Indicator_Icon_Object *obj);
extern void indicator_util_icon_hide(Indicator_Icon_Object *obj);
extern void indicator_util_icon_animation_set(Indicator_Icon_Object *icon,
					      enum indicator_icon_ani type);
extern int indicator_util_icon_width_set(Indicator_Icon_Object *icon);
extern void indicator_util_icon_fixed_set(Indicator_Icon_Object *icon,
					  Eina_Bool fixed);
extern char *indicator_util_icon_label_set(const char *buf, char *font_name,
					   char *font_style, int font_size,
					   void *data);
extern Eina_Bool indicator_util_icon_add(Indicator_Icon_Object *icon);
extern Eina_Bool indicator_util_icon_del(Indicator_Icon_Object *icon);

extern Eina_Bool indicator_util_is_show_icon(Indicator_Icon_Object *obj);

extern void indicator_util_event_count_set(int count, void *data);
extern unsigned int indicator_util_max_visible_event_count(void);
#endif /*__INDICATOR_ICON_UTIL_H__*/
