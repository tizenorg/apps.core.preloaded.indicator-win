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


#include <Eina.h>
#include "common.h"
#include "indicator_box_util.h"
#include "indicator_icon_util.h"
#include "indicator_icon_list.h"
#include "indicator_ui.h"
#include "indicator_gui.h"
#include "indicator_util.h"

/******************************************************************************
 *
 * DEFINITAIONS: Static variables
 *
 *****************************************************************************/

extern Eina_List *_view_list;
extern Eina_List *_view_nonfixed_list;

static void _animation_set(Indicator_Icon_Object *icon, int type)
{
	Evas_Object *img_edje, *txt_edje;
	retif(icon == NULL, , "Invalid parameter!");

	const char *BLINK_SIGNAL = "icon,state,blink";
	const char *ROATATE_SIGNAL = "icon,state,roate";
	const char *METRONOME_SIGNAL = "icon,state,metronome";
	const char *DEFAULT = "icon,state,default";

	const char *send_signal = DEFAULT;

	switch (type) {
	case ICON_ANI_BLINK:
		send_signal = BLINK_SIGNAL;
		break;
	case ICON_ANI_ROTATE:
		send_signal = ROATATE_SIGNAL;
		break;
	case ICON_ANI_METRONOME:
		send_signal = METRONOME_SIGNAL;
		break;
	default:
		break;
	}

	switch (icon->type) {
	case INDICATOR_IMG_ICON:
		img_edje = elm_layout_edje_get(icon->img_obj.obj);
		edje_object_signal_emit(img_edje, send_signal,
					"elm.swallow.icon");
		break;
	case INDICATOR_TXT_ICON:
		txt_edje = elm_layout_edje_get(icon->txt_obj.obj);
		edje_object_signal_emit(txt_edje, send_signal,
					"elm.swallow.icon");
		break;
	case INDICATOR_TXT_WITH_IMG_ICON:
		txt_edje = elm_layout_edje_get(icon->txt_obj.obj);
		img_edje = elm_layout_edje_get(icon->img_obj.obj);
		edje_object_signal_emit(txt_edje, send_signal,
					"elm.swallow.lefticon");
		edje_object_signal_emit(img_edje, send_signal,
					"elm.swallow.righticon");
		break;
	default:
		break;
	}
}

void indicator_util_icon_animation_set(Indicator_Icon_Object *icon,
				       enum indicator_icon_ani type)
{
	retif(icon == NULL, , "Invalid parameter!");

	icon->ani = type;
	if (icon->obj_exist)
		_animation_set(icon, type);
}

static Evas_Object *_img_icon_add(const char *imgpath,
			Eina_Bool isFixed, int width, void *data)
{
	struct appdata *ad = data;
	char path[PATH_MAX];
	Evas_Object *icon;
	Evas_Object *ly;

	retif(ad == NULL || ad->layout_main == NULL || imgpath == NULL,
		NULL, "Invalid parameter!");

	memset(path, 0x00, sizeof(path));

	ly = elm_layout_add(ad->layout_main);
	retif(ly == NULL, NULL, "Cannot create layout object!");

	if (isFixed)
		elm_layout_file_set(ly, ICON_THEME_FILE,
				"elm/indicator/icon/base");
	else
		elm_layout_file_set(ly, ICON_NONFIXED_THEME_FILE,
				"elm/indicator/icon/base");

	icon = elm_icon_add(ly);
	retif(icon == NULL, NULL, "Cannot create elm icon object!");

	/* absolute path? */
	if (strncmp(imgpath, "/", 1) != 0) {
		snprintf(path, sizeof(path), "%s/%s", get_icon_dir(), imgpath);
		elm_icon_file_set(icon, path, NULL);
	} else {
		elm_icon_file_set(icon, imgpath, NULL);
	}
	evas_object_size_hint_min_set(icon, width * elm_config_scale_get(),
				      ICON_HEIGHT * elm_config_scale_get());

	elm_object_part_content_set(ly, "elm.swallow.icon", icon);

	evas_object_data_set(ly, "imgicon", icon);
	evas_object_size_hint_min_set(ly, width * elm_config_scale_get(),
				      ICON_HEIGHT * elm_config_scale_get());
	evas_object_hide(ly);
	return ly;
}

static Evas_Object *_txt_icon_add(const char *txt,
			Eina_Bool isFixed, int width, void *data)
{
	struct appdata *ad = data;
	char *color_added_str = NULL;
	Evas_Object *icon;
	Evas_Object *ly;

	retif(ad == NULL || ad->layout_main == NULL
	      || txt == NULL, NULL, "Invalid parameter!");

	ly = elm_layout_add(ad->layout_main);
	elm_layout_file_set(ly, ICON_THEME_FILE, "elm/indicator/icon/base");

	icon = elm_label_add(ly);
	color_added_str = (char *)set_label_text_color(txt);

	elm_object_text_set(icon, color_added_str);
	evas_object_size_hint_align_set(icon, 0.5, 0.5);

	elm_object_part_content_set(ly, "elm.swallow.icon", icon);

	evas_object_data_set(ly, "txticon", icon);
	evas_object_hide(ly);

	return ly;
}

char *indicator_util_icon_label_set(const char *buf, char *font_name,
				    char *font_style, int font_size, void *data)
{
	Eina_Strbuf *temp_buf = NULL;
	char *ret_str = NULL;
	char *label_font = ICON_FONT_NAME;
	char *label_font_style = ICON_FONT_STYLE;
	int label_font_size = ICON_FONT_SIZE;
	Eina_Bool buf_result = EINA_FALSE;

	retif(data == NULL || buf == NULL, NULL, "Invalid parameter!");

	temp_buf = eina_strbuf_new();
	if (font_name != NULL)
		label_font = font_name;
	if (font_style != NULL)
		label_font_style = font_style;
	if (font_size > 0)
		label_font_size = font_size;

	buf_result = eina_strbuf_append_printf(temp_buf, CUSTOM_LABEL_STRING,
					       label_font, label_font_style,
					       label_font_size, buf);

	if (buf_result != EINA_FALSE)
		ret_str = eina_strbuf_string_steal(temp_buf);

	eina_strbuf_free(temp_buf);

	return ret_str;
}

Eina_Bool indicator_util_icon_add(Indicator_Icon_Object *icon)
{
	retif(icon == NULL, EINA_FALSE, "Invalid parameter!");
	int t_width = icon->txt_obj.width;
	int i_width = icon->img_obj.width;

	if (icon->txt_obj.width <= 0)
		t_width = icon->txt_obj.width = ICON_WIDTH;
	if (icon->img_obj.width <= 0)
		i_width = icon->img_obj.width = ICON_WIDTH;

	switch (icon->type) {
	case INDICATOR_TXT_ICON:
		icon->txt_obj.obj = _txt_icon_add(icon->txt_obj.data,
					icon->fixed, t_width, icon->ad);
		break;
	case INDICATOR_IMG_ICON:
		icon->img_obj.obj = _img_icon_add(icon->img_obj.data,
					icon->fixed, i_width, icon->ad);
		break;
	case INDICATOR_TXT_WITH_IMG_ICON:
		icon->txt_obj.obj = _txt_icon_add(icon->txt_obj.data,
					icon->fixed, t_width, icon->ad);
		icon->img_obj.obj = _img_icon_add(icon->img_obj.data,
					icon->fixed, i_width, icon->ad);
		break;

	default:
		ERR("Icon type check error!");
		return EINA_FALSE;
	}
	_animation_set(icon, icon->ani);
	return EINA_TRUE;
}

Eina_Bool indicator_util_icon_del(Indicator_Icon_Object *icon)
{
	Evas_Object *icon_obj;
	retif(icon == NULL, EINA_FALSE, "Invalid parameter!");

	if (icon->obj_exist != EINA_FALSE) {
		if (icon->txt_obj.obj) {
			icon_obj =
			    evas_object_data_get(icon->txt_obj.obj, "txticon");
			evas_object_del(icon_obj);
			evas_object_del(icon->txt_obj.obj);
			icon->txt_obj.obj = NULL;
		}
		if (icon->img_obj.obj) {
			icon_obj =
			    evas_object_data_get(icon->img_obj.obj, "imgicon");
			evas_object_del(icon_obj);
			evas_object_del(icon->img_obj.obj);
			icon->img_obj.obj = NULL;
		}
	}
	return EINA_TRUE;
}

Eina_Bool indicator_util_is_show_icon(Indicator_Icon_Object *obj)
{
	retif(obj == NULL, FAIL, "Invalid parameter!");

	if (obj->fixed == EINA_TRUE) {
		if (eina_list_data_find(_view_list, obj))
			return 1;
		else
			return 0;
	} else {
		if (eina_list_data_find(_view_nonfixed_list, obj))
			return 1;
		else
			return 0;
	}
}

/******************************************************************************
 *
 * Static functions : util functions - check priority
 *
 *****************************************************************************/

static int show_other_icon_in_same_priority(Indicator_Icon_Object *icon)
{
	Indicator_Icon_Object *wish_add_icon;
	retif(icon == NULL, FAIL, "Invalid parameter!");

	wish_add_icon = indicator_get_wish_to_show_icon(icon);
	if (wish_add_icon == NULL) {
		DBG("Wish Icon is not exist!");
		return OK;
	}

	if (indicator_util_is_show_icon(wish_add_icon)) {
		/* already shown icon */
		DBG("Wish Icon is alreay shown!");
		return OK;
	}

	DBG("%s icon is shown!", wish_add_icon->name);
	return icon_box_pack(wish_add_icon);
}

static int hide_other_icons_in_view_list(Indicator_Icon_Object *icon)
{
	Indicator_Icon_Object *wish_remove_icon = NULL;
	retif(icon == NULL, FAIL, "Invalid parameter!");

	if (EINA_FALSE == icon->fixed) {
		Icon_AddType ret;
		Eina_Error err;

		/* In Case of Nonfixed icon, remove same or
		 * lower priority icon. Check count of non-fixed view list
		 * to insert icon
		 */
		ret = indicator_is_enable_to_insert_in_non_fixed_list(icon);
		icon->wish_to_show = EINA_TRUE;
		err = indicator_icon_list_update(icon);

		switch (ret) {
		case CAN_ADD_WITH_DEL:
			wish_remove_icon =
				indicator_get_wish_to_remove_icon(icon);

			DBG("[ICON UTIL NON-FIXED] %s icon is hidden!",
					wish_remove_icon->name);
			icon_box_unpack(wish_remove_icon);

			retif(wish_remove_icon == NULL, FAIL,
				"Unexpected Error : CAN_ADD_WITH_DEL");
			break;
		case CAN_ADD_WITHOUT_DEL:
			break;
		case CANNOT_ADD:
			return FAIL;
			break;
		}

		return OK;
	} else {
		/* In Case of fixed icon, remove same priority icon */
		wish_remove_icon = indicator_get_wish_to_remove_icon(icon);

		/* first icon in the priority */
		if (wish_remove_icon == NULL)
			return OK;


		/* already shown icon */
		if (wish_remove_icon == icon)
			return FAIL;

		icon->wish_to_show = EINA_TRUE;
		indicator_icon_list_update(icon);

		/* wish_remove_icon is always_top icon */
		if (wish_remove_icon->always_top)
			return FAIL;

		/* Other Icon of Same Priority should remove in view list */
		DBG("[ICON UTIL FIXED] %s icon is hidden!",
					wish_remove_icon->name);
		icon_box_unpack(wish_remove_icon);
	}
	return OK;
}

/******************************************************************************
 *
 * Util Functions : external
 *
 *****************************************************************************/

int indicator_util_layout_del(void *data)
{
	return icon_box_fini(data);
}

int indicator_util_layout_add(void *data)
{
	struct appdata *ad = data;
	retif(ad == NULL
	      || ad->layout_main == NULL, FAIL, "Invalid parameter!");
	/* first, clear layout */
	indicator_util_layout_del(data);
	return icon_box_init(data);
}


int indicator_util_icon_width_set(Indicator_Icon_Object *icon)
{
	return 0;
}

void indicator_util_icon_fixed_set(Indicator_Icon_Object *icon,
				   Eina_Bool state)
{
	retif(icon == NULL, , "Invalid parameter!");

	if (icon->fixed == state)
		return;
	icon->fixed = state;

	if (indicator_util_is_show_icon(icon))
		indicator_util_update_display(NULL);
}

static int _icon_update(Indicator_Icon_Object *icon)
{
	struct appdata *ad = NULL;
	Evas_Object *txt_eo, *img_eo;
	char buf[PATH_MAX];

	retif(icon == NULL || icon->ad == NULL, FAIL, "Invalid parameter!");
	ad = icon->ad;

	memset(buf, 0x00, sizeof(buf));

	if (icon->type == INDICATOR_IMG_ICON
	    || icon->type == INDICATOR_TXT_WITH_IMG_ICON) {
		img_eo = evas_object_data_get(icon->img_obj.obj, "imgicon");

		/* Check absolute path */
		if (strncmp(icon->img_obj.data, "/", 1) != 0) {
			snprintf(buf, sizeof(buf), "%s/%s", get_icon_dir(),
				 icon->img_obj.data);
			elm_icon_file_set(img_eo, buf, NULL);
		} else {
			retif(icon->img_obj.data[0] == '\0', FAIL,
			      "Invalid parameter!");
			elm_icon_file_set(img_eo, icon->img_obj.data, NULL);
		}

		if (icon->img_obj.width >= 0) {
			evas_object_size_hint_min_set(img_eo,
				icon->img_obj.width * elm_config_scale_get(),
				ICON_HEIGHT * elm_config_scale_get());
		} else {
			evas_object_size_hint_min_set(img_eo,
				ICON_WIDTH * elm_config_scale_get(),
				ICON_HEIGHT * elm_config_scale_get());
	}
	}

	if (icon->type == INDICATOR_TXT_ICON
	    || icon->type == INDICATOR_TXT_WITH_IMG_ICON) {
		char *color_added_str = NULL;
		txt_eo = evas_object_data_get(icon->txt_obj.obj, "txticon");
		color_added_str =
		    (char *)set_label_text_color(icon->txt_obj.data);
		elm_object_text_set(txt_eo, color_added_str);
		free(color_added_str);
	}
	return OK;
}

void indicator_util_icon_show(Indicator_Icon_Object *icon)
{
	retif(icon == NULL, , "Invalid parameter!");

	/* Icon Update */
	if (icon->obj_exist != EINA_FALSE)
		_icon_update(icon);

	if (hide_other_icons_in_view_list(icon) == FAIL)
		return;

	icon->wish_to_show = EINA_TRUE;
	DBG("[ICON UTIL] %s icon is shown!", icon->name);

	icon_box_pack(icon);
	indicator_util_update_display(NULL);
}

void indicator_util_icon_hide(Indicator_Icon_Object *icon)
{
	int ret;

	retif(icon == NULL, , "Invalid parameter!");

	memset(icon->data, 0x00, sizeof(icon->data));
	icon->wish_to_show = EINA_FALSE;

	DBG("%s icon is hidden!", icon->name);

	if (icon->exist_in_view == EINA_TRUE) {
		ret = icon_box_unpack(icon);

		if (ret == FAIL)
			ERR("Failed to unpack %s!", icon->name);

		show_other_icon_in_same_priority(icon);
	}

	/* In case of 3rd event, "Hide" means remove in view list & Icon List */
	if (icon->priority == INDICATOR_PRIORITY_NON_FIXED_10)
		indicator_icon_list_remove(icon);

	indicator_util_update_display(NULL);
}

void indicator_util_event_count_set(int count, void *data)
{
	struct appdata *ad = data;
	Evas_Object *edje;
	static int _cnt = -1;
	char buf[1024];

	retif(data == NULL, , "Cannot get layout!");

	edje = elm_layout_edje_get(ad->layout_main);
	if (_cnt != count) {
		memset(buf, 0x00, sizeof(buf));
		if (count) {
			snprintf(buf, sizeof(buf), "%d", count);
			edje_object_signal_emit(edje, "badge,show,1",
						"elm.image.badge");
		} else {
			edje_object_signal_emit(edje, "badge,hide,1",
						"elm.image.badge");
		}
		edje_object_part_text_set(edje, "elm.text.badge", buf);
		_cnt = count;
	}
}

unsigned int indicator_util_max_visible_event_count(void)
{
	return indicator_get_max_count_in_non_fixed_list();
}
