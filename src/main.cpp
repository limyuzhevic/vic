// NLM - Neural Learning Machine Entry Point
// Enhanced CLI-based main entry point with comprehensive feature support

#include "cli/CLI.hpp"
#include "monitoring/Profiler.hpp"
#include <iostream>
#include <string>

using namespace nlm;

int main(int argc, char** argv) {
    // Initialize global profiler for performance tracking
    Profiler::setProfilingMode(ProfilingMode::Detailed);
    
    // Create and run CLI
    CLI cli;
    
    if (!cli.parseArguments(argc, argv)) {
        std::cerr << "Failed to parse command line arguments" << std::endl;
        return 1;
    }
    
    int result = cli.execute();
    
    // Print profiler report if profiling was enabled
    Profiler profiler;
    profiler.printReport();
    
    return result;
}
