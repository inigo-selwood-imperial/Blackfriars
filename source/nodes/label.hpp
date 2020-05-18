#pragma once

class Label : public Node {

public:

    void set_text(const std::string &text, const Font &font,
            const SDL_Colour &colour) {

        surface = Surface(text, font, colour);
    }

};
