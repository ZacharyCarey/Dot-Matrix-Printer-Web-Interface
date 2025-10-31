#ifndef OKIDATA120_H
#define OKIDATA120_H

#include <stdint.h>
#include <stddef.h>

const uint8_t CMD_CHAR_SIZE_PICA = 15; // 10 CPI
const uint8_t CMD_CHAR_SIZE_ELITE = 28; // 12 CPI
const uint8_t CMD_CHAR_SIZE_FINE = 29; // 17.1 CPI

const uint8_t CMD_DOUBLE_WIDTH_ON = 14;
const uint8_t CMD_DOUBLE_WIDTH_OFF = 129;

const uint8_t CMD_UNDERLINE = 27; // Send this before an underline cmd
const uint8_t CMD_UNDERLINE_START = 67;
const uint8_t CMD_UNDERLINE_STOP = 68;

const uint8_t CMD_SCRIPT = 27; // Send this before a script cmd
const uint8_t CMD_SCRIPT_SUB_START = 76;
const uint8_t CMD_SCRIPT_SUB_STOP = 77;
const uint8_t CMD_SCRIPT_SUPER_START = 74;
const uint8_t CMD_SCRIPT_SUPER_STOP = 75;

const uint8_t CMD_BOLD = 27; // Send this before a bold cmd
const uint8_t CMD_BOLD_ENHANCED_START = 72;
const uint8_t CMD_BOLD_EMPHASIZED_START = 84;
const uint8_t CMD_BOLD_STOP = 73;

const uint8_t CMD_CURSOR_UP = 145;
const uint8_t CMD_CURSOR_DOWN = 17;

const uint8_t CMD_REVERSE_PRINT_START = 18;
const uint8_t CMD_REVERSE_PRINT_STOP = 146;

const uint8_t CMD_CR_WITH_LINE_FEED = 13;
const uint8_t CMD_CR_WITHOUT_LINE_FEED = 141;

const uint8_t CMD_LINE_FEED = 10;

const uint8_t CMD_LINE_SPACING = 27; // Send this before any line spacing commands
const uint8_t CMD_LINE_SPACING_6_LPI = 54;
const uint8_t CMD_LINE_SPACING_8_LPI = 56;
const uint8_t CMD_LINE_SPACING_CUSTOM = 10; // after this send 'n' where n = [0, 255], n/144" lines per inch

const uint8_t CMD_FORM_FEED = 12;

const uint8_t CMD_SET_FORM_LENGTH_1 = 27;
const uint8_t CMD_SET_FORM_LENGTH_2 = 70; // Following this, send 'nn' lines. (See page 31 in documentation)

const uint8_t CMD_CANCEL = 24; // Resets printer to default settings

const uint8_t CMD_GRAPHICS_START = 8;
const uint8_t CMD_GRAPHICS_DENSITY = 27; // Send this before other density commands
const uint8_t CMD_GRAPHICS_DENSITY_NORMAL = 80;
const uint8_t CMD_GRAPHICS_DENSITY_GRAPHICS = 81;
const uint8_t CMD_GRAPHICS_REPEAT = 26; // After this send 'n' number of times to repeat

size_t okidata_translate(uint8_t* data, size_t length, uint8_t* out_buffer, size_t out_buffer_len);

#endif