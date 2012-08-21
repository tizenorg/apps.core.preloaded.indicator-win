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


#ifndef __INDICATOR_BOX_UTIL_H__
#define __INDICATOR_BOX_UTIL_H__

#include <Elementary.h>
#include "indicator.h"

typedef enum _Icon_AddType {
	CANNOT_ADD = -1,
	CAN_ADD_WITH_DEL,
	CAN_ADD_WITHOUT_DEL,
} Icon_AddType;

typedef enum _Icon_Display_Count {
	BATTERY_TEXT_ON_COUNT = -1,
	BATTERY_TEXT_OFF_COUNT = 0,
	PORT_NONFIXED_ICON_COUNT = 6,
	LAND_NONFIXED_ICON_COUNT = 18,
} Icon_Display_Count;

extern int icon_box_pack(Indicator_Icon_Object *icon);
extern int icon_box_unpack(Indicator_Icon_Object *icon);
extern int icon_box_init(void *data);
extern int icon_box_fini(void *data);
extern void indicator_util_update_display(void *data);
extern void indicator_util_hide_all_icons(void);
extern unsigned int indicator_get_count_in_fixed_list(void);
extern unsigned int indicator_get_count_in_non_fixed_list(void);
extern void indicator_set_count_in_non_fixed_list(int);
extern int indicator_get_max_count_in_non_fixed_list(void);
extern Icon_AddType
indicator_is_enable_to_insert_in_non_fixed_list(Indicator_Icon_Object *obj);
extern int indicator_util_get_priority_in_move_area(Evas_Coord, Evas_Coord);
int indicator_util_check_home_icon_area(Evas_Coord curr_x, Evas_Coord curr_y);

#endif /*__INDICATOR_BOX_UTIL_H__*/
