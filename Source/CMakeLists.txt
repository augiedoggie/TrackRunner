
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

set(${PROJECT_NAME}_sources
	CommandListItem.cpp
	CommandSelectWindow.cpp
	CommandsWindow.cpp
	RunnerAddOn.cpp
	RunnerApp.cpp
	Settings.cpp
	SettingsWindow.cpp
	SortableListView.cpp
	TrackRunner.rdef
	)

include_directories(AFTER SYSTEM
	/boot/system/develop/headers/private/shared
	/boot/system/develop/headers/private/tracker
	${CMAKE_CURRENT_SOURCE_DIR}/tracker)

haiku_add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_sources})

target_link_libraries(${PROJECT_NAME} be shared tracker)

if(HAIKU_ENABLE_I18N)
	set("${PROJECT_NAME}-APP_MIME_SIG" x-vnd.cpr.TrackRunner)
	set("${PROJECT_NAME}-LOCALES" ca cs de en es_419 es fr fur ru sv tr)
	target_link_libraries(${PROJECT_NAME} localestub)
	haiku_add_i18n(${PROJECT_NAME})
endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION add-ons/Tracker)
