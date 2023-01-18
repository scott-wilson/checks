cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

set(CPPCHECKS_VERSION_MAJOR "0")
set(CPPCHECKS_VERSION_MINOR "1")
set(CPPCHECKS_VERSION_PATCH "0")
set(CPPCHECKS_VERSION ${CPPCHECKS_VERSION_MAJOR}.${CPPCHECKS_VERSION_MINOR}.${CPPCHECKS_VERSION_PATCH})

project(cppchecks VERSION ${CPPCHECKS_VERSION} LANGUAGES CXX)

# --------------------------------------------------------------------------------
# Build Options
# --------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------------------------------------
# Do the build
# --------------------------------------------------------------------------------
set(CHECKS_HEADERS
${PROJECT_SOURCE_DIR}/include/cppchecks/check.h
${PROJECT_SOURCE_DIR}/include/cppchecks/item.h
${PROJECT_SOURCE_DIR}/include/cppchecks/items.h
${PROJECT_SOURCE_DIR}/include/cppchecks/result.h
${PROJECT_SOURCE_DIR}/include/cppchecks/runner.h
${PROJECT_SOURCE_DIR}/include/cppchecks/status.h
)

add_library(${CMAKE_PROJECT_NAME}
${CHECKS_HEADERS}
)

set(CCHECKS_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../c)
add_subdirectory(${PROJECT_SOURCE_DIR}/../c ${CMAKE_BINARY_DIR}/cchecks)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
${PROJECT_SOURCE_DIR}/include
${PROJECT_SOURCE_DIR}/../c/include
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${CMAKE_PROJECT_NAME} cchecks)

# add_subdirectory(src)

if (BUILD_CPPCHECKS_TESTS)
    enable_testing()
    add_subdirectory(googletest)
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory(tests)
endif()
