#include "solarus/graphics/RenderTargetAtlas.h"

#include "solarus/graphics/RenderTexture.h"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace Solarus {


constexpr unsigned atlas_size = 1024;
constexpr float inv_size = 1.f / atlas_size;

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

void RenderTargetView::setup_viewport() {
    sf::View v(sf::FloatRect(0,0,rect.get_width(),rect.get_height()));
    sf::FloatRect vp{
        rect.get_left() * inv_size,
        rect.get_top() * inv_size,
        rect.get_width() * inv_size,
        rect.get_height() * inv_size
    };
    v.setViewport(vp);
    atlas.back_target.setView(v);
}

void RenderTargetView::draw(const sf::Drawable& drawable,const sf::RenderStates& states) {
    setup_viewport();
    tex_dirty = true;
    atlas.back_target.draw(drawable,states);
}

void RenderTargetView::draw(const sf::Vertex* vertices, size_t vertexCount,sf::PrimitiveType type,const sf::RenderStates& states) {
    setup_viewport();
    tex_dirty = true;
    atlas.back_target.draw(vertices,vertexCount,type,states);
}

void RenderTargetView::draw_on(RenderTexture& target, const Rectangle& region, const Point& dst_position, uint8_t opacity) const {
    Rectangle true_region = region;
    true_region.add_xy(rect.get_xy());
    sprite.setTexture(get_texture());
    sprite.setTextureRect(true_region);
    sprite.setPosition(dst_position);
    sprite.setColor(sf::Color(255,255,255,opacity));
    target.draw(sprite);
}

const sf::Texture& RenderTargetView::get_texture() const {
    if(tex_dirty) {
        atlas.flush_to_front();
    } //TODO use a back and front buffer to allow self blit!
    return atlas.front_target.getTexture();
}

void RenderTargetView::clear(const sf::Color& clearColor) {
    clear_sub(rect,clearColor);
}

void RenderTargetView::clear_sub(const Rectangle& where,const sf::Color& clearColor) {
    sf::RectangleShape r(where.get_size());
    r.setFillColor(clearColor);
    r.setPosition(where.get_top_left());
    sf::RenderStates rs(sf::BlendNone);
    tex_dirty = true;
    atlas.back_target.setView(atlas.back_target.getDefaultView());
    atlas.back_target.draw(r,rs);
}

void RenderTargetView::clear(const Rectangle& where, const sf::Color &clearColor) {
    Rectangle corrected = where;
    corrected.add_xy(rect.get_xy());
    corrected = corrected & rect;
    clear_sub(corrected,clearColor);
}


RenderTargetView::~RenderTargetView() {
    clear(sf::Color::Red); //TODO remove
    atlas.pack.unref(bin);
}


RenderTargetAtlas::RenderTargetAtlas()
{
}

RenderTargetViewPtr RenderTargetAtlas::create_view(Atlas& atlas,Bin& b) const {
    Rectangle rect(b.x,b.y,b.w,b.h);
    RenderTargetView* rtv = new RenderTargetView(atlas,rect,b);
    rtv->clear(); //Make sur target is cleaned
    return RenderTargetViewPtr(rtv);
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
