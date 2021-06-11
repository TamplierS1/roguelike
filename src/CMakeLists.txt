add_subdirectory(sdl_wrappers)
target_sources(roguelike PRIVATE main.cpp game.cpp log.cpp object.cpp texture_manager.cpp map.cpp)
