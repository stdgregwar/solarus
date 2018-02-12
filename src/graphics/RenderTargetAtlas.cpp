#include "solarus/graphics/RenderTargetAtlas.h"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/OpenGL.hpp>

namespace Solarus {


RenderTargetView::RenderTargetView(Atlas &atlas,
                 const Rectangle& rect,
                 Bin& bin) :
    rect(rect),
    bin(bin),
    atlas(atlas),
    tex_dirty(true)
{

}

int RenderTargetView::get_width() const {
    return rect.get_width();
}
int RenderTargetView::get_height() const {
    return rect.get_height();
}

sf::RenderStates RenderTargetView::prepare_render_states(const sf::RenderStates& states) {
    sf::Transform t;
    t.translate(rect.get_xy());
    sf::RenderStates s = states;
    s.transform *= t;
    //glEnable(GL_SCISSOR_TEST); //TODO debug this
    glScissor(rect.get_left(),rect.get_bottom(),rect.get_width(),rect.get_height());
    return s;
}

void RenderTargetView::draw(const sf::Drawable& drawable,const sf::RenderStates& states) {
    auto s = prepare_render_states(states);
    tex_dirty = true;
    atlas.target.draw(drawable,s);
    glDisable(GL_SCISSOR_TEST);
}

void RenderTargetView::draw(const sf::Vertex* vertices, size_t vertexCount,sf::PrimitiveType type,const sf::RenderStates& states) {
    auto s = prepare_render_states(states);
    tex_dirty = true;
    atlas.target.draw(vertices,vertexCount,type,s);
    glDisable(GL_SCISSOR_TEST);
}

void RenderTargetView::draw_on(SurfaceImpl& target, const Rectangle& region, const Point& dst_position) const {
    Rectangle true_region = region;
    true_region.add_xy(rect.get_xy());
    sprite.setTexture(get_texture());
    sprite.setTextureRect(true_region);
    sprite.setPosition(dst_position);
    target.draw(sprite);
}

const sf::Texture& RenderTargetView::get_texture() const {
    if(tex_dirty) {
        atlas.target.display();
        tex_dirty = false;
    } //TODO use a back and front buffer to allow self blit!
    return atlas.target.getTexture();
}

void RenderTargetView::clear() {
    clear_sub(rect);
}

void RenderTargetView::clear_sub(const Rectangle& where) {
    sf::RectangleShape r(where.get_size());
    r.setFillColor(sf::Color::Transparent);
    r.setPosition(where.get_top_left());
    sf::RenderStates rs(sf::BlendNone);
    tex_dirty = true;
    atlas.target.draw(r,rs);
}

void RenderTargetView::clear(const Rectangle& where) {
    Rectangle corrected = where;
    corrected.add_xy(rect.get_xy());
    corrected = corrected & rect;
    clear_sub(corrected);
}


RenderTargetView::~RenderTargetView() {
    atlas.pack.unref(bin);
}

constexpr unsigned atlas_size = 320;

RenderTargetAtlas::RenderTargetAtlas()
{
}

RenderTargetViewPtr RenderTargetAtlas::create_view(Atlas& atlas,Bin& b) const {
    Rectangle rect(b.x,b.y,b.w,b.h);
    return std::make_shared<RenderTargetView>(atlas,rect,b);
}

RenderTargetViewPtr RenderTargetAtlas::create_view(int width, int height) {
    //Try to fit into the previous atlases in case there is free space
    for(const AtlasPtr& a : mAtlases) {
        Bin* b = a->pack.packOne(-1,width,height);
        if(b) {
            return create_view(*a,*b);
        }
    }
    //No space found
    expand();
    //Retry
    return create_view(width,height);
}

void RenderTargetAtlas::expand() {
    mAtlases.emplace_back(new Atlas(atlas_size,atlas_size));
}

void RenderTargetAtlas::repack() {
    //TODO
}

}
