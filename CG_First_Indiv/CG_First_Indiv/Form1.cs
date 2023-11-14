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

        private List<Triangle> triangles = new List<Triangle>();
 

        public Form1()
        {
            InitializeComponent();
            g = pictureBox1.CreateGraphics();
            mTriangle = Calculate_Main_Triangle();
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
            Console.WriteLine($"{e.Location.X}  {e.Location.Y}");
            point = e.Location;
            if (!points.Contains(point))
                points.Add(point);
            if (points.Count >= 3) 
            {
                triangles = Delaunay_Triagulation();
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
            }

            Point[] t = { mTriangle.fpoint, mTriangle.spoint, mTriangle.tpoint };
            e.Graphics.DrawPolygon(new Pen(Color.Black), t);
            
            if (points.Count() >= 3) 
            {
                foreach (Triangle tr in triangles) 
                {
                    Point[] tp = { tr.fpoint, tr.spoint, tr.tpoint };
                    e.Graphics.DrawPolygon(new Pen(Color.Black), tp);
                }
            }
            

        }

        private void button1_Click(object sender, EventArgs e)
        {
            point = Point.Empty;
            label1.Text = "Point";
            CurPoint = Point.Empty;
            points.Clear();
            comboBox1.Items.Clear();
            comboBox1.Text = "";
            trackBar1.Value = 0;
            trackBar2.Value = 0;
            CountPoints = 0;
            pictureBox1.Invalidate();
            //g.Clear(Color.White);
        }

        private void Scroll_X_Bar(object sender, EventArgs e)
        {
            if (CurPoint != Point.Empty) 
            {
                CurPoint.X = trackBar2.Value;
                points[comboBox1.SelectedIndex] = CurPoint;
                pictureBox1.Invalidate();
            }

            if (points.Count >= 3)
            {
                triangles = Delaunay_Triagulation();
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

            if (points.Count >= 3)
            {
                triangles = Delaunay_Triagulation();
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
            var x1 = 0;
            var y1 = 727;
            var x2 = 532;
            var y2 = 0;
            var x3 = 1063;
            var y3 = 727;
            var t = new Triangle(new Point(x1, y1), new Point(x2, y2), new Point(x3, y3));
            triangles.Add(t);
            pictureBox1.Invalidate();
            return t;
        }

        private List<Triangle> Delaunay_Triagulation() 
        {
            List<Triangle> ttriangles = new List<Triangle>();
            ttriangles.Add(mTriangle);

            for (int i = 0; i < points.Count; i++) //points.Count
            {
                Console.WriteLine();
                Console.WriteLine($"Количество треугольников до удаления {ttriangles.Count}");
                var tp = points[i];
                List<Triangle> bTriangles = new List<Triangle>();

                for (int j = ttriangles.Count - 1; j >= 0; j--) 
                {
                    var tt = ttriangles[j];
                    double dst = Math.Sqrt(Math.Pow(tp.X - tt.center.X, 2) + Math.Pow(tp.Y - tt.center.Y, 2));
                    //Console.WriteLine($"Точка {j} Радиус треугольника {tt.radius} Расстояние {dst}");
                    if (dst < tt.radius)
                        bTriangles.Add(tt);
                }
                Console.WriteLine($"Количетсво плохих треугольников {bTriangles.Count}");

                List<Edge> poly = new List<Edge>();
                for (int j = 0; j < bTriangles.Count; j++) 
                {
                    var t = bTriangles[j];
                    var lines = t.Get_Lines();

                    for (int k = 0; k < lines.Count; k++) 
                    {
                        bool flag = false;
                        for (int l = 0; l < bTriangles.Count; l++) 
                        {
                            if (l != j && bTriangles[l].Contains_Line(lines[k]))
                                flag = true;
                        }
                        if (!flag)
                            poly.Add(lines[k]);
                    }
                }
                Console.WriteLine($"Количество сторон полигона из уникальных граней: {poly.Count}");

                for (int j = bTriangles.Count - 1; j >= 0 ; j--)
                    ttriangles.Remove(bTriangles[j]);
                Console.WriteLine($"Количество треугольников после удаления {ttriangles.Count}");

                for (int j = 0; j < poly.Count; j++) 
                {
                    Edge tl = poly[j];
                    Point fp = new Point(tp.X,tp.Y);
                    Point sp = tl.fpoint;
                    Point thirdp = tl.spoint;
                    ttriangles.Add(new Triangle(fp, sp, thirdp));
                }
                Console.WriteLine($"Количество треугольников после добавления новых {ttriangles.Count}");
                /*
                */
            }
            
            for (int j = ttriangles.Count - 1; j >= 0; j--)
            {
                var t = ttriangles[j];
                for (int k = 0; k < t.points.Count; k++)
                {
                    bool flag = false;
                    Point pp = t.points[k];
                    for (int l = 0; l < mTriangle.points.Count; l++)
                    {
                        if (pp.Equals(mTriangle.points[l]))
                        {
                            flag = true;
                            break;
                        }
                    }

                    if (flag)
                    {
                        ttriangles.RemoveAt(j);
                        break;
                    }

                }
            }
            return ttriangles;
        }
    }

    public class Edge
    {
        public Point fpoint;
        public Point spoint;

        public Edge(Point fpoint, Point spoint)
        {
            this.fpoint = fpoint;
            this.spoint = spoint;
        }

        public bool Equal_Lines(Edge line)
        {
            return fpoint.X == line.fpoint.X && fpoint.Y == line.fpoint.Y
            && spoint.X == line.spoint.X && spoint.Y == line.spoint.Y;
        }
    }
    public class Triangle
    {
        public Point fpoint;
        public Point spoint;
        public Point tpoint;
        public Point center;
        public List<Point> points = new List<Point>();

        public double radius;

        public Triangle(Point fpoint, Point spoint, Point tpoint)
        {
            this.fpoint = fpoint;
            this.spoint = spoint;
            this.tpoint = tpoint;
            points.Add(fpoint);
            points.Add(spoint);
            points.Add(tpoint);
            Calculate_Centre();
        }

        public Triangle() { }

        public void Calculate_Centre()
        {
            float d = (fpoint.X * (spoint.Y - tpoint.Y) + spoint.X * (tpoint.Y - fpoint.Y) + tpoint.X * (fpoint.Y - spoint.Y)) * 2f;
            var tfpoint = new Point((int)(Math.Pow(fpoint.X, 2)), (int)(Math.Pow(fpoint.Y, 2)));
            var tspoint = new Point((int)(Math.Pow(spoint.X, 2)), (int)(Math.Pow(spoint.Y, 2)));
            var ttpoint = new Point((int)(Math.Pow(tpoint.X, 2)), (int)(Math.Pow(tpoint.Y, 2)));
            float x = ((tfpoint.X + tfpoint.Y) * (spoint.Y - tpoint.Y) + (tspoint.X + tspoint.Y) * (tpoint.Y - fpoint.Y) + (ttpoint.X + ttpoint.Y) * (fpoint.Y - spoint.Y)) / d;
            float y = ((tfpoint.X + tfpoint.Y) * (tpoint.X - spoint.X) + (tspoint.X + tspoint.Y) * (fpoint.X - tpoint.X) + (ttpoint.X + ttpoint.Y) * (spoint.X - fpoint.X)) / d;

            center = new Point((int)x, (int)y);
            radius = Math.Sqrt(Math.Pow(center.X - fpoint.X, 2) + Math.Pow(center.Y - fpoint.Y, 2));
        }

        public List<Edge> Get_Lines()
        {
            List<Edge> lines = new List<Edge>();
            lines.Add(new Edge(fpoint, spoint));
            lines.Add(new Edge(spoint, tpoint));
            lines.Add(new Edge(tpoint, fpoint));
            return lines;
        }

        public bool Contains_Line(Edge line)
        {
            int t = 0;
            for (int i = 0; i < points.Count; i++)
            {
                if (points[i] == line.fpoint || points[i] == line.spoint)
                    t++;
            }

            return t == 2;
        }
    }

}
