#pragma once

class Label : public Node {

public:

    Surface surface;

    SDL_Point origin;

    Label() {
        origin = {0, 0};
    }

    void set_text(const std::string &text, const Font &font,
            const SDL_Colour &colour) {

        surface = Surface(text, font, colour);
    }

    void render(Renderer &renderer) override {
        renderer.copy(surface, origin);
    }

};
