#pragma once

#include <iostream>
#include <string>

namespace nlm {

/**
 * @brief Prints the NLM banner and version information
 * 
 * Displays an ASCII art banner with project information and Phase details.
 */
void printBanner();

/**
 * @brief Displays configuration and version information
 * 
 * Shows current configuration parameters and system status.
 */
void printSystemInfo(const std::string& configFile);

/**
 * @brief Parses command-line arguments
 * 
 * @param argc Argument count
 * @param argv Argument vector
 * @param configFile Output parameter for config file path
 * @param help Whether to display help and exit
 * @return true if successful, false on error
 */
bool parseCommandLine(int argc, char** argv, std::string& configFile, bool& help);

/**
 * @brief Displays help information
 */
void printHelp();

} // namespace nlm