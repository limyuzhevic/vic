//!/usr/bin/env cpp
// Example: NLM Performance Monitoring Integration
//
// This example demonstrates how to integrate the PerformanceSystem
// with existing NLM subsystems and use it in a real application.

#include "performance/PerformanceSystem.hpp"
#include "core/Logger/Logger.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"
#include "brain/Brain.hpp"
#include "memory/MemoryPool.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <string>

// Example NLM application using PerformanceSystem
class ExampleNLMApplication {
public:
    ExampleNLMApplication() : performanceSystem_(nlm::PerformanceSystem::getInstance()) {
        // Initialize all NLM subsystems
        initializeBrain();
        initializeMemory();
        initializeConfig();
        initializeLogger();
        
        // Set up PerformanceSystem callbacks
        setupPerformanceCallbacks();
        
        // Configure performance monitoring
        configurePerformanceMonitoring();
        
        // Start performance monitoring
        performanceSystem_.startRealTimeMonitoring(std::chrono::milliseconds(100));
        
        std::cout << "NLM Performance Monitoring Example Initialized" << std::endl;
    }
    
    ~ExampleNLMApplication() {
        shutdown();
    }
    
    void run() {
        std::cout << "Starting NLM simulation with performance monitoring..." << std::endl;
        
        // Main simulation loop
        for (uint64_t step = 0; step < maxSteps_; ++step) {
            auto stepStart = std::chrono::high_resolution_clock::now();
            
            // Update performance metrics
            performanceSystem_.updateSnapshot();
            
            // Run simulation step
            runSimulationStep(step);
            
            // Check for performance issues
            performanceSystem_.checkThresholds();
            
            auto stepEnd = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stepEnd - stepStart);
            
            // Log performance for this step
            NLM_LOG_INFO("Step " + std::to_string(step) + " completed in " + 
                        std::to_string(duration.count()) + "ms");
        
            // Wait for next step (simulate real-time)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        std::cout << "NLM simulation completed" << std::endl;
    }
    
    void shutdown() {
        std::cout << "Shutting down NLM application..." << std::endl;
        
        // Stop performance monitoring
        performanceSystem_.stopRealTimeMonitoring();
        
        // Shutdown NLM subsystems
        shutdownLogger();
        shutdownConfig();
        shutdownMemory();
        shutdownBrain();
        
        // Shutdown performance system
        performanceSystem_.shutdown();
        
        std::cout << "NLM application shutdown complete" << std::endl;
    }
    
private:
    void initializeBrain() {
        std::cout << "Initializing Brain subsystem..." << std::endl;
        
        // Create brain configuration
        auto config = std::make_shared<nlm::BrainConfig>();
        config->regionCount = 4;
        config->neuronCountPerRegion = 10000;
        config->enablePlasticity = true;
        
        // Create brain
        brain_ = std::make_unique<nlm::Brain>(config);
        
        // Initialize brain
        brain_->initialize();
        
        std::cout << "Brain subsystem initialized" << std::endl;
    }
    
    void initializeMemory() {
        std::cout << "Initializing Memory subsystem..." << std::endl;
        
        // Create memory pool configuration
        auto config = std::make_shared<nlm::MemoryPoolConfig>();
        config->neuronPoolSize = 1000000;
        config->synapsePoolSize = 10000000;
        config->workingMemorySize = 100 * 1024 * 1024;  // 100MB
        config->enableMemoryProfiling = true;
        
        // Create memory pool
        memoryPool_ = std::make_unique<nlm::MemoryPool>(config);
        
        // Initialize memory pool
        memoryPool_->initialize();
        
        std::cout << "Memory subsystem initialized" << std::endl;
    }
    
    void initializeConfig() {
        std::cout << "Initializing Configuration subsystem..." << std::endl;
        
        // Create configuration
        auto config = std::make_shared<nlm::NLMConfig>();
        config->simulationSteps = 10000;
        config->timeStep = 0.001;  // 1ms
        config->enableLogging = true;
        config->logLevel = nlm::LogLevel::Info;
        
        // Create NLM configuration system
        nlmConfig_ = std::make_unique<nlm::NLMConfigSystem>(config);
        
        // Initialize configuration system
        nlmConfig_->initialize();
        
        std::cout << "Configuration subsystem initialized" << std::endl;
    }
    
    void initializeLogger() {
        std::cout << "Initializing Logger subsystem..." << std::endl;
        
        // Configure logger
        auto& logger = nlm::Logger::getGlobal();
        logger.setLevel(nlm::LogLevel::Info);
        logger.startAsync();
        
        std::cout << "Logger subsystem initialized" << std::endl;
    }
    
    void setupPerformanceCallbacks() {
        std::cout << "Setting up PerformanceSystem callbacks..." << std::endl;
        
        // Set up brain performance callback
        performanceSystem_.setBrainCallback([this](const nlm::PerformanceSnapshot& snapshot) {
            this->handleBrainPerformance(snapshot);
        });
        
        // Set up memory performance callback
        performanceSystem_.setMemoryCallback([this](const nlm::PerformanceSnapshot& snapshot) {
            this->handleMemoryPerformance(snapshot);
        });
        
        // Set up prediction performance callback
        performanceSystem_.setPredictionCallback([this](const nlm::PerformanceSnapshot& snapshot) {
            this->handlePredictionPerformance(snapshot);
        });
        
        // Set up logging performance callback
        performanceSystem_.setLoggingCallback([this](const nlm::PerformanceSnapshot& snapshot) {
            this->handleLoggingPerformance(snapshot);
        });
        
        std::cout << "PerformanceSystem callbacks set up" << std::endl;
    }
    
    void configurePerformanceMonitoring() {
        std::cout << "Configuring PerformanceSystem monitoring..." << std::string(""" << std::endl;
        
        // Add performance thresholds
        performanceSystem_.addThreshold("cpu_usage", 80.0, nlm::MetricType::Gauge, "warning");
        performanceSystem_.addThreshold("memory_used", 1024 * 1024 * 1024, nlm::MetricType::Memory, "critical");
        performanceSystem_.addThreshold("event_queue_size", 10000, nlm::MetricType::Gauge, "warning");
        performanceSystem_.addThreshold("response_time_ms", 100.0, nlm::MetricType::Timer, "warning");
        performanceSystem_.addThreshold("spike_rate", 1000000.0, nlm::MetricType::Rate, "critical");
        
        // Enable advanced profiling
        performanceSystem_.startMemoryProfiling();
        
        // Configure performance system
        performanceSystem_.initialize("config/performance_config.json");
        
        std::cout << "PerformanceSystem monitoring configured" << std::endl;
    }
    
    void runSimulationStep(uint64_t step) {
        // Simulate brain activity
        simulateBrainActivity(step);
        
        // Simulate memory operations
        simulateMemoryOperations();
        
        // Simulate configuration updates
        simulateConfigurationUpdates();
        
        // Simulate logging operations
        simulateLoggingOperations();
        
        // Simulate prediction operations
        simulatePredictionOperations();
        
        // Record performance for this step
        performanceSystem_.updateSnapshot();
    }
    
    void simulateBrainActivity(uint64_t step) {
        // Record brain-specific metrics
        performanceSystem_.profileFunction([this, step]() {
            // Simulate neural computation
            auto neurons = brain_->getNeurons();
            uint64_t activeNeurons = 0;
            
            for (const auto& neuron : neurons) {
                if (neuron->isActive()) {
                    activeNeurons++;
                    // Simulate spike generation
                    if (neuron->shouldSpike()) {
                        brain_->processSpike(neuron->getId());
                    }
                }
            }
            
            // Record spike count
            performanceSystem_.recordMetric("active_neurons", activeNeurons, nlm::MetricType::Counter);
            
            // Simulate synaptic activity
            auto synapses = brain_->getSynapses();
            uint64_t activeSynapses = 0;
            
            for (const auto& synapse : synapses) {
                if (synapse->isActive()) {
                    activeSynapses++;
                }
            }
            
            performanceSystem_.recordMetric("active_synapses", activeSynapses, nlm::MetricType::Counter);
            
        }, "brain_simulation");
    }
    
    void simulateMemoryOperations() {
        performanceSystem_.profileFunction([this]() {
            // Simulate memory pool usage
            size_t allocated = memoryPool_->getAllocatedSize();
            size_t available = memoryPool_->getAvailableSize();
            
            performanceSystem_.recordMetric("memory_pool_allocated", allocated, nlm::MetricType::Memory);
            performanceSystem_.recordMetric("memory_pool_available", available, nlm::MetricType::Memory);
            
            // Simulate memory allocations/deallocations
            for (int i = 0; i < 100; ++i) {
                // Allocate memory (simulated)
                performanceSystem_.recordMemoryAllocation(nullptr, "memory_simulation");
                
                // Deallocate memory (simulated)
                performanceSystem_.recordMemoryDeallocation(nullptr);
            }
            
        }, "memory_simulation");
    }
    
    void simulateConfigurationUpdates() {
        performanceSystem_.profileFunction([this]() {
            // Simulate configuration parsing
            if (nlmConfig_) {
                // Record configuration processing metrics
                performanceSystem_.recordMetric("config_parse_count", 1, nlm::MetricType::Counter);
                performanceSystem_.recordMetric("config_validate_count", 1, nlm::MetricType::Counter);
            }
            
        }, "config_simulation");
    }
    
    void simulateLoggingOperations() {
        performanceSystem_.profileFunction([this]() {
            // Simulate logging operations
            uint64_t logMessages = 1000;
            uint64_t droppedMessages = 10;
            
            performanceSystem_.recordMetric("log_messages_total", logMessages, nlm::MetricType::Counter);
            performanceSystem_.recordMetric("log_messages_dropped", droppedMessages, nlm::MetricType::Counter);
            
            double throughput = (logMessages - droppedMessages) / 100.0;  // per 100ms
            performanceSystem_.recordMetric("log_processing_throughput", throughput, nlm::MetricType::Rate);
            
        }, "logging_simulation");
    }
    
    void simulatePredictionOperations() {
        performanceSystem_.profileFunction([this]() {
            // Simulate neural network operations
            uint64_t forwardPasses = 1000;
            double forwardTime = forwardPasses * 1.5;  // ms
            uint64_t backwardPasses = 500;
            double backwardTime = backwardPasses * 3.0;  // ms
            uint64_t examplesProcessed = 10000;
            double accuracy = 0.95;
            
            // Record neural network metrics
            performanceSystem_.recordNeuralNetworkMetrics("brain_network",
                                                          forwardPasses, forwardTime,
                                                          backwardPasses, backwardTime,
                                                          examplesProcessed, accuracy);
            
        }, "prediction_simulation");
    }
    
    void handleBrainPerformance(const nlm::PerformanceSnapshot& snapshot) {
        // Handle brain-specific performance metrics
        uint64_t spikeCount = snapshot.spikeCount;
        double firingRate = snapshot.averageFiringRate;
        
        if (spikeCount > 1000000) {
            performanceSystem_.raisePerformanceError(
                nlm::PerformanceErrorType::CPUUsageExceeded,
                "spike_count",
                "Consider reducing neural population size or increasing simulation time step"
            );
        }
        
        if (firingRate > 1000.0) {
            performanceSystem_.raisePerformanceError(
                nlm::PerformanceErrorType::SlowdownDetected,
                "firing_rate",
                "Consider optimizing neural dynamics or reducing network connectivity"
            );
        }
    }
    
    void handleMemoryPerformance(const nlm::PerformanceSnapshot& snapshot) {
        // Handle memory-specific performance metrics
        uint64_t memoryUsed = snapshot.memoryUsed;
        uint64_t memoryPoolUsed = snapshot.memoryPoolUsed;
        
        if (memoryUsed > memoryPoolUsed * 0.9) {
            performanceSystem_.raisePerformanceError(
                nlm::PerformanceErrorType::MemoryUsageExceeded,
                "memory_used",
                "Consider increasing memory pool size or optimizing memory allocation"
            );
        }
        
        performanceSystem_.detectMemoryLeaks();
    }
    
    void handlePredictionPerformance(const nlm::PerformanceSnapshot& snapshot) {
        // Handle prediction-specific performance metrics
        uint64_t forwardPassCount = snapshot.forwardPassCount;
        double forwardPassAverageTime = snapshot.forwardPassAverageTimeMs;
        
        if (forwardPassCount > 0 && forwardPassAverageTime > 100.0) {
            performanceSystem_.raisePerformanceError(
                nlm::PerformanceErrorType::LatencyExceeded,
                "forward_pass_time",
                "Consider optimizing neural network operations or using SIMD instructions"
            );
        }
    }
    
    void handleLoggingPerformance(const nlm::PerformanceSnapshot& snapshot) {
        // Handle logging-specific performance metrics
        uint64_t logMessagesTotal = snapshot.logMessagesTotal;
        uint64_t logMessagesDropped = snapshot.logMessagesDropped;
        
        if (logMessagesTotal > 0) {
            double dropRate = static_cast<double>(logMessagesDropped) / logMessagesTotal;
            if (dropRate > 0.1) {
                performanceSystem_.raisePerformanceError(
                    nlm::PerformanceErrorType::ThroughputBelowMinimum,
                    "log_throughput",
                    "Consider increasing log queue size or optimizing log processing"
                );
            }
        }
    }
    
    void shutdownBrain() {
        if (brain_) {
            brain_->shutdown();
            brain_.reset();
        }
    }
    
    void shutdownMemory() {
        if (memoryPool_) {
            memoryPool_->shutdown();
            memoryPool_.reset();
        }
    }
    
    void shutdownConfig() {
        if (nlmConfig_) {
            nlmConfig_->shutdown();
            nlmConfig_.reset();
        }
    }
    
    void shutdownLogger() {
        auto& logger = nlm::Logger::getGlobal();
        logger.stopAsync();
    }
    
    // Member variables
    nlm::PerformanceSystem& performanceSystem_;
    std::unique_ptr<nlm::Brain> brain_;
    std::unique_ptr<nlm::MemoryPool> memoryPool_;
    std::unique_ptr<nlm::NLMConfigSystem> nlmConfig_;
    uint64_t maxSteps_ = 100;
};

// Performance monitoring setup function
void setupPerformanceMonitoring() {
    auto& performanceSystem = nlm::PerformanceSystem::getInstance();
    
    // Enable performance monitoring
    performanceSystem.setEnabled(true);
    
    // Configure performance monitoring
    performanceSystem.addThreshold("cpu_usage", 80.0, nlm::MetricType::Gauge, "warning");
    performanceSystem.addThreshold("memory_used", 1024 * 1024 * 1024, nlm::MetricType::Memory, "critical");
    performanceSystem.addThreshold("event_queue_size", 10000, nlm::MetricType::Gauge, "warning");
    
    std::cout << "Performance monitoring setup complete" << std::endl;
}

// Performance monitoring teardown function
void teardownPerformanceMonitoring() {
    auto& performanceSystem = nlm::PerformanceSystem::getInstance();
    
    // Stop real-time monitoring
    performanceSystem.stopRealTimeMonitoring();
    
    // Shutdown performance system
    performanceSystem.shutdown();
    
    std::cout << "Performance monitoring teardown complete" << std::endl;
}

// Utility functions for performance analysis
namespace performance {
    void exportPerformanceData(const std::string& filepath) {
        auto& performanceSystem = nlm::PerformanceSystem::getInstance();
        performanceSystem.exportPerformanceData(filepath);
    }
    
    void generatePerformanceReports(const std::string& outputDir) {
        auto& performanceSystem = nlm::PerformanceSystem::getInstance();
        
        std::string htmlReport = performanceSystem.generateDashboardHTML();
        std::string jsonReport = performanceSystem.generateDashboardJSON();
        std::string textReport = performanceSystem.generatePerformanceReport();
        
        // Save reports to files
        saveReportToFile(htmlReport, outputDir + "/dashboard.html");
        saveReportToFile(jsonReport, outputDir + "/dashboard.json");
        saveReportToFile(textReport, outputDir + "/report.txt");
    }
    
    void analyzePerformanceBottlenecks() {
        auto& performanceSystem = nlm::PerformanceSystem::getInstance();
        
        std::string bottlenecks = performanceSystem.identifyBottlenecks();
        std::cout << "Performance bottlenecks: " << bottlenecks << std::endl;
        
        auto optimizationSuggestions = performanceSystem.getOptimizationSuggestions();
        std::cout << "Optimization suggestions:" << std::endl;
        for (const std::string& suggestion : optimizationSuggestions) {
            std::cout << "  - " << suggestion << std::endl;
        }
    }
    
    std::string formatNumber(double value) {
        std::ostringstream oss;
        if (value >= 1000000) {
            oss << std::fixed << std::setprecision(2) << (value / 1000000) << "M";
        } else if (value >= 1000) {
            oss << std::fixed << std::setprecision(2) << (value / 1000) << "K";
        } else {
            oss << std::fixed << std::setprecision(2) << value;
        }
        return oss.str();
    }
    
    std::string formatBytes(uint64_t bytes) {
        if (bytes >= 1024 * 1024 * 1024) {
            return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
        } else if (bytes >= 1024 * 1024) {
            return std::to_string(bytes / (1024 * 1024)) + " MB";
        } else if (bytes >= 1024) {
            return std::to_string(bytes / 1024) + " KB";
        } else {
            return std::to_string(bytes) + " B";
        }
    }
    
    std::string formatPercentage(double value) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << value << "%";
        return oss.str();
    }
    
    void saveReportToFile(const std::string& content, const std::string& filepath) {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << content;
            file.close();
            std::cout << "Report saved to: " << filepath << std::endl;
        } else {
            NLM_LOG_ERROR("Failed to save report to: " + filepath);
        }
    }
} // namespace performance

// Main function
int main() {
    std::cout << "=== NLM Performance Monitoring Example ===" << std::endl;
    
    try {
        // Initialize NLM application with performance monitoring
        ExampleNLMApplication app;
        
        // Run simulation
        app.run();
        
        // Generate performance reports
        performance::generatePerformanceReports("performance_reports");
        
        // Analyze performance bottlenecks
        performance::analyzePerformanceBottlenecks();
        
        // Export performance data
        performance::exportPerformanceData("performance_data.json");
        
        std::cout << "=== Example completed successfully ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}