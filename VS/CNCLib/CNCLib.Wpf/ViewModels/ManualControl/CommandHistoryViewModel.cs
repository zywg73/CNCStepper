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

using System.Windows.Input;
using System.Collections.ObjectModel;
using Framework.Wpf.Helpers;
using Framework.Arduino;
using CNCLib.Wpf.Models;
using System;

namespace CNCLib.Wpf.ViewModels.ManualControl
{
	public class CommandHistoryViewModel : DetailViewModel
	{
		public CommandHistoryViewModel(IManualControlViewModel vm)
			: base(vm)
		{
		}

		public const string CommandHistoryFile = @"%USERPROFILE%\Documents\Command.txt";

		#region Properties

		private ObservableCollection<SentCNCCommand> _commandHistoryCollection;
		public ObservableCollection<SentCNCCommand> CommandHistoryCollection
		{
			get { return _commandHistoryCollection; }
			set { SetProperty(ref _commandHistoryCollection, value); }
		}

		#endregion

		#region Commands / CanCommands

		public void RefreshAfterCommand()
		{
			RefreshCommandHistory();
		}

		public void RefreshCommandHistory()
		{
			var results = new ObservableCollection<SentCNCCommand>();

			foreach (ArduinoSerialCommunication.Command rc in Com.CommandHistoryCopy)
			{
                DateTime senttime = (rc.SentTime.HasValue) ? rc.SentTime.Value : DateTime.Today;

                results.Add(new SentCNCCommand() { CommandDate = senttime, CommandText = rc.CommandText, Result = rc.ResultText });

			}
			CommandHistoryCollection = results;
		}
		public void ClearCommandHistory()
		{
			Com.ClearCommandHistory();
			RefreshCommandHistory();
		}

		#endregion

		#region ICommand

		public ICommand RefreshHistoryCommand => new DelegateCommand(RefreshCommandHistory, CanSend);
		public ICommand ClearHistoryCommand => new DelegateCommand(ClearCommandHistory, CanSend);

		#endregion
	}
}