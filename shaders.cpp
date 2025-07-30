#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <cmath>
#include <fstream>
using namespace std;

const int RESOLUTION_X = 481;
const int RESOLUTION_Y = 280;
const int RESOLUTION = RESOLUTION_X * RESOLUTION_Y;
const int FRAMERATE = 30;

// Renders the contents from top left to bottom right
class Screen
{
public:
    int resolution_x;
    int resolution_y;
    Screen(int custom_resolution_x, int custom_resolution_y)
    {
        resolution_x = custom_resolution_x;
        resolution_y = custom_resolution_y;
    }
    void render(double *raw_data)
    {
        string *image_data = decodeData(raw_data);
        int image_length = resolution_x * resolution_y;
        int image_cursor = 0;
        for (int image_y = 0; image_y < resolution_y; image_y++)
        {
            for (int image_x = 0; image_x < resolution_x; image_x++, image_cursor++)
            {
                cout << image_data[image_cursor];
            }
            cout << endl;
        }
        cout << endl;
    }
    void renderToFile(double *raw_data)
    {
        ofstream render_file("rendered_image.txt");
        string *image_data = decodeData(raw_data);
        int image_length = resolution_x * resolution_y;
        int image_cursor = 0;
        for (int image_y = 0; image_y < resolution_y; image_y++)
        {
            for (int image_x = 0; image_x < resolution_x; image_x++, image_cursor++)
            {
                render_file << image_data[image_cursor];
            }
            render_file << endl;
        }
        render_file << endl;
    }
    string *decodeData(double *raw_data)
    {
        string *image_data = new string[resolution_x * resolution_y];
        for (int i = 0; i < resolution_x * resolution_y; i++)
        {
            if (raw_data[i] == 1)
            {
                image_data[i] = "██";
                continue;
            }
            if (raw_data[i] == 0.8)
            {
                image_data[i] = "▒▒";
                continue;
            }
            if (raw_data[i] == 0.2)
            {
                image_data[i] = "░░";
                continue;
            }
            image_data[i] = "  ";
        }
        return image_data;
    }
};

// An object that represents a cartesian system for a specified resolution
class CoordinatePlane
{
public:
    int resolution_x;
    int resolution_y;
    int resolution;
    CoordinatePlane(int input_resolution_x, int input_resolution_y)
    {
        resolution_x = input_resolution_x;
        resolution_y = input_resolution_y;
        resolution = resolution_x * resolution_y;
    }
    // Returns index of array that corresponds to specified coordinates
    int coords(int x, int y)
    {
        if (x > resolution_x || 0 > x)
            return 0;
        if (y > resolution_y || 0 > y)
            return 1;
        int coord_index = (resolution - (y * resolution_x)) - (resolution_x - x);
        return coord_index;
    }
};

// Geometric Objects
// Point
class Point
{
public:
    int x;
    int y;
    bool isDisplayed = false;
    Point(int x_value, int y_value)
    {
        x = x_value;
        y = y_value;
    }
    void Translate(int x_value, int y_value)
    {
        x += x_value;
        y += y_value;
    }
};

class geometricObject
{
public:
    vector<Point *> line_points;
    void Translate(int x_value, int y_value)
    {
        for (int i = 0; i < line_points.size(); i++)
        {
            line_points[i]->Translate(x_value, y_value);
        }
    }
    vector<Point *> getLinePoints()
    {
        return line_points;
    }
};

// Line object
class Line : public geometricObject
{
public:
    double m;
    int c;
    Line(Point *point1, Point *point2)
    {
        double x1 = point1->x, y1 = point1->y, x2 = point2->x, y2 = point2->y;
        generateLinePoints(x1, y1, x2, y2);
    }
    Line(double x1, double y1, double x2, double y2)
    {
        generateLinePoints(x1, y1, x2, y2);
    }
    void generateLinePoints(double x1, double y1, double x2, double y2)
    {
        m = (y2 - y1) / (x2 - x1);
        c = y1 - (m * x1);
        cout << "m: " << m << ", c: " << c << endl;
        if (m < 1 && m > -1)
        {
            if (x1 > x2)
            {
                double temp = x1;
                x1 = x2;
                x2 = temp;
                temp = y1;
                y1 = y2;
                y2 = temp;
            }
            for (int x = x1 + 1; x < x2; x++)
            {
                int y = (m * x) + c;
                Point *point = new Point(x, y);
                line_points.push_back(point);
            }
        }
        else
        {
            if (y1 > y2)
            {
                double temp = y1;
                y1 = y2;
                y2 = temp;
                temp = x1;
                x1 = x2;
                x2 = temp;
            }
            for (int y = y1 + 1; y < y2; y++)
            {
                int x = (y - c) / m;
                if (x1 == x2)
                    x = x1;
                Point *point = new Point(x, y);
                line_points.push_back(point);
            }
        }
    }
};

class Circle : public geometricObject
{
public:
    Circle(Point *centre, int radius)
    {
        generateLinePoints(centre->x, centre->y, radius);
    }
    Circle(int centre_x, int centre_y, int radius)
    {
        generateLinePoints(centre_x, centre_y, radius);
    }
    void generateLinePoints(int centre_x, int centre_y, int radius)
    {
        for (int x = centre_x - (radius / sqrt(2)); x < centre_x + ((radius / sqrt(2)) + 1); x++)
        {
            int y_upper = centre_y + sqrt((pow(radius, 2)) - pow((x - centre_x), 2));
            int y_lower = centre_y - sqrt((pow(radius, 2)) - pow((x - centre_x), 2));
            Point *point_upper = new Point(x, y_upper);
            Point *point_lower = new Point(x, y_lower);
            line_points.push_back(point_upper);
            line_points.push_back(point_lower);
        }
        for (int y = centre_y - (radius / sqrt(2)); y < centre_y + ((radius / sqrt(2)) + 1); y++)
        {
            int x_right = centre_x + sqrt((pow(radius, 2)) - pow((y - centre_y), 2));
            int x_left = centre_x - sqrt((pow(radius, 2)) - pow((y - centre_y), 2));
            Point *point_right = new Point(x_right, y);
            Point *point_left = new Point(x_left, y);
            line_points.push_back(point_right);
            line_points.push_back(point_left);
        }
        // for (double t = 0; t < (2 * 3.1415) + 1; t += 0.01)
        // {
        //     int x = centre_x + radius * cos(t);
        //     int y = centre_y + radius * sin(t);
        //     Point *point = new Point(x, y);
        //     line_points.push_back(point);
        // }
    }
};

class Bezier
{
};

// Handles and generates rendering data for graphics
class Viewport
{
public:
    int width;
    int height;
    int resolution;
    double *viewport_data;
    CoordinatePlane *viewPlane;
    vector<Point *> displayed_points;
    Viewport()
    {
        width = RESOLUTION_X;
        height = RESOLUTION_Y;
        resolution = RESOLUTION;
        viewport_data = new double[RESOLUTION];
        viewPlane = new CoordinatePlane(width, height);
        generateBG();
    }
    Viewport(int resolution_x, int resolution_y)
    {
        width = resolution_x;
        height = resolution_y;
        resolution = resolution_x * resolution_y;
        viewport_data = new double[resolution];
        viewPlane = new CoordinatePlane(width, height);
        generateBG();
    }
    ~Viewport()
    {
        delete[] viewport_data;
        delete viewPlane;
    }

    void generateBG()
    {
        for (int i = 0; i < (width * height); i++)
        {
            viewport_data[i] = 0;
        }
        refreshViewport();
    }

    // DEBUGGING: generates a checkered background pattern
    // WARNING: Very laggy at high resolutions!
    void generateCheckedBG()
    {
        for (int i = 0; i < resolution; i++)
        {
            if ((i + 1) % 2 == 0)
            {
                viewport_data[i] = 0.2;
                continue;
            }
            viewport_data[i] = 0.8;
        }
        refreshViewport();
    }

    // Plotting object(s) on the viewport.
    // Coordinate-specified point plotting (DEPRECATED)
    void plotPoint(int x, int y)
    {
        int coord_index = viewPlane->coords(x, y);
        if (0 <= coord_index && coord_index < resolution)
        {
            viewport_data[coord_index] = 1;
        }
    }
    // Point Object plotting
    void plotPoint(Point *point)
    {
        point->isDisplayed = true;
        displayed_points.push_back(point);
        plotPoint(point->x, point->y);
    }
    // Plots all points stored in viewport memory
    void refreshViewport()
    {
        clearAllPoints();
        for (int i = 0; i < displayed_points.size(); i++)
        {
            plotPoint(displayed_points[i]->x, displayed_points[i]->y);
        }
    }
    // Plots any complex object with a line_points vector
    void plotObject(vector<Point *> line_points)
    {
        for (int i = 0; i < line_points.size(); i++)
        {
            plotPoint(line_points[i]);
        }
    }

    // Viewport Clearing Methods
    // Clears everything in viewport (even BG)
    void clearImage()
    {
        clearAllPoints();
        generateBG();
    }
    // Clears all points stored in viewport
    void clearAllPoints()
    {
        for (int i = displayed_points.size() - 1; i >= 0; i--)
        {
            int coord_index = viewPlane->coords(displayed_points[i]->x, displayed_points[i]->y);
            if (coord_index < resolution && coord_index > 0)
                viewport_data[coord_index] = 0;
            displayed_points[i]->isDisplayed = false;
            displayed_points.pop_back();
        }
    }

    // NOT WORKING
    void scale(int scale_x, int scale_y, int origin_x, int origin_y)
    {
        for (int i = 0; i < displayed_points.size(); i++)
        {
            int x = displayed_points[i]->x;
            int y = displayed_points[i]->y;

            displayed_points[i]->x = origin_x + scale_x * (x - origin_x);
            displayed_points[i]->y = origin_y + scale_y * (y - origin_y);
            cout << x << ", " << displayed_points[i]->x << endl;
        }
    }
    void scale(int scale_value, int origin_x, int origin_y)
    {
        scale(scale_value, scale_value, origin_x, origin_y);
    }
    // Returns the complete rendering data in viewport
    double *getViewportData()
    {
        return viewport_data;
    }
};

int main()
{
    Screen screen(RESOLUTION_X, RESOLUTION_Y);
    Viewport viewport;
    cout << "Viewport created!" << endl;

    Line line(0, 0, RESOLUTION_X, RESOLUTION_Y);
    Circle circle(RESOLUTION_X / 2, RESOLUTION_Y / 2, 50);
    viewport.plotObject(circle.getLinePoints());

    screen.renderToFile(viewport.getViewportData());
    cout << "Viewport rendered!" << endl;
}

// BG Rendering and manual point plotting test
//  DEPRECATED NAMES USED!!
//   string image[resolution];
//  for (int i = 0; i < resolution; i++)
//  {
//      if (i % 2 == 0)
//      {
//          image[i] = "░░";
//          continue;
//      }
//      image[i] = "▒▒";
//  }
//  CoordinatePlane coordinate_plane(resolution_x, resolution_y);
//  image[coordinate_plane.coords(0, 5)] = "██";

// Animation test
//  DEPRECATED NAMES USED!!
//   for (int x = 0; x < RESOLUTION_X; x++)
//   {
//       for (int y = 0; y < RESOLUTION_Y; y++)
//       {
//           viewport.plotPoint(x, y);
//           new_screen.renderScreen(viewport.image_data);
//           cout << endl;
//           this_thread::sleep_for(0.08s);
//       }
//   }
//  viewport.plotPoint(3, 5);
//  viewport.plotPoint(28, 9);
//  new_screen.renderScreen(viewport.image_data);

// Points clearing test
//  Point point1(3, 5);
//  Point point2(28, 9);
//  viewport.plotPoint(&point1);
//  viewport.plotPoint(&point2);
//  new_screen.render(viewport.getViewportData());
//  viewport.clearAllPoints();
//  viewport.generateCheckedBG();
//  cout << endl;
//  new_screen.render(viewport.getViewportData());

// Line plot test
//  Point point1(0, 8);
//  Point point2(68, 32);
//  viewport.plotPoint(&point1);
//  viewport.plotPoint(&point2);
//  cout << "points created." << endl;
//  Line line(&point1, &point2);
//  cout << "line created." << endl;
//  viewport.plotLine(line);
//  cout << "line plotted." << endl;
//  new_screen.render(viewport.getViewportData());
//  cout << "rendered." << endl;

// Point Translate test
//  Point point1(0, 0);
//  viewport.plotPoint(&point1);
//  for (int i = 0; i < 10; i++)
//  {
//      viewport.clearAllPoints();
//      viewport.generateCheckedBG();
//      point1.Translate(1, 1);
//      viewport.plotPoint(&point1);
//      new_screen.render(viewport.getViewportData());
//      cout << endl;
//  }

// Line Translate test
//  Point point1(0, 0);
//  Point point2(10, 10);
//  viewport.plotPoint(&point1);
//  viewport.plotPoint(&point2);
//  cout << "points created." << endl;
// Line line(&point1, &point2);
// cout << "line created." << endl;
// viewport.plotLine(line);
// cout << "line plotted." << endl;
// screen.render(viewport.getViewportData());
// cout << "rendered." << endl;
// line.Translate(20, 20);
// viewport.clearAllPoints();
// viewport.generateCheckedBG();
// viewport.plotLine(line);
// screen.render(viewport.getViewportData());

// Circle Plot test
// Point point3(50, 20);
// viewport.plotPoint(&point3);
// Circle circle(&point3, 20);
// viewport.plotCircle(circle);
// screen.render(viewport.getViewportData());

// Car Drawing
//  // Car shapes
//  Point point1(10, 10);
//  Point point2(60, 10);
//  Point point3(40, 30);
//  Circle circle1(&point1, 10);
//  Circle circle2(&point2, 10);
//  Line line1(&point1, &point2);
//  Line line2(&point2, &point3);
//  viewport.plotCircle(&circle1);
//  viewport.plotCircle(&circle2);
//  viewport.plotLine(&line1);
//  viewport.plotLine(&line2);
//  screen.render(viewport.getViewportData());
//  // move car
//  circle1.Translate(30, 20);
//  circle2.Translate(30, 20);
//  line1.Translate(30, 20);
//  line2.Translate(30, 20);
//  viewport.clearImage();
//  viewport.plotCircle(&circle1);
//  viewport.plotCircle(&circle2);
//  viewport.plotLine(&line1);
//  viewport.plotLine(&line2);
//  screen.render(viewport.getViewportData());