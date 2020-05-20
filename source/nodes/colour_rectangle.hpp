#pragma once

class ColourRectangle : public Node {

public:

    SDL_Colour colour;

    SDL_Rect render_region;

    ColourRectangle() {
        colour = {0, 0, 0, 0};
        render_region = {0, 0, 0, 0};
    }

    void render(Renderer &renderer) override {
        renderer.draw_rectangle(render_region, colour);
    }

};
