cmake_minimum_required(VERSION 3.20)
project(atk LANGUAGES CXX)

# ===== C++ standard =====
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ===== Warning =====
add_compile_options(-Wall -Wextra -Wpedantic)

# ===== Shared code =====
add_library(globals OBJECT globals.cpp)

target_include_directories(globals PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ===== Main =====
add_executable(atk atk.cpp)
target_link_libraries(atk PRIVATE globals)

# ===== atk-* =====
file(GLOB_RECURSE ATK_COMMANDS CONFIGURE_DEPENDS
    atk-cmd/*.cpp
)

foreach(cmd ${ATK_COMMANDS})
    # Path relativo a atk-cmd (es: net/scan.cpp)
    file(RELATIVE_PATH rel_path
        ${CMAKE_CURRENT_SOURCE_DIR}/atk-cmd
        ${cmd}
    )

    # Sottodirectory (es: net)
    get_filename_component(subdir ${rel_path} DIRECTORY)

    # Nome eseguibile (scan)
    get_filename_component(exec_name ${cmd} NAME_WE)

    # Directory di output
    set(output_dir
        ${CMAKE_BINARY_DIR}/atk-cmd-bin/${subdir}
    )

    add_executable(${exec_name} ${cmd})
    target_link_libraries(${exec_name} PRIVATE globals)

    set_target_properties(${exec_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${output_dir}
        OUTPUT_NAME ${exec_name}
    )
endforeach()