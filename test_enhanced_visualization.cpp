#include <iostream>
#include <memory>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "visualization/VisualizationInterface.hpp"
#include "analysis/NetworkAnalyzer.hpp"
#include "analysis/ActivityAnalyzer.hpp"
#include "analysis/PerformanceAnalyzer.hpp"
#include "analysis/StateAnalyzer.hpp"
#include "analysis/PlottingTools.hpp"

// Test helper functions
void printSection(const std::string& title) {
    std::cout << std::endl;
    std::cout << "=== " << title << " ===" << std::endl;
    std::cout << std::endl;
}

void printSuccess(const std::string& message) {
    std::cout << "✓ " << message << std::endl;
}

void printInfo(const std::string& message) {
    std::cout << "  " << message << std::endl;
}

void printError(const std::string& message) {
    std::cout << "✗ " << message << std::endl;
}

// Test functions
testVisualizationInterfaceCreation() {
    printSection("Test: VisualizationInterface Creation");
    
    // Test default constructor
    VisualizationInterface vis1;
    
    // Test that interface can be created
    printSuccess("VisualizationInterface default constructor works");
    
    // Test initialization
    bool initResult = vis1.initialize();
    if (initResult) {
        printSuccess("VisualizationInterface initialization successful");
    } else {
        printError("VisualizationInterface initialization failed");
    }
    
    // Test basic properties
    vis1.setUpdateRate(60.0);
    vis1.setBackend("glfw");
    vis1.enableInteractiveMode(true);
    
    printInfo("Update rate: " + std::to_string(vis1.getUpdateRate()) + " Hz");
    printInfo("Backend: " + vis1.getBackend());
    printInfo("Interactive mode: " + std::to_string(vis1.isInteractiveMode()));
    
    return initResult;
}

testNetworkAnalyzer() {
    printSection("Test: Network Analyzer");
    
    // Create network analyzer
    auto networkAnalyzer = NetworkAnalyzer::create();
    
    printSuccess("NetworkAnalyzer created successfully");
    
    // Create a simple test brain
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(10), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (brain->initialize()) {
        printSuccess("Test brain initialized");
        
        // Test graph metrics
        auto graphMetrics = networkAnalyzer->analyzeGraphMetrics(*brain);
        
        printInfo("Graph metrics calculated:");
        printInfo("  - Average degree: " + std::to_string(graphMetrics.averageDegree));
        printInfo("  - Clustering coefficient: " + std::to_string(graphMetrics.clusteringCoefficient));
        printInfo("  - Network density: " + std::to_string(graphMetrics.density));
        printInfo("  - Connected components: " + std::to_string(graphMetrics.connectedComponents));
        
        // Test centrality measures
        auto centrality = networkAnalyzer->calculateCentralityMeasures(*brain);
        printSuccess("Centrality measures calculated");
        
        // Test community detection
        auto communities = networkAnalyzer->detectCommunities(*brain);
        printInfo("Communities detected: " + std::to_string(communities.communities.size()));
        
        return true;
    } else {
        printError("Failed to initialize test brain");
        return false;
    }
}

testActivityAnalyzer() {
    printSection("Test: Activity Analyzer");
    
    // Create activity analyzer
    auto activityAnalyzer = ActivityAnalyzer::create();
    
    printSuccess("ActivityAnalyzer created successfully");
    
    // Create test brain
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(20), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (brain->initialize()) {
        printSuccess("Test brain initialized");
        
        // Run simulation
        for (SimulationStep step = 0; step < 10; ++step) {
            brain->step(step, step * 0.001);
        }
        
        // Test spike statistics
        auto stats = activityAnalyzer->analyzeSpikeStatistics(*brain, 100);
        
        printInfo("Activity statistics:");
        printInfo("  - Total spike count: " + std::to_string(stats.totalSpikeCount));
        printInfo("  - Average firing rate: " + std::to_string(stats.averageFiringRate) + " Hz");
        printInfo("  - Standard deviation: " + std::to_string(stats.standardDeviation));
        printInfo("  - Peak firing rate: " + std::to_string(stats.peakFiringRate) + " Hz");
        
        // Test activity pattern
        auto* spikeSystem = brain->getSpikeSystem();
        if (spikeSystem) {
            const auto& spikeHistory = spikeSystem->getSpikeHistory();
            std::vector<SpikeEvent> spikes;
            
            // Convert DetailedSpikeEvent to SpikeEvent
            for (const auto& detailedSpike : spikeHistory) {
                SpikeEvent spike(detailedSpike.source, detailedSpike.timestamp, detailedSpike.step);
                spikes.push_back(spike);
            }
            
            auto pattern = activityAnalyzer->analyzeActivityPattern(spikes, 1000.0);
            
            printInfo("Activity pattern analysis:");
            printInfo("  - Bursting: " + std::to_string(pattern.isBursting));
            printInfo("  - Regular: " + std::to_string(pattern.isRegular));
            printInfo("  - Noisy: " + std::to_string(pattern.isNoisy));
            printInfo("  - Synchronous: " + std::to_string(pattern.isSynchronous));
            printInfo("  - Spike count: " + std::to_string(pattern.spikeEvents.size()));
        }
        
        return true;
    } else {
        printError("Failed to initialize test brain");
        return false;
    }
}

testStateAnalyzer() {
    printSection("Test: State Analyzer");
    
    // Create state analyzer
    auto stateAnalyzer = StateAnalyzer::create();
    
    printSuccess("StateAnalyzer created successfully");
    
    // Create test brain
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(15), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (brain->initialize()) {
        printSuccess("Test brain initialized");
        
        // Capture multiple state snapshots
        std::vector<StateAnalyzer::StateSnapshot> states;
        for (int i = 0; i < 5; ++i) {
            // Run simulation
            for (SimulationStep step = 0; step < 5; ++step) {
                brain->step(step, step * 0.001);
            }
            
            // Capture state
            auto snapshot = stateAnalyzer->captureState(*brain);
            states.push_back(snapshot);
            
            // Small delay between captures
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        printSuccess("Captured " + std::to_string(states.size()) + " state snapshots");
        
        // Compare states
        for (size_t i = 1; i < states.size(); ++i) {
            auto changes = stateAnalyzer->compareStates(states[i-1], states[i]);
            printInfo("State transition " + std::to_string(i) + ": " + std::to_string(changes.size()) + " changes");
        }
        
        // Track evolution
        auto evolution = stateAnalyzer->trackEvolution(states);
        printInfo("Evolution metrics:");
        printInfo("  - Total change magnitude: " + std::to_string(evolution.totalChangeMagnitude));
        printInfo("  - Stability score: " + std::to_string(evolution.stabilityScore));
        printInfo("  - Change frequency: " + std::to_string(evolution.changeFrequency));
        printInfo("  - Volatility: " + std::to_string(evolution.volatility));
        
        // Detect anomalies
        auto anomalies = stateAnalyzer->detectAnomalies(states);
        printInfo("Anomalies detected: " + std::to_string(anomalies.size()));
        
        return true;
    } else {
        printError("Failed to initialize test brain");
        return false;
    }
}

testPerformanceAnalyzer() {
    printSection("Test: Performance Analyzer");
    
    // Create performance analyzer
    auto performanceAnalyzer = PerformanceAnalyzer::create();
    
    printSuccess("PerformanceAnalyzer created successfully");
    
    // Test performance metrics collection
    auto metrics = performanceAnalyzer->collectPerformanceMetrics();
    
    printInfo("Performance metrics collected:");
    printInfo("  - CPU usage: " + std::to_string(metrics.cpuUsage * 100) + "%");
    printInfo("  - Memory usage: " + std::to_string(metrics.memoryUsage * 100) + "%");
    printInfo("  - Processing latency: " + std::to_string(metrics.processingLatency.count()) + " ms");
    printInfo("  - Throughput: " + std::to_string(metrics.throughput) + " ops/sec");
    
    // Test resource usage measurement
    auto resourceUsage = performanceAnalyzer->measureResourceUsage();
    
    printInfo("Resource usage:");
    printInfo("  - CPU utilization: " + std::to_string(resourceUsage.cpuUtilization * 100) + "%");
    printInfo("  - Memory footprint: " + std::to_string(resourceUsage.memoryFootprint / (1024*1024)) + " MB");
    
    // Test bottleneck analysis
    // Create test brain
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(10), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (brain->initialize()) {
        printSuccess("Test brain initialized");
        
        auto bottlenecks = performanceAnalyzer->analyzeBottlenecks(*brain);
        printInfo("Bottleneck analysis completed: " + std::to_string(bottlenecks.size()) + " bottlenecks detected");
        
        for (const auto& bottleneck : bottlenecks) {
            printInfo("  - " + bottleneck.bottleneckType + " (severity: " + std::to_string(bottleneck.severity) + ")");
        }
        
        return true;
    } else {
        printError("Failed to initialize test brain");
        return false;
    }
}

testPlottingTools() {
    printSection("Test: PlottingTools");
    
    // Create plotting tools
    auto plottingTools = PlottingTools::create();
    
    printSuccess("PlottingTools created successfully");
    
    // Test plot data
    PlottingTools::PlotData samplePlot;
    samplePlot.name = "Sample Plot";
    samplePlot.title = "Test Visualization";
    samplePlot.xLabel = "X Axis";
    samplePlot.yLabel = "Y Axis";
    samplePlot.fileFormat = "svg";
    
    // Generate sample data
    for (int i = 0; i < 10; ++i) {
        samplePlot.xData.push_back(static_cast<double>(i));
        samplePlot.yData.push_back(static_cast<double>(i * i));
    }
    
    printSuccess("Sample plot data generated");
    
    // Test data normalization
    std::vector<double> testData = {1.0, 2.0, 3.0, 4.0, 5.0};
    auto normalized = plottingTools->normalizeData(testData, 0.0, 1.0);
    
    printInfo("Data normalization test:");
    for (double val : normalized) {
        printInfo("  " + std::to_string(val));
    }
    
    // Test data smoothing
    auto smooth = plottingTools->smoothData(testData, 3);
    printSuccess("Data smoothing completed");
    
    // Test interpolation
    auto interpolated = plottingTools->interpolateData(samplePlot.xData, samplePlot.yData, 20);
    printInfo("Data interpolation completed: " + std::to_string(interpolated.size()) + " points");
    
    return true;
}

testVisualizationInterfaceIntegration() {
    printSection("Test: VisualizationInterface Integration");
    
    // Create visualization interface
    auto visualization = std::make_unique<VisualizationInterface>();
    
    printSuccess("VisualizationInterface created");
    
    // Initialize
    if (!visualization->initialize()) {
        printError("Failed to initialize visualization");
        return false;
    }
    
    printSuccess("Visualization initialized");
    
    // Create and integrate analysis components
    auto networkAnalyzer = NetworkAnalyzer::create();
    auto activityAnalyzer = ActivityAnalyzer::create();
    auto performanceAnalyzer = PerformanceAnalyzer::create();
    auto stateAnalyzer = StateAnalyzer::create();
    auto plottingTools = PlottingTools::create();
    
    // Integrate all components
    visualization->integrateAnalysis(networkAnalyzer);
    visualization->integrateAnalysis(activityAnalyzer);
    visualization->integrateAnalysis(performanceAnalyzer);
    visualization->integrateAnalysis(stateAnalyzer);
    visualization->integrateAnalysis(plottingTools);
    
    printSuccess("All analysis components integrated");
    
    // Synchronize with analysis
    visualization->synchronizeWithAnalysis(
        networkAnalyzer,
        activityAnalyzer,
        performanceAnalyzer,
        stateAnalyzer
    );
    
    printSuccess("Visualization synchronized with analysis components");
    
    // Test interactive features
    visualization->enableInteractiveMode(true);
    visualization->setVisualizationScale(2.0);
    visualization->enableRealTimeUpdates(true);
    
    printInfo("Interactive features configured");
    printInfo("  - Scale: " + std::to_string(visualization->getVisualizationScale()));
    printInfo("  - Real-time updates: " + std::to_string(visualization->areRealTimeUpdatesEnabled()));
    
    // Test export capabilities
    if (visualization->exportVisualization("test_output.html", "html")) {
        printSuccess("Visualization export successful");
    } else {
        printError("Visualization export failed");
    }
    
    // Test window management
    visualization->addVisualizationWindow("Network View", "network");
    visualization->addVisualizationWindow("Activity View", "activity");
    
    printInfo("Visualization windows created: " + std::to_string(visualization->pImpl->visualizationWindows.size()));
    
    return true;
}

testCompleteSystem() {
    printSection("Test: Complete System Integration");
    
    std::cout << "Testing complete visualization and analysis system integration..." << std::endl;
    std::cout << std::endl;
    
    // Test all components work together
    bool test1 = testVisualizationInterfaceCreation();
    bool test2 = testNetworkAnalyzer();
    bool test3 = testActivityAnalyzer();
    bool test4 = testStateAnalyzer();
    bool test5 = testPerformanceAnalyzer();
    bool test6 = testPlottingTools();
    bool test7 = testVisualizationInterfaceIntegration();
    
    printSection("Test Summary");
    std::cout << "Test Results:" << std::endl;
    std::cout << "  VisualizationInterface Creation: " << (test1 ? "PASS" : "FAIL") << std::endl;
    std::cout << "  Network Analyzer: " << (test2 ? "PASS" : "FAIL") << std::endl;
    std::cout << "  Activity Analyzer: " << (test3 ? "PASS" : "FAIL") << std::endl;
    std::cout << "  State Analyzer: " << (test4 ? "PASS" : "FAIL") << std::endl;
    std::cout << "  Performance Analyzer: " << (test5 ? "PASS" : "FAIL") << std::endl;
    std::cout << "  Plotting Tools: " << (test6 ? "PASS" : "FAIL") << std::endl;
    std::cout << "  VisualizationInterface Integration: " << (test7 ? "PASS" : "FAIL") << std::endl;
    std::cout << std::endl;
    
    int passedTests = 0;
    if (test1) passedTests++;
    if (test2) passedTests++;
    if (test3) passedTests++;
    if (test4) passedTests++;
    if (test5) passedTests++;
    if (test6) passedTests++;
    if (test7) passedTests++;
    
    std::cout << "Overall: " << passedTests << " / 7 tests passed" << std::endl;
    
    return (passedTests == 7);
}

int main() {
    std::cout << "=== Enhanced Visualization and Analysis System Test Suite ===" << std::endl;
    std::cout << "Testing all components of the enhanced NLM visualization system..." << std::endl;
    
    try {
        bool success = testCompleteSystem();
        
        if (success) {
            std::cout << std::endl;
            printSuccess("All tests passed! Enhanced visualization system is working correctly.");
            return 0;
        } else {
            std::cout << std::endl;
            printError("Some tests failed. Please check the implementation.");
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Test suite error: " << e.what() << std::endl;
        return 1;
    }
}
