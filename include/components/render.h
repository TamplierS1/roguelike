#ifndef RENDER_H
#define RENDER_H

#include "component.h"
#include "sdl_wrappers/texture.h"
#include "common.h"

namespace Rg::Components
{
class Render : public IComponent
{
public:
    Render(const s_ptr<Texture>& texture, const s_ptr<Renderer>& renderer)
        : m_texture(texture)
        , m_renderer(renderer)
    {
    }

    void update(Object* object) override;

private:
    w_ptr<Texture> m_texture;
    w_ptr<Renderer> m_renderer;
};
}

#endif  // RENDER_H
