#include "texture_manager.h"

namespace Rg
{
void TextureManager::load_texture(const std::string& path, const std::string& name,
                                  Renderer* renderer)
{
    if (m_textures.find(name) != m_textures.end())
    {
        Log::warning("Warning: texture `" + name + "` already exists.\n");
    }

    auto texture = std::make_shared<Texture>(path, renderer);
    m_textures.emplace(name, texture);
}
}
