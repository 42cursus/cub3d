# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    CMakeLists.txt                                     :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/08 16:59:37 by abelov            #+#    #+#              #
#    Updated: 2025/03/08 16:59:37 by abelov           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

cmake_minimum_required(VERSION 3.8)
set(CMAKE_C_COMPILER clang)
#set(CMAKE_C_COMPILER gcc)

project(cub3d C)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 99)
include_directories(include)

set(FT_CFLAGS
        -Wall -Wextra -Werror
        -Wwrite-strings
        -Wimplicit
#        -Wformat -Werror=format-security
#        -Wstrict-prototypes
#        -Wmissing-prototypes
        -include include/cub3d.h
        -fstack-protector-strong
        -Wno-error=unused-parameter
        -Wno-error=unused-variable
        -O0
        -masm=att
        -g3 -gdwarf-3
        -fasynchronous-unwind-tables
        -fno-omit-frame-pointer
        -fsanitize=address
        -fsanitize=undefined
        -fno-sanitize-recover=all
        -fsanitize=float-divide-by-zero
        -fsanitize=float-cast-overflow)

set(COMMON_LDFLAGS
        -fsanitize=address
        -fsanitize=undefined
        -fno-sanitize-recover=all
        -fsanitize=float-divide-by-zero
        -fsanitize=float-cast-overflow)

#===========================================================================

set(MLX_LIB_NAME mlx)
set(MLX_LIB_FILENAME libmlx.a)
set(MLX_LIB_DIR "${PROJECT_SOURCE_DIR}/lib/mlx")

file(GLOB_RECURSE MLX_LIB_HEADERS "${MLX_LIB_DIR}/*.h")
list(FILTER MLX_LIB_HEADERS EXCLUDE REGEX "${MLX_LIB_DIR}/test/*" )
file(GLOB_RECURSE MLX_LIB_SOURCES "${MLX_LIB_DIR}/*.c")
list(FILTER MLX_LIB_SOURCES EXCLUDE REGEX "${MLX_LIB_DIR}/test/*|${MLX_LIB_DIR}/mlx_lib_xpm.c")

add_custom_target(${MLX_LIB_NAME}_target ALL
        COMMAND make -C ${MLX_LIB_DIR}
        BYPRODUCTS "${MLX_LIB_DIR}/${MLX_LIB_FILENAME}"
        SOURCES "${MLX_LIB_HEADERS};${MLX_LIB_SOURCES}"
        COMMENT "Building MLX_LIB")

add_library(${MLX_LIB_NAME} STATIC IMPORTED)
add_dependencies(${MLX_LIB_NAME} "${MLX_LIB_NAME}_target")
set_target_properties(${MLX_LIB_NAME} PROPERTIES
        IMPORTED_LOCATION "${MLX_LIB_DIR}/${MLX_LIB_FILENAME}" # Make sure to use absolute path here
        INTERFACE_COMPILE_DEFINITIONS ""
        INTERFACE_SOURCES "${MLX_LIB_HEADERS}"
        INTERFACE_INCLUDE_DIRECTORIES "${MLX_LIB_DIR}")

#==============================================================================#
# FT lib
#==============================================================================#

set(FT_LIB_NAME ft)
set(FT_LIB_FILENAME libft.a)
set(FT_LIB_DIR "${PROJECT_SOURCE_DIR}/lib/ft")

file(GLOB_RECURSE FT_LIB_HEADERS "${FT_LIB_DIR}/*.h")
file(GLOB_RECURSE FT_LIB_SOURCES "${FT_LIB_DIR}/ft_*.c")

add_custom_target(${FT_LIB_NAME}_target ALL
        COMMAND make -C ${FT_LIB_DIR} #BUILD_WITH_ASAN=1
        BYPRODUCTS "${FT_LIB_DIR}/${FT_LIB_FILENAME}"
        SOURCES "${FT_LIB_HEADERS};${FT_LIB_SOURCES}"
        COMMENT "Building FT_LIB")

add_library(${FT_LIB_NAME} STATIC IMPORTED)

add_dependencies(${FT_LIB_NAME} "${FT_LIB_NAME}_target")
set_target_properties(${FT_LIB_NAME} PROPERTIES
        IMPORTED_LOCATION ${FT_LIB_DIR}/${FT_LIB_FILENAME}
        INTERFACE_COMPILE_DEFINITIONS ""
        INTERFACE_LINK_OPTIONS ""
        INTERFACE_SOURCES "${FT_LIB_HEADERS}"
        INTERFACE_INCLUDE_DIRECTORIES "${FT_LIB_DIR};${FT_LIB_DIR}/include;${FT_LIB_DIR}/include/ft")

#==============================================================================#

file(GLOB_RECURSE TARGET_HEADERS "src/*.h")
file(GLOB_RECURSE TARGET_SOURCES "src/*.c")

set(TARGET_NAME "${PROJECT_NAME}_exe")

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(RL REQUIRED IMPORTED_TARGET readline)
find_package(X11 REQUIRED) # https://cmake.org/cmake/help/latest/module/FindX11.html
find_library(MATH_LIB m)
# Locate additional libraries that are not covered by FindX11
find_library(XT_LIBRARY Xt REQUIRED)
find_library(XM_LIBRARY Xm REQUIRED)
find_library(MRM_LIBRARY Mrm REQUIRED)
find_library(XPM_LIBRARY Xpm REQUIRED)

set(TARGET_LIBRARIES
        ${MLX_LIB_NAME}
        ${MATH_LIB}
        ${FT_LIB_NAME}
        Threads::Threads
        ${X11_LIBRARIES}
        ${XT_LIBRARY}
        ${XM_LIBRARY}
        ${MRM_LIBRARY}
        ${XPM_LIBRARY}
        Xrandr  # Xrandr is usually part of X11_LIBRARIES but may need explicit linking
        Xext    # Xext is typically in X11_LIBRARIES, but we ensure it's linked
)

add_executable(${TARGET_NAME}
        ${TARGET_HEADERS}
        ${TARGET_SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
        ${PROJECT_SOURCE_DIR}
        /usr/include
        /usr/include/x86_64-linux-gnu)

target_link_libraries(${TARGET_NAME} "${TARGET_LIBRARIES}")
target_link_options(${TARGET_NAME} PRIVATE ${COMMON_LDFLAGS})
target_compile_options(${TARGET_NAME} PRIVATE "${FT_CFLAGS}")
set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}"
        OUTPUT_NAME "${PROJECT_NAME}")

#==============================================================================#
# Custom targets
#==============================================================================#
add_custom_target(fclean
        COMMAND make -C ${FT_LIB_DIR} fclean)

add_custom_target(re
        COMMAND make -C ${FT_LIB_DIR} re
        COMMAND make -C ${PROJECT_SOURCE_DIR} re)

add_custom_target(norm
        COMMAND make -C ${FT_LIB_DIR} norm
        COMMAND make -C ${PROJECT_SOURCE_DIR} norm)
