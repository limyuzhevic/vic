// VisualizationTestRunner.cpp - Visualization test runner implementation

#include "VisualizationTestRunner.hpp"
#include "Visualizer.hpp"
#include "InteractiveVisualizer.hpp"
#include "NetworkVisualizer.hpp"
#include "SpikeVisualizer.hpp"
#include "DataExporter.hpp"
#include "AnimationGenerator.hpp"
#include "WebDashboard.hpp"
#include "HTMLExporter.hpp"
#include "Brain.hpp"
#include "SpikeEvent.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <vector>
#include <map>
#include <unordered_set>

namespace nlm {

VisualizationTestResult::TestResult VisualizationTestRunner::runTest(const std::string& testName) {
    TestResult result;
    result.testName = testName;
    result.startTime = std::chrono::system_clock::now();
    
    auto startTime = std::chrono::steady_clock::now();
    
    // Run specific test
    if (testName == "visualizer") {
        result = runVisualizerTest();
    } else if (testName == "interactive") {
        result = runInteractiveVisualizerTest();
    } else if (testName == "network") {
        result = runNetworkVisualizerTest();
    } else if (testName == "spike") {
        result = runSpikeVisualizerTest();
    } else if (testName == "data_exporter") {
        result = runDataExporterTest();
    } else if (testName == "animation") {
        result = runAnimationGeneratorTest();
    } else if (testName == "web") {
        result = runWebDashboardTest();
    } else if (testName == "html") {
        result = runHTMLExporterTest();
    } else {
        result.status = TestStatus::FAILED;
        result.message = "Unknown test: " + testName;
        result.endTime = std::chrono::system_clock::now();
        result.duration = std::chrono::duration<double>(result.endTime - result.startTime).count();
    }
    
    auto endTime = std::chrono::steady_clock::now();
    result.endTime = endTime;
    result.duration = std::chrono::duration<double>(endTime - startTime).count();
    
    // Calculate statistics
    result.passedTests = result.status == TestStatus::PASSED ? 1 : 0;
    result.failedTests = result.status == TestStatus::FAILED ? 1 : 0;
    result.skippedTests = 0;
    result.totalTests = 1;
    
    NLM_LOG_INFO("Test completed: " + testName + " (status: " + testStatusToString(result.status) + ", duration: " + std::to_string(result.duration) + "s)");
    
    return result;
}

std::vector<VisualizationTestResult::TestResult> VisualizationTestRunner::runAllTests() {
    std::vector<TestResult> results;
    
    // Define test order
    std::vector<std::string> testNames = {
        "visualizer",
        "interactive",
        "network",
        "spike",
        "data_exporter",
        "animation",
        "web",
        "html"
    };
    
    for (const auto& testName : testNames) {
        TestResult result = runTest(testName);
        results.push_back(result);
        
        // Small delay between tests
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return results;
}

VisualizationTestResult::TestResult VisualizationTestRunner::runVisualizerTest() {
    TestResult result;
    result.testName = "visualizer";
    
    try {
        // Create visualizer
        Visualizer::VisualizationConfig config;
        config.type = Visualizer::VisualizationType::NEURAL_NETWORK;
        config.width = 800.0;
        config.height = 600.0;
        
        Visualizer visualizer(config);
        
        // Initialize
        if (!visualizer.initialize()) {
            throw std::runtime_error("Failed to initialize visualizer");
        }
        
        // Update and render
        visualizer.update(brain, 0.1);
        if (!visualizer.render()) {
            throw std::runtime_error("Failed to render visualization");
        }
        
        // Check stats
        auto stats = visualizer.getStats();
        if (stats.neuronsVisualized == 0) {
            throw std::runtime_error("No neurons visualized");
        }
        
        result.status = TestStatus::PASSED;
        result.message = "Visualizer test passed";
        result.details["neurons_visualized"] = std::to_string(stats.neuronsVisualized);
        result.details["render_time"] = std::to_string(stats.renderTime);
        
    } catch (const std::exception& e) {
        result.status = TestStatus::FAILED;
        result.message = "Visualizer test failed: " + std::string(e.what());
        result.errors.push_back(e.what());
    }
    
    return result;
}

VisualizationTestResult::TestResult VisualizationTestRunner::runInteractiveVisualizerTest() {
    TestResult result;
    result.testName = "interactive";
    
    try {
        // Create interactive visualizer
        InteractiveVisualizer::InteractiveConfig config;
        config.enableMouseInteraction = true;
        config.enableKeyboardControls = true;
        config.zoomLevel = 1.0;
        
        InteractiveVisualizer interactiveVisualizer(config);
        
        // Initialize
        if (!interactiveVisualizer.initialize(800, 600, "Test Interactive Visualization")) {
            throw std::runtime_error("Failed to initialize interactive visualizer");
        }
        
        // Start
        interactiveVisualizer.start();
        
        // Update
        interactiveVisualizer.update(brain, 0.1);
        
        // Handle event
        InteractiveVisualizer::InteractionEvent event = InteractiveVisualizer::InteractionEvent::MOUSE_CLICK;
        std::vector<double> params = {100.0, 200.0, 0.0}; // x, y, button
        
        if (!interactiveVisualizer.handleEvent(event, params)) {
            throw std::runtime_error("Failed to handle interaction event");
        }
        
        // Process input
        interactiveVisualizer.processInput(0.1);
        
        // Render
        if (!interactiveVisualizer.render()) {
            throw std::runtime_error("Failed to render interactive visualization");
        }
        
        result.status = TestStatus::PASSED;
        result.message = "Interactive visualizer test passed";
        
    } catch (const std::exception& e) {
        result.status = TestStatus::FAILED;
        result.message = "Interactive visualizer test failed: " + std::string(e.what());
        result.errors.push_back(e.what());
    }
    
    return result;
}

VisualizationTestResult::TestResult VisualizationTestRunner::runNetworkVisualizerTest() {
    TestResult result;
    result.testName = "network";
    
    try {
        // Create network visualizer
        NetworkVisualizer::NetworkVisualizationConfig config;
        config.layout = NetworkVisualizer::LayoutAlgorithm::SPRING_EMBEDDING;
        config.mode = NetworkVisualizer::VisualizationMode::GRAPH;
        config.nodeSize = 5.0;
        config.connectionWidth = 1.0;
        
        NetworkVisualizer networkVisualizer(config);
        
        // Initialize
        if (!networkVisualizer.initialize(brain)) {
            throw std::runtime_error("Failed to initialize network visualizer");
        }
        
        // Update
        networkVisualizer.update(brain, 0.1);
        
        // Render
        if (!networkVisualizer.render()) {
            throw std::runtime_error("Failed to render network visualization");
        }
        
        // Get statistics
        auto stats = networkVisualizer.getStats();
        if (stats.totalNeurons == 0) {
            throw std::runtime_error("No neurons in network visualization");
        }
        
        result.status = TestStatus::PASSED;
        result.message = "Network visualizer test passed";
        result.details["neurons_visualized"] = std::to_string(stats.totalNeurons);
        result.details["synapses_visualized"] = std::to_string(stats.totalSynapses);
        
    } catch (const std::exception& e) {
        result.status = TestStatus::FAILED;
        result.message = "Network visualizer test failed: " + std::string(e.what());
        result.errors.push_back(e.what());
    }
    
    return result;
}

VisualizationTestResult::TestResult VisualizationTestRunner::runSpikeVisualizerTest() {
    TestResult result;
    result.testName = "spike";
    
    try {
        // Create spike visualizer
        SpikeVisualizer::SpikeVisualizationConfig config;
        config.mode = SpikeVisualizer::VisualizationMode::RASTER_PLOT;
        config.timeScale = 1.0;
        config.binWidth = 0.001;
        config.showIndividualSpikes = true;
        
        SpikeVisualizer spikeVisualizer(config);
        
        // Initialize
        if (!spikeVisualizer.initialize(brain)) {
            throw std::runtime_error("Failed to initialize spike visualizer");
        }
        
        // Update with spike data
        spikeVisualizer.update(brain, spikes, 0.1);
        
        // Render
        if (!spikeVisualizer.render()) {
            throw std::runtime_error("Failed to render spike visualization");
        }
        
        // Get spike statistics
        SpikeVisualizer::SpikeStats spikeStats = spikeVisualizer.getSpikeStats();
        if (spikeStats.totalSpikes == 0) {
            throw std::runtime_error("No spikes recorded");
        }
        
        result.status = TestStatus::PASSED;
        result.message = "Spike visualizer test passed";
        result.details["total_spikes"] = std::to_string(spikeStats.totalSpikes);
        result.details["spiking_neurons"] = std::to_string(spikeStats.totalSpikingNeurons);
        
    } catch (const std::exception& e) {
        result.status = TestStatus::FAILED;
        result.message = "Spike visualizer test failed: " + std::string(e.what());
        result.errors.push_back(e.what());
    }
    
    return result;
}

VisualizationTestResult::TestResult VisualizationTestRunner::runDataExporterTest() {
    TestResult result;
    result.testName = "data_exporter";
    
    try {
        // Create data exporter
        DataExporter::ExportConfig config;
        config.format = DataExporter::ExportFormat::CSV;
        config.outputPath = ".";
        config.filename = "test_export";
        config.includeMetadata = true;
        config.validateData = true;
        
        DataExporter dataExporter(config);
        
        // Export brain data
        if (!dataExporter.exportBrainData(brain, "test_brain")) {
            throw std::runtime_error("Failed to export brain data");
        }
        
        // Export neuron data
        if (!dataExporter.exportNeuronData(brain, "test_neurons")) {
            throw std::runtime_error("Failed to export neuron data");
        }
        
        // Export spike data
        if (!dataExporter.exportSpikeData(spikes, "test_spikes")) {
            throw std::runtime_error("Failed to export spike data");
        }
        
        result.status = TestStatus::PASSED;
        result.message = "Data exporter test passed";
        
    } catch (const std::exception& e) {
        result.status = TestStatus::FAILED;
        result.message = "Data exporter test failed: " + std::string(e.what());
        result.errors.push_back(e.what());
    }
    
    return result;
}

VisualizationTestResult::TestResult VisualizationTestRunner::runAnimationGeneratorTest() {
    TestResult result;
    result.testName = "animation";
    
    try {
        // Create animation generator
        AnimationGenerator::AnimationConfig config;
        config.type = AnimationGenerator::AnimationType::NEURAL_DYNAMICS;
        config.format = AnimationGenerator::AnimationFormat::GIF;
        config.frameRate = 10;
        config.duration = 5.0;
        config.width = 400;
        config.height = 300;
        
        AnimationGenerator animationGenerator(config);
        
        // Initialize
        if (!animationGenerator.initialize(brain)) {
            throw std::runtime_error("Failed to initialize animation generator");
        }
        
        // Add simple frame renderer
        animationGenerator.addFrameRenderer(
            [this](int frameIndex, double time) {
                // Simple frame renderer that returns a colored frame
                std::vector<uint8_t> frameData(config.width * config.height * 3, 255);
                // In real implementation, would render actual frame
                return frameData;
            }
        );
        
        // Set data source
        animationGenerator.setDataSource(
            [this](double time) {
                std::vector<std::vector<double>> data;
                // Generate data for animation
                return data;
            }
        );
        
        // Generate animation
        bool animationGenerated = animationGenerator.generate(brain, spikes,
            [this](double progress, const std::string& status) {
                NLM_LOG_INFO("Animation progress: " + std::to_string(progress) + "% - " + status);
            });
        
        if (!animationGenerated) {
            throw std::runtime_error("Failed to generate animation");
        }
        
        // Get animation statistics
        AnimationGenerator::AnimationStats stats = animationGenerator.getStats();
        if (stats.totalFrames == 0) {
            throw std::runtime_error("No frames generated");
        }
        
        result.status = TestStatus::PASSED;
        result.message = "Animation generator test passed";
        result.details["total_frames"] = std::to_string(stats.totalFrames);
        result.details["duration"] = std::to_string(stats.totalDuration);
        
    } catch (const std::exception& e) {
        result.status = TestStatus::FAILED;
        result.message = "Animation generator test failed: " + std::string(e.what());
        result.errors.push_back(e.what());
    }
    
    return result;
}

VisualizationTestResult::TestResult VisualizationTestRunner::runWebDashboardTest() {
    TestResult result;
    result.testName = "web";
    
    try {
        // Create web dashboard
        WebDashboard::DashboardConfig config;
        config.host = "localhost";
        config.port = 0; // Use random port for testing
        config.enableSSL = false;
        config.enableWebSocket = true;
        config.enableREST = true;
        config.enableCORS = true;
        
        WebDashboard webDashboard(config);
        
        // Start
        if (!webDashboard.start()) {
            throw std::runtime_error("Failed to start web dashboard");
        }
        
        // Update
        webDashboard.update(brain, spikes);
        
        // Check if running
        if (!webDashboard.isRunning()) {
            throw std::runtime_error("Web dashboard not running");
        }
        
        // Get statistics
        std::string stats = webDashboard.getStatistics();
        
        // Stop
        webDashboard.stop();
        
        result.status = TestStatus::PASSED;
        result.message = "Web dashboard test passed";
        result.details["stats_length"] = std::to_string(stats.length());
        
    } catch (const std::exception& e) {
        result.status = TestStatus::FAILED;
        result.message = "Web dashboard test failed: " + std::string(e.what());
        result.errors.push_back(e.what());
    }
    
    return result;
}

VisualizationTestResult::TestResult VisualizationTestRunner::runHTMLExporterTest() {
    TestResult result;
    result.testName = "html";
    
    try {
        // Create HTML exporter
        HTMLExporter::HTMLExportConfig config;
        config.template = HTMLExporter::TemplateType::DASHBOARD;
        config.pageConfig.title = "Test HTML Exporter";
        config.pageConfig.description = "HTML export test";
        config.includeCharts = true;
        config.includeChartsLibrary = true;
        config.enableNavigation = true;
        config.enableSearch = true;
        
        HTMLExporter htmlExporter(config);
        
        // Export brain visualization
        std::string outputPath = "test_html_output";
        bool htmlExported = htmlExporter.exportBrainVisualization(brain, outputPath);
        
        if (!htmlExported) {
            throw std::runtime_error("Failed to export brain visualization to HTML");
        }
        
        // Check if export is complete
        if (!htmlExporter.isComplete()) {
            throw std::runtime_error("HTML export not complete");
        }
        
        // Get export statistics
        HTMLExporter::ExportStats stats = htmlExporter.getStats();
        if (stats.totalSections == 0) {
            throw std::runtime_error("No sections exported");
        }
        
        result.status = TestStatus::PASSED;
        result.message = "HTML exporter test passed";
        result.details["total_sections"] = std::to_string(stats.totalSections);
        result.details["total_charts"] = std::to_string(stats.totalCharts);
        
    } catch (const std::exception& e) {
        result.status = TestStatus::FAILED;
        result.message = "HTML exporter test failed: " + std::string(e.what());
        result.errors.push_back(e.what());
    }
    
    return result;
}

std::string VisualizationTestRunner::testStatusToString(TestStatus status) const {
    switch (status) {
        case TestStatus::PASSED: return "PASSED";
        case TestStatus::FAILED: return "FAILED";
        case TestStatus::SKIPPED: return "SKIPPED";
        default: return "UNKNOWN";
    }
}

} // namespace nlm
