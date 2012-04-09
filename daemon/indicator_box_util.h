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

#endif /*__INDICATOR_BOX_UTIL_H__*/
