#pragma once

class TestState : public State {

private:

    Button save_button;
    Button open_button;

    Button move_button;

    Button route_button;

    Button add_button;
    Button remove_button;

    Button cut_button;
    Button copy_button;
    Button paste_button;

    Button undo_button;
    Button redo_button;

    ColourRectangle panel;

public:

    TestState() {
        Surface button_surface("buttons");

        save_button.normal_surface = button_surface({0, 0, 32, 32});
        save_button.pressed_surface = button_surface({32, 0, 32, 32});
        save_button.render_origin = {16, 16};

        open_button.normal_surface = button_surface({64, 0, 32, 32});
        open_button.pressed_surface = button_surface({96, 0, 32, 32});
        open_button.render_origin = {64, 16};

        move_button.normal_surface = button_surface({0, 32, 32, 32});
        move_button.pressed_surface = button_surface({32, 32, 32, 32});
        move_button.render_origin = {16, 64};

        route_button.normal_surface = button_surface({0, 64, 32, 32});
        route_button.pressed_surface = button_surface({32, 64, 32, 32});
        route_button.render_origin = {16, 112};

        add_button.normal_surface = button_surface({0, 96, 32, 32});
        add_button.pressed_surface = button_surface({32, 96, 32, 32});
        add_button.render_origin = {16, 160};

        remove_button.normal_surface = button_surface({64, 96, 32, 32});
        remove_button.pressed_surface = button_surface({96, 96, 32, 32});
        remove_button.render_origin = {64, 160};

        copy_button.normal_surface = button_surface({0, 128, 32, 32});
        copy_button.pressed_surface = button_surface({32, 128, 32, 32});
        copy_button.render_origin = {16, 208};

        paste_button.normal_surface = button_surface({64, 128, 32, 32});
        paste_button.pressed_surface = button_surface({96, 128, 32, 32});
        paste_button.render_origin = {64, 208};

        cut_button.normal_surface = button_surface({0, 160, 32, 32});
        cut_button.pressed_surface = button_surface({32, 160, 32, 32});
        cut_button.render_origin = {16, 256};

        undo_button.normal_surface = button_surface({0, 192, 32, 32});
        undo_button.pressed_surface = button_surface({32, 192, 32, 32});
        undo_button.render_origin = {16, 304};

        redo_button.normal_surface = button_surface({64, 192, 32, 32});
        redo_button.pressed_surface = button_surface({96, 192, 32, 32});
        redo_button.render_origin = {64, 304};

        panel.colour = {234, 248, 191, 255};
        panel.render_region = {8, 8, 104, 344};
    }

    // void start() override {}
    // void stop() override {}

    // void key_pressed(const SDL_Keycode &keycode) override {}
    // void key_released(const SDL_Keycode &keycode) override {}

    // void mouse_moved(const SDL_MouseMotionEvent &motion_event) override {}

    void mouse_pressed(const SDL_MouseButtonEvent &button_event) override {
        std::vector<std::reference_wrapper<Button>> buttons = {
            save_button,
            open_button,
            move_button,
            route_button,
            add_button,
            remove_button,
            cut_button,
            copy_button,
            paste_button,
            undo_button,
            redo_button,
        };

        SDL_Point point = {button_event.x, button_event.y};
        SDL_Rect region = {0, 0, 32, 32};
        for(Button &button : buttons) {
            region.x = button.render_origin.x;
            region.y = button.render_origin.y;

            if(SDL_PointInRect(&point, &region))
                button.pressed = true;
        }
    }

    void mouse_released(const SDL_MouseButtonEvent &button_event) override {
        std::vector<std::reference_wrapper<Button>> buttons = {
            save_button,
            open_button,
            move_button,
            route_button,
            add_button,
            remove_button,
            cut_button,
            copy_button,
            paste_button,
            undo_button,
            redo_button,
        };

        for(Button &button : buttons)
            button.pressed = false;
    }

    // void wheel_moved(const SDL_MouseWheelEvent &wheel_event) override {}

    void render(Renderer &renderer) override {
        panel.render(renderer);
        
        std::vector<std::reference_wrapper<Button>> buttons = {
            save_button,
            open_button,
            move_button,
            route_button,
            add_button,
            remove_button,
            cut_button,
            copy_button,
            paste_button,
            undo_button,
            redo_button,
        };

        for(Button &button : buttons)
            button.render(renderer);
    }

    // void update(const long &ticks) override {}

};
