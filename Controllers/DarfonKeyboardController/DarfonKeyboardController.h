/*---------------------------------------------------------*\
| DarfonKeyboardController.h                                |
|                                                           |
|   Driver for Darfon RGB keyboard (Alienware laptop        |
|   integrated keyboards)                                   |
|                                                           |
|   Based on alienfx-tools by T-Troll                       |
|   https://github.com/T-Troll/alienfx-tools                |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#pragma once

#include <string>
#include <hidapi.h>
#include "RGBController.h"

/*---------------------------------------------------------*\
| Darfon Keyboard Protocol Definitions (API V5)             |
| The feature report has 63 bytes of data (0x3F)            |
\*---------------------------------------------------------*/
#define DARFON_REPORT_SIZE              63
#define DARFON_REPORT_ID                0xCC

/*---------------------------------------------------------*\
| Command codes                                             |
\*---------------------------------------------------------*/
enum
{
    DARFON_CMD_RESET                    = 0x94,
    DARFON_CMD_STATUS                   = 0x93,
    DARFON_CMD_COLOR_SET                = 0x8C,
    DARFON_CMD_UPDATE                   = 0x8B,
    DARFON_CMD_BRIGHTNESS               = 0x83,
    DARFON_CMD_EFFECT                   = 0x80,
};

/*---------------------------------------------------------*\
| Color set sub-commands                                    |
\*---------------------------------------------------------*/
enum
{
    DARFON_COLOR_SET_BEGIN              = 0x02,
    DARFON_COLOR_SET_LOOP               = 0x13,
};

/*---------------------------------------------------------*\
| Effect types                                              |
\*---------------------------------------------------------*/
enum
{
    DARFON_EFFECT_COLOR                 = 0x00,
    DARFON_EFFECT_RESET                 = 0x01,
    DARFON_EFFECT_BREATHING             = 0x02,
    DARFON_EFFECT_WAVE_SINGLE           = 0x03,
    DARFON_EFFECT_WAVE_DUAL             = 0x04,
    DARFON_EFFECT_PULSE                 = 0x08,
    DARFON_EFFECT_PULSE_MIX             = 0x09,
    DARFON_EFFECT_NIGHT_RIDER           = 0x0A,
    DARFON_EFFECT_LASER                 = 0x0B,
};

/*---------------------------------------------------------*\
| Speed/Tempo values                                        |
\*---------------------------------------------------------*/
enum
{
    DARFON_SPEED_SLOWEST                = 0x64,
    DARFON_SPEED_NORMAL                 = 0x32,
    DARFON_SPEED_FASTEST                = 0x0A,
};

/*---------------------------------------------------------*\
| Brightness scale (0-255 for V5 API)                       |
\*---------------------------------------------------------*/
#define DARFON_BRIGHTNESS_MAX           0xFF

class DarfonKeyboardController
{
public:
    DarfonKeyboardController(hid_device* dev_handle, const char* path);
    ~DarfonKeyboardController();

    std::string     GetDeviceLocation();
    std::string     GetSerialString();
    std::string     GetFirmwareVersion();

    void            Reset();
    void            UpdateColors();

    void            SetLEDColor(unsigned char led_index, unsigned char r, unsigned char g, unsigned char b);
    void            SetAllLEDs(unsigned char r, unsigned char g, unsigned char b);
    void            SetMultipleColors(std::vector<RGBColor>& colors);
    void            SetBrightness(unsigned char brightness);

    void            SetEffect
                        (
                        unsigned char   effect,
                        unsigned char   speed,
                        unsigned char   r1,
                        unsigned char   g1,
                        unsigned char   b1,
                        unsigned char   r2,
                        unsigned char   g2,
                        unsigned char   b2
                        );

    unsigned int    GetLEDCount();

private:
    hid_device*     dev;
    std::string     location;
    std::string     serial_number;
    std::string     firmware_version;

    void            SendFeatureReport(unsigned char* data, size_t length);
    void            GetFeatureReport(unsigned char* data, size_t length);
    unsigned char   GetDeviceStatus();
};
