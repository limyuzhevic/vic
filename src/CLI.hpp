// NLM (熙然) - Neural Learning Machine
// Main entry point with unified command-line interface
// This version provides a comprehensive CLI for running different experiments

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>

using namespace nlm;

// Simple logging function for CLI
void logMessage(const std::string& level, const std::string& message) {
    std::cout << "[" << level << "] " << message << std::endl;
}