/*******************************************************************************
 *                         COPYRIGHT NOTICE
 *                   "Copyright 2023 Nova Biomedical Corporation"
 *             This program is the property of Nova Biomedical Corporation
 *                 200 Prospect Street, Waltham, MA 02454-9141
 *             Any unauthorized use or duplication is prohibited
 ********************************************************************************
 *
 *  Title            - core
 *  Source Filename  - core.cpp
 *  Author           - Anthony Meng-Lim
 *  Description      - Main file to run Battery Info app.
 *
 *******************************************************************************/

#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <dlfcn.h>
#include <gpiod.h>

#include "Audio.h"
#include "core.h"
#include "Gui.hpp"
#include "Battery.hpp"

#undef DBGLVL
#define DBGLVL DBG_ALL

GuiObj title;
GuiObj header;
GuiObj battery_temp_label;
GuiObj battery_temp;
GuiObj battery_health_label;
GuiObj battery_health;
GuiObj battery_level_label;
GuiObj battery_level;
GuiObj in_dock_label;
GuiObj in_dock;
GuiObj charging_label;
GuiObj charging;
GuiObj brightness_label;
GuiObj brightness_value;
GuiObj current_label;
GuiObj current;
GuiObj voltage_label;
GuiObj voltage;
GuiObj increase_btn;
GuiObj decrease_btn;
GuiObj reset_btn;
pthread_mutex_t start_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t start_cond1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t adjust_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t adjust_cond1 = PTHREAD_COND_INITIALIZER;

const char PLUGIN_PATH[]    = "/usr/nova/battery_info/plugins/";
const char LIB_PREFIX[]     = "libdiag.";
const char PLUGIN_SUFFIX[]  = ".pi.so";
const char INIT_SUFFIX[]    = "_init";
const char START_SUFFIX[]   = "_start";
const char END_SUFFIX[]     = "_end";
int brightness = 60;

void SetScreenBrightness(int brightness)
{
	std::string brightness_string = std::to_string(brightness);
	std::string command = "echo " + brightness_string + " >> /sys/class/backlight/lcd-backlight/brightness";
	system((char*)command.c_str());
	brightness_value.text = brightness_string;
}

void adjust_brightness_btn_callback(lv_obj_t *obj, lv_event_t event)
{
	if(event == LV_EVENT_CLICKED)
	{
		std::string id = (char*)lv_obj_get_user_data(obj);

		Audio_PlayTrack(KeyPress);

		if(decrease_btn.btn_toggle)
		{
			DBGPRT(DBG_INFO1, "Adjust Brightness: decrease clicked at display, %s\n", id.c_str());
			brightness--;
			SetScreenBrightness(brightness);
		}
		if(increase_btn.btn_toggle)
		{
			DBGPRT(DBG_INFO1, "Adjust Brightness: decrease clicked at display, %s\n", id.c_str());
			brightness++;
			SetScreenBrightness(brightness);
		}
		pthread_mutex_lock(&adjust_lock);
		pthread_cond_signal(&adjust_cond1);
		pthread_mutex_unlock(&adjust_lock);
	}
}

void AddAdjustBrightness()
{
	// Setup Decrease.
	decrease_btn.font      = &statstrip_bold_40;
	decrease_btn.text      = "-";
	decrease_btn.cb        = adjust_brightness_btn_callback;
	decrease_btn.obj_id    = 9;
	decrease_btn.color	   = LV_COLOR_RED;
	decrease_btn.x         = 13;
	decrease_btn.y         = STRT_Y;
	decrease_btn.w         = BTN_WIDTH;
	decrease_btn.h         = BTN_HEIGHT;
	decrease_btn.obj	   = AddButton(&decrease_btn);

	usleep(1000);

	// Setup Increase.
	increase_btn.font      = &statstrip_bold_40;
	increase_btn.text      = "+";
	increase_btn.cb        = adjust_brightness_btn_callback;
	increase_btn.obj_id    = 9;
	increase_btn.color	   = LV_COLOR_GREEN;
	increase_btn.x         = 246;
	increase_btn.y         = STRT_Y;
	increase_btn.w         = BTN_WIDTH;
	increase_btn.h         = BTN_HEIGHT;
	increase_btn.obj	   = AddButton(&increase_btn);
}

void *adjust_brightness(void *arg)
{
	UNUSED(arg);

	Audio_PlayTrack(QuickThree);

	sleep(2);

	AddAdjustBrightness();

	return NULL;
}

void *battery_monitor(void *arg)
{
	UNUSED(arg);

	title.text   = "Battery Monitor";
	title.y      = 35;
	AddTitle(title, (char*)title.text.c_str());

	char *version = (char*)arg;
	std::string ver = version;

	while (1)
	{

		DBGPRT(DBG_INFO1, "battery_monitor: started\n");

		usleep(250000);

		pthread_t batteryMonitor;
		pthread_t adjustBrightness;
		lv_coord_t right_column = HALF_SCREEN - 35;

		pthread_create(&batteryMonitor, NULL, RunBatteryMonitor, NULL);
		pthread_create(&adjustBrightness, NULL, adjust_brightness, NULL);

		usleep(10000);

		current_label.font        = &statstrip_reg_40;
		current_label.text    	 = "Current:";
		current_label.text_align  = LV_LABEL_ALIGN_RIGHT;
		current_label.x           = 0;
		current_label.y           = 175;
		current_label.w           = right_column;
		current_label.h           = 50;
		AddLabel(current_label, (char*)current_label.text.c_str());

		usleep(10000);

		current.font        = &statstrip_reg_40;
		current.text        = "Checking...";
		current.text_align  = LV_LABEL_ALIGN_CENTER;
		current.set_color   = true;
		current.color       = LV_COLOR_ORANGE;
		current.x           = 240;
		current.y           = 175;
		current.w           = HALF_SCREEN;
		current.h           = 50;
		current.obj         = AddLabel(current, (char*)current.text.c_str());

		usleep(10000);

		voltage_label.font        = &statstrip_reg_40;
		voltage_label.text    	 = "Voltage:";
		voltage_label.text_align  = LV_LABEL_ALIGN_RIGHT;
		voltage_label.x           = 0;
		voltage_label.y           = 225;
		voltage_label.w           = right_column;
		voltage_label.h           = 50;
		AddLabel(voltage_label, (char*)voltage_label.text.c_str());

		usleep(10000);

		voltage.font        = &statstrip_reg_40;
		voltage.text        = "Checking...";
		voltage.text_align  = LV_LABEL_ALIGN_CENTER;
		voltage.set_color   = true;
		voltage.color       = LV_COLOR_ORANGE;
		voltage.x           = 240;
		voltage.y           = 225;
		voltage.w           = HALF_SCREEN;
		voltage.h           = 50;
		voltage.obj         = AddLabel(voltage, (char*)voltage.text.c_str());

		usleep(10000);

		battery_health_label.font        = &statstrip_reg_40;
		battery_health_label.text        = "Health:";
		battery_health_label.text_align  = LV_LABEL_ALIGN_RIGHT;
		battery_health_label.x           = 0;
		battery_health_label.y           = 275;
		battery_health_label.w           = right_column;
		battery_health_label.h           = 50;
		AddLabel(battery_health_label, (char*)battery_health_label.text.c_str());

		usleep(10000);

		battery_health.font        = &statstrip_reg_40;
		battery_health.text        = "Checking...";
		battery_health.text_align  = LV_LABEL_ALIGN_CENTER;
		battery_health.set_color   = true;
		battery_health.color       = LV_COLOR_ORANGE;
		battery_health.x           = 240;
		battery_health.y           = 275;
		battery_health.w           = HALF_SCREEN;
		battery_health.h           = 50;
		battery_health.obj         = AddLabel(battery_health, (char*)battery_health.text.c_str());

		usleep(10000);

		battery_temp_label.font        = &statstrip_reg_40;
		battery_temp_label.text        = "Temp:";
		battery_temp_label.text_align  = LV_LABEL_ALIGN_RIGHT;
		battery_temp_label.x           = 0;
		battery_temp_label.y           = 325;
		battery_temp_label.w           = right_column;
		battery_temp_label.h           = 50;
		AddLabel(battery_temp_label, (char*)battery_temp_label.text.c_str());

		usleep(10000);

		battery_temp.font        = &statstrip_reg_40;
		battery_temp.text        = "Checking...";
		battery_temp.text_align  = LV_LABEL_ALIGN_CENTER;
		battery_temp.set_color   = true;
		battery_temp.color       = LV_COLOR_ORANGE;
		battery_temp.x           = 240;
		battery_temp.y           = 325;
		battery_temp.w           = HALF_SCREEN;
		battery_temp.h           = 50;
		battery_temp.obj 		 = AddLabel(battery_temp, (char*)battery_temp.text.c_str());

		usleep(10000);

		battery_level_label.font        = &statstrip_reg_40;
		battery_level_label.text        = "Level:";
		battery_level_label.text_align  = LV_LABEL_ALIGN_RIGHT;
		battery_level_label.x           = 0;
		battery_level_label.y           = 375;
		battery_level_label.w           = right_column;
		battery_level_label.h           = 50;
		AddLabel(battery_level_label, (char*)battery_level_label.text.c_str());

		usleep(10000);

		battery_level.font        = &statstrip_reg_40;
		battery_level.text        = "Checking...";
		battery_level.text_align  = LV_LABEL_ALIGN_CENTER;
		battery_level.set_color   = true;
		battery_level.color       = LV_COLOR_ORANGE;
		battery_level.x           = 240;
		battery_level.y           = 375;
		battery_level.w           = HALF_SCREEN;
		battery_level.h           = 50;
		battery_level.obj		  = AddLabel(battery_level, (char*)battery_level.text.c_str());

		usleep(10000);

		in_dock_label.font        = &statstrip_reg_40;
		in_dock_label.text        = "In Dock:";
		in_dock_label.text_align  = LV_LABEL_ALIGN_RIGHT;
		in_dock_label.x           = 0;
		in_dock_label.y           = 425;
		in_dock_label.w           = right_column;
		in_dock_label.h           = 50;
		AddLabel(in_dock_label, (char*)in_dock_label.text.c_str());

		usleep(10000);

		in_dock.font        = &statstrip_reg_40;
		in_dock.text        = "Checking...";
		in_dock.text_align  = LV_LABEL_ALIGN_CENTER;
		in_dock.set_color   = true;
		in_dock.color       = LV_COLOR_ORANGE;
		in_dock.x           = 240;
		in_dock.y           = 425;
		in_dock.w           = HALF_SCREEN;
		in_dock.h           = 50;
		in_dock.obj			= AddLabel(in_dock, (char*)in_dock.text.c_str());

		usleep(10000);

		charging_label.font        = &statstrip_reg_40;
		charging_label.text        = "Charging:";
		charging_label.text_align  = LV_LABEL_ALIGN_RIGHT;
		charging_label.x           = 0;
		charging_label.y           = 475;
		charging_label.w           = right_column;
		charging_label.h           = 50;
		AddLabel(charging_label, (char*)charging_label.text.c_str());

		usleep(10000);

		charging.font        = &statstrip_reg_40;
		charging.text        = "Checking...";
		charging.text_align  = LV_LABEL_ALIGN_CENTER;
		charging.set_color   = true;
		charging.color       = LV_COLOR_ORANGE;
		charging.x           = 240;
		charging.y           = 475;
		charging.w           = HALF_SCREEN;
		charging.h           = 50;
		charging.obj		 = AddLabel(charging, (char*)charging.text.c_str());

		usleep(10000);

		brightness_label.font        = &statstrip_reg_40;
		brightness_label.text    	 = "Brightness:";
		brightness_label.text_align  = LV_LABEL_ALIGN_RIGHT;
		brightness_label.x           = 0;
		brightness_label.y           = 525;
		brightness_label.w           = right_column;
		brightness_label.h           = 50;
		AddLabel(brightness_label, (char*)brightness_label.text.c_str());

		usleep(10000);

		brightness_value.font        = &statstrip_reg_40;
		brightness_value.text_align  = LV_LABEL_ALIGN_CENTER;
		brightness_value.x           = 240;
		brightness_value.y           = 525;
		brightness_value.w           = right_column;
		brightness_value.h           = 50;
		AddLabel(brightness_value, (char*)brightness_value.text.c_str());

		usleep(250000);

		int batteryCurrent = GetBatteryCurrent();
		std::string batteryCurrentFormatted = std::to_string(batteryCurrent);
		batteryCurrentFormatted = batteryCurrentFormatted.append(" mA");
		if ((batteryCurrent >= 300) && (batteryCurrent <= 700))
		{
			current.color = LV_COLOR_GREEN;
		}
		else if ((batteryCurrent >= 200) && (batteryCurrent < 300))
		{
			current.color = LV_COLOR_ORANGE;
		}
		else if ((batteryCurrent >= 700) && (batteryCurrent < 850))
		{
			current.color = LV_COLOR_ORANGE;
		}
		else
		{
			current.color = LV_COLOR_RED;
		}
		ChangeLabel(current, (char*)batteryCurrentFormatted.c_str());

		usleep(100000);

		int batteryVoltage = GetBatteryVoltage();
		std::string batteryVoltageFormatted = std::to_string(batteryVoltage);
		batteryVoltageFormatted = batteryVoltageFormatted.insert(batteryVoltageFormatted.length() - 2, ".").append(" V");
		if ((batteryVoltage >= 350) && (batteryVoltage <= 550))
		{
			voltage.color = LV_COLOR_GREEN;
		}
		else if (batteryVoltage > 560)
		{
			voltage.color = LV_COLOR_ORANGE;
		}
		else
		{
			voltage.color = LV_COLOR_RED;
		}
		ChangeLabel(voltage, (char*)batteryVoltageFormatted.c_str());

		usleep(100000);

		std::string batteryHealth = GetBatteryHealth();

		if (batteryHealth.length() <= 0)
		{
			batteryHealth = "ERROR";
		}

		if (batteryHealth.compare("Good") == 0)
		{
			battery_health.color = LV_COLOR_GREEN;
		}
		else if (batteryHealth.compare("Ok") == 0)
		{
			battery_health.color = LV_COLOR_ORANGE;
		}
		else
		{
			battery_health.color = LV_COLOR_RED;
		}

		ChangeLabel(battery_health, (char*)batteryHealth.c_str());

		usleep(100000);

		int batteryTemp = GetBatteryTemp();
		std::string batteryTempFormatted = std::to_string(batteryTemp);
		batteryTempFormatted = batteryTempFormatted.insert(batteryTempFormatted.length() - 1, ".").append(" ºC");

		if ((batteryTemp >= 400) && (batteryTemp <= 500))
		{
			battery_temp.color = LV_COLOR_GREEN;
		}
		else if (batteryTemp > 480)
		{
			battery_temp.color = LV_COLOR_ORANGE;
		}
		else
		{
			battery_temp.color = LV_COLOR_RED;
		}

		ChangeLabel(battery_temp, (char*)batteryTempFormatted.c_str());

		usleep(100000);

		int batteryLevel = GetBatteryPercentage() + 1;
		std::string batteryLevelFormatted = std::to_string(batteryLevel) + "%";

		if (batteryLevel >= 75)
		{
			battery_level.color = LV_COLOR_GREEN;
		}
		else if ((batteryLevel >= 25) && (batteryLevel < 75))
		{
			battery_level.color = LV_COLOR_ORANGE;
		}
		else
		{
			battery_level.color = LV_COLOR_RED;
		}

		ChangeLabel(battery_level, (char*)batteryLevelFormatted.c_str());

		usleep(100000);

		int isMeterInDock = IsMeterInDock();
		std::string isMeterInDockValue = (isMeterInDock == 0) ? "TRUE" : "FALSE";
		in_dock.color = (isMeterInDock == 0) ? LV_COLOR_GREEN : LV_COLOR_RED;
		ChangeLabel(in_dock, (char*)isMeterInDockValue.c_str());

		usleep(100000);

		int isBatteryCharging = IsBatteryCharging();
		std::string isBatteryChargingValue = (isBatteryCharging == 1) ? "TRUE" : "FALSE";
		charging.color = (isBatteryCharging == 1) ? LV_COLOR_GREEN : LV_COLOR_RED;
		ChangeLabel(charging, (char*)isBatteryChargingValue.c_str());

		usleep(500000);

		//RunBatteryMonitorCleanup();
		pthread_cancel(batteryMonitor);
		pthread_cancel(adjustBrightness);

		sleep(3);

		std::string resetLabel = "";
		ChangeLabel(battery_health, (char*)resetLabel.c_str());
		ChangeLabel(battery_temp, (char*)resetLabel.c_str());
		ChangeLabel(battery_level, (char*)resetLabel.c_str());
		ChangeLabel(in_dock, (char*)resetLabel.c_str());
		ChangeLabel(charging, (char*)resetLabel.c_str());
		ChangeLabel(current, (char*)resetLabel.c_str());
		ChangeLabel(voltage, (char*)resetLabel.c_str());
		CloseGPIOs();
	}

	return NULL;
}

int main(int argc, char **argv)
{

	UNUSED(argc);
	UNUSED(argv);

	pthread_t tid;
	pthread_t tid2;

	std::string version =
			MAJOR_VER + std::string(".") +
			MINOR_VER + std::string(".") +
			BUILD_VER + std::string(".") +
			PATCH_VER;

	DBGPRT(DBG_INFO1, "Hospital Meter Battery Info version - %s\n", version.c_str());

	SetScreenBrightness(brightness);

	GuiInit();

	AddAdjustBrightness();

	pthread_create(&tid, NULL, battery_monitor, (void*)version.c_str());
	pthread_mutex_lock(&start_lock);
	pthread_cond_wait(&start_cond1, &start_lock);
	pthread_mutex_unlock(&start_lock);
	pthread_join(tid, NULL);

	DBGPRT(DBG_INFO1, "Hospital Meter Battery Info Completed\n");

	return 0;
}
