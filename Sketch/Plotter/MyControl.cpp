////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2017 Herbert Aitenbichler

  CNCLib is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  CNCLib is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  http://www.gnu.org/licenses/
*/
////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arduino.h>

#include <StepperLib.h>
#include <CNCLib.h>
#include "MyControl.h"
#include "HPGLParser.h"
#include "PlotterControl.h"

////////////////////////////////////////////////////////////

CMotionControl MotionControl;
CConfigEeprom Eprom;
HardwareSerial& StepperSerial = Serial;

////////////////////////////////////////////////////////////

#ifndef MYNUM_AXIS
#error Please define MYNUM_AXIS
#endif

////////////////////////////////////////////////////////////

const CConfigEeprom::SCNCEeprom CMyControl::_eepromFlash PROGMEM =
{
	EPROM_SIGNATURE,
	NUM_AXIS, MYNUM_AXIS, offsetof(CConfigEeprom::SCNCEeprom,axis), sizeof(CConfigEeprom::SCNCEeprom::SAxisDefinitions),
	GetInfo1a(),0,
	0,
	STEPPERDIRECTION,0,0,0,
	SPINDLE_MAXSPEED,0,
	CNC_MAXSPEED,
	CNC_ACC,
	CNC_DEC,
	STEPRATERATE_REFMOVE,
	MOVEAWAYFROMREF_MM1000,
	X_STEPSPERMM/1000.0,
	{
		{ X_MAXSIZE,     X_USEREFERENCE, REFMOVE_1_AXIS,  X_REFERENCEHITVALUE },
		{ Y_MAXSIZE,     Y_USEREFERENCE, REFMOVE_2_AXIS,  Y_REFERENCEHITVALUE },
		{ Z_MAXSIZE,     Z_USEREFERENCE, REFMOVE_3_AXIS,  Z_REFERENCEHITVALUE },
#if NUM_AXIS > 3
		{ A_MAXSIZE,     A_USEREFERENCE, REFMOVE_4_AXIS,  A_REFERENCEHITVALUE },
#endif
#if NUM_AXIS > 4
		{ B_MAXSIZE,     B_USEREFERENCE, REFMOVE_5_AXIS,  B_REFERENCEHITVALUE },
#endif
#if NUM_AXIS > 5
		{ C_MAXSIZE,     C_USEREFERENCE, REFMOVE_6_AXIS,  C_REFERENCEHITVALUE },
#endif
	}
};

////////////////////////////////////////////////////////////

void CMyControl::Init()
{
	CSingleton<CConfigEeprom>::GetInstance()->Init(sizeof(CConfigEeprom::SCNCEeprom), &_eepromFlash, EPROM_SIGNATURE);

#ifdef DISABLELEDBLINK
	DisableBlinkLed();
#endif

	StepperSerial.println(MESSAGE_MYCONTROL_Starting);

	super::Init();

	InitFromEeprom();

	_data.Init();

	CHPGLParser::Init();

#ifdef MYUSE_LCD
	Lcd.Init();
#endif

}

////////////////////////////////////////////////////////////

void CMyControl::IOControl(uint8_t tool, unsigned short level)
{
	if (!_data.IOControl(tool, level))
	{
		super::IOControl(tool, level);
	}
}

////////////////////////////////////////////////////////////

#ifndef REDUCED_SIZE

unsigned short CMyControl::IOControl(uint8_t tool)
{
	switch (tool)
	{
		case SpindleCW:
		case SpindleCCW:	{ return _data._spindle.IsOn(); }
		case Probe:			{ return _data._probe.IsOn(); }
		case Coolant:		{ return _data._coolant.IsOn(); }
		case ControllerFan: { return _data._controllerfan.GetLevel(); }
	}

	return super::IOControl(tool);
}
#endif
////////////////////////////////////////////////////////////

void CMyControl::Kill()
{
	super::Kill();
	_data.Kill();
}

////////////////////////////////////////////////////////////

bool CMyControl::IsKill()
{
	if (_data.IsKill())
	{
#ifdef MYUSE_LCD
		Lcd.Diagnostic(F("E-Stop"));
#endif
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////

void CMyControl::TimerInterrupt()
{
	super::TimerInterrupt();
	_data.TimerInterrupt();
}

////////////////////////////////////////////////////////////

void CMyControl::Initialized()
{
	super::Initialized();
	_data.Initialized();
}

////////////////////////////////////////////////////////////

void CMyControl::Poll()
{
	super::Poll();

	if (IsHold())
	{
		if (_data._resume.IsOn() || _data._holdresume.IsOn())
		{
			Resume();
#ifdef MYUSE_LCD
			Lcd.ClearDiagnostic();
#endif
		}
	}
	else if (_data._hold.IsOn() || _data._holdresume.IsOn())
	{
		Hold();
#ifdef MYUSE_LCD
		Lcd.Diagnostic(F("LCD Hold"));
#endif
	}
}

////////////////////////////////////////////////////////////

bool CMyControl::OnEvent(EnumAsByte(EStepperControlEvent) eventtype, uintptr_t addinfo)
{
	_data.OnEvent(eventtype, addinfo);
	return super::OnEvent(eventtype, addinfo);
}

////////////////////////////////////////////////////////////

void CMyControl::Idle(unsigned int idletime)
{
	Plotter.Idle(idletime);
	super::Idle(idletime);
}

////////////////////////////////////////////////////////////

bool CMyControl::Parse(CStreamReader* reader, Stream* output)
{
	CHPGLParser hpgl(reader, output);
	return ParseAndPrintResult(&hpgl, output);
}

////////////////////////////////////////////////////////////
