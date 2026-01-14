cmake_minimum_required(VERSION 3.30)
project(raylib-template VERSION 0.1.0 LANGUAGES C)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set options for build types
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")

# Dependencies
# set(RAYLIB_VERSION 5.5) # Change this to the version you want to use
set(RAYLIB_VERSION "master")
include(FetchContent)

if(RAYLIB_VERSION MATCHES "^[0-9.]+$") 
    find_package(raylib ${RAYLIB_VERSION} QUIET)
else() 
    find_package(raylib QUIET)
endif()

if (NOT raylib_FOUND) # If there's none, fetch and build raylib
    # check if RAYLIB_VERSION is a number
    if(RAYLIB_VERSION MATCHES "^[0-9.]+$")
        FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
        )
    else()
        FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG ${RAYLIB_VERSION} 
        ) 
    endif()  
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_MakeAvailable(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    endif()

endif()

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.c)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC include)

# Detect the compiler
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GCC
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Clang
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Do not use /W4 for now, as it is too strict and generates too many warnings
    # target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX) 
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  #Intel Compiler
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)

#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} raylib)

# Web Configurations
if ("${PLATFORM}" STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")    
    # Since WASM is used, ALLOW_MEMORY_GROWTH has no extra overheads
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1 -s ALLOW_MEMORY_GROWTH=1 --shell-file ${CMAKE_SOURCE_DIR}/minshell.html")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    #DEPENDS ${PROJECT_NAME}
    add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/../resources
    )
else()
    add_custom_command(
        TARGET ${PROJECT_NAME} 
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
        MAIN_DEPENDENCY ${target}Refresh
        
    )
    #DEPENDS ${PROJECT_NAME}
endif()

# Web Configurations
if ("${PLATFORM}" STREQUAL "Web")
    # Preload files
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    target_link_options(${PROJECT_NAME} PUBLIC -sUSE_GLFW=3 PUBLIC --preload-file resources)
endif()
# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()

# set(GOOGLETEST_VERSION 1.15.2)

# # --- Add Google Test using FetchContent ---
# if (NOT googletest_FOUND) # If there's none, fetch and build googletest
#     FetchContent_Declare(
#         googletest
#         DOWNLOAD_EXTRACT_TIMESTAMP OFF
#         URL https://github.com/google/googletest/archive/refs/tags/v${GOOGLETEST_VERSION}.tar.gz
#     )
# endif()
# Check if we are in Debug mode before making Google Test available
# if (CMAKE_BUILD_TYPE MATCHES "Debug")
#     find_package(googletest ${GOOGLETEST_VERSION} QUIET) # QUIET or REQUIRED
#     FetchContent_GetProperties(googletest)
#     if (NOT googletest_POPULATED) # Have we downloaded googletest yet?
#         set(FETCHCONTENT_QUIET NO)
#         FetchContent_MakeAvailable(googletest)
#         set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#     endif()   

#     add_subdirectory(tests)
#     enable_testing() # Enable CTest only when tests are included
# endif()