﻿include(FetchContent)

add_executable (chip8 "chip8-emulator.cpp" "Chip8.cpp" "Chip8.hpp" "Platform.cpp" "Platform.hpp")
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

target_link_libraries(chip8 PRIVATE sfml-graphics)

if(WIN32)
    add_custom_command(
        TARGET chip8
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:chip8>
        VERBATIM)
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET chip8 PROPERTY CXX_STANDARD 20)
endif()