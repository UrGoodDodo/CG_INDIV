using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;

namespace CG_First_Indiv
{

    public class Triangle 
    {
        public Point fpoint;
        public Point spoint;
        public Point tpoint;

        public Triangle(Point fpoint, Point spoint, Point tpoint)
        {
            this.fpoint = fpoint;
            this.spoint = spoint;
            this.tpoint = tpoint;
        }

        public Triangle() { }
    }

    public partial class Form1 : Form
    {

        private Point point = new Point(0, 0);

        private Point CurPoint = Point.Empty;

        private int CurPointInd = 0;

        private int CountPoints = 0;

        private List<Point> points = new List<Point>();

        private SolidBrush brush = new SolidBrush(Color.Red);

        private Graphics g;

        private Triangle mTriangle = new Triangle();

        public Form1()
        {
            InitializeComponent();
            g = pictureBox1.CreateGraphics();

            //X
            trackBar1.Minimum = 0;
            trackBar1.Maximum = pictureBox1.Height;
            trackBar1.TickFrequency = 1;
            label7.Text = pictureBox1.Height.ToString();

            //Y
            trackBar2.Minimum = 0;
            trackBar2.Maximum = pictureBox1.Width;
            trackBar2.TickFrequency = 1;
            label3.Text = pictureBox1.Width.ToString();
        }

        private void DrawPoint(object sender, MouseEventArgs e)
        {
            point = e.Location;
            if (!points.Contains(point))
                points.Add(point);
            if (points.Count >= 4) 
            {
                mTriangle = Calculate_Main_Triangle();
            }
            comboBox1.Items.Add($"Point {CountPoints}");
            CountPoints++;
            pictureBox1.Invalidate();
        }

        private void pictureBox1_Paint(object sender, PaintEventArgs e)
        {
            foreach (Point p in points) 
            {
                e.Graphics.FillEllipse(brush, p.X - 3, p.Y - 3, 3, 3);
                Console.WriteLine(p.X);
            }

            if (points.Count() >= 4) 
            {
                Point[] tp = { mTriangle.fpoint, mTriangle.tpoint, mTriangle.spoint };
                e.Graphics.DrawPolygon(new Pen(Color.Black), tp);
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            point = Point.Empty;
            mTriangle = new Triangle();
            label1.Text = "Point";
            CurPoint = Point.Empty;
            points.Clear();
            comboBox1.Items.Clear();
            comboBox1.Text = "";
            trackBar1.Value = 0;
            trackBar2.Value = 0;
            CountPoints = 0;
            g.Clear(Color.White);
        }

        private void Scroll_X_Bar(object sender, EventArgs e)
        {
            if (CurPoint != Point.Empty) 
            {
                CurPoint.X = trackBar2.Value;
                points[comboBox1.SelectedIndex] = CurPoint;
                pictureBox1.Invalidate();
            }

            if (points.Count >= 4)
            {
                mTriangle = Calculate_Main_Triangle();
            }
        }

        private void Scroll_Y_Bar(object sender, EventArgs e)
        {
            if (CurPoint != Point.Empty)
            {
                CurPoint.Y = trackBar1.Value;
                points[comboBox1.SelectedIndex] = CurPoint;
                pictureBox1.Invalidate();
            }

            if (points.Count >= 4)
            {
                mTriangle = Calculate_Main_Triangle();
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            CurPoint = points[comboBox1.SelectedIndex];
            label1.Text = $"Point {comboBox1.SelectedIndex}";
            trackBar2.Value = points[comboBox1.SelectedIndex].X;
            trackBar1.Value = points[comboBox1.SelectedIndex].Y;
        }


        private Triangle Calculate_Main_Triangle() 
        {
            int minx = int.MaxValue; 
            int miny = int.MaxValue;
            int maxx = int.MinValue;
            int maxy = int.MinValue;
            foreach (Point p in points) 
            {
                if (p.X < minx)
                    minx = p.X;
                if (p.X > maxx)
                    maxx = p.X;
                if (p.Y < miny)
                    miny = p.Y;
                if (p.Y > maxy)
                    maxy = p.Y;
            }

            var x1 = minx + 10;
            var y1 = miny - 10;

            var x2 = (minx + maxx) / 2;
            var y2 = maxy + 10;

            var x3 = maxx + 10;
            var y3 = miny - 10;

            var k1 = (y2 - y1) / (x2 - x1);
            var l1 = -1 * ((x1 * y2 + x2 * y1) / (x2 - x1));

            var k2 = (y2 - y3) / (x2 - x3);
            var l2 = -1 * ((x3 * y2 + x2 * y3) / (x2 - x3));

            var k3 = (y3 - y1 / (x3 - x1));
            var l3 = -1 * ((x1 * y3 + x3 * y1) / (x3 - x1));

            bool flag = false;
            while (!flag) 
            {
                foreach (Point p in points) 
                {
                    if ((k1 * x2 + l1) > p.Y && (k2 * x2 + l2) > p.Y && (k3 * x1 + l3) < p.Y)
                    {
                        flag = true;
                    }
                    else
                        flag = false;
                }
                x1--;
                y2++;
                x3++;
                k1 = (y2 - y1) / (x2 - x1);
                l1 = -1 * ((x1 * y2 + x2 * y1) / (x2 - x1));

                k2 = (y2 - y3) / (x2 - x3);
                l2 = -1 * ((x3 * y2 + x2 * y3) / (x2 - x3));

                k3 = (y3 - y1 / (x3 - x1));
                l3 = -1 * ((x1 * y3 + x3 * y1) / (x3 - x1));
            }

            return new Triangle(new Point(x1, y1), new Point(x2, y2), new Point(x3, y3));
        }
    }
}
