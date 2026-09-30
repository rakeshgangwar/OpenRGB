/*---------------------------------------------------------*\
| DarfonKeyboardController.cpp                              |
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

#include <chrono>
#include <cstring>
#include <thread>
#include "DarfonKeyboardController.h"
#include "StringUtils.h"

using namespace std::chrono_literals;

DarfonKeyboardController::DarfonKeyboardController(hid_device* dev_handle, const char* path, std::string dev_name)
{
    dev         = dev_handle;
    location    = path;
    name        = dev_name;

    wchar_t serial_buf[128];

    if(hid_get_serial_number_string(dev, serial_buf, 128) == 0)
    {
        serial_number = StringUtils::wstring_to_string(serial_buf);
    }

    DisableEffect();
}

DarfonKeyboardController::~DarfonKeyboardController()
{
    hid_close(dev);
}

std::string DarfonKeyboardController::GetDeviceLocation()
{
    return("HID: " + location);
}

std::string DarfonKeyboardController::GetNameString()
{
    return(name);
}

std::string DarfonKeyboardController::GetSerialString()
{
    return(serial_number);
}

void DarfonKeyboardController::SendCommand(const unsigned char* data, size_t length)
{
    unsigned char buf[DARFON_REPORT_SIZE];

    memset(buf, 0, sizeof(buf));
    buf[0] = DARFON_REPORT_ID;
    memcpy(&buf[1], data, length);

    hid_send_feature_report(dev, buf, sizeof(buf));

    std::this_thread::sleep_for(5ms);
}

void DarfonKeyboardController::Reset()
{
    /*---------------------------------------------------------*\
    | Reset, then query the status and read the answer, as the  |
    | vendor software does before each set of colours           |
    \*---------------------------------------------------------*/
    const unsigned char reset[]     = { DARFON_CMD_RESET  };
    const unsigned char status[]    = { DARFON_CMD_STATUS };
    unsigned char       buf[DARFON_REPORT_SIZE];

    SendCommand(reset,  sizeof(reset));
    SendCommand(status, sizeof(status));

    memset(buf, 0, sizeof(buf));
    buf[0] = DARFON_REPORT_ID;
    hid_get_feature_report(dev, buf, sizeof(buf));
}

void DarfonKeyboardController::DisableEffect()
{
    /*---------------------------------------------------------*\
    | While a firmware effect is active the keyboard accepts    |
    | per-key colours and does not show them.  Switch effects   |
    | off before taking control.                                |
    \*---------------------------------------------------------*/
    const unsigned char effect_off[]    = { DARFON_CMD_EFFECT, 0x01, 0xFE, 0x00, 0x00, 0x01, 0x01, 0x01 };
    const unsigned char update[]        = { DARFON_CMD_UPDATE, 0x01, 0xFF };

    Reset();
    SendCommand(effect_off, sizeof(effect_off));
    SendCommand(update,     sizeof(update));
}

void DarfonKeyboardController::SetLEDs(const std::vector<unsigned char>& led_ids, const std::vector<RGBColor>& colors)
{
    Reset();

    for(std::size_t first = 0; first < led_ids.size(); first += DARFON_COLOR_BLOCKS_PER_PACKET)
    {
        unsigned char cmd[DARFON_REPORT_SIZE - 1];

        memset(cmd, 0, sizeof(cmd));
        cmd[0] = DARFON_CMD_COLOR_SET;
        cmd[1] = DARFON_COLOR_SET_DATA;

        /*-----------------------------------------------------*\
        | cmd[] starts after the report ID, so byte 4 of the    |
        | report is cmd[3]                                      |
        \*-----------------------------------------------------*/
        std::size_t offset = DARFON_COLOR_BLOCK_OFFSET - 1;

        for(std::size_t idx = first; idx < led_ids.size() && idx < first + DARFON_COLOR_BLOCKS_PER_PACKET; idx++)
        {
            cmd[offset++] = led_ids[idx] + 1;
            cmd[offset++] = RGBGetRValue(colors[idx]);
            cmd[offset++] = RGBGetGValue(colors[idx]);
            cmd[offset++] = RGBGetBValue(colors[idx]);
        }

        SendCommand(cmd, sizeof(cmd));
    }

    const unsigned char end[]       = { DARFON_CMD_COLOR_SET, DARFON_COLOR_SET_END };
    const unsigned char update[]    = { DARFON_CMD_UPDATE, 0x01, 0xFF };

    SendCommand(end,    sizeof(end));
    SendCommand(update, sizeof(update));
}
