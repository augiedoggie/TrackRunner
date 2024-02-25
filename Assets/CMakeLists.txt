
#TODO remove extra files that the theme installs
add_custom_command(
	OUTPUT ${PROJECT_BINARY_DIR}/UserGuide/index.html
	COMMAND ${SPHINX_BUILD_EXE} -a -W -q -b html -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/UserGuide ${PROJECT_BINARY_DIR}/UserGuide
	MAIN_DEPENDENCY UserGuide/conf.py
	DEPENDS UserGuide/index.rst UserGuide/custom.css
	COMMENT "Building user guide"
	VERBATIM
)

add_custom_target("UserGuide" ALL DEPENDS ${PROJECT_BINARY_DIR}/UserGuide/index.html)

add_custom_command(TARGET "UserGuide" POST_BUILD
	COMMAND settype -t text/css ${PROJECT_BINARY_DIR}/UserGuide/_static/*.css
	COMMAND settype -t text/html ${PROJECT_BINARY_DIR}/UserGuide/*.html
	COMMAND mimeset ${PROJECT_BINARY_DIR}/UserGuide/**
	)

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES
	${PROJECT_BINARY_DIR}/UserGuide
	${CMAKE_CURRENT_BINARY_DIR}/environment.pickle
	${CMAKE_CURRENT_BINARY_DIR}/index.doctree
)

install(DIRECTORY ${PROJECT_BINARY_DIR}/UserGuide DESTINATION documentation/${PROJECT_NAME})
