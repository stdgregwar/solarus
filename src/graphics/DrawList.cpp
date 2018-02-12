#include "solarus/graphics/DrawList.h"

#include <SFML/Graphics/Drawable.hpp>


namespace Solarus {

DrawList::DrawList(int width, int height):
    width(width),height(height)
{
}

const sf::Texture& DrawList::get_texture() const {

}
sf::Image& DrawList::get_image() {

}

int DrawList::get_width() const {

}
int DrawList::get_height() const {

}

void DrawList::draw_region_other(const Rectangle& region, const SurfaceImpl &other, const Point& dst_position) {
    other.draw_on(*this,region,dst_position);
}

void DrawList::draw_on(RenderTexture& render_texture, const Rectangle& region, const Point& dst_position) const {
    for(const Draw& draw: draws) {
        draw((SurfaceImpl&)render_texture,region,dst_position);
    }
}

void DrawList::draw_on(DrawList& list, const Rectangle& region, const Point& dst_position) const {
    for(const Draw& draw: draws) {
        draw(list,region,dst_position);
    }
}

RenderTexture* DrawList::to_render_texture() {

}

sf::RenderStates prepare_draw(const sf::RenderStates& states, const Rectangle& region, const Point& dst_position) {
    sf::RenderStates s = states;
    sf::Transform t;
    t.translate(dst_position);
    s.transform *= t;
    return s;
}

void DrawList::draw(const sf::Drawable&drawable, const sf::RenderStates& states) {
    draws.push_back(
    [&drawable,states](SurfaceImpl& srf, const Rectangle& region, const Point& dst_position){
        //TODO use scissors to cut out region
        sf::RenderStates s = prepare_draw(states,region,dst_position);
        srf.draw(drawable,s);
    });
}

void DrawList::draw(const sf::Vertex* vertices, size_t vertexCount,sf::PrimitiveType type,const sf::RenderStates& states) {
    draws.push_back(
    [=](SurfaceImpl& srf, const Rectangle& region, const Point& dst_position){
        sf::RenderStates s = prepare_draw(states,region,dst_position);
        srf.draw(vertices,vertexCount,type,s);
    });
}

void DrawList::clear() {
    draws.clear();
}

void DrawList::clear(const Rectangle& where) {
    draws.clear();
}

}
