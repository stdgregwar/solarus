#include "solarus/graphics/Texture.h"
#include "solarus/graphics/RenderTexture.h"
#include "solarus/core/Debug.h"
#include "solarus/graphics/Surface.h"

#include <SFML/Graphics/Sprite.hpp>

namespace Solarus {

Texture::Texture(const sf::Image &image)
    :image(image)
{
    texture.loadFromImage(image);
    sprite.setTexture(texture);
}

const sf::Texture& Texture::get_texture() const {
    return texture;
}

sf::Image& Texture::get_image() {
    return image;
}

int Texture::get_width() const {
    return image.getSize().x;
}

int Texture::get_height() const {
    return image.getSize().y;
}

void Texture::draw_on(RenderTexture& render_texture, const Rectangle& region, const Point& dst_position) const {
    sprite.setTextureRect(region);
    sprite.setPosition(dst_position);
    sprite.setColor(sf::Color(255,255,255,parent().get_opacity()));
    render_texture.draw(sprite);
}

void Texture::draw_region_other(const Rectangle& region, const SurfaceImpl &other, const Point& dst_position) {
    Debug::error("Trying to draw on a texture!");
}

RenderTexture* Texture::to_render_texture() {
    RenderTexture* rt = new RenderTexture(texture.getSize().x,texture.getSize().y);
    rt->draw(sprite);
    return rt;
}

void Texture::draw(const sf::Drawable&drawable, const sf::RenderStates& states) {
    Debug::error("trying to draw on a texture");
}

void Texture::draw(const sf::Vertex* vertices, size_t vertexCount,sf::PrimitiveType type,const sf::RenderStates& states) {
    Debug::error("trying to draw on a texture");
}

void Texture::clear() {
    Debug::error("Trying to clear a texture");
}

void Texture::clear(const Rectangle& where) {
    Debug::error("Trying to clear part of a texture");
}

}
