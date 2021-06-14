#include <iostream>

#include "map.h"
#include "texture_manager.h"

namespace Rg::Map
{
Map::Map(const s_ptr<Renderer>& renderer)
    : m_renderer(renderer)
    , m_rock_floor_texture(TextureManager::get().get_texture("rock_floor"))
{
    fill_map_with_walls();
    dig_rooms();
    dig_mazes();
    connect_regions();
    remove_dead_ends();
}

void Map::render(const Camera& camera)
{
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = 0; y < m_height; ++y)
        {
            auto object = m_map[x][y].m_object;
            object->render(camera, m_map[x][y].m_color);

            if (!m_map[x][y].is_dynamic)
                continue;

            object->regen_energy();

            auto action = object->get_action();
            if (action != nullptr)
            {
                if (!action->execute(object.get()))
                {
                    Log::warning("Not enough energy to perform an action.\n");
                }
            }
        }
    }
}

void Map::fill_map_with_walls()
{
    // Fill the map with walls.
    auto rock_wall_texture = TextureManager::get().get_texture("rock_wall");
    for (int x = 0; x < m_width; ++x)
    {
        for (int y = 0; y < m_height; ++y)
        {
            auto rock_wall = std::make_shared<Object>(
                Vec2{x * g_tile_size, y * g_tile_size}, rock_wall_texture, m_renderer);
            m_map[x][y] = Cell{CellType::Wall, Vec2{x, y}, false, rock_wall,
                               Color{255, 255, 255, 255}};
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
                room.m_cells.push_back(&m_map[x][y]);
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

            if (num_of_solid_neighbours(m_map[x][y]) == 8 &&
                m_map[x][y].m_type == CellType::Wall)
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

                    if (cell->m_pos.x >= m_width - 2 || cell->m_pos.y >= m_height - 2 ||
                        cell->m_pos.x <= 1 || cell->m_pos.y <= 1)
                    {
                        m_live_cells.pop_back();
                        continue;
                    }

                    // Randomly choose a neighbouring wall that can become an open space.
                    std::vector<Cell*> neighbours;
                    Vec2 pos = cell->m_pos;
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

                        if (neighbours[index]->m_type == CellType::Floor ||
                            !are_cells_around_free(*neighbours[index]) ||
                            num_of_solid_neighbours(*neighbours[index]) < 6)
                        {
                            neighbours.erase(std::find(
                                neighbours.begin(), neighbours.end(), neighbours[index]));
                            continue;
                        }

                        m_neighbour_probs = {25, 25, 25, 25};
                        m_neighbour_probs[index] = m_corridor_windiness;

                        m_live_cells.push_back(
                            std::make_shared<Cell>(*neighbours[index]));
                        dig(neighbours[index]->m_pos, corridor_color);
                        region.m_cells.push_back(neighbours[index]);
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
            if (m_map[x][y].m_type == CellType::Wall &&
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
    main_region.m_color = g_color_white;
    for (auto cell : main_region.m_cells)
    {
        cell->m_color = g_color_white;
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

        dig(chosen_connector->m_pos, main_region.m_color);

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

void Map::dig(Vec2 begin, Vec2 end, Color color)
{
    for (int x = begin.x; x < end.x; ++x)
    {
        for (int y = begin.y; y < end.y; ++y)
        {
            auto object = std::make_shared<Object>(Vec2{x * g_tile_size, y * g_tile_size},
                                                   m_rock_floor_texture, m_renderer);
            m_map[x][y] = Cell{CellType::Floor, Vec2{x, y}, false, object, color};
        }
    }
}

void Map::dig(Vec2 pos, Color color)
{
    auto object = std::make_shared<Object>(Vec2{pos.x * g_tile_size, pos.y * g_tile_size},
                                           m_rock_floor_texture, m_renderer);
    m_map[pos.x][pos.y] = Cell{CellType::Floor, Vec2{pos.x, pos.y}, false, object, color};
}

void Map::fill(Vec2 pos)
{
    auto rock_wall_texture = TextureManager::get().get_texture("rock_wall");
    auto rock_wall = std::make_shared<Object>(
        Vec2{pos.x * g_tile_size, pos.y * g_tile_size}, rock_wall_texture, m_renderer);
    m_map[pos.x][pos.y] =
        Cell{CellType::Wall, pos, false, rock_wall, Color{255, 255, 255, 255}};
}

bool Map::is_area_available(Vec2 begin, Vec2 end)
{
    if (end.x >= m_width || end.y >= m_height)
        return false;

    for (int x = begin.x; x < end.x; ++x)
    {
        for (int y = begin.y; y < end.y; ++y)
        {
            if (m_map[x][y].m_type != CellType::Wall)
            {
                return false;
            }
        }
    }

    return true;
}

constexpr int Map::num_of_solid_neighbours(const Cell& cell) const
{
    int num = 0;
    for (int x = cell.m_pos.x - 1; x <= cell.m_pos.x + 1; ++x)
    {
        for (int y = cell.m_pos.y - 1; y <= cell.m_pos.y + 1; ++y)
        {
            num += m_map[x][y].m_type == CellType::Wall;
        }
    }
    // Subtract the cell itself.
    return num - 1;
}

bool Map::are_cells_around_free(const Cell& cell) const
{
    for (int x = cell.m_pos.x - 1; x <= cell.m_pos.x + 1; ++x)
    {
        for (int y = cell.m_pos.y - 1; y <= cell.m_pos.y + 1; ++y)
        {
            if (m_map[x][y].m_type == CellType::Floor &&
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
        if (live_cell->m_pos == cell.m_pos)
            return true;
    }

    return false;
}

bool Map::is_adjacent_to_different_regions(const Cell& cell) const
{
    Vec2 pos = cell.m_pos;
    int result = false;
    // Find a cell that is not a wall.
    Cell ref_cell;
    if (m_map[pos.x][pos.y - 1].m_type != CellType::Wall)  // Upper
        ref_cell = m_map[pos.x][pos.y - 1];
    else if (m_map[pos.x + 1][pos.y].m_type != CellType::Wall)  // Right
        ref_cell = m_map[pos.x + 1][pos.y];
    else if (m_map[pos.x - 1][pos.y].m_type != CellType::Wall)  // Left
        ref_cell = m_map[pos.x - 1][pos.y];
    else if (m_map[pos.x][pos.y + 1].m_type != CellType::Wall)  // Bottom
        ref_cell = m_map[pos.x][pos.y + 1];

    // Upper cell.
    result += m_map[pos.x][pos.y - 1].m_color != ref_cell.m_color &&
              m_map[pos.x][pos.y - 1].m_type != CellType::Wall;
    // Right cell.
    result += m_map[pos.x + 1][pos.y].m_color != ref_cell.m_color &&
              m_map[pos.x + 1][pos.y].m_type != CellType::Wall;
    // Bottom cell.
    result += m_map[pos.x][pos.y + 1].m_color != ref_cell.m_color &&
              m_map[pos.x][pos.y + 1].m_type != CellType::Wall;
    // Left cell.
    result += m_map[pos.x - 1][pos.y].m_color != ref_cell.m_color &&
              m_map[pos.x - 1][pos.y].m_type != CellType::Wall;

    return result > 0;
}

bool Map::is_adjacent_to_main_region(const Cell& cell, const Region& main_region)
{
    for (int x = cell.m_pos.x - 1; x <= cell.m_pos.x + 1; ++x)
    {
        for (int y = cell.m_pos.y - 1; y <= cell.m_pos.y + 1; ++y)
        {
            if (m_map[x][y].m_color == main_region.m_color)
            {
                return true;
            }
        }
    }

    return false;
}
Region* Map::new_region_cell_connects_to(const Region& main_region, const Cell& cell)
{
    for (int x = cell.m_pos.x - 1; x <= cell.m_pos.x + 1; ++x)
    {
        for (int y = cell.m_pos.y - 1; y <= cell.m_pos.y + 1; ++y)
        {
            if (m_map[x][y].m_color != main_region.m_color &&
                m_map[x][y].m_type != CellType::Wall)
            {
                for (auto& region : m_regions)
                {
                    if (region.m_color == m_map[x][y].m_color)
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
    Vec2 pos = cell.m_pos;
    // Upper cell.
    result += m_map[pos.x][pos.y - 1].m_type == CellType::Wall;
    // Right cell.
    result += m_map[pos.x + 1][pos.y].m_type == CellType::Wall;
    // Bottom cell.
    result += m_map[pos.x][pos.y + 1].m_type == CellType::Wall;
    // Left cell.
    result += m_map[pos.x - 1][pos.y].m_type == CellType::Wall;

    return result >= 3;
}

bool Map::merge_regions_at_cell(Region& main_region, Cell& cell)
{
    Region* new_region = new_region_cell_connects_to(main_region, cell);

    if (new_region == nullptr)
        return false;

    new_region->m_color = main_region.m_color;
    for (auto new_cell : new_region->m_cells)
    {
        new_cell->m_color = main_region.m_color;
        main_region.m_cells.push_back(new_cell);
    }

    return true;
}

}
