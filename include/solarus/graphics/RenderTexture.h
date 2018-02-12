#pragma once

#include "solarus/graphics/SurfaceImpl.h"
#include "solarus/graphics/RenderTargetAtlas.h"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace Solarus {

class RenderTexture : public SurfaceImpl
{
public:
    RenderTexture(int width,int height,bool depth_buffer = false);
    const sf::Texture& get_texture() const override;
    sf::Image& get_image() override;

    int get_width() const override;
    int get_height() const override;

    void draw_region_other(const Rectangle& region, const SurfaceImpl &other, const Point& dst_position) override;

    void draw_on(RenderTexture& render_texture, const Rectangle& region, const Point& dst_position) const override;
    void draw_on(DrawList& list, const Rectangle& region, const Point& dst_position) const override;

    RenderTexture* to_render_texture() override;
    void draw(const sf::Drawable&drawable, const sf::RenderStates& states=sf::RenderStates::Default) override;
    void draw(const sf::Vertex* vertices, size_t vertexCount,sf::PrimitiveType type,const sf::RenderStates& states =sf::RenderStates::Default) override;

    void clear() override;
    void clear(const Rectangle& where) override;
private:
    static RenderTargetAtlas render_atlas;
    bool image_dirty = true;
    mutable bool texture_dirty = true;
    sf::Image image;
    //mutable sf::Sprite sprite;
    //mutable sf::RenderTexture target;
    RenderTargetViewPtr target;
};

}
