#=============================================================================
# SPDX-FileCopyrightText: 2021 Chris Roberts
#
# SPDX-License-Identifier: MIT
#=============================================================================

#
# - Haiku module for CMake
#


#
#	Use the standard non-packaged directory if no prefix was given to cmake
#
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	execute_process(COMMAND finddir B_USER_NONPACKAGED_DIRECTORY OUTPUT_VARIABLE B_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
	set(CMAKE_INSTALL_PREFIX "${B_PREFIX}" CACHE PATH "Default non-packaged install path" FORCE)
endif()


#
#	Find the proper stdc++ library
#
find_library(HAIKU_LIBSTDCPP NAMES "stdc++" "stdc++.r4")


#
#	Add our options to control localization & target BFS tagging
#
option(HAIKU_ENABLE_I18N "Enable Haiku localization support")
option(HAIKU_ENABLE_TARGET_ATTRS "Enable setting custom BFS attributes on built targets" ON)


#
#	Set up some top-level targets if localization is enabled
#
if(HAIKU_ENABLE_I18N)

	add_custom_target("catkeys")
	add_custom_target("catalogs")
	add_custom_target("bindcatalogs")
	add_custom_target("catalogsinstall"
	  DEPENDS "catalogs"
	  COMMAND "${CMAKE_COMMAND}" "-DCMAKE_INSTALL_COMPONENT=locales" "-P" "${CMAKE_BINARY_DIR}/cmake_install.cmake"
	)

	if(NOT DEFINED CMAKE_INSTALL_LOCALEDIR)
		set(CMAKE_INSTALL_LOCALEDIR "data/locale")
	endif()

endif()


#
#	Convenience function to create an app with rdef/rsrc files
#
function(haiku_add_executable TARGET)

	foreach(arg ${ARGN})
		if(${arg} MATCHES ".*rdef$")
			list(APPEND rdeflist ${arg})
		elseif(${arg} MATCHES ".*rsrc$")
			list(APPEND rsrclist "${CMAKE_CURRENT_SOURCE_DIR}/${arg}")
		else()
			list(APPEND REAL_SOURCES ${arg})
		endif()
	endforeach()

	# Call the original function with the filtered source list.
	add_executable(${TARGET} ${REAL_SOURCES})

	# rdef/rsrc targets must be added after the main target has been created with _add_executable()
	foreach(rdef ${rdeflist})
		haiku_add_resource_def(${TARGET} ${rdef})
	endforeach()

	# any precompiled resources that were given to us
	foreach(rsrc ${rsrclist})
		haiku_add_resource(${TARGET} ${rsrc})
	endforeach()

	haiku_mimeset_target(${TARGET})

endfunction()


#
#	Convenience function to create a shared object with rdef/rsrc files and no lib prefix or .so suffix
#
function(haiku_add_addon TARGET)

	foreach(arg ${ARGN})
		if(${arg} MATCHES ".*rdef$")
			list(APPEND rdeflist ${arg})
		elseif(${arg} MATCHES ".*rsrc$")
			list(APPEND rsrclist "${CMAKE_CURRENT_SOURCE_DIR}/${arg}")
		else()
			list(APPEND REAL_SOURCES ${arg})
		endif()
	endforeach()

	# Call the original function with the filtered source list.
	add_library(${TARGET} MODULE ${REAL_SOURCES})

	set_target_properties(${TARGET}
		PROPERTIES
		PREFIX ""
		SUFFIX "")

	# rdef/rsrc targets must be added after the main target has been created with _add_executable()
	foreach(rdef ${rdeflist})
		haiku_add_resource_def(${TARGET} ${rdef})
	endforeach()

	# any precompiled resources that were given to us
	foreach(rsrc ${rsrclist})
		haiku_add_resource(${TARGET} ${rsrc})
	endforeach()

	haiku_mimeset_target(${TARGET})

endfunction()


#
#	Adds the target to the top-level targets catkeys/catalogs/bindcatalogs/catalogsinstall
#
function(haiku_add_i18n TARGET)

	if(NOT DEFINED "${TARGET}-APP_MIME_SIG")
		message(WARNING "No APP_MIME_SIG property for ${TARGET}. Using 'application/x-vnd.Foo-Bar'")
		set("${TARGET}-APP_MIME_SIG" "application/x-vnd.Foo-Bar")
	endif()

	if(NOT DEFINED "${TARGET}-LOCALES")
		message(WARNING "No LOCALES property for ${TARGET}. Using 'en'")
		set("${TARGET}-LOCALES" "en")
	endif()

	haiku_generate_base_catkeys(${TARGET})
	haiku_compile_catalogs(${TARGET} "${${TARGET}-LOCALES}")
	haiku_bind_catalogs(${TARGET} "${${TARGET}-LOCALES}")
	haiku_install_catalogs(${TARGET} "${${TARGET}-LOCALES}")

	target_compile_definitions(${TARGET} PRIVATE "HAIKU_ENABLE_I18N")

endfunction()


#
#	Add a post-build command to merge a resource file into the target
#
function(haiku_add_resource TARGET RSRC)

	get_filename_component(shortname ${RSRC} NAME)

	add_custom_command(
		TARGET ${TARGET}
		POST_BUILD
		COMMAND "xres" "-o" "$<TARGET_FILE:${TARGET}>" "${RSRC}"
		COMMENT "Merging resources from ${shortname} into ${TARGET}")

endfunction()


#
#	Add a post-build command to run mimeset to the target
#
function(haiku_mimeset_target TARGET)

	add_custom_command(
		TARGET ${TARGET}
		POST_BUILD
		COMMAND "mimeset" "-f" "$<TARGET_FILE:${TARGET}>"
		COMMENT "Setting mimetype for ${TARGET}")

endfunction()


#
#	Compile a resource definition file(.rdef) to a resource file(.rsrc)
#
function(haiku_add_resource_def TARGET RDEF_SOURCE)

	haiku_compile_resource_def(${RDEF_SOURCE} RSRC_OUT)

	get_filename_component(rsrc_target ${RSRC_OUT} NAME)

	add_dependencies(${TARGET} ${rsrc_target})

	haiku_add_resource(${TARGET} ${RSRC_OUT})

	# somewhat ugly hack to avoid generating a dependency on the target
	# like the regular $<TARGET_FILE> generator does
	set(TARGET_PATH "$<TARGET_FILE_DIR:${TARGET}>/$<TARGET_FILE_NAME:${TARGET}>")
	# remove the target(exe/lib) and force it to be rebuilt
	add_custom_command(
		OUTPUT "${RSRC_OUT}"
		COMMAND "${CMAKE_COMMAND}" "-E" "remove" "-f" "${TARGET_PATH}"
		APPEND)

endfunction()


#
#	Compile a resource definition file(.rdef) to a resource file(.rsrc)
#
function(haiku_compile_resource_def RDEF_SOURCE RSRC_OUT)

	get_filename_component(shortname ${RDEF_SOURCE} NAME)
	get_filename_component(rdefpath ${RDEF_SOURCE} ABSOLUTE)
	get_filename_component(basename ${RDEF_SOURCE} NAME_WE)

	set(rsrcfile "${basename}.rsrc")
	set(rsrcdir "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${rsrcfile}.dir")

	add_custom_command(
		OUTPUT "${rsrcdir}/${rsrcfile}"
		COMMAND "rc" "-o" "${rsrcfile}" "${rdefpath}"
		DEPENDS ${rdefpath}
		WORKING_DIRECTORY ${rsrcdir}
		COMMENT "Compiling resource definition ${shortname}")

	add_custom_target(${rsrcfile} DEPENDS "${rsrcdir}/${rsrcfile}")

	set_source_files_properties(${rsrcfile} PROPERTIES GENERATED TRUE)

	set("${RSRC_OUT}" "${rsrcdir}/${rsrcfile}" PARENT_SCOPE)

endfunction()


#
#	Add a BFS attribute to a built target
#
function(haiku_add_target_attr TARGET ANAME AVALUE)

	#TODO allow overriding the working diretory
	add_custom_command(
		TARGET ${TARGET}
		POST_BUILD
		COMMAND sh -c "addattr -t string \"${ANAME}\" \"${AVALUE}\" '$<TARGET_FILE:${TARGET}>'"
		WORKING_DIRECTORY $<TARGET_PROPERTY:${TARGET},SOURCE_DIR>
		VERBATIM
		COMMENT "Adding ${ANAME} BFS attribute to ${TARGET}")

endfunction()


#
#	Regenerate the main locales/en.catkeys file for a target
#
function(haiku_generate_base_catkeys TARGET)

	haiku_get_app_mime_subtype("${${TARGET}-APP_MIME_SIG}" SUBTYPE)

	add_custom_target(
		"${TARGET}-generate-en.catkeys"
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/locales
		COMMAND sh -c "${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS} -I$<JOIN:$<TARGET_PROPERTY:${TARGET},INCLUDE_DIRECTORIES>, -I> -DB_COLLECTING_CATKEYS -DHAIKU_ENABLE_I18N -E ${CMAKE_CURRENT_SOURCE_DIR}/$<JOIN:$<TARGET_PROPERTY:${TARGET},SOURCES>, ${CMAKE_CURRENT_SOURCE_DIR}/> >${TARGET}.cpp.i"
		COMMAND sh -c "grep -av '^#' ${TARGET}.cpp.i >${TARGET}.cpp.ck"
		COMMAND collectcatkeys -pvw -s ${SUBTYPE} -o $<TARGET_PROPERTY:${TARGET},SOURCE_DIR>/locales/en.catkeys ${TARGET}.cpp.ck
		COMMAND ${CMAKE_COMMAND} -E rm ${TARGET}.cpp.i ${TARGET}.cpp.ck
		DEPENDS $<TARGET_PROPERTY:${TARGET},SOURCES>
		VERBATIM
		COMMAND_EXPAND_LISTS
		COMMENT "Generating locales/en.catkeys for ${TARGET}")

	add_dependencies("catkeys" "${TARGET}-generate-en.catkeys")

endfunction()


#
#	Compile catkeys files into binary catalog files for a target
#
function(haiku_compile_catalogs TARGET)

	haiku_get_app_mime_subtype("${${TARGET}-APP_MIME_SIG}" SUBTYPE)

	if(DEFINED HAIKU_CATALOG_BUILD_DIR)
		set(catalogspath "${HAIKU_CATALOG_BUILD_DIR}/locale/catalogs/${SUBTYPE}")
	else()
		set(catalogspath "${CMAKE_CURRENT_BINARY_DIR}/locale/catalogs/${SUBTYPE}")
	endif()

	foreach(lang ${ARGN})
		set(catalogoutput "${catalogspath}/${lang}.catalog")
		set(catkeyspath "${CMAKE_CURRENT_SOURCE_DIR}/locales/${lang}.catkeys")

		add_custom_command(
			OUTPUT ${catalogoutput}
			COMMAND "${CMAKE_COMMAND}" "-E" "make_directory" "${catalogspath}"
			COMMAND "linkcatkeys" "-o" "${catalogoutput}" "-s" "${${TARGET}-APP_MIME_SIG}" "-l" "${lang}" "${catkeyspath}"
			DEPENDS ${catkeyspath}
			COMMENT "Compiling ${lang}.catalog for ${TARGET}")

		add_custom_target("${TARGET}-${lang}.catalog" DEPENDS ${catalogoutput})
		add_dependencies("catalogs" "${TARGET}-${lang}.catalog")
	endforeach()

endfunction()


#
#	Compile and bind catkeys files directly into the target executable as resources
#
function(haiku_bind_catalogs TARGET)

	foreach(lang ${ARGN})
		set(catkeyspath "${CMAKE_CURRENT_SOURCE_DIR}/locales/${lang}.catkeys")

		add_custom_target(
			"${TARGET}-bind-${lang}.catalog"
			COMMAND "linkcatkeys" "-o" "$<TARGET_FILE:${TARGET}>" "-s" "${${TARGET}-APP_MIME_SIG}" "-tr" "-l" "${lang}" "${catkeyspath}"
			DEPENDS ${catkeyspath} ${TARGET}
			COMMENT "Binding ${lang}.catalog to ${TARGET}")

		add_dependencies("bindcatalogs" "${TARGET}-bind-${lang}.catalog")
	endforeach()

endfunction()


#
#	Generate install rules for catalog files
#
function(haiku_install_catalogs TARGET)

	haiku_get_app_mime_subtype("${${TARGET}-APP_MIME_SIG}" SUBTYPE)

	if(DEFINED HAIKU_CATALOG_BUILD_DIR)
		set(catalogspath "${HAIKU_CATALOG_BUILD_DIR}/locale/catalogs/${SUBTYPE}")
	else()
		set(catalogspath "${CMAKE_CURRENT_BINARY_DIR}/locale/catalogs/${SUBTYPE}")
	endif()

	foreach(lang ${ARGN})
		install(FILES "${catalogspath}/${lang}.catalog"
			DESTINATION "${CMAKE_INSTALL_LOCALEDIR}/catalogs/${SUBTYPE}"
			COMPONENT "locales"
			EXCLUDE_FROM_ALL)
	endforeach()

endfunction()


#
#	Split mimetype at last / if needed
#
function(haiku_get_app_mime_subtype APP_MIME_SIG OUTVAR)

	# ensure that we have a shortened mimetype without the application/ prefix
	# find the last / and split the mime string
	string(FIND "${${TARGET}-APP_MIME_SIG}" "/" SUBPOS REVERSE)
	if("${SUBPOS}" EQUAL "-1")
		set("${OUTVAR}" "${${TARGET}-APP_MIME_SIG}" PARENT_SCOPE)
		return()
	endif()

	math(EXPR SUBPOS "${SUBPOS}+1")
	string(SUBSTRING "${${TARGET}-APP_MIME_SIG}" "${SUBPOS}+1" "-1" SUBTYPE)
	set("${OUTVAR}" "${SUBTYPE}" PARENT_SCOPE)

endfunction()
