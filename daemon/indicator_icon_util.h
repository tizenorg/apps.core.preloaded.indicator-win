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
