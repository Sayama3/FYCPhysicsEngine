cmake_minimum_required(VERSION 3.16) # Precompiled header available

project(FYCPhysicsEngine
		VERSION 1.0.0
		DESCRIPTION "A 2D Physics Library built for learning purposes."
		LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Libraries)
add_subdirectory(Physics)
add_subdirectory(Application)