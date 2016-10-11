﻿////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2016 Herbert Aitenbichler

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

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using CNCLib.GCode.Commands;
using CNCLib.GCode.Load;
using CNCLib.GUI;
using CNCLib.Logic.Contracts.DTO;
using Framework.Arduino;
using Framework.Tools.Drawing;

namespace CNCLib.Wpf.Controls
{
	/// <summary>
	/// Interaction logic for GCodeUserControl.xaml
	/// </summary>
	public partial class GCodeUserControl : System.Windows.Controls.UserControl
	{
		private GCodeBitmapDraw _bitmapDraw = new GCodeBitmapDraw();

		public GCodeUserControl()
		{
			InitializeComponent();

			LoadOptions loadinfo = new LoadOptions();
			loadinfo.FileName = @"c:\tmp\test.nc";
			loadinfo.LoadType = LoadOptions.ELoadType.GCode;
			LoadBase load = LoadBase.Create(loadinfo);

			load.LoadOptions = loadinfo;
			load.Load();

			_bitmapDraw.RenderSize = new System.Drawing.Size((int)ActualWidth, (int)ActualHeight);

			_bitmapDraw.Commands.Clear();
			_bitmapDraw.Commands.AddRange(load.Commands);
			Zoom = 1;

			MouseWheel += GCodeUserControl_MouseWheel;

			MouseDown += GCodeUserControl_MouseDown;
			MouseUp   += GCodeUserControl_MouseUp;
			MouseMove += GCodeUserControl_MouseMove;
		}

		#region Properties

		private static void OnZoomChanged(DependencyObject dependencyObject,  DependencyPropertyChangedEventArgs e)
		{
			var godeCtrl = (GCodeUserControl)dependencyObject;
			godeCtrl._bitmapDraw.Zoom = (double) e.NewValue;
			godeCtrl.InvalidateVisual();
		}

		public static DependencyProperty ZoomProperty = DependencyProperty.Register("Zoom", typeof(double), typeof(GCodeUserControl), new PropertyMetadata(OnZoomChanged));
		public double Zoom
		{
			get { return (double)GetValue(ZoomProperty); }
			set { SetValue(ZoomProperty, value); }
		}

		private static void OnOffsetXChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs e)
		{
			var godeCtrl = (GCodeUserControl)dependencyObject;
			godeCtrl._bitmapDraw.OffsetX = (decimal)e.NewValue;
			godeCtrl.InvalidateVisual();
		}
		private static void OnOffsetYChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs e)
		{
			var godeCtrl = (GCodeUserControl)dependencyObject;
			godeCtrl._bitmapDraw.OffsetY = (decimal)e.NewValue;
			godeCtrl.InvalidateVisual();
		}

		public static DependencyProperty OffsetXProperty = DependencyProperty.Register("OffsetX", typeof(decimal), typeof(GCodeUserControl), new PropertyMetadata(OnOffsetXChanged));
		public static DependencyProperty OffsetYProperty = DependencyProperty.Register("OffsetY", typeof(decimal), typeof(GCodeUserControl), new PropertyMetadata(OnOffsetYChanged));
		public decimal OffsetX
		{
			get { return (decimal)GetValue(OffsetXProperty); }
			set { SetValue(OffsetXProperty, value); }
		}
		public decimal OffsetY
		{
			get { return (decimal)GetValue(OffsetYProperty); }
			set { SetValue(OffsetYProperty, value); }
		}

		public static DependencyProperty MachineColorProperty = DependencyProperty.Register("MachineColor", typeof(Color), typeof(GCodeUserControl), new PropertyMetadata(OnMachineColorChanged));

		private static void OnMachineColorChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs e)
		{
			var godeCtrl = (GCodeUserControl)dependencyObject;
			godeCtrl._bitmapDraw.MachineColor = ColorToColor((Color)e.NewValue);
			godeCtrl.InvalidateVisual();
		}
		public Color MachineColor
		{
			get { return (Color)GetValue(MachineColorProperty); }
			set { SetValue(MachineColorProperty, value); }
		}


		#endregion

		#region Drag/Drop

		public delegate void GCodeEventHandler(object sender, GCoderUserControlEventArgs e);

		public event GCodeEventHandler GCodeMousePosition;
		public event GCodeEventHandler ZoomOffsetChanged;

		private bool _isdragging = false;
		private Point3D _mouseDown;
		private decimal _mouseDownOffsetX;
		private decimal _mouseDownOffsetY;
		private Stopwatch _sw = new Stopwatch();

		private void GCodeUserControl_MouseWheel(object sender, MouseWheelEventArgs e)
		{
			if (e.Delta > 0)
				Zoom *= 1.1;
			else
				Zoom /= 1.1;

			OnZoomOffsetChanged();
			InvalidateVisual();
		}

		private void GCodeUserControl_MouseDown(object sender, MouseEventArgs e)
		{
			{
				if (!_isdragging)
				{
					var pt = new System.Drawing.Point((int) e.GetPosition(null).X, (int) e.GetPosition(null).Y);
					_mouseDown = _bitmapDraw.FromClient(pt);
					_mouseDownOffsetX = OffsetX;
					_mouseDownOffsetY = OffsetY;
					_sw.Start();
				}
				_isdragging = true;
			}
		}

		private void GCodeUserControl_MouseMove(object sender, MouseEventArgs e)
		{
			var pt = new System.Drawing.Point((int)e.GetPosition(null).X, (int)e.GetPosition(null).Y);

			if (GCodeMousePosition != null)
			{
				GCodeMousePosition(this, new GCoderUserControlEventArgs() { GCodePosition = _bitmapDraw.FromClient(pt) });
			}
			if (_isdragging)
			{
				OffsetX = _mouseDownOffsetX;
				OffsetY = _mouseDownOffsetY;
				Point3D c = _bitmapDraw.FromClient(pt);
				decimal newX = _mouseDownOffsetX - (c.X.Value - _mouseDown.X.Value);
				decimal newY = _mouseDownOffsetY + (c.Y.Value - _mouseDown.Y.Value);
				OffsetX = newX;
				OffsetY = newY;
				OnZoomOffsetChanged();
				if (_sw.ElapsedMilliseconds > 300)
				{
					_sw.Start();
					InvalidateVisual();
				}
			}
		}

		private void GCodeUserControl_MouseUp(object sender, MouseEventArgs e)
		{
			if (_isdragging)
			{
				InvalidateVisual();
				OnZoomOffsetChanged();
			}
			_isdragging = false;
		}

		private void OnZoomOffsetChanged()
		{
			if (ZoomOffsetChanged != null)
			{
				ZoomOffsetChanged(this, new GCoderUserControlEventArgs());
			}
		}

		#endregion


		#region private Members

		private ArduinoSerialCommunication Com
		{
			get { return Framework.Tools.Pattern.Singleton<ArduinoSerialCommunication>.Instance; }
		}

		#endregion

		#region private

		static Color ColorToColor(System.Drawing.Color color)
		{
			return Color.FromArgb(color.A, color.R, color.G, color.B);
		}
		static System.Drawing.Color ColorToColor(Color color)
		{
			return System.Drawing.Color.FromArgb(color.A, color.R, color.G, color.B);
		}

		private void ReInitDraw()
		{
			//InitPen();
			//Invalidate();
		}

		protected override void OnRenderSizeChanged(System.Windows.SizeChangedInfo sizeInfo)
		{
			base.OnRenderSizeChanged(sizeInfo);
			_bitmapDraw.RenderSize = new System.Drawing.Size((int) sizeInfo.NewSize.Width, (int) sizeInfo.NewSize.Height);
			InvalidateVisual();
		}
		#endregion

		protected override void OnRender(System.Windows.Media.DrawingContext drawingContext)
		{
			base.OnRender(drawingContext);
			this.DrawCommands(drawingContext);
		}

		private void DrawCommands(System.Windows.Media.DrawingContext context)
		{
			if (_bitmapDraw.RenderSize.Height == 0 || _bitmapDraw.RenderSize.Width == 0)
				return;

			if (Global.Instance.Machine != null)
			{
				_bitmapDraw.SizeX = Global.Instance.Machine.SizeX;
				_bitmapDraw.SizeY = Global.Instance.Machine.SizeY;
			}

			var curBitmap = _bitmapDraw.DrawToBitmap();
			MemoryStream stream = new MemoryStream();
			curBitmap.Save(stream, System.Drawing.Imaging.ImageFormat.Png);
			var cc = new System.Windows.Media.ImageSourceConverter().ConvertFrom(stream);
			context.DrawImage((System.Windows.Media.ImageSource)cc, new System.Windows.Rect(0,0, this.ActualWidth, this.ActualHeight));
			curBitmap.Dispose();
		}
	}
}
