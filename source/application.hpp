#pragma once

class Application {

public:

    Window &window;

    Renderer &renderer;

    std::shared_ptr<State> state;

    long previous_ticks;

    Application(Window &window, Renderer &renderer);

    template <typename DerivedState, typename ... Arguments>
    void load_state(Arguments &&... arguments);

    void run();

};

Application::Application(Window &window, Renderer &renderer) :
        window(window), renderer(renderer) {

    previous_ticks = 0;
}

template <typename DerivedState, typename ... Arguments>
void Application::load_state(Arguments &&... arguments) {
    if(std::is_base_of<State, DerivedState>::value == false) {
        std::cerr << "Can't load a class as a state if it isn't derived "
                "from the State class";
        throw -1;
    }

#ifdef DEBUG
    std::cout << "Creating state" << std::endl;
#endif // DEBUG

    state = std::shared_ptr<State>(new DerivedState(std::forward<Arguments>(
            arguments)...));

    if(state == nullptr) {
        std::cerr << "Couldn't create state" << std::endl;
        throw -1;
    }

    state->start();
}

void Application::run() {
    if(state == nullptr) {
        std::cerr << "No state loaded" << std::endl;
        throw -1;
    }

    bool running = true;
    while(running) {

        SDL_Event event;
        while(SDL_PollEvent(&event)) {

            if(event.type == SDL_QUIT)
                running = false;

            else if(event.type == SDL_KEYDOWN)
                state->key_pressed(event.key.keysym.sym);

            else if(event.type == SDL_KEYUP)
                state->key_released(event.key.keysym.sym);

            else if(event.type == SDL_MOUSEMOTION)
                state->mouse_moved(event.motion);

            else if(event.type == SDL_MOUSEBUTTONDOWN)
                state->mouse_pressed(event.button);

            else if(event.type == SDL_MOUSEBUTTONUP)
                state->mouse_released(event.button);

            else if(event.type == SDL_MOUSEWHEEL)
                state->wheel_moved(event.wheel);
        }

        state->update(SDL_GetTicks());

        renderer.clear();
        state->render(renderer);
        renderer.present();

        long current_ticks = SDL_GetTicks();
        long delta = current_ticks - previous_ticks;
        previous_ticks = current_ticks;
        if(delta < 33.3)
            SDL_Delay(33.3 - delta);
    }

    if(state)
        state->stop();
}
