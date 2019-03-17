///////////////////////////////////////////////////////////////////////////
// GSi BURN Editor - Version 1.0 - October 2013
// Uses wxWidgets and RtMidi for cross platform (Win and Mac) compatibility
//
// www.GenuineSoundware.com
// 
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdio.h>
#if WIN32
#include "windows.h"
#include <tchar.h>
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

/*********** for fstream (ifstream, ofstream) ********/
#include <iostream>
#include <sstream>  // Required for stringstreams
#include <fstream>
using namespace std;


//#define MAXCHARACTERS 84
//const char BURN_LCD_Characters[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789,.-;:_@#+*!$%&/()?'<>";
//const char BURN_LCD_Characters[MAXCHARACTERS] = { ' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ',', '.', '-', ';', ':', '_', '@', '#', '+', '*', '!', '$', '%', '&', '/', '(', ')', '?', '\'', '<', '>', };
