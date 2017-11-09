//
// Created by ephtron on 08.11.17.
//

#include "common/SDL2.hpp"
#include "common/timediff.hpp"

#include <cairo.h>
#include <cairo/cairo-gl.h>
#include <GL/glu.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>
#include <iostream>


// Size of the surface.
const unsigned int WIDTH = 512;
const unsigned int HEIGHT = 512;

int main(int argc, char** argv) {
    SDL2Window window;
    Display *disp;
    Window root;
    int scr;

    disp = XOpenDisplay(NULL);
    scr = DefaultScreen(disp);
    root = DefaultRootWindow(disp);

    if(!window.init(WIDTH, HEIGHT)) {
        std::cerr << "Couldn't initialize SDL2 window; fatal." << std::endl;
        return 2;
    }

    if(window.makeCurrent()) {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glViewport(0.0, 0.0, WIDTH, HEIGHT);
        glClearColor(0.0, 0.1, 0.2, 1.0);
    }

    cairo_device_t* device = cairo_glx_device_create(
            window.getDisplay(),
            reinterpret_cast<GLXContext>(window.getCairoContext())
    );

    if(!device) {
        std::cerr << "Couldn't create device; fatal." << std::endl;

        return 3;
    }

    GLuint texture = 0;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, nullptr);

    cairo_surface_t* surface_tex = cairo_gl_surface_create_for_texture(
            device,
            CAIRO_CONTENT_COLOR_ALPHA,
            texture,
            WIDTH,
            HEIGHT
    );

    if(!surface_tex) {
        std::cerr << "Couldn't create surface; fatal." << std::endl;

        return 4;
    }

    unsigned long frames = 0;
    unsigned long cairoTime = 0;
    unsigned long sdlTime = 0;
    cairo_surface_t* sf_screen;

    auto drawStart = std::chrono::system_clock::now();

    window.main([&]() {
        auto start = std::chrono::system_clock::now();

        if(window.makeCairoCurrent()) {
            // get surface of current screen
            sf_screen = cairo_xlib_surface_create(disp, root, DefaultVisual(disp, scr), WIDTH, HEIGHT);

            // create texture surface
            cairo_t* cr = cairo_create(surface_tex);

            // set current screen as source and paint it on the texture surface
            cairo_set_source_surface(cr, sf_screen, 0, 0);
            cairo_paint(cr);


            cairo_surface_flush(surface_tex);
            cairo_destroy(cr);

            // clear current screen surface
            cairo_surface_destroy(sf_screen);
            sf_screen = nullptr;

            cairo_gl_surface_swapbuffers(surface_tex);
        }

        cairoTime += timediff(start);

        start = std::chrono::system_clock::now();

        if(window.makeCurrent()) {
            int x = 0;
            int y = 0;
            int width = 512;
            int height = 512;


            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH,HEIGHT, 0,GL_BGRA, GL_UNSIGNED_BYTE, data);

            glBindTexture(GL_TEXTURE_2D, texture);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glBegin(GL_QUADS);

            // Bottom-Left
            glTexCoord2i(0, 1);
            glVertex2i(x, y);

            // Upper-Left
            glTexCoord2i(0, 0);
            glVertex2i(x, y + height);

            // Upper-Right
            glTexCoord2i(1, 0);
            glVertex2i(x + width, y + height);

            // Bottom-Right
            glTexCoord2i(1, 1);
            glVertex2i(x + width, y);

            glEnd();
        }

        sdlTime += timediff(start);

        frames++;
    });

    auto fps = frames / (timediff(drawStart) / 1000);

    std::cout << "FPS: " << fps << std::endl;
    std::cout << "Cairo average time: " << cairoTime / frames << "ms" << std::endl;
    std::cout << "SDL2 average time: " << sdlTime / frames << "ms" << std::endl;

    cairo_surface_destroy(surface_tex);
    cairo_device_destroy(device);

    window.deinit();

    return 0;
}

