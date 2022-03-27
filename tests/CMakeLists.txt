cmake_minimum_required(VERSION 3.12)
project(mcp2515_tests C CXX)

#include(CTest)
enable_testing()

set(CTEST_OUTPUT_ON_FAILURE ON)
set(library_dir "${CMAKE_SOURCE_DIR}/../libmcp2515")


# clang-tidy
if (NOT NO_STATIC_ANALYSIS)
  find_program(clang_tidy NAMES clang-tidy)
  if (NOT clang_tidy)
    message(FATAL_ERROR "Could not locate clang-tidy")
  endif ()
  message(STATUS "Using clang-tidy: ${clang_tidy}")
  set(CMAKE_C_CLANG_TIDY ${clang_tidy})
  set(CMAKE_CXX_CLANG_TIDY ${clang_tidy})
endif ()


# clang-format
find_program(clang_format NAMES clang-format)
if (NOT clang_format)
    message(STATUS "Could not locate clang-format")
else ()
    file(GLOB format_files ${library_dir}/*.[ch] ${CMAKE_SOURCE_DIR}/*.[ch]pp)
    message(STATUS "Using clang-format: ${clang_format}; files: ${format_files}")
    add_custom_target(format COMMAND ${clang_format} -i -fallback-style=none -style=file --verbose ${format_files})
endif ()


set(CMAKE_CXX_STANDARD 17)
set(CXX_EXTENSIONS OFF)
add_compile_options(
  -Wall 
  -Wextra 
  -Werror 
  -pedantic 
  -fstrict-aliasing 
  -Wdouble-promotion 
  -Wswitch-enum 
  -Wfloat-equal 
  -Wundef
  -Wconversion 
  -Wtype-limits 
  -Wsign-conversion 
  -Wcast-align
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual -Wnon-virtual-dtor -Wsign-promo")

include_directories(catch ${library_dir})
add_definitions(-DCATCH_CONFIG_FAST_COMPILE=1 -DCATCH_CONFIG_ENABLE_ALL_STRINGMAKERS=1)

set(common_sources catch/main.cpp ${library_dir}/driver_mcp2515.c)

function(generate_test name files defs compileFlags linkFlags standard)
  add_executable(${name} ${common_sources} ${files})
  target_compile_definitions(${name} PUBLIC ${defs})
  target_link_libraries(${name} pthread)
  set_target_properties(${name} PROPERTIES 
    COMPILE_FLAGS ${compileFlags}
    LINK_FLAGS ${linkFlags}
    C_STANDARD ${standard}
  )
  add_test("run_${name}" "${name}" --rng-seed time)
endfunction()

generate_test("x64_c99" 
  "unittest.cpp" 
  "" 
  "-Wno-missing-declarations -m64" 
  "-m64" 
  "99"
)

generate_test("x64_c11" 
  "unittest.cpp" 
  "" 
  "-Wno-missing-declarations -m64" 
  "-m64" 
  "11"
)
