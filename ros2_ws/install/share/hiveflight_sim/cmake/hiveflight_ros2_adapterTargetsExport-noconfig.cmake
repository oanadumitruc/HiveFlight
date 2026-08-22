#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "hiveflight_sim::hiveflight_ros2_adapter" for configuration ""
set_property(TARGET hiveflight_sim::hiveflight_ros2_adapter APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(hiveflight_sim::hiveflight_ros2_adapter PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libhiveflight_ros2_adapter.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS hiveflight_sim::hiveflight_ros2_adapter )
list(APPEND _IMPORT_CHECK_FILES_FOR_hiveflight_sim::hiveflight_ros2_adapter "${_IMPORT_PREFIX}/lib/libhiveflight_ros2_adapter.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
