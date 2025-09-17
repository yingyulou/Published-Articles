#pragma once

#include "Keyboard.h"
#include "Print.h"
#include "Util.h"

const char __KEYBOARD_MAP_LIST[][2] =
{
    {'\0', '\0'}, {'\0', '\0'}, {'1', '!'}, {'2', '@'}, {'3', '#'}, {'4', '$'}, {'5', '%'}, {'6', '^'}, {'7', '&'}, {'8', '*'},
    {'9', '('}, {'0', ')'}, {'-', '_'}, {'=', '+'}, {'\b', '\b'}, {'\0', '\0'}, {'q', 'Q'}, {'w', 'W'}, {'e', 'E'}, {'r', 'R'},
    {'t', 'T'}, {'y', 'Y'}, {'u', 'U'}, {'i', 'I'}, {'o', 'O'}, {'p', 'P'}, {'[', '{'}, {']', '}'}, {'\n', '\n'}, {'\0', '\0'},
    {'a', 'A'}, {'s', 'S'}, {'d', 'D'}, {'f', 'F'}, {'g', 'G'}, {'h', 'H'}, {'j', 'J'}, {'k', 'K'}, {'l', 'L'}, {';', ':'},
    {'\'', '"'}, {'`', '~'}, {'\0', '\0'}, {'\\', '|'}, {'z', 'Z'}, {'x', 'X'}, {'c', 'C'}, {'v', 'V'}, {'b', 'B'}, {'n', 'N'},
    {'m', 'M'}, {',', '<'}, {'.', '>'}, {'/', '?'}, {'\0', '\0'}, {'\0', '\0'}, {'\0', '\0'}, {' ', ' '},
};

bool __shiftBool    = false;
bool __capsLockBool = false;

void keyboardDriver(uint8_t scanCode)
{
    if (scanCode == 0x2a || scanCode == 0xaa || scanCode == 0x36 || scanCode == 0xb6)
    {
        __shiftBool = !__shiftBool;
    }
    else if (scanCode == 0x3a)
    {
        __capsLockBool = !__capsLockBool;
    }
    else if (scanCode <= 0x39)
    {
        char asciiChar = __KEYBOARD_MAP_LIST[scanCode][__shiftBool ^ (__capsLockBool && (
            (0x10 <= scanCode && scanCode <= 0x19) ||
            (0x1e <= scanCode && scanCode <= 0x26) ||
            (0x2c <= scanCode && scanCode <= 0x32)))];

        if (asciiChar)
        {
            printChar(asciiChar);
        }
    }
}
