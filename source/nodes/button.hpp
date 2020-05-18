#pragma once

class Button : public Node {

public:

    Surface normal_surface;

    Surface pressed_surface;

    bool toggle;

    bool pressed;

    Button(const Surface &normal_surface, const Surface &pressed_surface) {
        this->normal_surface = normal_surface;
        this->pressed_surface = pressed_surface;

        toggle = false;
        pressed = false;
    }

    void render(Renderer &renderer) override {
        Surface &surface = pressed ? pressed_surface : normal_surface;
        renderer.copy(surface, render_region);
    }

};
