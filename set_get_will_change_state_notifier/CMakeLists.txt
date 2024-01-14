cmake_minimum_required(VERSION 3.10)

project(set_get_will_change_state_notifier_example
    VERSION 0.0.1
    DESCRIPTION ""
    LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

# Build the shared library
add_executable(set_get_will_change_state_notifier_example ${SOURCES})
target_include_directories(set_get_will_change_state_notifier_example PRIVATE /usr/local/include)

# Extract the compiler name without the path
get_filename_component(COMPILER_NAME "${CMAKE_C_COMPILER}" NAME)

function(split_string_into_list _input_string _output_list)
    string(REGEX REPLACE "[ ]+" ";" _split_list "${_input_string}")
    set(${_output_list} ${_split_list} PARENT_SCOPE)
endfunction()

# Import warning_flags.txt
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/../../flags/${COMPILER_NAME}/warning_flags.txt" WARNING_FLAGS_STRING)
split_string_into_list("${WARNING_FLAGS_STRING}" WARNING_FLAGS_LIST)

# Import analyzer_flags.txt
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/../../flags/${COMPILER_NAME}/analyzer_flags.txt" ANALYZER_FLAGS_STRING)
split_string_into_list("${ANALYZER_FLAGS_STRING}" ANALYZER_FLAGS_LIST)

# Import debug_flags.txt
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/../../flags/${COMPILER_NAME}/debug_flags.txt" DEBUG_FLAGSSTRING)
split_string_into_list("${DEBUG_FLAGS_STRING}" DEBUG_FLAGS_LIST)

# Import sanitizer_flags.txt
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/../../flags/${COMPILER_NAME}/sanitizer_flags.txt" SANITIZER_FLAGS_STRING)
split_string_into_list("${SANITIZER_FLAGS_STRING}" SANITIZER_FLAGS_LIST)

# Check if the system uses 64-bit libraries
get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)

# Set the library suffix based on whether it's 64-bit or not
if ("${LIB64}" STREQUAL "TRUE")
    set(LIBSUFFIX 64)
else()
    set(LIBSUFFIX "")
endif()

target_link_directories(set_get_will_change_state_notifier_example PRIVATE /usr/local/lib${LIBSUFFIX})

# Enable sanitizer checks
target_link_libraries(set_get_will_change_state_notifier_example PRIVATE ${SANITIZER_FLAGS_STRING})

# Link libraries
target_link_libraries(set_get_will_change_state_notifier_example PRIVATE p101_error)
target_link_libraries(set_get_will_change_state_notifier_example PRIVATE p101_env)
target_link_libraries(set_get_will_change_state_notifier_example PRIVATE p101_c)
target_link_libraries(set_get_will_change_state_notifier_example PRIVATE p101_posix)
target_link_libraries(set_get_will_change_state_notifier_example PRIVATE p101_fsm)

# Common compiler flags
set(STANDARD_FLAGS
    -D_POSIX_C_SOURCE=200809L
    -D_XOPEN_SOURCE=700
    -D_GNU_SOURCE
    -D_DARWIN_C_SOURCE
    -D__BSD_VISIBLE
    -Werror
)

# Set compiler flags for the target
target_compile_options(set_get_will_change_state_notifier_example PRIVATE
    ${STANDARD_FLAGS}
    ${WARNING_FLAGS_LIST}
    ${ANALYZER_FLAGS_LIST}
    ${DEBUG_FLAGS_LIST}
    ${SANITIZER_FLAGS_LIST}
)

if (NOT DEFINED CLANG_FORMAT_NAME)
    set(CLANG_FORMAT_NAME "clang-format")
endif()

if (NOT DEFINED CLANG_TIDY_NAME)
    set(CLANG_TIDY_NAME "clang-tidy")
endif()

if (NOT DEFINED CPPCHECK_NAME)
    set(CPPCHECK_NAME "cppcheck")
endif()

find_program(CLANG_FORMAT NAMES ${CLANG_FORMAT_NAME} REQUIRED)
find_program(CLANG_TIDY NAMES ${CLANG_TIDY_NAME} REQUIRED)
find_program(CPPCHECK NAMES ${CPPCHECK_NAME} REQUIRED)

# Format source files using clang-format
add_custom_target(set_get_will_change_state_notifier_example_format
    COMMAND ${CLANG_FORMAT} --style=file -i ${SOURCES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running clang-format"
)

# Lint source files using clang-tidy
add_custom_target(set_get_will_change_state_notifier_example_tidy
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CLANG_TIDY} ${SOURCES} -quiet --warnings-as-errors='*' -checks=*,-llvmlibc-restrict-system-libc-headers,-altera-struct-pack-align,-readability-identifier-length,-altera-unroll-loops,-cppcoreguidelines-init-variables,-cert-err33-c,-modernize-macro-to-enum,-bugprone-easily-swappable-parameters,-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling,-altera-id-dependent-backward-branch,-concurrency-mt-unsafe,-misc-unused-parameters,-hicpp-signed-bitwise,-google-readability-todo,-cert-msc30-c,-cert-msc50-cpp,-readability-function-cognitive-complexity,-clang-analyzer-security.insecureAPI.strcpy,-cert-env33-c,-android-cloexec-accept,-clang-analyzer-security.insecureAPI.rand,-misc-include-cleaner,-llvm-header-guard -- ${CMAKE_C_FLAGS} ${STANDARD_FLAGS} -I/usr/local/include
    COMMENT "Running clang-tidy"
)

# Add a custom target for cppcheck
add_custom_target(set_get_will_change_state_notifier_example_cppcheck
    COMMAND ${CPPCHECK} --error-exitcode=1 --force --quiet --library=posix --enable=all --suppress=missingIncludeSystem --suppress=unusedFunction --suppress=unmatchedSuppression --suppress=checkersReport -I/usr/local/include ${SOURCES} ${HEADER_LIST}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running cppcheck"
)

# Check if CMAKE_C_COMPILER starts with "clang"
if (CMAKE_C_COMPILER MATCHES ".*/clang.*")
    # Add a custom target for clang --analyze
    add_custom_target(set_get_will_change_state_notifier_example_analysis
        COMMAND ${CMAKE_C_COMPILER} --analyze --analyzer-output text -Xclang -analyzer-checker=core --analyze -Xclang -analyzer-checker=deadcode -Xclang -analyzer-checker=security -Xclang -analyzer-disable-checker=security.insecureAPI.DeprecatedOrUnsafeBufferHandling -Xclang -analyzer-checker=unix -Xclang -analyzer-checker=unix ${CMAKE_C_FLAGS} ${STANDARD_FLAGS} -I${CMAKE_CURRENT_SOURCE_DIR}/include -I/usr/local/include ${SOURCES} ${HEADER_LIST}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running clang --analyze"
    )

    # Add a custom command to delete .gch files after the analysis
    add_custom_command(
        TARGET set_get_will_change_state_notifier_example POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_SOURCE_DIR}/include/p101_env/*.gch
        COMMENT "Removing .gch files"
    )

    # Add dependencies including 'analysis' for Clang compiler
    add_custom_target(set_get_will_change_state_notifier_example_all
        DEPENDS set_get_will_change_state_notifier_example set_get_will_change_state_notifier_example_analysis set_get_will_change_state_notifier_example_tidy set_get_will_change_state_notifier_example_cppcheck
        COMMENT "Running code analysis tools"
    )
else()
    # Add dependencies without 'analysis' for other compilers
    add_custom_target(set_get_will_change_state_notifier_example_all
        DEPENDS set_get_will_change_state_notifier_example set_get_will_change_state_notifier_example_tidy set_get_will_change_state_notifier_example_cppcheck
        COMMENT "Running code analysis tools"
    )
endif()

add_dependencies(set_get_will_change_state_notifier_example set_get_will_change_state_notifier_example_format)
