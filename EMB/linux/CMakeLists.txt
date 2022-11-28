cmake_minimum_required(VERSION 3.1..3.20)

project(EmbarkWorms VERSION 1.0 LANGUAGES CXX)

if (WIN32)
    message(FATAL_ERROR "This CMakeLists.txt is only intended for use in Linux, use the provided visual studio project files instead!")
endif()

find_package(X11 REQUIRED)
find_package(ALSA REQUIRED)
find_package(Threads REQUIRED)

if (${CMAKE_VERSION} VERSION_LESS "3.20.0")
    message(WARNING "Your CMake version does not support automatic import of xcb and xkbcommon libraries, trying to link anyway, you may need to install extra libs (xcb, xcb-util, xcb-image xkbcommon, xkbcommon-X11")
    set(X_EXTRA_LIBS xcb-util xcb-image xkbcommon xkbcommon-x11)
else()
    # cmake doesnt know about xcb-image, let's hope for the best
    set(X_EXTRA_LIBS X11::xcb_util xcb-image X11::xkbcommon X11::xkbcommon_X11)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin)

add_library(WormsLib 
    ../WormsLib/TinyPixelNoise.cpp
    ../WormsLib/WormsAudioHelp.cpp  
    ../WormsLib/WormsAudioHelp.h  
    ../WormsLib/Worms.cpp  
    ../WormsLib/Worms.h  
    ../WormsLib/WormsVideoHelp.cpp
    ../WormsLib/WormsVideoHelp.h
)
target_link_libraries(WormsLib PUBLIC X11::xcb X11::xcb_icccm ${X_EXTRA_LIBS} ALSA::ALSA Threads::Threads)
target_include_directories(WormsLib PUBLIC ${PROJECT_SOURCE_DIR}/..)

add_executable(Worms
    ../WormsGame/Game.cpp
    ../WormsGame/WormAI.cpp
)

target_link_libraries(Worms WormsLib)
