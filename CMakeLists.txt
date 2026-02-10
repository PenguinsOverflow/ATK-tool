cmake_minimum_required(VERSION 3.20)
project(atk LANGUAGES CXX)

# ===== C++ standard =====
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ===== Warning =====
add_compile_options(-Wall -Wextra -Wpedantic)

# ===== add CLI11 =====
include(FetchContent)
FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.6.1
)
FetchContent_MakeAvailable(CLI11)

# ===== utils files =====
file(GLOB_RECURSE UTILS_SRC
    utils/src/*.cpp
)

add_library(utils OBJECT
    ${UTILS_SRC}
)

target_include_directories(utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/inc
)


# ===== Main =====
add_executable(atk atk.cpp)
target_link_libraries(atk PRIVATE
    utils
    CLI11::CLI11
)

# ===== atk cmd files (C++) =====
file(GLOB_RECURSE ATK_COMMANDS CONFIGURE_DEPENDS
    atk-cmd/*.cpp
)

foreach(cmd ${ATK_COMMANDS})

    file(RELATIVE_PATH rel_path
        ${CMAKE_CURRENT_SOURCE_DIR}/atk-cmd
        ${cmd}
    )

    # Subdirectory
    get_filename_component(subdir ${rel_path} DIRECTORY)

    # Executable name
    get_filename_component(exec_name ${cmd} NAME_WE)

    # Output directory
    set(output_dir
        ${CMAKE_BINARY_DIR}/atk-cmd-bin/${subdir}
    )

    add_executable(${exec_name} ${cmd})

    target_link_libraries(${exec_name} PRIVATE
        utils
        CLI11::CLI11
    )

    set_target_properties(${exec_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${output_dir}
        OUTPUT_NAME ${exec_name}
    )
endforeach()

# ===== atk cmd files (Python) =====
file(GLOB_RECURSE ATK_PYTHON_SCRIPTS CONFIGURE_DEPENDS
    atk-cmd/*.py
)

foreach(script ${ATK_PYTHON_SCRIPTS})

    file(RELATIVE_PATH rel_path
        ${CMAKE_CURRENT_SOURCE_DIR}/atk-cmd
        ${script}
    )

    # Subdirectory
    get_filename_component(subdir ${rel_path} DIRECTORY)

    # Script name without extension
    get_filename_component(script_name ${script} NAME_WE)

    # Output directory
    set(output_dir
        ${CMAKE_BINARY_DIR}/atk-cmd-bin/${subdir}
    )

    # Output file (without .py extension)
    set(output_file ${output_dir}/${script_name})

    # Create output directory
    file(MAKE_DIRECTORY ${output_dir})

    # Copy script, remove extension, and set executable permission
    add_custom_command(
        OUTPUT ${output_file}
        COMMAND ${CMAKE_COMMAND} -E copy ${script} ${output_file}
        COMMAND chmod +x ${output_file}
        DEPENDS ${script}
        COMMENT "Copying Python script: ${script_name}"
    )

    # Create custom target
    add_custom_target(${script_name}_copy ALL
        DEPENDS ${output_file}
    )

endforeach()

# ==================================================
# Install
# ==================================================
include(GNUInstallDirs)

# Install main command
install(
    TARGETS atk
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install atk-* subcommands (both C++ and Python)
install(
    DIRECTORY ${CMAKE_BINARY_DIR}/atk-cmd-bin/
    DESTINATION ${CMAKE_INSTALL_BINDIR}/atk-cmd-bin
    USE_SOURCE_PERMISSIONS
)