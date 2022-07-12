cmake_minimum_required(VERSION 3.16)

project(questa-hidpi C)

option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." ON)

if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
       add_compile_options(-fdiagnostics-color=always)
    elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang")
       add_compile_options(-fcolor-diagnostics)
    endif ()
endif ()

add_compile_options(-Wall -Wextra)
add_compile_options(-fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables)
add_compile_options(-Os -fvisibility=hidden -ffunction-sections -fdata-sections)
add_link_options(-Wl,--gc-sections)
add_compile_options(-g0)

# add_compile_options(-O0 -ggdb3 -fno-omit-frame-pointer)

# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)

# add_compile_options(-fsanitize=memory)
# add_link_options(-fsanitize=memory)

# add_link_options(-fuse-ld=mold)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS NO)

add_subdirectory(3rdparty)
add_subdirectory(lib)
