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
#include <notification.h>
#include "common.h"
#include "indicator.h"
#include "indicator_icon_util.h"
#include "indicator_ui.h"
#include "indicator_icon_list.h"

#define ICON_PRIORITY	INDICATOR_PRIORITY_NON_FIXED_10
#define MODULE_NAME		"notification"

static int register_noti_module(void *data);
static int unregister_noti_module(void);
static int hib_enter_noti_module(void);
static int hib_leave_noti_module(void *data);

Indicator_Icon_Object noti = {
	.type = INDICATOR_IMG_ICON,
	.name = MODULE_NAME,
	.priority = ICON_PRIORITY,
	.always_top = EINA_FALSE,
	.txt_obj = {0,},
	.img_obj = {0,},
	.obj_exist = EINA_FALSE,
	.fixed = EINA_FALSE,
	.exist_in_view = EINA_FALSE,
	.init = register_noti_module,
	.fini = unregister_noti_module,
	.hib_enter = hib_enter_noti_module,
	.hib_leave = hib_leave_noti_module
};

struct noti_status {
	notification_h noti;
	int type;
	int cnt;
	Indicator_Icon_Object *icon;
};


static Eina_List *status_list;
static int noti_cnt;

static void show_image_icon(struct noti_status *data)
{
	retif(data == NULL, , "Invalid parameter!");

		char *icon_path;
		notification_h noti = NULL;

		if (data->noti) {
			noti = data->noti;
			if (noti) {
				notification_get_icon(noti, &icon_path);
				DBG("Get Path of Notication %s : %s",
						data->icon->name, icon_path);
				if (icon_path == NULL
					|| !ecore_file_exists(icon_path)) {
					/* This Noti Event is invalid Item.
					* This Item will be not displayed.
					*/
					data->icon->img_obj.data = NULL;
				} else {
					data->icon->img_obj.data =
							strdup(icon_path);
					indicator_util_icon_show(data->icon);
				}
			}
		}
	/* TODO: move event rect */
}

static void hide_image_icon(struct noti_status *data)
{
	retif(data == NULL, , "Invalid parameter!");
	indicator_util_icon_hide(data->icon);
}

static Indicator_Icon_Object *_icon_add(const char *name, void *data)
{
	Indicator_Icon_Object *obj = NULL;
	retif(data == NULL, NULL, "Invalid parameter!");
	obj = calloc(1, sizeof(Indicator_Icon_Object));

	if (obj) {
		memset(obj, 0, sizeof(Indicator_Icon_Object));

		obj->type = INDICATOR_IMG_ICON;
		obj->name = strdup(name);
		obj->priority = ICON_PRIORITY;
		obj->always_top = EINA_FALSE;
		obj->ad = data;
		obj->fixed = EINA_FALSE;
		obj->exist_in_view = EINA_FALSE;
	}
	return obj;
}

static void _set_noti_cnt(int cnt)
{
	if (cnt >= 0) {
		noti_cnt = cnt;
		indicator_util_event_count_set(noti_cnt, noti.ad);
	}
}

static int _get_noti_cnt(void)
{
	return noti_cnt;
}

static void _update_all_status(void)
{
	Eina_List *l;
	struct noti_status *data;
	int total = 0, noti_count = 0;

	EINA_LIST_FOREACH(status_list, l, data) {
		if (data) {
			DBG("%s is updated! Cnt : %d",
				data->icon->name, data->cnt);
			notification_get_count(NOTIFICATION_TYPE_NONE, NULL,
				NOTIFICATION_GROUP_ID_NONE,
				NOTIFICATION_PRIV_ID_NONE,
				&noti_count);
			data->cnt = noti_count;

			if (data->cnt == 0)
				hide_image_icon(data);
			else
				total += data->cnt;
		}
	}
}

static void _change_icon_status(void *data, notification_list_h noti_list)
{
	int cnt = 0, new_cnt = 0;
	Eina_List *l = NULL;
	notification_h noti = NULL;
	struct noti_status *n_data = NULL;
	int noti_count = 0, ongoing_count = 0;

	notification_get_count(NOTIFICATION_TYPE_NOTI, NULL,
		NOTIFICATION_GROUP_ID_NONE, NOTIFICATION_PRIV_ID_NONE,
		&noti_count);
	notification_get_count(NOTIFICATION_TYPE_ONGOING, NULL,
		NOTIFICATION_GROUP_ID_NONE, NOTIFICATION_PRIV_ID_NONE,
		&ongoing_count);

	new_cnt = noti_count + ongoing_count;

	retif(noti_list == NULL, , "Invalid parameter!");

	/* Clear List and objects in list */
	EINA_LIST_FOREACH(status_list, l, n_data) {
		DBG("Clear Status List : %s", n_data->icon->name);
		hide_image_icon(n_data);

		if (n_data->icon->img_obj.data)
			free((char *)n_data->icon->img_obj.data);
		if (n_data->icon)
			free(n_data->icon);
		status_list = eina_list_remove_list(status_list, l);
	}
	eina_list_free(status_list);

	while (noti_list != NULL) {
		char *pkgname = NULL;
		Eina_List *l = NULL;
		struct noti_status *n_data = NULL;
		struct noti_status *status = NULL;
		Eina_Bool status_exist = EINA_FALSE;
		notification_error_e noti_ret = NOTIFICATION_ERROR_NONE;
		int applist;

		noti = notification_list_get_data(noti_list);
		noti_list = notification_list_get_next(noti_list);

		noti_ret = notification_get_display_applist(noti, &applist);
		if (noti_ret != NOTIFICATION_ERROR_NONE) {
			INFO("Cannot Get display app of notication! : %p ",
					noti);
			continue;
		}
		if (!(applist & NOTIFICATION_DISPLAY_APP_INDICATOR))
			continue;

		noti_ret = notification_get_pkgname(noti, &pkgname);

		if (noti_ret != NOTIFICATION_ERROR_NONE)
			DBG("Cannot Get pkgname of notication! : %p %p",
					noti, pkgname);
		else {
			EINA_LIST_FOREACH(status_list, l, n_data) {
				if (!strcmp(n_data->icon->name, pkgname)) {
					DBG("%s is already existed", pkgname);
					/* In this case,
					 * We need not make new noti status
					 */
					status_exist = EINA_TRUE;
					break;
				}
			}

			if (status_exist != EINA_TRUE) {
				DBG("Make New Event Icon : %s", pkgname);
				status = calloc(1, sizeof(struct noti_status));
				status->type = 0;
				status->cnt = new_cnt;
				status->icon = _icon_add(pkgname, data);
				status->noti = noti;
				indicator_icon_list_insert(status->icon);
				status_list =
					eina_list_append(status_list, status);

				show_image_icon(status);
				cnt = _get_noti_cnt() + status->cnt;
			}
		}
	}
}

void update_noti_module_new(void *data, notification_type_e type)
{
	notification_list_h list = NULL;
	notification_error_e noti_err = NOTIFICATION_ERROR_NONE;
	int get_event_count = indicator_util_max_visible_event_count();

	retif(data == NULL, , "Invalid parameter!");

	/* Get ongoing + noti count */
	noti_err = notification_get_grouping_list(NOTIFICATION_TYPE_NONE,
				get_event_count, &list);

	if (noti_err != NOTIFICATION_ERROR_NONE || list == NULL) {
		_update_all_status();
		return;
	}

	_change_icon_status(data, list);
}


static int register_noti_module(void *data)
{
	retif(data == NULL, FAIL, "Invalid parameter!");
	notification_error_e ret = NOTIFICATION_ERROR_NONE;

	ret = notification_resister_changed_cb(update_noti_module_new, data);

	if (ret != NOTIFICATION_ERROR_NONE)
		DBG("Fail to Register notification_resister_changed_cb!");

	update_noti_module_new(data, NOTIFICATION_TYPE_NOTI);

	return OK;
}

static int unregister_noti_module(void)
{
	Eina_List *l;
	struct noti_status *data = NULL;
	notification_error_e ret = NOTIFICATION_ERROR_NONE;

	ret = notification_unresister_changed_cb(update_noti_module_new);

	if (ret != NOTIFICATION_ERROR_NONE)
		DBG("Fail to unregister notification_resister_changed_cb!");

	EINA_LIST_FOREACH(status_list, l, data) {
		hide_image_icon(data);

		if (data->icon->img_obj.data != NULL)
			free((char *)data->icon->img_obj.data);
		if (data->icon)
			free(data->icon);
		status_list = eina_list_remove_list(status_list, l);
	}

	eina_list_free(status_list);
	return OK;
}

static int hib_enter_noti_module(void)
{
	return OK;
}

static int hib_leave_noti_module(void *data)
{
	retif(data == NULL, FAIL, "Invalid parameter!");
	update_noti_module_new(data, NOTIFICATION_TYPE_NOTI);
	return OK;
}
