cmake_minimum_required(VERSION 3.16)

set(PROJECT_VERSION "0.3.1")

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

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Network)

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
   i18n/MovieMetaM_en.ts
   i18n/MovieMetaM_de.ts
)

set(PROJECT_SOURCES
        main.cpp
        mainwin.cpp
        mainwin.h
        mainwin.ui
        setupdialog.h
        setupdialog.cpp
)

set(QM_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/i18n")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MovieMetaM
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        Icons.qrc
        translation.qrc
        Images.qrc
        imagelabelm.h
        imagelabelm.cpp
        tagwidget.h
        tagwidget.cpp
        initialize.cpp
        Html.qrc
        htmltemplates.h
        tableviewm.cpp
        tableviewm.h
        moviedata.cpp
        moviedata.h
        mediainfom.cpp
        mediainfom.h
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET MovieMetaM APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_add_translations(MovieMetaM
        TS_FILES ${TS_FILES}
        QM_OUTPUT_DIRECTORY "${QM_OUTPUT_DIR}"
    )
else()
    if(ANDROID)
        add_library(MovieMetaM SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(MovieMetaM
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(MovieMetaM
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Network
        #Qt${QT_VERSION_MAJOR}::WebEngineWidgets
)

target_include_directories(MovieMetaM
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(MovieMetaM
    PRIVATE
        PROJECT_VERSION="${PROJECT_VERSION}"
        PROJECT_NAME="${PROJECT_NAME}"
        TMDB_TOKEN="${TMDB_TOKEN}"
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.MovieMetaM)
endif()
set_target_properties(MovieMetaM PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS MovieMetaM
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MovieMetaM)
endif()
