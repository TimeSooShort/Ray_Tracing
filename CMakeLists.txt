# CMakeList.txt: LearnRayTracing 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project (LearnRayTracing)

# Set to c++11
set(CMAKE_CXX_STANDARD 11)

SET(RESOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/resources CACHE PATH "This directory contains resource")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/configuration/root_directory.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/common/root_directory.h")

# 将源代码添加到此项目的可执行文件。
add_executable (LearnRayTracing  "src/common/vec3.cpp" "src/InOneWeekend/main.cpp" "src/common/ray.h" "src/common/hittable.h" 
	"src/common/sphere.h" "src/common/hittable_list.h" "src/common/const_var.h" "src/common/camera.h" "src/common/vec3.h" 
	"src/common/color.h" "src/common/material.h" "src/common/moving_sphere.h" "src/common/aabb.h" "src/common/bvh_node.h" 
	"src/common/texture.h" "src/other/interpolation.h" "src/other/value_noise.h" "src/other/perlin_noise.h" "src/thirdParty/stb_image.h" 
	"src/thirdParty/rtw_stb_image.h" "src/common/filesystemUtil.h" "src/common/aarect.h")

target_include_directories(LearnRayTracing
	PRIVATE
		${PROJECT_SOURCE_DIR}/src/common
		${PROJECT_SOURCE_DIR}/src/other
		${PROJECT_SOURCE_DIR}/src/thirdParty
)

# TODO: 如有需要，请添加测试并安装目标。
