if (NOT WIN32)
	return()
endif()

project(TestPlugin)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
	
set(TestPlugin_HEADERS
	${TestPlugin_HEADERS}
	TestPlugin.h
)
set(TestPlugin_SOURCES
	${TestPlugin_SOURCES}
	TestPlugin.cpp
)

add_library(TestPlugin MODULE
	${TestPlugin_SOURCES}
	${TestPlugin_HEADERS}
)

target_link_libraries(TestPlugin
	${TestPlugin_DEPS}
	libobs
	obs-frontend-api
	Qt5::Widgets
	Qt5::Core
)

