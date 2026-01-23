/** @file */
#ifndef __GLITTER_UTILS_H__
#define __GLITTER_UTILS_H__

#ifndef RESOURCE_DIR
#define RESOURCE_DIR "."
#endif

#include <string>

/**
 * @brief reads the content of the file
 * @param filename the name of the target file
 * @return a single string with the content of the file
 *
 * @note a resource directory is prepended to the filename (RESOURCE_DIR preprocessor macro).
 */
std::string fileContent(const std::string & filename);

/// @brief checks if a file exists
bool fileExists(const std::string & name);

/// @brief retrieves the absolute name of a file
std::string absolutename(const std::string & filename);

/// @brief retrieves the basename of a file
std::string basename(const std::string & filepath);

/// @brief check whether a string ends with a given suffix
bool endsWith(const std::string & str, const std::string & suffix);

/// @brief pop the last open GL error and display it in human readable format
void checkGLerror();

#endif // __GLITTER_UTILS_H__
