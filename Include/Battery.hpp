/*******************************************************************************
 *                         COPYRIGHT NOTICE
 *                   "Copyright 2023 Nova Biomedical Corporation"
 *             This program is the property of Nova Biomedical Corporation
 *                 200 Prospect Street, Waltham, MA 02454-9141
 *             Any unauthorized use or duplication is prohibited
 ********************************************************************************
 *
 *  Title            - Battery
 *  Source Filename  - Battery.hpp
 *  Author           - Zach Johnson
 *  Description      - This is the header file for Battery.cpp that defines the
 *  				   GPIO information as well as external methods.
 *
 *******************************************************************************/
#pragma once

#include <string>
#include <stdint.h>

// Corresponds to Rev9 board. "IN_DOCK" is for the hall effect sensor.
#define IN_DOCK_PORT			(1)
#define IN_DOCK_PIN 			(5)
#define CHARGING_PORT			(5)
#define CHARGING_PIN 			(15)

#define CONSUMER				"In_Base"

#ifndef BATTERY_DRIVER_SYMLINK
	#define BATTERY_PATH "/sys/class/power_supply/max1726x_battery/"
#else
	#define BATTERY_PATH "/root/battery_driver/"
#endif // BATTERY_DRIVER_SYMLINK

#define BATTERY_CAPACITY_FILE "/sys/class/power_supply/max1726x_battery/capacity"
#define BATTERY_CHARGE_NOW_FILE "/sys/class/power_supply/max1726x_battery/charge_now"
#define BATTERY_CHARGE_FULL_FILE "/sys/class/power_supply/max1726x_battery/charge_full"
#define BATTERY_HEALTH_FILE "/sys/class/power_supply/max1726x_battery/health"
#define BATTERY_TEMP_FILE "/sys/class/power_supply/max1726x_battery/temp"
#define BATTERY_VOLTAGE_FILE "/sys/class/power_supply/max1726x_battery/voltage_now"
#define BATTERY_CURRENT_FILE "/sys/class/power_supply/max1726x_battery/current_now"

typedef enum
{
	BATTERY_GPIO_NONE = 0,
	BATTERY_GPIO_IN_BASE = 0x01,
	BATTERY_GPIO_CHARGING = 0x02,
} BatteryGPIO;

typedef enum
{
	IN_BASE,
	CHARGING,
	MAX
} BatteryState;

typedef enum
{
	LINE_IN_BASE,
	LINE_CHARGING,
	LINE_MAX
} line_id;

typedef struct
{
	struct gpiod_line * line;
	const unsigned int pin;
} power_line;

#ifdef __cplusplus
extern "C" {
#endif

int GetIsBatteryCharging(void);
void *RunBatteryMonitor(void *args);
void RunBatteryMonitorCleanup(void);
int Battery_Init(void);
int InitializeBatteryGPIOs();
int OpenGPIO(line_id line_id);
int ReadGPIO(uint32_t GPIO);
void CloseGPIOs();
void CloseGPIOInDock();
void CloseGPIO(line_id line_id);
int GetGPIOLineValue(line_id line_id);
void MonitorForGPIOEvent(uint32_t GPIOs);
void *BatteryTaskLoop(void *arg);
int IsMeterInDock(void);
int IsBatteryCharging(void);
int GetBatteryPercentage(void);
std::string GetBatteryHealth(void);
int GetBatteryTemp(void);
int GetBatteryCurrent(void);
int GetBatteryVoltage(void);

#ifdef __cplusplus
}
#endif
