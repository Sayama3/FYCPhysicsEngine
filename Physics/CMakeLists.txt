cmake_minimum_required(VERSION 3.16) # Precompiled header available

set(PHYSICS_SRC
		include/Physics/Physics.hpp
		src/Physics.cpp
		src/Math.cpp
		include/Physics/Math.hpp
)

add_library(Physics STATIC ${PHYSICS_SRC})

target_include_directories(Physics PUBLIC include)
target_include_directories(Physics PRIVATE src)

target_precompile_headers(Physics PUBLIC
		# Utility stuff
		<functional>
		<utility>
		<algorithm>
		<memory>
		<source_location>

		# Exception related stuff
		<exception>
		<stdexcept>

		# String & IO manipulation
		<filesystem>
		<string>
		<string_view>

		# Containers
		<array>
		<vector>
		<stack>
		<queue>

		# C-Types Helpers
		<cstdint>
		<cstring>
		<cmath>
)

if(FYC_DOUBLE)
	target_compile_definitions(Physics PUBLIC FYC_DOUBLE=1)
endif ()

add_library(FYC::Physics ALIAS Physics)
