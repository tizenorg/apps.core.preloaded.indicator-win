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


#ifndef __INDICATOR_GUI_H__
#define __INDICATOR_GUI_H__

/* DISPLAY */
#define INDICATOR_HEIGHT 50
#define ICON_PADDING		3

#define PADDING_WIDTH		10
#define LEFT_PADDING_WIDTH	0
#define RIGHT_PADDING_WIDTH	8
#define INDICATOR_PADDING_H 6


#define CLOCK_WIDTH 	98
#define CLOCK_PADDING 	10

#define DEFAULT_ICON_PADDING 10

#define ICON_HEIGHT		   38
#define ICON_WIDTH		   38
#define DEFAULT_ICON_HEIGHT	38
#define DEFAULT_ICON_WIDTH	38

#define NONFIXED_DEFAULT_PADDING 25
#define NONFIXED_DYNAMIC_PADDING 11
#define FIXED4_DYNAMIC_PADDING 0
#define FIXED2_PADDING  2
#define FIXED1_PADDING 72
#define FIXED0_PADDING 72
#define LANDSCAPE_FIXED1_PADDING 122
#define LANDSCAPE_FIXED0_PADDING 122

#define HOME_PADDING_PORTRAIT 180
#define HOME_PADDING_LANDSCAPE 280
#define DEFAULT_NON_FIXED_PADDING 0

#define INDI_DEFAULT_BG_COLOR 0 0 0 0
#define INDI_DYNAMIC_BG_COLOR 0 0 0 0
#define INDI_SEPARATOR_BG_COLOR 0 0 0 0

/* FONT */
#define ICON_FONT_NAME		"SLP"
#define ICON_FONT_STYLE		"Medium"
#define ICON_FONT_SIZE		20
#define FONT_COLOR		255, 255, 255, 255

#define DEFAULT_LABEL_STRING	"<font_size=%d><color=#%02x%02x%02x%02x>%s</color></font_size>"
#define CUSTOM_LABEL_STRING		"<font=%s><style=%s><font_size=%d>%s</font_size></style></font>"

#endif /*__INDICATOR_GUI_H__*/
