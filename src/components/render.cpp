#include "components/render.h"
#include "sdl_wrappers/renderer.h"

namespace Rg::Components
{
void Render::update(Object* object)
{
    m_renderer.lock()->render(object, m_texture.lock().get());
}
}
