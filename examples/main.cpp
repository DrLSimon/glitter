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
// loading stuffs
#include "PA1Application.hpp"
#include "PA2Application.hpp"
#include "PA3Application.hpp"
#include "PA4Application.hpp"
#include "PA5Application.hpp"
#include "glApi.hpp"
#include "termcolor/termcolor.hpp"
#include "utils.hpp"

void printUsage(int argc, char * argv[])
{
  if (argc <= 2) {
    std::string pa1ShortDescription;
    std::string pa2ShortDescription;
    std::string pa3ShortDescription;
    std::string pa4ShortDescription;
    std::string pa5ShortDescription;
    std::string synopsis;
    std::string description;
    PA1Application::usage(pa1ShortDescription, synopsis, description);
    PA2Application::usage(pa2ShortDescription, synopsis, description);
    PA3Application::usage(pa3ShortDescription, synopsis, description);
    PA4Application::usage(pa4ShortDescription, synopsis, description);
    PA5Application::usage(pa5ShortDescription, synopsis, description);
    std::cout << termcolor::bold << "Usage: " << termcolor::reset << argv[0] << " <command> [<args>]\n\n"
              << "The following commands are available:\n"
              << "  help        "
              << "print usage (of other commands if specified in <args>)\n"
              << "  pa1         " << pa1ShortDescription << "\n"
              << "  pa2         " << pa2ShortDescription << "\n"
              << "  pa3         " << pa3ShortDescription << "\n"
              << "  pa4         " << pa4ShortDescription << "\n"
              << "  pa5         " << pa5ShortDescription << "\n";
  } else {
    std::string name = argv[2];
    std::string shortDescription;
    std::string synopsis;
    std::string description;
    if (name == "pa1") {
      PA1Application::usage(shortDescription, synopsis, description);
    } else if (name == "pa2") {
      PA2Application::usage(shortDescription, synopsis, description);
    } else if (name == "pa3") {
      PA3Application::usage(shortDescription, synopsis, description);
    } else if (name == "pa4") {
      PA4Application::usage(shortDescription, synopsis, description);
    } else if (name == "pa5") {
      PA5Application::usage(shortDescription, synopsis, description);
    }
    std::cout << termcolor::bold << "NAME\n"
              << termcolor::reset << "  " << name << " - " << shortDescription << "\n\n"
              << termcolor::bold << "SYNOPSIS\n"
              << termcolor::reset << "  " << synopsis << "\n\n"
              << termcolor::bold << "DESCRIPTION\n"
              << termcolor::reset << description << "\n";
  }
}

//-------------------- MAIN
int main(int argc, char * argv[])
{
  Application * app = nullptr;
  if (argc < 2 or !strcmp(argv[1], "help")) {
    printUsage(argc, argv);
    exit(0);
  } else if (!strcmp(argv[1], "pa1")) {
    unsigned int part = 1;
    if (argc >= 3) {
      part = atoi(argv[2]);
    }
    app = new PA1Application(640, 480, part);
  } else if (!strcmp(argv[1], "pa2")) {
    app = new PA2Application(640, 480);
  } else if (!strcmp(argv[1], "pa3")) {
    app = new PA3Application(640, 480);
  } else if (!strcmp(argv[1], "pa4")) {
    PA4Application::part = 1;
    if (argc >= 3) {
      PA4Application::part = atoi(argv[2]);
    }
    app = new PA4Application(640, 480);
  } else if (!strcmp(argv[1], "pa5")) {
    PA5Application::displayNormals = false;
    app = new PA5Application(640, 480);
  }
  app->setCallbacks();
  app->mainLoop();
  delete app;
}
