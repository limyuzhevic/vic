// demo_logging.cpp - Demonstration program for the enhanced logging and monitoring system

#include "core/Logger/AdvancedLogger.hpp"
#include "core/Logger/PerformanceMonitor.hpp"
#include "core/Logger/RuntimeMetricsCollector.hpp"
#include "core/Logger/LogFormatter.hpp"
#include "core/Logger/MetricCalculator.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

namespace nlm {

void simulateBrainOperation(Brain& brain, AdvancedLogger& logger, 
                           PerformanceMonitor& perfMonitor, RuntimeMetricsCollector& metricsCollector) {
    // Simulate brain operation
    logger.info("Starting brain simulation");
    
    // Profile brain initialization
    perfMonitor.startProfiling("brain_initialization", {{"component", "brain"}});
    brain.initialize();
    perfMonitor.stopProfiling("brain_initialization");
    
    // Simulate neural activity
    for (int step = 1; step <= 10; ++step) {
        logger.info("Simulation step " + std::to_string(step));
        
        // Profile simulation step
        perfMonitor.startProfiling("simulation_step", {{"step", std::to_string(step)}});
        
        // Simulate some brain activity
        brain.step(step);
        
        // Simulate spike generation
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        perfMonitor.stopProfiling("simulation_step");
        
        // Record brain metrics
        perfMonitor.recordBrainMetrics(brain);
        metricsCollector.updateFromBrain(brain);
        
        // Record simulation metrics
        SimulationMetrics simMetrics;
        simMetrics.step = step;
        simMetrics.stepDuration = std::chrono::milliseconds(50);
        simMetrics.spikeCount = 100 + (step * 10); // Simulate increasing spike count
        simMetrics.firingRate = 5.0f + (step * 0.5f);
        simMetrics.activeNeuronCount = 40 + (step * 5);
        
        perfMonitor.recordSimulationMetrics(simMetrics.step, simMetrics.spikeCount, simMetrics.firingRate);
        metricsCollector.recordSimulationMetrics(simMetrics);
        
        // Log every 5 steps
        if (step % 5 == 0) {
            logger.logStructured(LogLevel::Info, "SimulationProgress",
                               {{ "step", std::to_string(step) },
                                { "spike_count", std::to_string(simMetrics.spikeCount) },
                                { "firing_rate", std::to_string(simMetrics.firingRate) },
                                { "active_neurons", std::to_string(simMetrics.activeNeuronCount) }},
                               "Halfway through simulation");
        }
    }
    
    logger.info("Brain simulation completed successfully");
}

void demonstratePerformanceMonitoring(AdvancedLogger& logger, PerformanceMonitor& perfMonitor) {
    logger.info("Demonstrating performance monitoring features");
    
    // Start various operations to profile
    std::vector<std::string> operations = {
        "memory_allocation", "spike_propagation", "plasticity_update", 
        "synapse_creation", "neuron_activation"
    };
    
    for (const auto& op : operations) {
        perfMonitor.startProfiling(op);
        
        // Simulate work
        std::this_thread::sleep_for(std::chrono::milliseconds(20 + (rand() % 30)));
        
        perfMonitor.stopProfiling(op);
        perfMonitor.recordMetric("operation_count", 1);
    }
    
    // Update system metrics
    perfMonitor.updateSystemMetrics();
    
    // Record custom metrics
    perfMonitor.recordMetric("total_neurons", 1024);
    perfMonitor.recordMetric("total_synapses", 5120);
    perfMonitor.recordTiming("decision_making", 0.15);
    perfMonitor.recordMemory(50 * 1024 * 1024, 100 * 1024 * 1024); // 50MB used, 100MB peak
    
    // Check alerts
    perfMonitor.checkAlerts();
    
    logger.info("Performance monitoring demonstration completed");
}

void demonstrateRuntimeMetrics(AdvancedLogger& logger, RuntimeMetricsCollector& metricsCollector) {
    logger.info("Demonstrating runtime metrics collection");
    
    // Simulate brain state metrics
    BrainStateMetrics brainMetrics;
    brainMetrics.totalNeuronCount = 1024;
    brainMetrics.activeNeuronCount = 128;
    brainMetrics.firingNeuronCount = 96;
    brainMetrics.averageFiringRate = 4.8f;
    brainMetrics.totalSpikeCount = 5000;
    brainMetrics.excitationInhibitionRatio = 1.1f;
    
    metricsCollector.recordBrainMetrics(brainMetrics);
    
    // Simulate memory pool metrics
    std::vector<std::string> pools = {"working_memory", "episodic_memory", "associative_memory"};
    for (const auto& pool : pools) {
        MemoryPoolMetrics poolMetrics;
        poolMetrics.poolName = pool;
        poolMetrics.currentSize = (pool == "working_memory") ? 1024 : (pool == "episodic_memory") ? 512 : 256;
        poolMetrics.maxSize = poolMetrics.currentSize * 5;
        poolMetrics.totalAllocated = poolMetrics.currentSize * 3;
        poolMetrics.utilizationPercent = 20.0 + (rand() % 30);
        
        metricsCollector.recordMemoryPoolMetrics(poolMetrics);
    }
    
    // Simulate event metrics
    EventMetrics eventMetrics;
    eventMetrics.queueDepth = 50;
    eventMetrics.processingLatency = 25; // microseconds
    eventMetrics.eventCount = 1000;
    eventMetrics.throughput = 1000.0 / 60.0; // events per second
    
    metricsCollector.recordEventMetrics(eventMetrics);
    
    // Simulate neuromodulator metrics
    std::vector<std::string> neuromodulators = {"dopamine", "curiosity", "novelty", "reward"};
    for (const auto& mod : neuromodulators) {
        NeuromodulatorMetrics modMetrics;
        modMetrics.type = mod;
        modMetrics.currentLevel = 0.5 + (rand() % 100) / 100.0;
        modMetrics.previousLevel = modMetrics.currentLevel - (rand() % 20) / 100.0;
        modMetrics.targetLevel = 0.7 + (rand() % 30) / 100.0;
        modMetrics.changeRate = (rand() % 10) / 100.0;
        modMetrics.effectMagnitude = 0.5 + (rand() % 50) / 100.0;
        modMetrics.effectDuration = 10 + (rand() % 20);
        
        metricsCollector.recordNeuromodulatorMetrics(modMetrics);
    }
    
    // Simulate developmental stage metrics
    DevelopmentalStageMetrics devMetrics;
    devMetrics.stage = DevelopmentalStage::Phase3;
    devMetrics.developmentProgress = 0.6;
    devMetrics.plasticityRate = 0.7;
    devMetrics.excitationChangeRate = 0.1;
    
    metricsCollector.recordDevelopmentalStageMetrics(devMetrics);
    
    logger.info("Runtime metrics demonstration completed");
}

void demonstrateLogFormatting(AdvancedLogger& logger, LogFormatter& formatter) {
    logger.info("Demonstrating log formatting capabilities");
    
    // Create sample data
    std::vector<LogEntry> sampleLogs;
    for (int i = 0; i < 5; ++i) {
        LogEntry entry;
        entry.level = LogLevel::Info;
        entry.message = "Log entry " + std::to_string(i);
        entry.category = "test";
        entry.file = "demo_logging.cpp";
        entry.line = 100 + i;
        entry.function = "main";
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
        sampleLogs.push_back(entry);
    }
    
    // Export logs in different formats
    formatter.exportToFile("demo_logs", "demo_logs.json", "json");
    formatter.exportToFile("demo_logs", "demo_logs.csv", "csv");
    formatter.exportToFile("demo_logs", "demo_logs.html", "html");
    
    // Generate report
    std::string report = formatter.generateReport("demo_logs.json");
    logger.logStructured(LogLevel::Info, "ReportGeneration",
                       {{ "format", "json" },
                        { "entry_count", std::to_string(sampleLogs.size()) },
                        { "report_size", std::to_string(report.size()) }},
                       "Log report generated");
    
    logger.info("Log formatting demonstration completed");
}

void demonstrateMetricCalculation(AdvancedLogger& logger, MetricCalculator& calculator) {
    logger.info("Demonstrating metric calculation capabilities");
    
    // Generate sample performance data
    std::vector<double> performanceValues;
    for (int i = 0; i < 20; ++i) {
        performanceValues.push_back(100 + (rand() % 100));
    }
    
    // Analyze performance
    StatisticalAnalysis stats = calculator.analyze(performanceValues);
    logger.logStructured(LogLevel::Info, "PerformanceAnalysis",
                       {{ "mean", std::to_string(stats.mean) },
                        { "median", std::to_string(stats.median) },
                        { "std_dev", std::to_string(stats.standardDeviation) },
                        { "min", std::to_string(stats.min) },
                        { "max", std::to_string(stats.max) },
                        { "sample_size", std::to_string(stats.sampleSize) }},
                       "Performance statistics calculated");
    
    // Demonstrate baseline comparison
    PerformanceComparison comparison = calculator.compareWithBaseline(
        "test_operation", 150.0);
    logger.logStructured(LogLevel::Info, "BaselineComparison",
                       {{ "baseline_duration", std::to_string(comparison.baselineDuration) },
                        { "current_duration", std::to_string(comparison.currentDuration) },
                        { "improvement_percent", std::to_string(comparison.improvementPercent) },
                        { "is_better", comparison.isBetter ? "true" : "false"}},
                       "Performance baseline comparison completed");
    
    // Demonstrate anomaly detection
    AnomalyDetection anomaly = calculator.detectAnomaly(performanceValues, 2.0);
    logger.logStructured(LogLevel::Info, "AnomalyDetection",
                       {{ "is_anomaly", anomaly.isAnomaly ? "true" : "false" },
                        { "anomaly_score", std::to_string(anomaly.anomalyScore) },
                        { "anomaly_type", anomaly.anomalyType }},
                       "Anomaly detection completed");
    
    logger.info("Metric calculation demonstration completed");
}

void demonstrateIntegration(AdvancedLogger& logger, PerformanceMonitor& perfMonitor,
                          RuntimeMetricsCollector& metricsCollector, LogFormatter& formatter,
                          MetricCalculator& calculator, Brain& brain) {
    logger.info("Starting comprehensive integration demonstration");
    
    // Setup logging system
    setupLoggingSystem();
    
    // Run brain simulation
    simulateBrainOperation(brain, logger, perfMonitor, metricsCollector);
    
    // Demonstrate monitoring features
    demonstratePerformanceMonitoring(logger, perfMonitor);
    demonstrateRuntimeMetrics(logger, metricsCollector);
    
    // Demonstrate formatting and calculation
    demonstrateLogFormatting(logger, formatter);
    demonstrateMetricCalculation(logger, calculator);
    
    // Generate summary report
    logger.logStructured(LogLevel::Info, "IntegrationSummary",
                       {{ "status", "success" },
                        { "brain_initialized", "true" },
                        { "simulation_steps", "10" },
                        { "log_formats", "3" },
                        { "metrics_calculated", "true" }},
                       "Complete logging system integration demonstration");
    
    logger.info("Comprehensive integration demonstration completed successfully");
}

int main() {
    std::cout << "=== NLM Enhanced Logging and Monitoring System ===" << std::endl;
    std::cout << "Demonstrating comprehensive logging and monitoring capabilities" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    try {
        // Initialize logging system
        setupLoggingSystem();
        
        // Get references to all logging components
        auto& logger = AdvancedLogger::getInstance();
        auto& perfMonitor = PerformanceMonitor::getInstance();
        auto& metricsCollector = RuntimeMetricsCollector::getInstance();
        auto& formatter = LogFormatter::getInstance();
        auto& calculator = MetricCalculator::getInstance();
        
        // Create brain for demonstration
        Brain brain(Config());
        
        // Run demonstration
        demonstrateIntegration(logger, perfMonitor, metricsCollector, 
                             formatter, calculator, brain);
        
        std::cout << "\n=== Demonstration Completed Successfully ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during demonstration: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error during demonstration" << std::endl;
        return 1;
    }
}

} // namespace nlm