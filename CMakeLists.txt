# CMakeList.txt: LearnRayTracing 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project (LearnRayTracing)

# Set to c++11
set(CMAKE_CXX_STANDARD 11)

# 将源代码添加到此项目的可执行文件。
add_executable (LearnRayTracing  "src/common/vec3.cpp" "src/InOneWeekend/main.cpp" "src/common/ray.h" "src/common/hittable.h" 
	"src/common/sphere.h" "src/common/hittable_list.h" "src/common/const_var.h" "src/common/camera.h" "src/common/vec3.h" 
	"src/common/color.h" "src/common/material.h" "src/common/moving_sphere.h" "src/common/aabb.h" "src/common/bvh_node.h" 
	"src/common/texture.h" "src/other/interpolation.h" "src/other/value_noise.h")

target_include_directories(LearnRayTracing
	PRIVATE
		${PROJECT_SOURCE_DIR}/src/common
)

# TODO: 如有需要，请添加测试并安装目标。
