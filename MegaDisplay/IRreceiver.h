#ifndef __INCLUDE_IR_RECEIVER_H__
#define __INCLUDE_IR_RECEIVER_H__

/*********************************** Pins *************************************/
// Left Score Display
const uint8_t irReceiverPin = 48;

/********************************* Constants **********************************/
// only process one remote command every 500 ms
const uint16_t MIN_REMOTE_MESSAGE_PERIOD_MS = 500;

/*************************** Remote Button Values *****************************/
// fencer A score
const unsigned long FENCER_A_SCORE_INCREASE = 0xF2F0D0;
const unsigned long FENCER_A_SCORE_DECREASE = 0xF2E0D1;

// fencer B score
const unsigned long FENCER_B_SCORE_INCREASE = 0xF2D0D2;
const unsigned long FENCER_B_SCORE_DECREASE = 0xF2C0D3;

// both scores
const unsigned long DOUBLE_TOUCH = 0xE3A1C5;
const unsigned long ZERO_SCORES = 0xE051FA;

// timer
const unsigned long RESET_TIMER = 0xE1C1E3; // apparently TIMER_RESET is already used somewhere?
const unsigned long TIMER_GO_STOP = 0xE151EA;
const unsigned long TIMER_RESET_AND_INCREASE = 0xE191E6;
const unsigned long TIMER_RESET_AND_DECREASE = 0xE1F1E0;

// weapon change
const unsigned long CHANGE_WEAPON_MODE = 0xF3F0C0;

/******************************************************************************/

#endif // #ifndef __INCLUDE_IR_RECEIVER_H__

