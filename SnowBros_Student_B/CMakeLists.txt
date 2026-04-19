cmake_minimum_required(VERSION 3.16)
project(SnowBros)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Source files
set(SOURCES
    main.cpp
    src/Game.h
    src/Game.cpp
    src/Entities/Entity.h
    src/Entities/Entity.cpp
    src/Entities/Enemy.h
    src/Entities/Enemy.cpp
    src/Entities/Botom.h
    src/Entities/Botom.cpp
    src/Systems/EventBus.h
    src/Systems/EventBus.cpp
)

# Create executable
add_executable(SnowBros ${SOURCES})

# Link SFML
target_link_libraries(SnowBros PRIVATE sfml-graphics sfml-window sfml-system)

# Set startup project (Visual Studio)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SnowBros)

# Copy assets to build directory (optional - add when assets are ready)
# add_custom_command(TARGET SnowBros POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#     ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets)
