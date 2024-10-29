cmake_minimum_required(VERSION 3.16) # Precompiled header available

project(FYCPhysicsEngine
		VERSION 1.0.0
		DESCRIPTION "A 2D Physics Library built for learning purposes."
		LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(FYC_DOUBLE "Use 64bits precision float for the physics engine." OFF)
option(FYC_APPLICATION "Build the application." ON)

add_subdirectory(Physics)
if(FYC_APPLICATION)
	add_subdirectory(Libraries)
	add_subdirectory(Application)
endif()