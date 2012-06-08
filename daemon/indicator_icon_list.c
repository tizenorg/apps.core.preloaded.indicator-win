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
#include "common.h"
#include "indicator.h"
#include "indicator_icon_list.h"
#include "indicator_icon_util.h"

static Eina_List *icon_list;
static Eina_List *nonfixed_icon_list;

void indicator_icon_object_free(Indicator_Icon_Object *icon)
{
	if (icon) {
		if (icon->fini)
			icon->fini();

		if (icon->obj_exist == EINA_TRUE) {
			if (indicator_util_icon_del(icon) == EINA_TRUE) {
				icon->obj_exist = EINA_FALSE;
				icon->txt_obj.obj = NULL;
				icon->img_obj.obj = NULL;
			}
		}
		free(icon);
	}
}

int indicator_icon_list_free(void)
{
	Eina_List *l;
	Eina_List *l_next;
	Indicator_Icon_Object *data;

	retif(icon_list == NULL, OK, "Empty List!");

	EINA_LIST_FOREACH_SAFE(icon_list, l, l_next, data) {
		indicator_icon_object_free(data);
		icon_list = eina_list_remove_list(icon_list, l);
		if (eina_error_get())
			return FAIL;
	}
	eina_list_free(icon_list);
	return eina_error_get();
}

int indicator_icon_list_update(Indicator_Icon_Object *obj)
{
	Eina_List *l;

	Indicator_Icon_Object *data;

	retif(obj == NULL, FAIL, "Invalid parameter!");

	if (obj->fixed == EINA_TRUE) {
		icon_list = eina_list_remove(icon_list, obj);

		/* insert object to list */
		EINA_LIST_REVERSE_FOREACH(icon_list, l, data) {
			if (data->priority == obj->priority &&
			    data->always_top == EINA_TRUE)
				continue;

			if (data->priority <= obj->priority) {
				icon_list = eina_list_append_relative_list(
						icon_list, obj, l);
				return eina_error_get();
			}
		}

		/* if finding condition is failed, append it at tail */
		icon_list = eina_list_prepend(icon_list, obj);
	} else {
		nonfixed_icon_list = eina_list_remove(nonfixed_icon_list, obj);

		/* insert object to list */
		EINA_LIST_REVERSE_FOREACH(nonfixed_icon_list, l, data) {
			if (data->priority == obj->priority &&
			    data->always_top == EINA_TRUE) {
				continue;
			}
			if (data->priority <= obj->priority) {
				nonfixed_icon_list =
					eina_list_append_relative_list(
						nonfixed_icon_list, obj, l);
				return eina_error_get();
			}
		}

		/* if finding condition is failed, append it at tail */
		nonfixed_icon_list = eina_list_prepend(nonfixed_icon_list, obj);
	}
	return eina_error_get();
}

int indicator_icon_list_insert(Indicator_Icon_Object *obj)
{
	Eina_List *l;
	Indicator_Icon_Object *data;

	retif(obj == NULL || obj->name == NULL, FAIL, "Invalid parameter!");

	if (obj->fixed == EINA_TRUE) {
		/* check name */
		EINA_LIST_REVERSE_FOREACH(icon_list, l, data) {
			retif(data->name == obj->name, FAIL,
			      "%s is already exist in the list!", obj->name);
		}

		/* Set internal data */
		obj->wish_to_show = EINA_FALSE;

		/* insert object to list */
		EINA_LIST_REVERSE_FOREACH(icon_list, l, data) {
			if (data->priority == obj->priority
				&& data->always_top == EINA_TRUE)
				continue;

			if (data->priority <= obj->priority) {
				icon_list = eina_list_append_relative_list(
							icon_list, obj, l);
					return eina_error_get();
			}
		}
		icon_list = eina_list_prepend(icon_list, obj);
	} else {
		/* check name */
		EINA_LIST_REVERSE_FOREACH(nonfixed_icon_list, l, data) {
			retif(data->name == obj->name, FAIL,
			      "%s is already exist in the list!", obj->name);
		}

		/* Set internal data */
		obj->wish_to_show = EINA_FALSE;

		/* insert object to list */
		EINA_LIST_REVERSE_FOREACH(nonfixed_icon_list, l, data) {
			if (data->priority == obj->priority &&
				data->always_top == EINA_TRUE)
				continue;

			if (data->priority <= obj->priority) {
				nonfixed_icon_list =
					eina_list_append_relative_list(
						nonfixed_icon_list, obj, l);
				return eina_error_get();
			}
		}
		/* if finding condition is failed, append it at tail */
		nonfixed_icon_list = eina_list_prepend(nonfixed_icon_list, obj);
	}

	return eina_error_get();
}

int indicator_icon_list_remove(Indicator_Icon_Object *obj)
{
	if (obj->fixed == EINA_TRUE) {
		retif(icon_list == NULL
			|| obj == NULL, FAIL, "Invalid parameter!");
		icon_list = eina_list_remove(icon_list, obj);
	} else {
		retif(nonfixed_icon_list == NULL
			|| obj == NULL, FAIL, "Invalid parameter!");
		nonfixed_icon_list = eina_list_remove(nonfixed_icon_list, obj);
	}

	return eina_error_get();
}

Indicator_Icon_Object *indicator_get_prev_icon(Indicator_Icon_Object * obj)
{
	Eina_List *l;
	Indicator_Icon_Object *data;

	retif(obj == NULL || obj->name == NULL, NULL, "Invalid parameter!");

	if (obj->fixed == EINA_TRUE) {
		EINA_LIST_FOREACH(icon_list, l, data) {
			if (data->name == obj->name)
				break;
		}
	} else {
		EINA_LIST_FOREACH(nonfixed_icon_list, l, data) {
			if (data->name == obj->name)
				break;
		}
	}

	retif(l == NULL, NULL, "Cannot find %s in icon list!", obj->name);

	for (l = eina_list_prev(l), data = eina_list_data_get(l);
	     l; l = eina_list_prev(l), data = eina_list_data_get(l)) {
		if (obj->priority == data->priority
			&& indicator_util_is_show_icon(data))
			return data;
	}
	return obj;
}

Indicator_Icon_Object *indicator_get_last_shown_icon(void)
{
	Eina_List *l;
	Indicator_Icon_Object *data;

	retif(icon_list == NULL, NULL, "Invalid parameter!");

	EINA_LIST_REVERSE_FOREACH(icon_list, l, data) {
		if (indicator_util_is_show_icon(data))
			return data;
	}
	return NULL;
}

Indicator_Icon_Object *indicator_get_first_shown_icon(void)
{
	Eina_List *l;
	Indicator_Icon_Object *data;

	retif(icon_list == NULL, NULL, "Invalid parameter!");

	EINA_LIST_FOREACH(icon_list, l, data) {
		if (indicator_util_is_show_icon(data))
			return data;
	}
	return NULL;
}

Indicator_Icon_Object *indicator_get_hidden_icon(void)
{
	Eina_List *l;
	Indicator_Icon_Object *data;
	int prio;

	retif(icon_list == NULL, NULL, "Invalid parameter!");

	EINA_LIST_REVERSE_FOREACH(icon_list, l, data) {
		if (indicator_util_is_show_icon(data)) {
			prio = data->priority;
			break;
		}
	}

	for (l = eina_list_next(l), data = eina_list_data_get(l);
	     l; l = eina_list_next(l), data = eina_list_data_get(l)) {
		if (data->wish_to_show == EINA_TRUE && data->priority != prio)
			return data;
	}
	return NULL;
}

Indicator_Icon_Object
*indicator_get_shown_same_priority_icon_in_fixed_view_list(int prio)
{
	Eina_List *l;
	Indicator_Icon_Object *data;

	retif(icon_list == NULL, NULL, "Invalid parameter!");

	EINA_LIST_FOREACH(icon_list, l, data) {
		if (data->priority == prio && data->wish_to_show == EINA_TRUE)
			return data;
	}
	return NULL;
}

Indicator_Icon_Object
*indicator_get_wish_to_show_icon(Indicator_Icon_Object *obj)
{
	Eina_List *l;
	Indicator_Icon_Object *data;

	DBG("indicator_get_wish_to_show_icon!");
	retif(obj == NULL, NULL, "Invalid parameter!");

	if (obj->fixed == EINA_TRUE) {
		EINA_LIST_REVERSE_FOREACH(icon_list, l, data) {
			if (data->priority == obj->priority
				&& data->wish_to_show == EINA_TRUE
				&& data->exist_in_view == EINA_FALSE) {
				return data;
			}
		}
		return NULL;
	} else {
		EINA_LIST_REVERSE_FOREACH(nonfixed_icon_list, l, data) {
			if (data->wish_to_show == EINA_TRUE
				&& data->exist_in_view == EINA_FALSE)
				return data;
		}
		return NULL;
	}
}

Indicator_Icon_Object
*indicator_get_wish_to_remove_icon(Indicator_Icon_Object *obj)
{
	Eina_List *l;
	Indicator_Icon_Object *data;

	retif(obj == NULL, NULL, "Invalid parameter!");

	if (obj->fixed == EINA_TRUE) {
		EINA_LIST_REVERSE_FOREACH(icon_list, l, data) {
			if (data->priority == obj->priority
				&& data->wish_to_show == EINA_TRUE)
				return data;
		}
		return NULL;
	} else {
		/* find lowest priority of icon */
		EINA_LIST_FOREACH(nonfixed_icon_list, l, data) {
			if (data->wish_to_show == EINA_TRUE
				&& data->exist_in_view == EINA_TRUE)
				return data;
		}
		return NULL;
	}
}


Indicator_Icon_Object
*indicator_get_lowest_priority_icon_in_wish_show_list(void)
{
	Eina_List *l;
	Indicator_Icon_Object *data;

	EINA_LIST_FOREACH(nonfixed_icon_list, l, data) {
		if (data->wish_to_show == EINA_TRUE
			&& data->exist_in_view == EINA_FALSE) {
			DBG("Get Lowest Priority Item : %s", data->name);
			return data;
		}
	}

	return NULL;
}

Indicator_Icon_Object
*indicator_get_highest_priority_icon_in_wish_show_list(void)
{
	Eina_List *l;
	Indicator_Icon_Object *data;

	EINA_LIST_REVERSE_FOREACH(nonfixed_icon_list, l, data) {
		if (data->wish_to_show == EINA_TRUE
			&& data->exist_in_view == EINA_FALSE) {
			DBG("Get Highest Priority Item : %s", data->name);
			return data;
		}
	}

	return NULL;
}

int indicator_get_same_priority_icons(Eina_List **result,
				      Indicator_Icon_Object *obj)
{
	Eina_List *l;
	Indicator_Icon_Object *data;

	retif(icon_list == NULL || obj == NULL, FAIL, "Invalid parameter!");

	EINA_LIST_FOREACH(icon_list, l, data) {
		if (data->priority == obj->priority)
			*result = eina_list_append(*result, data);
	}

	if (eina_list_count(*result) == 1)
		return FAIL;

	return OK;
}
