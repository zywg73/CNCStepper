////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2018 Herbert Aitenbichler

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

#include "Stepper.h"

////////////////////////////////////////////////////////

#if defined(__AVR_ATmega2560__) || defined(_MSC_VER) || defined(__SAM3X8E__)

// only available on Arduino Mega or due

////////////////////////////////////////////////////////

#define E0_AXIS A_AXIS
#define E1_AXIS B_AXIS
#define E2_AXIS C_AXIS

#define RAMPSFD_ENDSTOPCOUNT 6

class CStepperRampsFD : public CStepper
{
private:
	typedef CStepper super;
public:

	////////////////////////////////////////////////////////

	CStepperRampsFD()
	{
		_num_axis = 6;
	}

	////////////////////////////////////////////////////////

	virtual void Init() override
	{
		super::Init();

#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif

		CHAL::pinModeOutput(RAMPSFD_X_STEP_PIN);
		CHAL::pinModeOutput(RAMPSFD_X_DIR_PIN);
		CHAL::pinModeOutput(RAMPSFD_X_ENABLE_PIN);
		CHAL::pinModeInputPullUp(RAMPSFD_X_MIN_PIN);
		CHAL::pinModeInputPullUp(RAMPSFD_X_MAX_PIN);
		HALFastdigitalWrite(RAMPSFD_X_STEP_PIN, RAMPSFD_PIN_STEP_ON);

		CHAL::pinModeOutput(RAMPSFD_Y_STEP_PIN);
		CHAL::pinModeOutput(RAMPSFD_Y_DIR_PIN);
		CHAL::pinModeOutput(RAMPSFD_Y_ENABLE_PIN);
		CHAL::pinModeInputPullUp(RAMPSFD_Y_MIN_PIN);
		CHAL::pinModeInputPullUp(RAMPSFD_Y_MAX_PIN);
		HALFastdigitalWrite(RAMPSFD_Y_STEP_PIN, RAMPSFD_PIN_STEP_ON);

		CHAL::pinModeOutput(RAMPSFD_Z_STEP_PIN);
		CHAL::pinModeOutput(RAMPSFD_Z_DIR_PIN);
		CHAL::pinModeOutput(RAMPSFD_Z_ENABLE_PIN);
		CHAL::pinModeInputPullUp(RAMPSFD_Z_MIN_PIN);
		CHAL::pinModeInputPullUp(RAMPSFD_Z_MAX_PIN);
		HALFastdigitalWrite(RAMPSFD_Z_STEP_PIN, RAMPSFD_PIN_STEP_ON);

		CHAL::pinModeOutput(RAMPSFD_E0_STEP_PIN);
		CHAL::pinModeOutput(RAMPSFD_E0_DIR_PIN);
		CHAL::pinModeOutput(RAMPSFD_E0_ENABLE_PIN);
		//  CHAL::pinModeInputPullUp(RAMPSFD_E0_MIN_PIN);         
		//  CHAL::pinModeInputPullUp(RAMPSFD_E0_MAX_PIN);         
		HALFastdigitalWrite(RAMPSFD_E0_STEP_PIN, RAMPSFD_PIN_STEP_ON);

#ifndef RAMPSFD_DISABLE_E1
		CHAL::pinModeOutput(RAMPSFD_E1_STEP_PIN);
		CHAL::pinModeOutput(RAMPSFD_E1_DIR_PIN);
		CHAL::pinModeOutput(RAMPSFD_E1_ENABLE_PIN);
		//  CHAL::pinModeInputPullUp(RAMPSFD_E1_MIN_PIN);         
		//  CHAL::pinModeInputPullUp(RAMPSFD_E1_MAX_PIN);         
		HALFastdigitalWrite(RAMPSFD_E1_STEP_PIN, RAMPSFD_PIN_STEP_ON);
#endif

		CHAL::pinModeOutput(RAMPSFD_E2_STEP_PIN);
		CHAL::pinModeOutput(RAMPSFD_E2_DIR_PIN);
		CHAL::pinModeOutput(RAMPSFD_E2_ENABLE_PIN);
		//  CHAL::pinModeInputPullUp(E2_MIN_PIN);         
		//  CHAL::pinModeInputPullUp(E2_MAX_PIN);         
		HALFastdigitalWrite(RAMPSFD_E2_STEP_PIN, RAMPSFD_PIN_STEP_ON);

#ifdef _MSC_VER
#pragma warning( default : 4127 )
#endif

		// init some outputs!

		CHAL::pinModeInputPullUp(RAMPSFD_ESTOP_PIN);

		CHAL::pinModeOutput(RAMPSFD_FET5D12_PIN); HALFastdigitalWrite(RAMPSFD_FET5D12_PIN, 0);
		CHAL::pinModeOutput(RAMPSFD_FET6D2_PIN);  HALFastdigitalWrite(RAMPSFD_FET6D2_PIN, 0);
	}

protected:

	////////////////////////////////////////////////////////

	virtual void  SetEnable(axis_t axis, uint8_t level, bool /* force */) override
	{
		switch (axis)
		{
#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif
			case X_AXIS:  if (level != LevelOff)	HALFastdigitalWrite(RAMPSFD_X_ENABLE_PIN,  RAMPSFD_PIN_ENABLE_ON);	else	HALFastdigitalWrite(RAMPSFD_X_ENABLE_PIN,  RAMPSFD_PIN_ENABLE_OFF); break;
			case Y_AXIS:  if (level != LevelOff)	HALFastdigitalWrite(RAMPSFD_Y_ENABLE_PIN,  RAMPSFD_PIN_ENABLE_ON);	else	HALFastdigitalWrite(RAMPSFD_Y_ENABLE_PIN,  RAMPSFD_PIN_ENABLE_OFF); break;
			case Z_AXIS:  if (level != LevelOff)	HALFastdigitalWrite(RAMPSFD_Z_ENABLE_PIN,  RAMPSFD_PIN_ENABLE_ON);	else	HALFastdigitalWrite(RAMPSFD_Z_ENABLE_PIN,  RAMPSFD_PIN_ENABLE_OFF); break;
			case E0_AXIS: if (level != LevelOff)	HALFastdigitalWrite(RAMPSFD_E0_ENABLE_PIN, RAMPSFD_PIN_ENABLE_ON);	else	HALFastdigitalWrite(RAMPSFD_E0_ENABLE_PIN, RAMPSFD_PIN_ENABLE_OFF); break;
#ifndef RAMPSFD_DISABLE_E1
			case E1_AXIS: if (level != LevelOff)	HALFastdigitalWrite(RAMPSFD_E1_ENABLE_PIN, RAMPSFD_PIN_ENABLE_ON);	else	HALFastdigitalWrite(RAMPSFD_E1_ENABLE_PIN, RAMPSFD_PIN_ENABLE_OFF); break;
#endif
			case E2_AXIS: if (level != LevelOff)	HALFastdigitalWrite(RAMPSFD_E2_ENABLE_PIN, RAMPSFD_PIN_ENABLE_ON);	else	HALFastdigitalWrite(RAMPSFD_E2_ENABLE_PIN, RAMPSFD_PIN_ENABLE_OFF); break;

#ifdef _MSC_VER
#pragma warning( default : 4127 )
#endif
		}
	}

	////////////////////////////////////////////////////////

	virtual uint8_t GetEnable(axis_t axis) override
	{
		switch (axis)
		{
#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif
			case X_AXIS:  return ConvertLevel(HALFastdigitalRead(RAMPSFD_X_ENABLE_PIN) == RAMPSFD_PIN_ENABLE_ON);
			case Y_AXIS:  return ConvertLevel(HALFastdigitalRead(RAMPSFD_Y_ENABLE_PIN) == RAMPSFD_PIN_ENABLE_ON);
			case Z_AXIS:  return ConvertLevel(HALFastdigitalRead(RAMPSFD_Z_ENABLE_PIN) == RAMPSFD_PIN_ENABLE_ON);
			case E0_AXIS: return ConvertLevel(HALFastdigitalRead(RAMPSFD_E0_ENABLE_PIN) == RAMPSFD_PIN_ENABLE_ON);
#ifndef RAMPSFD_DISABLE_E1
			case E1_AXIS: return ConvertLevel(HALFastdigitalRead(RAMPSFD_E1_ENABLE_PIN) == RAMPSFD_PIN_ENABLE_ON);
#endif
			case E2_AXIS: return ConvertLevel(HALFastdigitalRead(RAMPSFD_E2_ENABLE_PIN) == RAMPSFD_PIN_ENABLE_ON);
#ifdef _MSC_VER
#pragma warning( default : 4127 )
#endif
		}
		return 0;
	}

	////////////////////////////////////////////////////////

#if defined(RAMPSFD_USE_A4998)
#define USE_A4998
#else
#undef USE_A4998
#endif
#include "StepperA4998_DRV8825.h"

	////////////////////////////////////////////////////////

	virtual void  Step(const uint8_t steps[NUM_AXIS], axisArray_t directionUp) override
	{
		// The timing requirements for minimum pulse durations on the STEP pin are different for the two drivers. 
		// With the DRV8825, the high and low STEP pulses must each be at least 1.9 us; 
		// they can be as short as 1 us when using the A4988.

		// Step:   LOW to HIGH

		if ((directionUp&(1 << X_AXIS)) != 0)  HALFastdigitalWriteNC(RAMPSFD_X_DIR_PIN, RAMPSFD_PIN_DIR_OFF); else HALFastdigitalWriteNC(RAMPSFD_X_DIR_PIN, RAMPSFD_PIN_DIR_ON);
		if ((directionUp&(1 << Y_AXIS)) != 0)  HALFastdigitalWriteNC(RAMPSFD_Y_DIR_PIN, RAMPSFD_PIN_DIR_OFF); else HALFastdigitalWriteNC(RAMPSFD_Y_DIR_PIN, RAMPSFD_PIN_DIR_ON);
		if ((directionUp&(1 << Z_AXIS)) != 0)  HALFastdigitalWriteNC(RAMPSFD_Z_DIR_PIN, RAMPSFD_PIN_DIR_OFF); else HALFastdigitalWriteNC(RAMPSFD_Z_DIR_PIN, RAMPSFD_PIN_DIR_ON);
		if ((directionUp&(1 << E0_AXIS)) != 0) HALFastdigitalWriteNC(RAMPSFD_E0_DIR_PIN, RAMPSFD_PIN_DIR_OFF); else HALFastdigitalWriteNC(RAMPSFD_E0_DIR_PIN, RAMPSFD_PIN_DIR_ON);
#ifndef RAMPSFD_DISABLE_E1
		if ((directionUp&(1 << E1_AXIS)) != 0) HALFastdigitalWriteNC(RAMPSFD_E1_DIR_PIN, RAMPSFD_PIN_DIR_OFF); else HALFastdigitalWriteNC(RAMPSFD_E1_DIR_PIN, RAMPSFD_PIN_DIR_ON);
#endif
		if ((directionUp&(1 << E2_AXIS)) != 0) HALFastdigitalWriteNC(RAMPSFD_E2_DIR_PIN, RAMPSFD_PIN_DIR_OFF); else HALFastdigitalWriteNC(RAMPSFD_E2_DIR_PIN, RAMPSFD_PIN_DIR_ON);

		for (uint8_t cnt = 0;; cnt++)
		{
			register bool have = false;
			if (steps[X_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPSFD_X_STEP_PIN, RAMPSFD_PIN_STEP_OFF); have = true; }
			if (steps[Y_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPSFD_Y_STEP_PIN, RAMPSFD_PIN_STEP_OFF); have = true; }
			if (steps[Z_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPSFD_Z_STEP_PIN, RAMPSFD_PIN_STEP_OFF); have = true; }
			if (steps[E0_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPSFD_E0_STEP_PIN, RAMPSFD_PIN_STEP_OFF); have = true; }
#ifndef RAMPSFD_DISABLE_E1
			if (steps[E1_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPSFD_E1_STEP_PIN, RAMPSFD_PIN_STEP_OFF); have = true; }
#endif
			if (steps[E2_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPSFD_E2_STEP_PIN, RAMPSFD_PIN_STEP_OFF); have = true; }

			Delay1(RAMPSFD_NUM_AXIS);

			if (steps[X_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPSFD_X_STEP_PIN, RAMPSFD_PIN_STEP_ON); }
			if (steps[Y_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPSFD_Y_STEP_PIN, RAMPSFD_PIN_STEP_ON); }
			if (steps[Z_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPSFD_Z_STEP_PIN, RAMPSFD_PIN_STEP_ON); }
			if (steps[E0_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPSFD_E0_STEP_PIN, RAMPSFD_PIN_STEP_ON); }
#ifndef RAMPSFD_DISABLE_E1
			if (steps[E1_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPSFD_E1_STEP_PIN, RAMPSFD_PIN_STEP_ON); }
#endif
			if (steps[E2_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPSFD_E2_STEP_PIN, RAMPSFD_PIN_STEP_ON); }

			if (!have) break;

			Delay2();
		}
	}

public:

	////////////////////////////////////////////////////////

	virtual uint8_t GetReferenceValue(uint8_t referenceid) override
	{
		switch (referenceid)
		{
			case 0: return HALFastdigitalRead(RAMPSFD_X_MIN_PIN);
			case 1: return HALFastdigitalRead(RAMPSFD_X_MAX_PIN);
			case 2: return HALFastdigitalRead(RAMPSFD_Y_MIN_PIN);
			case 3: return HALFastdigitalRead(RAMPSFD_Y_MAX_PIN);
			case 4: return HALFastdigitalRead(RAMPSFD_Z_MIN_PIN);
			case 5: return HALFastdigitalRead(RAMPSFD_Z_MAX_PIN);
				/* No reference for E0 & E1 & E2
				case 6: return HALFastdigitalRead(RAMPSFD_E0_MIN_PIN)==REF_ON;
				case 7: return HALFastdigitalRead(RAMPSFD_E0_MAX_PIN)==REF_ON;
				case 8: return HALFastdigitalRead(RAMPSFD_E1_MIN_PIN)==REF_ON;
				case 9: return HALFastdigitalRead(RAMPSFD_E1_MAX_PIN)==REF_ON;
				case 10:return HALFastdigitalRead(RAMPSFD_E2_MIN_PIN)==REF_ON;
				case 11:return HALFastdigitalRead(RAMPSFD_E2_MAX_PIN)==REF_ON;
				*/
		}
		return 255;
	}

	////////////////////////////////////////////////////////

	virtual bool IsAnyReference() override
	{
		return
			_pod._referenceHitValue[0] == HALFastdigitalRead(RAMPSFD_X_MIN_PIN) ||
			_pod._referenceHitValue[1] == HALFastdigitalRead(RAMPSFD_X_MAX_PIN) ||
			_pod._referenceHitValue[2] == HALFastdigitalRead(RAMPSFD_Y_MIN_PIN) ||
			_pod._referenceHitValue[3] == HALFastdigitalRead(RAMPSFD_Y_MAX_PIN) ||
			_pod._referenceHitValue[4] == HALFastdigitalRead(RAMPSFD_Z_MIN_PIN) ||
			_pod._referenceHitValue[5] == HALFastdigitalRead(RAMPSFD_Z_MAX_PIN);
	}
};

#endif