#ifndef DRAWLIST_H
#define DRAWLIST_H

#include "solarus/graphics/SurfaceImpl.h"

#include <functional>
#include <vector>

namespace Solarus {

class DrawList : public SurfaceImpl
{
    using Draw = std::function<void(SurfaceImpl&,const Rectangle&,const Point&)>;
public:
    DrawList(int width, int height);
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
    int width,height;
    std::vector<Draw> draws;
};

}

#endif // DRAWLIST_H
