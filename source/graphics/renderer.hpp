#pragma once

// TODO: Move elsewhere
std::ostream &operator<<(std::ostream &stream, const SDL_Rect &region) {
    return stream << "(" << region.x << ", " << region.y << ", " << region.w <<
            ", " << region.h << ")";
}

class Renderer {

private:

    std::map<Hash, std::shared_ptr<SDL_Texture>> textures;

    std::shared_ptr<SDL_Texture> get_texture(Surface &surface);

public:

    std::shared_ptr<SDL_Renderer> context;

    SDL_Colour clear_colour;

    SDL_Rect size;

    unsigned int scale;

    Renderer(Window &window, const unsigned int scale);

    void copy(Surface &surface, const SDL_Point &render_origin);

    void clear();
    void present();

    void draw_rectangle(const SDL_Rect &region, const SDL_Colour &colour);

    void set_colour(const SDL_Colour &colour);

};

std::shared_ptr<SDL_Texture> Renderer::get_texture(Surface &surface) {
    auto texture = textures[surface.hash];
    if(texture)
        return texture;

    texture = std::shared_ptr<SDL_Texture>(
        SDL_CreateTextureFromSurface(context.get(), surface.data.get()),
        Resources::delete_texture);

    if(texture == nullptr) {
        std::cerr << SDL_GetError() << std::endl;
        throw -1;
    }

    textures[surface.hash] = texture;
    return texture;
}

Renderer::Renderer(Window &window, const unsigned int scale = 1) {

#ifdef DEBUG
    std::cout << "Creating renderer" << std::endl;
#endif // DEBUG

    auto flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    context = std::shared_ptr<SDL_Renderer>(
            SDL_CreateRenderer(window.frame.get(), -1, flags),
            Resources::delete_renderer);

    if(context == nullptr) {
        std::cout << SDL_GetError() << std::endl;
        throw -1;
    }

    SDL_RenderSetScale(context.get(), scale, scale);

    window.show();

    clear_colour = {0, 0, 0, 0};

    size = window.size;
    size.w /= scale;
    size.h /= scale;

    this->scale = scale;
}

void Renderer::copy(Surface &surface, const SDL_Point &render_origin) {
    auto texture = get_texture(surface);
    auto flip_flags = surface.mirrored ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    SDL_Rect copy_region = {
        surface.copy_origin.x,
        surface.copy_origin.y,
        surface.size.w,
        surface.size.h
    };
    SDL_Rect render_region = {
        render_origin.x,
        render_origin.y,
        surface.size.w,
        surface.size.h
    };

    // std::cout << copy_region << ", " << render_region << std::endl;

    SDL_RenderCopyEx(context.get(), texture.get(), &copy_region,
            &render_region, surface.theta, nullptr, flip_flags);
}

void Renderer::clear() {
    set_colour(clear_colour);
    SDL_RenderClear(context.get());
}

void Renderer::present() {
    SDL_RenderPresent(context.get());
}

void Renderer::draw_rectangle(const SDL_Rect &region,
        const SDL_Colour &colour) {

    set_colour(colour);
    SDL_RenderFillRect(context.get(), &region);
    set_colour(clear_colour);
}

void Renderer::set_colour(const SDL_Colour &colour) {
    SDL_SetRenderDrawColor(context.get(), colour.r, colour.g, colour.b,
            colour.a);
}
