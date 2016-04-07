/*
 *  Indicator
 *
 * Copyright (c) 2000 - 2015 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#ifndef __INDICATOR_TICKER_H__
#define __INDICATOR_TICKER_H__

typedef struct ticker {
	Evas_Object *win;
	Evas_Object *scroller;
	Ecore_Timer *timer;
	Eina_List *ticker_list;
} ticker_info_s;

extern int ticker_init(void *data);
extern int ticker_fini(void *data);

#endif
