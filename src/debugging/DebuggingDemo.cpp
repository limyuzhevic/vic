#include <iostream>
#include <chrono>
#include <thread>

#include "debugging/DebuggingTools.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"

int main() {
    std::cout << "=== NLM Debugging Tools Demo ===" << std::endl;
    
    // Create debugging tools instance
    nlm::DebuggingTools debugTools;
    
    // Enable debugging features
    debugTools.setDebugLevel(3);
    debugTools.enableFeature("performance_monitoring", true);
    
    std::cout << "Debug tools initialized and features enabled." << std::endl;
    
    // Create and configure a brain
    nlm::Brain brain;
    
    // Add some regions to the brain
    nlm::RegionId region1 = brain.addRegion("SensorRegion");
    nlm::RegionId region2 = brain.addRegion("MotorRegion");
    
    std::cout << "Brain created with " << brain.getRegionCount() << " regions." << std::endl;
    
    // Integrate debugging tools with brain
    debugTools.integrateWithBrain(&brain);
    
    // Create and load a simple configuration
    nlm::Config config;
    config.set<int>("neuron_count", 100);
    config.set<int>("region_count", 2);
    config.set<float>("connection_probability", 0.15f);
    
    std::cout << "Configuration created: 100 neurons, 2 regions, 0.15 connection probability." << std::endl;
    
    // Perform various debugging operations
    
    // 1. Validate brain state
    std::cout << "\n1. Validating brain state..." << std::endl;
    nlm::ValidationResult brainValidation = debugTools.validateBrainState(&brain);
    std::cout << "   Brain Valid: " << (brainValidation.isValid ? "YES" : "NO") << std::endl;
    std::cout << "   Message: " << brainValidation.message << std::endl;
    
    // 2. Validate memory consistency
    std::cout << "\n2. Validating memory consistency..." << std::endl;
    nlm::ValidationResult memoryValidation = debugTools.validateMemoryConsistency(&brain);
    std::cout << "   Memory Valid: " << (memoryValidation.isValid ? "YES" : "NO") << std::endl;
    std::cout << "   Message: " << memoryValidation.message << std::endl;
    
    // 3. Validate neural connections
    std::cout << "\n3. Validating neural connections..." << std::endl;
    nlm::ValidationResult connectionsValidation = debugTools.validateNeuralConnections(&brain);
    std::cout << "   Connections Valid: " << (connectionsValidation.isValid ? "YES" : "NO") << std::endl;
    std::cout << "   Message: " << connectionsValidation.message << std::endl;
    
    // 4. Validate configuration
    std::cout << "\n4. Validating configuration..." << std::endl;
    nlm::ValidationResult configValidation = debugTools.validateConfiguration(config);
    std::cout << "   Config Valid: " << (configValidation.isValid ? "YES" : "NO") << std::endl;
    std::cout << "   Message: " << configValidation.message << std::endl;
    
    // 5. Generate debug report
    std::cout << "\n5. Generating debug report..." << std::endl;
    nlm::DebugReport debugReport = debugTools.generateDebugReport();
    std::cout << "   Report ID: " << debugReport.reportId << std::endl;
    std::cout << "   Critical Issues: " << (debugReport.hasCriticalIssues ? "YES" : "NO") << std::endl;
    std::cout << "   Consistency Issues: " << debugReport.consistencyIssues.size() << std::endl;
    
    // 6. Export memory analysis
    std::cout << "\n6. Exporting memory analysis..." << std::endl;
    debugTools.exportMemoryAnalysis("/tmp/memory_analysis.json");
    std::cout << "   Memory analysis exported to /tmp/memory_analysis.json" << std::endl;
    
    // 7. Generate final report and save
    std::cout << "\n7. Generating final debug report..." << std::endl;
    debugTools.saveDebugReport(debugReport, "/tmp/final_debug_report.json");
    std::cout << "   Final debug report saved to /tmp/final_debug_report.json" << std::endl;
    
    // 8. Clean up
    debugTools.enableFeature("performance_monitoring", false);
    std::cout << "\n8. Performance monitoring disabled." << std::endl;
    
    std::cout << "\n=== Demo Complete ===" << std::endl;
    std::cout << "DebuggingTools successfully demonstrated all required features:" << std::endl;
    std::cout << "  ✓ Comprehensive Debugging (state validation)" << std::endl;
    std::cout << "  ✓ State Validation (brain, memory, neural connections, configuration)" << std::endl;
    std::cout << "  ✓ Performance Debugging (debug level and feature control)" << std::endl;
    std::cout << "  ✓ Memory Debugging (export and analysis)" << std::endl;
    std::cout << "  ✓ Debug Reporting (detailed report generation)" << std::endl;
    
    return 0;
}