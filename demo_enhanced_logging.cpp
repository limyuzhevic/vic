// This file should be in a test or examples directory
// For now, let's create a simple main that demonstrates the enhanced logging

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Enhanced Logging and Debugging Demo ===" << std::endl;
    std::cout << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("simulation_timestep", 0.001);
    config->set("debug_mode", true);
    config->set("performance_monitoring", true);
    config->set("system_health_checks", true);
    config->set("error_codes_enabled", true);
    config->set("event_logging", true);
    config->set("connection_tracking", true);
    
    // Create brain with configuration
    nlm::Brain brain(config);
    
    // Initialize brain
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    std::cout << std::endl;
    
    // Enable all debugging features
    brain.enableDebugMode(true);
    brain.enablePerformanceMonitoring(true);
    brain.enableSystemHealthChecks(true);
    brain.enableErrorCodes(true);
    brain.enableEventLogging(true);
    brain.enableConnectionTracking(true);
    
    std::cout << "All debugging features enabled" << std::endl;
    std::cout << std::endl;
    
    // Perform system health check
    bool isHealthy = brain.performSystemHealthCheck();
    std::cout << "System health check: " << (isHealthy ? "PASSED" : "FAILED") << std::endl;
    std::cout << std::endl;
    
    // Validate configuration
    bool configValid = brain.validateConfiguration();
    std::cout << "Configuration validation: " << (configValid ? "PASSED" : "FAILED") << std::endl;
    std::cout << std::endl;
    
    // Validate connections
    bool connectionsValid = brain.validateConnections();
    std::cout << "Connection validation: " << (connectionsValid ? "PASSED" : "FAILED") << std::endl;
    std::cout << std::endl;
    
    // Get system statistics
    auto stats = brain.getSystemStats();
    std::cout << "System Statistics:" << std::endl;
    std::cout << "  Total neurons: " << stats.totalNeurons << std::endl;
    std::cout << "  Total synapses: " << stats.totalSynapses << std::endl;
    std::cout << "  Total spikes: " << stats.totalSpikes << std::endl;
    std::cout << "  Active neurons: " << stats.activeNeurons << std::endl;
    std::cout << "  Firing neurons: " << stats.firingNeurons << std::endl;
    std::cout << "  Average firing rate: " << stats.averageFiringRate << std::endl;
    std::cout << "  E/I ratio: " << stats.excitationInhibitionRatio << std::endl;
    std::cout << "  Memory traces: " << stats.memoryTraces << std::endl;
    std::cout << "  Episodic episodes: " << stats.episodicEpisodes << std::endl;
    std::cout << "  Dopamine level: " << stats.dopamineLevel << std::endl;
    std::cout << "  System uptime: " << stats.systemUptime << " seconds" << std::endl;
    std::cout << std::endl;
    
    // Get performance metrics
    auto metrics = brain.getPerformanceMetrics();
    std::cout << "Performance Metrics:" << std::endl;
    std::cout << "  Steps count: " << metrics.stepsCount << std::endl;
    std::cout << "  Last step time: " << metrics.lastStepTime << " s" << std::endl;
    std::cout << "  Average step time: " << metrics.averageStepTime << " s" << std::endl;
    std::cout << "  Min step time: " << metrics.minStepTime << " s" << std::endl;
    std::cout << "  Max step time: " << metrics.maxStepTime << " s" << std::endl;
    std::cout << std::endl;
    
    // Get connections
    auto connections = brain.getConnections();
    std::cout << "Connections: " << connections.size() << std::endl;
    for (const auto& conn : connections) {
        std::cout << "  " << conn.sourceSystem << " -> " << conn.targetSystem 
                  << " (weight: " << conn.weight << ", active: " << (conn.isActive ? "yes" : "no") << ")" << std::endl;
    }
    std::cout << std::endl;
    
    // Log status
    std::cout << "=== Brain Status ===" << std::endl;
    brain.logStatus();
    std::cout << std::endl;
    
    // Get debug info
    std::cout << "=== Debug Information ===" << std::endl;
    std::cout << brain.getDebugInfo() << std::endl;
    std::cout << std::endl;
    
    // Get health report
    std::cout << "=== Health Report ===" << std::endl;
    std::cout << brain.getHealthReport() << std::endl;
    std::cout << std::endl;
    
    // Simulate a few steps
    std::cout << "=== Simulating Steps ===" << std::endl;
    for (size_t i = 0; i < 5; ++i) {
        std::cout << "Step " << i << ": ";
        brain.step(i, i * config->getOr<double>("simulation_timestep", 0.001));
        std::cout << "completed" << std::endl;
    }
    std::cout << std::endl;
    
    // Final status
    std::cout << "=== Final Status ===" << std::endl;
    brain.logStatus();
    std::cout << std::endl;
    
    // Generate final reports
    std::cout << "=== Final Reports ===" << std::endl;
    std::cout << "Debug Info:" << std::endl;
    std::cout << brain.getDebugInfo() << std::endl;
    std::cout << std::endl;
    
    std::cout << "Health Report:" << std::endl;
    std::cout << brain.getHealthReport() << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Demo Complete ===" << std::endl;
    
    return 0;
}
