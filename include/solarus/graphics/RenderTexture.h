#pragma once

#include "solarus/graphics/SurfaceImpl.h"
#include <SFML/Graphics/RenderTexture.hpp>

class RenderTexture : public SurfaceImpl
{
public:
    RenderTexture(int width,int height,bool depth_buffer = false);
    const sf::Texture& get_texture() const override;
    sf::Image& get_image() override;
    RenderTexture* to_render_texture() override;

    sf::RenderTexture& get_render_texture();

    //Forward draw arguments
    template<class ...Args>
    void draw(Args... args) {
        image_dirty = texture_dirty = true;
        target.draw(args...);
    }
private:
    bool image_dirty = true;
    mutable bool texture_dirty = true;
    sf::Image image;

    mutable sf::RenderTexture target;
};

