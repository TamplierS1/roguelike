#ifndef MAP_H
#define MAP_H

#include <random>

#include "object.h"
#include "vec2.h"

namespace Rg
{
enum class CellType
{
    Wall,
    Floor
};

struct Cell
{
    CellType m_type = CellType::Wall;
    Vec2 m_pos = {-1, -1};
    bool is_dynamic = false;
    s_ptr<Object> m_object;
};

struct Room
{
    Vec2 begin;
    Vec2 end;
};

class Map
{
public:
    explicit Map(const s_ptr<Renderer>& renderer);

    void render(const Camera& camera);

private:
    void fill_map_with_walls();
    void dig_rooms();
    void dig_mazes();

    void dig(Vec2 begin, Vec2 end);
    void dig(Vec2 pos);
    bool is_area_available(Vec2 begin, Vec2 end);
    [[nodiscard]] constexpr int num_of_solid_neighbours(const Cell* cell) const;

    constexpr static int m_width = 70;
    constexpr static int m_height = 70;

    using CellMap = std::array<std::array<Cell, m_height>, m_width>;
    CellMap m_map;
    std::vector<Room> m_rooms;

    s_ptr<Renderer> m_renderer;
    s_ptr<Texture> m_rock_floor_texture;

    using RandomInt = std::uniform_int_distribution<>;
    std::random_device m_rd;
    std::mt19937 m_engine;

    constexpr static int m_attempts_to_gen_rooms = 300;
    constexpr static int m_room_size_min = 5;
    constexpr static int m_room_size_max = 13;

    // Maze generation.
    std::vector<s_ptr<Cell>> m_live_cells;
};
}

#endif  // MAP_H
