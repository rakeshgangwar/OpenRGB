/*---------------------------------------------------------*\
| DarfonKeyboardControllerDetect.cpp                        |
|                                                           |
|   Detector for Darfon RGB keyboard                        |
|   (Alienware laptop integrated keyboards)                 |
|                                                           |
|   Based on alienfx-tools by T-Troll                       |
|   https://github.com/T-Troll/alienfx-tools                |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#include <hidapi.h>
#include "Detector.h"
#include "LogManager.h"
#include "DarfonKeyboardController.h"
#include "RGBController_DarfonKeyboard.h"

/*-----------------------------------------------------*\
| Darfon vendor ID                                      |
\*-----------------------------------------------------*/
#define DARFON_VID                              0x0D62

/*-----------------------------------------------------*\
| Alienware laptop keyboard product IDs                 |
| These are manufactured by Darfon for Dell/Alienware   |
\*-----------------------------------------------------*/
#define DARFON_ALIENWARE_KEYBOARD_PID           0xBABC

/*-----------------------------------------------------*\
| HID Usage and Usage Page for the control interface    |
| The keyboard exposes multiple HID interfaces:         |
|   - Usage 0x10, Page 0xFF89: Standard HID interface   |
|   - Usage 0xCC, Page 0xFF89: AlienFX control interface|
\*-----------------------------------------------------*/
#define DARFON_USAGE                            0x00CC
#define DARFON_USAGE_PAGE                       0xFF89

/******************************************************************************************\
*                                                                                          *
*   DetectDarfonKeyboardControllers                                                        *
*                                                                                          *
*       Tests the USB address to see if a Darfon RGB Keyboard controller exists there.     *
*                                                                                          *
\******************************************************************************************/

void DetectDarfonKeyboardControllers(hid_device_info* info, const std::string& /*name*/)
{
    LOG_DEBUG("[Darfon Keyboard] Attempting to open device at path: %s", info->path);

    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        LOG_INFO("[Darfon Keyboard] Successfully opened device, creating controller");
        DarfonKeyboardController*     controller     = new DarfonKeyboardController(dev, info->path);
        RGBController_DarfonKeyboard* rgb_controller = new RGBController_DarfonKeyboard(controller);

        ResourceManager::get()->RegisterRGBController(rgb_controller);
        LOG_INFO("[Darfon Keyboard] Registering RGB controller");
    }
    else
    {
        LOG_WARNING("[Darfon Keyboard] Failed to open device at path: %s", info->path);
    }
}

REGISTER_HID_DETECTOR_PU("Alienware Laptop Keyboard", DetectDarfonKeyboardControllers, DARFON_VID, DARFON_ALIENWARE_KEYBOARD_PID, DARFON_USAGE_PAGE, DARFON_USAGE);
