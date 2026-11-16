cmake_minimum_required(VERSION 3.16)
project(unicomuse VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(unicomuse src/ucomuse.c)
add_library(unicomuse::unicomuse ALIAS unicomuse)
set_target_properties(unicomuse PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
target_include_directories(unicomuse
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)
if(NOT MSVC)
    target_compile_options(unicomuse PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(WIN32)
    target_sources(unicomuse PRIVATE src/platform_windows.c)
elseif(APPLE)
    target_sources(unicomuse PRIVATE src/platform_macos.c)
    target_link_libraries(unicomuse PRIVATE
        "-framework CoreGraphics"
        "-framework ApplicationServices"
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(unicomuse PRIVATE src/platform_linux.c)
    find_package(X11 QUIET)
    if(X11_FOUND AND TARGET X11::Xtst)
        target_compile_definitions(unicomuse PRIVATE UCOMUSE_HAVE_X11=1)
        target_link_libraries(unicomuse PRIVATE X11::X11 X11::Xtst)
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    target_sources(unicomuse PRIVATE src/platform_freebsd.c)
    find_package(X11 REQUIRED)
    if(NOT TARGET X11::Xtst)
        message(FATAL_ERROR "FreeBSD backend requires X11 with XTEST (libXtst)")
    endif()
    target_compile_definitions(unicomuse PRIVATE UCOMUSE_HAVE_X11=1)
    target_link_libraries(unicomuse PRIVATE X11::X11 X11::Xtst)
else()
    message(FATAL_ERROR "Unsupported operating system: ${CMAKE_SYSTEM_NAME}")
endif()
if(UNIX AND NOT APPLE)
    target_link_libraries(unicomuse PRIVATE m)
endif()

include(GNUInstallDirs)
install(TARGETS unicomuse
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY include/unicomuse DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(CTest)
if(BUILD_TESTING)
    add_executable(unicomuse_api_test
        tests/fake_platform.c
        tests/test_api.c
        src/ucomuse.c
    )
    target_include_directories(unicomuse_api_test PRIVATE include src tests)
    if(NOT MSVC)
        target_compile_options(unicomuse_api_test PRIVATE
            -Wall -Wextra -Wpedantic
        )
    endif()
    if(UNIX AND NOT APPLE)
        target_link_libraries(unicomuse_api_test PRIVATE m)
    endif()
    add_test(NAME unicomuse_api_test COMMAND unicomuse_api_test)
endif()
