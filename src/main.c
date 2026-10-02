//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------
// School: University of Victoria, Canada.
// Course: ECE 355 "Microprocessor-Based Systems".
// This is template code for Part 2 of Introductory Lab.
//
// See "system/include/cmsis/stm32f051x8.h" for register/bit definitions.
// See "system/src/cmsis/vectors_stm32f051x8.c" for handler declarations.
// ----------------------------------------------------------------------------

#include <stdio.h>
#include "diag/Trace.h"
#include "cmsis/cmsis_device.h"

// ----------------------------------------------------------------------------
//
// STM32F0 empty sample (trace via $(trace)).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the $(trace) output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


/* Definitions of registers and their bits are
   given in system/include/cmsis/stm32f051x8.h */


/* Clock prescaler for TIM2 timer: no prescaling */
#define myTIM2_PRESCALER ((uint16_t)0x0000)
/* Maximum possible setting for overflow */
#define myTIM2_PERIOD ((uint32_t)0xFFFFFFFF)

void myGPIOA_Init(void);
void myGPIOB_Init(void);
void myADC_Init(void);
void myDAC_Init(void);
void myTIM2_Init(void);
void myEXTI_Init(void);

// Initialize global variables
float Freq = 6969;  // measured frequency value (global variable)
unsigned int Res = 6969;   // measured resistance value (global variable)

float input_voltage = 0;
float dac_voltage = 0;
float pot_measurement = 0;

int inSig = 0;


// Start of SPI set up

void oled_Write(unsigned char);
void oled_Write_Cmd(unsigned char);
void oled_Write_Data(unsigned char);

void oled_config(void);

void refresh_OLED(void);

SPI_HandleTypeDef SPI_Handle;

// LED Display initialization commands
unsigned char oled_init_cmds[] =
{
    0xAE,
    0x20, 0x00,
    0x40,
    0xA0 | 0x01,
    0xA8, 0x40 - 1,
    0xC0 | 0x08,
    0xD3, 0x00,
    0xDA, 0x32,
    0xD5, 0x80,
    0xD9, 0x22,
    0xDB, 0x30,
    0x81, 0xFF,
    0xA4,
    0xA6,
    0xAD, 0x30,
    0x8D, 0x10,
    0xAE | 0x01,
    0xC0,
    0xA0
};


// Character specifications for LED Display (1 row = 8 bytes = 1 ASCII character)
// Example: to display '4', retrieve 8 data bytes stored in Characters[52][X] row
//          (where X = 0, 1, ..., 7) and send them one by one to LED Display.
// Row number = character ASCII code (e.g., ASCII code of '4' is 0x34 = 52)
unsigned char Characters[][8] = {
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b01011111, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // !
    {0b00000000, 0b00000111, 0b00000000, 0b00000111, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // "
    {0b00010100, 0b01111111, 0b00010100, 0b01111111, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // #
    {0b00100100, 0b00101010, 0b01111111, 0b00101010, 0b00010010,0b00000000, 0b00000000, 0b00000000},  // $
    {0b00100011, 0b00010011, 0b00001000, 0b01100100, 0b01100010,0b00000000, 0b00000000, 0b00000000},  // %
    {0b00110110, 0b01001001, 0b01010101, 0b00100010, 0b01010000,0b00000000, 0b00000000, 0b00000000},  // &
    {0b00000000, 0b00000101, 0b00000011, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // '
    {0b00000000, 0b00011100, 0b00100010, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // (
    {0b00000000, 0b01000001, 0b00100010, 0b00011100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // )
    {0b00010100, 0b00001000, 0b00111110, 0b00001000, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // *
    {0b00001000, 0b00001000, 0b00111110, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // +
    {0b00000000, 0b01010000, 0b00110000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // ,
    {0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // -
    {0b00000000, 0b01100000, 0b01100000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // .
    {0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010,0b00000000, 0b00000000, 0b00000000},  // /
    {0b00111110, 0b01010001, 0b01001001, 0b01000101, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // 0
    {0b00000000, 0b01000010, 0b01111111, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // 1
    {0b01000010, 0b01100001, 0b01010001, 0b01001001, 0b01000110,0b00000000, 0b00000000, 0b00000000},  // 2
    {0b00100001, 0b01000001, 0b01000101, 0b01001011, 0b00110001,0b00000000, 0b00000000, 0b00000000},  // 3
    {0b00011000, 0b00010100, 0b00010010, 0b01111111, 0b00010000,0b00000000, 0b00000000, 0b00000000},  // 4
    {0b00100111, 0b01000101, 0b01000101, 0b01000101, 0b00111001,0b00000000, 0b00000000, 0b00000000},  // 5
    {0b00111100, 0b01001010, 0b01001001, 0b01001001, 0b00110000,0b00000000, 0b00000000, 0b00000000},  // 6
    {0b00000011, 0b00000001, 0b01110001, 0b00001001, 0b00000111,0b00000000, 0b00000000, 0b00000000},  // 7
    {0b00110110, 0b01001001, 0b01001001, 0b01001001, 0b00110110,0b00000000, 0b00000000, 0b00000000},  // 8
    {0b00000110, 0b01001001, 0b01001001, 0b00101001, 0b00011110,0b00000000, 0b00000000, 0b00000000},  // 9
    {0b00000000, 0b00110110, 0b00110110, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // :
    {0b00000000, 0b01010110, 0b00110110, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // ;
    {0b00001000, 0b00010100, 0b00100010, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // <
    {0b00010100, 0b00010100, 0b00010100, 0b00010100, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // =
    {0b00000000, 0b01000001, 0b00100010, 0b00010100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // >
    {0b00000010, 0b00000001, 0b01010001, 0b00001001, 0b00000110,0b00000000, 0b00000000, 0b00000000},  // ?
    {0b00110010, 0b01001001, 0b01111001, 0b01000001, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // @
    {0b01111110, 0b00010001, 0b00010001, 0b00010001, 0b01111110,0b00000000, 0b00000000, 0b00000000},  // A
    {0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b00110110,0b00000000, 0b00000000, 0b00000000},  // B
    {0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00100010,0b00000000, 0b00000000, 0b00000000},  // C
    {0b01111111, 0b01000001, 0b01000001, 0b00100010, 0b00011100,0b00000000, 0b00000000, 0b00000000},  // D
    {0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b01000001,0b00000000, 0b00000000, 0b00000000},  // E
    {0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // F
    {0b00111110, 0b01000001, 0b01001001, 0b01001001, 0b01111010,0b00000000, 0b00000000, 0b00000000},  // G
    {0b01111111, 0b00001000, 0b00001000, 0b00001000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // H
    {0b01000000, 0b01000001, 0b01111111, 0b01000001, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // I
    {0b00100000, 0b01000000, 0b01000001, 0b00111111, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // J
    {0b01111111, 0b00001000, 0b00010100, 0b00100010, 0b01000001,0b00000000, 0b00000000, 0b00000000},  // K
    {0b01111111, 0b01000000, 0b01000000, 0b01000000, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // L
    {0b01111111, 0b00000010, 0b00001100, 0b00000010, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // M
    {0b01111111, 0b00000100, 0b00001000, 0b00010000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // N
    {0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // O
    {0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000110,0b00000000, 0b00000000, 0b00000000},  // P
    {0b00111110, 0b01000001, 0b01010001, 0b00100001, 0b01011110,0b00000000, 0b00000000, 0b00000000},  // Q
    {0b01111111, 0b00001001, 0b00011001, 0b00101001, 0b01000110,0b00000000, 0b00000000, 0b00000000},  // R
    {0b01000110, 0b01001001, 0b01001001, 0b01001001, 0b00110001,0b00000000, 0b00000000, 0b00000000},  // S
    {0b00000001, 0b00000001, 0b01111111, 0b00000001, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // T
    {0b00111111, 0b01000000, 0b01000000, 0b01000000, 0b00111111,0b00000000, 0b00000000, 0b00000000},  // U
    {0b00011111, 0b00100000, 0b01000000, 0b00100000, 0b00011111,0b00000000, 0b00000000, 0b00000000},  // V
    {0b00111111, 0b01000000, 0b00111000, 0b01000000, 0b00111111,0b00000000, 0b00000000, 0b00000000},  // W
    {0b01100011, 0b00010100, 0b00001000, 0b00010100, 0b01100011,0b00000000, 0b00000000, 0b00000000},  // X
    {0b00000111, 0b00001000, 0b01110000, 0b00001000, 0b00000111,0b00000000, 0b00000000, 0b00000000},  // Y
    {0b01100001, 0b01010001, 0b01001001, 0b01000101, 0b01000011,0b00000000, 0b00000000, 0b00000000},  // Z
    {0b01111111, 0b01000001, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // [
    {0b00010101, 0b00010110, 0b01111100, 0b00010110, 0b00010101,0b00000000, 0b00000000, 0b00000000},  // back slash
    {0b00000000, 0b00000000, 0b00000000, 0b01000001, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // ]
    {0b00000100, 0b00000010, 0b00000001, 0b00000010, 0b00000100,0b00000000, 0b00000000, 0b00000000},  // ^
    {0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // _
    {0b00000000, 0b00000001, 0b00000010, 0b00000100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // `
    {0b00100000, 0b01010100, 0b01010100, 0b01010100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // a
    {0b01111111, 0b01001000, 0b01000100, 0b01000100, 0b00111000,0b00000000, 0b00000000, 0b00000000},  // b
    {0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // c
    {0b00111000, 0b01000100, 0b01000100, 0b01001000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // d
    {0b00111000, 0b01010100, 0b01010100, 0b01010100, 0b00011000,0b00000000, 0b00000000, 0b00000000},  // e
    {0b00001000, 0b01111110, 0b00001001, 0b00000001, 0b00000010,0b00000000, 0b00000000, 0b00000000},  // f
    {0b00001100, 0b01010010, 0b01010010, 0b01010010, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // g
    {0b01111111, 0b00001000, 0b00000100, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // h
    {0b00000000, 0b01000100, 0b01111101, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // i
    {0b00100000, 0b01000000, 0b01000100, 0b00111101, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // j
    {0b01111111, 0b00010000, 0b00101000, 0b01000100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // k
    {0b00000000, 0b01000001, 0b01111111, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // l
    {0b01111100, 0b00000100, 0b00011000, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // m
    {0b01111100, 0b00001000, 0b00000100, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // n
    {0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b00111000,0b00000000, 0b00000000, 0b00000000},  // o
    {0b01111100, 0b00010100, 0b00010100, 0b00010100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // p
    {0b00001000, 0b00010100, 0b00010100, 0b00011000, 0b01111100,0b00000000, 0b00000000, 0b00000000},  // q
    {0b01111100, 0b00001000, 0b00000100, 0b00000100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // r
    {0b01001000, 0b01010100, 0b01010100, 0b01010100, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // s
    {0b00000100, 0b00111111, 0b01000100, 0b01000000, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // t
    {0b00111100, 0b01000000, 0b01000000, 0b00100000, 0b01111100,0b00000000, 0b00000000, 0b00000000},  // u
    {0b00011100, 0b00100000, 0b01000000, 0b00100000, 0b00011100,0b00000000, 0b00000000, 0b00000000},  // v
    {0b00111100, 0b01000000, 0b00111000, 0b01000000, 0b00111100,0b00000000, 0b00000000, 0b00000000},  // w
    {0b01000100, 0b00101000, 0b00010000, 0b00101000, 0b01000100,0b00000000, 0b00000000, 0b00000000},  // x
    {0b00001100, 0b01010000, 0b01010000, 0b01010000, 0b00111100,0b00000000, 0b00000000, 0b00000000},  // y
    {0b01000100, 0b01100100, 0b01010100, 0b01001100, 0b01000100,0b00000000, 0b00000000, 0b00000000},  // z
    {0b00000000, 0b00001000, 0b00110110, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // {
    {0b00000000, 0b00000000, 0b01111111, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // |
    {0b00000000, 0b01000001, 0b00110110, 0b00001000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // }
    {0b00001000, 0b00001000, 0b00101010, 0b00011100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // ~
    {0b00001000, 0b00011100, 0b00101010, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000}   // <-
};

// END of spi set up


/*** Call this function to boost the STM32F0xx clock to 48 MHz ***/


void SystemClock48MHz( void ) {
//
// Disable the PLL
//
    RCC->CR &= ~(RCC_CR_PLLON);
//
// Wait for the PLL to unlock
//
    while (( RCC->CR & RCC_CR_PLLRDY ) != 0 );
//
// Configure the PLL for 48-MHz system clock
//
    RCC->CFGR = 0x00280000;
//
// Enable the PLL
//
    RCC->CR |= RCC_CR_PLLON;
//
// Wait for the PLL to lock
//
    while (( RCC->CR & RCC_CR_PLLRDY ) != RCC_CR_PLLRDY );
//
// Switch the processor to the PLL clock source
//
    RCC->CFGR = ( RCC->CFGR & (~RCC_CFGR_SW_Msk)) | RCC_CFGR_SW_PLL;
//
// Update the system with the new clock Freq
//
    SystemCoreClockUpdate();

}


/*****************************************************************/


int
main(int argc, char* argv[]) {

	SystemClock48MHz();

	trace_printf("This is Part 2 of Introductory Lab...\n");
	trace_printf("System clock: %u Hz\n", SystemCoreClock);

	myGPIOA_Init();		/* Initialize I/O port PA */
	myGPIOB_Init();		/* Initialize I/O port PB */
	myADC_Init();		/* Initialize ADC */
	myDAC_Init();		/* Initialize DAC */
	myTIM2_Init();		/* Initialize timer TIM2 */
	myEXTI_Init();		/* Initialize EXTI */
	oled_config();		/* Initialize LED Display */

	while (1) {

		// Start the ADC conversion
		ADC1->CR |= ADC_CR_ADSTART;

		// Ensure the ADC conversion is completed
		while((ADC1->ISR & ADC_ISR_EOC) == 0);

		// Reset ADC conversion flag
		ADC1->ISR &= ~(ADC_ISR_EOC);

		// Get converted ADC value from potentiometer measurement
		pot_measurement = (uint16_t)ADC1->DR & 0x0FFFF;

		// Convert the ADC potentiometer measurement to resistance
		Res = 5000 * ( (pot_measurement) / ((float)0XFFF) );
		// 0xFFF -> 4095 which is the max timeout for the board timers

		trace_printf("ADC Value: %f\n", pot_measurement);



		// Determine voltage value
		input_voltage = (uint16_t)ADC1->DR;

		DAC->DHR12R1 = pot_measurement;

		// Enable switch trigger
		DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;

		// trace_printf(" ", )

		refresh_OLED();

	}

	return 0;

}


// LED Display Functions

void refresh_OLED( void ) {

    // Buffer size = at most 16 characters per PAGE + terminating '\0'
    unsigned char Buffer[17];

   snprintf( Buffer, sizeof( Buffer ), "R: %5u Ohms", Res );
    /* Buffer now contains your character ASCII codes for LED Display
       - select PAGE (LED Display line) and set starting SEG (column)
       - for each c = ASCII code = Buffer[0], Buffer[1], ...,
           send 8 bytes in Characters[c][0-7] to LED Display
    */
   oled_Write_Cmd(0xB2);
   oled_Write_Cmd(0x03);
   oled_Write_Cmd(0x10);
   for(int i=0;Buffer[i] != '\0';i++){
	   for(int j =0;j<8;j++){
		   oled_Write_Data(Characters[Buffer[i]][j]);
	   }

   }

    snprintf( Buffer, sizeof( Buffer ), "F: %5u Hz", (uint32_t)Freq );
    /* Buffer now contains your character ASCII codes for LED Display
       - select PAGE (LED Display line) and set starting SEG (column)
       - for each c = ASCII code = Buffer[0], Buffer[1], ...,
           send 8 bytes in Characters[c][0-7] to LED Display
    */
    oled_Write_Cmd(0xB5);
    oled_Write_Cmd(0x03);
    oled_Write_Cmd(0x10);
    for(int i=0;Buffer[i] != '\0';i++){
	   for(int j =0;j<8;j++){
		   oled_Write_Data(Characters[Buffer[i]][j]);
	   }

   }

    for(int i=0xFF;i>0x0;i--){ // Delay Timer
    }

}

void oled_Write_Cmd( unsigned char cmd ) {

	GPIOB->ODR &= ~(GPIO_ODR_6);// make PB6 = CS# = 0
    GPIOB->ODR &= ~(GPIO_ODR_7); // make PB7 = D/C# = 0
    GPIOB->ODR &= ~(GPIO_ODR_6); // make PB6 = CS# = 0
    oled_Write( cmd );
    GPIOB->ODR |= GPIO_ODR_6; // make PB6 = CS# = 1

}

void oled_Write_Data( unsigned char data ) {

    GPIOB->ODR &= ~(GPIO_ODR_6); // make PB6 = CS# = 0
	GPIOB->ODR |= GPIO_ODR_7; // make PB7 = D/C# = 1
	GPIOB->ODR &= ~(GPIO_ODR_6); // make PB6 = CS# = 0
	oled_Write( data );
	GPIOB->ODR |= GPIO_ODR_6; // make PB6 = CS# = 1

}

void oled_Write( unsigned char Value ) {

    /* Wait until SPI1 is ready for writing (TXE = 1 in SPI1_SR) */
	while (!(SPI1->SR & SPI_SR_TXE));

    /* Send one 8-bit character:
       - This function also sets BIDIOE = 1 in SPI1_CR1
    */
    HAL_SPI_Transmit( &SPI_Handle, &Value, 1, HAL_MAX_DELAY );

    /* Wait until transmission is complete (TXE = 1 in SPI1_SR) */
    while (!(SPI1->SR & SPI_SR_TXE));

}

void oled_config( void ) {

// Don't forget to enable GPIOB clock in RCC - COMPLETED (GPIOB_Init Function)
// Don't forget to configure PB3/PB5 as AF0 - COMPLETED (GPIOB_Init Function)
// Don't forget to enable SPI1 clock in RCC - COMPLETED (LINE 500)

	// Enable SPI1 clock in RCC
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    SPI_Handle.Instance = SPI1;

    SPI_Handle.Init.Direction = SPI_DIRECTION_1LINE;
    SPI_Handle.Init.Mode = SPI_MODE_MASTER;
    SPI_Handle.Init.DataSize = SPI_DATASIZE_8BIT;
    SPI_Handle.Init.CLKPolarity = SPI_POLARITY_LOW;
    SPI_Handle.Init.CLKPhase = SPI_PHASE_1EDGE;
    SPI_Handle.Init.NSS = SPI_NSS_SOFT;
    SPI_Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    SPI_Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SPI_Handle.Init.CRCPolynomial = 7;

    // Initialize the SPI interface
    HAL_SPI_Init( &SPI_Handle );

    // Enable the SPI
    __HAL_SPI_ENABLE( &SPI_Handle );

    /* Reset LED Display (RES# = PB4):
       - make pin PB4 = 0, wait for a few ms
       - make pin PB4 = 1, wait for a few ms
    */
    GPIOB->BSRR &= ~(GPIO_BSRR_BS_4); // Make pin PB4 = 0
    for(int i=0xF;i>0x0;i--){
    	//    sleep(2); // Wait for a few ms - decrementing for loop based on clock speed
    }

	GPIOB->BSRR = GPIO_BSRR_BS_4; // Setting Pin PB4 = 1
	for(int i=0xF;i>0x0;i--){
		//	sleep(2); // Wait for a few ms - decrementing for loop based on clock speed
	}


	// Send initialization commands to LED Display
    for ( unsigned int i = 0; i < sizeof( oled_init_cmds ); i++ )
    {
        oled_Write_Cmd( oled_init_cmds[i] );
    }

    /* Fill LED Display data memory (GDDRAM) with zeros:
       - for each PAGE = 0, 1, ..., 7
           set starting SEG = 0
           call oled_Write_Data( 0x00 ) 128 times
    */
    for ( unsigned int i = 0; i < 8; i++) {

    	for ( unsigned int j = 0x0; j < 128; j++ ) {

    		oled_Write_Cmd((0xB0 | i)); // Page selection 0 - 7
    		oled_Write_Cmd(0x00 | (j & 0xF));
    		oled_Write_Cmd(0x10 | ((j & 0xF0) >> 4));
    		oled_Write_Data(0x00);

    	}
    }
}

// End of LED Display Functions


void myGPIOA_Init() {

	/* Enable clock for GPIOA peripheral */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	/* Configure PA2 as input */
	GPIOA->MODER &= ~(GPIO_MODER_MODER2);

	/* Ensure no pull-up/pull-down for PA2 */
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR2);

	/* Configure PA4 as input */
	GPIOA->MODER |= GPIO_MODER_MODER4;

	/* Ensure no pull-up/pull-down for PA4 */
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR4);


	// User Button Configuration:

	/* Configure PA0 as input */
	GPIOA->MODER &= ~(GPIO_MODER_MODER0);

	/* Ensure no pull-up/pull-down for PA0 */
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0);

}

void myGPIOB_Init() {

	/* Enable clock for GPIOB peripheral */
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	/* Configure PB3 to use alternate function SCK */
	GPIOB->MODER |= GPIO_MODER_MODER3_1;

	/* Ensure no pull-up/pull-down for PB3 */
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR3);

	/* Configure PB4 as RES# (Reset) */
	GPIOB->MODER |= GPIO_MODER_MODER4_0;

	/* Ensure no pull-up/pull-down for PB4 */
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR4);

	/* Configure PB5 as output MOSI (Serial Data) */
	GPIOB->MODER |= GPIO_MODER_MODER5_1;

	/* Ensure no pull-up/pull-down for PB5 */
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR5);

	/* Configure board to recognize PB5 is being used as alternate function SCK (Serial Clock) */
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL0;

	/* Configure PB6 as CS# (Chip Select) */
	GPIOB->MODER |= GPIO_MODER_MODER6_0;

	/* Configure PB7 as D/C# (Data/Command) */
	GPIOB->MODER |= GPIO_MODER_MODER7_0;

}

void myGPIOC_Init() {

	/* Enable clock for GPIOC peripheral */
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	/* Configure PC8 and PC9 as outputs */
	GPIOC->MODER |= (GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0);

	/* Ensure push-pull mode selected for PC8 and PC9 */
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_8 | GPIO_OTYPER_OT_9);

	/* Ensure high-speed mode for PC8 and PC9 */
	GPIOC->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9);

	/* Ensure no pull-up/pull-down for PC8 and PC9 */
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR8 | GPIO_PUPDR_PUPDR9);

}

void myADC_Init() {

	// SPI1 clock enable for the ADC
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;

	// ADC clock enable
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;

	// Configure ADC for overrun mode
	ADC1->CFGR1 |= ADC_CFGR1_OVRMOD;

	// Configure ADC for continuous mode
	ADC1->CFGR1 |= ADC_CFGR1_CONT;

	// Configure ADC for right-side data alignment
	ADC1->CFGR1 &= ~(ADC_CFGR1_ALIGN);

	// Set ADC to use 12-bit resolution
	ADC1->CFGR1 &= ~(ADC_CFGR1_RES);

	// Configure ADC to select channel 5 for regular sequencer
	ADC1->CHSELR |= ADC_CHSELR_CHSEL5;

	// Configure ADC to use fastest sampling speed
	ADC1->SMPR &= ~(ADC_SMPR_SMP);

	// ADC enable
	ADC1->CR |= ADC_CR_ADEN;
	while ((ADC1->ISR & ADC_ISR_ADRDY) != 1);

}

void myDAC_Init() {

	// DAC clock enable
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;

	// DAC enable channel 1
	DAC->CR |= DAC_CR_EN1;

}

void myTIM2_Init() {

	// Enable clock for TIM2 peripheral
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	// Configure TIM2: buffer auto-reload, count up, stop on overflow,
	// enable update events, interrupt on overflow only
	TIM2->ARR = myTIM2_PERIOD;
	TIM2->CR1 |= (uint16_t)0x8C;
	TIM2->CR1 &= (uint16_t)0xFF8C;

	// Set clock prescaler value
	TIM2->PSC = myTIM2_PRESCALER;

	// Update timer registers with above changes
	TIM2->EGR |= TIM_EGR_UG;

	// Assign TIM2 interrupt priority = 0 (highest) in NVIC
	NVIC_SetPriority(15, 0);

	// Enable TIM2 interrupts in NVIC
	NVIC_EnableIRQ(15);

	// Enable TIM2 interrupt generation
	TIM2->DIER |= TIM_DIER_UIE;

}


void myEXTI_Init() {

	//Function Generator Interrupt Configurations:

	/* Map EXTI2 line to PA2 */
	// Relevant register: SYSCFG->EXTICR[0]
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2;

	/* EXTI2 line interrupts: set rising-edge trigger */
	// Relevant register: EXTI->RTSR
	EXTI->RTSR |= EXTI_RTSR_TR2;

	/* Unmask interrupts from EXTI2 line */
	// Relevant register: EXTI->IMR
	EXTI->IMR |= EXTI_IMR_MR2;

	/* Assign EXTI2 interrupt priority = 0 in NVIC */
	// Relevant register: NVIC->IP[2], or use NVIC_SetPriority
	NVIC_SetPriority(EXTI2_3_IRQn, 1);

	/* Enable EXTI2 interrupts in NVIC */
	// Relevant register: NVIC->ISER[0], or use NVIC_EnableIRQ
	NVIC_EnableIRQ(EXTI2_3_IRQn);

	//End of Function Generator Interrupt Configurations



	//555 Timer / User Button Interrupt Configurations:

	// Map EXTI1 line to PA1
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1;

	// Map EXTI0 line to PA0 (User Button)
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0;

	// Configure EXTI1 line to interrupt using rising-edge trigger
	EXTI->RTSR |= EXTI_RTSR_TR1;

	// Configure EXTI0 line to interrupt using rising-edge trigger (User Button)
	EXTI->RTSR |= EXTI_RTSR_TR0;

	// Enable interrupts from EXTI1 line
	EXTI->IMR |= EXTI_IMR_MR1;

	// Enable interrupts from EXTI0 line (User Button)
	EXTI->IMR |= EXTI_IMR_MR0;

	// Assign EXTI1 interrupt priority = 0 (highest) in NVIC
	NVIC_SetPriority(EXTI0_1_IRQn, 0);

	// Enable EXTI1 interrupts in NVIC
	NVIC_EnableIRQ(EXTI0_1_IRQn);

	//End of 555 Timer / User Button Interrupt Configurations

	EXTI->IMR &= ~(EXTI_IMR_MR1);
	EXTI->IMR |= EXTI_IMR_MR2;

}

/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void TIM2_IRQHandler() {

	// Check that TIM2 interrupt flag is set
	if ((TIM2->SR & TIM_SR_UIF) != 0) {

		// TIM2 overflow has occurred
		trace_printf("\n*** Overflow! ***\n");

		// Clear interrupt flag
		TIM2->SR &= (uint32_t)0xFFFE;

		// Restart stopped timer
		TIM2->CR1 |= (uint16_t)0x1;

	}

}

void calcFreq() {

	// Initialize local variables for calculations
	float measuredCount = 0.0;

	// 1. This is the second edge:
	if ((TIM2->CR1 & TIM_CR1_CEN) == 1) {

		TIM2->CR1 &= ~(TIM_CR1_CEN); // Stop timer
		measuredCount = TIM2->CNT; // Get timer value

		if (measuredCount < SystemCoreClock) {

			Freq = ((float)SystemCoreClock/measuredCount);// Frequency Calculation

		} else {

			Freq = (measuredCount/(float)SystemCoreClock);// Frequency Calculation

		}

	}

	// 2. This is the first edge:
	else {

		TIM2->CNT = (uint32_t)0x0;

		/* ENABLE TIM2 COUNTER i.e. start counter because input signal detected */
		TIM2->CR1 |= TIM_CR1_CEN;

	}

}

void EXTI0_1_IRQHandler() {

	if ((EXTI->PR & EXTI_PR_PR1) != 0 ) {

		calcFreq();

		// Reset interrupt flag
		EXTI->PR = EXTI_PR_PR1;

	}

	if ((EXTI->PR & EXTI_PR_PR0) != 0 ) {
	/* If inSig = 0, then: let inSig = 1, disable
	EXTI1 interrupts, enable EXTI2 interrupts */
	/* Else: let inSig = 0, disable EXTI2 interrupts,
	enable EXTI1 interrupts */

		inSig = (inSig ^ 0x1);

		if ((inSig & 0x1) == 1) {
			// Mask Line 2, unmask Line 1
			EXTI->IMR &= ~(EXTI_IMR_MR2);
			EXTI->IMR |= EXTI_IMR_MR1;
		} else {
			// Mask Line 1, unmask Line 2
			EXTI->IMR &= ~(EXTI_IMR_MR1);
			EXTI->IMR |= EXTI_IMR_MR2;
		}

		// reset interrupt flag
		EXTI->PR = EXTI_PR_PR0;

	}

}

/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void EXTI2_3_IRQHandler() {

	/* Check if EXTI2 interrupt pending flag is indeed set */
	if ((EXTI->PR & EXTI_PR_PR2) != 0) {

		calcFreq();

	}

	EXTI->PR = EXTI_PR_PR2;

}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
