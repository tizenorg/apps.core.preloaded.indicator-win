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

#include <stdio.h>
#include <stdlib.h>
#include <appcore-efl.h>

#include "common.h"
#include "indicator_icon_util.h"
#include "indicator_icon_list.h"
#include "indicator_ui.h"
#include "indicator_test_util.h"

int print_indicator_icon_object(Indicator_Icon_Object *obj)
{
#ifdef DEBUG_MODE
	retif(cond, ret, str, args...)(obj == NULL, FAIL, "Invalid parameter!");

	INFO(str, args...)(%s : priority(%d) obj(%x), obj->name, obj->priority,
			   (unsigned int)obj->obj);
#endif
	return OK;
}

int print_indicator_icon_list(Eina_List *list)
{
#ifdef DEBUG_MODE
	Eina_List *l;
	void *data;

	retif(list == NULL, FAIL, "Invalid parameter!");

	INFO("*******Indicator_Icon List(%x) *******", (unsigned int)list);
	EINA_LIST_FOREACH(list, l, data) {
		if (data) {
			print_indicator_icon_object(data);
		}
	}
#endif
	return OK;
}
