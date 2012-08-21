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


#include <Ecore_X.h>
#include <Eina.h>
#include "common.h"
#include "indicator_box_util.h"
#include "indicator_icon_util.h"
#include "indicator_icon_list.h"
#include "indicator_ui.h"
#include "indicator_gui.h"
#include <vconf.h>

#define DEFAULT_SIZE	(CLOCK_WIDTH + (PADDING_WIDTH * 2))
#define _FIXED_BOX_PART_NAME		"elm.swallow.fixed"
#define _NON_FIXED_BOX_PART_NAME	"elm.swallow.nonfixed"
#define _FIXED_COUNT	4
#define CORRECTION 10

Eina_List *_view_list;
Eina_List *_view_nonfixed_list;

static Evas_Object *_fixed_box[_FIXED_COUNT] = {0, };
static Evas_Object *_non_fixed_box;
static int indicator_non_fixed_list_max_cnt;

static Evas_Object *indicator_box_add(Evas_Object * parent)
{
	Evas_Object *obj;

	retif(parent == NULL, NULL, "Invalid parameter!");

	obj = elm_box_add(parent);
	evas_object_size_hint_weight_set(obj, EVAS_HINT_EXPAND,
					 EVAS_HINT_EXPAND);
	evas_object_size_hint_fill_set(obj, EVAS_HINT_FILL, EVAS_HINT_FILL);

	/* Align to left-top */
	elm_box_horizontal_set(obj, EINA_TRUE);
	evas_object_show(obj);

	return obj;
}

static void _update_window(void *data, int width)
{
	struct appdata *ad = data;
	int root_w, root_h;
	Ecore_X_Window xwin, root;

	INFO("_update_window");

	xwin = elm_win_xwindow_get(ad->win_main);
	if (!xwin)
		return;
	root = ecore_x_window_root_get(xwin);
	if (!root)
		return;
	ecore_x_window_size_get(root, &root_w, &root_h);

		if (ad->angle == 0 || ad->angle == 180)
			ad->w = root_w;
		else
			ad->w = root_h;

	switch (ad->angle) {
	case 0:
		ecore_x_window_shape_input_rectangle_set(xwin, root_w - ad->w,
							 0, ad->w, ad->h);
		break;
	case 90:
		ecore_x_window_shape_input_rectangle_set(xwin, 0, 0, ad->h,
							 ad->w);
		break;
	case 180:
		ecore_x_window_shape_input_rectangle_set(xwin, 0, 0, ad->w,
							 ad->h);
		break;
	case 270:
		ecore_x_window_shape_input_rectangle_set(xwin,
							 0, root_h - ad->w,
							 ad->h, ad->w);
		break;
	default:
		break;
	}

}

static void _update_display(void *data)
{
	struct appdata *ad = data;
	Indicator_Icon_Object *icon;
	Eina_List *l;
	int i = 0;

	for (i = 0; i < _FIXED_COUNT; ++i)
		elm_box_unpack_all(_fixed_box[i]);

	elm_box_unpack_all(_non_fixed_box);

	/* Box Pack : Fixed Icon List */
	EINA_LIST_FOREACH(_view_list, l, icon) {
		if (icon->obj_exist == EINA_FALSE) {
			if (indicator_util_icon_add(icon) == EINA_TRUE)
				icon->obj_exist = EINA_TRUE;
		}
		if (icon->obj_exist == EINA_TRUE) {
			if (icon->fixed == EINA_TRUE
			&& icon->priority < INDICATOR_PRIORITY_FIXED_MAX) {
				Evas_Coord x, y, h, w;
				Evas_Object *img_eo =
					evas_object_data_get(icon->img_obj.obj,
					"imgicon");
				evas_object_size_hint_min_set(img_eo,
					icon->img_obj.width * elm_config_scale_get(),
					ICON_HEIGHT * elm_config_scale_get());
				switch (icon->type) {
				case INDICATOR_IMG_ICON:
					evas_object_show(icon->img_obj.obj);
					elm_box_pack_start(_fixed_box
							   [icon->priority],
							   icon->img_obj.obj);
					break;
				case INDICATOR_TXT_ICON:
					evas_object_show(icon->txt_obj.obj);
					elm_box_pack_start(_fixed_box
							   [icon->priority],
							   icon->txt_obj.obj);
					break;
				case INDICATOR_TXT_WITH_IMG_ICON:
					evas_object_show(icon->txt_obj.obj);
					evas_object_show(icon->img_obj.obj);
					elm_box_pack_start(_fixed_box
							   [icon->priority],
							   icon->img_obj.obj);
					elm_box_pack_start(_fixed_box
							   [icon->priority],
							   icon->txt_obj.obj);
					break;
				}

				evas_object_geometry_get(
						_fixed_box[icon->priority],
						&x, &y, &h, &w);

				INFO("Fixed Icon : %s %d %d %d %d",
						icon->name, x, y, h, w);
			}
		}
	}

	/* Box Pack : Non-Fixed Icon List */
	EINA_LIST_FOREACH(_view_nonfixed_list, l, icon) {
		if (icon->obj_exist == EINA_FALSE) {
			if (indicator_util_icon_add(icon) == EINA_TRUE)
				icon->obj_exist = EINA_TRUE;
		}
		if (icon->obj_exist == EINA_TRUE) {
			if (icon->fixed == EINA_FALSE
			&& icon->priority >= INDICATOR_PRIORITY_NON_FIXED_MIN
			&& icon->priority <= INDICATOR_PRIORITY_NON_FIXED_MAX) {
				Evas_Coord x, y, h, w;
				Evas_Object *img_eo =
					evas_object_data_get(icon->img_obj.obj,
							"imgicon");
				evas_object_size_hint_min_set(img_eo,
					icon->img_obj.width * elm_config_scale_get(),
					ICON_HEIGHT * elm_config_scale_get());
				switch (icon->type) {
				case INDICATOR_IMG_ICON:
					evas_object_show(icon->img_obj.obj);
					elm_box_pack_end(_non_fixed_box,
							   icon->img_obj.obj);
					break;
				case INDICATOR_TXT_ICON:
					evas_object_show(icon->txt_obj.obj);
					elm_box_pack_end(_non_fixed_box,
							   icon->txt_obj.obj);
					break;
				case INDICATOR_TXT_WITH_IMG_ICON:
					evas_object_show(icon->txt_obj.obj);
					evas_object_show(icon->img_obj.obj);
					elm_box_pack_end(_non_fixed_box,
							   icon->txt_obj.obj);
					elm_box_pack_end(_non_fixed_box,
							   icon->img_obj.obj);
					break;
				}
				evas_object_geometry_get(_non_fixed_box,
						&x, &y, &h, &w);
				INFO("Non-Fixed Icon : %s %d %d %d %d",
						icon->name, x, y, h, w);
			}


		}
	}

	if (ad)
		_update_window(ad, -1);
}

void indicator_util_hide_all_icons(void)
{
	Indicator_Icon_Object *icon;
	Eina_List *l;

	EINA_LIST_FOREACH(_view_list, l, icon) {
		if (icon && icon->obj_exist == EINA_TRUE) {
			if (indicator_util_icon_del(icon) == EINA_TRUE) {
				icon->obj_exist = EINA_FALSE;
				icon->txt_obj.obj = NULL;
				icon->img_obj.obj = NULL;
			}
		}
	}
}

void indicator_util_update_display(void *data)
{
	struct appdata *ad = data;

	if (ad != NULL)
		_update_window(ad, -1);

	_update_display(ad);
}

int icon_box_pack(Indicator_Icon_Object *icon)
{
	retif(icon == NULL, FAIL, "Invalid parameter!");

	if (indicator_util_is_show_icon(icon))
		return OK;

	if (EINA_TRUE == icon->fixed) {
		icon->exist_in_view = EINA_TRUE;
		_view_list = eina_list_append(_view_list, icon);
	} else {
		Indicator_Icon_Object *data;
		Eina_List *l;

		EINA_LIST_FOREACH(_view_nonfixed_list, l, data) {
			if (data->priority >= icon->priority) {
				icon->exist_in_view = EINA_TRUE;
				_view_nonfixed_list =
					eina_list_prepend_relative_list(
						_view_nonfixed_list, icon, l);
				return OK;
			}
		}

		INFO("[icon_box_pack] It is first!");
		/* if finding condition is failed, append it at tail */
		icon->exist_in_view = EINA_TRUE;
		_view_nonfixed_list =
			eina_list_append(_view_nonfixed_list, icon);
	}

	return OK;
}

int icon_box_unpack(Indicator_Icon_Object *icon)
{
	retif(icon == NULL, FAIL, "Invalid parameter!");

	if (icon->fixed == EINA_TRUE) {
		icon->exist_in_view = EINA_FALSE;
		_view_list = eina_list_remove(_view_list, icon);
	} else {
		icon->exist_in_view = EINA_FALSE;
		_view_nonfixed_list =
			eina_list_remove(_view_nonfixed_list, icon);
	}

	if (icon->obj_exist == EINA_TRUE) {
		if (indicator_util_icon_del(icon) == EINA_TRUE) {
			icon->obj_exist = EINA_FALSE;
			INFO("%s icon object is freed!", icon->name);
		}
	}

	return OK;
}

int icon_box_init(void *data)
{
	struct appdata *ad = data;
	char *str_text = NULL;
	int i = 0;
	retif(data == NULL
	      || ad->layout_main == NULL, FAIL, "Invalid parameter!");

	/* Make Fixed Box Object */
	for (i = 0; i < _FIXED_COUNT; ++i) {
		if (_fixed_box[i] == NULL) {
			Eina_Bool ret;

			_fixed_box[i] = indicator_box_add(ad->layout_main);
			retif(_fixed_box[i] == NULL, FAIL,
				"Failed to add _fixed_box object!");

			Eina_Strbuf *temp_str = eina_strbuf_new();
			eina_strbuf_append_printf(temp_str, "%s%d",
				_FIXED_BOX_PART_NAME, i);
			str_text = eina_strbuf_string_steal(temp_str);

			ret = edje_object_part_swallow(elm_layout_edje_get
						 (ad->layout_main), str_text,
						 _fixed_box[i]);
			INFO("[ICON INIT] : %d %s %d " , i, str_text, ret);
			eina_strbuf_free(temp_str);
			free(str_text);
		}
	}

	/* Make Non Fixed Box Object */
	_non_fixed_box = indicator_box_add(ad->layout_main);
	retif(_non_fixed_box == NULL, FAIL,
		"Failed to create _non_fixed_box object!");
	evas_object_size_hint_align_set(_non_fixed_box,
			EVAS_HINT_FILL, EVAS_HINT_FILL);

	edje_object_part_swallow(elm_layout_edje_get
				 (ad->layout_main), _NON_FIXED_BOX_PART_NAME,
				 _non_fixed_box);
	indicator_util_update_display(data);

	return 0;
}

int icon_box_fini(void *data)
{
	struct appdata *ad = data;
	int i = 0;

	retif(data == NULL || ad->layout_main == NULL,
		FAIL, "Invalid parameter!");

	if (_non_fixed_box != NULL) {
		edje_object_part_unswallow(elm_layout_edje_get(ad->layout_main),
					   _non_fixed_box);
		elm_box_unpack_all(_non_fixed_box);
		evas_object_del(_non_fixed_box);
		_non_fixed_box = NULL;
	}

	for (i = 0; i < _FIXED_COUNT; ++i) {
		if (_fixed_box[i] != NULL) {
			edje_object_part_unswallow(elm_layout_edje_get
						   (ad->layout_main),
						   _fixed_box[i]);
			elm_box_unpack_all(_fixed_box[i]);
			evas_object_del(_fixed_box[i]);
			_fixed_box[i] = NULL;
		}
	}
	return 0;
}

unsigned int indicator_get_count_in_fixed_list(void)
{
	return eina_list_count(_view_list);
}
unsigned int indicator_get_count_in_non_fixed_list(void)
{
	return eina_list_count(_view_nonfixed_list);
}

void indicator_set_count_in_non_fixed_list(int text_count)
{
	unsigned int count = indicator_get_count_in_non_fixed_list();

	indicator_non_fixed_list_max_cnt = text_count;
	DBG("Count : %d Max : %d", count, indicator_non_fixed_list_max_cnt);

	if (count > indicator_non_fixed_list_max_cnt) {
		/* Hide */
		while (count > indicator_non_fixed_list_max_cnt) {
			Indicator_Icon_Object *lowest_icon =
				eina_list_nth(_view_nonfixed_list, 0);
			Indicator_Icon_Object *wish_remove_icon =
				indicator_get_wish_to_remove_icon(lowest_icon);

			if (wish_remove_icon) {
				icon_box_unpack(wish_remove_icon);
				count = indicator_get_count_in_non_fixed_list();
			} else
				break;
		}
	} else {
		/* Show */
		while (count <= indicator_non_fixed_list_max_cnt) {
			Indicator_Icon_Object *wait_icon =
			indicator_get_highest_priority_icon_in_wish_show_list();

			if (wait_icon) {
				icon_box_pack(wait_icon);
				count = indicator_get_count_in_non_fixed_list();
			} else
				break;
		}
	}
}

int indicator_get_max_count_in_non_fixed_list(void)
{
	return indicator_non_fixed_list_max_cnt;
}

Icon_AddType indicator_is_enable_to_insert_in_non_fixed_list(
						Indicator_Icon_Object *obj)
{
	Indicator_Icon_Object *icon;
	Eina_List *l;

	int higher_cnt = 0;
	int same_cnt = 0;
	int same_top_cnt = 0;
	int lower_cnt = 0;
	int total_cnt = eina_list_count(_view_nonfixed_list);

	EINA_LIST_FOREACH(_view_nonfixed_list, l, icon) {
		/* If same Icon exist in non-fixed view list,
		 * it need not to add icon
		 */
		if (!strcmp(icon->name, obj->name))
			return CANNOT_ADD;

		if (icon->priority > obj->priority)
			++higher_cnt;

		else if (icon->priority == obj->priority) {
			++same_cnt;
			if (icon->always_top == EINA_TRUE)
				++same_top_cnt;
		} else
			lower_cnt++;
	}

	INFO("[INSERT ENABLE] : %d %d %d %d %d %d",
			higher_cnt, lower_cnt, same_cnt, same_top_cnt,
			indicator_non_fixed_list_max_cnt, total_cnt);

	/* Icon can bed added in non fixed list
	 * and not need to remove icon in list.
	 */
	if (higher_cnt + same_cnt < indicator_non_fixed_list_max_cnt
	/* In case of below, We should remove one icon
	 * in list and add new icon.
	 */
		|| (higher_cnt + same_cnt == indicator_non_fixed_list_max_cnt
			&& same_top_cnt < same_cnt
			&& obj->always_top == EINA_TRUE)) {

		if (total_cnt >= indicator_non_fixed_list_max_cnt)
			return CAN_ADD_WITH_DEL;
		else
			return CAN_ADD_WITHOUT_DEL;
	}
	return CANNOT_ADD;
}

int indicator_util_get_priority_in_move_area(Evas_Coord curr_x,
					Evas_Coord curr_y)
{
	Evas_Coord x, y, h, w;

	/* Home Area Check for launching home */
	evas_object_geometry_get(_fixed_box[INDICATOR_PRIORITY_FIXED5],
			&x, &y, &h, &w);
	INFO("[Current Location] %d %d %d %d %d %d",
			x, y, h, w, curr_x, curr_y);

	if (curr_x >= x - CORRECTION && curr_x <= x+h + CORRECTION) {
		if (curr_y == -1)
			return INDICATOR_PRIORITY_FIXED5;
		else if (curr_y >= y - CORRECTION && curr_y <= y+h + CORRECTION)
			return INDICATOR_PRIORITY_FIXED5;
	}


	/* Non Fixed Area check for show/hide quickpanel */
	return -1;
}

int indicator_util_check_home_icon_area(Evas_Coord curr_x, Evas_Coord curr_y)
{
	Evas_Coord x, y, w, h;

	/* Home Area Check for launching home */
	evas_object_geometry_get(_fixed_box[INDICATOR_PRIORITY_FIXED5],
			&x, &y, &w, &h);

	INFO("[Home icon area] [%d, %d] [wxh][%dx%d], cur[%d, %d]",
			x, y, w, h, curr_x, curr_y);

	if (curr_x >= x && curr_x <= x + w && curr_y >= y && curr_y <= y + h)
		return 0;

	return -1;
}

