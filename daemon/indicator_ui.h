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


#ifndef __DEF_indicator_H_
#define __DEF_indicator_H_

#include <Elementary.h>
#include <Ecore_X.h>


#if !defined(PACKAGE)
#  define PACKAGE "indicator"
#endif

#if !defined(LOCALEDIR)
#  define LOCALEDIR "/opt/apps/org.tizen.indicator/res/locale"
#endif

#if !defined(EDJDIR)
#  define EDJDIR "/opt/apps/org.tizen.indicator/res/edje"
#endif

#if !defined(ICONDIR)
#  define ICONDIR "/opt/apps/org.tizen.indicator/res/icons"
#endif

#define EDJ_FILE EDJDIR			"/" PACKAGE ".edj"
#define ICON_THEME_FILE			EDJDIR "/" PACKAGE "_icon_theme.edj"
#define ICON_NONFIXED_THEME_FILE			EDJDIR "/" PACKAGE "_icon_nonfixed_theme.edj"


#define MENU_SCREEN_NAME		"org.tizen.menu-screen"
#define LOCK_SCREEN_NAME		"org.tizen.idle-lock"
#define QUICKPANEL_NAME			"E Popup"
#define CALL_NAME			"org.tizen.call"
#define VTCALL_NAME			"org.tizen.vtmain"

enum _win_type {
	/* clock view */
	TOP_WIN_NORMAL = 0,
	TOP_WIN_LOCK_SCREEN,
	/* full line of indicator */
	/* CAUTION: Don't change order!!!! */
	TOP_WIN_CALL,
	TOP_WIN_MENU_SCREEN,
	TOP_WIN_HOME_SCREEN,
	TOP_WIN_QUICKPANEL
};

enum indicator_opacity_mode{
	INDICATOR_OPACITY_OPAQUE = 0,
	INDICATOR_OPACITY_TRANSLUCENT,
	INDICATOR_OPACITY_TRANSPARENT,
};

struct appdata {
	Evas *evas;
	Evas_Object *win_main;
	Evas_Object *layout_main;	/* layout widget based on EDJ */

#ifdef HOME_KEY_EMULATION
	/* for H/W home key emulation */
	Ecore_X_Atom atom_hwkey;
	Ecore_X_Window win_hwkey;
#endif /* HOME_KEY_EMULATION */

	int w;
	int h;
	double xscale;
	double yscale;
	double scale;
	int angle;

	Eina_Bool lock;
	Eina_Bool menu;
	Eina_Bool quickpanel;

	int notifd;
	Eina_List *evt_handlers;

	enum indicator_opacity_mode opacity_mode;

	enum _win_type top_win;
	struct {
		Eina_Bool trigger;
		int x;
		int y;
	} mouse_event;

	void (*update_display) (int);
};

#endif				/* __DEF_indicator_H__ */
