#pragma once

// ************************************************************************ Hash

/*
Uses of hashes in the codebase:
- Providing unique IDs for resources like fonts and surfaces, which prevents
    them from being loaded more than once

Some template specializations are provided to aid in hashing SDL specific types
like SDL_Rect and SDL_Colour
*/

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
