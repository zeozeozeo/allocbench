cmake_minimum_required(VERSION 3.16)
project(allocbench C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
include(FetchContent)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
    set(CMAKE_C_FLAGS_RELEASE "/O2 /DNDEBUG")
    set(CMAKE_C_FLAGS_DEBUG "/Od /DDEBUG")
else()
    set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
    set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG")
endif()

option(BUILD_WITH_RPMALLOC "Build with rpmalloc allocator" ON)
option(BUILD_WITH_MIMALLOC "Build with mimalloc allocator" ON)
option(BUILD_WITH_JEMALLOC "Build with jemalloc allocator (Unix only)" OFF)
option(BUILD_WITH_TCMALLOC "Build with tcmalloc allocator" OFF)

add_library(allocbench_core STATIC
    src/benchmark.c
    src/allocator_api.c
    src/metrics/timer.c
    src/metrics/memory_stats.c
    src/metrics/results.c
    src/data_structures/vector.c
    src/data_structures/linked_list.c
    src/data_structures/binary_tree.c
    src/data_structures/hash_table.c
    src/data_structures/stack.c
    src/benchmarks/micro_benchmarks.c
    src/benchmarks/data_structure_benchmarks.c
    src/benchmarks/threaded_benchmarks.c
    src/benchmarks/fragmentation_benchmarks.c
)

target_include_directories(allocbench_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/data_structures
    ${CMAKE_CURRENT_SOURCE_DIR}/src/metrics
    ${CMAKE_CURRENT_SOURCE_DIR}/src/benchmarks
)

find_package(Threads REQUIRED)
target_link_libraries(allocbench_core PUBLIC Threads::Threads)

if(WIN32)
    target_link_libraries(allocbench_core PUBLIC psapi)
endif()

if(BUILD_WITH_RPMALLOC)
    add_library(rpmalloc STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/rpmalloc/rpmalloc/rpmalloc.c
    )
    target_include_directories(rpmalloc PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/rpmalloc/rpmalloc
    )
    target_compile_definitions(rpmalloc PRIVATE ENABLE_OVERRIDE=0)
    target_compile_definitions(allocbench_core PUBLIC HAVE_RPMALLOC=1)
    target_link_libraries(allocbench_core PUBLIC rpmalloc)
    message(STATUS "Building with rpmalloc")
endif()

if(BUILD_WITH_MIMALLOC)
    set(MI_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(MI_BUILD_STATIC ON CACHE BOOL "" FORCE)
    set(MI_BUILD_OBJECT OFF CACHE BOOL "" FORCE)
    set(MI_OVERRIDE OFF CACHE BOOL "" FORCE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/mimalloc mimalloc_build)
    target_compile_definitions(allocbench_core PUBLIC HAVE_MIMALLOC=1)
    target_link_libraries(allocbench_core PUBLIC mimalloc-static)
    message(STATUS "Building with mimalloc")
endif()

if(BUILD_WITH_JEMALLOC AND NOT WIN32)
    include(ExternalProject)
    set(JEMALLOC_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/jemalloc)
    set(JEMALLOC_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/jemalloc_build)
    set(JEMALLOC_STATIC_LIB ${JEMALLOC_BUILD_DIR}/lib/libjemalloc.a)

    ExternalProject_Add(jemalloc_external
        SOURCE_DIR ${JEMALLOC_SOURCE_DIR}
        CONFIGURE_COMMAND ${JEMALLOC_SOURCE_DIR}/autogen.sh
            --prefix=${JEMALLOC_BUILD_DIR}
            --disable-shared
            --enable-static
            --with-jemalloc-prefix=je_
        BUILD_COMMAND make -j4
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND make install
        BUILD_BYPRODUCTS ${JEMALLOC_STATIC_LIB}
    )

    add_library(jemalloc STATIC IMPORTED GLOBAL)
    set_property(TARGET jemalloc PROPERTY IMPORTED_LOCATION ${JEMALLOC_STATIC_LIB})
    target_include_directories(jemalloc INTERFACE ${JEMALLOC_SOURCE_DIR}/include)
    add_dependencies(jemalloc jemalloc_external)

    target_compile_definitions(allocbench_core PUBLIC HAVE_JEMALLOC=1)
    target_link_libraries(allocbench_core PUBLIC jemalloc pthread dl)
    message(STATUS "Building with jemalloc")
endif()

if(BUILD_WITH_TCMALLOC)

    FetchContent_Declare(abseil_cpp
        GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
    )
    set(ABSL_PROPAGATE_CXX_STD ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(abseil_cpp)

    include(tcmalloc/CMakeLists.txt)
    target_compile_definitions(allocbench_core PUBLIC HAVE_TCMALLOC=1)
    target_link_libraries(allocbench_core PUBLIC tcmalloc)
    message(STATUS "Building with tcmalloc")
endif()

add_executable(allocbench src/main.c)
target_link_libraries(allocbench PRIVATE allocbench_core)

add_executable(allocbench_system src/main.c)
target_compile_definitions(allocbench_system PRIVATE USE_SYSTEM_ALLOC=1)
target_link_libraries(allocbench_system PRIVATE allocbench_core)

message(STATUS "")
message(STATUS "=== allocbench Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "rpmalloc: ${BUILD_WITH_RPMALLOC}")
message(STATUS "mimalloc: ${BUILD_WITH_MIMALLOC}")
message(STATUS "jemalloc: ${BUILD_WITH_JEMALLOC}")
message(STATUS "tcmalloc: ${BUILD_WITH_TCMALLOC}")
message(STATUS "================================")
