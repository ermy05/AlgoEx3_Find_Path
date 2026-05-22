# CMakeList.txt : CMake project for BinTree, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("find_path")

# Add source to this project's executable.
add_executable (find_path "find_path.cpp" "find_path.h"   "headers/Station.h" "src/Station.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET find_path PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

target_include_directories(find_path PUBLIC "${PROJECT_SOURCE_DIR}/")

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/")
