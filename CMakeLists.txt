cmake_minimum_required(VERSION 3.16)

project(Hellborne LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(hellborne
    main.cpp
    game.cpp
)

target_include_directories(hellborne PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(SDL2 QUIET CONFIG)
find_package(SDL2_image QUIET CONFIG)

if(NOT SDL2_FOUND)
    find_package(SDL2 QUIET)
endif()

if(NOT SDL2_image_FOUND)
    find_package(SDL2_image QUIET)
endif()

if(TARGET SDL2::SDL2)
    target_link_libraries(hellborne PRIVATE SDL2::SDL2)
elseif(TARGET SDL2::SDL2-static)
    target_link_libraries(hellborne PRIVATE SDL2::SDL2-static)
elseif(SDL2_FOUND)
    target_include_directories(hellborne PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(hellborne PRIVATE ${SDL2_LIBRARIES})
else()
    if(CMAKE_SYSTEM_NAME STREQUAL "QNX")
        find_path(SDL2_INCLUDE_DIR
            NAMES SDL2/SDL.h
            PATHS
                $ENV{QNX_TARGET}/usr/include
                ${CMAKE_SYSROOT}/usr/include
            NO_DEFAULT_PATH
        )
        find_library(SDL2_LIBRARY
            NAMES SDL2
            PATHS
                $ENV{QNX_TARGET}/aarch64le/lib
                ${CMAKE_SYSROOT}/aarch64le/lib
            NO_DEFAULT_PATH
        )

        if(NOT SDL2_INCLUDE_DIR OR NOT SDL2_LIBRARY)
            message(FATAL_ERROR "SDL2 not found for QNX. Set QNX_TARGET or provide SDL2 include/library paths.")
        endif()

        target_include_directories(hellborne PRIVATE ${SDL2_INCLUDE_DIR})
        target_link_libraries(hellborne PRIVATE ${SDL2_LIBRARY})
    else()
        message(FATAL_ERROR "SDL2 not found. Install SDL2 dev files or set SDL2_DIR/CMAKE_PREFIX_PATH.")
    endif()
endif()

if(TARGET SDL2_image::SDL2_image)
    target_link_libraries(hellborne PRIVATE SDL2_image::SDL2_image)
elseif(TARGET SDL2_image::SDL2_image-static)
    target_link_libraries(hellborne PRIVATE SDL2_image::SDL2_image-static)
elseif(SDL2_image_FOUND)
    target_include_directories(hellborne PRIVATE ${SDL2_IMAGE_INCLUDE_DIRS})
    target_link_libraries(hellborne PRIVATE ${SDL2_IMAGE_LIBRARIES})
else()
    if(CMAKE_SYSTEM_NAME STREQUAL "QNX")
        find_path(SDL2_IMAGE_INCLUDE_DIR
            NAMES SDL2/SDL_image.h
            PATHS
                $ENV{QNX_TARGET}/usr/include
                ${CMAKE_SYSROOT}/usr/include
            NO_DEFAULT_PATH
        )
        find_library(SDL2_IMAGE_LIBRARY
            NAMES SDL2_image
            PATHS
                $ENV{QNX_TARGET}/aarch64le/lib
                ${CMAKE_SYSROOT}/aarch64le/lib
            NO_DEFAULT_PATH
        )

        if(NOT SDL2_IMAGE_INCLUDE_DIR OR NOT SDL2_IMAGE_LIBRARY)
            message(FATAL_ERROR "SDL2_image not found for QNX. Set QNX_TARGET or provide SDL2_image include/library paths.")
        endif()

        target_include_directories(hellborne PRIVATE ${SDL2_IMAGE_INCLUDE_DIR})
        target_link_libraries(hellborne PRIVATE ${SDL2_IMAGE_LIBRARY})
    else()
        message(FATAL_ERROR "SDL2_image not found. Install SDL2_image dev files or set SDL2_image_DIR/CMAKE_PREFIX_PATH.")
    endif()
endif()

if(MSVC)
    target_compile_options(hellborne PRIVATE /W3)
else()
    target_compile_options(hellborne PRIVATE -Wall -Wextra -Wpedantic)
endif()
