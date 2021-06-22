#include "object.h"

namespace Rg
{
void Object::render(Color color)
{
    DrawTexture(*m_texture, m_pos.x * g_tile_size, m_pos.y * g_tile_size, color);
}

}
