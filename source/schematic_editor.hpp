#pragma once

#include <functional>
#include <map>
#include <vector>

#include "state.hpp"

// ********************************************************************** Button

class Button {

public:

    Surface surface;

    SDL_Point copy_origin_normal;
    SDL_Point copy_origin_pressed;

    SDL_Rect render_region;

    bool pressed;

    Button() {
        copy_origin_normal = copy_origin_pressed = {0, 0};
        render_region = {0, 0, 0, 0};

        pressed = false;
    }

    void render(Renderer &renderer) {
        auto copy_origin = pressed ? copy_origin_pressed : copy_origin_normal;
        SDL_Rect copy_region = {copy_origin.x, copy_origin.y,
                render_region.w, render_region.h};

        renderer.copy(surface, copy_region, render_region);
    }

};

// ********************************************************************* Toolbar

class Toolbar {

private:

    Button copy_button;
    Button cut_button;
    Button move_button;
    Button paste_button;
    Button place_button;
    Button remove_button;
    Button undo_button;

public:

    Toolbar();

    void mouse_pressed(const SDL_MouseButtonEvent &button_event);
    std::string mouse_released(const SDL_MouseButtonEvent &button_event);

    void render(Renderer &renderer);

};

Toolbar::Toolbar() {
    std::vector<std::reference_wrapper<Button>> buttons = {
        copy_button,
        cut_button,
        move_button,
        paste_button,
        place_button,
        remove_button,
        undo_button
    };

    Surface surface("buttons.png");

    int index = 0;
    for(Button &button : buttons) {
        button.surface = surface;

        button.copy_origin_normal = {0, index * 32};
        button.copy_origin_pressed = {32, index * 32};
        button.render_region = {16, 16 + (index * 48), 32, 32};

        index += 1;
    }
}

void Toolbar::mouse_pressed(const SDL_MouseButtonEvent &button_event) {
    SDL_Point point_pressed = {button_event.x, button_event.y};

    std::vector<std::reference_wrapper<Button>> buttons = {
        copy_button,
        cut_button,
        move_button,
        paste_button,
        place_button,
        remove_button,
        undo_button
    };

    for(Button &button : buttons) {
        if(SDL_PointInRect(&point_pressed, &button.render_region))
            button.pressed = true;
    }
}

std::string Toolbar::mouse_released(const SDL_MouseButtonEvent &button_event) {
    SDL_Point point_released = {button_event.x, button_event.y};

    std::map<std::string, std::reference_wrapper<Button>> buttons = {
        {"copy", copy_button},
        {"cut", cut_button},
        {"move", move_button},
        {"paste", paste_button},
        {"place", place_button},
        {"remove", remove_button},
        {"undo", undo_button}
    };

    std::string button_pressed = "";
    for(std::pair<std::string, Button &>pair : buttons) {
        pair.second.pressed = false;
        if(SDL_PointInRect(&point_released, &pair.second.render_region)
                && pair.second.pressed)
            button_pressed = "";
    }

    return button_pressed;
}

void Toolbar::render(Renderer &renderer) {
    copy_button.render(renderer);
    cut_button.render(renderer);
    move_button.render(renderer);
    paste_button.render(renderer);
    place_button.render(renderer);
    remove_button.render(renderer);
    undo_button.render(renderer);
}

// ******************************************************************* Schematic

class Schematic {

};

// ********************************************************************* Textbox

class Label {

private:

    Surface surface;

public:

    void set_text(const std::string &text, const Font &font,
            const SDL_Colour colour = {0, 0, 0, 0}) {

        surface = Surface(text, font, colour);
    }

    void render(Renderer &renderer) {
        // renderer.copy(texture, )
    }

};

class Textbox {

};

// ************************************************************* SchematicEditor

class SchematicEditor : public State {

private:

    Toolbar toolbar;

    // Schematic schematic;

    // Textbox input_textbox;

    // Textbox status_textbox;

    // virtual void start() {}
    // virtual void stop() {}

    // virtual void key_pressed(const SDL_Keycode &keycode) {}
    // virtual void key_released(const SDL_Keycode &keycode) {}

    // virtual void mouse_moved(const SDL_MouseMotionEvent &motion_event) {}

    void mouse_pressed(const SDL_MouseButtonEvent &button_event) override {
        toolbar.mouse_pressed(button_event);
    }

    void mouse_released(const SDL_MouseButtonEvent &button_event) override {
        toolbar.mouse_released(button_event);
    }

    // virtual void wheel_moved(const SDL_MouseWheelEvent &wheel_event) {}

    void render(Renderer &renderer) override {
        toolbar.render(renderer);
    }

};
