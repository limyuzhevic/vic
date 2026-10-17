#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace nlm {

// Command line interface for all NLM executables
// Provides modern argument parsing and unified interface for demo, benchmark, and main executables

// Main entry point - should be used by all executables
int main(int argc, char** argv);

// Wrapper for backward compatibility - used by existing executables
namespace CLIAdapter {
    // Adapt existing main() functions to work with new CLI
    int adaptMain(int argc, char* argv[]);
    int adaptPhase3Demo(int argc, char* argv[]);
    int adaptPhase4Demo(int argc, char* argv[]);
    int adaptPhase6Demo(int argc, char* argv[]);
    int adaptBenchmark(int argc, char* argv[]);
}

} // namespace nlm