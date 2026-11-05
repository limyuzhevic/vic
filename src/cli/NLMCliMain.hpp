// NLM Command-Line Interface - Main entry point header

#pragma once

#include "CliOptions.hpp"
#include <memory>

namespace nlm {

// Initialize NLM with command-line options
void initializeNLM(const CliOptions& options);

// Run all available demos
void runAllDemos(const CliOptions& options);

// Run a specific demo
void runDemo(const CliOptions& options);

// Print help information
void printHelp();

// Print version information
void printVersion();

} // namespace nlm