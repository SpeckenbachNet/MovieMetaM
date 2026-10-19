cmake_minimum_required(VERSION 3.16)

set(PROJECT_VERSION "0.3.20")
set(ORGANIZATION_NAME "SpeckenbachNet")
set(ORGANIZATION_DOMAIN "net.speckenbach")

project(
    MovieMetaM
    VERSION ${PROJECT_VERSION}
    LANGUAGES CXX
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Network Svg SvgWidgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Svg SvgWidgets LinguistTools)

# TMDB API read access token: kept out of git in tmdb_token.txt
set(TMDB_TOKEN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/tmdb_token.txt")
set(TMDB_TOKEN "")
if(EXISTS "${TMDB_TOKEN_FILE}")
    file(READ "${TMDB_TOKEN_FILE}" TMDB_TOKEN)
    string(STRIP "${TMDB_TOKEN}" TMDB_TOKEN)
    # Reconfigure when the token changes
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${TMDB_TOKEN_FILE}")
else()
    message(WARNING "tmdb_token.txt not found: the movie search will not work")
endif()

set(TS_FILES
    i18n/${PROJECT_NAME}_en.ts
    i18n/${PROJECT_NAME}_de.ts
)

set(PROJECT_SOURCES
        main.cpp
        mainwin.cpp
        mainwin.h
        mainwin.ui
)

qt_add_executable(${PROJECT_NAME}
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    Icons.qrc
    Images.qrc
    Help.qrc
    initialize.cpp
    setupdialog.h
    setupdialog.cpp
    imagelabelm.h
    imagelabelm.cpp
    tagwidget.h
    tagwidget.cpp
    tableviewm.h
    tableviewm.cpp
    moviedata.h
    moviedata.cpp
    mediainfom.h
    mediainfom.cpp
    toolbarm.h
    toolbarm.cpp
    searchfield.h
    searchfield.cpp
    themecolors.h
    twolinedelegate.h
    twolinedelegate.cpp
    posterpreview.h
    posterpreview.cpp
    helppage.h
    helppage.cpp
)

# Windows: icon of the .exe and version in the file properties
if(WIN32)
    if(PROJECT_VERSION_PATCH STREQUAL "")
        set(PROJECT_VERSION_PATCH 0)   # e.g. "0.4": the version info needs all three numbers
    endif()
    configure_file(app.rc.in ${CMAKE_CURRENT_BINARY_DIR}/app.rc @ONLY)
    target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/app.rc)
endif()

# macOS: icon of the app bundle
if(APPLE)
    set(APP_ICON_MACOS ${CMAKE_CURRENT_SOURCE_DIR}/ressources/icons/app_icon.icns)
    set_source_files_properties(${APP_ICON_MACOS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(${PROJECT_NAME} PRIVATE ${APP_ICON_MACOS})
endif()

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        APP_VERSION="${PROJECT_VERSION}"
        APP_NAME="${PROJECT_NAME}"
        COMPANY_NAME="${ORGANIZATION_NAME}"
        TMDB_TOKEN="${TMDB_TOKEN}"
)

# The .qm files are compiled and embedded into the app as :/i18n/<name>.qm
# ("update_translations" target: collects all tr() texts into the .ts files)
qt_add_translations(${PROJECT_NAME}
    TS_FILES ${TS_FILES}
    RESOURCE_PREFIX "/i18n"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Svg   # SVG icons (tinted toolbar icons, window icon)
    Qt${QT_VERSION_MAJOR}::SvgWidgets   # TMDB logo that shrinks with the search column
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER "${ORGANIZATION_DOMAIN}.${PROJECT_NAME}"
    MACOSX_BUNDLE_ICON_FILE app_icon.icns
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(${PROJECT_NAME})
