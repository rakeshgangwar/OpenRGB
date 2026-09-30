/*---------------------------------------------------------*\
| DarfonKeyboardControllerDetect.cpp                        |
|                                                           |
|   Detector for Darfon per-key RGB keyboards built into    |
|   Alienware laptops                                       |
|                                                           |
|   Rakesh Gangwar                              01 Oct 2026 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#include <hidapi.h>
#include "DetectionManager.h"
#include "DarfonKeyboardController.h"
#include "RGBController_DarfonKeyboard.h"

#define DARFON_VID                              0x0D62
#define DARFON_ALIENWARE_X15_R2_KEYBOARD_PID    0xBABC

/*---------------------------------------------------------*\
| The lighting interface is the vendor collection with      |
| usage page 0xFF89, usage 0xCC                             |
\*---------------------------------------------------------*/
#define DARFON_USAGE_PAGE                       0xFF89
#define DARFON_USAGE                            0x00CC

DetectedControllers DetectDarfonKeyboardControllers(hid_device_info* info, const std::string& name)
{
    DetectedControllers detected_controllers;
    hid_device*         dev;

    dev = hid_open_path(info->path);

    if(dev)
    {
        DarfonKeyboardController*     controller     = new DarfonKeyboardController(dev, info->path, name);
        RGBController_DarfonKeyboard* rgb_controller = new RGBController_DarfonKeyboard(controller);

        detected_controllers.push_back(rgb_controller);
    }

    return(detected_controllers);
}

REGISTER_HID_DETECTOR_PU("Alienware x15 R2 Keyboard", DetectDarfonKeyboardControllers, DARFON_VID, DARFON_ALIENWARE_X15_R2_KEYBOARD_PID, DARFON_USAGE_PAGE, DARFON_USAGE);
