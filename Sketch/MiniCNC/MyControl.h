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

#pragma once

////////////////////////////////////////////////////////

#include <Control.h>
#include <OnOffIOControl.h>
#include <Analog8IOControl.h>
#include <ReadPinIOControl.h>
#include <PushButtonLow.h>
#include <DummyIOControl.h>
#include <ConfigEeprom.h>

#include "Configuration.h"

////////////////////////////////////////////////////////

class CMyControl : public CControl
{
private:

	typedef CControl super;

public:

	CMyControl()				 { }

	virtual void Kill() override;

	virtual void IOControl(uint8_t tool, unsigned short level) override;
	virtual unsigned short IOControl(uint8_t tool) override;

protected:

	virtual void Init() override;
	virtual void Initialized() override;

	virtual void TimerInterrupt() override;

	virtual bool IsKill() override;
	virtual void Poll() override;

	virtual bool OnEvent(EnumAsByte(EStepperControlEvent) eventtype, uintptr_t addinfo) override;

private:

  static const CConfigEeprom::SCNCEeprom CMyControl::_eepromFlash;

#ifdef SPINDLE_ENABLE_PIN
	#ifdef SPINDLE_ANALOGSPEED
		CAnalog8IOControl<SPINDLE_ENABLE_PIN> _spindle;
		#if SPINDLE_MAXSPEED == 255
			inline uint8_t ConvertSpindleSpeedToIO(unsigned short level) { return (uint8_t)level; }
		#else	
			inline uint8_t ConvertSpindleSpeedToIO(unsigned short level) { return ConvertSpindleSpeedToIO8(CConfigEeprom::GetConfigU16(offsetof(CConfigEeprom::SCNCEeprom, maxspindlespeed)),level); }
		#endif
	#else
		COnOffIOControl<SPINDLE_ENABLE_PIN, SPINDLE_DIGITAL_ON, SPINDLE_DIGITAL_OFF> _spindle;
		inline uint8_t ConvertSpindleSpeedToIO(unsigned short level) { return (uint8_t) level; }
	#endif
	#ifdef SPINDLE_DIR_PIN
		COnOffIOControl<SPINDLE_DIR_PIN, SPINDLE_DIR_CLW, SPINDLE_DIR_CCLW> _spindleDir;
	#else
		CDummyIOControl _spindleDir;
	#endif
#else
	CDummyIOControl _spindle;
	CDummyIOControl _spindleDir;
	inline uint8_t ConvertSpindleSpeedToIO(unsigned short level) { return (uint8_t) level; }
#endif  

#ifdef COOLANT_PIN
	COnOffIOControl<COOLANT_PIN, COOLANT_ON, COOLANT_OFF> _coolant;
#else
	CDummyIOControl _coolant;
#endif
#ifdef PROBE_PIN
	CReadPinIOControl<PROBE_PIN, PROBE_ON> _probe;
#else
	CDummyIOControl _probe;
#endif

#ifdef KILL_PIN
	CReadPinIOControl<KILL_PIN, KILL_PIN_ON> _kill;
#else
	CDummyIOControl _kill;
#endif

#if defined(HOLD_PIN) && defined(RESUME_PIN)
	CPushButtonLow<HOLD_PIN,LOW> _hold;
	CPushButtonLow<RESUME_PIN, LOW> _resume;
#else
	CDummyIOControl _hold;
	CDummyIOControl _resume;
#endif

#if defined(HOLDRESUME_PIN)
	CPushButtonLow<HOLDRESUME_PIN,LOW> _holdresume;
#else
	CDummyIOControl _holdresume;
#endif

#ifdef CONTROLLERFAN_FAN_PIN
	#ifdef CONTROLLERFAN_ANALOGSPEED
		#if defined(USE_RAMPSFD)
			CAnalog8InvertIOControl<CONTROLLERFAN_FAN_PIN> _controllerfan;
		#else
			CAnalog8IOControl<CONTROLLERFAN_FAN_PIN> _controllerfan;
		#endif
	#else
		COnOffIOControl<CONTROLLERFAN_FAN_PIN, CONTROLLERFAN_DIGITAL_ON, CONTROLLERFAN_DIGITAL_OFF> _controllerfan;
	#endif
	inline bool IsControllerFanTimeout() { return millis() - CStepper::GetInstance()->IdleTime() > CONTROLLERFAN_ONTIME;	}
#else
	CDummyIOControl _controllerfan;
	inline bool IsControllerFanTimeout() { return false; }
#endif

	// constexpr uint32_t CoolantInfo() const  { return _controllerfan.IsConnected() ? CConfigEeprom::EEpromInfo1::HAVE_COOLANT : CConfigEeprom::EEpromInfo1::NONE; }

};

////////////////////////////////////////////////////////

extern CMyControl Control;

