cmake_minimum_required(VERSION 3.20)
project(atk LANGUAGES CXX)

# ===== C++ standard =====
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ===== Warning =====
add_compile_options(-Wall -Wextra -Wpedantic)

# ===== Codice condiviso =====
add_library(globals OBJECT globals.cpp)

target_include_directories(globals PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ===== Eseguibile principale =====
add_executable(atk atk.cpp)
target_link_libraries(atk PRIVATE globals)

# ===== Comandi atk-* =====
file(GLOB ATK_COMMANDS CONFIGURE_DEPENDS
    atk-cmd/*.cpp
)

foreach(cmd ${ATK_COMMANDS})
    get_filename_component(cmd_name ${cmd} NAME_WE)

    add_executable(${cmd_name} ${cmd})
    target_link_libraries(${cmd_name} PRIVATE globals)

    set_target_properties(${cmd_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_BINARY_DIR}/atk-cmd-bin
    )
endforeach()
