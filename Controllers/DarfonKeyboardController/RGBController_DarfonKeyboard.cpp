/*---------------------------------------------------------*\
| RGBController_DarfonKeyboard.cpp                          |
|                                                           |
|   RGBController for Darfon RGB keyboard                   |
|   (Alienware laptop integrated keyboards)                 |
|                                                           |
|   Based on alienfx-tools by T-Troll                       |
|   https://github.com/T-Troll/alienfx-tools                |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#include "RGBController_DarfonKeyboard.h"

/**------------------------------------------------------------------*\
    @name Darfon RGB Keyboard (Alienware Laptop)
    @category Keyboard
    @type USB
    @save :x:
    @direct :white_check_mark:
    @effects :white_check_mark:
    @detectors DetectDarfonKeyboardControllers
    @comment Alienware laptop integrated keyboard with 4-zone RGB lighting.
        Known to work with Alienware x15 R2, m15 R6/R7, x17 R1/R2.
\*-------------------------------------------------------------------*/

static const char* zone_names[] =
{
    "Left",
    "Center Left",
    "Center Right",
    "Right"
};

RGBController_DarfonKeyboard::RGBController_DarfonKeyboard(DarfonKeyboardController* controller_ptr)
{
    controller                      = controller_ptr;

    name                            = "Alienware Laptop Keyboard";
    vendor                          = "Alienware";
    type                            = DEVICE_TYPE_KEYBOARD;
    description                     = "Alienware Laptop RGB Keyboard (Darfon)";
    location                        = controller->GetDeviceLocation();
    serial                          = controller->GetSerialString();
    version                         = controller->GetFirmwareVersion();

    /*---------------------------------------------------------*\
    | Direct mode - per-zone color control                      |
    \*---------------------------------------------------------*/
    mode Direct;
    Direct.name                     = "Direct";
    Direct.value                    = DARFON_EFFECT_COLOR;
    Direct.flags                    = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    Direct.color_mode               = MODE_COLORS_PER_LED;
    Direct.brightness_min           = 0;
    Direct.brightness_max           = 255;
    Direct.brightness               = 255;
    modes.push_back(Direct);

    /*---------------------------------------------------------*\
    | Static mode - all zones same color                        |
    \*---------------------------------------------------------*/
    mode Static;
    Static.name                     = "Static";
    Static.value                    = DARFON_EFFECT_COLOR;
    Static.flags                    = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    Static.colors_min               = 1;
    Static.colors_max               = 1;
    Static.color_mode               = MODE_COLORS_MODE_SPECIFIC;
    Static.colors.resize(1);
    Static.brightness_min           = 0;
    Static.brightness_max           = 255;
    Static.brightness               = 255;
    modes.push_back(Static);

    /*---------------------------------------------------------*\
    | Breathing mode                                            |
    \*---------------------------------------------------------*/
    mode Breathing;
    Breathing.name                  = "Breathing";
    Breathing.value                 = DARFON_EFFECT_BREATHING;
    Breathing.flags                 = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS;
    Breathing.colors_min            = 1;
    Breathing.colors_max            = 1;
    Breathing.color_mode            = MODE_COLORS_MODE_SPECIFIC;
    Breathing.colors.resize(1);
    Breathing.speed_min             = DARFON_SPEED_SLOWEST;
    Breathing.speed_max             = DARFON_SPEED_FASTEST;
    Breathing.speed                 = DARFON_SPEED_NORMAL;
    Breathing.brightness_min        = 0;
    Breathing.brightness_max        = 255;
    Breathing.brightness            = 255;
    modes.push_back(Breathing);

    /*---------------------------------------------------------*\
    | Single Wave mode                                          |
    \*---------------------------------------------------------*/
    mode SingleWave;
    SingleWave.name                 = "Single Wave";
    SingleWave.value                = DARFON_EFFECT_WAVE_SINGLE;
    SingleWave.flags                = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS;
    SingleWave.colors_min           = 1;
    SingleWave.colors_max           = 1;
    SingleWave.color_mode           = MODE_COLORS_MODE_SPECIFIC;
    SingleWave.colors.resize(1);
    SingleWave.speed_min            = DARFON_SPEED_SLOWEST;
    SingleWave.speed_max            = DARFON_SPEED_FASTEST;
    SingleWave.speed                = DARFON_SPEED_NORMAL;
    SingleWave.brightness_min       = 0;
    SingleWave.brightness_max       = 255;
    SingleWave.brightness           = 255;
    modes.push_back(SingleWave);

    /*---------------------------------------------------------*\
    | Rainbow Wave mode                                         |
    \*---------------------------------------------------------*/
    mode RainbowWave;
    RainbowWave.name                = "Rainbow Wave";
    RainbowWave.value               = DARFON_EFFECT_WAVE_DUAL;
    RainbowWave.flags               = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS;
    RainbowWave.color_mode          = MODE_COLORS_NONE;
    RainbowWave.speed_min           = DARFON_SPEED_SLOWEST;
    RainbowWave.speed_max           = DARFON_SPEED_FASTEST;
    RainbowWave.speed               = DARFON_SPEED_NORMAL;
    RainbowWave.brightness_min      = 0;
    RainbowWave.brightness_max      = 255;
    RainbowWave.brightness          = 255;
    modes.push_back(RainbowWave);

    /*---------------------------------------------------------*\
    | Pulse mode                                                |
    \*---------------------------------------------------------*/
    mode Pulse;
    Pulse.name                      = "Pulse";
    Pulse.value                     = DARFON_EFFECT_PULSE;
    Pulse.flags                     = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS;
    Pulse.colors_min                = 1;
    Pulse.colors_max                = 1;
    Pulse.color_mode                = MODE_COLORS_MODE_SPECIFIC;
    Pulse.colors.resize(1);
    Pulse.speed_min                 = DARFON_SPEED_SLOWEST;
    Pulse.speed_max                 = DARFON_SPEED_FASTEST;
    Pulse.speed                     = DARFON_SPEED_NORMAL;
    Pulse.brightness_min            = 0;
    Pulse.brightness_max            = 255;
    Pulse.brightness                = 255;
    modes.push_back(Pulse);

    /*---------------------------------------------------------*\
    | Off mode                                                  |
    \*---------------------------------------------------------*/
    mode Off;
    Off.name                        = "Off";
    Off.value                       = DARFON_EFFECT_RESET;
    Off.flags                       = 0;
    Off.color_mode                  = MODE_COLORS_NONE;
    modes.push_back(Off);

    SetupZones();
}

RGBController_DarfonKeyboard::~RGBController_DarfonKeyboard()
{
    delete controller;
}

void RGBController_DarfonKeyboard::SetupZones()
{
    /*---------------------------------------------------------*\
    | Create a single zone for the 4 keyboard regions           |
    \*---------------------------------------------------------*/
    unsigned int led_count = controller->GetLEDCount();

    zone keyboard_zone;
    keyboard_zone.name              = "Keyboard";
    keyboard_zone.type              = ZONE_TYPE_LINEAR;
    keyboard_zone.leds_min          = led_count;
    keyboard_zone.leds_max          = led_count;
    keyboard_zone.leds_count        = led_count;
    keyboard_zone.matrix_map        = NULL;
    zones.push_back(keyboard_zone);

    /*---------------------------------------------------------*\
    | Create LEDs for each zone                                 |
    \*---------------------------------------------------------*/
    for(unsigned int led_idx = 0; led_idx < led_count; led_idx++)
    {
        led new_led;
        new_led.name                = zone_names[led_idx];
        new_led.value               = led_idx;
        leds.push_back(new_led);
    }

    SetupColors();
}

void RGBController_DarfonKeyboard::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_DarfonKeyboard::DeviceUpdateLEDs()
{
    /*---------------------------------------------------------*\
    | Reset device first                                        |
    \*---------------------------------------------------------*/
    controller->Reset();

    /*---------------------------------------------------------*\
    | Set brightness to enable RGB (V5 API requirement)         |
    \*---------------------------------------------------------*/
    unsigned char brightness = modes[active_mode].brightness;
    controller->SetBrightness(brightness);

    /*---------------------------------------------------------*\
    | Set all LED colors in one batch                           |
    \*---------------------------------------------------------*/
    controller->SetMultipleColors(colors);

    /*---------------------------------------------------------*\
    | Apply the changes                                         |
    \*---------------------------------------------------------*/
    controller->UpdateColors();
}

void RGBController_DarfonKeyboard::UpdateZoneLEDs(int zone)
{
    /*---------------------------------------------------------*\
    | Update all LEDs in the zone                               |
    \*---------------------------------------------------------*/
    for(unsigned int led_idx = 0; led_idx < zones[zone].leds_count; led_idx++)
    {
        unsigned int led_index = zones[zone].start_idx + led_idx;
        unsigned char r = RGBGetRValue(colors[led_index]);
        unsigned char g = RGBGetGValue(colors[led_index]);
        unsigned char b = RGBGetBValue(colors[led_index]);

        controller->SetLEDColor(led_idx, r, g, b);
    }

    controller->UpdateColors();
}

void RGBController_DarfonKeyboard::UpdateSingleLED(int led)
{
    unsigned char r = RGBGetRValue(colors[led]);
    unsigned char g = RGBGetGValue(colors[led]);
    unsigned char b = RGBGetBValue(colors[led]);

    controller->SetLEDColor(led, r, g, b);
    controller->UpdateColors();
}

void RGBController_DarfonKeyboard::DeviceUpdateMode()
{
    /*---------------------------------------------------------*\
    | Handle different modes                                    |
    \*---------------------------------------------------------*/
    switch(modes[active_mode].value)
    {
        case DARFON_EFFECT_COLOR:
            /*-------------------------------------------------*\
            | Direct/Static mode - update LEDs                  |
            \*-------------------------------------------------*/
            if(modes[active_mode].color_mode == MODE_COLORS_PER_LED)
            {
                DeviceUpdateLEDs();
            }
            else if(modes[active_mode].colors.size() >= 1)
            {
                unsigned char r = RGBGetRValue(modes[active_mode].colors[0]);
                unsigned char g = RGBGetGValue(modes[active_mode].colors[0]);
                unsigned char b = RGBGetBValue(modes[active_mode].colors[0]);

                controller->Reset();
                controller->SetAllLEDs(r, g, b);
                controller->UpdateColors();
            }
            break;

        case DARFON_EFFECT_RESET:
            /*-------------------------------------------------*\
            | Off mode - reset device                           |
            \*-------------------------------------------------*/
            controller->Reset();
            break;

        default:
            /*-------------------------------------------------*\
            | Hardware effect modes                             |
            \*-------------------------------------------------*/
            {
                unsigned char r1 = 0, g1 = 0, b1 = 0;
                unsigned char r2 = 0, g2 = 0, b2 = 0;
                unsigned char speed = modes[active_mode].speed;

                if(modes[active_mode].colors.size() >= 1)
                {
                    r1 = RGBGetRValue(modes[active_mode].colors[0]);
                    g1 = RGBGetGValue(modes[active_mode].colors[0]);
                    b1 = RGBGetBValue(modes[active_mode].colors[0]);
                }

                if(modes[active_mode].colors.size() >= 2)
                {
                    r2 = RGBGetRValue(modes[active_mode].colors[1]);
                    g2 = RGBGetGValue(modes[active_mode].colors[1]);
                    b2 = RGBGetBValue(modes[active_mode].colors[1]);
                }

                controller->SetEffect(modes[active_mode].value, speed, r1, g1, b1, r2, g2, b2);
            }
            break;
    }
}
