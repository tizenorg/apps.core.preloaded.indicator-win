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


#ifndef __DEF_indicator_H_
#define __DEF_indicator_H_

#include <Elementary.h>
#include <Ecore_X.h>


#if !defined(PACKAGE)
#  define PACKAGE "indicator"
#endif

#if !defined(LOCALEDIR)
#  define LOCALEDIR "/usr/apps/com.samsung.indicator/res/locale"
#endif

#if !defined(EDJDIR)
#  define EDJDIR "/usr/apps/com.samsung.indicator/res/edje"
#endif

#if !defined(ICONDIR)
#  define ICONDIR "/usr/apps/com.samsung.indicator/res/icons"
#endif

#define EDJ_FILE EDJDIR"/"PACKAGE".edj"
#define ICON_THEME_FILE EDJDIR"/"PACKAGE"_icon_theme.edj"
#define ICON_NONFIXED_THEME_FILE EDJDIR"/"PACKAGE"_icon_nonfixed_theme.edj"

#define HOME_SCREEN_NAME		"com.samsung.live-magazine"
#define MENU_SCREEN_NAME		"com.samsung.menu-screen"
#define LOCK_SCREEN_NAME		"com.samsung.idle-lock"
#define QUICKPANEL_NAME			"E Popup"
#define CALL_NAME			"com.samsung.call"
#define VTCALL_NAME			"com.samsung.vtmain"

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
