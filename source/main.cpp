#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include <SDL2/SDL.h>

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// #define DEBUG

// TODO: Split the includes into their own condensed headers

#include "hash.hpp"

#include "resources.hpp"

#include "font.hpp"

#include "graphics/surface.hpp"
#include "graphics/window.hpp"
#include "graphics/renderer.hpp"

#include "node.hpp"

#include "nodes/button.hpp"
#include "nodes/colour_rectangle.hpp"
#include "nodes/label.hpp"

#include "state.hpp"

#include "states/test_state.hpp"

#include "application.hpp"

int main(int argument_count, char *argument_vector[]) {
    try {
        Resources::start();

        Window window("Blackfriars", 768, 512);
        Renderer renderer(window);

        Application application(window, renderer);
        application.load_state<TestState>();
        application.run();
    }
    catch(...) {
        return -1;
    }

    Resources::stop();
    return 0;
}
