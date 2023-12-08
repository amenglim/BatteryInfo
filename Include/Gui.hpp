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


#pragma once

#include <lvgl.h>
#include <fbdev.h>
#include <evdev.h>
#include <string>
#include <vector>

#define DISP_BUF_SIZE           (LV_HOR_RES_MAX * LV_VER_RES_MAX * LV_COLOR_DEPTH)
#define CANVAS_WIDTH            LV_HOR_RES_MAX//(LV_HOR_RES_MAX - 1)
#define CANVAS_HEIGHT           LV_VER_RES_MAX//(LV_VER_RES_MAX - 1)
#define HALF_SCREEN				(CANVAS_WIDTH / 2)
#define HALF_SCREEN_Y			(CANVAS_HEIGHT / 2)
#define TOOLBAR_HEIGHT          (60)
#define MAX_T_IDS               (10)
#define BRDR_WIDTH              (10)
#define BTN_WIDTH               (220)
#define LARGE_BTN_WIDTH         (330)
#define BTN_HEIGHT              (100)
#define STRT_X                  ((CANVAS_WIDTH / 2) - (BTN_WIDTH / 2))
#define STRT_Y                  (CANVAS_HEIGHT - BTN_HEIGHT - BRDR_WIDTH)
#define LEFT_BTN_X   			(13)
#define RIGHT_BTN_X				(246)
#define NOVA_COLOR_BKGND        LV_COLOR_MAKE(0xE8, 0xE8, 0xE8)
#define TOUCHSCREEN				"/dev/input/event0"

typedef void (*callback)(lv_obj_t *obj, lv_event_t event);

struct GuiImage
{
	const void * image;
    lv_obj_t * obj;
    lv_color_t color;
    int x;
    int y;
    int w;
    int h;
};

struct GuiObj
{
    lv_obj_t * obj;
    lv_obj_t * label;
    lv_font_t * font;
    lv_label_align_t text_align;
    bool hidden = false;
    bool set_color = false;
    lv_color_t color;
    bool btn_toggle;
    callback cb;
    std::string text;
    uint32_t obj_id;
    lv_coord_t x;
    lv_coord_t y;
    lv_coord_t w;
    lv_coord_t h;
};

struct GuiTable
{
	lv_obj_t * obj;
	lv_font_t * font;
	lv_align_t table_align;
	lv_label_align_t text_align;
	uint16_t r;
	uint16_t c;
	lv_coord_t x;
	lv_coord_t y;
	lv_coord_t w;
	lv_coord_t h;
};

enum {
    LV_STATE_PASSED  =  0x30,
    LV_STATE_FAILED =  0x40,
};

typedef uint8_t lv_state_t;

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * GuiInit
     * init LVGL library
     */
    int GuiInit(void);

    /**
     * GuiCleanUp
     * cancels the GUI threads.
     */
    void GuiCleanUp(void);

    /*
     * RunTaskThread
     * runs the lvgl handler in main while(1) loop
     */
    void * RunTaskThread(void *arg);

    /*
	 * RunTickThread
	 * runs the lvgl tick in its own while(1) loop
	 */
    void * RunTickThread(void *arg);

    /**
     * DelareStyles
     * declares the styles that are used throughout the app.
     */
    void DeclareStyles(void);

    /**
	 * CreateScreen
	 * creates a new screen
	 * screen - the new screen to be created.
	 */
    void CreateScreen();

    /**
	 * ClearScreen
	 * deletes all children on the current screen
	 */
    void ClearScreen();

    /**
     * DrawQuadGridDisplay
     * This will draw four different colored rectangles
     * in a quadruple grid for the LCD test.
     */
    void DrawQuadGridDisplay();

    /**
     * AddButton
     * adds button to screen
     * btn - button object used to describe button
     */
    lv_obj_t * AddButton(GuiObj *btn);

    /**
	 * AddTouchButton
	 * adds circle button for touch test to screen
	 * btn - button object used to describe button
	 */
    lv_obj_t * AddTouchButton(GuiObj *btn);

    /**
	 * DisableButton
	 * sets checkable property to false
	 * btn - button object used to describe button
	 */
    void DisableButton(lv_obj_t * obj);

    /**
	 * AddTitle
	 * creates a label at the top of the screen
	 * title  - label object
	 * text   - text to display
	 */
    lv_obj_t * AddTitle(GuiObj title, char *text);

    /**
     * AddLabel
     * creates a label on the screen
     * obj   - label object
     * text  - text to display
     */
	lv_obj_t * AddLabel(GuiObj obj, char *text);

    /**
	 * AddDirectionsLabel
	 * creates a label on the screen
	 */
	void AddDirectionLabel();

    /**
     * ChangeLabel
     * changes text of active label
     * obj   - label object
     * text  - text to display
     */
    void ChangeLabel(GuiObj label, char *text);

    /**
     * AddCountdownLabel
     * creates a label on the screen
     * obj   - label object
     * text  - text to display
     */
    lv_obj_t * AddCountdownLabel(GuiObj ta, char *text);

    /**
     * AddTable
     * adds a table to the screen
     * table	- table to be added.
     */
    lv_obj_t * AddTable(GuiTable guiTable, std::vector<std::vector<std::string>> values, bool wasTestRun, bool didTestsPass);

    /**
     * SetGuiObjVisibility
     * sets the visibility of the obj
     * guiObj    - the object to be hidden or not
     * hide      - true if hide, false if not
     */
    lv_obj_t * SetGuiObjVisibility(GuiObj guiObj, bool hide);

    /**
     * RemoveGuiObj
     * removes provided object
     * guiObj	- object to be removed
     */
    lv_obj_t * RemoveGuiObj(GuiObj guiObj);

#ifdef __cplusplus
}
#endif





