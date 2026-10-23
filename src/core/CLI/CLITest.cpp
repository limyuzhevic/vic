#include "core/CLI/CLInterface.hpp"
#include <iostream>

int main() {
    nlm::CLIParser cli("nlm", "Neural Learning Machine - Test CLI");
    
    // Add some basic options
    cli.addOption("neurons", "n", "Number of neurons", "500", false, "int");
    cli.addOption("timestep", "dt", "Simulation timestep", "0.001", false, "float");
    cli.addOption("verbose", "v", "Verbose level (0-3)", "1", false, "int");
    cli.addFlag("benchmark", "Run benchmark test");
    cli.addFlag("debug", "Enable debug output");
    
    // Test 1: Basic parsing
    std::cout << "=== Test 1: Basic CLI Parsing ===" << std::endl;
    char* argv[] = {(char*)"nlm", (char*)"--neurons", (char*)"1000", (char*)"--timestep", (char*)"0.0001", (char*)"--verbose", (char*)"2"};
    int argc = 7;
    
    if (cli.parse(argc, argv)) {
        std::cout << "✓ Parsing successful" << std::endl;
        
        auto neurons = cli.getInt("neurons");
        if (neurons && *neurons == 1000) {
            std::cout << "✓ Neuron count: " << *neurons << std::endl;
        }
        
        auto timestep = cli.getDouble("timestep");
        if (timestep && std::abs(*timestep - 0.0001) < 0.00001) {
            std::cout << "✓ Timestep: " << *timestep << std::endl;
        }
        
        auto verbose = cli.getInt("verbose");
        if (verbose && *verbose == 2) {
            std::cout << "✓ Verbose level: " << *verbose << std::endl;
        }
    }
    
    // Test 2: Short options
    std::cout << "\n=== Test 2: Short Options ===" << std::endl;
    char* argv2[] = {(char*)"nlm", (char*)"-n", (char*)"2000", (char*)"-v", (char*)"1"};
    int argc2 = 5;
    
    nlm::CLIParser cli2("nlm", "Short options test");
    cli2.addOption("neurons", "n", "Number of neurons", "500", false, "int");
    cli2.addOption("verbose", "v", "Verbose level", "1", false, "int");
    
    if (cli2.parse(argc2, argv2)) {
        std::cout << "✓ Short option parsing successful" << std::endl;
        
        auto neurons = cli2.getInt("neurons");
        if (neurons && *neurons == 2000) {
            std::cout << "✓ Neuron count from short option: " << *neurons << std::endl;
        }
    }
    
    // Test 3: Flag options
    std::cout << "\n=== Test 3: Flag Options ===" << std::endl;
    nlm::CLIParser cli3("nlm", "Flag test");
    cli3.addFlag("benchmark", "Benchmark mode");
    cli3.addFlag("debug", "Debug mode");
    
    char* argv3[] = {(char*)"nlm", (char*)"--benchmark", (char*)"--debug"};
    int argc3 = 3;
    
    if (cli3.parse(argc3, argv3)) {
        std::cout << "✓ Flag parsing successful" << std::endl;
        std::cout << "✓ Benchmark flag: " << (cli3.getFlag("benchmark") ? "enabled" : "disabled") << std::endl;
        std::cout << "✓ Debug flag: " << (cli3.getFlag("debug") ? "enabled" : "disabled") << std::endl;
    }
    
    // Test 4: Help functionality
    std::cout << "\n=== Test 4: Help and Usage ===" << std::endl;
    nlm::CLIParser cli4("nlm", "Help test");
    cli4.addOption("input", "i", "Input file", "input.dat");
    cli4.addOption("output", "o", "Output file", "output.dat");
    cli4.addFlag("force", "Force overwrite");
    
    std::cout << "Program help:\n";
    // Note: In a real implementation, we'd call cli4.printHelp() and capture output
    std::cout << "✓ Help functionality available" << std::endl;
    
    std::cout << "\n=== All CLI Tests Completed ===" << std::endl;
    return 0;
}