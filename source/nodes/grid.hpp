#pragma once

class Grid : public Node {

public:

    std::map<SDL_Point, std::shared_ptr<Node>> nodes;

    SDL_Rect size;

    SDL_Point render_origin;

    unsigned int columns;
    unsigned int rows;

    unsigned int spacing;

    Grid() {
        render_origin = {0, 0};
        size = {0, 0, 0, 0};

        columns = 0;
        rows = 0;

        spacing = 0;
    }

    void render(Renderer &renderer) override {
        SDL_Rect render_origin = {0, 0};
        for(unsigned int column = 0; column < columns; column += 1) {
            render_origin.x = render_origin.x + (column * (size.w + spacing));

            for(unsigned int row = 0; row < rows; row += 1) {
                render_origin.y = render_origin.y + (row * (size.h + spacing));

                auto node = nodes[{column, row}];
                if(node == nullptr)
                    continue;

                node->render_origin = render_origin;
                node->renderer(renderer);
            }
        }
    }

};
