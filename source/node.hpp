#pragma once

class Node {

public:

    SDL_Rect render_region;

    Surface surface;

    virtual void render(Renderer &renderer) {
        renderer.copy(surface, render_region);
    }

};
