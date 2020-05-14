
#include "graphics.hpp"
#include "state.hpp"

#include "schematic_editor.hpp"

// Start SDL, true-type font, and .png loading subsystems
static inline void start() {
    auto format_flags = IMG_INIT_PNG;
    if(SDL_Init(SDL_INIT_EVERYTHING) || TTF_Init() ||
           !(IMG_Init(format_flags) & format_flags)) {
        std::cerr << "Subsystem initialization issue. Error message: " <<
                std::endl << SDL_GetError() << std::endl;
        throw -1;
    }
}

// Quit subsystems
static inline void quit() {
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

int main(int argument_count, char *argument_vector[]) {
    try {

        // Start subsystems
        start();

        // Create a window and a rendering context
        Window window("Blackfriars", 768, 512);
        Renderer renderer(window);

        // Create and start a state
        auto state = std::shared_ptr<State>(new SchematicEditor());
        state->start(renderer);

        // Start the application loop
        double previous_ticks = 0;
        bool running = true;
        while(running) {

            // Poll input events
            SDL_Event event;
            while(SDL_PollEvent(&event)) {

                // Handle quit events
                if(event.type == SDL_QUIT)
                    running = false;

                // Delegate input events to the current state
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

            // Render the current state
            renderer.clear();
            state->render(renderer);
            renderer.present();

            // Cap the frame rate

            long current_ticks = SDL_GetTicks();
            long delta = current_ticks - previous_ticks;
            previous_ticks = current_ticks;
            if(delta < 33.3)
                SDL_Delay(33.3 - delta);
        }

        // Stop the current state
        state->stop();

        // Free subsystem memory
        quit();
        return 0;
    }
    catch(...) {
        return -1;
    }
}
