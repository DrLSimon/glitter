# +------------------------------------------------------------------+
# |  Projects Settings                                               |
# +------------------------------------------------------------------+
cmake_minimum_required (VERSION 2.8)
project(GLITTER)

# +------------------------------------------------------------------+
# |  Test system endian type                                         |
# +------------------------------------------------------------------+
include (TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
  add_definitions(-DIS_BIG_ENDIAN)
else()
  add_definitions(-DIS_LITTLE_ENDIAN)
endif()

# +------------------------------------------------------------------+
# |  Compilation flags                                               |
# +------------------------------------------------------------------+
set (CMAKE_CXX_STANDARD 11)
add_definitions(-Wall -Wextra)
add_definitions(-DRESOURCE_DIR=\"${CMAKE_SOURCE_DIR}\")

# +------------------------------------------------------------------+
# |  Load libraries                                                  |
# +------------------------------------------------------------------+
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include(external)

# +------------------------------------------------------------------+
# |  Internal API library                                            |
# +------------------------------------------------------------------+

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/)
set(UTILS_SRC src/glApi.hpp
              src/glApi.cpp
              src/Application.hpp
              src/Application.cpp
              src/ObjLoader.hpp
              src/ObjLoader.cpp
              src/Image.hpp
              src/SimpleMaterial.hpp
              src/utils.hpp
              src/utils.cpp
              src/Serialize.hpp
              src/Serialize.cpp
              src/AttributeProperties.hpp)
add_library(utils ${UTILS_SRC})

# +------------------------------------------------------------------+
# |  glitter executable                                              |
# +------------------------------------------------------------------+

file(GLOB GLSLFiles shaders/*.glsl)

add_executable(glitter
  examples/main.cpp
  examples/PA1Application.hpp
  examples/PA1Application.cpp
  examples/PA2Application.hpp
  examples/PA2Application.cpp
  examples/PA3Application.hpp
  examples/PA3Application.cpp
  examples/PA4Application.hpp
  examples/PA4Application.cpp
  examples/PA5Application.hpp
  examples/PA5Application.cpp
  ${GLSLFiles}
  )
target_link_libraries(glitter utils ${GLFW3_LIBRARIES} ${GLEW_LIBRARIES})

# +------------------------------------------------------------------+
# |  rubik's cube                                                    |
# +------------------------------------------------------------------+
add_executable(rubik
  rubik/main.cpp
  rubik/RubikApplication.hpp
  rubik/RubikApplication.cpp
  rubik/RubikRenderer.hpp
  rubik/RubikRenderer.cpp
  rubik/RubikLogic.hpp
  rubik/RubikLogic.cpp
  rubik/GameStage.hpp
  rubik/GameStage.cpp
  rubik/TextPrinter.hpp
  rubik/TextPrinter.cpp
  rubik/rubik.v.glsl
  rubik/rubik.f.glsl
  rubik/font.v.glsl
  rubik/font.f.glsl
  rubik/hud.v.glsl
  rubik/hud.f.glsl
  )
target_link_libraries(rubik utils ${GLFW3_LIBRARIES} ${GLEW_LIBRARIES})

# +------------------------------------------------------------------+
# |  obj2glitter file converter                                      |
# +------------------------------------------------------------------+

add_executable(obj2glitter
  examples/obj2glitter.cpp
  )
target_link_libraries(obj2glitter utils ${GLEW_LIBRARIES})

# +------------------------------------------------------------------+
# |  Doxygen Generation                                              |
# +------------------------------------------------------------------+

option(GLITTER_BUILD_DOCUMENTATION "Build the Doxygen documentation" ON)
if (GLITTER_BUILD_DOCUMENTATION)
  find_package(Doxygen)
  if (GLITTER_BUILD_DOCUMENTATION AND DOXYGEN_FOUND)
    configure_file(${GLITTER_SOURCE_DIR}/docs/Doxyfile.in
      ${GLITTER_BINARY_DIR}/docs/Doxyfile @ONLY)
    add_custom_target( doc_doxygen
      COMMAND ${DOXYGEN_EXECUTABLE} ${GLITTER_BINARY_DIR}/docs/Doxyfile
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM )
  endif()
endif()
