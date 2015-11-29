////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2015 Herbert Aitenbichler

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

#include "Configuration_CNCShield.h"

////////////////////////////////////////////////////////

class CMyControl : public CControl
{
private:

	typedef CControl super;

public:

	CMyControl()				 { }

	virtual void Kill() override;

	virtual void IOControl(unsigned char tool, unsigned short level) override;
	virtual unsigned short IOControl(unsigned char tool) override;

protected:

	virtual void Init() override;
	virtual void TimerInterrupt() override;

	bool IsKill() override;
	virtual void Poll() override;
	virtual bool Parse(CStreamReader* reader, Stream* output) override;
	virtual void GoToReference() override;

	virtual bool OnStepperEvent(CStepper*stepper, EnumAsByte(CStepper::EStepperEvent) eventtype, void* addinfo) override;

private:

#ifdef SPINDEL_ENABLE_PIN
	#ifdef SPINDEL_ANALOGSPEED
		CAnalog8IOControl<SPINDEL_ENABLE_PIN> _spindel;
	#else
		COnOffIOControl<SPINDEL_ENABLE_PIN, SPINDEL_DIGITAL_ON, SPINDEL_DIGITAL_OFF> _spindel;
	#endif
	COnOffIOControl<SPINDEL_DIR_PIN, SPINDEL_DIR_CLW, SPINDEL_DIR_CCLW> _spindelDir;
#endif  
#ifdef COOLANT_PIN
	COnOffIOControl<COOLANT_PIN, COOLANT_ON, COOLANT_OFF> _coolant;
#endif
#ifdef PROBE_PIN
	CReadPinIOControl<PROBE_PIN, PROBE_ON> _probe;
#endif
#ifdef KILL_PIN
	CReadPinIOControl<KILL_PIN, KILL_PIN_ON> _kill;
#endif
#if defined(HOLD_PIN) && defined(RESUME_PIN)
	CPushButtonLow _hold;
	CPushButtonLow _resume;
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
#endif

};

////////////////////////////////////////////////////////

extern CMyControl Control;

