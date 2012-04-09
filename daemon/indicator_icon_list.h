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


#ifndef __INDICATOR_ICON_LIST_H__
#define __INDICATOR_ICON_LIST_H__

#include "indicator.h"
extern void indicator_icon_object_free(Indicator_Icon_Object *icon);
extern int indicator_icon_list_free(void);
extern int indicator_icon_list_insert(Indicator_Icon_Object *obj);
extern int indicator_icon_list_remove(Indicator_Icon_Object *obj);
extern int indicator_icon_list_update(Indicator_Icon_Object *obj);
extern Indicator_Icon_Object
*indicator_get_prev_icon(Indicator_Icon_Object      *obj);
extern Indicator_Icon_Object *indicator_get_last_shown_icon(void);
extern Indicator_Icon_Object *indicator_get_first_shown_icon(void);
extern Indicator_Icon_Object *indicator_get_hidden_icon(void);
extern Indicator_Icon_Object
*indicator_get_shown_same_priority_icon_in_fixed_view_list(int);
extern Indicator_Icon_Object
*indicator_get_wish_to_show_icon(Indicator_Icon_Object *obj);
extern Indicator_Icon_Object
*indicator_get_wish_to_remove_icon(Indicator_Icon_Object *obj);
extern Indicator_Icon_Object
*indicator_get_lowest_priority_icon_in_wish_show_list(void);
extern Indicator_Icon_Object
*indicator_get_highest_priority_icon_in_wish_show_list(void);
extern int indicator_get_same_priority_icons(Eina_List **result,
					     Indicator_Icon_Object *obj);

#endif /*__INDICATOR_ICON_LIST_H__*/
