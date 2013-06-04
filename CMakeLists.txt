cmake_minimum_required(VERSION 2.8)

# project name
project(describeit)

# explicitly set language and linker language
set_property(GLOBAL PROPERTY LANGUAGE "C++")
set_property(GLOBAL PROPERTY LINKER_LANGUAGE "C++")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # compiler is pedantic and shows every warning and uses debugging flags
    set (CMAKE_CXX_FLAGS "-pedantic -Wall -Wextra -Werror -O0 -fno-inline -ggdb")
#add_definitions ("-std=c++11")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # disable language extensions and show all warnings
    set(CMAKE_CXX_FLAGS "\Za \Wall")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set (CMAKE_CXX_FLAGS "-pedantic -Wall -Wextra -Werror -O0 -fno-inline -ggdb -g")
endif()

# list all cpp files in directory test
file(GLOB test_files
    "test/*.cpp"
)

# set include directory
include_directories("${PROJECT_SOURCE_DIR}/include")
# set source files to target test
add_executable(test ${test_files})

# install rule
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/uticc DESTINATION include)
