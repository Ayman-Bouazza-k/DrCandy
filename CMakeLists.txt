cmake_minimum_required(VERSION 3.10)

project(p2_project)

set(CMAKE_CXX_STANDARD 20)

add_executable(p2_project)

# Compile with verbose output, add debug information and don't optimize anything for easier instruction stepping
target_compile_options(p2_project PRIVATE -Wextra -g3 -O0)

# Include from anywhere
include_directories("src/")
include_directories("src/private/sdl_wrapper")
include_directories("src/private/extlibs/libpng")
include_directories("src/private/extlibs/SDL/include")
include_directories("src/private/extlibs/SDL_ttf/include")
include_directories("test/")

# Automatically add everything under the Logic Game folder
file(GLOB_RECURSE files
        "src/*.cpp"
        "test/*.cpp"
)
target_sources(p2_project PRIVATE ${files})

find_package(SDL2 REQUIRED)
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
target_link_libraries(p2_project PRIVATE
        SDL2_ttf
        SDL2_image
        SDL2_mixer
        SDL2_gfx
        SDL2_test
        SDL2main
        SDL2
        png
        png16
)