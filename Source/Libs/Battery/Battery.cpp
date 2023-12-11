/*******************************************************************************
 *                         COPYRIGHT NOTICE
 *                   "Copyright 2023 Nova Biomedical Corporation"
 *             This program is the property of Nova Biomedical Corporation
 *                 200 Prospect Street, Waltham, MA 02454-9141
 *             Any unauthorized use or duplication is prohibited
 ********************************************************************************
 *
 *  Title            - Battery
 *  Source Filename  - Batter.cpp
 *  Author           - Zach Johnson
 *  Description      - This file is used to test connect to the GPIO pins for
 *  				 - battery and check its charge.
 *
 *******************************************************************************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <gpiod.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "Battery.hpp"
#include "debug.hpp"

#undef DBGLVL
#define DBGLVL DBG_INFO1

static pthread_mutex_t battery_lock = PTHREAD_MUTEX_INITIALIZER;

power_line power_lines[] =
{
		{
				.line = NULL,
				.pin  = IN_DOCK_PIN
		},
		{
				.line = NULL,
				.pin  = CHARGING_PIN
		}
};

static struct gpiod_chip *in_dock_chip;
static struct gpiod_line *in_dock_line;
static struct gpiod_chip *charging_chip;
static struct gpiod_line *charging_line;

static int IsInDock   = -1;
static int IsCharging = -1;

void *RunBatteryMonitor(void *args)
{
	UNUSED(args);

	if (Battery_Init() != 0)
	{
		DBGPRT(DBG_ERR, "RunBatteryMonitor: Battery_Init failed\n");
	}

	return NULL;
}

void RunBatteryMonitorCleanup(void)
{
	pthread_mutex_lock(&battery_lock);

	CloseGPIOs();

	pthread_mutex_unlock(&battery_lock);
}

int Battery_Init(void)
{
	pthread_mutex_lock(&battery_lock);

	if (InitializeBatteryGPIOs() < 0)
	{
		DBGPRT(DBG_ERR, "Battery_Init: InitializeGPIOs failed\n");
		return -1;
	}

	pthread_mutex_unlock(&battery_lock);

	return 0;
}

int InitializeBatteryGPIOs()
{
	DBGPRT(DBG_INFO1, "InitializeBatteryGPIOs: started\n");

	// IN_DOCK_PORT = 1
	const char *in_dock_port_name  = "gpiochip1";
	const int in_dock_pin 		   = IN_DOCK_PIN;
	// CHARGING_PORT = 5
	const char *charging_port_name  = "gpiochip5";
	const int charging_pin 		   = CHARGING_PIN;

	if ((in_dock_chip = gpiod_chip_open_by_name(in_dock_port_name)) == NULL)
	{
		DBGPRT(DBG_ERR, "InitializeBatteryGPIOs: Failed to open in dock chip, %s\n", strerror(errno));
		return -1;
	}

	if ((in_dock_line = gpiod_chip_get_line(in_dock_chip, in_dock_pin)) == NULL)
	{
		DBGPRT(DBG_ERR, "InitializeBatteryGPIOs: Unable to get in dock line, %s\n", strerror(errno));
		return -1;
	}

	if ((gpiod_line_request_falling_edge_events(in_dock_line, "")) == -1)
	{
		DBGPRT(DBG_ERR, "InitializeBatteryGPIOs: Unable to set input in dock line active low, %s\n", strerror(errno));
		return -1;
	}

	DBGPRT(DBG_INFO1, "InitializeBatteryGPIOs: in dock initialized\n");

	if ((charging_chip = gpiod_chip_open_by_name(charging_port_name)) == NULL)
	{
		DBGPRT(DBG_ERR, "InitializeBatteryGPIOs: Failed to open charging chip, %s\n", strerror(errno));
		return -1;
	}

	if ((charging_line = gpiod_chip_get_line(charging_chip, charging_pin)) == NULL)
	{
		DBGPRT(DBG_ERR, "InitializeBatteryGPIOs: Unable to get charging line, %s\n", strerror(errno));
		return -1;
	}

	if ((gpiod_line_request_falling_edge_events(charging_line, "")) == -1)
	{
		DBGPRT(DBG_ERR, "InitializeBatteryGPIOs: Unable to set input charging line active low, %s\n", strerror(errno));
		return -1;
	}

	DBGPRT(DBG_INFO1, "InitializeBatteryGPIOs: charging initialized\n");

	return 0;
}

int OpenGPIO(line_id line_id)
{
	if (power_lines[line_id].line == NULL)
	{
		if ((power_lines[line_id].line = gpiod_chip_get_line(in_dock_chip, power_lines[line_id].pin)) == NULL)
		{
			DBGPRT(DBG_ERR, "InitializeGPIOs: Unable to get battery line, %s\n", strerror(errno));
			return -1;
		}
	}

	if ((gpiod_line_request_falling_edge_events(power_lines[line_id].line, CONSUMER)) == -1)
	{
		DBGPRT(DBG_ERR, "InitializeGPIOs: Unable to set input battery line active low, %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

int ReadGPIO(uint32_t GPIO)
{
	int result = -1;

	switch (GPIO)
	{
	case BATTERY_GPIO_IN_BASE:
		result = GetGPIOLineValue(LINE_IN_BASE);
		DBGPRT(DBG_INFO1, "ReadGPIO: GPIO value for In Base: %d\n", result);
		break;
	case BATTERY_GPIO_CHARGING:
		result = GetGPIOLineValue(LINE_CHARGING);
		DBGPRT(DBG_INFO1, "ReadGPIO: GPIO value for Charging: %d\n", result);
		break;
	default:
		break;
	}

	return result;
}

void CloseGPIOs()
{
	if (in_dock_line != NULL)
	{
		gpiod_line_release(in_dock_line);
	}

	if (in_dock_chip != NULL)
	{
		gpiod_chip_close(in_dock_chip);
		in_dock_chip = NULL;
	}

	if (charging_line != NULL)
	{
		gpiod_line_release(charging_line);
	}

	if (charging_chip != NULL)
	{
		gpiod_chip_close(charging_chip);
		charging_chip = NULL;
	}
}

void CloseGPIO(line_id line_id)
{
	if (power_lines[line_id].line != NULL)
	{
		gpiod_line_release(power_lines[line_id].line);
		power_lines[line_id].line = NULL;
	}
}

int GetGPIOLineValue(line_id line_id)
{
	struct gpiod_line * line = power_lines[line_id].line;

	if (line == NULL)
	{
		DBGPRT(DBG_ERR, "GetGPIOLineValue: GPIO was null\n");
		return -1;
	}

	return gpiod_line_get_value(power_lines[line_id].line);
}

void MonitorForGPIOEvent(uint32_t * GPIOs)
{
	struct gpiod_line_bulk bulk_lines;

	gpiod_line_bulk_init(&bulk_lines);

	for(int i = 0; i < LINE_MAX; i++)
	{
		if (bulk_lines.num_lines < GPIOD_LINE_BULK_MAX_LINES)
		{
			if (power_lines[i].line != NULL)
			{
				gpiod_line_bulk_add(&bulk_lines, power_lines[i].line);
			}
		}
	}

	struct gpiod_line_bulk event_bulk_lines;

	gpiod_line_bulk_init(&event_bulk_lines);

	if (gpiod_line_event_wait_bulk(&bulk_lines, NULL, &event_bulk_lines) <= 0)
	{
		DBGPRT(DBG_ERR, "MonitorForGPIOEvent: No events were returned for any lines\n");
		return;
	}

	for (unsigned int i = 0; i < event_bulk_lines.num_lines; i++)
	{
		struct gpiod_line * event_line = gpiod_line_bulk_get_line(&event_bulk_lines, i);

		if (event_line != NULL)
		{
			struct gpiod_line_event event;

			unsigned int line_offset = gpiod_line_offset(event_line);

			gpiod_line_event_read(event_line, &event);

			if (line_offset == IN_DOCK_PIN)
			{
				*GPIOs |= BATTERY_GPIO_IN_BASE;
			}
			else if (line_offset == CHARGING_PIN)
			{
				*GPIOs |= BATTERY_GPIO_CHARGING;
			}
		}
	}
}

void *BatteryTaskLoop(void *arg)
{
	UNUSED(arg);

	while (1)
	{
		uint32_t GPIOs = 0;

		MonitorForGPIOEvent(&GPIOs);

		if (GPIOs == BATTERY_GPIO_IN_BASE)
		{
			IsInDock = IsMeterInDock();
		}

		if (GPIOs == BATTERY_GPIO_CHARGING)
		{
			IsCharging = IsBatteryCharging();
		}
	}
}

int IsMeterInDock(void)
{
	int in_dock_value = -1;

	for (int i = 0; i < 1; i++)
	{
		in_dock_value = gpiod_line_get_value(in_dock_line);

		if (in_dock_value == 0)
		{
			DBGPRT(DBG_INFO4, "IsMeterInDock: In Dock value captured, %d\n", in_dock_value);
			usleep(1000000);
			break;
		}
		else if (in_dock_value == 1)
		{
			DBGPRT(DBG_INFO4, "IsMeterInDock: Get in base value low, re-checking, value = %d\n", in_dock_value);
			usleep(1000000);
		}
		else
		{
			DBGPRT(DBG_ERR, "IsMeterInDock: Get in base value failed, %s\n", strerror(errno));
			usleep(1000000);
		}
	}

	return in_dock_value;
}

int IsBatteryCharging(void)
{
	int charging_value = -1;

	for (int i = 0; i < 1; i++)
	{
		charging_value = gpiod_line_get_value(charging_line);

		if (charging_value == 1)
		{
			DBGPRT(DBG_INFO4, "IsBatteryCharging: Charging value captured, %d\n", charging_value);
			usleep(1000000);
			break;
		}
		else if (charging_value == 0)
		{
			DBGPRT(DBG_INFO4, "IsBatteryCharging: Get charging value high, re-checking, value = %d\n", charging_value);
			usleep(1000000);
		}
		else
		{
			DBGPRT(DBG_ERR, "IsBatteryCharging: Get charging value failed, %s\n", strerror(errno));
			usleep(1000000);
		}
	}

	return charging_value;
}

int GetBatteryPercentage(void)
{
	FILE * file;
	int percentage = -1;
	//int charge_now = -1;
	//int charge_full = -1;
	int error;

	if ((file = fopen(BATTERY_CAPACITY_FILE, "r")) == NULL)
	{
		DBGPRT(DBG_ERR, "GetBatteryPercentage: Failed to open Battery Capacity File\n");
		return -1;
	}

	if ((error = fscanf(file, "%d", &percentage)) == EOF)
	{
		percentage = -1;
	}

	if (fclose(file) != 0)
	{
		DBGPRT(DBG_ERR, "GetBatteryPercentage: Failed to close Battery Capacity File\n");
		return -1;
	}

	/*if ((file = fopen(BATTERY_CHARGE_NOW_FILE, "r")) == NULL)
	{
		DBGPRT(DBG_ERR, "GetBatteryPercentage: Failed to open Battery Charge Now File\n");
		return -1;
	}

	if ((error = fscanf(file, "%d", &charge_now)) == EOF)
	{
		charge_now = -1;
	}

	fclose(file);

	if ((file = fopen(BATTERY_CHARGE_FULL_FILE, "r")) == NULL)
	{
		DBGPRT(DBG_ERR, "GetBatteryPercentage: Failed to open Battery Charge Full File\n");
		return -1;
	}

	if ((error = fscanf(file, "%d", &charge_full)) == EOF)
	{
		charge_full = -1;
	}

	fclose(file);

	int percentage = (charge_now * 100) / charge_full;*/

	DBGPRT(DBG_INFO4, "GetBatteryPercentage: Level = %d\n", percentage);

	return percentage;
}

std::string GetBatteryHealth(void)
{
	std::ifstream file (BATTERY_HEALTH_FILE);
	std::string h = "N/A";

	if (!file.is_open())
	{
		DBGPRT(DBG_ERR, "GetBatteryHealth: Failed to open Battery Health File\n");
		return "Error";
	}
	else
	{
		file >> h;
	}

	file.close();

	return h;
}

int GetBatteryTemp(void)
{
	FILE * file;
	int temp = -1;
	int error;

	if ((file = fopen(BATTERY_TEMP_FILE, "r")) == NULL)
	{
		DBGPRT(DBG_ERR, "GetBatteryTemp: Failed to open Battery Temp File\n");
		return -1;
	}

	if ((error = fscanf(file, "%d", &temp)) == EOF)
	{
		DBGPRT(DBG_ERR, "GetBatteryTemp: Failed to get Battery Temp\n");
		temp = -1;
	}

	fclose(file);

	return temp;
}

int GetBatteryCurrent(void)
{
	FILE * file;
	int current = -1;
	int error;

	if ((file = fopen(BATTERY_CURRENT_FILE, "r")) == NULL)
	{
		DBGPRT(DBG_ERR, "GetBatteryCurrent: Failed to open Battery Current File\n");
		return -1;
	}
	if ((error = fscanf(file, "%d", &current)) == EOF)
	{
		DBGPRT(DBG_ERR, "GetBatteryCurrent: Failed to get Battery Current\n");
		current = -1;
	}

	current = current * 0.001;
	DBGPRT(DBG_INFO4, "GetBatteryCurrent: %d\n", current);
	fclose(file);

	return current;
}

int GetBatteryVoltage(void)
{
	FILE * file;
	int voltage = -1;
	int error;

	if ((file = fopen(BATTERY_VOLTAGE_FILE, "r")) == NULL)
	{
		DBGPRT(DBG_ERR, "GetBatteryVoltage: Failed to open Battery Voltage File\n");
		return -1;
	}
	if ((error = fscanf(file, "%d", &voltage)) == EOF)
	{
		DBGPRT(DBG_ERR, "GetBatteryVoltage: Failed to get Battery Voltage\n");
		voltage = -1;
	}

	voltage = voltage * 0.0001;
	DBGPRT(DBG_INFO4, "GetBatteryVoltage: %d\n", voltage);
	fclose(file);

	return voltage;
}





