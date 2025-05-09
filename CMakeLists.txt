cmake_minimum_required(VERSION 3.10.0)
project(ds-cpp VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

## useful cmake utils
include(cmake/Sanitizers.cmake)
include(cmake/CompilerWarnings.cmake)

add_executable(ds-cpp ds-cpp.cpp)


## enable sanitizers on Debug build
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  project_enable_sanitizers(
    ds-cpp
    ON  # AddressSanitizer
    OFF # LeakSanitizer
    ON  # UndefinedBehaviorSanitizer
    OFF # ThreadSanitizer
    OFF # MemorySanitizer
  )   
endif()

