#pragma once

#include <SFML/Graphics/Texture.hpp>

class Texture;
class RenderTexture;

/**
 * @brief Interface for internal surface pixel representation and manipulation
 */
class SurfaceImpl
{
public:
    SurfaceImpl();
    virtual const sf::Texture& get_texture() const = 0;
    virtual sf::Image& get_image() = 0;
    //virtual  Texture* to_texture() = 0;
    virtual  RenderTexture* to_render_texture() = 0;
};

