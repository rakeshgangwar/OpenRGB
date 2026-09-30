/*---------------------------------------------------------*\
| RGBController_DarfonKeyboard.h                            |
|                                                           |
|   RGBController for Darfon per-key RGB keyboards built    |
|   into Alienware laptops                                  |
|                                                           |
|   Rakesh Gangwar                              01 Oct 2026 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#pragma once

#include "RGBController.h"
#include "DarfonKeyboardController.h"

class RGBController_DarfonKeyboard : public RGBController
{
public:
    RGBController_DarfonKeyboard(DarfonKeyboardController* controller_ptr);
    ~RGBController_DarfonKeyboard();

    void        SetupZones();

    void        DeviceUpdateLEDs();
    void        DeviceUpdateZoneLEDs(int zone);
    void        DeviceUpdateSingleLED(int led);

    void        DeviceUpdateMode();

private:
    DarfonKeyboardController* controller;
};
