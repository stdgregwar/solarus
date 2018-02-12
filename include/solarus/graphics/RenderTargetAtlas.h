#pragma once

#include "solarus/core/Rectangle.h"

#include "solarus/third_party/shelf-pack.hpp"
#include "solarus/graphics/SurfaceImpl.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>


#include <memory>

namespace Solarus {

using RenderTarget = sf::RenderTarget;
using Pack = mapbox::ShelfPack;
using Bin = mapbox::Bin;

class Atlas;

class RenderTargetView {
public:
    RenderTargetView(Atlas& atlas,
                     const Rectangle& rect,
                     Bin& bi);
    sf::RenderStates prepare_render_states(const sf::RenderStates& states);
    void draw(const sf::Drawable&drawable, const sf::RenderStates& states=sf::RenderStates::Default);
    void draw(const sf::Vertex* vertices, size_t vertexCount,sf::PrimitiveType type,const sf::RenderStates& states =sf::RenderStates::Default);
    int get_width() const;
    int get_height() const;
    const sf::Texture& get_texture() const;
    void draw_on(Solarus::SurfaceImpl &target, const Rectangle& region, const Point& dst_position) const;
    void clear();
    void clear(const Rectangle& where);
    ~RenderTargetView();
private:
    void clear_sub(const Rectangle& where);
    Rectangle rect;
    Bin& bin;
    Atlas& atlas;
    mutable bool tex_dirty;
    mutable sf::Sprite sprite;
};

using RenderTargetViewPtr = std::shared_ptr<RenderTargetView>;

struct Atlas {
    Pack pack;
    mutable sf::RenderTexture target;
    Atlas(unsigned width,unsigned height) : pack(width,height) {
        if(!target.create(width,height)) {
            //TODO fail gracefully
        }
    }
};

using AtlasPtr = std::unique_ptr<Atlas>;

class RenderTargetAtlas
{
public:
    RenderTargetAtlas();
    RenderTargetViewPtr create_view(int width, int height);
private:
    RenderTargetViewPtr create_view(Atlas&,Bin&) const;
    void expand();
    void repack();
    std::vector<AtlasPtr> mAtlases;
};

}
