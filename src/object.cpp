#include "object.h"
#include "sdl_wrappers/renderer.h"

namespace Rg
{
void Object::render(const Camera& camera, Color color)
{
    m_renderer.lock()->render(this, m_texture.lock().get(), camera, color);
}

}
