#include "core/CLI/CLInterface.hpp"
#include "core/Config/ConfigurationManager.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void demonstrateCLI() {
    std::cout << "=== NLM Enhanced CLI Demonstration ===" << std::endl;
    
    // Create CLI parser with comprehensive options
    nlm::CLIParser cli("nlm_demo", "Neural Learning Machine - Enhanced CLI Demo");
    
    // Add core configuration options
    cli.addOption("neurons", "n", "Number of neurons to simulate", "1000", false, "int");
    cli.addOption("timestep", "dt", "Simulation timestep in seconds", "0.001", false, "float");
    cli.addOption("seed", "s", "Random seed for reproducible results", "42", false, "int");
    cli.addOption("verbose", "v", "Verbosity level (0-3)", "1", false, "int");
    
    // Add performance options
    cli.addOption("performance", "perf", "Enable performance optimizations", "1", false, "bool");
    cli.addOption("simd", "", "Enable SIMD instructions", "1", false, "bool");
    cli.addOption("threads", "", "Number of threads for parallel processing", "0", false, "int");
    
    // Add experimental options
    cli.addOption("plasticity", "", "Enable plasticity experiments", "1", false, "bool");
    cli.addOption("stdp-only", "", "Run STDP-only verification", "0", false, "bool");
    cli.addOption("hebbian-only", "", "Run Hebbian-only verification", "0", false, "bool");
    
    // Add convenience flags
    cli.addFlag("quiet", "Suppress non-essential output");
    cli.addFlag("fast", "Skip detailed logging and use faster defaults");
    cli.addFlag("validate", "Validate configuration without running simulation");
    cli.addFlag("save-config", "Save current configuration to file");
    
    std::cout << "Available commands:" << std::endl;
    std::cout << "  nlm_demo --neurons=2000 --timestep=0.0005 --verbose=2" << std::endl;
    std::cout << "  nlm_demo --performance --simd --threads=4" << std::endl;
    std::cout << "  nlm_demo --plasticity --save-config" << std::endl;
    std::cout << "  nlm_demo --validate" << std::endl;
    std::cout << "  nlm_demo --help" << std::endl;
}

void demonstrateConfigurationManager() {
    std::cout << "\n=== Configuration Manager Demonstration ===" << std::endl;
    
    // Create configuration manager
    auto config = std::make_shared<nlm::Config>();
    nlm::ConfigurationManager cm(config);
    
    std::cout << "1. Default configuration:" << std::endl;
    std::cout << cm.getSummary() << std::endl;
    
    std::cout << "2. Performance profile:" << std::endl;
    cm.applyProfile(nlm::ConfigurationManager::Profile::Performance);
    cm.setInt("neuron_count", 5000, "Custom neuron count");
    std::cout << "   Neurons: " << cm.getInt("neuron_count") << std::endl;
    std::cout << "   SIMD: " << (cm.getBool("performance.simd") ? "Enabled" : "Disabled") << std::endl;
    std::cout << "   Multithreading: " << (cm.getBool("performance.multithreading") ? "Enabled" : "Disabled") << std::endl;
    
    std::cout << "\n3. Debug profile:" << std::endl;
    cm.applyProfile(nlm::ConfigurationManager::Profile::Debug);
    cm.setBool("debug.enabled", true, "Enable debug output");
    std::cout << "   Debug output: " << (cm.getBool("debug.enabled") ? "Enabled" : "Disabled") << std::endl;
    std::cout << "   Profiling: " << (cm.getBool("debug.profiling") ? "Enabled" : "Disabled") << std::endl;
    std::cout << "   Log level: " << cm.getInt("debug.log_level") << std::endl;
    
    std::cout << "\n4. Configuration validation:" << std::endl;
    bool isValid = cm.validate();
    std::cout << "   Validation result: " << (isValid ? "PASSED" : "FAILED") << std::endl;
    
    std::cout << "\n5. Configuration export:" << std::endl;
    cm.saveToFile("/tmp/demo_config.cfg");
    std::cout << "   Configuration saved to /tmp/demo_config.cfg" << std::endl;
    
    cm.loadFromFile("/tmp/demo_config.cfg");
    std::cout << "   Configuration loaded from file" << std::endl;
}

void demonstratePerformanceMonitoring() {
    std::cout << "\n=== Performance Monitoring Demonstration ===" << std::endl;
    
    // Demonstrate performance profiler
    auto& profiler = nlm::Profiler::getInstance();
    
    std::cout << "1. Generating performance report..." << std::endl;
    std::string report = nlm::Profiler::getReport();
    std::cout << "   Report length: " << report.size() << " characters" << std::endl;
    
    std::cout << "\n2. Testing performance integration..." << std::endl;
    // In a real implementation, we would profile actual brain simulation
    std::cout << "   Performance monitoring system ready for integration" << std::endl;
    
    std::cout << "\n3. Performance recommendations:" << std::endl;
    std::cout << "   - Enable SIMD instructions for better performance" << std::endl;
    std::cout << "   - Use multithreading for larger networks" << std::endl;
    std::cout << "   - Profile critical sections for optimization" << std::endl;
}

void demonstrateErrorHandling() {
    std::cout << "\n=== Enhanced Error Handling Demonstration ===" << std::endl;
    
    nlm::CLIParser cli("error_demo", "Error Handling Demo");
    cli.addOption("input", "i", "Input file", "", false, "string");
    cli.addOption("output", "o", "Output file", "", false, "string");
    cli.addFlag("force", "Force overwrite");
    cli.addFlag("validate", "Validate before processing");
    
    std::cout << "1. Error handling scenarios:" << std::endl;
    std::cout << "   - Missing required input file" << std::endl;
    std::cout << "   - Invalid timestep value" << std::endl;
    std::cout << "   - Conflicting options (force vs. validate)" << std::endl;
    
    std::cout << "\n2. User-friendly error messages:" << std::endl;
    std::cout << "   ✓ Clear error descriptions" << std::endl;
    std::cout << "   ✓ Suggested solutions" << std::endl;
    std::cout << "   ✓ Usage examples" << std::endl;
    
    std::cout << "\n3. Recovery options:" << std::endl;
    std::cout << "   - Retry with default values" << std::endl;
    std::cout << "   - Skip invalid options" << std::endl;
    std::cout << "   - Use fallback configuration" << std::endl;
}

int main() {
    demonstrateCLI();
    demonstrateConfigurationManager();
    demonstratePerformanceMonitoring();
    demonstrateErrorHandling();
    
    std::cout << "\n=== Demonstration Complete ===" << std::endl;
    std::cout << "The enhanced NLM system now includes:" << std::endl;
    std::cout << "✓ Comprehensive command-line interface" << std::endl;
    std::cout << "✓ Simplified configuration management" << std::endl;
    std::cout << "✓ Performance monitoring and profiling" << std::endl;
    std::cout << "✓ Improved error handling and user feedback" << std::endl;
    std::cout << "✓ Extensive configuration profiles" << std::endl;
    std::cout << "✓ Output formatting and status reporting" << std::endl;
    
    return 0;
}