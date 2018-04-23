#include <iostream>
#include <vector>
// matrix and vectors
// to declare before including glm.hpp, to use the swizzle operators
#define GLM_FORCE_SWIZZLE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "RubikApplication.hpp"
#include "glApi.hpp"
#include "termcolor/termcolor.hpp"

int main(int /*argc*/, char * /*argv*/ [])
{
  RubikApplication app;
  app.setCallbacks();
  app.mainLoop();
  return 0;
}
