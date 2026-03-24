#pragma once
#include <string>

/// @return the dynamic path to the "data" folder.
///   The path is relative to the current working directory, which may vary
///   depending on how the program is run. This prefix followed by the resource
///   (e.g., "img/candy/red.png") yields the complete file path of the resource.
std::string getDataDirPath();