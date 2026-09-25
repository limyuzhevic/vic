// LoggerIntegration.cpp - Integration examples for the logging system

#include "core/Logger/AdvancedLogger.hpp"
#include "core/Logger/PerformanceMonitor.hpp"
#include "core/Logger/RuntimeMetricsCollector.hpp"
#include "core/Logger/LogFormatter.hpp"
#include "core/Logger/MetricCalculator.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

namespace nlm {

// Integration helper functions
void setupLoggingSystem() {
    auto& logger = AdvancedLogger::getInstance();
    auto& performanceMonitor = PerformanceMonitor::getInstance();
    auto& metricsCollector = RuntimeMetricsCollector::getInstance();
    
    // Initialize logger with comprehensive configuration
    logger.initialize("nlm.log", LogLevel::Info);
    
    // Enable all logging features
    logger.enableColors(true);
    logger.enableAsyncLogging(true);
    logger.enablePerformanceLogging(true);
    
    // Set up logging outputs
    logger.clearLogOutputs();
    logger.addLogOutput(std::make_shared<ConsoleLogOutput>(LogLevel::Info));
    logger.addLogOutput(std::make_shared<FileLogOutput>("nlm_detailed.log", 50 * 1024 * 1024, 5));
    logger.addLogOutput(std::make_shared<JSONLogOutput>("nlm_structured.json"));
    logger.addLogOutput(std::make_shared<AsyncLogOutput>(10000));
    
    // Configure performance monitoring
    performanceMonitor.setCollectionInterval(std::chrono::milliseconds(100));
    performanceMonitor.enableSystemMonitoring(true);
    performanceMonitor.enableNetworkMonitoring(false);
    
    // Configure runtime metrics collection
    metricsCollector.setCollectionInterval(std::chrono::milliseconds(100));
    metricsCollector.setMaxHistorySize(10000);
    metricsCollector.enableRealTimeUpdates(true);
    
    logger.info("Logging system initialized successfully");
}

void integrateWithBrain(AdvancedLogger& logger, PerformanceMonitor& perfMonitor, 
                       RuntimeMetricsCollector& metricsCollector, Brain& brain) {
    logger.info("Integrating logging with Brain component");
    
    // Record brain configuration metrics
    auto config = brain.getConfig();
    if (config) {
        auto neuronCount = config->getOr<int>("neuron_count", 0);
        auto regionCount = config->getOr<int>("region_count", 0);
        auto synapticDensity = config->getOr<double>("synaptic_density", 0.0);
        
        perfMonitor.recordMetric("config_neuron_count", neuronCount);
        perfMonitor.recordMetric("config_region_count", regionCount);
        perfMonitor.recordMetric("config_synaptic_density", synapticDensity);
        
        logger.logStructured(LogLevel::Info, "BrainConfiguration",
                           {{ "neuron_count", std::to_string(neuronCount) },
                            { "region_count", std::to_string(regionCount) },
                            { "synaptic_density", std::to_string(synapticDensity) }},
                           "Brain configuration loaded");
    }
    
    // Set up brain integration monitoring
    perfMonitor.recordBrainMetrics(brain);
    metricsCollector.updateFromBrain(brain);
    
    logger.info("Brain integration completed");
}

void integrateWithSensorySystems(AdvancedLogger& logger, PerformanceMonitor& perfMonitor) {
    logger.info("Integrating logging with sensory systems");
    
    // Simulate vision system logging
    perfMonitor.startProfiling("vision_processing");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    perfMonitor.stopProfiling("vision_processing");
    
    // Simulate audio system logging
    perfMonitor.startProfiling("audio_processing");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    perfMonitor.stopProfiling("audio_processing");
    
    // Log sensory metrics
    perfMonitor.recordMetric("vision_update_count", 1);
    perfMonitor.recordMetric("audio_update_count", 1);
    
    logger.info("Sensory systems integration completed");
}

void integrateWithMotorSystems(AdvancedLogger& logger, PerformanceMonitor& perfMonitor) {
    logger.info("Integrating logging with motor systems");
    
    // Simulate motor system logging
    perfMonitor.startProfiling("motor_command_execution");
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
    perfMonitor.stopProfiling("motor_command_execution");
    
    // Log motor metrics
    perfMonitor.recordMetric("motor_command_count", 1);
    perfMonitor.recordMetric("action_selection_time", 0.015);
    
    logger.info("Motor systems integration completed");
}

void integrateWithMemorySystems(AdvancedLogger& logger, PerformanceMonitor& perfMonitor,
                                RuntimeMetricsCollector& metricsCollector) {
    logger.info("Integrating logging with memory systems");
    
    // Record memory pool metrics
    perfMonitor.recordMemoryPoolMetrics("working_memory", 1000, 5000, 25000);
    perfMonitor.recordMemoryPoolMetrics("episodic_memory", 500, 2000, 10000);
    perfMonitor.recordMemoryPoolMetrics("associative_memory", 200, 1000, 5000);
    
    metricsCollector.updateMemoryPool("working_memory", 1000, 5000, 25000);
    metricsCollector.updateMemoryPool("episodic_memory", 500, 2000, 10000);
    metricsCollector.updateMemoryPool("associative_memory", 200, 1000, 5000);
    
    logger.info("Memory systems integration completed");
}

void integrateWithNeuromodulation(AdvancedLogger& logger, PerformanceMonitor& perfMonitor,
                                  RuntimeMetricsCollector& metricsCollector) {
    logger.info("Integrating logging with neuromodulation systems");
    
    // Simulate neuromodulator metrics
    perfMonitor.recordNeuromodulatorMetrics("dopamine", 0.7, 0.8);
    perfMonitor.recordNeuromodulatorMetrics("curiosity", 0.5, 0.6);
    perfMonitor.recordNeuromodulatorMetrics("novelty", 0.8, 0.9);
    perfMonitor.recordNeuromodulatorMetrics("reward", 0.9, 0.95);
    
    metricsCollector.updateNeuromodulator("dopamine", 0.7, 0.8);
    metricsCollector.updateNeuromodulator("curiosity", 0.5, 0.6);
    metricsCollector.updateNeuromodulator("novelty", 0.8, 0.9);
    metricsCollector.updateNeuromodulator("reward", 0.9, 0.95);
    
    logger.info("Neuromodulation systems integration completed");
}

void integrateWithDevelopment(AdvancedLogger& logger, PerformanceMonitor& perfMonitor,
                             RuntimeMetricsCollector& metricsCollector) {
    logger.info("Integrating logging with development system");
    
    // Simulate developmental stage
    perfMonitor.recordMetric("development_stage", 3); // Phase 3
    perfMonitor.recordMetric("plasticity_rate", 0.7);
    perfMonitor.recordMetric("excitatory_inhibition_ratio", 1.2);
    
    metricsCollector.updateDevelopment(DevelopmentalStage::Phase3, 0.6, 0.7, 0.1);
    
    logger.info("Development systems integration completed");
}

void setupAlertsAndThresholds(AdvancedLogger& logger, PerformanceMonitor& perfMonitor) {
    logger.info("Setting up alerts and thresholds");
    
    // Memory usage alert
    perfMonitor.addAlert({"memory_usage_alert", "memory.used_percent", 
                         LogLevel::Warning, 0.8, 
                         "Memory usage exceeded 80% threshold"});
    
    // CPU usage alert
    perfMonitor.addAlert({"cpu_usage_alert", "cpu.usage_percent",
                         LogLevel::Warning, 0.9,
                         "CPU usage exceeded 90% threshold"});
    
    // Spike rate alert
    perfMonitor.addAlert({"spike_rate_alert", "simulation.spike_count",
                         LogLevel::Warning, 1000,
                         "Spike count exceeded 1000 per second"});
    
    logger.info("Alerts and thresholds setup completed");
}

void configureLoggingForProduction(AdvancedLogger& logger) {
    logger.info("Configuring logging for production environment");
    
    // Performance optimizations
    logger.enableColors(false); // Disable colors in production
    logger.enableAsyncLogging(true); // Enable async logging for performance
    
    // Log filtering
    LogFilter productionFilter;
    productionFilter.minLevel = LogLevel::Warning; // Only warnings and above
    productionFilter.categories = { "brain", "simulation", "error" };
    productionFilter.includePerformanceLogs = false;
    logger.setFilter(productionFilter);
    
    logger.info("Production logging configuration completed");
}

void configureLoggingForDevelopment(AdvancedLogger& logger) {
    logger.info("Configuring logging for development environment");
    
    // Enable all logging features for development
    logger.enableColors(true);
    logger.enableAsyncLogging(true);
    logger.enablePerformanceLogging(true);
    
    // Log filtering
    LogFilter devFilter;
    devFilter.minLevel = LogLevel::Debug;
    devFilter.categories = { "brain", "simulation", "memory", "neuromodulation" };
    devFilter.includePerformanceLogs = true;
    logger.setFilter(devFilter);
    
    // Enable all outputs
    logger.clearLogOutputs();
    logger.addLogOutput(std::make_shared<ConsoleLogOutput>(LogLevel::Debug));
    logger.addLogOutput(std::make_shared<FileLogOutput>("nlm_dev.log"));
    
    logger.info("Development logging configuration completed");
}

void demonstrateStructuredLogging(AdvancedLogger& logger) {
    logger.info("Demonstrating structured logging");
    
    // Log with context
    logger.logStructured(LogLevel::Info, "SimulationStep",
                        {{ "step", "100" },
                         { "neurons_active", "42" },
                         { "firing_rate", "5.2" }},
                        "Simulation step completed");
    
    // Log with performance metrics
    PerformanceMetrics metrics;
    metrics.operation = "brain_initialization";
    metrics.duration = 2.5;
    metrics.memoryUsed = 50 * 1024 * 1024; // 50 MB
    metrics.memoryPercent = 15.3;
    
    logger.logPerformance(LogLevel::Info, "brain_initialization", metrics);
    
    // Log with custom metrics
    logger.recordMetric("neuron_count", 1024);
    logger.recordMetric("firing_rate", 5.2);
    logger.recordMetric("plasticity_strength", 0.7);
    
    logger.info("Structured logging demonstration completed");
}

void demonstratePerformanceMonitoring(AdvancedLogger& logger, PerformanceMonitor& perfMonitor) {
    logger.info("Demonstrating performance monitoring");
    
    // Start profiling
    perfMonitor.startProfiling("brain_initialization", {{"component", "brain"}});
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    perfMonitor.stopProfiling("brain_initialization");
    
    // Record custom metrics
    perfMonitor.recordMetric("neuron_count", 1024);
    perfMonitor.recordMetric("firing_rate", 5.2);
    perfMonitor.recordTiming("decision_making", 0.15);
    perfMonitor.recordMemory(50 * 1024 * 1024, 100 * 1024 * 1024); // 50MB used, 100MB peak
    
    // Update system metrics
    perfMonitor.updateSystemMetrics();
    
    // Check alerts
    perfMonitor.checkAlerts();
    
    logger.info("Performance monitoring demonstration completed");
}

void demonstrateRuntimeMetrics(AdvancedLogger& logger, RuntimeMetricsCollector& metricsCollector) {
    logger.info("Demonstrating runtime metrics collection");
    
    // Record brain state metrics
    BrainStateMetrics brainMetrics;
    brainMetrics.totalNeuronCount = 1024;
    brainMetrics.activeNeuronCount = 42;
    brainMetrics.firingNeuronCount = 38;
    brainMetrics.averageFiringRate = 5.2f;
    brainMetrics.totalSpikeCount = 1000;
    brainMetrics.excitationInhibitionRatio = 1.2f;
    
    metricsCollector.recordBrainMetrics(brainMetrics);
    
    // Record simulation metrics
    SimulationMetrics simMetrics;
    simMetrics.step = 100;
    simMetrics.stepDuration = std::chrono::milliseconds(50);
    simMetrics.spikeCount = 1000;
    simMetrics.firingRate = 5.2f;
    simMetrics.activeNeuronCount = 42;
    
    metricsCollector.recordSimulationMetrics(simMetrics);
    
    // Record memory pool metrics
    MemoryPoolMetrics memoryMetrics;
    memoryMetrics.poolName = "working_memory";
    memoryMetrics.currentSize = 1000;
    memoryMetrics.maxSize = 5000;
    memoryMetrics.totalAllocated = 25000;
    memoryMetrics.utilizationPercent = 20.0;
    
    metricsCollector.recordMemoryPoolMetrics(memoryMetrics);
    
    logger.info("Runtime metrics demonstration completed");
}

void demonstrateLogFormatting(AdvancedLogger& logger, LogFormatter& formatter) {
    logger.info("Demonstrating log formatting");
    
    // Export logs to different formats
    formatter.exportToFile("test.log", "nlm_logs.json", "json");
    formatter.exportToFile("test.log", "nlm_logs.csv", "csv");
    formatter.exportToFile("test.log", "nlm_logs.html", "html");
    
    // Generate report
    std::string report = formatter.generateReport("nlm_logs.json");
    logger.logStructured(LogLevel::Info, "ReportGeneration",
                        {{ "file_count", "3" },
                         { "report_size", std::to_string(report.size()) }},
                        "Log report generated successfully");
    
    logger.info("Log formatting demonstration completed");
}

void demonstrateIntegration(AdvancedLogger& logger, PerformanceMonitor& perfMonitor,
                          RuntimeMetricsCollector& metricsCollector, LogFormatter& formatter,
                          Brain& brain) {
    logger.info("Starting comprehensive integration demonstration");
    
    // Demonstrate all integration features
    demonstrateStructuredLogging(logger);
    demonstratePerformanceMonitoring(logger, perfMonitor);
    demonstrateRuntimeMetrics(logger, metricsCollector);
    demonstrateLogFormatting(logger, formatter);
    
    // Integrate with all major systems
    integrateWithBrain(logger, perfMonitor, metricsCollector, brain);
    integrateWithSensorySystems(logger, perfMonitor);
    integrateWithMotorSystems(logger, perfMonitor);
    integrateWithMemorySystems(logger, perfMonitor, metricsCollector);
    integrateWithNeuromodulation(logger, perfMonitor, metricsCollector);
    integrateWithDevelopment(logger, perfMonitor, metricsCollector);
    
    // Set up alerts
    setupAlertsAndThresholds(logger, perfMonitor);
    
    logger.info("Comprehensive integration demonstration completed");
}

} // namespace nlm