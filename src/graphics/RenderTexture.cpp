#include "solarus/graphics/RenderTexture.h"
#include "solarus/graphics/Surface.h"

#include "solarus/core/Debug.h"

#include <SFML/Graphics/RectangleShape.hpp>

namespace Solarus {

RenderTargetAtlas RenderTexture::render_atlas;

RenderTexture::RenderTexture(int width, int height, bool depth_buffer)
{
    /*if(!target.create(width,height,depth_buffer)) {
        Solarus::Debug::error("Could not create render target");
    }*/
    target = render_atlas.create_view(width,height);
    //sprite.setTexture(target.getTexture());
    target->clear();
}

int RenderTexture::get_width() const {
    return target->get_width();
}

int RenderTexture::get_height() const {
    return target->get_height();
}

const sf::Texture &RenderTexture::get_texture() const {
    return target->get_texture();
}


void RenderTexture::draw_other(const SurfaceImpl& other, const Point& dst_position) {
    draw_region_other(Rectangle(0,0,other.get_width(),other.get_height()),other,dst_position);
}

void RenderTexture::draw_region_other(const Rectangle& region, const SurfaceImpl& other, const Point& dst_position) {
    other.draw_on(*this,region,dst_position);
}

void RenderTexture::draw_on(RenderTexture& render_texture, const Rectangle& region, const Point& dst_position) const {
    target->draw_on(render_texture,region,dst_position,parent().get_opacity());
}

void RenderTexture::draw(const sf::Drawable& drawable,const sf::RenderStates& states) {
    texture_dirty = true;
    target->draw(drawable,states);
}

void RenderTexture::draw(const sf::Vertex* vertices, size_t vertexCount,sf::PrimitiveType type,const sf::RenderStates& states) {
    texture_dirty = true;
    target->draw(vertices,vertexCount,type,states);
}

sf::Image& RenderTexture::get_image() {
    if (image_dirty) {
        image = get_texture().copyToImage(); //Slow but functionnal
        image_dirty = false;
    }
    return image;
}

RenderTexture* RenderTexture::to_render_texture() {
    return this;
}

void RenderTexture::clear() {
    target->clear();
}

void RenderTexture::clear(const Rectangle& where) {
   target->clear(where);
}

}
