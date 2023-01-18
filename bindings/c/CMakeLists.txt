cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

set(CCHECKS_VERSION_MAJOR "0")
set(CCHECKS_VERSION_MINOR "1")
set(CCHECKS_VERSION_PATCH "0")
set(CCHECKS_VERSION ${CCHECKS_VERSION_MAJOR}.${CCHECKS_VERSION_MINOR}.${CCHECKS_VERSION_PATCH})

project(cchecks VERSION ${CCHECKS_VERSION} LANGUAGES C)

# --------------------------------------------------------------------------------
# Build Options
# --------------------------------------------------------------------------------

# --------------------------------------------------------------------------------
# Do the build
# --------------------------------------------------------------------------------
set(UNIT_TESTING OFF)

add_subdirectory(corrosion)

if (NOT DEFINED CCHECKS_SOURCE_DIR OR CCHECKS_SOURCE_DIR MATCHES "")
set(CCHECKS_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
endif()

corrosion_import_crate(MANIFEST_PATH "${CCHECKS_SOURCE_DIR}/Cargo.toml")

if (BUILD_CCHECKS_TESTS)
    enable_testing()
    add_subdirectory(cmocka)
    add_subdirectory(tests)
endif()
