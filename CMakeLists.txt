if (NOT WIN32)
	return()
endif()

project(HeroesSceneSwitch)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
	
if(WIN32)
	set(HeroesSceneSwitch_HEADERS
		${HeroesSceneSwitch_HEADERS}
		HeroesSceneSwitch.h
		settingsdialog.h
	)
	set(HeroesSceneSwitch_SOURCES
		${HeroesSceneSwitch_SOURCES}
		HeroesSceneSwitch.cpp
		settingsdialog.cpp
	)
	set(HeroesSceneSwitch_UI
		${HeroesSceneSwitch_UI}
		settingsdialog.ui
	)
endif()

qt5_wrap_ui(HeroesSceneSwitch_UI_HEADERS
	${HeroesSceneSwitch_UI}
)

add_library(HeroesSceneSwitch MODULE
	${HeroesSceneSwitch_SOURCES}
	${HeroesSceneSwitch_HEADERS}
	${HeroesSceneSwitch_UI_HEADERS}
)

target_link_libraries(HeroesSceneSwitch
	${HeroesSceneSwitch_DEPS}
	obs-frontend-api
	libobs
	Qt5::Widgets
	Qt5::Core
	Qt5::Gui
)