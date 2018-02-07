#include "solarus/graphics/Texture.h"
#include "solarus/graphics/RenderTexture.h"

#include <SFML/Graphics/Sprite.hpp>

Texture::Texture(const sf::Image &image)
    :image(image)
{
    texture.loadFromImage(image);
}

const sf::Texture& Texture::get_texture() const {
    return texture;
}

sf::Image& Texture::get_image() {
    return image;
}

RenderTexture* Texture::to_render_texture() {
    RenderTexture* rt = new RenderTexture(texture.getSize().x,texture.getSize().y);
    rt->draw(sf::Sprite(texture));
    return rt;
}
