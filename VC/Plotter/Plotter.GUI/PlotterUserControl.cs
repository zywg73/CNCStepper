﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Plotter.Logic;
using Plotter.GUI.Shapes;
using Framework.Tools;
using System.Threading;

namespace Plotter.GUI
{
	#region EventArg

	public class PlotterUserControlEventArgs : EventArgs
	{
		public PlotterUserControlEventArgs()
		{
		}
		public Shape Shape { get; set; }
		public PolyLine.LinePoint PolyLinePoint { get; set; }

		public Point HpglPosition { get; set; }
	}

	#endregion

	public partial class PlotterUserControl : UserControl
    {
        #region crt

        public PlotterUserControl()
        {
            SizeXHPGL = Tools.MulDivRound32(55600,29,77);
            SizeYHPGL = Tools.MulDivRound32(32000,29,77);

            _shapefactory.RegisterShape("Rectangle", new Plotter.GUI.Shapes.Rectangle());
            _shapefactory.RegisterShape("Line", new Line());
            _shapefactory.RegisterShape("Ellipse", new Ellipse());
            _shapefactory.RegisterShape("Triangle", new Triangle());
            _shapefactory.RegisterShape("PolyLine", new PolyLine());
            _shapefactory.RegisterShape("Dot", new Dot());

            CreateNewShape = "PolyLine";
            SelectedLinesize = 1;
            SelectedColor = Color.Black;

            SelectedShape = -1;

            InitializeComponent();
        }

        #endregion

        #region Properties
		public bool ReadOnly { get; set; }

        public int SizeXHPGL { get; set; }
        public int SizeYHPGL { get; set; }

        public ShapeList Shapes { get { return _shapelist;  } }

        public string CreateNewShape { get; set; }

        public int SelectedLinesize { get; set; }
        
        public Color SelectedColor { get; set; }

        public int SelectedShape
        {
            get { return _selectedShape >= Shapes.Count ? -1 : _selectedShape; }
            set
            {
                _selectedShape = value;
                if (_selectedShape >= Shapes.Count)
                    _selectedShape = Shapes.Count - 1;
                if (_selectedShape < -1)
                    _selectedShape = - 1;
                Invalidate();
            }
        }

//        public Control OutCoordinate { get; set; }

        #endregion

		#region Events

		public delegate void ShapeEventHandler(object o, PlotterUserControlEventArgs info);

		public event ShapeEventHandler ShapeCreating;
		public event ShapeEventHandler ShapeCreated;
		public event ShapeEventHandler PolylineAddPoint;
		public event ShapeEventHandler HpglMousePosition;

		#endregion

		#region private Members

		int _selectedShape = -1;

        Shapes.ShapeFactory _shapefactory = new Shapes.ShapeFactory();
        ShapeList _shapelist = new ShapeList();

        private Communication Com
        {
            get { return Framework.Tools.Singleton<Communication>.Instance; }
        }

        #endregion

        #region Convert Coordinats

        int AdjustHPGLCordX(int xx)
        {
            return  xx;
        }

        int AdjustHPGLCordY(int yy)
        {
            return SizeYHPGL - (yy);
        }

        Point ClientToHPGL(Point pt)
        {
            // with e.g.  867
            // max pt.X = 686 , pt.x can be 0
            return new Point(AdjustHPGLCordX(Tools.MulDivRound32(SizeXHPGL,pt.X,ClientSize.Width-1)), AdjustHPGLCordY(Tools.MulDivRound32(SizeYHPGL,pt.Y,ClientSize.Height-1)));
        }

        Point HPGLToClient(Point pt)
        {
            int x = pt.X;
            int y = SizeYHPGL - (pt.Y);
            return new Point(Tools.MulDivRound32(x, ClientSize.Width, SizeXHPGL), Tools.MulDivRound32(y, ClientSize.Height, SizeYHPGL));
        }

        #endregion

        #region Drag/Drop

        private bool _isdragging = false;

        private void PlotterUserControl_MouseDown(object sender, MouseEventArgs e)
        {
            if (!ReadOnly)
			{
                if (!_isdragging)
                {
                    Shape r = _shapefactory.Create(CreateNewShape);

                    r.ForgroundColor = SelectedColor;
                    r.LineSize = SelectedLinesize;
                    r.DrawStart = e.Location;
                    r.DrawEnd = e.Location;
                    r.HPGLStart = 
                    r.HPGLEnd = ClientToHPGL(e.Location);

                    _shapelist.Add(r);

					if (ShapeCreating != null)
					{
						ShapeCreating(this, new PlotterUserControlEventArgs() { Shape = r } );
					}

					Invalidate(new System.Drawing.Rectangle(r.DrawStart,new Size(1,1) ));
                }
                _isdragging = true;
            }
        }
        private void PlotterUserControl_MouseMove(object sender, MouseEventArgs e)
        {
			if (HpglMousePosition!=null)
            {
				HpglMousePosition(this, new PlotterUserControlEventArgs() { HpglPosition = ClientToHPGL(e.Location) });
                //Point hpgl = ClientToHPGL(e.Location);
                //OutCoordinate.Text = // e.Location.X.ToString() + ":" + e.Location.Y.ToString() + "(" + 
                //                     hpgl.X.ToString() + " : " + hpgl.Y.ToString() + "  =>  " + 
                //                     Math.Round(hpgl.X/40.0,1) + " : " + Math.Round(hpgl.Y/40.0,1) +  "mm";
            }
            if (_isdragging && _shapelist.Count > 0)
            {
                Shape r = (Shape)_shapelist[_shapelist.Count - 1];

                System.Drawing.Rectangle oldrect = r.NormalizedRect;

                r.DrawEnd = e.Location;
                r.HPGLEnd = ClientToHPGL(e.Location);


                if (r is PolyLine)
                {
                    if (((PolyLine)r).LastDrawPos() != e.Location)
                    {
                        PolyLine.LinePoint lp = new PolyLine.LinePoint() { DrawPos = e.Location, HPGLPos = ClientToHPGL(e.Location) };
                        ((PolyLine)r).Points.Add(lp);

						if (PolylineAddPoint != null)
						{
							PolylineAddPoint(this, new PlotterUserControlEventArgs() {  Shape = r,  PolyLinePoint = lp });
						}
                    }
                }

                Invalidate(oldrect);
            }
        }

        private void PlotterUserControl_MouseUp(object sender, MouseEventArgs e)
        {
            if (_isdragging && _shapelist.Count > 0)
            {
                Shape r = (Shape)_shapelist[_shapelist.Count - 1];
                r.DrawEnd = e.Location;
                r.HPGLEnd = ClientToHPGL(e.Location);

                if (r is PolyLine)
                {
                    if (((PolyLine)r).LastDrawPos() != e.Location)
                    {
                        PolyLine.LinePoint lp = new PolyLine.LinePoint() { DrawPos = e.Location, HPGLPos = ClientToHPGL(e.Location) };
                        ((PolyLine)r).Points.Add(lp);

						if (PolylineAddPoint != null)
						{
							PolylineAddPoint(this, new PlotterUserControlEventArgs() { Shape = r });
						}
                    }
                }

				if (ShapeCreated != null)
				{
					ShapeCreated(this, new PlotterUserControlEventArgs() { Shape = r });
				}

				Invalidate();
            }
            _isdragging = false;
        }

        #endregion

        #region private

        private void PlotterUserControl_Paint(object sender, PaintEventArgs e)
        {
            Shape.PaintState paintstate = new Shape.PaintState();
            paintstate.ShapeIdx = 0;
            paintstate.SelectedIdx = SelectedShape;
            foreach (Shape r in _shapelist)
            {
                r.Draw(e, paintstate);
                paintstate.ShapeIdx++;
            }
        }

        private Size _lastsize;
        private void PlotterUserControl_Resize(object sender, EventArgs e)
        {
            if (_lastsize.Height != 0 && Size.Width > 0 && Size.Height > 0)
            {
                RecalcClientCoord();
            }
            _lastsize = Size;
            Invalidate();
        }

        #endregion

        #region Operations

        public void RecalcClientCoord()
        {
            foreach (Shape r in _shapelist)
            {
                r.AdjustDrawPos(HPGLToClient);
            }
        }

        #endregion
    }
}