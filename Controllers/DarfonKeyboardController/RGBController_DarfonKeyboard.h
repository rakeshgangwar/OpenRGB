/*---------------------------------------------------------*\
| RGBController_DarfonKeyboard.h                            |
|                                                           |
|   RGBController for Darfon RGB keyboard                   |
|                                                           |
|   Based on alienfx-tools by T-Troll                       |
|   https://github.com/T-Troll/alienfx-tools                |
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
    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        DeviceUpdateMode();

private:
    DarfonKeyboardController* controller;
};
