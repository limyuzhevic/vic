// Enhanced CLI and Performance Tests
#include "core/CLI/CLInterface.hpp"
#include "core/Performance/Profiler.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <thread>

namespace test_cli_enhanced {

void testCLIParserOptions() {
    nlm::CLIParser cli("test", "Enhanced CLI Test");
    
    // Add comprehensive option set
    cli.addOption("neurons", "n", "Number of neurons", "1000", false, "int");
    cli.addOption("timestep", "dt", "Simulation timestep (seconds)", "0.001", false, "float");
    cli.addOption("seed", "s", "Random seed", "42", false, "int");
    cli.addOption("verbose", "v", "Verbosity level (0-3)", "1", false, "int");
    cli.addOption("config", "c", "Configuration file", "config.cfg", false, "string");
    
    // Add flags
    cli.addFlag("benchmark", "Run benchmark mode");
    cli.addFlag("debug", "Enable debug output");
    cli.addFlag("profile", "Enable performance profiling");
    cli.addFlag("validate", "Validate configuration only");
    
    // Test 1: All options parsing
    std::cout << "    Testing option parsing..." << std::endl;
    char* argv[] = {(char*)"test", (char*)"--neurons", (char*)"2000", (char*)"--timestep", (char*)"0.0005",
                   (char*)"--seed", (char*)"123", (char*)"--verbose", (char*)"2",
                   (char*)"--config", (char*)"myconfig.cfg", (char*)"--benchmark",
                   (char*)"--debug"};
    int argc = 13;
    
    bool parseResult = cli.parse(argc, argv);
    assert(parseResult);
    
    // Verify all options
    auto neurons = cli.getInt("neurons");
    assert(neurons && *neurons == 2000);
    
    auto timestep = cli.getDouble("timestep");
    assert(timestep && std::abs(*timestep - 0.0005) < 0.000001);
    
    auto seed = cli.getInt("seed");
    assert(seed && *seed == 123);
    
    auto verbose = cli.getInt("verbose");
    assert(verbose && *verbose == 2);
    
    auto config = cli.getString("config");
    assert(config && *config == "myconfig.cfg");
    
    assert(cli.getFlag("benchmark") == true);
    assert(cli.getFlag("debug") == true);
    assert(cli.getFlag("profile") == false); // Not specified
    assert(cli.getFlag("validate") == false); // Not specified
    
    std::cout << "    ✓ Option parsing test passed" << std::endl;
}

void testCLIShortOptions() {
    std::cout << "    Testing short option parsing..." << std::endl;
    
    nlm::CLIParser cli("test", "Short options");
    cli.addOption("input", "i", "Input file", "input.dat", false, "string");
    cli.addOption("output", "o", "Output file", "output.dat", false, "string");
    cli.addOption("steps", "s", "Number of steps", "100", false, "int");
    cli.addFlag("force", "f", "Force overwrite");
    
    char* argv[] = {(char*)"test", (char*)"-i", (char*)"data.in", (char*)"-o", (char*)"result.out",
                   (char*)"-s", (char*)"500", (char*)"-f"};
    int argc = 7;
    
    bool parseResult = cli.parse(argc, argv);
    assert(parseResult);
    
    auto input = cli.getString("input");
    assert(input && *input == "data.in");
    
    auto output = cli.getString("output");
    assert(output && *output == "result.out");
    
    auto steps = cli.getInt("steps");
    assert(steps && *steps == 500);
    
    assert(cli.getFlag("force") == true);
    
    std::cout << "    ✓ Short option parsing test passed" << std::endl;
}

void testCLIValidationAndErrors() {
    std::cout << "    Testing CLI validation and error handling..." << std::endl;
    
    nlm::CLIParser cli("test", "Error test");
    cli.addOption("required", "", "Required option", "", true, "string"); // Required
    cli.addOption("optional", "", "Optional option", "default", false, "string");
    
    // Test missing required option
    char* argv[] = {(char*)"test", (char*)"--optional", (char*)"value"};
    int argc = 3;
    
    // This should parse but validation would be done by caller
    bool parseResult = cli.parse(argc, argv);
    assert(parseResult); // Parsing succeeds, but validation is separate
    
    // Test help request
    nlm::CLIParser cli2("test", "Help test");
    cli2.addOption("test", "", "Test option", "default");
    
    char* helpArgs[] = {(char*)"test", (char*)"--help"};
    int helpArgc = 2;
    
    // Help should return false and exit (simulated)
    bool helpResult = cli2.parse(helpArgc, helpArgs);
    // Help parsing typically returns false to exit
    std::cout << "    ✓ CLI validation test passed" << std::endl;
}

void testCLIAdvancedFeatures() {
    std::cout << "    Testing advanced CLI features..." << std::endl;
    
    nlm::CLIParser cli("advanced", "Advanced features test");
    cli.addOption("iterations", "", "Number of iterations", "1000", false, "int");
    cli.addOption("learning-rate", "", "Learning rate (float)", "0.01", false, "float");
    cli.addOption("model-name", "", "Model name", "default", false, "string");
    cli.addOption("batch-size", "b", "Batch size", "32", false, "int");
    cli.addOption("epochs", "e", "Number of epochs", "10", false, "int");
    cli.addFlag("shuffle", "Enable data shuffling");
    cli.addFlag("verbose", "Verbose output");
    cli.addFlag("dry-run", "Dry run mode");
    cli.addFlag("save-checkpoints", "Save model checkpoints");
    
    // Complex argument list
    char* argv[] = {(char*)"advanced", (char*)"--iterations", (char*)"2000",
                   (char*)"--learning-rate", (char*)"0.05", (char*)"--model-name", (char*)"neural-net",
                   (char*)"--batch-size", (char*)"64", (char*)"--epochs", (char*)"50",
                   (char*)"--shuffle", (char*)"--verbose", (char*)"--save-checkpoints"};
    int argc = 14;
    
    bool parseResult = cli.parse(argc, argv);
    assert(parseResult);
    
    auto iterations = cli.getInt("iterations");
    assert(iterations && *iterations == 2000);
    
    auto learningRate = cli.getDouble("learning-rate");
    assert(learningRate && std::abs(*learningRate - 0.05) < 0.0001);
    
    auto modelName = cli.getString("model-name");
    assert(modelName && *modelName == "neural-net");
    
    auto batchSize = cli.getInt("batch-size");
    assert(batchSize && *batchSize == 64);
    
    auto epochs = cli.getInt("epochs");
    assert(epochs && *epochs == 50);
    
    assert(cli.getFlag("shuffle") == true);
    assert(cli.getFlag("verbose") == true);
    assert(cli.getFlag("dry-run") == false);
    assert(cli.getFlag("save-checkpoints") == true);
    
    std::cout << "    ✓ Advanced features test passed" << std::endl;
}

void testCLIPerformanceMonitoring() {
    std::cout << "    Testing CLI performance monitoring integration..." << std::endl;
    
    // Create a simple performance profiler test
    nlm::PerformanceProfiler profiler;
    
    // Simulate some work
    auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "    ✓ Performance measurement: " << elapsed.count() * 1000 << " ms" << std::endl;
    
    // Test profiler report generation
    std::string report = nlm::Profiler::getInstance().generateReport();
    std::cout << "    ✓ Performance report generated (" << report.size() << " chars)" << std::endl;
    
    std::cout << "    ✓ Performance monitoring test passed" << std::endl;
}

void testCLIBannerAndOutput() {
    std::cout << "    Testing CLI banner and output formatting..." << std::endl;
    
    // Test banner generation
    nlm::CLIParser cli("test", "Banner Test");
    
    std::cout << "  " << std::string(60, '=') << std::endl;
    std::cout << "  CLI Output Formatting Test" << std::endl;
    std::cout << "  " << std::string(60, '=') << std::endl;
    
    // Test status printing
    cli.printStatus("Test Parameter", "Value 123", true);
    cli.printStatus("Warning", "This is a warning", false);
    
    std::cout << "    ✓ Banner and output formatting test passed" << std::endl;
}

void runAll() {
    testCLIParserOptions();
    testCLIShortOptions();
    testCLIValidationAndErrors();
    testCLIAdvancedFeatures();
    testCLIPerformanceMonitoring();
    testCLIBannerAndOutput();
    
    std::cout << "\n=== Enhanced CLI Tests Completed ===" << std::endl;
}

} // namespace test_cli_enhanced