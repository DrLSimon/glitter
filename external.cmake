#
function(checkLocalDependency  localPath)
    if(NOT EXISTS ${localPath})
        execute_process(COMMAND git submodule update --init ${localPath}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
endfunction()

# glfw
set( ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:$ENV{HOME}/local_install/lib/pkgconfig")
FIND_PACKAGE( PkgConfig REQUIRED )
PKG_SEARCH_MODULE( GLFW3  glfw3 )
if(NOT GLFW3_FOUND)
    set(GLFW_BUILD_EXAMPLES OFF CACHE STRING "" FORCE)
    set(GLFW_BUILD_TESTS    OFF CACHE STRING "" FORCE)
    set(GLFW_INSTALL        OFF CACHE STRING "" FORCE)
    checkLocalDependency(ext/glfw)
    add_subdirectory(ext/glfw)
    include_directories(ext/glfw/include)
    set(GLFW3_LIBRARIES glfw)
endif()

# glew
find_package(GLEW OPTIONAL_COMPONENTS glew)
if(GLEW_FOUND)
    find_package(OpenGL)
    if (OPENGL_FOUND)
        include_directories(${OPENGL_INCLUDE_DIR})
    else (OPENGL_FOUND)
        message("OpenGL environment missing")
    endif (OPENGL_FOUND)
    set(GLEW_LIBRARIES ${OPENGL_LIBRARIES} GLEW)
    if(APPLE)
        set(CMAKE_CXX_FLAGS "-stdlib=libc++ ${CMAKE_CXX_FLAGS}")
        SET(APPLE_FRAMEWORKS "-framework Cocoa -framework IOkit -framework CoreVideo")
        set(GLEW_LIBRARIES ${GLEW_LIBRARIES}  ${APPLE_FRAMEWORKS})
    endif(APPLE)
else()
    add_definitions(-DGLEW_STATIC)
    option(glew-cmake_BUILD_SHARED "" OFF)
    checkLocalDependency(ext/glew)
    add_subdirectory(ext/glew)
    include_directories(ext/glew/include ext/glew/src)
    include_directories(${OPENGL_INCLUDE_DIR})
    set(GLEW_LIBRARIES libglew_static)
endif()

# glm
checkLocalDependency(ext/glm)
include_directories(ext/glm)

# stb
checkLocalDependency("./ext/stb")
include_directories("./ext/stb")

# tinyobjloader
checkLocalDependency("./ext/tinyobjloader")
include_directories("./ext/tinyobjloader")

# termcolor
checkLocalDependency("./ext/termcolor")
include_directories("./ext/termcolor/include")
