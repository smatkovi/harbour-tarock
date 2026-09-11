# Sailfish OS build of harbour-tarock (Qt 5.6, Silica). The Android build is
# android/CMakeLists.txt; both compile the same sources from src/ and the same
# shared QML from qml-common/.
cmake_minimum_required(VERSION 3.6)
project(harbour-tarock LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Core LinguistTools Network Qml Quick)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SAILFISHAPP REQUIRED sailfishapp)

# The Qt-free rule core (src/core/…, docs/design.md §4) is added in M1.
set(TAROCK_CORE_SOURCES
    src/core/Card.cpp
    src/core/Deck.cpp
    src/core/Action.cpp
    src/core/Reason.cpp
    src/core/RuleProfile.cpp
    src/core/profiles/AtKrOoe2023.cpp
    src/core/profiles/HuIlluItvb2019.cpp
)

add_executable(harbour-tarock
    src/main.cpp
    ${TAROCK_CORE_SOURCES}
    src/TarockEngine.cpp
)

target_include_directories(harbour-tarock PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SAILFISHAPP_INCLUDE_DIRS}
)

target_link_libraries(harbour-tarock
    Qt5::Core Qt5::Network Qt5::Qml Qt5::Quick ${SAILFISHAPP_LIBRARIES}
)

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
# the lessons and expected ledgers in the source tree.
option(TAROCK_BUILD_TESTS "Build rule, lesson and LAN tests" OFF)
if(TAROCK_BUILD_TESTS)
    enable_testing()
    foreach(t test_skeleton test_cards)
        add_executable(${t} tests/${t}.cpp ${TAROCK_CORE_SOURCES} src/TarockEngine.cpp)
        target_include_directories(${t} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
        target_compile_definitions(${t} PRIVATE TAROCK_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
        target_link_libraries(${t} Qt5::Core)
        add_test(NAME ${t} COMMAND ${t})
    endforeach()
endif()
