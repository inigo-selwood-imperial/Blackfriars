#pragma once

class Button : public Node {

public:

    Surface normal_surface;

    Surface pressed_surface;

    SDL_Point render_origin;

    bool pressed;

    Button() {
        render_origin = {0, 0};
        pressed = false;
    }

    void render(Renderer &renderer) override {
        Surface &surface = pressed ? pressed_surface : normal_surface;
        renderer.copy(surface, render_origin);
    }

};
