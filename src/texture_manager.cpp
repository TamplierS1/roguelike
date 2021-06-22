#include "log.h"
#include "texture_manager.h"

namespace Rg
{
void TextureManager::cleanup()
{
    for (auto& texture : m_textures)
    {
        UnloadTexture(*texture.second);
    }
}

void TextureManager::load_texture(const std::string& path, const std::string& name)
{
    if (m_textures.find(name) != m_textures.end())
    {
        Log::warning("Warning: texture `" + name + "` already exists.\n");
    }

    m_textures.emplace(name, std::make_shared<Texture2D>(LoadTexture(path.c_str())));
}
}
