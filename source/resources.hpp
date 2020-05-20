#pragma once

namespace Resources {

static std::map<Hash, std::shared_ptr<SDL_Surface>> images;
static std::map<Hash, std::shared_ptr<TTF_Font>> fonts;

std::shared_ptr<SDL_Surface> load_image(const std::string &name) {
    auto hash = std::hash<std::string>{}(name);

    if(images[hash])
        return images[hash];

    else {
        std::string path = SDL_GetBasePath();
        path += "..\\resources\\" + name + ".png";

#ifdef DEBUG
        std::cout << "Loading image: " << path << std::endl;
#endif // DEBUG

        auto image = std::shared_ptr<SDL_Surface>(IMG_Load(path.c_str()));

        if(image == nullptr) {
            std::cerr << IMG_GetError() << std::endl;
            throw -1;
        }

        images[hash] = image;
        return image;
    }
}

std::shared_ptr<TTF_Font> load_font(const std::string &name,
        const unsigned int &size) {

    auto name_hash = std::hash<std::string>{}(name);
    auto size_hash = std::hash<unsigned int>{}(size);
    auto hash = combine(name_hash, size_hash);

    if(fonts[hash])
        return fonts[hash];

    else {
        std::string path = SDL_GetBasePath();
        path += "..\\resources\\" + name + ".ttf";

#ifdef DEBUG
        std::cout << "Loading font: " << path << std::endl;
#endif // DEBUG

        auto font = std::shared_ptr<TTF_Font>(TTF_OpenFont(path.c_str(), size),
                TTF_CloseFont);

        if(font == nullptr) {
            std::cerr << TTF_GetError() << std::endl;
            throw -1;
        }

        fonts[hash] = font;
        return font;
    }
}

void start() {

#ifdef DEBUG
    std::cout << "Starting subsystems" << std::endl;
#endif // DEBUG

    auto image_format_flags = IMG_INIT_PNG;
    if(SDL_Init(SDL_INIT_EVERYTHING) || TTF_Init() ||
           !(IMG_Init(image_format_flags) & image_format_flags)) {
        std::cerr << SDL_GetError() << std::endl;
        throw -1;
    }
}

void stop() {

#ifdef DEBUG
    std::cout << "Stopping subsystems" << std::endl;
#endif // DEBUG

    images.clear();
    fonts.clear();

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void delete_window(SDL_Window *window) {
    if(window == nullptr)
        return;

#ifdef DEBUG
    std::cout << "Destroying window" << std::endl;
#endif // DEBUG

    SDL_DestroyWindow(window);
    window = nullptr;
}

void delete_renderer(SDL_Renderer *renderer) {
    if(renderer == nullptr)
        return;

#ifdef DEBUG
    std::cout << "Destroying renderer" << std::endl;
#endif // DEBUG

    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
}

void delete_surface(SDL_Surface *surface) {
    if(surface == nullptr)
        return;

#ifdef DEBUG
    std::cout << "Destroying surface" << std::endl;
#endif // DEBUG

    SDL_FreeSurface(surface);
    surface = nullptr;
}

void delete_texture(SDL_Texture *texture) {
    if(texture == nullptr)
        return;

#ifdef DEBUG
    std::cout << "Destroying texture" << std::endl;
#endif // DEBUG

    SDL_DestroyTexture(texture);
    texture = nullptr;
}

}; // Namespace Resources
