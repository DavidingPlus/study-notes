# 在X11下使用cairo引擎绘制图形

`cairo`是一个方便和高性能的第三方`C`库。它可以作为绘制引擎，帮助我们绘制各种图形，并且提供多种输出方式。本文将在`Linux`下结合`X11`图形显示协议绘制一面五星红旗。

这是效果图：

<img src="https://img-blog.csdnimg.cn/direct/399d4439eeff4e02812c87f6250c3de3.png" alt="6be4cdf74bcd179c38d446809ccb67e0" style="zoom: 50%;" />

代码如下：

```cpp
#include <iostream>
#include <chrono>
#include <exception>
#include <cmath>

#include <X11/Xlib.h>

#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>


void draw_star(cairo_t *cr, double cx, double cy, double radius, double rotation);


// now only run on unix with X11
int main()
{
    Display *dpy = XOpenDisplay(nullptr);
    if (!dpy)
    {
        throw std::runtime_error("Failed to open X display");
    }

    int screen = DefaultScreen(dpy);
    Window w = XCreateSimpleWindow(
        dpy,
        RootWindow(dpy, DefaultScreen(dpy)),
        100, 100, 640, 480, 0,
        BlackPixel(dpy, screen),
        BlackPixel(dpy, screen));
    XSelectInput(dpy, w, ExposureMask);

    XMapWindow(dpy, w);

    std::cout << "Entering loop ..." << std::endl;

    XWindowAttributes attr;
    XGetWindowAttributes(dpy, w, &attr);
    cairo_surface_t *surface = cairo_xlib_surface_create(dpy, w, attr.visual, attr.width, attr.height);
    cairo_t *cr = cairo_create(surface);

    XEvent e;
    while (true)
    {
        XNextEvent(dpy, &e);
        switch (e.type)
        {
            case Expose:
            {
                std::cout << "event: Expose" << std::endl;

                cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
                cairo_paint(cr);

                cairo_set_source_rgb(cr, 1.0, 1.0, 0.0);

                draw_star(cr, 100.0, 100.0, 50.0, M_PI / 2);

                double small_star_radius = 20.0;
                double positions[4][2] = {
                    {200.0, 40.0},
                    {240.0, 80.0},
                    {240.0, 140.0},
                    {200.0, 180.0}};

                for (int i = 0; i < 4; ++i)
                {
                    double x = positions[i][0];
                    double y = positions[i][1];
                    double angle = atan2(y - 100.0, x - 100.0);
                    draw_star(cr, x, y, small_star_radius, angle);
                }

                break;
            }
            default:
                std::cout << "event: " << e.type << std::endl;
                break;
        }
    }

    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    XDestroyWindow(dpy, w);
    XCloseDisplay(dpy);


    return 0;
}


void draw_star(cairo_t *cr, double cx, double cy, double radius, double rotation)
{
    cairo_move_to(cr, cx + radius * cos(rotation), cy - radius * sin(rotation));
    for (int i = 1; i < 5; ++i)
    {
        double angle = i * 144.0 * (M_PI / 180.0) + rotation;
        cairo_line_to(cr, cx + radius * cos(angle), cy - radius * sin(angle));
    }
    cairo_close_path(cr);
    cairo_fill(cr);
}

```

TODO

