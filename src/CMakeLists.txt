add_subdirectory(sdl_wrappers)
add_subdirectory(components)
target_sources(roguelike PRIVATE main.cpp game.cpp log.cpp object.cpp texture_manager.cpp)
