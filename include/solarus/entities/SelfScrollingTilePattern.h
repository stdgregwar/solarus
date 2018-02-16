/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SOLARUS_SELF_SCROLLING_TILE_PATTERN_H
#define SOLARUS_SELF_SCROLLING_TILE_PATTERN_H

#include "solarus/core/Common.h"
#include "solarus/entities/SimpleTilePattern.h"
#include "solarus/entities/ParallaxScrollingTilePattern.h"

namespace Solarus {

/**
 * \brief Tile pattern that scrolls on itself when the camera moves.
 *
 * The pattern scolls on itself from 1 pixel when the camera moves from 2
 * pixels. This gives an illusion of depth.
 * It achieves the same effect as ParallaxScrollingTilePattern, but the tile
 * remains displayed at its initial position.
 * However, this is limited to simple, repeatable patterns:
 * you cannot make big scrolling backgrounds composed of different tiles
 * (use ParallaxScrollingTilePatterns for this) because the tile actually
 * does not move with the camera, it just scrolls on ifself.
 */
class SelfScrollingTilePattern: public SimpleTilePattern {

  public:
    struct SelfScrollUpdater : public TilePattern::Updater {
        SelfScrollUpdater(const VerticeView& view, const SelfScrollingTilePattern& pattern,const Rectangle& base_position):
            quad(view), pattern(pattern), base_position(base_position) {

        }
        void update(const Point& viewport,const Rectangle& clip) override {
            Point offset = viewport / ParallaxScrollingTilePattern::ratio;
            int x_mod = offset.x % base_position.get_width();
            offset.x = x_mod < 0 ? x_mod + base_position.get_width() : x_mod;
            int y_mod = offset.y % base_position.get_height();
            offset.y = y_mod < 0 ? y_mod + base_position.get_height() : y_mod;
            //offset-=Point(base_position.get_width(),base_position.get_height());
            quad.set_4quad_offset(base_position,offset,pattern.position_in_tileset);
        }
        VerticeView quad;
        const SelfScrollingTilePattern& pattern;
        const Rectangle base_position;
    };

    SelfScrollingTilePattern(Ground ground, const Point& xy, const Size& size);

    virtual void draw(
        const SurfacePtr& dst_surface,
        const Point& dst_position,
        const Tileset& tileset,
        const Point& viewport
    ) const override;

    TilePattern::UpdaterPtr add_vertices(VertexArray& array,
                                       const Point& dst_position,
                                       const Tileset&,
                                        const Rectangle& clip
                                       ) const override; //TODO

    virtual bool is_animated() const override;

};

}

#endif

