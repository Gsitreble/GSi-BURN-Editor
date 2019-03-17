///////////////////////////////////////////////////////////////////////////
// GSi BURN Editor - Version 1.0 - October 2013
// Uses wxWidgets and RtMidi for cross platform (Win and Mac) compatibility
//
// www.GenuineSoundware.com
//
///////////////////////////////////////////////////////////////////////////

#ifndef __BURNED_MAIN_H__
#define __BURNED_MAIN_H__

#include "stdafx.h"
#include "RtMidi.h"


// Basic definitions
#define APPLICATION_NAME	"BURN Editor v.1.0"
#define MAIN_WIN_SIZE		970,520

#ifdef __WXMAC__
#define LABELSIZE		70
#define	SLIDERSIZE		80
#define LISTHEIGHT		330
#else
#define LABELSIZE		60
#define	SLIDERSIZE		90
#define LISTHEIGHT		310
#endif

#define FILEDESCLEN			64
#define NUMBEROFPRESETS		32
#define PRESETCHUNKSIZE		64
#define ACTUALPRESETSIZE	35

// Define location in preset memory block
#define PRESET_LOC_FX			12
#define PRESET_LOC_TUBE_SW		13
#define PRESET_LOC_TUBE_GAIN	14
#define PRESET_LOC_TUBE_LEVEL	15
#define PRESET_LOC_PARAM1		16
#define PRESET_LOC_PARAM2		17
#define PRESET_LOC_PARAM3		18
#define PRESET_LOC_BYPASS		19
#define PRESET_LOC_NRPARAM0		20
#define PRESET_LOC_NRPARAM1		21
#define PRESET_LOC_NRPARAM2		22
#define PRESET_LOC_NRPARAM3		23
#define PRESET_LOC_NRPARAM4		24
#define PRESET_LOC_NRPARAM5		25
#define PRESET_LOC_NRPARAM6		26
#define PRESET_LOC_NRPARAM7		27
#define PRESET_LOC_NRPARAM8		28
#define PRESET_LOC_NRPARAM9		29
#define PRESET_LOC_NRPARAM10	30
#define PRESET_LOC_NRPARAM11	31
#define PRESET_LOC_NRPARAM12	32
#define PRESET_LOC_NRPARAM13	33
#define PRESET_LOC_NRPARAM14	34


enum
{
    TIMER_ID				= 1010,

	ID_MENU_IMPORT			= 2001,
	ID_MENU_EXPORT_PRESET	= 2002,
	ID_MENU_EXPORT_BANK		= 2003,
	ID_MENU_QUIT			= 2004,

	ID_MENU_MIDI_CONNECT	= 2011,
	ID_MENU_MIDI_DISCONNECT	= 2012,
	ID_MENU_MIDI_INPUT		= 2100,
	ID_MENU_MIDI_OUTPUT		= 2200,

	ID_BTN_CONNECT			= 3001,
	ID_BTN_DISCONNECT		= 3002,
	ID_BTN_WRITEPRESET		= 3003,
	ID_BTN_INITPRESET		= 3004,
	ID_BTN_UPLOADBANK		= 3005,
	ID_BTN_DWNBANK			= 3006,
	ID_BTN_DWNPRESET		= 3007,
	ID_BTN_MOVEUP			= 3008,
	ID_BTN_MOVEDOWN			= 3009,

	ID_MENU_HELP_ABOUT		= 4001,

	ID_PRESETLIST			= 5001,

	ID_CHK_BYPASS			= 6001,
	ID_CHK_OVERDRIVE		= 6002,
	ID_SLIDER_GAIN			= 6003,
	ID_SLIDER_LEVEL			= 6004,
	ID_SLIDER_PARAM1		= 6005,
	ID_SLIDER_PARAM2		= 6006,
	ID_SLIDER_PARAM3		= 6007,

	ID_CMB_NRTP0			= 7000,
	ID_CMB_NRTP1			= 7001,
	ID_CMB_NRTP2			= 7002,
	ID_CMB_NRTP3			= 7003,
	ID_CMB_NRTP4			= 7004,
	ID_CMB_NRTP5			= 7005,
	ID_CMB_NRTP6			= 7006,
	ID_CMB_NRTP7			= 7007,
	ID_CMB_NRTP8			= 7008,
	ID_CMB_NRTP9			= 7009,
	ID_CMB_NRTP10			= 7010,
	ID_CMB_NRTP11			= 7011,
	ID_CMB_NRTP12			= 7012,
	ID_CMB_NRTP13			= 7013,
	ID_CMB_NRTP14			= 7014,

	ID_FX_SELECT			= 8000,
	ID_TXT_PRESETNAME		= 8001

};


/////////////////////////////////////////////////////////////////////////////
// Effects
/////////////////////////////////////////////////////////////////////////////
#define TOTEFFECTS			26	// +1
// WARNING: update the following IDs if TOTEFFECTS changes!!!
#define LAST_ROTARY4ORGAN	8	// ID of last Rotary effect for organ (with all NR parameters)
#define LAST_ROTARY_FX		10	// ID of last Rotary effect (including those with no NR parameters)
#define FX_UTIL_PASSTHRU	25	// ID of effect PASS THRU
#define FX_UTIL_MUTE		26	// ID of effect MUTE

typedef struct
{
	unsigned char name[17];		// effect name
	unsigned char prm1[15];		// parameter 1 name
	unsigned char prm2[15];		// parameter 2 name
	unsigned char prm3[15];		// parameter 3 name
	unsigned char def_val_1;
	unsigned char def_val_2;
	unsigned char def_val_3;
	unsigned char show_prm3;	// 0 = hide; 1 = 0...127; // DEPRECATED: 2 = On/Off (0/127)
} EffectStr;

// NOTE: The first 8 are Rotary and use all NR parameters, others only use the last 2 NR params
const EffectStr EffectData[TOTEFFECTS+1] =
{
	{	"ROTARY: ROCK 147",	"HORN LEVEL   ", "REVERB       ", "SPEED        ", 100, 24,  0, 0 },//  0
	{	"ROTARY: JAZZ 122",	"HORN LEVEL   ", "REVERB       ", "SPEED        ", 100, 24,  0, 0 },//  1
	{	"ROTARY: DISTANT ",	"BALANCE      ", "REVERB       ", "SPEED        ",  64, 24,  0, 0 },//  2
	{	"ROTARY: CLOSER  ",	"BALANCE      ", "REVERB       ", "SPEED        ",  64, 24,  0, 0 },//  3
	{	"ROTARY: XY MIKES",	"BALANCE      ", "REVERB       ", "SPEED        ",  64, 24,  0, 0 },//  4
	{	"ROTARY: ROCK 1  ",	"BALANCE      ", "REVERB       ", "SPEED        ",  64, 24,  0, 0 },//  5
	{	"ROTARY: ROCK 2  ",	"BALANCE      ", "REVERB       ", "SPEED        ",  64, 24,  0, 0 },//  6
	{	"ROTARY: MONO    ",	"BALANCE      ", "REVERB       ", "SPEED        ",  64, 24,  0, 0 },//  7
	{	"ROTARY: SPLIT   ",	"BALANCE      ", "REVERB       ", "SPEED        ",  64, 24,  0, 0 },//  8	// LAST ROTARY FOR ORGAN
	{	"AMP: MODEL 16   ",	"MID. FREQ.   ", "REVERB       ", "SPEED        ",  64, 24,  0, 0 },//  9
	{	"AMP: RA-200     ",	"BALANCE      ", "REVERB       ", "SPEED        ",  64, 24,  0, 0 },//  10	// LAST ROTARY FX
	{	"AMP: VARISPEED  ",	"MID. FREQ.   ", "REVERB       ", "ROTARY SPEED ",  64, 24,  0, 1 },//  11
	{	"AMP: STACK      ",	"MID. FREQ.   ", "MID. GAIN    ", "REVERB       ",  64, 64, 24, 1 },//  12
	{	"AMP: COMBO      ",	"MID. FREQ.   ", "MID. GAIN    ", "REVERB       ",  64, 64, 24, 1 },//  13
	{	"EP: PHA+PAN+REV ",	"PANNER RATE  ", "REVERB       ", "PHASER       ",  64, 24,  0, 1 },//  14
	{	"EP: PHA+AMP+CHO ",	"BRILLIANCE   ", "REVERB       ", "PHASER       ",  64, 24,  0, 1 },//  15
	{	"FX: ANALOG ECHO ",	"FEEDBACK     ", "MIX DRY/WET  ", "ECHO TIME    ",  40, 64, 80, 1 },//  16
	{	"FX: PING DELAY  ",	"DELAY TIME   ", "FEEDBACK     ", "MIX DRY/WET  ",  80, 40, 64, 1 },//  17
	{	"FX: STEREOPHASER",	"FEEDBACK     ", "REVERB       ", "LFO SPEED    ",  34, 24, 26, 1 },//  18
	{	"FX: STEREOCHORUS",	"LFO DEPTH    ", "REVERB       ", "LFO SPEED    ",  64, 24, 30, 1 },//  19
	{	"FX: WAH WAH     ",	"LFO SPEED    ", "REVERB       ", "MANUAL WAH   ",  50, 24, 64, 1 },//  20
	{	"FX: RING MOD.   ",	"SQR <-> SIN  ", "REVERB       ", "LFO SPEED    ", 127, 24, 58, 1 },//  21
	{	"FX: STEP-FILTER ",	"STEP SPEED   ", "REVERB       ", "FILTER SPEED ",  60, 40, 34, 1 },//  22
	{	"REVERB: MONO MIX",	"DRY LEVEL    ", "REVERB       ", "             ", 127, 24,  0, 0 },//  23
	{	"REVERB: STEREO  ",	"DRY LEVEL    ", "REVERB       ", "             ", 127, 24,  0, 0 },//  24
	{	"UTIL: PASS THRU ",	"NONE         ", "NONE         ", "             ",   0,  0,  0, 0 },//  25
	{	"UTIL: MUTE      ",	"NONE         ", "NONE         ", "             ",   0,  0,  0, 0 },//  26
};


/////////////////////////////////////////////////////////////////////////////
// NON REAL-TIME PARAMETERS
/////////////////////////////////////////////////////////////////////////////
#define NUM_NR_PARAMETERS	15
typedef struct
{
	unsigned char name[17];			// Parameter name
	char max_idx;					// Max value index
	unsigned char value_str[4][7];	// Value strings
} nr_param;

const nr_param nr_parameters[NUM_NR_PARAMETERS] =
{
	// Location 512 means "don't write bytes"
	{ "HORN SLOW MOTOR ", 3, {"UNPLUG", "0,65Hz", "0,77Hz", "0,93Hz"}, 	},
	{ "HORN FAST MOTOR ", 3, {"UNPLUG", "6,3 Hz", "6,8 Hz", "7,6 Hz"}, 	},
	{ "BASS SLOW MOTOR ", 3, {"UNPLUG", "0,62Hz", "0,72Hz", "0,92Hz"}, 	},
	{ "BASS FAST MOTOR ", 3, {"UNPLUG", "5,8 Hz", "6,4 Hz", "7,2 Hz"}, 	},
	{ "HORN RAMP UP    ", 2, {"SLOW  ", "NORMAL", "FAST  ", "      "}, 	},
	{ "HORN RAMP DOWN  ", 2, {"SLOW  ", "NORMAL", "FAST  ", "      "}, 	},
	{ "BASS RAMP UP    ", 2, {"SLOW  ", "NORMAL", "FAST  ", "      "}, 	},
	{ "BASS RAMP DOWN  ", 2, {"SLOW  ", "NORMAL", "FAST  ", "      "}, 	},
	{ "FREQ.MOD. AMOUNT", 2, {"LESS  ", "NORMAL", "MORE  ", "      "}, 	},
	{ "CROSSOVER CUTOFF", 2, {"LOWER ", "NORMAL", "HIGHER", "      "}, 	},
	{ "HORN RESONANCE  ", 3, {"OFF   ", "TYPE 1", "TYPE 2", "TYPE 3"}, 	},
	{ "HORN TIMBRE     ", 3, {"DARK  ", "NORMAL", "OPEN  ", "BRIGHT"}, 	},
	{ "REVERB LENGTH   ", 3, {"ROOM  ", "STUDIO", "HALL  ", "CHURCH"}, 	},
	{ "REVERB TONE     ", 2, {"DARK  ", "NORMAL", "BRIGHT", "      "}, 	},
	{ "REVERB GAIN     ", 3, {"OFF   ", "-3 dB ", "0 dB  ", "+3 dB "}, 	},
};

const unsigned char InitPresetData[ACTUALPRESETSIZE] =
{
	  0x49, 0x6e, 0x69, 0x74, 0x20, 0x50, 0x72, 0x65, 0x73, 0x65, 0x74, 0x20, 0x00, 0x00, 0x00, 0x64, 0x64, 0x18, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02
};


#endif //__BURNED_MAIN_H__
