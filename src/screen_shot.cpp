//
// Created by ephtron on 02.11.17.
//

#include <iostream>
#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>

int main(int argc, char** argv) {
    Display *disp;
    Window root;
    cairo_surface_t *surface;
    int scr;

    disp = XOpenDisplay(NULL);
    scr = DefaultScreen(disp);
    root = DefaultRootWindow(disp);

    surface = cairo_xlib_surface_create(disp, root, DefaultVisual(disp, scr),
            DisplayWidth(disp, scr), DisplayHeight(disp, scr));
    cairo_surface_write_to_png(
            surface,
            "test.png");
    cairo_surface_destroy(surface);

    std::cout << "hello" << std::endl;

    return 0;
}

