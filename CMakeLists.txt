cmake_minimum_required(VERSION 3.19)

project(TrackRunner)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules")

include(UseHaiku)

option(BUILD_USERGUIDE "Use sphinx to generate the user guide" ON)
if(BUILD_USERGUIDE)
	find_package(Sphinx)
	if (Sphinx_FOUND)
		add_subdirectory(Assets)
	else()
		message(WARNING "Sphinx was not found.  You will not be able to build the user guide.")
	endif()
else()
	message("Generating user guide is disabled.")
endif()

option(USE_CLANG "Enable building with clang instead of gcc" OFF)
if(USE_CLANG)
	set(CMAKE_CXX_COMPILER clang++)
	set(CMAKE_C_COMPILER clang)
	add_compile_options(-fPIC)
endif()

option(STRICT_WARNINGS "Compile with extra warnings and errors" ON)
if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 2.96 AND STRICT_WARNINGS)
	add_compile_options(-Wall -Wextra -Wshadow -Werror)
	if(USE_CLANG)
		# clang finds a lot more problems than gcc, disable some of the warnings
		# set these here instead of earlier, they must come after -Wall, -Werror, etc...
		add_compile_options(-Wno-overloaded-virtual -Wno-unused-private-field
							-Wno-return-type-c-linkage -Wno-unused-const-variable
							-Wno-deprecated-register)
	endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-DDEBUG)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

option(USE_MENUITEM_ICONS "(Experimental) Enable use of icons for the context menu items" OFF)
if(USE_MENUITEM_ICONS)
	add_definitions(-DUSE_MENUITEM_ICONS)
endif()

add_subdirectory(Source)
