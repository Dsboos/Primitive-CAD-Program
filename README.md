# Primitive-CAD-program-CPP-
A primitive and simple CAD program I created that renders 2D lines and shapes onto the console using nothing but standard libraries.

# How to Use
There are prebuilt classes for all basic geometries you may wish to plot. 
However you need to setup the viewport first with parameters such as the screen resolution.

## Viewport Initialization
You need to first create a Screen object, initialized with the values for x_resolution and y_resolution of the display size you want. 
Next, you need to create a Viewport that will automatically generate a cartesian coordinate system that fits the screen. 
The Viewport handles all plotting and drawing functions and subsequently encodes them into data arrays. 
The Screen performs the rendering of this data. The render function renders three shades corresponding to 1, 0.8, 0.2, 0 (darkest to transparent).

```cpp 
      Screen screen(RESOLUTION_X, RESOLUTION_Y);
    Viewport viewport;
```
## Plotting Points
The Point class is provided for creating points that are initialized with their x and y coordinates on the Viewport. Any point that falls outside of the displayed coordinates will not be rendered.
The plot must be plotted onto the viewport after creation by using the plotPoint() method of the Viewport (pass by refrence). 
To see the point rendered, use the renderToFile() method of Screen and pass the return value of the getViewportData() method of Viewport. Alternately use render() method to render right to console.

```cpp
    Point pt1(12,36);
    viewport.plotPoint(&pt1); //Plots point onto viewport cartesian plane
    screen.renderToFile(viewport.getViewportData()); //Passes image data to Screen to be rendered into a .txt file
```
The point has a Translate() method that takes in parameters (int x_distance, int y_distance) corresponsing to the desired translation.

```cpp
    Point pt1(12,36);
    pt1.Translate(5,5) //Translates the point 5 to right and 5 up
    viewport.plotPoint(&pt1);
    screen.renderToFile(viewport.getViewportData());
```
## Creating and Plotting Lines
The Line class can be used to create vectors of Points that trace a line between two desired Point Objects. Alternately the Line object can be created without passing Point objects to it and instead the coordinates (int x1,int y1,int x2,int y2).
Use plotObject() method to plot line by passing the value of Line.getLinePoints() to it.

```cpp
    Line line1(2, 2, 10, 10); //Line from (2,2) to (10,10)
    viewport.plotObject(line1.getLinePoints()); //Plots all points belonging to line onto viewport
    screen.renderToFile(viewport.getViewportData()); //render
```

## Creating Circles
The Circles class takes in two parameters: center and radius. One option is to pass the center as a Point Object along with an int value of radius.
Alternately, two int values may be passed as x_center and y_center respectively along with int radius.

```cpp
    Circle circle(RESOLUTION_X / 2, RESOLUTION_Y / 2, 50); //creates circle at centre of screen with radius=50
    viewport.plotObject(circle.getLinePoints());  //plot circle points onto viewport
    screen.renderToFile(viewport.getViewportData()); // render
```

## Debugging Tools
### 1)(Viewport Method) generateCheckedBG(): 
generates a checked bg but lags a lot at high resolutions.
### 2)(Viewport Method) refreshViewport(): 
refreshes viewport by deleting all pts and replotting only pts that exist in the displayed_points vector of Viewport. Clears all ghost points.
### 3)(Viewport Method) clearImage(): 
clears everything and makes Viewport completely blank again.

## Work in Progress
### 1)Scale and Rotate functions for Geometric Objects (Lines, Circles, Curves etc.)

### 2)Scale and Rotate functions for entire Viewport

### 3)Curves (Bezier, Polynomial)

### 4)Rectangles and Polygons
