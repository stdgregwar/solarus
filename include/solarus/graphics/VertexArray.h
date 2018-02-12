#pragma once

#include "solarus/core/Rectangle.h"
#include "solarus/graphics/Color.h"
#include "solarus/graphics/SurfacePtr.h"

#include <SFML/Graphics/VertexArray.hpp>

#include <memory>

namespace Solarus {

using Vertex = sf::Vertex; //Actually two triangles

enum PrimitiveType {
    Points = sf::Points,
    Lines = sf::Lines,
    LineStrip = sf::LineStrip,
    Triangles = sf::Triangles,
    TriangleStrip = sf::TriangleStrip,
    TriangleFan = sf::Triangles
    //Don't put quad as it is deprecated
};

class VertexArray;

using VertexArrayPtr = std::shared_ptr<VertexArray>; //TODO put this in a separate file

class VerticeView
{
public:
    VerticeView(VertexArray& array, off_t offset, size_t size):
        array(array),offset(offset),size(size){}

    //TODO add utility methods here
    void update_quad_uvs(const Rectangle& uvs);
    inline Vertex& operator [](size_t index);
    Vertex& at(size_t index);
    const Vertex& at(size_t index) const;
    inline const Vertex& operator [](size_t index) const;
    size_t get_size() const;
private:
    VertexArray& array;
    off_t offset;
    size_t size;
};

class VertexArray
{
public:
    VertexArray(PrimitiveType type = Triangles);
    VertexArray(PrimitiveType type,size_t vertex_count);
    static VertexArrayPtr create(PrimitiveType type = Triangles);
    static VertexArrayPtr create(PrimitiveType type,size_t vertex_count);
    void set_primitive_type(PrimitiveType type);
    PrimitiveType get_primitive_type() const;
    Vertex* data();
    const Vertex* data() const;
    void add_vertex(const Vertex& v);
    VerticeView add_quad(const Rectangle& rect, const Rectangle& uvs, const Color &color);
    size_t vertex_count() const;
    Vertex& operator [](size_t index);
    void draw(const SurfacePtr &dst_surface, const Point& dst_position, const SurfacePtr &texture) const;
    const Vertex& operator [](size_t index) const;
private:
    std::vector <Vertex> vertices;
    PrimitiveType type;
};



}
