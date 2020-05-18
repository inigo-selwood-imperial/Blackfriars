#pragma once

class Window {

public:

    std::shared_ptr<SDL_Window> frame;

    SDL_Rect size;

    Window(const std::string &title, const unsigned int &width,
            const unsigned int &height);

    void hide();
    void show();

};

Window::Window(const std::string &title, const unsigned int &width,
        const unsigned int &height) {

#ifdef DEBUG
    std::cout << "Creating window" << std::endl;
#endif // DEBUG

    auto position = SDL_WINDOWPOS_CENTERED;
    auto flags = SDL_WINDOW_HIDDEN;
    frame = std::shared_ptr<SDL_Window>(SDL_CreateWindow(title.c_str(),
            position, position, width, height, flags),
            Resources::delete_window);

    if(frame == nullptr) {
        std::cout << SDL_GetError() << std::endl;
        throw -1;
    }

    size = {0, 0, width, height};
}

void Window::hide() {
    SDL_HideWindow(frame.get());
}

void Window::show() {
    SDL_ShowWindow(frame.get());
}
