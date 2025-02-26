cmake_minimum_required(VERSION 3.21) # Introduction of file(COPY_FILE ...)


set(APPLICATION_VENDORS
		vendors/extras/FA6FreeSolidFontData.h
		vendors/extras/IconsFontAwesome6.h
		vendors/imconfig.h
		vendors/imgui.cpp
		vendors/imgui.h
		vendors/imgui_demo.cpp
		vendors/imgui_draw.cpp
		vendors/imgui_impl_raylib.h
		vendors/imgui_internal.h
		vendors/imgui_stdlib.cpp
		vendors/imgui_stdlib.h
		vendors/imgui_tables.cpp
		vendors/imgui_widgets.cpp
		vendors/imstb_rectpack.h
		vendors/imstb_textedit.h
		vendors/imstb_truetype.h
		vendors/rlImGui.cpp
		vendors/rlImGui.h
		vendors/rlImGuiColors.h
)

# Our Project
set(APPLICATION_SRC
		include/Application.hpp
		src/Application.cpp
		src/main.cpp
		src/Camera.cpp
		include/Camera.hpp
		include/ImGuiLib.hpp
		src/WorldSerializer.cpp
		include/WorldSerializer.hpp
		src/CharacterController.cpp
		include/CharacterController.hpp
		src/EnemyParameters.cpp
		include/EnemyParameters.hpp
)

add_executable(${PROJECT_NAME} ${APPLICATION_SRC} ${APPLICATION_VENDORS})

file(GLOB level_files "${CMAKE_SOURCE_DIR}/Levels/*.fyc")
foreach (level_file ${level_files})
	get_filename_component(level_file_name ${level_file} NAME)
	if(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
		file(CREATE_LINK ${level_file} ${CMAKE_CURRENT_BINARY_DIR}/${level_file_name} RESULT copy_result COPY_ON_ERROR SYMBOLIC)
		if(NOT (copy_result EQUAL 0))
			message(WARNING
					"Symbolic Link of '${level_file_name}' failed. Should have made a copy instead. Error bellow:\n"
					"${copy_result}"
			)
		else ()
			message(VERBOSE "Symlink made of '${level_file}' to '${CMAKE_CURRENT_BINARY_DIR}/${level_file_name}'.")
		endif()
	else()
		file(COPY_FILE ${level_file} ${CMAKE_CURRENT_BINARY_DIR}/${level_file_name} RESULT copy_result ONLY_IF_DIFFERENT)

		if(copy_result EQUAL 0)
			message(VERBOSE "Copy made of '${level_file}' to '${CMAKE_CURRENT_BINARY_DIR}/${level_file_name}'.")
		else ()
			message(WARNING
					"Copy of '${level_file}' failed.\n"
					"${copy_result}"
			)
		endif()
	endif()
endforeach()

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_include_directories(${PROJECT_NAME} PRIVATE vendors)

#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} raylib FYC::Physics)

target_precompile_headers(${PROJECT_NAME} REUSE_FROM Physics)

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
	set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html") # Tell Emscripten to build an example.html file.
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1")
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
	target_link_libraries(${PROJECT_NAME} "-framework IOKit")
	target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
	target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()