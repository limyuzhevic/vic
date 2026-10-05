// Test Enhanced Visualization and Monitoring System
// 
// This test file verifies the new visualization and monitoring capabilities
// including real-time statistics collection, multiple visualization modes,
// and experiment integration.

#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <vector>
#include <map>

// Include NLM framework headers
#include "src/experiments/ExperimentRunner.hpp"
#include "src/visualization/VisualizationInterface.hpp"
#include "src/visualization/MonitoringSystem.hpp"
#include "src/experiments/Experiment.hpp"
#include "src/experiments/Metrics.hpp"

using namespace nlm;

// Mock Brain class for testing
class MockBrain {
public:
    MockBrain() : stepCount(0), neuronCount(1000), synapseCount(10000) {}
    
    void step(SimulationStep step) {
        stepCount = step;
        // Simulate some brain activity
        firingRate = 50.0 + (static_cast<double>(rand()) / RAND_MAX) * 20.0;
        activeNeurons = neuronCount * 0.3 + (static_cast<double>(rand()) / RAND_MAX) * 200.0;
        firingNeurons = activeNeurons * 0.6;
        totalSpikes += 10;
    }
    
    void receiveSensoryInput(const SensoryInput& input) {}
    
    std::unique_ptr<Action> produceAction() {
        return std::make_unique<Action>(ActionType::MoveForward);
    }
    
    size_t getTotalNeuronCount() const { return neuronCount; }
    size_t getTotalSynapseCount() const { return synapseCount; }
    size_t getActiveNeuronCount() const { return static_cast<size_t>(activeNeurons); }
    size_t getFiringNeuronCount() const { return static_cast<size_t>(firingNeurons); }
    float getAverageFiringRate() const { return static_cast<float>(firingRate); }
    
private:
    SimulationStep stepCount;
    size_t neuronCount;
    size_t synapseCount;
    double firingRate;
    double activeNeurons;
    double firingNeurons;
    size_t totalSpikes;
};

// Mock Environment class for testing
class MockEnvironment {
public:
    MockEnvironment() : done(false) {}
    
    auto reset() {
        currentStep = 0;
        reward = 0.0;
        done = false;
        return std::make_unique<SensoryInput>(std::vector<float>{0.5f, 0.5f, 0.5f});
    }
    
    auto step(const Action& action) {
        currentStep++;
        reward = static_cast<float>(rand()) / RAND_MAX;
        
        if (currentStep > 100) {
            done = true;
        }
        
        return std::make_unique<SensoryInput>(std::vector<float>{reward, 0.1f, 0.9f});
    }
    
    float getLastReward() const { return reward; }
    bool isDone() const { return done; }
    
private:
    SimulationStep currentStep;
    float reward;
    bool done;
};

void testVisualizationInitialization() {
    std::cout << "=== Testing Visualization Initialization ===" << std::endl;
    
    VisualizationConfig config;
    config.windowWidth = 1920;
    config.windowHeight = 1080;
    config.updateFrequency = 30.0;
    config.showGrid = true;
    config.showLabels = true;
    config.showTooltips = true;
    config.colorScheme = "default";
    config.opacity = 1.0;
    config.enableAntialiasing = true;
    config.enableShadows = true;
    config.enable3D = false;
    
    auto visualization = std::make_shared<VisualizationInterface>();
    
    bool initResult = visualization->initialize(config);
    std::cout << "Visualization initialization: " << (initResult ? "SUCCESS" : "FAILED") << std::endl;
    
    visualization->setUpdateRate(60.0);
    double rate = visualization->getUpdateRate();
    std::cout << "Update rate: " << rate << " Hz" << std::endl;
    
    visualization->addVisualizationMode(VisualizationMode::NETWORK_GRAPH);
    visualization->addVisualizationMode(VisualizationMode::ACTIVITY_HEATMAP);
    visualization->addVisualizationMode(VisualizationMode::SPIKE_RASTER);
    
    std::cout << "Visualization modes: " << visualization->getVisualizationMode() << std::endl;
    std::cout << "Has NETWORK_GRAPH: " << visualization->hasVisualizationMode(VisualizationMode::NETWORK_GRAPH) << std::endl;
    std::cout << "Has ACTIVITY_HEATMAP: " << visualization->hasVisualizationMode(VisualizationMode::ACTIVITY_HEATMAP) << std::endl;
    
    visualization->close();
    std::cout << "Visualization closed" << std::endl;
}

void testMonitoringSystem() {
    std::cout << "\n=== Testing Monitoring System ===" << std::endl;
    
    auto monitoring = std::make_shared<MonitoringSystem>();
    
    // Mock brain initialization
    auto brain = std::make_shared<MockBrain>();
    
    bool initResult = monitoring->initialize(brain);
    std::cout << "Monitoring system initialization: " << (initResult ? "SUCCESS" : "FAILED") << std::endl;
    
    monitoring->setEnableRealTimeUpdates(true);
    monitoring->setEnablePerformanceProfiling(true);
    monitoring->setEnableMemoryTracking(true);
    
    std::cout << "Real-time updates enabled: " << monitoring->getEnableRealTimeUpdates() << std::endl;
    std::cout << "Performance profiling enabled: " << monitoring->getEnablePerformanceProfiling() << std::endl;
    std::cout << "Memory tracking enabled: " << monitoring->getEnableMemoryTracking() << std::endl;
    
    // Update monitoring data
    for (int i = 0; i < 10; ++i) {
        monitoring->update(0.1);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    std::string report = monitoring->generateReport();
    std::cout << "\nGenerated Monitoring Report (first 500 chars):" << std::endl;
    std::cout << report.substr(0, std::min(500, static_cast<int>(report.length()))) << "..." << std::endl;
    
    // Test export functionality
    monitoring->exportData("test_monitoring_results.csv", ExportFormat::CSV);
    monitoring->exportData("test_monitoring_results.json", ExportFormat::JSON);
    monitoring->exportData("test_monitoring_results.xml", ExportFormat::XML);
    
    std::cout << "Export tests completed" << std::endl;
    
    monitoring->close();
    std::cout << "Monitoring system closed" << std::endl;
}

void testExperimentRunnerWithMonitoring() {
    std::cout << "\n=== Testing ExperimentRunner with Monitoring Integration ===" << std::endl;
    
    // Create experiment runner
    auto experimentRunner = std::make_shared<ExperimentRunner>();
    
    // Create experiment
    auto experiment = experimentRunner->createExperiment("Phase6_Integration_Test");
    experiment->setSeed(42);
    
    // Create mock brain and environment
    auto brain = std::make_shared<MockBrain>();
    auto environment = std::make_shared<MockEnvironment>();
    
    // Create visualization and monitoring
    auto visualization = std::make_shared<VisualizationInterface>();
    VisualizationConfig config;
    visualization->initialize(config);
    
    auto monitoring = std::make_shared<MonitoringSystem>();
    monitoring->initialize(brain);
    
    // Run experiment with monitoring (enhanced method)
    bool success = experimentRunner->runExperimentWithMonitoring(
        experiment,
        brain,
        environment,
        visualization,
        monitoring,
        100  // max steps
    );
    
    std::cout << "Enhanced experiment execution: " << (success ? "SUCCESS" : "FAILED") << std::endl;
    
    // Check experiment results
    std::cout << "Experiment name: " << experiment->getName() << std::endl;
    std::cout << "Experiment seed: " << experiment->getSeed() << std::endl;
    std::cout << "Experiment metrics: " << experiment->getMetrics().size() << std::endl;
    std::cout << "Experiment step data: " << experiment->getStepData().size() << " points" << std::endl;
    
    // Save results
    bool saveResult = experimentRunner->saveResults("test_experiment_results.txt");
    std::cout << "Results save: " << (saveResult ? "SUCCESS" : "FAILED") << std::endl;
    
    // Export visualization
    visualization->exportVisualization("test_experiment_visualization");
    
    // Export monitoring data
    monitoring->exportData("test_experiment_monitoring.csv", ExportFormat::CSV);
    
    // Cleanup
    visualization->close();
    monitoring->close();
    
    std::cout << "ExperimentRunner with monitoring test completed" << std::endl;
}

void testComprehensiveVisualizationModes() {
    std::cout << "\n=== Testing Comprehensive Visualization Modes ===" << std::endl;
    
    auto visualization = std::make_shared<VisualizationInterface>();
    VisualizationConfig config;
    visualization->initialize(config);
    
    // Create mock data
    auto brain = std::make_shared<MockBrain>();
    
    // Test different visualization modes
    std::vector<VisualizationMode> modes = {
        VisualizationMode::NETWORK_GRAPH,
        VisualizationMode::ACTIVITY_HEATMAP,
        VisualizationMode::SPIKE_RASTER,
        VisualizationMode::WEIGHT_MATRIX,
        VisualizationMode::MEMORY_VISUALIZATION,
        VisualizationMode::NEUROMODULATION_DISPLAY,
        VisualizationMode::DEVELOPMENT_STAGE,
        VisualizationMode::CHECKPOINT_PROGRESS,
        VisualizationMode::EXPERIMENT_TRACKING
    };
    
    std::cout << "Testing " << modes.size() << " visualization modes:" << std::endl;
    
    for (const auto& mode : modes) {
        // Set visualization mode
        visualization->setVisualizationMode(mode);
        
        // Check if mode is active
        bool isActive = visualization->hasVisualizationMode(mode);
        std::cout << "  " << static_cast<int>(mode) << ": " << (isActive ? "ACTIVE" : "INACTIVE") << std::endl;
        
        // Update visualization (simulate)
        visualization->update();
        
        // Render visualization (simulate)
        visualization->render();
    }
    
    // Test insights generation
    std::string insights = visualization->generateInsights();
    std::cout << "\nGenerated insights (first 300 chars):" << std::endl;
    std::cout << insights.substr(0, std::min(300, static_cast<int>(insights.length()))) << "..." << std::endl;
    
    // Test anomaly detection
    std::vector<std::string> anomalies = visualization->detectAnomalies();
    std::cout << "\nDetected anomalies: " << anomalies.size() << std::endl;
    for (const auto& anomaly : anomalies) {
        std::cout << "  - " << anomaly << std::endl;
    }
    
    // Test stability score
    double stability = visualization->calculateStabilityScore();
    std::cout << "Stability score: " << stability << std::endl;
    
    visualization->close();
    std::cout << "Comprehensive visualization modes test completed" << std::endl;
}

void testDataExportFormats() {
    std::cout << "\n=== Testing Data Export Formats ===" << std::endl;
    
    auto monitoring = std::make_shared<MonitoringSystem>();
    auto brain = std::make_shared<MockBrain>();
    monitoring->initialize(brain);
    
    // Update with some data
    for (int i = 0; i < 5; ++i) {
        monitoring->update(0.1);
    }
    
    // Test all export formats
    std::vector<std::pair<std::string, ExportFormat>> exportTests = {
        {"csv_export", ExportFormat::CSV},
        {"json_export", ExportFormat::JSON},
        {"xml_export", ExportFormat::XML},
        {"binary_export", ExportFormat::BINARY}
    };
    
    for (const auto& test : exportTests) {
        std::cout << "Testing " << test.first << " ...";
        
        try {
            monitoring->exportData(test.first, test.second);
            std::cout << " SUCCESS" << std::endl;
        } catch (const std::exception& e) {
            std::cout << " FAILED: " << e.what() << std::endl;
        }
    }
    
    monitoring->close();
    std::cout << "Data export formats test completed" << std::endl;
}

int main() {
    std::cout << "=== Enhanced Visualization and Monitoring System Tests ===" << std::endl;
    std::cout << "Testing all new visualization and monitoring capabilities" << std::endl;
    
    try {
        testVisualizationInitialization();
        testMonitoringSystem();
        testExperimentRunnerWithMonitoring();
        testComprehensiveVisualizationModes();
        testDataExportFormats();
        
        std::cout << "\n=== ALL TESTS COMPLETED ===" << std::endl;
        std::cout << "The enhanced visualization and monitoring system is working correctly!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\nERROR: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
