#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include <SDL2/SDL.h>

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// TODO: Replace debug preprocessor guards with a logging system
#define DEBUG

// ************************************************************************ Hash

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

}; // Namespace std

// ************************************************************************ Font

struct Font {

    std::shared_ptr<TTF_Font> data;

    Hash hash;

    std::string name;

    unsigned int size;

    static void delete_font(TTF_Font *font);

    Font(const std::string &name, const unsigned int &size);

};

// Frees the memory held by a font
void Font::delete_font(TTF_Font *font) {
    if(font == nullptr)
        return;

#ifdef DEBUG
        std::cout << "Deleting font" << std::endl;
#endif // DEBUG

    TTF_CloseFont(font);
    font = nullptr;
}

Font::Font(const std::string &name, const unsigned int &size) {
    this->name = name;
    this->size = size;
    this->hash = std::hash<std::string>{}(name) ^
            (std::hash<unsigned int>{}(size) << 1);

    // Create an absolute path for the font file name
    std::string path = SDL_GetBasePath();
    path += "..\\resources\\" + name;

#ifdef DEBUG
    std::cout << "Creating font" << std::endl;
#endif // DEBUG

    // Try to load the font
    data = std::shared_ptr<TTF_Font>(TTF_OpenFont(path.c_str(), size),
            delete_font);

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

    static void delete_surface(SDL_Surface *surface);

    Surface() {}
    Surface(const std::string &file_name);
    Surface(const SDL_Rect &size, const SDL_Colour &colour);
    Surface(const std::string &text, const Font &font,
            const SDL_Colour &colour);

};

// Frees the memory held by a surface
void Surface::delete_surface(SDL_Surface *surface) {
    if(surface == nullptr)
        return;

#ifdef DEBUG
    std::cout << "Deleting surface" << std::endl;
#endif // DEBUG

    SDL_FreeSurface(surface);
    surface = nullptr;
}

Surface::Surface(const std::string &file_name) {
    hash = std::hash<std::string>{}(file_name);

    // Create an absolute path for the font file name
    std::string path = SDL_GetBasePath();
    path += "..\\resources\\" + file_name;

#ifdef DEBUG
    std::cout << "Creating surface" << std::endl;
#endif // DEBUG

    // Try to load the image
    data = std::shared_ptr<SDL_Surface>(IMG_Load(path.c_str()),
            delete_surface);

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

#ifdef DEBUG
    std::cout << "Creating surface" << std::endl;
#endif // DEBUG

    // Try to create the surface
    data = std::shared_ptr<SDL_Surface>(SDL_CreateRGBSurface(0, size.w, size.h,
            32, 0, 0, 0, 0), delete_surface);

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

#ifdef DEBUG
    std::cout << "Creating surface" << std::endl;
#endif // DEBUG

    // Try to create the text surface using the font specified
    data = std::shared_ptr<SDL_Surface>(TTF_RenderText_Blended(font.data.get(),
            text.c_str(), colour), delete_surface);

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

    static void delete_window(SDL_Window *window);

    Window(const std::string &title, const unsigned int &width,
            const unsigned int &height);

    void set_icon(const Surface &surface) const;

    void hide() const;
    void show() const;

};

// Frees the memory held by a window
void Window::delete_window(SDL_Window *window) {
    if(window == nullptr)
        return;

#ifdef DEBUG
    std::cout << "Deleting window" << std::endl;
#endif // DEBUG

    SDL_DestroyWindow(window);
    window = nullptr;
}

Window::Window(const std::string &title, const unsigned int &width,
        const unsigned int &height) {

#ifdef DEBUG
    std::cout << "Creating window" << std::endl;
#endif // DEBUG

    auto position = SDL_WINDOWPOS_CENTERED;
    auto flags = SDL_WINDOW_HIDDEN;
    context = std::shared_ptr<SDL_Window>(SDL_CreateWindow(title.c_str(),
            position, position, width, height, flags), delete_window);

    if(context == nullptr) {
        std::cerr << "Couldn't create window" << std::endl <<
                "\t" << "SDL error:" << std::endl <<
                SDL_GetError() << std::endl;
        throw -1;
    }
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

    std::map<Hash, std::shared_ptr<SDL_Texture>> textures;

    std::shared_ptr<SDL_Texture> get_texture(Surface &surface);

public:

    static void delete_renderer(SDL_Renderer *renderer);

    Renderer(const Window &window);

    void draw_line(const SDL_Point &from_point, const SDL_Point &to_point,
            const SDL_Colour &colour);
    void draw_filled_rectangle(const SDL_Rect &region,
            const SDL_Colour &colour);

    void set_clear_colour(const SDL_Colour &colour);
    void set_draw_colour(const SDL_Colour &colour);

    // TODO: Consider condensing into one function
    void copy(Surface &surface, const SDL_Rect &render_region);
    void copy(Surface &surface, const SDL_Rect &copy_region,
            const SDL_Rect &render_region);
    void copy(Surface &surface, const SDL_Rect &render_region,
            const bool &mirrored, const int &theta);
    void copy(Surface &surface, const SDL_Rect &copy_region,
            const SDL_Rect &render_region, const bool &mirrored,
            const int &theta);

    void clear();
    void present();

};

// Free the memory held by a renderer
void Renderer::delete_renderer(SDL_Renderer *renderer) {
    if(renderer == nullptr)
        return;

#ifdef DEBUG
    std::cout << "Deleting renderer" << std::endl;
#endif // DEBUG

    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
}

// Finds or loads the texture corresponding to a given surface
std::shared_ptr<SDL_Texture> Renderer::get_texture(Surface &surface) {
    if(textures[surface.hash] == nullptr) {

        // Check the surface is valid
        if(surface.data == nullptr) {
            std::cerr << "Can't create texture without surface data";
            throw -1;
        }

        // Create the texture
        auto texture = std::shared_ptr<SDL_Texture>(
            SDL_CreateTextureFromSurface(context.get(), surface.data.get()),
            SDL_DestroyTexture);

        // Check the texture was created properly
        if(texture == nullptr) {
            std::cerr << "Couldn't create texture from surface with hash: " <<
                    surface.hash << std::endl <<
                    "\tError message: " << SDL_GetError();
            throw -1;
        }

        // Add the texture to the texture map, and free the memory held by
        // the surface pointer
        textures[surface.hash] = texture;
        surface.data.reset();

        return texture;
    }

    // If the texture has already been loaded, return it
    else
        return textures[surface.hash];
}

Renderer::Renderer(const Window &window) {

#ifdef DEBUG
    std::cout << "Creating renderer" << std::endl;
#endif // DEBUG

    // Try to create renderer context
    auto flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    context = std::shared_ptr<SDL_Renderer>(
            SDL_CreateRenderer(window.context.get(), -1, flags),
            delete_renderer);

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

// Copy an entire surface to a given region of the render context
void Renderer::copy(Surface &surface, const SDL_Rect &render_region) {
    auto texture = get_texture(surface);
    SDL_RenderCopy(context.get(), texture.get(), nullptr, &render_region);
}

// Copy a given region of a surface to a given region of the render context
void Renderer::copy(Surface &surface, const SDL_Rect &copy_region,
        const SDL_Rect &render_region) {

    auto texture = get_texture(surface);
    SDL_RenderCopy(context.get(), texture.get(), &copy_region, &render_region);
}

// Copy an entire surface to a given region of the render context, optionally
// mirroring or rotating it
void Renderer::copy(Surface &surface, const SDL_Rect &render_region,
        const bool &mirrored, const int &theta) {

    auto texture = get_texture(surface);
    auto flip_flag = mirrored ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(context.get(), texture.get(), nullptr, &render_region,
            theta, nullptr, flip_flag);
}

// Copy a given region of a surface to a given region of the render context,
// optionally mirroring or rotating it
void Renderer::copy(Surface &surface, const SDL_Rect &copy_region,
        const SDL_Rect &render_region, const bool &mirrored,
        const int &theta) {

    auto texture = get_texture(surface);
    auto flip_flag = mirrored ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(context.get(), texture.get(), &copy_region, &render_region,
            theta, nullptr, flip_flag);
}

// Clear the render context
void Renderer::clear() {
    SDL_RenderClear(context.get());
}

// Present the render context to the window
void Renderer::present() {
    SDL_RenderPresent(context.get());
}
