#include "object.h"
#include "sdl_wrappers/renderer.h"

namespace Rg
{
void Object::render(const Camera& camera)
{
    m_renderer.lock()->render(this, m_texture.lock().get(), camera);
}

}
