#include "solarus/graphics/RenderTexture.h"

#include "solarus/core/Debug.h"

RenderTexture::RenderTexture(int width, int height, bool depth_buffer)
{
    if(!target.create(width,height,depth_buffer)) {
        Solarus::Debug::error("Could not create render target");
    }
    target.clear(sf::Color::Transparent);
}

const sf::Texture &RenderTexture::get_texture() const {
    if (texture_dirty) {
        target.display();
        texture_dirty = false;
    }
    return target.getTexture();
}

sf::Image& RenderTexture::get_image() {
    if (image_dirty) {
        image = get_texture().copyToImage(); //Slow but functionnal
        image_dirty = false;
    }
    return image;
}

sf::RenderTexture& RenderTexture::get_render_texture() {
    return target;
}


RenderTexture* RenderTexture::to_render_texture() {
    return this;
}
