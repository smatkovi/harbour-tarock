# Android build of harbour-tarock (Qt 6.9+, arm64). The Sailfish OS build is
# the CMakeLists.txt one directory up; both compile the same sources from
# src/ and the same shared QML from qml-common/.
cmake_minimum_required(VERSION 3.21)
project(tarock VERSION 0.6.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.9 REQUIRED COMPONENTS Core Gui Network Qml Quick QuickControls2)
qt_standard_project_setup(REQUIRES 6.9)

set(TAROCK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")

# The Qt-free rule core (src/core/…, docs/design.md §4); globbed exactly like
# TAROCK_CORE_SOURCES of the Sailfish build one directory up, so a file that
# one milestone adds is picked up by both builds at once and a file that is not
# written yet breaks neither.
file(GLOB TAROCK_CORE_SOURCES
    ${TAROCK_ROOT}/src/core/*.cpp
    ${TAROCK_ROOT}/src/core/profiles/*.cpp
)
list(SORT TAROCK_CORE_SOURCES)

# Everything in src/ except its main.cpp: the facade, the learning mode and
# its texts. Android has its own main.cpp next to this file.
file(GLOB TAROCK_ENGINE_SOURCES ${TAROCK_ROOT}/src/*.cpp)
list(REMOVE_ITEM TAROCK_ENGINE_SOURCES ${TAROCK_ROOT}/src/main.cpp)
# Der Tisch über das Netz (docs/design.md §8). BtLink.cpp kompiliert auf
# Android zur Absage -- eine App bekommt dort keine RFCOMM-Sockets --, der
# LAN-Weg funktioniert aber genauso wie auf den anderen Plattformen.
file(GLOB TAROCK_NET_SOURCES ${TAROCK_ROOT}/src/net/*.cpp)
list(APPEND TAROCK_ENGINE_SOURCES ${TAROCK_NET_SOURCES})
list(SORT TAROCK_ENGINE_SOURCES)

qt_add_executable(tarock
    main.cpp
    ScreenHelper.h
    ${TAROCK_CORE_SOURCES}
    ${TAROCK_ENGINE_SOURCES}
)
target_include_directories(tarock PRIVATE ${TAROCK_ROOT}/src)
target_link_libraries(tarock PRIVATE
    Qt6::Core Qt6::Gui Qt6::Network Qt6::Qml Qt6::Quick Qt6::QuickControls2)

# Theme and Style are the metrics, Prefs holds the choices that belong to the
# next match. sailfish/qmldir declares Style and Prefs the same way; Theme
# comes from Sailfish.Silica there.
set_source_files_properties(qml/Theme.qml qml/Style.qml qml/Prefs.qml
                            PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

# The table and every panel on it are shared with the Sailfish OS build. The
# whole directory is taken as it stands, so the panels of docs/design.md §2
# (TarockTable, SeatPanel, HandFan, TrickArea, BidBar, TalonPanel, DiscardTray,
# AnnouncePanel, ScoreSheet …) are picked up as they land, and one that is not
# written yet cannot break the configure step. The Sailfish package copies the
# same directory into /usr/share/harbour-tarock/qml.
file(GLOB TAROCK_SHARED_QML ${TAROCK_ROOT}/qml-common/*.qml)
list(SORT TAROCK_SHARED_QML)
# Without an alias the shared files would land under qml-common/ in the module
# and the imports would not find them next to the platform types.
foreach(file ${TAROCK_SHARED_QML})
    get_filename_component(name ${file} NAME)
    set_source_files_properties(${file} PROPERTIES QT_RESOURCE_ALIAS ${name})
endforeach()

qt_add_qml_module(tarock
    URI Tarock
    QML_FILES
        qml/Main.qml
        qml/Theme.qml
        qml/Style.qml
        qml/Prefs.qml
        qml/Card.qml
        qml/FlyingCard.qml
        qml/TableButton.qml
        qml/SubPage.qml
        qml/SectionLabel.qml
        qml/TextBlock.qml
        qml/MainPage.qml
        qml/NetPage.qml
        qml/TablePage.qml
        qml/ScorePage.qml
        qml/SettingsPage.qml
        qml/RulesPage.qml
        qml/GlossaryPage.qml
        qml/LearnPage.qml
        qml/LessonPage.qml
        qml/TutorialPage.qml
        qml/AboutPage.qml
        ${TAROCK_SHARED_QML}
)

# Android has no /usr/share, so everything the app reads at runtime becomes a
# resource. LearnEngine looks for the lessons under ":/lessons/<profile>".
# Die Kartenbilder (docs/design.md §10) liegen unter ":/decks/<deck>/<datei>".
file(GLOB TAROCK_DECK_FILES ${TAROCK_ROOT}/assets/decks/*/*)
foreach(file ${TAROCK_DECK_FILES})
    get_filename_component(name ${file} NAME)
    get_filename_component(dir ${file} DIRECTORY)
    get_filename_component(stem ${dir} NAME)
    set_source_files_properties(${file} PROPERTIES QT_RESOURCE_ALIAS "decks/${stem}/${name}")
endforeach()

file(GLOB TAROCK_LESSON_FILES ${TAROCK_ROOT}/assets/lessons/*/*.json)
foreach(file ${TAROCK_LESSON_FILES})
    get_filename_component(name ${file} NAME)
    get_filename_component(dir ${file} DIRECTORY)
    get_filename_component(stem ${dir} NAME)
    set_source_files_properties(${file} PROPERTIES QT_RESOURCE_ALIAS "lessons/${stem}/${name}")
endforeach()

qt_add_resources(tarock tarock_data
    PREFIX "/"
    BASE "${TAROCK_ROOT}"
    FILES "${TAROCK_ROOT}/sailfish/icons/icon-256.png" ${TAROCK_LESSON_FILES}
          ${TAROCK_DECK_FILES}
)

# The Android Qt package has no Linguist tools, so use lrelease of the host Qt.
find_program(TAROCK_LRELEASE NAMES lrelease-qt6 lrelease
             HINTS "${QT_HOST_PATH}/lib/qt6/bin" "${QT_HOST_PATH}/bin" REQUIRED)
set(TAROCK_QM_FILES)
foreach(lang de hu)
    set(ts "${TAROCK_ROOT}/translations/harbour-tarock-${lang}.ts")
    set(qm "${CMAKE_CURRENT_BINARY_DIR}/harbour-tarock-${lang}.qm")
    add_custom_command(OUTPUT "${qm}"
        COMMAND "${TAROCK_LRELEASE}" -silent "${ts}" -qm "${qm}"
        DEPENDS "${ts}"
        VERBATIM)
    set_source_files_properties("${qm}" PROPERTIES
        GENERATED TRUE
        QT_RESOURCE_ALIAS harbour-tarock-${lang}.qm)
    list(APPEND TAROCK_QM_FILES "${qm}")
endforeach()
qt_add_resources(tarock tarock_translations
    PREFIX "/i18n"
    FILES ${TAROCK_QM_FILES}
)

set_target_properties(tarock PROPERTIES
    QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/package"
    QT_ANDROID_PACKAGE_NAME "org.smatkovi.tarock"
    QT_ANDROID_APP_NAME "Tarock"
    QT_ANDROID_APP_ICON "@drawable/icon"
    QT_ANDROID_MIN_SDK_VERSION 28
    QT_ANDROID_TARGET_SDK_VERSION 36
    QT_ANDROID_VERSION_CODE 203
    QT_ANDROID_VERSION_NAME "0.2.3"
)

if(ANDROID)
    # Broadcast discovery needs a Wi-Fi multicast lock on many devices.
    qt_add_android_permission(tarock NAME android.permission.ACCESS_WIFI_STATE)
    qt_add_android_permission(tarock NAME android.permission.CHANGE_WIFI_MULTICAST_STATE)
    # Foreground service and Wi-Fi lock while a LAN game is hosted or played.
    qt_add_android_permission(tarock NAME android.permission.FOREGROUND_SERVICE)
    qt_add_android_permission(tarock NAME android.permission.FOREGROUND_SERVICE_SPECIAL_USE)
    qt_add_android_permission(tarock NAME android.permission.POST_NOTIFICATIONS)
    qt_add_android_permission(tarock NAME android.permission.WAKE_LOCK)
endif()
