# Android build of harbour-tarock (Qt 6.9+, arm64). The Sailfish OS build is
# the CMakeLists.txt one directory up; both compile the same sources from
# src/ and the same shared QML from qml-common/.
cmake_minimum_required(VERSION 3.21)
project(tarock VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.9 REQUIRED COMPONENTS Core Gui Network Qml Quick QuickControls2)
qt_standard_project_setup(REQUIRES 6.9)

set(TAROCK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")

# The Qt-free rule core (src/core/…, docs/design.md §4); this list mirrors
# TAROCK_CORE_SOURCES of the Sailfish build one directory up.
set(TAROCK_CORE_SOURCES
    ${TAROCK_ROOT}/src/core/Card.cpp
    ${TAROCK_ROOT}/src/core/Deck.cpp
    ${TAROCK_ROOT}/src/core/Action.cpp
    ${TAROCK_ROOT}/src/core/Reason.cpp
    ${TAROCK_ROOT}/src/core/RuleProfile.cpp
    ${TAROCK_ROOT}/src/core/profiles/AtKrOoe2023.cpp
    ${TAROCK_ROOT}/src/core/profiles/HuIlluItvb2019.cpp
    ${TAROCK_ROOT}/src/core/Scoring.cpp
)

qt_add_executable(tarock
    main.cpp
    ScreenHelper.h
    ${TAROCK_CORE_SOURCES}
    ${TAROCK_ROOT}/src/TarockEngine.cpp
    ${TAROCK_ROOT}/src/TarockEngine.h
)
target_include_directories(tarock PRIVATE ${TAROCK_ROOT}/src)
target_link_libraries(tarock PRIVATE
    Qt6::Core Qt6::Gui Qt6::Network Qt6::Qml Qt6::Quick Qt6::QuickControls2)

set_source_files_properties(qml/Theme.qml qml/Style.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

# The table and every panel on it are shared with the Sailfish OS build; the
# list grows with each milestone (docs/design.md §2).
set(TAROCK_SHARED_QML
    ${TAROCK_ROOT}/qml-common/TarockTable.qml
)
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
        qml/TableButton.qml
        qml/SubPage.qml
        qml/TablePage.qml
        qml/AboutPage.qml
        ${TAROCK_SHARED_QML}
)

# Android has no /usr/share, so everything the app reads at runtime becomes a
# resource. Card decks, lessons and rule texts follow in M6/M8.
qt_add_resources(tarock tarock_data
    PREFIX "/"
    BASE "${TAROCK_ROOT}"
    FILES "${TAROCK_ROOT}/sailfish/icons/icon-256.png"
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
    QT_ANDROID_VERSION_CODE 1
    QT_ANDROID_VERSION_NAME "0.1.0"
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
