#ifndef MAP_H
#define MAP_H

#include <list>

#include "rng.h"
#include "object.h"
#include "vec2.h"
#include "color.h"

namespace Rg::Map
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
    // Each region has a different color.
    // It helps when I start connecting regions.
    Color m_color;
};

struct Region
{
    std::vector<Cell*> m_cells;
    Color m_color;
    bool is_room;
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
    void connect_regions();
    void remove_dead_ends();

    void dig(Vec2 begin, Vec2 end, Color color);
    void dig(Vec2 pos, Color color);
    void fill(Vec2 pos);
    bool is_area_available(Vec2 begin, Vec2 end);
    [[nodiscard]] constexpr int num_of_solid_neighbours(const Cell& cell) const;
    bool are_cells_around_free(const Cell& cell) const;
    // Checks if floor cells around `cell` are in `m_live_cells`.
    bool is_cell_in_live_cells(const Cell& cell) const;
    bool is_adjacent_to_different_regions(const Cell& cell) const;
    bool is_adjacent_to_main_region(const Cell& cell, const Region& main_region);
    Region* new_region_cell_connects_to(const Region& main_region, const Cell& cell);
    bool is_dead_end(const Cell& cell);
    // The returned bool means success or failure.
    bool merge_regions_at_cell(Region& main_region, Cell& cell);

    // Width and height should always be odd.
    constexpr static int m_width = 71;
    constexpr static int m_height = 71;
    constexpr static int m_corridor_windiness = 150;
    constexpr static int m_room_density = 100;
    constexpr static int m_room_size_min = 5;
    constexpr static int m_room_size_max = 13;

    using CellMap = std::array<std::array<Cell, m_height>, m_width>;
    CellMap m_map;
    std::vector<Region> m_regions;
    std::vector<Room> m_rooms;

    Rng m_rng;
    // Probabilities of picking each neighbour when
    // generating a maze. The last picked neighbour has
    // the highest probability.
    std::vector<int> m_neighbour_probs = {25, 25, 25, 25};

    s_ptr<Renderer> m_renderer;
    s_ptr<Texture> m_rock_floor_texture;

    // Maze generation.
    std::list<s_ptr<Cell>> m_live_cells;
};
}

#endif  // MAP_H
