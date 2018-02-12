#include "solarus/graphics/VertexArray.h"
#include "solarus/graphics/Surface.h"

namespace Solarus {

Vertex& VerticeView::operator [](size_t index) {
    return array[offset+index];
}

const Vertex& VerticeView::operator [](size_t index) const {
    return array[offset+index];
}

Vertex& VerticeView::at(size_t index) {
    return (*this)[index];
}

const Vertex& VerticeView::at(size_t index) const {
    return (*this)[index];
}

void VerticeView::update_quad_uvs(const Rectangle& uvs) {
    sf::Vector2f u1 = uvs.get_top_left();
    sf::Vector2f u2 = uvs.get_bottom_left();
    sf::Vector2f u3 = uvs.get_bottom_right();
    sf::Vector2f u4 = uvs.get_top_right();
    at(0).texCoords = u1;
    at(1).texCoords = u2;
    at(2).texCoords = u3;
    at(3).texCoords = u3;
    at(4).texCoords = u4;
    at(5).texCoords = u1;
}

size_t VerticeView::get_size() const {return size;}


VertexArray::VertexArray(PrimitiveType type):
    type(type)
{

}

VertexArray::VertexArray(PrimitiveType type,size_t vertex_count):
    vertices(vertex_count),type(type)
{
}

VertexArrayPtr VertexArray::create(PrimitiveType type) {
    return std::make_shared<VertexArray>(type);
}

VertexArrayPtr VertexArray::create(PrimitiveType type,size_t vertex_count) {
    return std::make_shared<VertexArray>(type,vertex_count);
}

void VertexArray::set_primitive_type(PrimitiveType type) {
    this->type = type;
}

PrimitiveType VertexArray::get_primitive_type() const {
    return type;
}

Vertex* VertexArray::data() {
    return vertices.data();
}

VerticeView VertexArray::add_quad(const Rectangle& rect, const Rectangle& uvs, const Color& color) {
    Vertex v1(rect.get_top_left(),
              color,
              uvs.get_top_left());
    Vertex v2(rect.get_bottom_left(),
              color,
              uvs.get_bottom_left());
    Vertex v3(rect.get_bottom_right(),
              color,
              uvs.get_bottom_right());
    Vertex v4(rect.get_top_right(),
              color,
              uvs.get_top_right());
    add_vertex(v1);
    add_vertex(v2);
    add_vertex(v3);
    add_vertex(v3);
    add_vertex(v4);
    add_vertex(v1);
    return VerticeView(*this,vertices.size()-6,6);
}
const Vertex* VertexArray::data() const {
    return vertices.data();
}

void VertexArray::add_vertex(const Vertex& v) {
    vertices.push_back(v);
}

size_t VertexArray::vertex_count() const {
    return vertices.size();
}

Vertex& VertexArray::operator [](size_t index) {
    return vertices.at(index);
}

void VertexArray::draw(const SurfacePtr& dst_surface,const Point& dst_position, const SurfacePtr& texture) const {
    sf::Transform t;
    t.translate(dst_position);
    sf::RenderStates rs(t);
    rs.texture = &texture->internal_surface->get_texture();
    dst_surface->internal_surface->draw(static_cast<const sf::Vertex*>(data()),vertex_count(),(sf::PrimitiveType)type,rs); //TODO add method to dst_surface
}

const Vertex& VertexArray::operator [](size_t index) const {
    return vertices.at(index);
}

}
