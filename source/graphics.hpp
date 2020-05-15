#pragma once

#include <iostream>
#include <memory>
#include <string>

#include <SDL2/SDL.h>

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

typedef std::size_t Hash;

namespace std {

template<>
struct hash<SDL_Rect> {

    Hash operator()(SDL_Rect const &region) const noexcept {
        Hash x = std::hash<int>{}(region.x);
        Hash y = std::hash<int>{}(region.y);
        Hash width = std::hash<int>{}(region.w);
        Hash height = std::hash<int>{}(region.h);

        return x ^ (y ^ (width ^ (height << 1) << 1) << 1);
    }

};

template<>
struct hash<SDL_Colour> {

    Hash operator()(SDL_Colour const &colour) const noexcept {
        Hash red = std::hash<uint8_t>{}(colour.r);
        Hash green = std::hash<uint8_t>{}(colour.g);
        Hash blue = std::hash<uint8_t>{}(colour.b);
        Hash alpha = std::hash<uint8_t>{}(colour.a);

        return red ^ (green ^ (blue ^ (alpha << 1) << 1) << 1);
    }

};

};

// ************************************************************************ Font

struct Font {

    std::shared_ptr<TTF_Font> data;

    Hash hash;

    std::string name;

    unsigned int size;

    Font(const std::string &name, const unsigned int &size);

};

Font::Font(const std::string &name, const unsigned int &size) {
    this->name = name;
    this->size = size;
    this->hash = std::hash<std::string>{}(name) ^
            (std::hash<unsigned int>{}(size) << 1);

    // Create an absolute path for the font file name
    std::string path = SDL_GetBasePath();
    path += "..\\resources\\" + name;

    // Try to load the font
    data = std::shared_ptr<TTF_Font>(TTF_OpenFont(path.c_str(), size),
            TTF_CloseFont);

    // Check the font was loaded properly
    if(data == nullptr) {
        std::cerr << "Couldn't open font:" << std::endl <<
                "\t" << path << std::endl <<
                "\tTTF error: " << TTF_GetError() << std::endl;
        throw -1;
    }
}

// ********************************************************************* Surface

struct Surface {

    std::shared_ptr<SDL_Surface> data;

    Hash hash;

    Surface(const std::string &file_name);
    Surface(const SDL_Rect &size, const SDL_Colour &colour);
    Surface(const std::string &text, const Font &font,
            const SDL_Colour &colour);

};

Surface::Surface(const std::string &file_name) {
    hash = std::hash<std::string>{}(file_name);

    // Create an absolute path for the font file name
    std::string path = SDL_GetBasePath();
    path += "..\\resources\\" + file_name;

    // Try to load the image
    data = std::shared_ptr<SDL_Surface>(IMG_Load(path.c_str()),
            SDL_FreeSurface);

    // Check the image was loaded properly
    if(data == nullptr) {
        std::cout << "Couldn't load image:" << std::endl <<
                "\t" << path << std::endl <<
                "\tSDL error: " << SDL_GetError() << std::endl;
        throw -1;
    }
}

Surface::Surface(const SDL_Rect &size, const SDL_Colour &colour) {
    hash = std::hash<SDL_Rect>{}(size) ^ (std::hash<SDL_Colour>{}(colour) << 1);

    // Try to create the surface
    data = std::shared_ptr<SDL_Surface>(SDL_CreateRGBSurface(0, size.w, size.h,
            32, 0, 0, 0, 0), SDL_FreeSurface);

    // Check the surface was created
    if(data == nullptr) {
        std::cout << "Couldn't create surface" << std::endl <<
                "\tSDL error: " << SDL_GetError() << std::endl;
        throw -1;
    }

    // Fill the surface with a given colour
    SDL_FillRect(data.get(), nullptr, SDL_MapRGBA(data.get()->format,
            colour.r, colour.g, colour.b, colour.a));
}

Surface::Surface(const std::string &text, const Font &font,
        const SDL_Colour &colour) {
    auto colour_hash = std::hash<SDL_Colour>{}(colour);
    auto text_hash = std::hash<std::string>{}(text);
    hash = text_hash ^ (font.hash ^ (colour_hash << 1) << 1);

    // Try to render the text using the font specified
    data = std::shared_ptr<SDL_Surface>(TTF_RenderText_Blended(font.data.get(),
            text.c_str(), colour), SDL_FreeSurface);

    // Check the text was created
    if(data == nullptr) {
        std::cout << "Couldn't create surface" << std::endl <<
                "\tSDL error: " << SDL_GetError() << std::endl;
        throw -1;
    }
}


// ********************************************************************** Window

class Window {

public:

    std::shared_ptr<SDL_Window> context;

    Window(const std::string &title, const unsigned int &width,
            const unsigned int &height);

    void set_icon(const Surface &surface) const;

    void hide() const;
    void show() const;

};

Window::Window(const std::string &title, const unsigned int &width,
        const unsigned int &height) {

    auto position = SDL_WINDOWPOS_CENTERED;
    auto flags = SDL_WINDOW_HIDDEN;
    context = std::shared_ptr<SDL_Window>(SDL_CreateWindow(title.c_str(),
            position, position, width, height, flags), SDL_DestroyWindow);
}

// Set the window icon
// NOTE: This is not the same as the application icon
void Window::set_icon(const Surface &surface) const {
    SDL_SetWindowIcon(context.get(), surface.data.get());
}

// Hide the window
// NOTE: This is not the same as minimizing it
void Window::hide() const {
    SDL_HideWindow(context.get());
}

// Show the window
void Window::show() const {
    SDL_ShowWindow(context.get());
}

// ******************************************************************** Renderer

class Renderer {

private:

    std::shared_ptr<SDL_Renderer> context;

    SDL_Colour clear_colour;

public:

    Renderer(const Window &window);

    void draw_line(const SDL_Point &from_point, const SDL_Point &to_point,
            const SDL_Colour &colour);
    void draw_filled_rectangle(const SDL_Rect &region,
            const SDL_Colour &colour);

    void set_clear_colour(const SDL_Colour &colour);
    void set_draw_colour(const SDL_Colour &colour);

    void clear();
    void present();

};

Renderer::Renderer(const Window &window) {

    // Try to create renderer context
    auto flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    context = std::shared_ptr<SDL_Renderer>(
            SDL_CreateRenderer(window.context.get(), -1, flags),
            SDL_DestroyRenderer);

    // Check renderer was created properly
    if(context == nullptr) {
        std::cerr << "Couldn't create renderer" << std::endl <<
                "\t" << "SDL error:" << std::endl <<
                SDL_GetError() << std::endl;
        throw -1;
    }

    window.show();

    clear_colour = {0, 0, 0, 0};
}

// Draw a single-pixel thick line on the render context
void Renderer::draw_line(const SDL_Point &from_point, const SDL_Point &to_point,
        const SDL_Colour &colour) {

    set_draw_colour(colour);
    SDL_RenderDrawLine(context.get(), from_point.x, from_point.y, to_point.x,
            to_point.y);
    set_draw_colour(clear_colour);
}

// Draw a solid rectangle of a given colour to the render context
void Renderer::draw_filled_rectangle(const SDL_Rect &region,
        const SDL_Colour &colour) {

    set_draw_colour(colour);
    SDL_RenderFillRect(context.get(), &region);
    set_draw_colour(clear_colour);
}

// Set the colour to which the renderer is cleared
void Renderer::set_clear_colour(const SDL_Colour &colour) {
    clear_colour = colour;
}

// Set the colour used be the renderer, as used by the draw functions
void Renderer::set_draw_colour(const SDL_Colour &colour) {
    SDL_SetRenderDrawColor(context.get(), colour.r, colour.g, colour.b,
            colour.a);
}

// Clear the render context
void Renderer::clear() {
    SDL_RenderClear(context.get());
}

// Present the render context to the window
void Renderer::present() {
    SDL_RenderPresent(context.get());
}
