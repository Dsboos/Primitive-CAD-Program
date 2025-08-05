#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <unordered_map>
using namespace std;

const int RESOLUTION_X = 481;
const int RESOLUTION_Y = 280;
const int RESOLUTION = RESOLUTION_X * RESOLUTION_Y;

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
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

//--------------------------------------------------------
// General parent class for all complex geometric shapes
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

//--------------------------------------------------------
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
    Line(pair<int, int> p1, pair<int, int> p2)
    {
        generateLinePoints(p1.first, p1.second, p2.first, p2.second);
    }
    void generateLinePoints(double x1, double y1, double x2, double y2)
    {
        double mag = sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
        for (double t = 0; t < mag; t += 1.414) // increment by sqrt(2).
        {
            int x = x1 + t * (x2 - x1) / mag;
            int y = y1 + t * (y2 - y1) / mag;
            Point *point = new Point(x, y);
            line_points.push_back(point);
        }
    }
};

//--------------------------------------------------------
// Circle object
class Circle : public geometricObject
{
public:
    Circle(Point *centre, int radius)
    {
        generateLinePoints(centre->x, centre->y, radius);
    }
    Circle(pair<int, int> centre, int radius)
    {
        generateLinePoints(centre.first, centre.second, radius);
    }
    void generateLinePoints(int centre_x, int centre_y, int radius)
    {
        for (double t = 0; t < (2 * 3.1415); t += 1.0 / radius)
        {
            int x = centre_x + radius * cos(t);
            int y = centre_y + radius * sin(t);
            Point *point = new Point(x, y);
            line_points.push_back(point);
        }
    }
};

//--------------------------------------------------------
//(WIP) Bezier Curve
class Bezier : public geometricObject
{
public:
    Bezier(pair<int, int> p1, pair<int, int> p2, pair<int, int> p3)
    {
        generateLinePointsQuadratic(p1, p2, p3);
    }
    Bezier(pair<int, int> p1, pair<int, int> p2, pair<int, int> p3, pair<int, int> p4)
    {
        generateLinePointsCubic(p1, p2, p3, p4);
    }

    void generateLinePointsQuadratic(pair<int, int> p1, pair<int, int> p2, pair<int, int> p3)
    {
        int max_y = max(abs(p2.second - p1.second), abs(p3.second - p1.second));
        int max_x = max(abs(p2.first - p1.first), abs(p3.first - p1.first));
        for (double t = 0; t <= 1; t += 1.0 / max(max_x, max_y))
        {
            int x = pow((1 - t), 2) * p1.first + 2 * t * (1 - t) * p2.first + pow(t, 2) * p3.first;
            int y = pow((1 - t), 2) * p1.second + 2 * t * (1 - t) * p2.second + pow(t, 2) * p3.second;
            Point *point = new Point(x, y);
            line_points.push_back(point);
        }
    }
    void generateLinePointsCubic(pair<int, int> p1, pair<int, int> p2, pair<int, int> p3, pair<int, int> p4)
    {
        int max_y = max(max(abs(p2.second - p1.second), abs(p3.second - p1.second)), abs(p4.second - p1.second));
        int max_x = max(max(abs(p2.first - p1.first), abs(p3.first - p1.first)), abs(p4.first - p1.first));
        for (double t = 0; t <= 1; t += 1.0 / max(max_x, max_y))
        {
            int x = pow((1 - t), 3) * p1.first + 3 * t * pow((1 - t), 2) * p2.first + 3 * pow(t, 2) * (1 - t) * p3.first + pow(t, 3) * p4.first;
            int y = pow((1 - t), 3) * p1.second + 3 * t * pow((1 - t), 2) * p2.second + 3 * pow(t, 2) * (1 - t) * p3.second + pow(t, 3) * p4.second;
            Point *point = new Point(x, y);
            line_points.push_back(point);
        }
    }
};

//------------------------------------------------------------------------------
// Handles and generates rendering data for graphics
class Viewport
{
public:
    int width;
    int height;
    int resolution;
    double *viewport_data;
    CoordinatePlane *viewPlane;
    vector<pair<int, int>> displayed_points;
    Viewport(int resolution_x, int resolution_y)
    {
        width = resolution_x;
        height = resolution_y;
        resolution = resolution_x * resolution_y;
        viewport_data = new double[resolution];
        viewPlane = new CoordinatePlane(width, height);
    }
    ~Viewport()
    {
        delete[] viewport_data;
        delete viewPlane;
    }

    //--------------------------------------------------------
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
    }

    //--------------------------------------------------------
    // Plotting object(s) on the viewport.
    // Coordinate-specified point plotting (DEPRECATED)
    void plotPoint(int x, int y)
    {
        pair<int, int> point = {x, y};
        displayed_points.push_back(point);
        encodePoints();
    }
    // Point Object plotting
    void plotPoint(Point *point)
    {
        point->isDisplayed = true;
        plotPoint(point->x, point->y);
        encodePoints();
    }
    // Plots any complex object with a line_points vector
    void plotObject(vector<Point *> line_points)
    {
        for (int i = 0; i < line_points.size(); i++)
        {
            plotPoint(line_points[i]);
        }
        encodePoints();
    }

    //--------------------------------------------------------
    // Viewport Clearing Methods
    // Clears all points stored in viewport
    void clearAllPoints()
    {
        displayed_points.erase(displayed_points.begin(), displayed_points.end());
        encodePoints();
    }

    //--------------------------------------------------------
    // Encodes all point data from displayed points to viewport data
    void encodePoints()
    {
        for (auto coord : displayed_points)
        {
            int coord_index = viewPlane->coords(coord.first, coord.second);
            if (0 <= coord_index && coord_index < resolution)
            {
                viewport_data[coord_index] = 1;
            }
        }
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
    Viewport viewport(RESOLUTION_X, RESOLUTION_Y);
    Circle circle({RESOLUTION_X / 2, RESOLUTION_Y / 2}, 15);
    viewport.plotObject(circle.getLinePoints());

    Line line({10, 100}, {300, 110});
    viewport.plotObject(line.getLinePoints());

    Bezier curve({470 / 2, RESOLUTION_Y - 250}, {906 / 2, RESOLUTION_Y - 201}, {367 / 2, RESOLUTION_Y - 86}, {650 / 2, RESOLUTION_Y - 31});
    viewport.plotObject(curve.getLinePoints());

    screen.renderToFile(viewport.getViewportData());
}
