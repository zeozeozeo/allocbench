project(tcmalloc LANGUAGES CXX ASM)

include(./tcmalloc_files.cmake)

add_library(tcmalloc ${TCMALLOC_FILES} ${TCMALLOC_INTERNAL_FILES})

# Is a certain certain cxx standard required?
# does absl already carry the requirement?

target_include_directories(tcmalloc PUBLIC .)
target_link_libraries(tcmalloc PUBLIC
    absl::base
    absl::str_format_internal
    absl::strings
    absl::synchronization
    absl::time
    absl::log_internal_message
    absl::raw_hash_set
    absl::hash
    absl::status
    absl::statusor
)

if(${__tcmalloc_proto})
    # run protoc
    target_link_libraries(tcmalloc PUBLIC protobuf tcmallo-proto-bindings)
endif()

if(${__tcmalloc_test})
    target_link_libraries(tcmalloc PRIVATE gtest benchmark gmock)
endif()

add_library(tmalloc::tmalloc ALIAS tcmalloc)
