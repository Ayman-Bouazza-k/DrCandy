#include "util.h"
#include <filesystem>
#include <iostream>

std::string getDataDirPath()
{
    const std::string current_dirname = std::filesystem::current_path().filename().generic_string();
    std::string resource_prefix = "";

    if (current_dirname == "drcandy"
        || current_dirname == "drcandy_public"
        || current_dirname == "drcandy_solution"
        || current_dirname == "source")
    {
        resource_prefix = "./";
    }
    else if (current_dirname == "src"
        || current_dirname == "build"
        || current_dirname == "visual_studio"
        || current_dirname.find("cmake") != std::string::npos)
    {
        resource_prefix = "../";
    }
    else if (current_dirname == "0. Windows Desktop")
    {
        resource_prefix = "../../";
    }
    else if (current_dirname == "Program")
    {
        resource_prefix = "../../../";
    }
    else
    {
        std::cerr << "Cannot find resources folder. Current directory: '"
            + current_dirname + "'.\nPlease run with run.sh or run.bat" << std::endl;
    }

    return resource_prefix + "data/";
}
