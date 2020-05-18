#pragma once

class Font {

public:

    std::shared_ptr<TTF_Font> data;

    std::string name;

    unsigned int point_size;

    unsigned int height;

    Font(const std::string &name, const unsigned int &size);

};

Font::Font(const std::string &name, const unsigned int &size) {
    data = Resources::load_font(name, size);

    this->name = TTF_FontFaceFamilyName(data.get());
    point_size = size;
    height = TTF_FontHeight(data.get());
}
