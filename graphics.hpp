#pragma once

#include <memory>

#include <SDL2/SDL.h>

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// ************************************************************************ Font

struct Font {

    std::shared_ptr<TTF_Font> data;

    const std::string &name;

    const unsigned int &size;

    Font(const std::string &name, const unsigned int &size);

};

Font(const std::string &name, const unsigned int &size) : name(name),
        size(size) {

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

class Renderer;

class Surface {

private:

    friend class Renderer;

    std::shared_ptr<SDL_Surface> data;

public:

    Surface(const std::string &file_name);
    Surface(const SDL_Rect &size, const SDL_Colour &colour);
    Surface(const Font &font, const SDL_Colour &colour,
            const std::String &text);

};

Surface::Surface(const std::string &file_name) {

    // Create an absolute path for the font file name
    std::string path = SDL_GetBasePath();
    path += "..\\resources\\" + name;

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
    SDL_FillRect(data.get(), nullptr, SDL_MapRGBA(surface.get()->format,
            colour.r, colour.g, colour.b, colour.a));
}

Surface::Surface(const Font &font, const SDL_Colour &colour,
        const std::string &text) {

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

// ********************************************************************* Texture

struct Texture {

};

// ********************************************************************** Window

class Window {

    Window(const std::stirng &title, const unsigned int &width,
            const unsigned int &height);

    void set_icon(const Surface &surface);

};

Window::Window(const std::string &title, const unsigned int &width,
        const unsigned int &height) {

    auto position = SDL_WINDOWPOS_CENTERED;
    auto flags = SDL_WINDOW_HIDDEN;
    context = std::shared_ptr<SDL_Window>(SDL_CreateWindow(title.c_str(),
            position, position, width, height, flags), SDL_DestroyWindow);
}

void Window::set_icon(const Surface &surface) {
    SDL_SetWindowIcon(context.get(), surface.data.get());
}

// ******************************************************************** Renderer

class Renderer {

private:

    std::shared_ptr<SDL_Renderer> context;

public:

    Renderer(const Window &window);

    Texture create_texture(const Surface &surface);

    void draw_line(const SDL_Point &from_point, const SDL_Point &to_point,
            const SDL_Colour &colour);
    void draw_filled_rectangle(const SDL_Rect &region,
            const SDL_Colour &colour);

    void copy(const Texture &texture, const SDL_Rect &copy_region,
        const SDL_Rect &render_region);
    void copy(const Texture &texture, const SDL_Rect &copy_region,
            const SDL_Rect &render_region, const bool &mirrored,
            const int &theta);

    void set_clear_colour(const SDL_Colour &colour);

    void clear();
    void present();

};

Renderer::Renderer(const Window &window) {

    // Try to create renderer context
    auto flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    context = std::shared_ptr<SDL_Renderer>(
            SDL_CreateRenderer(window.data.get(), -1, flags),
            SDL_DestroyRenderer);

    // Check renderer was created properly
    if(renderer == nullptr) {
        std::cerr << "Couldn't create renderer" << std::endl <<
                "\t" << "SDL error:" << std::endl <<
                SDL_GetError() << std::endl;
        throw -1;
    }
}

Texture Renderer::create_texture(const Surface &surface) {
}

void Renderer::draw_line(const SDL_Point &from_point, const SDL_Point &to_point,
        const SDL_Colour &colour) {

}

void Renderer::draw_filled_rectangle(const SDL_Rect &region,
        const SDL_Colour &colour) {

}

void Renderer::copy(const Texture &texture, const SDL_Rect &copy_region,
    const SDL_Rect &render_region) {

}

void Renderer::copy(const Texture &texture, const SDL_Rect &copy_region,
        const SDL_Rect &render_region, const bool &mirrored,
        const int &theta) {

}

void Renderer::set_clear_colour(const SDL_Colour &colour) {

}

void Renderer::clear() {

}

void Renderer::present() {

}
