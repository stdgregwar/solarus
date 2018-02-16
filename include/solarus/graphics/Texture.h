#pragma once

#include "solarus/graphics/SurfaceImpl.h"

#include <SFML/Graphics/Sprite.hpp>

namespace Solarus {

class Texture : public SurfaceImpl
{
public:
    Texture(const sf::Image& image);
    const sf::Texture& get_texture() const override;
    sf::Image& get_image() override;

    int get_width() const override;
    int get_height() const override;

    void draw_on(RenderTexture& render_texture, const Rectangle& region, const Point& dst_position) const override;
    RenderTexture* to_render_texture() override;
private:
    sf::Image image;
    sf::Texture texture;
    mutable sf::Sprite sprite;
};

}
