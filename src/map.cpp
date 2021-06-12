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
        Vec2 pos{m_rng.rand_int(1, m_width), m_rng.rand_int(1, m_height)};

        Vec2 size{m_rng.rand_int(m_room_size_min, m_room_size_max),
                  m_rng.rand_int(m_room_size_min, m_room_size_max)};

        if (!is_area_available(pos, pos + size + 1))
        {
            continue;
        }

        dig(pos + 1, pos + size,
            Color{m_rng.rand_int(0, 255), m_rng.rand_int(0, 255), m_rng.rand_int(0, 255),
                  m_rng.rand_int(0, 255)});

        m_rooms.push_back(Room{pos, pos + size + 1});
    }
}

void Map::dig_mazes()
{
    for (int x = 3; x < m_width - 3; x += 2)
    {
        for (int y = 3; y < m_height - 3; y += 2)
        {
            auto corridor_color = Color{m_rng.rand_int(0, 255), m_rng.rand_int(0, 255),
                                        m_rng.rand_int(0, 255), m_rng.rand_int(0, 255)};
            if (num_of_solid_neighbours(&m_map[x][y]) == 8 &&
                m_map[x][y].m_type == CellType::Wall)
            {
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
                            !are_cells_around_free(neighbours[index]) ||
                            num_of_solid_neighbours(neighbours[index]) < 6)
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
                        success = true;
                        break;
                    }

                    if (!success)
                    {
                        m_live_cells.pop_back();
                    }
                }
            }
        }
    }
}

void Map::connect_regions()
{
    // Pick a random room to be the main region.
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

constexpr int Map::num_of_solid_neighbours(const Cell* cell) const
{
    int num = 0;
    for (int x = cell->m_pos.x - 1; x <= cell->m_pos.x + 1; ++x)
    {
        for (int y = cell->m_pos.y - 1; y <= cell->m_pos.y + 1; ++y)
        {
            num += m_map[x][y].m_type == CellType::Wall;
        }
    }
    // Subtract the cell itself.
    return num - 1;
}

bool Map::are_cells_around_free(const Cell* cell) const
{
    for (int x = cell->m_pos.x - 1; x <= cell->m_pos.x + 1; ++x)
    {
        for (int y = cell->m_pos.y - 1; y <= cell->m_pos.y + 1; ++y)
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

}
