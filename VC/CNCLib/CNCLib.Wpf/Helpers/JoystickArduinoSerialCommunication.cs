﻿////////////////////////////////////////////////////////
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

using CNCLib.Wpf.ViewModels.ManualControl;
using Framework.Arduino;
using System;
using System.Threading.Tasks;

namespace CNCLib.Wpf.Helpers
{
    class JoystickArduinoSerialCommunication : ArduinoSerialCommunication
	{
		private Framework.Arduino.ArduinoSerialCommunication Com
		{
			get { return Framework.Tools.Pattern.Singleton<Framework.Arduino.ArduinoSerialCommunication>.Instance; }
		}

		public JoystickArduinoSerialCommunication()
		{
			OkTag = "";		// every new line is "end of command"
		}

		public void RunCommandInNewTask(Action todo)
		{
			Task.Run(() =>
			{
				try
				{
					todo();
					Com.WriteCommandHistory(CommandHistoryViewModel.CommandHistoryFile);
				}
				finally
				{
				}
			});
		}

		protected override void OnReplyReceived(ArduinoSerialCommunicationEventArgs info)
		{
			base.OnReplyReceived(info);

			if (info.Info.StartsWith(";CNCJoystick"))
			{
				if (Global.Instance.Joystick?.InitCommands != null)
				{
					RunCommandInNewTask(async () =>
					{
						await new JoystickHelper().SendInitCommands(Global.Instance.Joystick?.InitCommands);
					});
				}
			}
			else
			{
				RunCommandInNewTask(() =>
				{
					new JoystickHelper().JoystickReplyReceived(info.Info.Trim());
				});
			}
		}
	}
}