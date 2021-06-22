#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <unordered_map>
#include <cassert>
#include <string>

#include <raylib.h>

#include "common.h"

namespace Rg
{
class Renderer;

class TextureManager
{
public:
    TextureManager(const TextureManager&) = delete;
    TextureManager(TextureManager&&) = delete;

    void cleanup();
    void load_texture(const std::string& path, const std::string& name);
    [[nodiscard]] s_ptr<Texture2D> get_texture(const std::string& name)
    {
        assert(m_textures.find(name) != m_textures.end());
        return m_textures[name];
    }

    [[nodiscard]] static TextureManager& get()
    {
        static TextureManager instance;
        return instance;
    }

private:
    TextureManager() = default;

    std::unordered_map<std::string, s_ptr<Texture>> m_textures;
};
}

#endif  // TEXTURE_MANAGER_H
