/*******************************************************************************
 *                         COPYRIGHT NOTICE
 *                   "Copyright 2023 Nova Biomedical Corporation"
 *             This program is the property of Nova Biomedical Corporation
 *                 200 Prospect Street, Waltham, MA 02454-9141
 *             Any unauthorized use or duplication is prohibited
 ********************************************************************************
 *
 *  Title            -
 *  Source Filename  -
 *  Author           -
 *  Description      -
 *
 *******************************************************************************/

#include "../Gui/gui.h"

#include <stdio.h>
#include <unistd.h>
#include <vector>

#include "gui.h"
#include "Gui.hpp"
#include "debug.hpp"

#undef DBGLVL
#define DBGLVL DBG_ALL

lv_disp_drv_t disp_drv;
lv_indev_drv_t indev_drv;

/* Initialize a descriptor for the buffer */
static lv_disp_buf_t disp_buf;
/* A buffer for LVGL to draw the screen's content*/
static lv_color_t buf[DISP_BUF_SIZE];

static lv_style_t line_style;
static lv_style_t button_style;
static lv_style_t regular_button_style;
static lv_style_t pass_button_style;
static lv_style_t fail_button_style;
static lv_style_t reset_button_style;
static lv_style_t touch_button_style;
static lv_style_t title_label_style;
static lv_style_t regular_label_style;
static lv_style_t small_bold_label_style;
static lv_style_t regular_bold_label_style;
static lv_style_t large_bold_label_style;
static lv_style_t cell_style1;
static lv_style_t cell_style2;
static lv_style_t table_style;

static lv_point_t line_points[2] { {0, 120}, {480, 120} };

pthread_t lvgl_task;
pthread_t lvgl_tick;
pthread_mutex_t lvgl_lock = PTHREAD_MUTEX_INITIALIZER;

int GuiInit(void)
{
	DBGPRT(DBG_INFO4, "GuiInit: Started\n");

	/* LVGL init. */
	lv_init();

	/* Linux frame buffer device init. */
	fbdev_init();

	/* Initialize the touch driver. */
	evdev_init();

	/* Initialize a descriptor for the buffer. */
	lv_disp_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

	/* Initialize and register a display driver */
	lv_disp_drv_init(&disp_drv);
	disp_drv.buffer = &disp_buf;
	disp_drv.flush_cb = fbdev_flush;
	lv_disp_drv_register(&disp_drv);

	/* Initialize and register a touch driver. */
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = evdev_read;
	lv_indev_drv_register(&indev_drv);

	pthread_create(&lvgl_task, NULL, RunTaskThread, NULL);
	pthread_create(&lvgl_tick, NULL, RunTickThread, NULL);

	CreateScreen();

	DeclareStyles();

	DBGPRT(DBG_INFO4, "GuiInit: Finished\n");

	return 0;
}

void GuiCleanUp(void)
{
	pthread_mutex_lock(&lvgl_lock);
	pthread_cancel(lvgl_task);
	pthread_cancel(lvgl_tick);
	pthread_mutex_unlock(&lvgl_lock);
}

void * RunTaskThread(void *arg)
{
	UNUSED(arg);

	DBGPRT(DBG_INFO4, "RunTaskThread: starting tick thread\n");

	while (1)
	{
		uint32_t time_till_next;
		pthread_mutex_lock(&lvgl_lock);
		time_till_next = lv_task_handler();
		pthread_mutex_unlock(&lvgl_lock);
		usleep(time_till_next);
	}

	return NULL;
}

void * RunTickThread(void *arg)
{
	UNUSED(arg);

	DBGPRT(DBG_INFO4, "RunTickThread: starting tick thread\n");

	while (1)
	{
		usleep(5* 1000);
		pthread_mutex_lock(&lvgl_lock);
		lv_tick_inc(5);
		pthread_mutex_unlock(&lvgl_lock);
	}

	return NULL;
}

void DeclareStyles(void)
{
	pthread_mutex_lock(&lvgl_lock);

	lv_style_init(&line_style);
	lv_style_set_line_width(&line_style, LV_STATE_DEFAULT, 5);
	lv_style_set_line_color(&line_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_line_rounded(&line_style, LV_STATE_DEFAULT, false);

	lv_style_init(&button_style);
	lv_style_set_text_color(&button_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_style_set_text_color(&button_style, LV_STATE_PRESSED, LV_COLOR_WHITE);
	lv_style_set_bg_opa(&button_style, LV_STATE_DEFAULT, LV_OPA_100);
	lv_style_set_bg_opa(&button_style, LV_STATE_PRESSED, LV_OPA_50);
	lv_style_set_border_width(&button_style, LV_STATE_DEFAULT, 5);
	lv_style_set_border_color(&button_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_color(&button_style, LV_STATE_PRESSED, LV_COLOR_BLACK);
	lv_style_set_border_color(&button_style, LV_STATE_DISABLED, LV_COLOR_BLACK);
	lv_style_set_outline_width(&button_style, LV_STATE_DEFAULT, 0);

	lv_style_init(&regular_button_style);
	lv_style_copy(&regular_button_style, &button_style);
	lv_style_set_bg_color(&regular_button_style, LV_STATE_DEFAULT, LV_COLOR_BLUE);
	lv_style_set_bg_color(&regular_button_style, LV_STATE_PRESSED, LV_COLOR_BLUE);
	lv_style_set_bg_color(&regular_button_style, LV_STATE_DISABLED, LV_COLOR_BLUE);

	lv_style_init(&pass_button_style);
	lv_style_copy(&pass_button_style, &button_style);
	lv_style_set_bg_color(&pass_button_style, LV_STATE_DEFAULT, LV_COLOR_GREEN);
	lv_style_set_bg_color(&pass_button_style, LV_STATE_PRESSED, LV_COLOR_GREEN);
	lv_style_set_bg_color(&pass_button_style, LV_STATE_DISABLED, LV_COLOR_GREEN);

	lv_style_init(&fail_button_style);
	lv_style_copy(&fail_button_style, &button_style);
	lv_style_set_bg_color(&fail_button_style, LV_STATE_DEFAULT, LV_COLOR_RED);
	lv_style_set_bg_color(&fail_button_style, LV_STATE_PRESSED, LV_COLOR_RED);
	lv_style_set_bg_color(&fail_button_style, LV_STATE_DISABLED, LV_COLOR_RED);

	lv_style_init(&reset_button_style);
	lv_style_copy(&reset_button_style, &button_style);
	lv_style_set_bg_color(&reset_button_style, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
	lv_style_set_bg_color(&reset_button_style, LV_STATE_PRESSED, LV_COLOR_ORANGE);
	lv_style_set_bg_color(&reset_button_style, LV_STATE_DISABLED, LV_COLOR_ORANGE);

	lv_style_init(&touch_button_style);
	lv_style_set_bg_color(&touch_button_style, LV_STATE_DEFAULT, LV_COLOR_GREEN);
	lv_style_set_bg_color(&touch_button_style, LV_STATE_CHECKED, LV_COLOR_RED);
	lv_style_set_bg_color(&touch_button_style, LV_STATE_DISABLED, LV_COLOR_GREEN);
	lv_style_set_bg_opa(&button_style, LV_STATE_DEFAULT, LV_OPA_100);
	lv_style_set_bg_opa(&button_style, LV_STATE_CHECKED, LV_OPA_100);
	lv_style_set_bg_opa(&button_style, LV_STATE_DISABLED, LV_OPA_100);
	lv_style_set_bg_opa(&button_style, LV_STATE_PRESSED, LV_OPA_50);
	lv_style_set_border_width(&touch_button_style, LV_STATE_DEFAULT, 0);
	lv_style_set_border_width(&touch_button_style, LV_STATE_CHECKED, 0);
	lv_style_set_border_width(&touch_button_style, LV_STATE_DISABLED, 0);
	lv_style_set_outline_width(&touch_button_style, LV_STATE_DEFAULT, 0);
	lv_style_set_outline_width(&touch_button_style, LV_STATE_CHECKED, 0);
	lv_style_set_outline_width(&touch_button_style, LV_STATE_DISABLED, 0);

	lv_style_init(&title_label_style);
	lv_style_set_text_font(&title_label_style, LV_STATE_DEFAULT, &statstrip_bold_50);
	lv_style_set_text_color(&title_label_style, LV_STATE_DEFAULT, LV_COLOR_BLUE);

	lv_style_init(&regular_label_style);
	lv_style_set_text_font(&regular_label_style, LV_STATE_DEFAULT, &statstrip_reg_30);

	lv_style_init(&small_bold_label_style);
	lv_style_set_text_font(&small_bold_label_style, LV_STATE_DEFAULT, &statstrip_bold_30);

	lv_style_init(&regular_bold_label_style);
	lv_style_set_text_font(&regular_bold_label_style, LV_STATE_DEFAULT, &statstrip_bold_40);

	lv_style_init(&large_bold_label_style);
	lv_style_set_text_font(&large_bold_label_style, LV_STATE_DEFAULT, &statstrip_bold_50);

	lv_style_init(&table_style);
	lv_style_set_text_font(&table_style, LV_STATE_DISABLED, &statstrip_bold_25);
	lv_style_set_bg_opa(&table_style, LV_STATE_DISABLED, LV_OPA_COVER);
	lv_style_set_bg_color(&table_style, LV_STATE_DISABLED, LV_COLOR_MAKE(0x80, 0x80, 0xC0));//LV_COLOR_MAKE(105, 105, 105)
	lv_style_set_border_color(&table_style, LV_STATE_DISABLED, LV_COLOR_BLACK);
	lv_style_set_border_width(&table_style, LV_STATE_DISABLED, 3);

	lv_style_init(&cell_style1);
	lv_style_set_bg_opa(&table_style, LV_STATE_DISABLED, LV_OPA_COVER);
	lv_style_set_text_font(&cell_style1, LV_STATE_DEFAULT, &statstrip_bold_25);
	lv_style_set_text_color(&cell_style1, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_style_set_border_side(&cell_style1, LV_STATE_DEFAULT, LV_BORDER_SIDE_NONE);
	lv_style_set_border_width(&cell_style1, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&cell_style1, LV_STATE_DEFAULT, 10);
	lv_style_set_pad_bottom(&cell_style1, LV_STATE_DEFAULT, 10);

	lv_style_init(&cell_style2);
	lv_style_set_bg_opa(&table_style, LV_STATE_DISABLED, LV_OPA_COVER);
	lv_style_set_text_font(&cell_style2, LV_STATE_DEFAULT, &statstrip_bold_25);
	lv_style_set_text_color(&cell_style2, LV_STATE_DEFAULT, LV_COLOR_YELLOW);
	lv_style_set_border_side(&cell_style2, LV_STATE_DEFAULT, LV_BORDER_SIDE_NONE);
	lv_style_set_border_width(&cell_style2, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&cell_style2, LV_STATE_DEFAULT, 10);
	lv_style_set_pad_bottom(&cell_style2, LV_STATE_DEFAULT, 10);

	pthread_mutex_unlock(&lvgl_lock);
}

void CreateScreen()
{
	DBGPRT(DBG_INFO4, "CreateScreen: started\n");

	static lv_style_t screen_style;

	pthread_mutex_lock(&lvgl_lock);

	lv_style_init(&screen_style);
	lv_style_set_bg_color(&screen_style, LV_STATE_DEFAULT, NOVA_COLOR_BKGND);
	lv_style_set_bg_opa(&button_style, LV_STATE_DEFAULT, 255);
	lv_style_set_border_width(&screen_style, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_bg_opa(lv_scr_act(), LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);

	lv_obj_t * new_screen = lv_obj_create(NULL, NULL);
	lv_obj_add_style(new_screen, LV_STATE_DEFAULT, &screen_style);

	lv_scr_load(new_screen);

	pthread_mutex_unlock(&lvgl_lock);

	DBGPRT(DBG_INFO4, "CreateScreen: complete\n");
}

void ClearScreen()
{
	DBGPRT(DBG_INFO4, "ClearScreen: started\n");

	pthread_mutex_lock(&lvgl_lock);
	lv_obj_clean(lv_scr_act());
	pthread_mutex_unlock(&lvgl_lock);

	DBGPRT(DBG_INFO4, "ClearScreen: complete\n");
}

void DrawQuadGridDisplay()
{
	DBGPRT(DBG_INFO4, "DrawQuadGridDisplay: started\n");

	static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT)];

	pthread_mutex_lock(&lvgl_lock);

	lv_obj_t * canvas = lv_canvas_create(lv_scr_act(), NULL);
	lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
	lv_canvas_fill_bg(canvas, NOVA_COLOR_BKGND, 1);

	pthread_mutex_unlock(&lvgl_lock);

	DBGPRT(DBG_INFO4, "DrawQuadGridDisplay: Canvas created\n");

	pthread_mutex_lock(&lvgl_lock);

	// Draw the top left rectangle in red.
	lv_draw_rect_dsc_t top_left_rect_dsc;
	lv_draw_rect_dsc_init(&top_left_rect_dsc);
	top_left_rect_dsc.bg_opa = LV_OPA_COVER;
	top_left_rect_dsc.bg_color = LV_COLOR_RED;

	lv_draw_label_dsc_t red_label;
	lv_draw_label_dsc_init(&red_label);
	red_label.color = LV_COLOR_WHITE;
	red_label.font  = &statstrip_bold_40;

	lv_canvas_draw_rect(canvas, 0, 0, 240, 400, &top_left_rect_dsc);
	lv_canvas_draw_text(canvas, 0, 180, 240, &red_label, "RED", LV_LABEL_ALIGN_CENTER);

	pthread_mutex_unlock(&lvgl_lock);

	usleep(250000);

	pthread_mutex_lock(&lvgl_lock);

	// Draw the top right rectangle in green.
	lv_draw_rect_dsc_t top_right_rect_dsc;
	lv_draw_rect_dsc_init(&top_right_rect_dsc);
	top_right_rect_dsc.bg_opa = LV_OPA_COVER;
	top_right_rect_dsc.bg_color = LV_COLOR_GREEN;

	lv_draw_label_dsc_t green_label;
	lv_draw_label_dsc_init(&green_label);
	green_label.color = LV_COLOR_WHITE;
	green_label.font  = &statstrip_bold_40;

	lv_canvas_draw_rect(canvas, 240, 0, 240, 400, &top_right_rect_dsc);
    lv_canvas_draw_text(canvas, 240, 180, 240, &green_label, "GREEN", LV_LABEL_ALIGN_CENTER);

	pthread_mutex_unlock(&lvgl_lock);

    usleep(250000);

	pthread_mutex_lock(&lvgl_lock);

	// Draw the bottom left rectangle in blue.
	lv_draw_rect_dsc_t bottom_left_rect_dsc;
	lv_draw_rect_dsc_init(&bottom_left_rect_dsc);
	bottom_left_rect_dsc.bg_opa = LV_OPA_COVER;
	bottom_left_rect_dsc.bg_color = LV_COLOR_BLUE;
	lv_draw_label_dsc_t blue_label;

	lv_draw_label_dsc_init(&blue_label);
	blue_label.color = LV_COLOR_WHITE;
	blue_label.font  = &statstrip_bold_40;

	lv_canvas_draw_rect(canvas, 0, 400, 240, 400, &bottom_left_rect_dsc);
	lv_canvas_draw_text(canvas, 0, 575, 240, &blue_label, "BLUE", LV_LABEL_ALIGN_CENTER);

	pthread_mutex_unlock(&lvgl_lock);

	usleep(250000);

	pthread_mutex_lock(&lvgl_lock);

	// Draw the bottom right rectangle in yellow.
	lv_draw_rect_dsc_t bottom_right_rect_dsc;
	lv_draw_rect_dsc_init(&bottom_right_rect_dsc);
	bottom_right_rect_dsc.bg_opa = LV_OPA_COVER;
	bottom_right_rect_dsc.bg_color = LV_COLOR_YELLOW;

	lv_draw_label_dsc_t yellow_label;
	lv_draw_label_dsc_init(&yellow_label);
	yellow_label.color = LV_COLOR_BLACK;
	yellow_label.font  = &statstrip_bold_40;

    lv_canvas_draw_rect(canvas, 240, 400, 240, 400, &bottom_right_rect_dsc);
    lv_canvas_draw_text(canvas, 240, 575, 240, &yellow_label, "YELLOW", LV_LABEL_ALIGN_CENTER);

	pthread_mutex_unlock(&lvgl_lock);

	DBGPRT(DBG_INFO4, "DrawQuadGridDisplay: Rectangles & Labels added\n");

	sleep(1);

	pthread_mutex_lock(&lvgl_lock);

	lv_obj_set_hidden(canvas, true);
	lv_obj_del(canvas);
	canvas = NULL;

	pthread_mutex_unlock(&lvgl_lock);

	DBGPRT(DBG_INFO4, "DrawQuadGridDisplay: Canvas deleted\n");
}

lv_obj_t * AddButton(GuiObj *btn)
{
	DBGPRT(DBG_INFO4, "AddButton: name = %s, x = %d, y = %d\n", btn->text.c_str(), btn->x, btn->y);

	pthread_mutex_lock(&lvgl_lock);

	lv_obj_t * button = lv_btn_create(lv_scr_act(), NULL);
	lv_obj_set_event_cb(button, btn->cb);
	lv_obj_set_pos(button, btn->x, btn->y);
	lv_obj_set_size(button, btn->w, btn->h);

	if (btn->text.find("PASS") != std::string::npos)
	{
		lv_obj_add_style(button, LV_BTN_PART_MAIN, &pass_button_style);
	}
	else if (btn->text.find("FAIL") != std::string::npos)
	{
		lv_obj_add_style(button, LV_BTN_PART_MAIN, &fail_button_style);
	}
	else if (btn->text.find("RESET") != std::string::npos)
	{
		lv_obj_add_style(button, LV_BTN_PART_MAIN, &reset_button_style);
	}
	else
	{
		lv_obj_add_style(button, LV_BTN_PART_MAIN, &regular_button_style);
	}


	lv_obj_t * label = lv_label_create(button, NULL);
	lv_obj_set_style_local_text_font(label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &statstrip_bold_40);
	lv_label_set_text(label, btn->text.c_str());
	lv_obj_set_state(label, LV_STATE_DEFAULT);

	if (btn->btn_toggle)
	{
		lv_btn_set_checkable(button, true);
		lv_btn_toggle(button);
	}

	lv_obj_set_user_data(button, (void *)btn->text.c_str());

	lv_obj_set_state(button, LV_STATE_DEFAULT);

	pthread_mutex_unlock(&lvgl_lock);

	return button;
}

lv_obj_t * AddTouchButton(GuiObj *btn)
{
	DBGPRT(DBG_INFO4, "AddTouchButton: x = %d, y = %d\n", btn->x, btn->y);

	pthread_mutex_lock(&lvgl_lock);

	lv_obj_t * button = lv_btn_create(lv_scr_act(), NULL);
	lv_obj_set_event_cb(button, btn->cb);
    lv_obj_set_pos(button, btn->x, btn->y);
	lv_obj_set_size(button, btn->w, btn->h);
	lv_btn_set_checkable(button, true);

	lv_obj_set_user_data(button, (void *)&btn->obj_id);

	lv_obj_add_style(button, LV_BTN_PART_MAIN, &touch_button_style);

	lv_obj_set_state(button, LV_STATE_CHECKED);

	pthread_mutex_unlock(&lvgl_lock);

	return button;
}

void DisableButton(lv_obj_t * obj)
{
	DBGPRT(DBG_INFO4, "DisableButton: disabled\n");

	//pthread_mutex_lock(&lvgl_lock);
	lv_obj_set_state(obj, LV_STATE_DISABLED);
	//pthread_mutex_unlock(&lvgl_lock);
}

lv_obj_t * AddTitle(GuiObj title, char* text)
{
	DBGPRT(DBG_INFO4, "AddTitle: Started adding - [%s]\n", text);

	pthread_mutex_lock(&lvgl_lock);

	title.label = lv_label_create(lv_scr_act(), NULL);

	DBGPRT(DBG_INFO4, "AddTitle: title created\n");

	lv_label_set_long_mode(title.label, LV_LABEL_LONG_BREAK);
	lv_label_set_align(title.label, LV_LABEL_ALIGN_CENTER);
	lv_label_set_text(title.label, text);
	lv_obj_set_size(title.label, CANVAS_WIDTH, 60);
	lv_obj_align(title.label, NULL, LV_ALIGN_IN_TOP_MID, 0, title.y);

	DBGPRT(DBG_INFO4, "AddTitle: attributes set \n");

	lv_obj_set_style_local_bg_color(title.label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, NOVA_COLOR_BKGND);
	lv_obj_set_style_local_bg_opa(title.label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
	lv_obj_set_style_local_text_opa(title.label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
	lv_obj_set_style_local_text_font(title.label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &statstrip_bold_50);
	lv_obj_set_style_local_text_color(title.label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLUE);

	DBGPRT(DBG_INFO4, "AddTitle: Style set\n");

	usleep(5000);

	lv_obj_t * line = lv_line_create(lv_scr_act(), NULL);
	lv_line_set_points(line, line_points, 2);
	lv_obj_add_style(line, LV_LINE_PART_MAIN, &line_style);

	DBGPRT(DBG_INFO4, "AddTitle: Completed\n");

	pthread_mutex_unlock(&lvgl_lock);

	return title.label;
}

lv_obj_t * AddLabel(GuiObj obj, char* text)
{
	DBGPRT(DBG_INFO4, "AddLabel: Started adding - [%s]\n", text);

	pthread_mutex_lock(&lvgl_lock);

	obj.label = lv_label_create(lv_scr_act(), NULL);

	DBGPRT(DBG_INFO4, "AddLabel: label created\n");

	lv_label_set_long_mode(obj.label, LV_LABEL_LONG_BREAK);
	lv_label_set_recolor(obj.label, true);
	lv_label_set_align(obj.label, obj.text_align);

	DBGPRT(DBG_INFO4, "AddLabel: label attributes set\n");

	lv_label_set_text_static(obj.label, text);

	DBGPRT(DBG_INFO4, "AddLabel: text added\n");

	lv_obj_set_width(obj.label, obj.w);
	lv_obj_set_pos(obj.label, obj.x, obj.y);

	DBGPRT(DBG_INFO4, "AddLabel: attributes set, adding style\n");

	lv_obj_set_style_local_text_opa(obj.label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
	lv_obj_set_style_local_text_font(obj.label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, obj.font);

	if (obj.set_color)
	{
		lv_obj_set_style_local_text_color(obj.label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, obj.color);
	}

	if (obj.hidden)
	{
		lv_obj_set_hidden(obj.label, true);
	}

	pthread_mutex_unlock(&lvgl_lock);

	DBGPRT(DBG_INFO4, "AddLabel: Completed\n");

	return obj.label;
}

void AddDirectionLabel()
{
	DBGPRT(DBG_INFO4, "AddDirectionsLabel: [%s]\n", "Directions:");

	pthread_mutex_lock(&lvgl_lock);

	lv_obj_t * directions = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_long_mode(directions, LV_LABEL_LONG_BREAK);
	lv_label_set_align(directions, LV_LABEL_ALIGN_LEFT);
	lv_label_set_text_static(directions, "Directions:");
	lv_obj_set_size(directions, CANVAS_WIDTH, 35);
	lv_obj_set_pos(directions, 35, 250);
	lv_obj_set_style_local_text_font(directions, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &statstrip_bold_40);

	pthread_mutex_unlock(&lvgl_lock);
}

lv_obj_t * AddCountdownLabel(GuiObj ta, char *text)
{
	DBGPRT(DBG_INFO4, "AddCountdownLabel: [%s]\n", text);

	pthread_mutex_lock(&lvgl_lock);

	lv_obj_t * countdown = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_long_mode(countdown, LV_LABEL_LONG_BREAK);
	lv_label_set_align(countdown, ta.text_align);
	lv_label_set_text_static(countdown, text);
	lv_obj_set_width(countdown, ta.w);
	lv_obj_set_pos(countdown, ta.x, ta.y);

	lv_obj_set_style_local_text_font(countdown, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, ta.font);

	pthread_mutex_unlock(&lvgl_lock);

	return countdown;
}

void ChangeLabel(GuiObj label, char *text)
{
	DBGPRT(DBG_INFO4, "ChangeLabel: Changing [%s] to [%s]\n", (char*)label.text.c_str(), text);

	pthread_mutex_lock(&lvgl_lock);

	lv_label_set_text_static(label.obj, text);

	if (label.set_color)
	{
		lv_obj_set_style_local_text_color(label.obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, label.color);
	}

	pthread_mutex_unlock(&lvgl_lock);
}

lv_obj_t * AddTable(GuiTable guiTable, std::vector<std::vector<std::string>> values, bool wasTestRun, bool didTestsPass)
{
	DBGPRT(DBG_INFO4, "AddTable: Results table started\n");

	pthread_mutex_lock(&lvgl_lock);

	lv_obj_t * table = lv_table_create(lv_scr_act(), NULL);
	lv_table_set_col_cnt(table, guiTable.c);
	lv_table_set_row_cnt(table, guiTable.r);
	lv_table_set_col_width(table, 0, 245);
	lv_table_set_col_width(table, 1, 175);

	DBGPRT(DBG_INFO4, "AddTable: table created\n");

	lv_obj_align(table, NULL, LV_ALIGN_CENTER, 0, 5);

	DBGPRT(DBG_INFO4, "AddTable: aligned\n");

	const char* header1 = (char*)values[0][0].c_str();
	const char* header2 = (char*)values[0][1].c_str();
	lv_table_set_cell_type(table, 0, 0, didTestsPass ? LV_TABLE_PART_CELL1 : LV_TABLE_PART_CELL2);
	lv_table_set_cell_type(table, 0, 1, didTestsPass ? LV_TABLE_PART_CELL1 : LV_TABLE_PART_CELL2);
	lv_table_set_cell_align(table, 0, 0, LV_LABEL_ALIGN_CENTER);
	lv_table_set_cell_align(table, 0, 1, LV_LABEL_ALIGN_CENTER);
	lv_table_set_cell_value(table, 0, 0, header1);
	lv_table_set_cell_value(table, 0, 1, header2);

	DBGPRT(DBG_INFO4, "AddTable: headers set\n");

	std::string result;
	const char* value1;
	const char* value2;

	DBGPRT(DBG_INFO4, "AddTable: now processing values\n");

	for (int i = 1; i < guiTable.r; i++)
	{
		result = values[i][1];
		value1 = (char*)values[i][0].c_str();
		value2 = (char*)values[i][1].c_str();

		DBGPRT(DBG_INFO4, "AddTable: values retrieved\n");

		lv_table_set_cell_align(table, i, 0, LV_LABEL_ALIGN_LEFT);
		lv_table_set_cell_align(table, i, 1, LV_LABEL_ALIGN_CENTER);

		DBGPRT(DBG_INFO4, "AddTable: align set\n");

		if ((wasTestRun) &&  (result.compare("PASSED") == 0))
		{
			lv_table_set_cell_type(table, i, 0, LV_TABLE_PART_CELL1);
			lv_table_set_cell_type(table, i, 1, LV_TABLE_PART_CELL1);
		}
		else
		{
			lv_table_set_cell_type(table, i, 0, LV_TABLE_PART_CELL2);
			lv_table_set_cell_type(table, i, 1, LV_TABLE_PART_CELL2);
		}

		DBGPRT(DBG_INFO4, "AddTable: cell type set\n");

		lv_table_set_cell_value(table, i, 0, value1);
		lv_table_set_cell_value(table, i, 1, value2);

		DBGPRT(DBG_INFO4, "AddTable: value1 = %s, value2 = %s, i = %d\n", value1, value2, i);

		result = "";
		value1 = "";
		value2 = "";
	}

	DBGPRT(DBG_INFO4, "AddTable: Cells added\n");

	lv_obj_add_style(table, LV_TABLE_PART_BG, &table_style);
	lv_obj_add_style(table, LV_TABLE_PART_CELL1, &cell_style1);
	lv_obj_add_style(table, LV_TABLE_PART_CELL2, &cell_style2);

	DBGPRT(DBG_INFO4, "AddTable: Styles added\n");

	if (wasTestRun)
	{
		//lv_table_set_cell_type(table, 0, 0, fail_found ? LV_TABLE_PART_CELL2 : LV_TABLE_PART_CELL1);
		//lv_table_set_cell_type(table, 0, 1, fail_found ? LV_TABLE_PART_CELL2 : LV_TABLE_PART_CELL1);

		lv_obj_set_style_local_text_color(table, LV_TABLE_PART_CELL1, LV_STATE_DEFAULT, LV_COLOR_LIME);
		lv_obj_set_style_local_text_color(table, LV_TABLE_PART_CELL2, LV_STATE_DEFAULT, LV_COLOR_RED);

		DBGPRT(DBG_INFO4, "AddTable: Pass/Fail styles added\n");
	}

	lv_obj_set_state(table, LV_STATE_DISABLED);

	DBGPRT(DBG_INFO4, "AddTable: Result Table completed\n");

	pthread_mutex_unlock(&lvgl_lock);

	return table;
}

lv_obj_t * SetGuiObjVisibility(GuiObj guiObj, bool hide)
{
	DBGPRT(DBG_INFO4, "SetGuiObjVisibility: [%s]\n", (char*)guiObj.text.c_str());

	pthread_mutex_lock(&lvgl_lock);
	lv_obj_set_hidden(guiObj.obj, hide);
	pthread_mutex_unlock(&lvgl_lock);

	return NULL;
}

lv_obj_t * RemoveGuiObj(GuiObj guiObj)
{
	DBGPRT(DBG_INFO4, "RemoveGuiObj: [%s]\n", (char*)guiObj.text.c_str());

	pthread_mutex_lock(&lvgl_lock);
	lv_obj_set_hidden(guiObj.obj, true);
	lv_obj_del(guiObj.obj);
	pthread_mutex_unlock(&lvgl_lock);

	return NULL;
}

