// DebugSystem Integration Test
// This file demonstrates the usage of the comprehensive debugging system

#include "debug/DebugTrace.hpp"
#include "debug/DebuggingSystem.hpp"
#include "debug/PerformanceProfiler.hpp"
#include "debug/EventLogger.hpp"
#include "debug/MemoryProfiler.hpp"
#include "debug/DebugVisualizer.hpp"

// Include existing NLM components for integration
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "visualization/VisualizationInterface.hpp"

void testDebugSystem() {
    // Initialize debugging system
    nlm::DebugConfiguration config;
    config.enableTrace = true;
    config.maxTraceEvents = 10000;
    config.enablePerformanceProfiling = true;
    config.enableMemoryTracking = true;
    config.enableLogging = true;
    config.enableConsoleOutput = true;
    
    auto debugSystem = std::make_shared<nlm::DebuggingSystem>(config);
    
    // Initialize performance profiler
    auto profiler = std::make_shared<nlm::PerformanceProfiler>(debugSystem);
    
    // Initialize event logger
    auto eventLogger = std::make_shared<nlm::EventLogger>(debugSystem);
    
    // Initialize memory profiler
    auto memoryProfiler = std::make_shared<nlm::MemoryProfiler>(debugSystem);
    
    // Start debugging system
    debugSystem->start();
    
    // Test performance measurement
    auto measurement = profiler->startMeasurement("brain_step");
    // Simulate work
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    profiler->stopMeasurement(measurement);
    
    // Test event logging
    eventLogger->logEvent(nlm::EventType::StepCompleted, "brain", "Step completed successfully");
    
    // Test memory tracking
    memoryProfiler->recordAllocation(1024, "neuron_memory", "brain_step");
    memoryProfiler->recordDeallocation(1024, "neuron_memory", "brain_step");
    
    // Test DebugVisualizer
    auto visualizer = std::make_shared<nlm::DebugVisualizer>(debugSystem);
    visualizer->setEnableWebInterface(true);
    visualizer->setPort(8080);
    
    // Integration with NLM components
    // (In a real implementation, these would be actual NLM component instances)
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>());
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    auto visualization = std::make_shared<nlm::VisualizationInterface>();
    
    debugSystem->integrateWithBrain(brain);
    debugSystem->integrateWithAgent(agentBrain);
    debugSystem->integrateWithVisualization(visualization);
    
    // Export debug data
    debugSystem->exportDebugData("debug_output.txt");
    profiler->exportProfile("performance_profile.txt");
    memoryProfiler->exportMemoryProfile("memory_profile.txt");
    
    // Log system status
    debugSystem->logStatus();
    
    // Stop debugging system
    debugSystem->stop();
}

int main() {
    nlm::Logger::getGlobal().info("Starting DebugSystem Integration Test");
    testDebugSystem();
    nlm::Logger::getGlobal().info("DebugSystem Integration Test completed");
    return 0;
}
