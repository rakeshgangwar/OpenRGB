/*---------------------------------------------------------*\
| RGBController_DarfonKeyboard.cpp                          |
|                                                           |
|   RGBController for Darfon per-key RGB keyboards built    |
|   into Alienware laptops                                  |
|                                                           |
|   Rakesh Gangwar                              01 Oct 2026 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#include "RGBControllerKeyNames.h"
#include "RGBController_DarfonKeyboard.h"

#define DARFON_MATRIX_HEIGHT    6
#define DARFON_MATRIX_WIDTH     16
#define NA                      0xFFFFFFFF

/*---------------------------------------------------------*\
| The firmware addresses LEDs as row * 20 + column on a     |
| 7 x 20 grid that does not follow the physical rows: the   |
| four media keys at the right edge are in grid row 0, and  |
| Page Up is in the bottom row's grid row.                  |
| The space bar has no LED.                                 |
|                                                           |
| Alienware x15 R2, US ANSI layout                          |
\*---------------------------------------------------------*/
typedef struct
{
    const char*     name;
    unsigned char   led_id;
    unsigned char   row;
    unsigned char   col;
} darfon_key;

static const darfon_key darfon_x15_r2_keys[] =
{
    { KEY_EN_ESCAPE,            0,   0,  0 },
    { KEY_EN_F1,                1,   0,  1 },
    { KEY_EN_F2,                2,   0,  2 },
    { KEY_EN_F3,                3,   0,  3 },
    { KEY_EN_F4,                4,   0,  4 },
    { KEY_EN_F5,                5,   0,  5 },
    { KEY_EN_F6,                6,   0,  6 },
    { KEY_EN_F7,                7,   0,  7 },
    { KEY_EN_F8,                8,   0,  8 },
    { KEY_EN_F9,                9,   0,  9 },
    { KEY_EN_F10,               10,  0, 10 },
    { KEY_EN_F11,               11,  0, 11 },
    { KEY_EN_F12,               12,  0, 12 },
    { KEY_EN_HOME,              13,  0, 13 },
    { KEY_EN_END,               14,  0, 14 },
    { KEY_EN_DELETE,            15,  0, 15 },

    { KEY_EN_BACK_TICK,         20,  1,  0 },
    { KEY_EN_1,                 21,  1,  1 },
    { KEY_EN_2,                 22,  1,  2 },
    { KEY_EN_3,                 23,  1,  3 },
    { KEY_EN_4,                 24,  1,  4 },
    { KEY_EN_5,                 25,  1,  5 },
    { KEY_EN_6,                 26,  1,  6 },
    { KEY_EN_7,                 27,  1,  7 },
    { KEY_EN_8,                 28,  1,  8 },
    { KEY_EN_9,                 29,  1,  9 },
    { KEY_EN_0,                 30,  1, 10 },
    { KEY_EN_MINUS,             31,  1, 11 },
    { KEY_EN_EQUALS,            32,  1, 12 },
    { KEY_EN_BACKSPACE,         39,  1, 13 },
    { "Key: Microphone Mute",   19,  1, 15 },

    { KEY_EN_TAB,               40,  2,  0 },
    { KEY_EN_Q,                 42,  2,  1 },
    { KEY_EN_W,                 43,  2,  2 },
    { KEY_EN_E,                 44,  2,  3 },
    { KEY_EN_R,                 45,  2,  4 },
    { KEY_EN_T,                 46,  2,  5 },
    { KEY_EN_Y,                 47,  2,  6 },
    { KEY_EN_U,                 48,  2,  7 },
    { KEY_EN_I,                 49,  2,  8 },
    { KEY_EN_O,                 50,  2,  9 },
    { KEY_EN_P,                 51,  2, 10 },
    { KEY_EN_LEFT_BRACKET,      52,  2, 11 },
    { KEY_EN_RIGHT_BRACKET,     53,  2, 12 },
    { KEY_EN_ANSI_BACK_SLASH,   55,  2, 13 },
    { KEY_EN_MEDIA_MUTE,        16,  2, 15 },

    { KEY_EN_CAPS_LOCK,         61,  3,  0 },
    { KEY_EN_A,                 62,  3,  1 },
    { KEY_EN_S,                 63,  3,  2 },
    { KEY_EN_D,                 64,  3,  3 },
    { KEY_EN_F,                 65,  3,  4 },
    { KEY_EN_G,                 66,  3,  5 },
    { KEY_EN_H,                 67,  3,  6 },
    { KEY_EN_J,                 68,  3,  7 },
    { KEY_EN_K,                 69,  3,  8 },
    { KEY_EN_L,                 70,  3,  9 },
    { KEY_EN_SEMICOLON,         71,  3, 10 },
    { KEY_EN_QUOTE,             72,  3, 11 },
    { KEY_EN_ANSI_ENTER,        74,  3, 13 },
    { KEY_EN_MEDIA_VOLUME_UP,   18,  3, 15 },

    { KEY_EN_LEFT_SHIFT,        81,  4,  0 },
    { KEY_EN_Z,                 83,  4,  1 },
    { KEY_EN_X,                 84,  4,  2 },
    { KEY_EN_C,                 85,  4,  3 },
    { KEY_EN_V,                 86,  4,  4 },
    { KEY_EN_B,                 87,  4,  5 },
    { KEY_EN_N,                 88,  4,  6 },
    { KEY_EN_M,                 89,  4,  7 },
    { KEY_EN_COMMA,             90,  4,  8 },
    { KEY_EN_PERIOD,            91,  4,  9 },
    { KEY_EN_FORWARD_SLASH,     92,  4, 10 },
    { KEY_EN_RIGHT_SHIFT,       94,  4, 12 },
    { KEY_EN_PAGE_UP,           114, 4, 14 },
    { KEY_EN_MEDIA_VOLUME_DOWN, 17,  4, 15 },

    { KEY_EN_LEFT_CONTROL,      100, 5,  0 },
    { KEY_EN_LEFT_FUNCTION,     101, 5,  1 },
    { KEY_EN_LEFT_WINDOWS,      103, 5,  2 },
    { KEY_EN_LEFT_ALT,          104, 5,  3 },
    { KEY_EN_RIGHT_ALT,         111, 5, 10 },
    { KEY_EN_RIGHT_WINDOWS,     109, 5, 11 },
    { KEY_EN_RIGHT_CONTROL,     112, 5, 12 },
    { KEY_EN_LEFT_ARROW,        133, 5, 13 },
    { KEY_EN_PAGE_DOWN,         134, 5, 14 },
    { KEY_EN_RIGHT_ARROW,       135, 5, 15 },
};

#define DARFON_X15_R2_KEY_COUNT (sizeof(darfon_x15_r2_keys) / sizeof(darfon_x15_r2_keys[0]))

/**------------------------------------------------------------------*\
    @name Alienware Laptop Keyboard (Darfon)
    @category Keyboard
    @type USB
    @save :x:
    @direct :white_check_mark:
    @effects :x:
    @detectors DetectDarfonKeyboardControllers
    @comment Per-key RGB keyboard built into the Alienware x15 R2.
    The firmware powers the LEDs only once the operating system has
    reported "display on" through ACPI (`\_SB.PEPD._DSM`, function 4).
    Linux sends that on resume from s2idle but not at boot, so after a
    fresh boot the keyboard accepts colours and stays dark until the
    notification is sent or the laptop has been suspended once.
\*-------------------------------------------------------------------*/

RGBController_DarfonKeyboard::RGBController_DarfonKeyboard(DarfonKeyboardController* controller_ptr)
{
    controller                      = controller_ptr;

    name                            = controller->GetNameString();
    vendor                          = "Darfon";
    type                            = DEVICE_TYPE_KEYBOARD;
    description                     = "Alienware Laptop Keyboard (Darfon)";
    location                        = controller->GetDeviceLocation();
    serial                          = controller->GetSerialString();

    mode Direct;
    Direct.name                     = "Direct";
    Direct.value                    = 0;
    Direct.flags                    = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode               = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    SetupZones();
}

RGBController_DarfonKeyboard::~RGBController_DarfonKeyboard()
{
    Shutdown();

    delete controller;
}

void RGBController_DarfonKeyboard::SetupZones()
{
    unsigned int matrix_map[DARFON_MATRIX_HEIGHT][DARFON_MATRIX_WIDTH];

    for(unsigned int row = 0; row < DARFON_MATRIX_HEIGHT; row++)
    {
        for(unsigned int col = 0; col < DARFON_MATRIX_WIDTH; col++)
        {
            matrix_map[row][col] = NA;
        }
    }

    for(unsigned int led_idx = 0; led_idx < DARFON_X15_R2_KEY_COUNT; led_idx++)
    {
        led new_led;

        new_led.name                = darfon_x15_r2_keys[led_idx].name;
        new_led.value               = darfon_x15_r2_keys[led_idx].led_id;
        leds.push_back(new_led);

        matrix_map[darfon_x15_r2_keys[led_idx].row][darfon_x15_r2_keys[led_idx].col] = led_idx;
    }

    zone keyboard_zone;

    keyboard_zone.name              = ZONE_EN_KEYBOARD;
    keyboard_zone.type              = ZONE_TYPE_MATRIX;
    keyboard_zone.leds_min          = DARFON_X15_R2_KEY_COUNT;
    keyboard_zone.leds_max          = DARFON_X15_R2_KEY_COUNT;
    keyboard_zone.leds_count        = DARFON_X15_R2_KEY_COUNT;
    keyboard_zone.matrix_map.Set(DARFON_MATRIX_HEIGHT, DARFON_MATRIX_WIDTH, (unsigned int *)matrix_map);
    zones.push_back(keyboard_zone);

    SetupColors();
}

void RGBController_DarfonKeyboard::DeviceUpdateLEDs()
{
    std::vector<unsigned char> led_ids;

    for(std::size_t led_idx = 0; led_idx < leds.size(); led_idx++)
    {
        led_ids.push_back((unsigned char)leds[led_idx].value);
    }

    controller->SetLEDs(led_ids, colors);
}

void RGBController_DarfonKeyboard::DeviceUpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_DarfonKeyboard::DeviceUpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_DarfonKeyboard::DeviceUpdateMode()
{
    DeviceUpdateLEDs();
}
