cmake_minimum_required(VERSION 3.16)
project(Djvureader VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Gui)

find_package(djvulibre QUIET)
if(NOT djvulibre_FOUND)
    message(WARNING "libdjvulibre not found, building internal version")
endif()

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/djvuview.cpp
    src/pagerenderer.cpp
)

set(HEADERS
    src/mainwindow.h
    src/djvuview.h
    src/pagerenderer.h
)

add_executable(djvureader ${SOURCES} ${HEADERS})

target_link_libraries(djvureader
    Qt5::Widgets
    Qt5::Gui
)

if(djvulibre_FOUND)
    target_link_libraries(djvureader djvulibre)
endif()

install(TARGETS djvureader
    RUNTIME DESTINATION bin
)

include(CPack)

set(CPACK_PACKAGE_NAME "DJVU Reader")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "DJVU Reader Team")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cross-platform DJVU document viewer")
set(CPACK_GENERATOR "TGZ")

if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_INSTALLER_ICON "${CMAKE_SOURCE_DIR}/resources/icons/app.ico")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/resources/icons/app.ico")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_OSX_BUNDLE_ICON "${CMAKE_SOURCE_DIR}/resources/icons/app.icns")
    set(CPACK_OSX_BUNDLE_GUI_IDENTIFIER "com.djvureader.app")
else()
    set(CPACK_GENERATOR "TGZ")
endif()