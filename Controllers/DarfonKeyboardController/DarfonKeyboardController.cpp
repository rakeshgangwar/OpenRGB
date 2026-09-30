/*---------------------------------------------------------*\
| DarfonKeyboardController.cpp                              |
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

#include <cstring>
#include <thread>
#include <chrono>
#include "DarfonKeyboardController.h"
#include "LogManager.h"
#include "StringUtils.h"

DarfonKeyboardController::DarfonKeyboardController(hid_device* dev_handle, const char* path)
{
    dev             = dev_handle;
    location        = path;

    /*---------------------------------------------------------*\
    | Get serial number                                         |
    \*---------------------------------------------------------*/
    wchar_t serial_buf[128];
    if(hid_get_serial_number_string(dev, serial_buf, 128) == 0)
    {
        serial_number = StringUtils::wstring_to_string(serial_buf);
    }

    /*---------------------------------------------------------*\
    | Get device firmware version through status command        |
    \*---------------------------------------------------------*/
    firmware_version = "Unknown";

    /*---------------------------------------------------------*\
    | Initialize the device - required to enable RGB control    |
    \*---------------------------------------------------------*/
    Initialize();
}

DarfonKeyboardController::~DarfonKeyboardController()
{
    hid_close(dev);
}

std::string DarfonKeyboardController::GetDeviceLocation()
{
    return("HID: " + location);
}

std::string DarfonKeyboardController::GetSerialString()
{
    return(serial_number);
}

std::string DarfonKeyboardController::GetFirmwareVersion()
{
    return(firmware_version);
}

unsigned int DarfonKeyboardController::GetLEDCount()
{
    /*---------------------------------------------------------*\
    | Alienware laptop keyboards typically have 4 zones:        |
    | Left, Center-Left, Center-Right, Right                    |
    | Some models may have more per-key RGB                     |
    \*---------------------------------------------------------*/
    return 4;
}

void DarfonKeyboardController::Initialize()
{
    unsigned char cmd[DARFON_REPORT_SIZE];

    LOG_INFO("[Darfon Keyboard] Initializing device...");

    /*---------------------------------------------------------*\
    | Step 1: Reset the device                                  |
    \*---------------------------------------------------------*/
    memset(cmd, 0, sizeof(cmd));
    cmd[0] = DARFON_CMD_RESET;
    SendFeatureReport(cmd, sizeof(cmd));

    /*---------------------------------------------------------*\
    | Wait for device to process reset                          |
    \*---------------------------------------------------------*/
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    /*---------------------------------------------------------*\
    | Step 2: Query device status                               |
    \*---------------------------------------------------------*/
    GetDeviceStatus();

    /*---------------------------------------------------------*\
    | Step 3: Send turn-on/brightness command to enable RGB     |
    | This is CRITICAL - without this, color commands are       |
    | ignored by the device (V5 API requirement)                |
    \*---------------------------------------------------------*/
    memset(cmd, 0, sizeof(cmd));
    cmd[0] = DARFON_CMD_BRIGHTNESS;
    cmd[1] = 0x38;
    cmd[2] = 0x9C;
    cmd[3] = DARFON_BRIGHTNESS_MAX;  /* Full brightness */
    SendFeatureReport(cmd, sizeof(cmd));

    LOG_INFO("[Darfon Keyboard] Initialization complete");
}

void DarfonKeyboardController::SendFeatureReport(unsigned char* data, size_t length)
{
    unsigned char buf[DARFON_REPORT_SIZE + 1];

    memset(buf, 0, sizeof(buf));
    buf[0] = DARFON_REPORT_ID;

    if(length > DARFON_REPORT_SIZE)
    {
        length = DARFON_REPORT_SIZE;
    }

    /*---------------------------------------------------------*\
    | Copy command data starting at buf[1] (after report ID)    |
    \*---------------------------------------------------------*/
    memcpy(&buf[1], data, length);

    /*---------------------------------------------------------*\
    | Try both feature report and regular write                 |
    \*---------------------------------------------------------*/
    int result = hid_send_feature_report(dev, buf, sizeof(buf));
    int result2 = hid_write(dev, buf, sizeof(buf));
    
    /*---------------------------------------------------------*\
    | Log the first 20 bytes of the buffer for debugging        |
    \*---------------------------------------------------------*/
    LOG_INFO("[Darfon Keyboard] Send: feature=%d write=%d buf[0-19]=%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", 
             result, result2,
             buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9],
             buf[10], buf[11], buf[12], buf[13], buf[14], buf[15], buf[16], buf[17], buf[18], buf[19]);

    /*---------------------------------------------------------*\
    | Small delay between commands for device stability         |
    \*---------------------------------------------------------*/
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

void DarfonKeyboardController::GetFeatureReport(unsigned char* data, size_t length)
{
    unsigned char buf[DARFON_REPORT_SIZE + 1];

    memset(buf, 0, sizeof(buf));
    buf[0] = DARFON_REPORT_ID;

    hid_get_feature_report(dev, buf, sizeof(buf));

    if(length > DARFON_REPORT_SIZE)
    {
        length = DARFON_REPORT_SIZE;
    }

    memcpy(data, &buf[1], length);
}

unsigned char DarfonKeyboardController::GetDeviceStatus()
{
    unsigned char cmd[DARFON_REPORT_SIZE];
    unsigned char response[DARFON_REPORT_SIZE];

    memset(cmd, 0, sizeof(cmd));
    cmd[0] = DARFON_CMD_STATUS;

    SendFeatureReport(cmd, sizeof(cmd));
    GetFeatureReport(response, sizeof(response));

    LOG_INFO("[Darfon Keyboard] GetDeviceStatus response: %02X %02X %02X %02X %02X %02X %02X %02X",
             response[0], response[1], response[2], response[3], 
             response[4], response[5], response[6], response[7]);

    return response[1];
}

void DarfonKeyboardController::Reset()
{
    unsigned char cmd[DARFON_REPORT_SIZE];

    memset(cmd, 0, sizeof(cmd));
    cmd[0] = DARFON_CMD_RESET;

    SendFeatureReport(cmd, sizeof(cmd));

    /*---------------------------------------------------------*\
    | Also try sending to report 0x5A                           |
    \*---------------------------------------------------------*/
    unsigned char buf5a[17];
    memset(buf5a, 0, sizeof(buf5a));
    buf5a[0] = 0x5A;  /* Report ID */
    buf5a[1] = 0x94;  /* Reset command */
    int result = hid_send_feature_report(dev, buf5a, sizeof(buf5a));
    LOG_INFO("[Darfon Keyboard] Reset via 0x5A: result=%d", result);

    /*---------------------------------------------------------*\
    | Wait for device to be ready                               |
    \*---------------------------------------------------------*/
    GetDeviceStatus();
}

void DarfonKeyboardController::UpdateColors()
{
    unsigned char cmd[DARFON_REPORT_SIZE];

    memset(cmd, 0, sizeof(cmd));
    cmd[0] = DARFON_CMD_UPDATE;
    cmd[1] = 0x01;
    cmd[2] = 0xFF;

    SendFeatureReport(cmd, sizeof(cmd));
}

void DarfonKeyboardController::SetLEDColor(unsigned char led_index, unsigned char r, unsigned char g, unsigned char b)
{
    unsigned char cmd[DARFON_REPORT_SIZE];

    /*---------------------------------------------------------*\
    | Start color set sequence                                  |
    | Format: [0x8C][0x02]...[idx+1][R][G][B]                   |
    | Data block starts at offset 3 in the command buffer       |
    | (which becomes offset 4 in the HID report after report ID)|
    \*---------------------------------------------------------*/
    memset(cmd, 0, sizeof(cmd));
    cmd[0] = DARFON_CMD_COLOR_SET;
    cmd[1] = DARFON_COLOR_SET_BEGIN;

    /*---------------------------------------------------------*\
    | Add color data block at offset 3                          |
    | LED indices are 1-based in protocol                       |
    \*---------------------------------------------------------*/
    cmd[3] = led_index + 1;
    cmd[4] = r;
    cmd[5] = g;
    cmd[6] = b;

    SendFeatureReport(cmd, sizeof(cmd));

    /*---------------------------------------------------------*\
    | Send loop/finish command                                  |
    \*---------------------------------------------------------*/
    memset(cmd, 0, sizeof(cmd));
    cmd[0] = DARFON_CMD_COLOR_SET;
    cmd[1] = DARFON_COLOR_SET_LOOP;

    SendFeatureReport(cmd, sizeof(cmd));
}

void DarfonKeyboardController::SetAllLEDs(unsigned char r, unsigned char g, unsigned char b)
{
    unsigned char cmd[DARFON_REPORT_SIZE];

    /*---------------------------------------------------------*\
    | Start color set sequence with multiple LEDs               |
    | Data blocks start at offset 3 in the command buffer       |
    \*---------------------------------------------------------*/
    memset(cmd, 0, sizeof(cmd));
    cmd[0] = DARFON_CMD_COLOR_SET;
    cmd[1] = DARFON_COLOR_SET_BEGIN;

    /*---------------------------------------------------------*\
    | Set all 4 keyboard zones to the same color                |
    | Format: [led_index+1] [R] [G] [B] repeated                |
    \*---------------------------------------------------------*/
    unsigned int led_count = GetLEDCount();
    unsigned int offset = 3;

    for(unsigned int i = 0; i < led_count && offset + 4 <= DARFON_REPORT_SIZE; i++)
    {
        cmd[offset++] = i + 1;  /* LED index (1-based) */
        cmd[offset++] = r;
        cmd[offset++] = g;
        cmd[offset++] = b;
    }

    SendFeatureReport(cmd, sizeof(cmd));

    /*---------------------------------------------------------*\
    | Send loop/finish command                                  |
    \*---------------------------------------------------------*/
    memset(cmd, 0, sizeof(cmd));
    cmd[0] = DARFON_CMD_COLOR_SET;
    cmd[1] = DARFON_COLOR_SET_LOOP;

    SendFeatureReport(cmd, sizeof(cmd));
}

void DarfonKeyboardController::SetMultipleColors(std::vector<RGBColor>& colors)
{
    unsigned char cmd[DARFON_REPORT_SIZE];
    RGBColor color = colors.size() > 0 ? colors[0] : 0;
    unsigned char r = RGBGetRValue(color);
    unsigned char g = RGBGetGValue(color);
    unsigned char b = RGBGetBValue(color);

    LOG_INFO("[Darfon Keyboard] SetMultipleColors: Setting all keys to R=%d G=%d B=%d", r, g, b);

    /*---------------------------------------------------------*\
    | For per-key RGB keyboards, we need to set many keys       |
    | Send multiple packets with key indices 0-127              |
    \*---------------------------------------------------------*/
    
    for(unsigned int batch = 0; batch < 128; batch += 14)
    {
        memset(cmd, 0, sizeof(cmd));
        cmd[0] = DARFON_CMD_COLOR_SET;
        cmd[1] = DARFON_COLOR_SET_BEGIN;

        unsigned int offset = 3;
        
        /*---------------------------------------------------------*\
        | Pack up to 14 LEDs per command (14 * 4 = 56 bytes + 3)    |
        \*---------------------------------------------------------*/
        for(unsigned int i = 0; i < 14 && (batch + i) < 128; i++)
        {
            cmd[offset++] = batch + i;  /* LED index */
            cmd[offset++] = r;
            cmd[offset++] = g;
            cmd[offset++] = b;
        }

        SendFeatureReport(cmd, sizeof(cmd));
    }

    /*---------------------------------------------------------*\
    | Send loop/finish command                                  |
    \*---------------------------------------------------------*/
    memset(cmd, 0, sizeof(cmd));
    cmd[0] = DARFON_CMD_COLOR_SET;
    cmd[1] = DARFON_COLOR_SET_LOOP;

    SendFeatureReport(cmd, sizeof(cmd));
}

void DarfonKeyboardController::SetBrightness(unsigned char brightness)
{
    unsigned char cmd[DARFON_REPORT_SIZE];

    /*---------------------------------------------------------*\
    | V5 API uses brightness scale 0-255 directly               |
    \*---------------------------------------------------------*/
    memset(cmd, 0, sizeof(cmd));
    cmd[0] = DARFON_CMD_BRIGHTNESS;
    cmd[1] = 0x38;
    cmd[2] = 0x9C;
    cmd[3] = brightness;

    SendFeatureReport(cmd, sizeof(cmd));
}

void DarfonKeyboardController::SetEffect
    (
    unsigned char   effect,
    unsigned char   speed,
    unsigned char   r1,
    unsigned char   g1,
    unsigned char   b1,
    unsigned char   r2,
    unsigned char   g2,
    unsigned char   b2
    )
{
    unsigned char cmd[DARFON_REPORT_SIZE];

    /*---------------------------------------------------------*\
    | If effect is reset/off, send reset first                  |
    \*---------------------------------------------------------*/
    if(effect == DARFON_EFFECT_RESET || effect == DARFON_EFFECT_COLOR)
    {
        Reset();
    }

    memset(cmd, 0, sizeof(cmd));
    cmd[0] = DARFON_CMD_EFFECT;
    cmd[1] = effect;
    cmd[2] = speed;
    cmd[3] = 0x00;
    cmd[4] = 0x00;
    cmd[5] = 0x01;
    cmd[6] = 0x01;
    cmd[7] = 0x01;

    /*---------------------------------------------------------*\
    | Number of colors (0 = 1 color, 1 = 2 colors, 2 = spectrum)|
    \*---------------------------------------------------------*/
    cmd[8] = 0x00;

    /*---------------------------------------------------------*\
    | Color 1                                                   |
    \*---------------------------------------------------------*/
    cmd[9]  = r1;
    cmd[10] = g1;
    cmd[11] = b1;

    /*---------------------------------------------------------*\
    | Color 2                                                   |
    \*---------------------------------------------------------*/
    cmd[12] = r2;
    cmd[13] = g2;
    cmd[14] = b2;

    SendFeatureReport(cmd, sizeof(cmd));

    /*---------------------------------------------------------*\
    | Update to apply effect                                    |
    \*---------------------------------------------------------*/
    UpdateColors();
}
