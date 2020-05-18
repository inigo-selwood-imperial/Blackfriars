#pragma once

class ColourRectangle : public Node {

public:

    SDL_Colour colour;

    void render(Renderer &renderer) override {
        renderer.draw_rectangle(render_region, colour);
    }

};
