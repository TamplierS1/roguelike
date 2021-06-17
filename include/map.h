#ifndef MAP_H
#define MAP_H

#include <list>

#include "rng.h"
#include "monster.h"
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
    CellType type = CellType::Wall;
    Vec2 pos = {-1, -1};
    bool is_dynamic = false;
    s_ptr<Object> object;
    // Each region has a different color.
    // It helps when I start connecting regions.
    Color color;
    bool is_visible = false;
    bool is_explored = false;
};

struct Region
{
    std::vector<Cell*> cells;
    Color color;
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

    void update(const Camera& camera);
    void set_player(const s_ptr<Monster>& player);

    bool is_cell_available(Vec2 pos) const;

    Vec2 m_player_start_position;

private:
    void render(const Camera& camera);

    void update_fog_of_war();
    void cast_ray(Vec2 begin, Vec2 end);

    // Dungeon generation.

    void fill_map_with_walls();
    void dig_rooms();
    void dig_mazes();
    void connect_regions();
    void remove_dead_ends();
    // Pillars are well cells with 8 non-wall neighbours.
    void remove_pillars();

    void dig(Vec2 begin, Vec2 end, Color color);
    void dig(Vec2 pos, Color color);
    void fill(Vec2 pos);
    bool is_area_available(Vec2 begin, Vec2 end);
    constexpr int num_of_wall_neighbours(const Cell& cell) const;
    constexpr int num_of_floor_neighbours(const Cell& cell) const;
    bool are_cells_around_free(const Cell& cell) const;
    // Checks if floor cells around `cell` are in `m_live_cells`.
    bool is_cell_in_live_cells(const Cell& cell) const;
    bool is_adjacent_to_different_regions(const Cell& cell) const;
    bool is_adjacent_to_main_region(const Cell& cell, const Region& main_region);
    Region* new_region_cell_connects_to(const Region& main_region, const Cell& cell);
    bool is_dead_end(const Cell& cell);
    // The returned bool means success or failure.
    bool merge_regions_at_cell(Region& main_region, Cell& cell);

    w_ptr<Monster> m_player;

    // Width and height should always be odd.
    constexpr static int m_width = 71;
    constexpr static int m_height = 71;
    constexpr static int m_corridor_windiness = 250;
    constexpr static int m_room_density = 100;
    constexpr static int m_room_size_min = 5;
    constexpr static int m_room_size_max = 13;

    s_ptr<Renderer> m_renderer;
    s_ptr<Texture> m_rock_floor_texture;

    using CellMap = std::array<std::array<Cell, m_height>, m_width>;
    CellMap m_map;
    std::vector<Region> m_regions;
    std::vector<Room> m_rooms;

    Rng m_rng;
    // Probabilities of picking each neighbour when
    // generating a maze. The last picked neighbour has
    // the highest probability.
    std::vector<int> m_neighbour_probs = {25, 25, 25, 25};

    // Maze generation.
    std::list<s_ptr<Cell>> m_live_cells;
};
}

#endif  // MAP_H
