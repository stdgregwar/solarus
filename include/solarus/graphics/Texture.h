#ifndef TEXTURE_H
#define TEXTURE_H

#include "solarus/graphics/SurfaceImpl.h"

class Texture : public SurfaceImpl
{
public:
    Texture(const sf::Image& image);
    const sf::Texture& get_texture() const override;
    sf::Image& get_image() override;
    RenderTexture* to_render_texture() override;
private:
    sf::Image image;
    sf::Texture texture;
};

#endif // TEXTURE_H
