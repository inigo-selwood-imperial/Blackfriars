#pragma once

class Surface {

public:

    std::shared_ptr<SDL_Surface> data;

    SDL_Rect size;

    bool mirrored;

    double theta;

    Hash hash;

    Surface();
    Surface(const std::string &name);
    Surface(const std::string &text, const Font &font,
            const SDL_Colour &colour);

    uint32_t pixel_value(const SDL_Point &point) const;
    SDL_Colour pixel_colour(const SDL_Point &point) const;

    void update_hash();

};

Surface::Surface() {
    size = {0, 0, 0, 0};
    mirrored = false;
    theta = 0;
    hash = 0;
}

Surface::Surface(const std::string &name) : Surface() {
    data = Resources::load_image(name);

    size = {0, 0, data->w, data->h};
    update_hash();
}

Surface::Surface(const std::string &text, const Font &font,
        const SDL_Colour &colour) : Surface() {

#ifdef DEBUG
    std::cout << "Creating text surface" << std::endl;
#endif // DEBUG

    data = std::shared_ptr<SDL_Surface>(TTF_RenderText_Blended(
            font.data.get(), text.c_str(), colour), Resources::delete_surface);

    if(data == nullptr) {
        std::cout << SDL_GetError() << std::endl;
        throw -1;
    }

    size = {0, 0, data->w, data->h};
    update_hash();
}

uint32_t Surface::pixel_value(const SDL_Point &point) const {
    int bits_per_pixel = data->format->BytesPerPixel;
    int pitch = point.y * data->pitch;
    int offset = point.x * bits_per_pixel;

    Uint8 *pixel = (Uint8 *)data->pixels + pitch + offset;
    switch (bits_per_pixel) {
    case 1:
        return *pixel;

    case 2:
        return *(Uint16 *)pixel;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return pixel[0] << 16 | pixel[1] << 8 | pixel[2];
        else
            return pixel[0] | pixel[1] << 8 | pixel[2] << 16;

    case 4:
        return *(Uint32 *)pixel;

    default:
        return 0;
    }
}

SDL_Colour Surface::pixel_colour(const SDL_Point &point) const {
    if(point.x < 0 || point.x >= data->w || point.y < 0 ||
            point.y >= data->h)
        return {0, 0, 0, 0};

    SDL_Colour colour;
    auto pixel = pixel_value(point);
    SDL_GetRGBA(pixel, data->format, &colour.r, &colour.g, &colour.b,
            &colour.a);
    return colour;
}

void Surface::update_hash() {
    std::string text = (char *)data->pixels;
    hash = std::hash<std::string>{}(text);
}
