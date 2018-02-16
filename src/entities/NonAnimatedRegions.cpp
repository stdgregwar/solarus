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
#include "solarus/core/Debug.h"
#include "solarus/core/Map.h"
#include "solarus/entities/Entities.h"
#include "solarus/entities/NonAnimatedRegions.h"
#include "solarus/entities/Tileset.h"
#include "solarus/graphics/Surface.h"
#include "solarus/entities/ParallaxScrollingTilePattern.h"

namespace Solarus {

constexpr Size cell_size(512,256);

/**
 * \brief Constructor.
 * \param map The map. Its size must be known.
 * \param layer The layer to represent.
 */
NonAnimatedRegions::NonAnimatedRegions(Map& map, int layer):
  map(map),
  layer(layer),
  static_tiles(map.get_size(), cell_size)
{

}

/**
 * \brief Adds a tile to the list of tiles.
 */
void NonAnimatedRegions::add_tile(const TileInfo& tile) {

  Debug::check_assertion(optimized_map_cells.empty(),
                         "Tile regions are already built");
  Debug::check_assertion(tile.layer == layer, "Wrong layer for add tile");

  tiles.push_back(tile);
}

/**
 * \brief Determines which rectangles are animated to allow drawing all non-animated
 * rectangles of tiles only once.
 * \param[out] rejected_tiles The list of tiles that are in animated regions.
 * They include all animated tiles plus the static tiles overlapping them.
 * You will have to redraw these tiles at each frame.
 */
void NonAnimatedRegions::build(std::vector<TileInfo>& rejected_tiles) {

  Debug::check_assertion(optimized_map_cells.empty(),
                         "Tile regions are already built");

  optimized_map_cells.resize(static_tiles.get_num_cells());

  // Build the list of animated tiles and tiles overlapping them. //TODO wipe this altogether
  for (const TileInfo& tile: tiles) {
    if(tile.pattern->is_drawn_at_its_position()) {
      static_tiles.add(tile,tile.box);
    } else {
      moving_tiles.push_back(tile);
    }
  }
  tiles.clear();
}

/**
 * \brief Clears previous drawings because the tileset has changed.
 */
void NonAnimatedRegions::notify_tileset_changed() {

  for (unsigned i = 0; i < static_tiles.get_num_cells(); ++i) {
    optimized_map_cells[i].v_array = nullptr;
    optimized_map_cells[i].updater = nullptr;
  }
  moving_cell.v_array = nullptr;
  moving_cell.updater = nullptr;
  // Everything will be redrawn when necessary.
}

/**
 * \brief Draws a layer of non-animated regions of tiles on the current map.
 */
void NonAnimatedRegions::draw_on_map() {

  const CameraPtr& camera = map.get_camera();
  if (camera == nullptr) {
    return;
  }

  // Check all grid cells that overlap the camera.
  const int num_rows = static_tiles.get_num_rows();
  const int num_columns = static_tiles.get_num_columns();
  const Size& cell_size = static_tiles.get_cell_size();
  const Rectangle& camera_position = camera->get_bounding_box();

  const int row1 = camera_position.get_y() / cell_size.height;
  const int row2 = (camera_position.get_y() + camera_position.get_height()) / cell_size.height;
  const int column1 = camera_position.get_x() / cell_size.width;
  const int column2 = (camera_position.get_x() + camera_position.get_width()) / cell_size.width;

  if (row1 > row2 || column1 > column2) {
    // No cell.
    return;
  }

  //Draw all moving tiles before the static ones
  if(moving_cell.v_array) {
    moving_cell.updater->update(-camera_position.get_xy(),Rectangle());
    moving_cell.v_array->draw(
          map.get_camera_surface(),-camera_position.get_xy() / ParallaxScrollingTilePattern::ratio,map.get_tileset().get_tiles_image()
        );
  } else {
    build_moving_cell();
  }

  for (int i = row1; i <= row2; ++i) {
    if (i < 0 || i >= num_rows) {
      continue;
    }

    for (int j = column1; j <= column2; ++j) {
      if (j < 0 || j >= num_columns) {
        continue;
      }

      // Make sure this cell is built.
      int cell_index = i * num_columns + j;
      if (optimized_map_cells[cell_index].v_array == nullptr) {
        // Lazily build the cell.
        build_cell(cell_index);
      }

      const Point cell_xy = {
        j * cell_size.width,
        i * cell_size.height
      };

      const Point dst_position = cell_xy - camera_position.get_xy();
      //Update cell content
      if(optimized_map_cells[cell_index].updater)
        optimized_map_cells[cell_index].updater->update(camera_position.get_xy(),Rectangle(Point(0,0),cell_size));

      //Draw vertex array corresponding to cell
      optimized_map_cells[cell_index].v_array->draw(
            map.get_camera_surface(), dst_position, map.get_tileset().get_tiles_image()
            );
    }
  }
}

/**
 * \brief Draws all non-animated tiles of a cell on its surface.
 * \param cell_index Index of the cell to draw.
 */
void NonAnimatedRegions::build_cell(int cell_index) {
  Debug::check_assertion(
        cell_index >= 0 && (size_t) cell_index < static_tiles.get_num_cells(),
        "Wrong cell index"
        );
  Debug::check_assertion(optimized_map_cells[cell_index].v_array == nullptr,
                         "This cell is already built"
                         );

  const int row = cell_index / static_tiles.get_num_columns();
  const int column = cell_index % static_tiles.get_num_columns();

  // Position of this cell on the map.
  const Size cell_size = static_tiles.get_cell_size();
  const Point cell_xy = {
    column * cell_size.width,
    row * cell_size.height
  };

  //Create vertex array for this particular cell
  VertexArrayPtr cell_array = VertexArray::create(Triangles);
  optimized_map_cells[cell_index].v_array = cell_array;

  const std::vector<TileInfo>& tiles_in_cell =
      static_tiles.get_elements(cell_index);

  std::vector<TilePattern::UpdaterPtr> updaters;

  for (const TileInfo& tile: tiles_in_cell) {
    Rectangle dst_position(
          tile.box.get_x() - cell_xy.x,
          tile.box.get_y() - cell_xy.y,
          tile.box.get_width(),
          tile.box.get_height()
          );

    auto u = tile.pattern->fill_vertex_array(
          *cell_array,
          dst_position,
          map.get_tileset(),
          cell_size
          );
    if(u) updaters.emplace_back(std::move(u));
  }

  if(!updaters.empty()) {
    optimized_map_cells[cell_index].updater.reset(new TilePattern::MultiUpdater(std::move(updaters)));
  }
}

void NonAnimatedRegions::build_moving_cell() {
  VertexArrayPtr cell_array = VertexArray::create(Triangles);
  moving_cell.v_array = cell_array;
  std::vector<TilePattern::UpdaterPtr> updaters;
  for (const TileInfo& tile: moving_tiles) {
    Rectangle dst_position(
          tile.box.get_x(),
          tile.box.get_y(),
          tile.box.get_width(),
          tile.box.get_height()
          );

    auto u = tile.pattern->fill_vertex_array(
          *cell_array,
          dst_position,
          map.get_tileset(),
          map.get_size()
          );
    if(u) updaters.emplace_back(std::move(u));
  }
  moving_cell.updater.reset(new TilePattern::MultiUpdater(std::move(updaters)));
}

}
