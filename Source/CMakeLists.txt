
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

set(${PROJECT_NAME}_sources
	CommandListItem.cpp
	CommandsWindow.cpp
	RunnerAddOn.cpp
	RunnerApp.cpp
	Preferences.cpp
	PreferencesWindow.cpp
	SortableListView.cpp
	TrackRunner.rdef
	)

include_directories(AFTER SYSTEM
	/boot/system/develop/headers/private/shared
	/boot/system/develop/headers/private/tracker
	${CMAKE_CURRENT_SOURCE_DIR}/tracker)

haiku_add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_sources})

target_link_libraries(${PROJECT_NAME} be shared tracker)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION add-ons/Tracker)
