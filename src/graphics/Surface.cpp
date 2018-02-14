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
#include "solarus/core/QuestFiles.h"
#include "solarus/core/Rectangle.h"
#include "solarus/core/Size.h"
#include "solarus/graphics/Color.h"
#include "solarus/graphics/SoftwarePixelFilter.h"
#include "solarus/graphics/Surface.h"
#include "solarus/graphics/Transition.h"
#include "solarus/graphics/Video.h"
#include "solarus/graphics/RenderTexture.h"
#include "solarus/graphics/Texture.h"
#include "solarus/lua/LuaContext.h"
#include <algorithm>
#include <iostream>
#include <sstream>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace Solarus {

/**
 * \brief Creates a surface with the specified size.
 * \param width The width in pixels.
 * \param height The height in pixels.
 */
Surface::Surface(int width, int height):
  Drawable(),
  opacity(255),
  internal_surface(nullptr)
{

  Debug::check_assertion(width > 0 && height > 0,
      "Attempt to create a surface with an empty size");

  internal_surface.reset(new RenderTexture(width,height));
  internal_surface->_parent = this;
}

Surface::Surface(const sf::Image& img)
    : opacity(255),
      internal_surface(new Texture(img))
{
    internal_surface->_parent = this;
}

/**
 * \brief Creates a surface form the specified SDL texture.
 *
 * This constructor must be used only by low-level classes that manipulate directly
 * SDL dependent surfaces.
 *
 * \param internal_surface The internal surface data.
 * The created surface takes ownership of this object.
 */
Surface::Surface(SurfaceImpl* impl):
  Drawable(),
  opacity(255),
  internal_surface(impl) //TODO refactor this...
{
    internal_surface->_parent = this;
}

/**
 * \brief Destructor.
 */
Surface::~Surface() {
    internal_surface.reset();
}


/**
 * \brief Creates a surface with the specified size.
 *
 * The surface is initially transparent.
 *
 * \param width The width in pixels.
 * \param height The height in pixels.
 * \return The created surface.
 */
SurfacePtr Surface::create(int width, int height) {
  SurfacePtr surface = std::make_shared<Surface>(width, height);
  return surface;
}

/**
 * \brief Creates a surface with the specified size.
 * \param size The size in pixels.
 * \return The created surface.
 */
SurfacePtr Surface::create(const Size& size) {
  SurfacePtr surface = std::make_shared<Surface>(size.width, size.height);
  return surface;
}

/**
 * \brief Creates a surface from the specified image file name.
 *
 * This function acts like a constructor excepts that it returns nullptr if the
 * file does not exist or is not a valid image.
 *
 * \param file_name Name of the image file to load, relative to the base directory specified.
 * \param base_directory The base directory to use.
 * \return The surface created, or nullptr if the file could not be loaded.
 */
SurfacePtr Surface::create(const std::string& file_name,
    ImageDirectory base_directory) {

  SurfaceImpl* surface = get_surface_from_file(file_name, base_directory);

  if (surface == nullptr) {
    return nullptr;
  }

  return std::make_shared<Surface>(surface);
}

/**
 * \brief Creates an SDL surface corresponding to the requested file.
 *
 * The returned SDL surface has to be manually deleted.
 *
 * \param file_name Name of the image file to load, relative to the base directory specified.
 * \param base_directory The base directory to use.
 * \return The SDL surface created.
 */
SurfaceImpl *Surface::get_surface_from_file(
    const std::string& file_name,
    ImageDirectory base_directory) {

  std::string prefix;
  bool language_specific = false;

  if (base_directory == DIR_SPRITES) {
    prefix = "sprites/";
  }
  else if (base_directory == DIR_LANGUAGE) {
    language_specific = true;
    prefix = "images/";
  }
  std::string prefixed_file_name = prefix + file_name;

  if (!QuestFiles::data_file_exists(prefixed_file_name, language_specific)) {
    // File not found.
    return nullptr;
  }

  const std::string& buffer = QuestFiles::data_file_read(prefixed_file_name, language_specific);
  sf::Image img;
  img.loadFromMemory(buffer.data(),buffer.size());
  return new Texture(img);
}

/**
 * \brief Returns the width of the surface.
 * \return the width in pixels
 */
int Surface::get_width() const {
  return internal_surface->get_width();
}

/**
 * \brief Returns the height of the surface.
 * \return the height in pixels
 */
int Surface::get_height() const {
  return internal_surface->get_height();
}

/**
 * \brief Returns the size of this surface.
 * \return the size of this surface
 */
Size Surface::get_size() const {
  return { get_width(), get_height() };
}

/**
 * \brief Returns the opacity of this surface.
 * \return The opacity (0 to 255).
 */
uint8_t Surface::get_opacity() const {
  return opacity;
}

/**
 * \brief Sets the opacity of this surface.
 * \param opacity The opacity (0 to 255).
 */
void Surface::set_opacity(uint8_t opacity) {
  this->opacity = opacity;
}

/**
 * \brief Returns the SDL surface wrapped.
 * \return The internal SDL surface.
 */
SurfaceImpl* Surface::get_internal_surface() {
  return internal_surface.get();
}

/**
 * \brief Returns a buffer of the raw pixels of this surface.
 *
 * Pixels returned have the RGBA 32-bit format.
 *
 * \return The pixel buffer.
 */
std::string Surface::get_pixels() const {
  const sf::Image& img = internal_surface->get_image();
  return std::string(reinterpret_cast<const char*>(img.getPixelsPtr()), get_width() * get_height() * 4);
}

/**
 * @brief Set pixels of this surface from a RGBA buffer
 * @param buffer a string considerer as array of bytes with pixels in RGBA
 */
void Surface::set_pixels(const std::string& buffer) {
    //TODO notify pixels changed
    internal_surface->get_image().create(get_width(),get_height(),reinterpret_cast<const uint8_t*>(buffer.data()));
}

RenderTexture &Surface::request_render() {
    SurfaceImpl* old = internal_surface.get();
    RenderTexture* rt = internal_surface->to_render_texture();
    if(old != rt) {
        internal_surface.reset(rt);
    }
    return *rt;
}

SurfacePtr Surface::seal_to_texture() const {
    return std::make_shared<Surface>(internal_surface->get_image());
}

/**
 * \brief Clears this surface.
 *
 * The surface becomes fully transparent and its size remains unchanged.
 * The opacity property of the surface is preserved.
 */
void Surface::clear() {
    internal_surface->clear();
}

/**
 * \brief Clears a rectangle of this surface.
 *
 * This is only supported for software surfaces.
 * The rectangle cleared becomes fully transparent.
 *
 * \param where The rectangle to clear.
 */
void Surface::clear(const Rectangle& where) { //TODO deprecate
    internal_surface->clear(where);
}

/**
 * \brief Fills the entire surface with the specified color.
 *
 * If the color has a non-opaque alpha component, then the color is
 * alpha-blended onto the surface.
 *
 * \param color A color.
 */
void Surface::fill_with_color(const Color& color) {
    fill_with_color(color,Rectangle(0,0,get_width(),get_height()));
}

/**
 * \brief Fills the rectangle at given coordinates with the specified color.
 *
 * If the color has a non-opaque alpha component, then the color is
 * alpha-blended onto the surface.
 *
 * \param color A color.
 * \param where The rectangle to fill.
 */
void Surface::fill_with_color(const Color& color, const Rectangle& where) {
    sf::RectangleShape rs(where.get_size());
    rs.setPosition(where.get_xy());
    rs.setFillColor(color);
    internal_surface->draw(rs);
}

/**
 * \brief Draws this surface on another surface.
 * \param dst_surface The destination surface.
 * \param dst_position Coordinates on the destination surface.
 */
void Surface::raw_draw(Surface& dst_surface, const Point& dst_position) {

  //Rectangle region(0, 0, get_width(), get_height());
  //raw_draw_region(region, dst_surface, dst_position);
  dst_surface.internal_surface->draw_other(*internal_surface,dst_position);
}

/**
 * \brief Draws a subrectangle of this surface on another surface.
 * \param region The subrectangle to draw in this object.
 * \param dst_surface The destination surface.
 * \param dst_position Coordinates on the destination surface.
 */
void Surface::raw_draw_region(
    const Rectangle& region,
    Surface& dst_surface,
    const Point& dst_position) {
    //TODO take opacity into acounnt
    dst_surface.internal_surface->draw_region_other(region,*internal_surface,dst_position);
}

/**
 * \brief Draws a transition effect on this drawable object.
 * \param transition The transition effect to apply.
 */
void Surface::draw_transition(Transition& transition) {
  transition.draw(*this);
}

/**
 * \brief Returns the surface where transitions on this drawable object
 * are applied.
 * \return The surface for transitions.
 */
Surface& Surface::get_transition_surface() {
  return *this;
}

/**
 * \brief Returns a pixel value of this surface.
 *
 * The pixel format is preserved: if it is lower than 32 bpp, then the unused
 * upper bits of the value are is padded with zeros.
 *
 * \param index Index of the pixel to get.
 * \return The value of this pixel.
 */
uint32_t Surface::get_pixel(int index) const {
    uint32_t* pixels = (uint32_t*)internal_surface->get_image().getPixelsPtr();
    return pixels[index];
}

/**
 * \brief Returns whether a pixel is transparent.
 *
 * A pixel is transparent if it corresponds to the colorkey
 * or if its alpha channel is equal to 0.
 *
 * \param index The index of the pixel to test.
 * \return \c true if the pixel is transparent.
 */
bool Surface::is_pixel_transparent(int index) const {
    unsigned x = index % get_width();
    unsigned y = index / get_width();
    return internal_surface->get_image().getPixel(x,y).a == 0;
}

/**
 * \brief Converts a color to a 32-bit value in the current video format.
 * \param color The color to convert.
 * \return The pixel value of this color in the current format.
 */
uint32_t Surface::get_color_value(const Color& color) const {
  uint8_t r, g, b, a;
  color.get_components(r, g, b, a);
  return sf::Color(r,g,b,a).toInteger();
}

/**
 * \brief Returns the blend mode as an SDL_BlendMode value.
 * \return The blend mode.
 */
sf::BlendMode Surface::get_sfml_blend_mode() const {

  switch (get_blend_mode()) {

  case BlendMode::NONE:
    return sf::BlendNone;

  case BlendMode::BLEND:
    return sf::BlendAlpha;

  case BlendMode::ADD:
    return sf::BlendAdd;

  case BlendMode::MULTIPLY:
    return sf::BlendMultiply;
  }

  return sf::BlendAlpha;
}

/**
 * \brief Renders this surface onto a hardware texture.
 */
void Surface::render(sf::RenderTarget& render_target) {
  render_target.draw(sf::Sprite(internal_surface->get_texture()));
  /*SDL_UpdateTexture(
      &render_target,
      nullptr,
      internal_surface->pixels,
      internal_surface->pitch
  );*/
}

/**
 * \brief Returns the name identifying this type in Lua.
 * \return The name identifying this type in Lua.
 */
const std::string& Surface::get_lua_type_name() const {
  return LuaContext::surface_module_name;
}

}

