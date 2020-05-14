#pragma once

#include "graphics.hpp"

class State {

public:

    virtual void start(Renderer &renderer) {}
    virtual void stop() {}

    virtual void key_pressed(const SDL_Keycode &keycode) {}
    virtual void key_released(const SDL_Keycode &keycode) {}

    virtual void mouse_moved(const SDL_MouseMotionEvent &motion_event) {}
    virtual void mouse_pressed(const SDL_MouseButtonEvent &button_event) {}
    virtual void mouse_released(const SDL_MouseButtonEvent &button_event) {}

    virtual void wheel_moved(const SDL_MouseWheelEvent &wheel_event) {}

    virtual void render(Renderer &renderer) {}

};
