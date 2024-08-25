cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("hx3dit")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

add_executable(hx3dit
    source/hx3dit.cpp
    source/editor.cpp
    source/editor.hpp
    source/input.cpp
    source/input.hpp
    source/output.cpp
    source/output.hpp
    )

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET hx3dit PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
	target_compile_options(hx3dit PRIVATE /W4 /WX)
else()
	target_compile_options(hx3dit PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_compile_definitions(hx3dit PRIVATE
    $<$<CONFIG:Debug>:
        _DEBUG
    >
)

target_precompile_headers(hx3dit
    PRIVATE source/hx3dit.hpp)
