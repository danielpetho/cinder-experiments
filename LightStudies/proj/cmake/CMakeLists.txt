cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( LightStudies )
set( APP_NAME "${PROJECT_NAME}App" )

set( CMAKE_BUILD_TYPE "Release" )
#set( CMAKE_BUILD_TYPE "Debug" )

if ( APPLE )
get_filename_component( CINDER_PATH "/Users/gabor/projects/cinder/" ABSOLUTE )
elseif ( UNIX )
get_filename_component( CINDER_PATH "/home/gabor/projects/cinder/" ABSOLUTE )
endif()

get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( APP_INCS
	${APP_PATH}/include
	${APP_PATH}/src/
)

set( APP_SRCS
	${APP_PATH}/src/DirectionalLight.cpp
	${APP_PATH}/src/LightStudiesApp.cpp
	${APP_PATH}/src/Material.cpp
	${APP_PATH}/src/PointLight.cpp
)

ci_make_app(
	APP_NAME ${APP_NAME}
	INCLUDES ${APP_INCS}
	SOURCES ${APP_SRCS}
	CINDER_PATH ${CINDER_PATH}
)

get_target_property( OUTPUT_DIR ${APP_NAME} RUNTIME_OUTPUT_DIRECTORY )

if ( APPLE )
	add_custom_target( run
		COMMAND open ${OUTPUT_DIR}/${APP_NAME}.app
		DEPENDS ${OUTPUT_DIR}/${APP_NAME}.app/Contents/MacOS/${APP_NAME}
		WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
	)
elseif ( UNIX )
	add_custom_target( run
		COMMAND ${OUTPUT_DIR}/${APP_NAME}
		DEPENDS ${OUTPUT_DIR}/${APP_NAME}
		WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
	)
endif()
