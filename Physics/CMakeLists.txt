cmake_minimum_required(VERSION 3.16) # Precompiled header available

set(PHYSICS_SRC
		src/Math.cpp
		include/Physics/Math.hpp
		src/Particle.cpp
		include/Physics/Particle.hpp
		src/World.cpp
		include/Physics/World.hpp
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
		<iterator>

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
		<any>
		<optional>
		<variant>
		<tuple>

		# C-Types Helpers
		<cstdint>
		<cstring>
		<cmath>
		<cstddef>
)

if(FYC_DOUBLE)
	target_compile_definitions(Physics PUBLIC FYC_DOUBLE=1)
endif ()

add_library(FYC::Physics ALIAS Physics)
