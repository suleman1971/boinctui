cmake_minimum_required(VERSION 3.22)
project(boinctui VERSION 2.7.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(WITHOUT_GNUTLS "Disable GnuTLS support" OFF)
set(BOINC_DIR "/var/lib/boinc-client" CACHE STRING "Specify the BOINC data directory")

add_compile_definitions(ENABLEMOUSE NCURSES_INTERNALS BOINC_DIR="${BOINC_DIR}")

find_package(PkgConfig REQUIRED)
pkg_check_modules(NCURSESW REQUIRED ncursesw)
pkg_check_modules(PANELW REQUIRED panelw)
pkg_check_modules(FORMW REQUIRED formw)
pkg_check_modules(MENUW REQUIRED menuw)
pkg_check_modules(EXPAT REQUIRED expat)

if(WITHOUT_GNUTLS)
    pkg_check_modules(OPENSSL REQUIRED openssl)
    add_compile_definitions(HAVE_OPENSSL)
else()
    pkg_check_modules(GNUTLS REQUIRED gnutls)
endif()

file(GLOB SRC_FILES src/*.cpp)

add_executable(boinctui ${SRC_FILES})

target_include_directories(boinctui PRIVATE
    ${NCURSESW_INCLUDE_DIRS}
    ${PANELW_INCLUDE_DIRS}
    ${FORMW_INCLUDE_DIRS}
    ${MENUW_INCLUDE_DIRS}
    ${EXPAT_INCLUDE_DIRS}
)

target_link_libraries(boinctui PRIVATE
    ${NCURSESW_LIBRARIES}
    ${PANELW_LIBRARIES}
    ${FORMW_LIBRARIES}
    ${MENUW_LIBRARIES}
    ${EXPAT_LIBRARIES}
    pthread
)

if(WITHOUT_GNUTLS)
    target_include_directories(boinctui PRIVATE ${OPENSSL_INCLUDE_DIRS})
    target_link_libraries(boinctui PRIVATE ${OPENSSL_LIBRARIES})
else()
    target_include_directories(boinctui PRIVATE ${GNUTLS_INCLUDE_DIRS})
    target_link_libraries(boinctui PRIVATE ${GNUTLS_LIBRARIES})
endif()

install(TARGETS boinctui RUNTIME DESTINATION bin)
