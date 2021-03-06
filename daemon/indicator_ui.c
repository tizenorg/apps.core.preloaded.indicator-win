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

#include <stdio.h>
#include <app.h>
#include <Ecore_X.h>
#include <vconf.h>
#include <heynoti.h>
#include <unistd.h>
#include <privilege-control.h>
#include <app_manager.h>
#include <signal.h>


#include "common.h"
#include "indicator_box_util.h"
#include "indicator_icon_util.h"
#include "indicator_ui.h"
#include "indicator_gui.h"
#include "modules.h"
#include "message.h"

#define GRP_MAIN "indicator"

#define WIN_TITLE "Illume Indicator"

#define VCONF_PHONE_STATUS "memory/startapps/sequence"

#define HIBERNATION_ENTER_NOTI	"HIBERNATION_ENTER"
#define HIBERNATION_LEAVE_NOTI	"HIBERNATION_LEAVE"

#define UNLOCK_ENABLED	0
#define TIMEOUT			5

#ifdef HOME_KEY_EMULATION
//#include <utilX.h>

/* predefine string */
#define PROP_HWKEY_EMULATION "_HWKEY_EMULATION"
#define KEY_MSG_PREFIX_PRESS "P:"
#define KEY_MSG_PREFIX_RELEASE "R:"
#define KEY_MSG_PREFIX_PRESS_C "PC"
#define KEY_MSG_PREFIX_RELEASE_C "RC"

#ifndef KEY_HOME
#define KEY_HOME "XF86Phone"
#endif /* KEY_HOME */
#endif /* HOME_KEY_EMULATION */

static Eina_Bool home_button_pressed = EINA_FALSE;

/******************************************************************************
 *
 * DEFINITAIONS: Static functions
 *
 *****************************************************************************/

/* util functions */
static void _change_home_padding(void *data, int angle);
static void _change_nonfixed_icon_padding(void *data, Eina_Bool status);
static void _change_max_nonfixed_icon_count(void *data,
					Eina_Bool status, int angle);
static Eina_Bool _change_view(Ecore_X_Window win, void *data);
static int check_system_status(void);

static int indicator_window_new(void *data);
static int indicator_window_del(void *data);

/* static callback functions */
static void _indicator_check_battery_percent_on_cb(keynode_t *node, void *data);
static void _indicator_low_bat_cb(void *data);
static void _indicator_lang_changed_cb(void *data);
static void _indicator_region_changed_cb(void *data);
static void _indicator_hibernation_enter_cb(void *data);
static void _indicator_hibernation_leave_cb(void *data);
static void _indicator_window_delete_cb(void *data, Evas_Object * obj,
					void *event);
static Eina_Bool _indicator_client_message_cb(void *data, int type,
					      void *event);
static void _indicator_mouse_down_cb(void *data, Evas * e, Evas_Object * obj,
				     void *event);
static void _indicator_mouse_move_cb(void *data, Evas * e, Evas_Object * obj,
				     void *event);
static void _indicator_mouse_up_cb(void *data, Evas * e, Evas_Object * obj,
				   void *event);

/******************************************************************************
 *
 * util functions
 *
 *****************************************************************************/

static void _change_home_padding(void *data, int angle)
{
	struct appdata *ad = (struct appdata *)data;
	Evas_Object *edje;

	retif(data == NULL, , "Invalid parameter!");
	edje = elm_layout_edje_get(ad->layout_main);

	switch (angle) {
	case 0:
		edje_object_signal_emit(edje,
			"change,home,pad,2", "elm.rect.*");
		break;
	case 90:
		edje_object_signal_emit(edje,
			"change,home,pad,1", "elm.rect.*");
		break;
	case 180:
		edje_object_signal_emit(edje,
			"change,home,pad,2", "elm.rect.*");
		break;
	case 270:
		edje_object_signal_emit(edje,
			"change,home,pad,1", "elm.rect.*");
		break;
	default:
		edje_object_signal_emit(edje,
			"change,home,pad,2", "elm.rect.*");
		break;
	}
}

static void _change_nonfixed_icon_padding(void *data, Eina_Bool status)
{
	struct appdata *ad = (struct appdata *)data;
	Evas_Object *edje;

	retif(data == NULL, , "Invalid parameter!");

	edje = elm_layout_edje_get(ad->layout_main);

	if (status == EINA_TRUE)
		edje_object_signal_emit(edje, "change,padding,1", "elm.rect.*");
	else
		edje_object_signal_emit(edje, "change,padding,2", "elm.rect.*");
}


static void _change_max_nonfixed_icon_count(void *data,
					Eina_Bool status, int angle)
{
	struct appdata *ad = (struct appdata *)data;
	Icon_Display_Count added_count = 0;
	retif(data == NULL, , "Invalid parameter!");

	if (status == EINA_TRUE)
		added_count = BATTERY_TEXT_ON_COUNT;
	else
		added_count = BATTERY_TEXT_OFF_COUNT;

	DBG("Current angle : %d", ad->angle);

	/* Set Displayable icon count in nonfixed_area by angle */
	switch (angle) {
	case 0:
		indicator_set_count_in_non_fixed_list(
			PORT_NONFIXED_ICON_COUNT+added_count);
		break;
	case 90:
		indicator_set_count_in_non_fixed_list(
			LAND_NONFIXED_ICON_COUNT+added_count);
		break;
	case 180:
		indicator_set_count_in_non_fixed_list(
			PORT_NONFIXED_ICON_COUNT+added_count);
		break;
	case 270:
		indicator_set_count_in_non_fixed_list(
			LAND_NONFIXED_ICON_COUNT+added_count);
		break;
	}
}

static int check_system_status(void)
{
	int ret, value = -1;

	ret = vconf_get_int(VCONF_PHONE_STATUS, &value);
	retif(ret == FAIL || value == 0, FAIL, "Booting is not completed yet!");

	ret = vconf_get_int(VCONFKEY_PWLOCK_STATE, &value);
	if (ret == OK &&
	    (value == VCONFKEY_PWLOCK_BOOTING_LOCK ||
	     value == VCONFKEY_PWLOCK_RUNNING_LOCK))
		return FAIL;

	return OK;
}

static void _change_top_win(enum _win_type type, void *data)
{
	struct appdata *ad = data;
	retif(data == NULL, , "Invalid parameter!");

	DBG("Current Top Window : %d", type);
	ad->top_win = type;
	indicator_util_update_display(data);
}

static char *_get_top_window_name(void *data)
{
	Ecore_X_Window topwin = ecore_x_window_root_first_get();
	Ecore_X_Window active;
	static Eina_Strbuf *temp_buf = NULL;

	char *pkgname = NULL;
	char *win_name = NULL;
	char *ret_name = NULL;

	int pid;

	retif(data == NULL, NULL, "Invalid parameter!");

	/* get xid for top win */
	if (ecore_x_window_prop_xid_get(topwin, ECORE_X_ATOM_NET_ACTIVE_WINDOW,
					ECORE_X_ATOM_WINDOW, &active,
					1) == FAIL)
		return NULL;

	/* get pid with xid */
	if (ecore_x_netwm_pid_get(active, &pid) == EINA_FALSE) {
		Ecore_X_Atom atom;
		unsigned char *in_pid;
		int num;

		atom = ecore_x_atom_get("X_CLIENT_PID");
		if (ecore_x_window_prop_property_get(topwin,
				atom, ECORE_X_ATOM_CARDINAL,
				sizeof(int), &in_pid, &num) == EINA_FALSE) {
			DBG("Failed to get PID from a window 0x%X", topwin);
			return NULL;
		}
		pid = *(int *)in_pid;
		free(in_pid);
	}

	DBG("Window (0x%X) PID is %d", topwin, pid);

	if (app_manager_get_package(pid,&pkgname) != APP_MANAGER_ERROR_NONE)
	{
		/* if failed to get pkgname from menu DB,
		 * check that it is quickpanel window or not */
		if (ecore_x_netwm_name_get(active, &win_name) == EINA_FALSE)
			return NULL;
		else
			return win_name;
	}

	DBG("Pkgname : %s", pkgname);

	temp_buf = eina_strbuf_new();
	eina_strbuf_append_printf(temp_buf, "%s", pkgname);
	ret_name = eina_strbuf_string_steal(temp_buf);
	eina_strbuf_free(temp_buf);

	if(pkgname != NULL)
		free(pkgname);

	return ret_name;
}

static Eina_Bool _change_view(Ecore_X_Window win, void *data)
{
	char *top_win_name = NULL;
	enum _win_type type;

	if (data == NULL)
		return EINA_FALSE;

	/* Get Top Window Package name */
	top_win_name = _get_top_window_name(data);

	if (top_win_name != NULL) {
		Eina_Bool ret = EINA_TRUE;

		INFO("TOP WINDOW NAME = %s", top_win_name);

		if (!strncmp(top_win_name, QUICKPANEL_NAME,
					strlen(top_win_name))) {
			type = TOP_WIN_QUICKPANEL;
			ret = EINA_FALSE;
		} else if (!strncmp(top_win_name, HOME_SCREEN_NAME,
						strlen(top_win_name)))
			type = TOP_WIN_HOME_SCREEN;
		else if (!strncmp(top_win_name, LOCK_SCREEN_NAME,
						strlen(top_win_name)))
			type = TOP_WIN_LOCK_SCREEN;
		else if (!strncmp(top_win_name, MENU_SCREEN_NAME,
						strlen(top_win_name)))
			type = TOP_WIN_MENU_SCREEN;
		else if (!strncmp(top_win_name, CALL_NAME,
						strlen(top_win_name)))
			type = TOP_WIN_CALL;
		else if (!strncmp(top_win_name, VTCALL_NAME,
						strlen(top_win_name)))
			type = TOP_WIN_CALL;
		else
			type = TOP_WIN_NORMAL;

		free(top_win_name);
		_change_top_win(type, data);

		return ret;
	} else {
		type = TOP_WIN_NORMAL;
	}
	_change_top_win(type, data);
	return EINA_TRUE;
}

/******************************************************************************
 *
 * callback functions
 *
 *****************************************************************************/
static void _indicator_check_battery_percent_on_cb(keynode_t *node, void *data)
{
	struct appdata *ad = (struct appdata *)data;
	int ret = FAIL;
	int status = 0;
	retif(data == NULL, , "Invalid parameter!");

	ret = vconf_get_bool(VCONFKEY_SETAPPL_BATTERY_PERCENTAGE_BOOL, &status);

	if (ret == OK) {
		_change_nonfixed_icon_padding(data, status);
		_change_max_nonfixed_icon_count(data, status, ad->angle);
	} else
		ERR("Fail to get vconfkey : %s",
			VCONFKEY_SETAPPL_BATTERY_PERCENTAGE_BOOL);
}

static void _indicator_low_bat_cb(void *data)
{
	INFO("LOW_BATTERY!");
}

static void _indicator_lang_changed_cb(void *data)
{
	INFO("CHANGE LANGUAGE!");
	indicator_lang_changed_modules(data);
}

static void _indicator_region_changed_cb(void *data)
{
	INFO("CHANGE REGION!");
	indicator_region_changed_modules(data);
}

static void _indicator_hibernation_enter_cb(void *data)
{
	indicator_hib_enter_modules(data);
}

static void _indicator_hibernation_leave_cb(void *data)
{
	indicator_hib_leave_modules(data);
}

static void _indicator_window_delete_cb(void *data, Evas_Object * obj,
					void *event)
{
	struct appdata *ad = (struct appdata *)data;
	retif(data == NULL, , "Invalid parameter!");

	indicator_window_del(ad);
}

static void _rotate_window(void *data, int new_angle)
{
	struct appdata *ad = (struct appdata *)data;
	Ecore_X_Window xwin, root;
	int ret = FAIL;
	int status = 0;
	int root_w, root_h;

	retif(data == NULL, , "Invalid parameter!");

	if (new_angle == ad->angle)
		return;

	INFO("angle = %d", new_angle);
	ad->angle = new_angle;
	xwin = elm_win_xwindow_get(ad->win_main);

	if (!xwin)
		return;
	root = ecore_x_window_root_get(xwin);
	if (!root)
		return;
	ecore_x_window_size_get(root, &root_w, &root_h);
	if (!root_w || !root_h)
		return;

	/* rotate window */
	evas_norender(evas_object_evas_get(ad->win_main));
	elm_win_rotation_set(ad->win_main, ad->angle);

	switch (ad->angle) {
	case 0:
		evas_object_move(ad->win_main, 0, 0);
		evas_object_resize(ad->win_main, root_w, ad->h);
		break;
	case 90:
		evas_object_move(ad->win_main, 0, 0);
		evas_object_resize(ad->win_main, root_h, ad->h);
		break;
	case 180:
		evas_object_move(ad->win_main, 0, root_h - ad->h);
		evas_object_resize(ad->win_main, root_w, ad->h);
		break;
	case 270:
		evas_object_move(ad->win_main, root_w - ad->h, 0);
		evas_object_resize(ad->win_main, root_h, ad->h);
		break;
	default:
		break;
	}

	ret = vconf_get_bool(VCONFKEY_SETAPPL_BATTERY_PERCENTAGE_BOOL, &status);

	if (ret == OK) {
		_change_home_padding(data, ad->angle);
		_change_nonfixed_icon_padding(data, status);
		_change_max_nonfixed_icon_count(data, status, ad->angle);
	} else
		ERR("Fail to get vconfkey : %s",
			VCONFKEY_SETAPPL_BATTERY_PERCENTAGE_BOOL);

	/* change to clock view */
	indicator_util_update_display(data);
	evas_render(evas_object_evas_get(ad->win_main));
}

#ifdef INDICATOR_SUPPORT_OPACITY_MODE
static void _change_opacity(void *data, enum indicator_opacity_mode mode)
{
	struct appdata *ad = NULL;
	const char *signal = NULL;
	retif(data == NULL, , "Invalid parameter!");

	ad = data;

	retif(ad->layout_main == NULL, , "layout_main is NULL");

	switch (mode) {
	case INDICATOR_OPACITY_OPAQUE:
		signal = "bg.opaque";
		ad->opacity_mode = mode;
		break;
	case INDICATOR_OPACITY_TRANSLUCENT:
		signal = "bg.translucent";
		ad->opacity_mode = mode;
		break;
	case INDICATOR_OPACITY_TRANSPARENT:
		signal = "bg.transparent";
		ad->opacity_mode = mode;
		break;
	default:
		ERR("unknown mode : %d", mode);
		signal = "bg.opaque";
		ad->opacity_mode = INDICATOR_OPACITY_OPAQUE;
		break;

	}
	ad->opacity_mode = mode;
	edje_object_signal_emit(elm_layout_edje_get(ad->layout_main),
			signal, "indicator.prog");

	DBG("send signal [%s] to indicator layout", signal);
}

static void _notification_panel_changed(void *data, int is_open)
{
	struct appdata *ad = NULL;
	retif(data == NULL, , "Invalid parameter!");

	ad = data;

	retif(ad->layout_main == NULL, , "layout_main is NULL");
	if (is_open) {
		edje_object_signal_emit(elm_layout_edje_get(ad->layout_main),
				"bg.notification", "indicator.prog");

		DBG("send signal [%s] to indicator layout", "bg.notification");
	}
	else
		_change_opacity(data, ad->opacity_mode);
}

#endif /* INDICATOR_SUPPORT_OPACITY_MODE */

static Eina_Bool _indicator_client_message_cb(void *data, int type, void *event)
{
	Ecore_X_Event_Client_Message *ev =
	    (Ecore_X_Event_Client_Message *) event;
	int new_angle;

	retif(data == NULL
	      || event == NULL, ECORE_CALLBACK_RENEW, "Invalid parameter!");

#ifdef INDICATOR_SUPPORT_OPACITY_MODE
	if (ev->message_type == ECORE_X_ATOM_E_ILLUME_INDICATOR_OPACITY_MODE) {
		int trans_mode;

		if (ev->data.l[0]
			== ECORE_X_ATOM_E_ILLUME_INDICATOR_TRANSLUCENT)
			trans_mode = INDICATOR_OPACITY_TRANSLUCENT;
		else if (ev->data.l[0]
			== ECORE_X_ATOM_E_ILLUME_INDICATOR_TRANSPARENT)
			trans_mode = INDICATOR_OPACITY_TRANSPARENT;
		else
			trans_mode = INDICATOR_OPACITY_OPAQUE;

		_change_opacity(data, trans_mode);
	}
#endif /* INDICATOR_SUPPORT_OPACITY_MODE */

	if (ev->message_type == ECORE_X_ATOM_E_ILLUME_QUICKPANEL_STATE) {
#ifdef INDICATOR_SUPPORT_OPACITY_MODE
		if (ev->data.l[0] == ECORE_X_ATOM_E_ILLUME_QUICKPANEL_ON)
			_notification_panel_changed(data, 1);
		else if (ev->data.l[0] == ECORE_X_ATOM_E_ILLUME_QUICKPANEL_OFF)
			_notification_panel_changed(data, 0);

#else /* INDICATOR_SUPPORT_OPACITY_MODE */
		if (ev->data.l[0] == ECORE_X_ATOM_E_ILLUME_QUICKPANEL_OFF)
			_change_view(ecore_x_window_root_first_get(), data);
#endif /* INDICATOR_SUPPORT_OPACITY_MODE */
	}

	if (ev->message_type == ECORE_X_ATOM_E_ILLUME_ROTATE_WINDOW_ANGLE) {
		new_angle = ev->data.l[0];
		_rotate_window(data, new_angle);
	}
	return ECORE_CALLBACK_RENEW;
}


static Eina_Bool _property_changed_cb(void *data, int type, void *event)
{
	Ecore_X_Event_Window_Property *ev = event;

	if (ev == NULL || ev->atom != ECORE_X_ATOM_NET_ACTIVE_WINDOW)
		return EINA_FALSE;

	return _change_view(ev->win, data);
}

static void register_event_handler(void *data)
{
	int ret;
	struct appdata *ad = data;
	Ecore_Event_Handler *hdl = NULL;
	retif(data == NULL, , "Invalid parameter!");

	evas_object_smart_callback_add(ad->win_main,
				       "delete,request",
				       _indicator_window_delete_cb, (void *)ad);

	hdl = ecore_event_handler_add(ECORE_X_EVENT_CLIENT_MESSAGE,
				      _indicator_client_message_cb, (void *)ad);
	retif(hdl == NULL, , "Failed to register ecore_event_handler!");
	ad->evt_handlers = eina_list_append(ad->evt_handlers, hdl);

	hdl = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_PROPERTY,
				      _property_changed_cb, data);
	retif(hdl == NULL, , "Failed to register ecore_event_handler!");
	ad->evt_handlers = eina_list_append(ad->evt_handlers, hdl);

	evas_object_event_callback_add(ad->layout_main,
				       EVAS_CALLBACK_MOUSE_DOWN,
				       _indicator_mouse_down_cb, (void *)ad);

	evas_object_event_callback_add(ad->layout_main,
				       EVAS_CALLBACK_MOUSE_MOVE,
				       _indicator_mouse_move_cb, (void *)ad);

	evas_object_event_callback_add(ad->layout_main,
				       EVAS_CALLBACK_MOUSE_UP,
				       _indicator_mouse_up_cb, (void *)ad);

	ad->notifd = heynoti_init();
	if (ad->notifd == -1) {
		ERR("noti init is failed");
		return;
	}

	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_BATTERY_PERCENTAGE_BOOL,
		       _indicator_check_battery_percent_on_cb, (void *)ad);

	if (ret == -1) {
		ERR("noti start is failed\n");
		return;
	}

	heynoti_subscribe(ad->notifd, HIBERNATION_ENTER_NOTI,
			  _indicator_hibernation_enter_cb, (void *)ad);
	heynoti_subscribe(ad->notifd, HIBERNATION_LEAVE_NOTI,
			  _indicator_hibernation_leave_cb, (void *)ad);

	ret = heynoti_attach_handler(ad->notifd);
	if (ret == -1) {
		ERR("noti start is failed\n");
		return;
	}
}

static int unregister_event_handler(void *data)
{
	struct appdata *ad = (struct appdata *)data;

	retif(data == NULL, FAIL, "Invalid parameter!");

	evas_object_smart_callback_del(ad->win_main,
				       "delete-request",
				       _indicator_window_delete_cb);

	evas_object_event_callback_del(ad->layout_main,
				       EVAS_CALLBACK_MOUSE_DOWN,
				       _indicator_mouse_down_cb);

	evas_object_event_callback_del(ad->layout_main,
				       EVAS_CALLBACK_MOUSE_MOVE,
				       _indicator_mouse_move_cb);

	evas_object_event_callback_del(ad->layout_main,
				       EVAS_CALLBACK_MOUSE_UP,
				       _indicator_mouse_up_cb);

	vconf_ignore_key_changed(VCONFKEY_SETAPPL_BATTERY_PERCENTAGE_BOOL,
				_indicator_check_battery_percent_on_cb);


	heynoti_unsubscribe(ad->notifd, HIBERNATION_ENTER_NOTI,
			    _indicator_hibernation_enter_cb);
	heynoti_unsubscribe(ad->notifd, HIBERNATION_LEAVE_NOTI,
			    _indicator_hibernation_leave_cb);

	heynoti_close(ad->notifd);
	ad->notifd = 0;

	Ecore_Event_Handler *hdl = NULL;
	EINA_LIST_FREE(ad->evt_handlers, hdl) {
		if (hdl)
			ecore_event_handler_del(hdl);
	}
	return OK;
}

/******************************************************************************
 *
 * UI
 *
 *****************************************************************************/

static Evas_Object *create_win(void)
{
	Evas_Object *eo;

	eo = elm_win_add(NULL, PACKAGE, ELM_WIN_DOCK);
	if (eo) {
		elm_win_alpha_set(eo, EINA_TRUE);
		elm_win_title_set(eo, WIN_TITLE);
		elm_win_borderless_set(eo, EINA_TRUE);
	}

	return eo;
}

static Evas_Object *load_edj(Evas_Object * parent, const char *file,
			     const char *group)
{
	Evas_Object *eo;
	int r;

	eo = elm_layout_add(parent);
	if (eo) {
		r = elm_layout_file_set(eo, file, group);
		if (!r) {
			evas_object_del(eo);
			return NULL;
		}

		evas_object_size_hint_weight_set(eo,
						 EVAS_HINT_EXPAND,
						 EVAS_HINT_EXPAND);
		elm_win_resize_object_add(parent, eo);
	}

	return eo;
}

static int indicator_window_new(void *data)
{
	struct appdata *ad = data;
	Ecore_Evas *ee = NULL;
	Ecore_X_Window xwin;
	Ecore_X_Window zone;
	Ecore_X_Window_State states[2];
	int root_w;
	int root_h;
	Ecore_X_Window root;

	retif(data == NULL, FAIL, "Invalid parameter!");

	/* create indicator window */
	ad->win_main = create_win();
	retif(ad->win_main == NULL, FAIL, "Cannot add window main!");
	evas_object_size_hint_fill_set(ad->win_main, EVAS_HINT_EXPAND,
				       EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(ad->win_main, 1.0, 0.5);

	/*******************************************************************
	 * make illume2 recognize indicator window
	 */

	xwin = elm_win_xwindow_get(ad->win_main);
	ecore_x_icccm_hints_set(xwin, 0, 0, 0, 0, 0, 0, 0);
	states[0] = ECORE_X_WINDOW_STATE_SKIP_TASKBAR;
	states[1] = ECORE_X_WINDOW_STATE_SKIP_PAGER;
	ecore_x_netwm_window_state_set(xwin, states, 2);

	ecore_x_icccm_name_class_set(xwin, "INDICATOR", "INDICATOR");

	zone = ecore_x_e_illume_zone_get(xwin);
	ecore_x_event_mask_set(zone, ECORE_X_EVENT_MASK_WINDOW_CONFIGURE);

	/*******************************************************************
	 * Get a evas object of main window
	 */
	ad->evas = evas_object_evas_get(ad->win_main);
	ee = ecore_evas_ecore_evas_get(ad->evas);

	/*******************************************************************
	 * Create a edje layout to display and add it to the main window
	 */
	ad->layout_main = load_edj(ad->win_main, EDJ_FILE, GRP_MAIN);
	retif(ad->layout_main == NULL, FAIL, "Failed to get layout main!");
	/*******************************************************************
	 * resize window
	 */
	root = ecore_x_window_root_first_get();
	ecore_x_window_size_get(root, &root_w, &root_h);
	INFO("xwin_size = %d %d", root_w, root_h);

	ad->scale = elm_config_scale_get();
	INFO("scale = %f", ad->scale);

	ad->w = root_w;
	ad->h = (int)(INDICATOR_HEIGHT * ad->scale);
	evas_object_resize(ad->win_main, ad->w, ad->h);
	evas_object_move(ad->win_main, 0, 0);
	INFO("win_size = %d, %d", ad->w, ad->h);

#ifdef HOME_KEY_EMULATION
	int ret = 0;
	ad->atom_hwkey = ecore_x_atom_get(PROP_HWKEY_EMULATION);
	ret = ecore_x_window_prop_window_get(root, ad->atom_hwkey,
					&ad->win_hwkey, 1);
	if (ret <= 0)
		ERR("Failed to get window property ! (ret=%d)", ret);
#endif /* HOME_KEY_EMULATION */

	/*******************************************************************
	 * register callback functions
	 */

	indicator_util_layout_add(ad);
	register_event_handler(ad);
	evas_object_show(ad->layout_main);
	evas_object_show(ad->win_main);

	return OK;
}

static int indicator_window_del(void *data)
{
	struct appdata *ad = (struct appdata *)data;

	retif(data == NULL, FAIL, "Invalid parameter!");

	indicator_fini_modules(data);
	indicator_message_fini();
	unregister_event_handler(ad);
	indicator_util_layout_del(ad);
	evas_image_cache_flush(ad->evas);

	if (ad->layout_main) {
		evas_object_del(ad->layout_main);
		ad->layout_main = NULL;
	}

	if (ad->win_main) {
		evas_object_del(ad->win_main);
		ad->win_main = NULL;
	}

	if (ad)
		free(ad);

	elm_exit();
	return OK;
}

/******************************************************************************
 *
 * Mouse event
 *
 *****************************************************************************/
static inline int _indicator_home_icon_action(void *data, int press)
{
	struct appdata *ad = NULL;
	int ret = -1;
	const char *signal = NULL;

	retif(!data, ret, "data is NULL");
	ad = data;
	retif(!ad->layout_main, ret, "ad->layout_main is NULL");

	if (press)
		signal = "home.pressed";
	else
		signal = "home.released";

	ret = vconf_set_int(VCONF_INDICATOR_HOME_PRESSED, !(!press));
	if (!ret)
		elm_object_signal_emit(ad->layout_main,
				signal, "indicator.prog");

	return ret;
}

#ifdef HOME_KEY_EMULATION
static Eina_Bool _indicator_hw_home_key_press(void *data)
{
	struct appdata *ad = NULL;
	char message[20] = {'\0', };

	retif(!data, EINA_FALSE, "data is NULL");

	ad = data;

	retif(!ad->win_hwkey, EINA_FALSE, "window for hw emulation is NULL");

	snprintf(message, sizeof(message), "%s%s",
			KEY_MSG_PREFIX_PRESS, KEY_HOME);

	return ecore_x_client_message8_send(ad->win_hwkey, ad->atom_hwkey,
		message, strlen(message));
}

static Eina_Bool _indicator_hw_home_key_release(void *data)
{
	struct appdata *ad = NULL;
	char message[20] = {'\0', };

	retif(!data, EINA_FALSE, "data is NULL");

	ad = data;

	retif(!ad->win_hwkey, EINA_FALSE, "window for hw emulation is NULL");


	snprintf(message, sizeof(message), "%s%s",
			KEY_MSG_PREFIX_RELEASE, KEY_HOME);

	return ecore_x_client_message8_send(ad->win_hwkey, ad->atom_hwkey,
		message, strlen(message));
}
static Eina_Bool _indicator_hw_home_key_press_cancel(void *data)
{
	struct appdata *ad = NULL;
	char message[20] = {'\0', };

	retif(!data, EINA_FALSE, "data is NULL");

	ad = data;

	retif(!ad->win_hwkey, EINA_FALSE, "window for hw emulation is NULL");

	snprintf(message, sizeof(message), "%s%s",
			KEY_MSG_PREFIX_PRESS_C, KEY_HOME);

	return ecore_x_client_message8_send(ad->win_hwkey, ad->atom_hwkey,
		message, strlen(message));
}

static Eina_Bool _indicator_hw_home_key_release_cancel(void *data)
{
	struct appdata *ad = NULL;
	char message[20] = {'\0', };

	retif(!data, EINA_FALSE, "data is NULL");

	ad = data;

	retif(!ad->win_hwkey, EINA_FALSE, "window for hw emulation is NULL");


	snprintf(message, sizeof(message), "%s%s",
			KEY_MSG_PREFIX_RELEASE_C, KEY_HOME);

	return ecore_x_client_message8_send(ad->win_hwkey, ad->atom_hwkey,
		message, strlen(message));
}

#endif /* HOME_KEY_EMULATION */

static void _indicator_mouse_down_cb(void *data, Evas * e, Evas_Object * obj,
				     void *event)
{
	struct appdata *ad = NULL;
	Evas_Event_Mouse_Down *ev = NULL;

	retif(data == NULL || event == NULL, , "Invalid parameter!");
	ad = data;
	ev = event;

	ad->mouse_event.x = ev->canvas.x;
	ad->mouse_event.y = ev->canvas.y;

#ifdef HOME_KEY_EMULATION
	if (!indicator_util_check_home_icon_area(ev->canvas.x, ev->canvas.y)) {
		_indicator_home_icon_action(data, 1);
		home_button_pressed = EINA_TRUE;
		_indicator_hw_home_key_press(data);
	}

#else /* HOME_KEY_EMULATION */
	int mouse_down_prio = -1;
	mouse_down_prio =
		indicator_util_get_priority_in_move_area(ad->mouse_event.x,
							ad->mouse_event.y);

	if (mouse_down_prio > -1) {
		switch (mouse_down_prio) {
		case INDICATOR_PRIORITY_FIXED5:
		{
			int lock_state = VCONFKEY_IDLE_UNLOCK;
			int lock_ret = -1;

			lock_ret = vconf_get_int(VCONFKEY_IDLE_LOCK_STATE,
					&lock_state);
			DBG("Check Lock State : %d %d", lock_ret, lock_state);

			if (lock_ret == 0
				&& lock_state == VCONFKEY_IDLE_UNLOCK) {
				if (!_indicator_home_icon_action(data, 1))
					home_button_pressed = EINA_TRUE;
			}
		}
		break;
		}
	}
#endif /* HOME_KEY_EMULATION */
}

static void _indicator_mouse_move_cb(void *data, Evas * e, Evas_Object * obj,
				     void *event)
{
	struct appdata *ad = NULL;
	Evas_Event_Mouse_Move *ev = NULL;

	retif(data == NULL || event == NULL, , "Invalid parameter!");
	ad = data;
	ev = event;

#ifdef HOME_KEY_EMULATION
	if (home_button_pressed) {
		if (indicator_util_check_home_icon_area(ev->cur.canvas.x,
			ev->cur.canvas.y)) {
			_indicator_home_icon_action(data, 0);
			home_button_pressed = EINA_FALSE;
			_indicator_hw_home_key_press_cancel(data);
			DBG("cancel home key");
		}
	}
#endif /* HOME_KEY_EMULATION */
}

static void _indicator_mouse_up_cb(void *data, Evas * e, Evas_Object * obj,
				   void *event)
{
	struct appdata *ad = NULL;
	Evas_Event_Mouse_Up *ev = NULL;

	retif(data == NULL || event == NULL, , "Invalid parameter!");
	ad = data;
	ev = event;

#ifdef HOME_KEY_EMULATION
	if (home_button_pressed) {
		_indicator_hw_home_key_release(data);
		_indicator_home_icon_action(data, 0);
	}

	home_button_pressed = EINA_FALSE;
#else /* HOME_REMOVE_LONGPRESS */
	int mouse_up_prio = -1;
	int mouse_down_prio = -1;

	if (home_button_pressed == EINA_TRUE) {
		_indicator_home_icon_action(data, 0);
		home_button_pressed = EINA_FALSE;
	}

	mouse_down_prio =
		indicator_util_get_priority_in_move_area(ad->mouse_event.x,
							ad->mouse_event.y);
	mouse_up_prio = indicator_util_get_priority_in_move_area(ev->canvas.x,
							ev->canvas.y);

	if (mouse_down_prio > -1 && mouse_up_prio > -1
		&& mouse_down_prio == mouse_up_prio) {
		switch (mouse_down_prio) {
		case INDICATOR_PRIORITY_FIXED5:
		{
			int lock_state = VCONFKEY_IDLE_UNLOCK;
			int lock_ret = -1;

			lock_ret = vconf_get_int(VCONFKEY_IDLE_LOCK_STATE,
					&lock_state);
			DBG("Check Lock State : %d %d", lock_ret, lock_state);

			/* In Lock Screen, home button don't have to do */
			if (lock_ret == 0 && lock_state == VCONFKEY_IDLE_LOCK)
				break;

			char *package = NULL;
			char *top_win_name = NULL;

			INFO("[Home Button Released]");

			if (check_system_status() == FAIL)
				break;

			package = vconf_get_str("db/menuscreen/pkgname");
			if (package) {
				/* Check top Window.
				 * If top window sames to string of
				 * package value,
				 * Call aul_launch_app */
				service_h service;
				int ret = SERVICE_ERROR_NONE;

				service_create(&service);

				service_set_operation(service, SERVICE_OPERATION_DEFAULT);

				service_set_package(service, package);

				top_win_name = _get_top_window_name(data);

				if (top_win_name != NULL
					&& !strncmp(top_win_name, package,
					strlen(package)))
				{

					DBG("service_send_launch_request : %s",
						top_win_name);

					// explicit launch without reply callback
					ret = service_send_launch_request(service, NULL, NULL);

					if(ret != SERVICE_ERROR_NONE)
					{
						ERR("Cannot launch app");
					}

				}
				else
				{
					DBG("app_manager_resume_app : %s",
						top_win_name);

					ret = app_manager_resume_app(service);
					if(ret != APP_MANAGER_ERROR_NONE)
					{
						ERR("Cannot resume app");
					}
				}

				if (top_win_name)
					free(top_win_name);

				free(package);

				service_destroy(service);

			} else
				ERR("Cannot get vconf");
		}
		break;
		}
	}
#endif /* HOME_KEY_EMULATION */
	ad->mouse_event.x = 0;
	ad->mouse_event.y = 0;
}

/******************************************************************************
 *
 * Modules
 *
 *****************************************************************************/
static int register_indicator_modules(void *data)
{
	indicator_init_modules(data);
	return OK;
}

static void _signal_handler(int signum, siginfo_t *info, void *unused)
{
    DBG("_signal_handler : Terminated...");
    app_efl_exit();
}
static void _heynoti_event_power_off(void *data)
{
    DBG("_heynoti_event_power_off : Terminated...");
    app_efl_exit();
}


/******************************************************************************
 *
 * IMPLEMENTATIONS: appcore_ops functions
 *
 *****************************************************************************/
static bool app_create(void *data)
{
	pid_t pid;
	int ret;

    // signal handler
	struct sigaction act;
	act.sa_sigaction = _signal_handler;
	act.sa_flags = SA_SIGINFO;

	ret = sigemptyset(&act.sa_mask);
	if (ret < 0) {
		ERR("Failed to sigemptyset[%s]", strerror(errno));
	}
	ret = sigaddset(&act.sa_mask, SIGTERM);
	if (ret < 0) {
		ERR("Failed to sigaddset[%s]", strerror(errno));
	}
	ret = sigaction(SIGTERM, &act, NULL);
	if (ret < 0) {
		ERR("Failed to sigaction[%s]", strerror(errno));
	}

	pid = setsid();
	if (pid < 0)
		fprintf(stderr, "[INDICATOR] Failed to set session id!");

	ret = control_privilege();
	if (ret != 0) {
		fprintf(stderr, "[INDICATOR] Failed to control privilege!");
		return false;
	}

	ret = nice(2);
	if (ret == -1)
		ERR("Failed to set nice value!");
	return true;
}

static void app_terminate(void *data)
{
	struct appdata *ad = data;

	indicator_fini_modules(data);
	indicator_message_fini();
	unregister_event_handler(ad);
	indicator_util_layout_del(ad);
	evas_image_cache_flush(ad->evas);

	if (ad->layout_main)
		evas_object_del(ad->layout_main);

	if (ad->win_main)
		evas_object_del(ad->win_main);

	INFO(" >>>>>>>>>>>>> INDICATOR IS TERMINATED!! <<<<<<<<<<<<<< ");

}

static void app_pause(void *data)
{

}

static void app_resume(void *data)
{

}

static void app_service(service_h service, void *data)
{
	struct appdata *ad = data;
	int ret;

	INFO("[INDICATOR IS STARTED]");
	ret = indicator_window_new(data);
	retif(ret != OK, FAIL, "Failed to create a new window!");

	/* change view */
	_change_view(ecore_x_window_root_first_get(), data);

	/* Set nonfixed-list size for display */
	_indicator_check_battery_percent_on_cb(NULL, data);
	register_indicator_modules(data);

	indicator_message_init(data);

	if (ad->win_main)
		elm_win_activate(ad->win_main);

}

int main(int argc, char *argv[])
{

	struct appdata ad;

	app_event_callback_s event_callback;

    int heyfd = heynoti_init();
	if (heyfd < 0) {
		ERR("Failed to heynoti_init[%d]", heyfd);
	}

	int ret = heynoti_subscribe(heyfd, "power_off_start", _heynoti_event_power_off, NULL);
	if (ret < 0) {
		ERR("Failed to heynoti_subscribe[%d]", ret);
	}
	ret = heynoti_attach_handler(heyfd);
	if (ret < 0) {
		ERR("Failed to heynoti_attach_handler[%d]", ret);
	}

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.service = app_service;
	event_callback.low_memory = NULL;
	event_callback.low_battery = _indicator_low_bat_cb;
	event_callback.device_orientation = NULL;
	event_callback.language_changed = _indicator_lang_changed_cb;
	event_callback.region_format_changed = _indicator_region_changed_cb;

	memset(&ad, 0x0, sizeof(struct appdata));

	return app_efl_main(&argc, &argv, &event_callback, &ad);

}
