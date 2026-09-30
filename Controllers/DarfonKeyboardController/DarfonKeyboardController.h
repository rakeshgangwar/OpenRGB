/*---------------------------------------------------------*\
| DarfonKeyboardController.h                                |
|                                                           |
|   Driver for Darfon per-key RGB keyboards built into      |
|   Alienware laptops                                       |
|                                                           |
|   Protocol ("API v5") from alienfx-tools by T-Troll       |
|   https://github.com/T-Troll/alienfx-tools                |
|                                                           |
|   Rakesh Gangwar                              01 Oct 2026 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#pragma once

#include <string>
#include <vector>
#include <hidapi.h>
#include "RGBController.h"

/*---------------------------------------------------------*\
| Feature report 0xCC: report ID plus 63 bytes of data      |
\*---------------------------------------------------------*/
#define DARFON_REPORT_ID                0xCC
#define DARFON_REPORT_SIZE              64

/*---------------------------------------------------------*\
| A colour packet carries up to 15 blocks of                |
| [LED index + 1, R, G, B], starting at byte 4              |
\*---------------------------------------------------------*/
#define DARFON_COLOR_BLOCK_OFFSET       4
#define DARFON_COLOR_BLOCKS_PER_PACKET  15

enum
{
    DARFON_CMD_RESET                    = 0x94,
    DARFON_CMD_STATUS                   = 0x93,
    DARFON_CMD_COLOR_SET                = 0x8C,
    DARFON_CMD_UPDATE                   = 0x8B,
    DARFON_CMD_EFFECT                   = 0x80,
};

enum
{
    DARFON_COLOR_SET_DATA               = 0x02,
    DARFON_COLOR_SET_END                = 0x13,
};

class DarfonKeyboardController
{
public:
    DarfonKeyboardController(hid_device* dev_handle, const char* path, std::string dev_name);
    ~DarfonKeyboardController();

    std::string     GetDeviceLocation();
    std::string     GetNameString();
    std::string     GetSerialString();

    void            SetLEDs(const std::vector<unsigned char>& led_ids, const std::vector<RGBColor>& colors);

private:
    hid_device*     dev;
    std::string     location;
    std::string     name;
    std::string     serial_number;

    void            DisableEffect();
    void            Reset();
    void            SendCommand(const unsigned char* data, size_t length);
};
