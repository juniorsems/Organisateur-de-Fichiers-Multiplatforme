cmake_minimum_required(VERSION 3.10)
project(FileOrganizer C)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

# Inclusion de NFDe via add_subdirectory
add_subdirectory(external/nfd-src)

# Inclusion des répertoires
include_directories(
    include 
    external 
    ${SDL2_INCLUDE_DIRS} 
    ${SDL2_TTF_INCLUDE_DIRS}
    external/nfd-src/src/include
)

# Sources
set(SOURCES 
    src/main.c 
    src/organizer.c 
    src/cwalk.c
)

add_executable(FileOrganizer ${SOURCES})

# Liaison
target_link_libraries(FileOrganizer 
    nfd 
    ${SDL2_LIBRARIES} 
    ${SDL2_TTF_LIBRARIES}
)

if(UNIX AND NOT APPLE)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    include_directories(${GTK3_INCLUDE_DIRS})
    target_link_libraries(FileOrganizer ${GTK3_LIBRARIES})
endif()
