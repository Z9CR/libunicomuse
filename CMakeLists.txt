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
        "-framework CoreFoundation"
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
install(FILES single_include/unicomuse.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

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

    add_executable(unicomuse_single_header_compile
        tests/single_header_compile.c
    )
    target_include_directories(unicomuse_single_header_compile PRIVATE
        single_include
    )
    if(APPLE)
        target_link_libraries(unicomuse_single_header_compile PRIVATE
            "-framework CoreFoundation"
            "-framework CoreGraphics"
            "-framework ApplicationServices"
        )
    elseif(UNIX)
        target_link_libraries(unicomuse_single_header_compile PRIVATE m)
    endif()
    if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
        target_link_libraries(unicomuse_single_header_compile PRIVATE
            X11::X11 X11::Xtst
        )
    endif()
    add_test(NAME unicomuse_single_header_compile
        COMMAND unicomuse_single_header_compile
    )

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND X11_FOUND AND
       TARGET X11::Xtst)
        add_executable(unicomuse_single_header_x11_compile
            tests/single_header_compile.c
        )
        target_include_directories(unicomuse_single_header_x11_compile PRIVATE
            single_include
        )
        target_compile_definitions(unicomuse_single_header_x11_compile PRIVATE
            UCOMUSE_USE_X11=1
        )
        target_link_libraries(unicomuse_single_header_x11_compile PRIVATE
            X11::X11 X11::Xtst m
        )
        add_test(NAME unicomuse_single_header_x11_compile
            COMMAND unicomuse_single_header_x11_compile
        )
    endif()
endif()

option(UCOMUSE_BUILD_EXAMPLES "Build libunicomuse examples" OFF)
if(UCOMUSE_BUILD_EXAMPLES)
    add_executable(unicomuse_example example/basic.c)
    target_link_libraries(unicomuse_example PRIVATE unicomuse::unicomuse)
endif()
