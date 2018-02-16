#include "solarus/graphics/SurfaceImpl.h"

namespace Solarus {

SurfaceImpl::SurfaceImpl()
{

}

/*void SurfaceImpl::draw_other(const SurfaceImpl& other, const Point& dst_position) {
    draw_region_other(Rectangle(0,0,other.get_width(),other.get_height()),other,dst_position);
}*/

const Surface& SurfaceImpl::parent() const {
    return *_parent;
}

SurfaceImpl::~SurfaceImpl() {

}

}
