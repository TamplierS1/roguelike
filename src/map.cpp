#include <iostream>
#include <cmath>

#include "map.h"
#include "log.h"
#include "texture_manager.h"

namespace Rg::Map
{
Map::Map()
{
    fill_map_with_walls();
    dig_rooms();
    dig_mazes();
    connect_regions();
    remove_dead_ends();
    remove_pillars();

    Room starting_room = m_rooms[m_rng.rand_int(0, m_rooms.size() - 1)];
    m_player_start_position =
        starting_room.begin + (starting_room.end - starting_room.begin) / 2;
}

void Map::update()
{
    update_fog_of_war();
    render();
}

void Map::set_player(const s_ptr<Monster>& player)
{
    m_player = player;
}

bool Map::is_cell_available(Vec2 pos) const
{
    return m_map[pos.x][pos.y].type == CellType::Floor;
}

void Map::render()
{
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = 0; y < m_height; ++y)
        {
            auto object = m_map[x][y].object;

            Color tile_color = BLACK;
            if (m_map[x][y].is_explored && !m_map[x][y].is_visible)
                tile_color = m_explored_tile_color;
            else if (m_map[x][y].is_visible)
                tile_color = m_visible_tile_color;

            object->update(tile_color);

            if (!m_map[x][y].is_dynamic)
                continue;

            auto action = std::dynamic_pointer_cast<Monster>(object)->get_action();
            if (action != nullptr)
            {
                try
                {
                    action->execute(object.get());
                }
                catch (const Actions::ActionFailed& e)
                {
                    Log::warning(e.m_msg);
                }
            }
        }
    }
}

void Map::update_fog_of_war()
{
    Vec2 pos = m_player->m_pos;
    int radius = m_player->m_vision_radius;

    // Make every cell in the visible radius invisible.
    for (int x = pos.x - radius - 1; x <= pos.x + radius + 1; ++x)
    {
        for (int y = pos.y - radius - 1; y <= pos.y + radius + 1; ++y)
        {
            m_map[x][y].is_visible = false;
        }
    }

    // Cast rays to the upper and bottom part of visible radius.
    for (int x = pos.x - radius; x <= pos.x + radius; ++x)
    {
        cast_ray(pos, Vec2{x, pos.y - radius});
        cast_ray(pos, Vec2{x, pos.y + radius});
    }

    // Right and left part.
    for (int y = pos.y - radius; y <= pos.y + radius; ++y)
    {
        cast_ray(pos, Vec2{pos.x + radius, y});
        cast_ray(pos, Vec2{pos.x - radius, y});
    }
}

void Map::cast_ray(Vec2 begin, Vec2 end)
{
    int x0 = begin.x, y0 = begin.y;
    int x1 = end.x, y1 = end.y;

    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    bool end_ray = false;
    for (;;)
    {
        if (end_ray)
            return;

        if (m_map[x0][y0].type == CellType::Wall)
            end_ray = true;

        m_map[x0][y0].is_visible = true;
        m_map[x0][y0].is_explored = true;

        if (x0 == x1 && y0 == y1)
            break;

        e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void Map::fill_map_with_walls()
{
    // Fill the map with walls.
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = 0; y < m_height; ++y)
        {
            auto rock_wall = std::make_shared<Object>(
                Vec2{x, y}, TextureManager::get().get_texture("rock_wall"));
            m_map[x][y] = Cell{CellType::Wall, Vec2{x, y}, false, rock_wall, WHITE};
        }
    }
}

void Map::dig_rooms()
{
    // Generate rooms at random areas in the map.
    for (int i = 0; i < m_room_density; ++i)
    {
        // Generate only odd positions.
        Vec2 pos{m_rng.rand_int(1, m_width), m_rng.rand_int(1, m_height)};
        while (pos.x % 2 == 0 || pos.y % 2 == 0)
        {
            pos = Vec2{m_rng.rand_int(1, m_width), m_rng.rand_int(1, m_height)};
        }

        // Generate only even sizes.
        Vec2 size{m_rng.rand_int(m_room_size_min, m_room_size_max),
                  m_rng.rand_int(m_room_size_min, m_room_size_max)};
        while (size.x % 2 != 0 || size.y % 2 != 0)
        {
            size = {m_rng.rand_int(m_room_size_min, m_room_size_max),
                    m_rng.rand_int(m_room_size_min, m_room_size_max)};
        }

        if (!is_area_available(pos, pos + size + 1))
        {
            continue;
        }

        auto color = Color{m_rng.rand_int(0, 255), m_rng.rand_int(0, 255),
                           m_rng.rand_int(0, 255), m_rng.rand_int(0, 255)};
        dig(pos + 1, pos + size, color);

        Region room{{}, color, true};
        for (int x = pos.x + 1; x < pos.x + size.x; ++x)
        {
            for (int y = pos.y + 1; y < pos.y + size.y; ++y)
            {
                room.cells.push_back(&m_map[x][y]);
            }
        }
        m_regions.push_back(room);

        m_rooms.push_back(Room{pos, pos + size + 1});
    }
}

void Map::dig_mazes()
{
    for (int x = 2; x < m_width - 2; ++x)
    {
        for (int y = 2; y < m_height - 2; ++y)
        {
            auto corridor_color = Color{m_rng.rand_int(0, 255), m_rng.rand_int(0, 255),
                                        m_rng.rand_int(0, 255), m_rng.rand_int(0, 255)};

            if (num_of_wall_neighbours(m_map[x][y]) == 8 &&
                m_map[x][y].type == CellType::Wall)
            {
                Region region{{}, corridor_color, false};

                // Starting cell.
                m_live_cells.push_back(std::make_shared<Cell>(m_map[x][y]));
                dig(Vec2{x, y}, corridor_color);

                // The actual algorithm (growing tree algorithm).
                while (!m_live_cells.empty())
                {
                    // Pick the oldest cell.
                    s_ptr<Cell> cell = m_live_cells.back();

                    if (cell->pos.x >= m_width - 2 || cell->pos.y >= m_height - 2 ||
                        cell->pos.x <= 1 || cell->pos.y <= 1)
                    {
                        m_live_cells.pop_back();
                        continue;
                    }

                    // Randomly choose a neighbouring wall that can become an open space.
                    std::vector<Cell*> neighbours;
                    Vec2 pos = cell->pos;
                    neighbours.push_back(&m_map[pos.x][pos.y - 1]);  // Upper cell.
                    neighbours.push_back(&m_map[pos.x + 1][pos.y]);  // Right cell.
                    neighbours.push_back(&m_map[pos.x][pos.y + 1]);  // Bottom cell.
                    neighbours.push_back(&m_map[pos.x - 1][pos.y]);  // Left cell.

                    bool success = false;
                    while (!neighbours.empty())
                    {
                        int index = m_rng.rand_prob_int(m_neighbour_probs);

                        try
                        {
                            neighbours.at(index);
                        }
                        catch (std::out_of_range& e)
                        {
                            continue;
                        }

                        if (neighbours[index]->type == CellType::Floor ||
                            !are_cells_around_free(*neighbours[index]) ||
                            num_of_wall_neighbours(*neighbours[index]) < 6)
                        {
                            neighbours.erase(std::find(
                                neighbours.begin(), neighbours.end(), neighbours[index]));
                            continue;
                        }

                        m_neighbour_probs = {25, 25, 25, 25};
                        m_neighbour_probs[index] = m_corridor_windiness;

                        m_live_cells.push_back(
                            std::make_shared<Cell>(*neighbours[index]));
                        dig(neighbours[index]->pos, corridor_color);
                        region.cells.push_back(neighbours[index]);
                        success = true;
                        break;
                    }

                    if (!success)
                    {
                        m_live_cells.pop_back();
                    }
                }

                m_regions.push_back(region);
            }
        }
    }
}

void Map::connect_regions()
{
    // Find all connectors.
    std::vector<Cell*> connectors;
    for (int x = 1; x < m_width - 1; ++x)
    {
        for (int y = 1; y < m_height - 1; ++y)
        {
            if (m_map[x][y].type == CellType::Wall &&
                is_adjacent_to_different_regions(m_map[x][y]))
            {
                connectors.push_back(&m_map[x][y]);
            }
        }
    }

    // Pick a random room to be the main region.
    Region& main_region = m_regions[m_rng.rand_int(0, m_regions.size() - 1)];
    while (!main_region.is_room)
    {
        main_region = m_regions[m_rng.rand_int(0, m_regions.size() - 1)];
    }
    // Color the main region white.
    main_region.color = WHITE;
    for (auto cell : main_region.cells)
    {
        cell->color = WHITE;
    }

    while (!connectors.empty())
    {
        // Choose a random adjacent connector.
        std::vector<Cell*> adjacent_connectors;
        for (auto connector : connectors)
        {
            if (is_adjacent_to_main_region(*connector, main_region))
            {
                adjacent_connectors.push_back(connector);
            }
        }

        Cell* chosen_connector =
            adjacent_connectors[m_rng.rand_int(0, adjacent_connectors.size() - 1)];

        merge_regions_at_cell(main_region, *chosen_connector);

        dig(chosen_connector->pos, main_region.color);

        connectors.erase(
            std::find(connectors.begin(), connectors.end(), chosen_connector));

        // Remove any extraneous connectors that no longer connect
        // two different regions.
        for (auto connector : adjacent_connectors)
        {
            if (!is_adjacent_to_different_regions(*connector))
            {
                if (connector == chosen_connector)
                    continue;

                connectors.erase(
                    std::find(connectors.begin(), connectors.end(), connector));
            }
        }
    }
}

void Map::remove_dead_ends()
{
    // The amount of dead ends removed depends on the size of the dungeon.
    for (int i = 0; i < m_width * 5; ++i)
    {
        for (int x = 1; x < m_width - 1; ++x)
        {
            for (int y = 1; y < m_height - 1; ++y)
            {
                if (is_dead_end(m_map[x][y]))
                {
                    fill(Vec2{x, y});
                }
            }
        }
    }
}

void Map::remove_pillars()
{
    for (int x = 1; x < m_width - 1; ++x)
    {
        for (int y = 1; y < m_height - 1; ++y)
        {
            if (m_map[x][y].type == CellType::Wall &&
                num_of_floor_neighbours(m_map[x][y]) == 8)
            {
                dig(Vec2{x, y}, m_map[x][y].color);
            }
        }
    }
}

void Map::dig(Vec2 begin, Vec2 end, Color color)
{
    for (int x = begin.x; x < end.x; ++x)
    {
        for (int y = begin.y; y < end.y; ++y)
        {
            auto object = std::make_shared<Object>(
                Vec2{x, y}, TextureManager::get().get_texture("rock_floor"));
            m_map[x][y] = Cell{CellType::Floor, Vec2{x, y}, false, object, color};
        }
    }
}

void Map::dig(Vec2 pos, Color color)
{
    auto rock_floor =
        std::make_shared<Object>(pos, TextureManager::get().get_texture("rock_floor"));
    m_map[pos.x][pos.y] = Cell{CellType::Floor, pos, false, rock_floor, color};
}

void Map::fill(Vec2 pos)
{
    auto rock_wall =
        std::make_shared<Object>(pos, TextureManager::get().get_texture("rock_wall"));
    m_map[pos.x][pos.y] = Cell{CellType::Wall, pos, false, rock_wall, WHITE};
}

bool Map::is_area_available(Vec2 begin, Vec2 end)
{
    if (end.x >= m_width || end.y >= m_height)
        return false;

    for (int x = begin.x; x < end.x; ++x)
    {
        for (int y = begin.y; y < end.y; ++y)
        {
            if (m_map[x][y].type != CellType::Wall)
            {
                return false;
            }
        }
    }

    return true;
}

constexpr int Map::num_of_wall_neighbours(const Cell& cell) const
{
    int num = 0;
    for (int x = cell.pos.x - 1; x <= cell.pos.x + 1; ++x)
    {
        for (int y = cell.pos.y - 1; y <= cell.pos.y + 1; ++y)
        {
            num += m_map[x][y].type == CellType::Wall;
        }
    }
    // Subtract the cell itself.
    return num - 1;
}

constexpr int Map::num_of_floor_neighbours(const Cell& cell) const
{
    int num = 0;
    for (int x = cell.pos.x - 1; x <= cell.pos.x + 1; ++x)
    {
        for (int y = cell.pos.y - 1; y <= cell.pos.y + 1; ++y)
        {
            num += m_map[x][y].type == CellType::Floor;
        }
    }
    return num;
}

bool Map::are_cells_around_free(const Cell& cell) const
{
    for (int x = cell.pos.x - 1; x <= cell.pos.x + 1; ++x)
    {
        for (int y = cell.pos.y - 1; y <= cell.pos.y + 1; ++y)
        {
            if (m_map[x][y].type == CellType::Floor &&
                !is_cell_in_live_cells(m_map[x][y]))
            {
                return false;
            }
        }
    }

    return true;
}

bool Map::is_cell_in_live_cells(const Cell& cell) const
{
    for (auto& live_cell : m_live_cells)
    {
        if (live_cell->pos == cell.pos)
            return true;
    }

    return false;
}

bool Map::is_adjacent_to_different_regions(const Cell& cell) const
{
    Vec2 pos = cell.pos;
    int result = false;
    // Find a cell that is not a wall.
    Cell ref_cell;
    if (m_map[pos.x][pos.y - 1].type != CellType::Wall)  // Upper
        ref_cell = m_map[pos.x][pos.y - 1];
    else if (m_map[pos.x + 1][pos.y].type != CellType::Wall)  // Right
        ref_cell = m_map[pos.x + 1][pos.y];
    else if (m_map[pos.x - 1][pos.y].type != CellType::Wall)  // Left
        ref_cell = m_map[pos.x - 1][pos.y];
    else if (m_map[pos.x][pos.y + 1].type != CellType::Wall)  // Bottom
        ref_cell = m_map[pos.x][pos.y + 1];

    // Upper cell.
    result += m_map[pos.x][pos.y - 1].color != ref_cell.color &&
              m_map[pos.x][pos.y - 1].type != CellType::Wall;
    // Right cell.
    result += m_map[pos.x + 1][pos.y].color != ref_cell.color &&
              m_map[pos.x + 1][pos.y].type != CellType::Wall;
    // Bottom cell.
    result += m_map[pos.x][pos.y + 1].color != ref_cell.color &&
              m_map[pos.x][pos.y + 1].type != CellType::Wall;
    // Left cell.
    result += m_map[pos.x - 1][pos.y].color != ref_cell.color &&
              m_map[pos.x - 1][pos.y].type != CellType::Wall;

    return result > 0;
}

bool Map::is_adjacent_to_main_region(const Cell& cell, const Region& main_region)
{
    for (int x = cell.pos.x - 1; x <= cell.pos.x + 1; ++x)
    {
        for (int y = cell.pos.y - 1; y <= cell.pos.y + 1; ++y)
        {
            if (m_map[x][y].color == main_region.color)
            {
                return true;
            }
        }
    }

    return false;
}
Region* Map::new_region_cell_connects_to(const Region& main_region, const Cell& cell)
{
    for (int x = cell.pos.x - 1; x <= cell.pos.x + 1; ++x)
    {
        for (int y = cell.pos.y - 1; y <= cell.pos.y + 1; ++y)
        {
            if (m_map[x][y].color != main_region.color &&
                m_map[x][y].type != CellType::Wall)
            {
                for (auto& region : m_regions)
                {
                    if (region.color == m_map[x][y].color)
                    {
                        return &region;
                    }
                }
            }
        }
    }

    return nullptr;
}

bool Map::is_dead_end(const Cell& cell)
{
    int result = 0;
    Vec2 pos = cell.pos;
    // Upper cell.
    result += m_map[pos.x][pos.y - 1].type == CellType::Wall;
    // Right cell.
    result += m_map[pos.x + 1][pos.y].type == CellType::Wall;
    // Bottom cell.
    result += m_map[pos.x][pos.y + 1].type == CellType::Wall;
    // Left cell.
    result += m_map[pos.x - 1][pos.y].type == CellType::Wall;

    return result >= 3;
}

bool Map::merge_regions_at_cell(Region& main_region, Cell& cell)
{
    Region* new_region = new_region_cell_connects_to(main_region, cell);

    if (new_region == nullptr)
        return false;

    new_region->color = main_region.color;
    for (auto new_cell : new_region->cells)
    {
        new_cell->color = main_region.color;
        main_region.cells.push_back(new_cell);
    }

    return true;
}

}
