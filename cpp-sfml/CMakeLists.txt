cmake_minimum_required(VERSION 3.16)
project(SpaceInvadersSFML LANGUAGES CXX)

# The project uses C++17 because it is widely supported and works well on macOS.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Fetch SFML automatically so other users can build the project without manually
# downloading the library first.
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
)
FetchContent_MakeAvailable(SFML)

add_executable(SpaceInvaders src/main.cpp)
target_link_libraries(SpaceInvaders PRIVATE SFML::Graphics)

# On macOS, this helps keep the executable simple for a class project.
if(APPLE)
    set_target_properties(SpaceInvaders PROPERTIES MACOSX_BUNDLE FALSE)
endif()
