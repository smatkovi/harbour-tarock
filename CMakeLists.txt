# Sailfish OS build of harbour-tarock (Qt 5.6, Silica). The Android build is
# android/CMakeLists.txt; both compile the same sources from src/ and the same
# shared QML from qml-common/.
cmake_minimum_required(VERSION 3.6)
project(harbour-tarock LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Core DBus LinguistTools Network Qml Quick)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SAILFISHAPP REQUIRED sailfishapp)

# The Qt-free rule core (src/core/…, docs/design.md §4). Globbed rather than
# listed: the rule files, the profiles and the AI land one by one over M1..M6,
# and neither a file that is still missing nor one that is added may break the
# build of everything else. Both the RPM and the Android package configure from
# a clean tree, so the glob is evaluated on every real build; after adding a
# file in a working tree, re-run cmake.
file(GLOB TAROCK_CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/profiles/*.cpp
)
list(SORT TAROCK_CORE_SOURCES)

# The QML facade of the core (docs/design.md §5); main.cpp installs it as the
# context property `tarockEngine`.
# Everything in src/ except main.cpp: the facade, the learning mode and its
# texts. Globbed for the same reason as the core, so a new file is picked up.
file(GLOB TAROCK_ENGINE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
# Der Tisch über Netz und Bluetooth (docs/design.md §8), aus harbour-snapszer
# übernommener Transport plus das Protokoll darüber.
file(GLOB TAROCK_NET_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/net/*.cpp)
list(APPEND TAROCK_ENGINE_SOURCES ${TAROCK_NET_SOURCES})
list(REMOVE_ITEM TAROCK_ENGINE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
list(SORT TAROCK_ENGINE_SOURCES)

add_executable(harbour-tarock
    src/main.cpp
    ${TAROCK_CORE_SOURCES}
    ${TAROCK_ENGINE_SOURCES}
)

target_include_directories(harbour-tarock PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SAILFISHAPP_INCLUDE_DIRS}
)

target_link_libraries(harbour-tarock
    Qt5::Core Qt5::DBus Qt5::Network Qt5::Qml Qt5::Quick ${SAILFISHAPP_LIBRARIES}
)

# The QML itself is not compiled in: rpm/harbour-tarock.spec copies sailfish/*.qml,
# sailfish/qmldir and qml-common/*.qml into /usr/share/harbour-tarock/qml, so
# the platform types and the shared table end up in one directory and the
# qmldir declares them all.

# German is the interface language of the rule specifications and of the app;
# the source strings are English.
set(TAROCK_TRANSLATIONS
    translations/harbour-tarock-de.ts
    translations/harbour-tarock-hu.ts
)
qt5_add_translation(TAROCK_QM_FILES ${TAROCK_TRANSLATIONS})
add_custom_target(harbour-tarock-translations ALL DEPENDS ${TAROCK_QM_FILES})
add_dependencies(harbour-tarock harbour-tarock-translations)

# The rule, lesson and LAN tests of docs/design.md §11 are added to this list
# as the milestones that introduce them land; TAROCK_DATA_DIR points them at
# the lessons and expected ledgers in the source tree. A test whose source is
# not written yet is skipped instead of failing the configure step.
option(TAROCK_BUILD_TESTS "Build rule, lesson and LAN tests" OFF)
if(TAROCK_BUILD_TESTS)
    enable_testing()
    foreach(t test_cards test_hands test_learn test_legality test_lessons test_profile_tapp test_qmlbridge test_scoring test_selfplay test_settings test_skeleton lan_tarock4 lan_tarock5)
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/${t}.cpp)
            add_executable(${t} tests/${t}.cpp ${TAROCK_CORE_SOURCES} ${TAROCK_ENGINE_SOURCES})
            target_include_directories(${t} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
            target_compile_definitions(${t} PRIVATE TAROCK_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
            # Qt5::Qml for test_qmlbridge, which evaluates the QML side of the
            # engine contract; the others do not use it and do not mind.
            target_link_libraries(${t} Qt5::Core Qt5::DBus Qt5::Gui Qt5::Network Qt5::Qml)
            add_test(NAME ${t} COMMAND ${t})
            # Derselbe Vierertisch noch einmal mit dem ungarischen Blatt: 42
            # Karten, neun auf der Hand, eigene Ansagen. Was am Netztisch nur
            # für die österreichischen Regeln geprüft wird, ist nicht geprüft.
            if(t STREQUAL "lan_tarock4")
                add_test(NAME lan_tarock4_hu COMMAND lan_tarock4 HU-ILLU-ITVB-2019)
            endif()
        else()
            message(STATUS "harbour-tarock: tests/${t}.cpp is not there yet, skipping it")
        endif()
    endforeach()
endif()
