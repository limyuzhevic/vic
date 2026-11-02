// Common CLI help system for NLM demo executables
#ifndef NLM_DEMO_HELP_H
#define NLM_DEMO_HELP_H

#include <iostream>
#include <string>
#include <vector>

namespace nlm {
namespace demo {

// Print program usage information
void printUsage(const std::string& programName, const std::string& description,
                const std::vector<std::string>& arguments);

// Print help message
void printHelp(const std::string& programName, const std::string& description,
               const std::vector<std::string>& arguments);

// Parse integer argument with default
int parseIntArg(int argc, char* argv[], int argIndex, int defaultValue,
                const std::string& argName);

// Print separator
void printSeparator(char ch = '=', int length = 60);

// Print header
void printHeader(const std::string& title);

// Print section
void printSection(const std::string& title);

} // namespace demo
} // namespace nlm

#endif // NLM_DEMO_HELP_H