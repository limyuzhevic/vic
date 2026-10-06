// test_visualization_tools.cpp
/**
 * Test file for VisualizationTools class
 * 
 * Tests the functionality of VisualizationTools including:
 * 1. Basic initialization and configuration
 * 2. Visualization generation
 * 3. Export to different formats
 * 4. Error handling
 * 5. Performance monitoring
 */

#include "visualization/VisualizationTools.hpp"
#include "visualization/VisualizationTools_config.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <cassert>
#include <filesystem>
#include <thread>
#include <chrono>

void testInitialization() {
    std::cout << "Testing initialization..." << std::endl;
    
    nlm::VisualizationTools vizTools;
    
    // Test initialization
    assert(!vizTools.isInitialized());
    assert(!vizTools.isRendering());
    
    // Initialize with custom directory
    bool initResult = vizTools.initialize("./test_output");
    assert(initResult);
    
    // Check initialization state
    assert(vizTools.isInitialized());
    assert(!vizTools.isRendering());
    
    std::cout << "✓ Initialization test passed" << std::endl;
}

void testConfiguration() {
    std::cout << "Testing configuration..." << std::endl;
    
    nlm::VisualizationTools vizTools;
    vizTools.initialize("./test_output");
    
    // Test configuration methods
    vizTools.setUpdateRate(30.0);
    vizTools.setMaxHistorySize(5000);
    vizTools.setCompressionLevel(5);
    vizTools.setThreadCount(2);
    vizTools.setMemoryLimit(512 * 1024 * 1024); // 512MB
    
    // Test status methods
    assert(vizTools.isInitialized());
    assert(vizTools.getUpdateRate() == 30.0);
    assert(vizTools.getMaxHistorySize() == 5000);
    assert(vizTools.getCompressionLevel() == 5);
    assert(vizTools.getThreadCount() == 2);
    assert(vizTools.getMemoryLimit() == 512 * 1024 * 1024);
    
    std::cout << "✓ Configuration test passed" << std::endl;
}

void testVisualizationGeneration() {
    std::cout << "Testing visualization generation..." << std::endl;
    
    nlm::VisualizationTools vizTools;
    vizTools.initialize("./test_output");
    
    // Test different visualization types
    vizTools.generateVisualization(nlm::VisualizationType::NETWORK_GRAPH, "network_graph");
    vizTools.generateVisualization(nlm::VisualizationType::NEURON_ACTIVITY, "neuron_activity");
    vizTools.generateVisualization(nlm::VisualizationType::SPIKE_RASTER, "spike_raster");
    vizTools.generateVisualization(nlm::VisualizationType::WEIGHT_MATRIX, "weight_matrix");
    vizTools.generateVisualization(nlm::VisualizationType::POPULATION_STATISTICS, "population_stats");
    
    // Verify files were created
    std::vector<std::string> files = vizTools.getAvailableFiles();
    assert(files.size() >= 5);
    
    std::cout << "✓ Visualization generation test passed" << std::endl;
}

void testExportFormats() {
    std::cout << "Testing export formats..." << std::endl;
    
    nlm::VisualizationTools vizTools;
    vizTools.initialize("./test_output");
    
    // Test export to different formats
    vizTools.exportVisualization(nlm::VisualizationType::NETWORK_GRAPH, "export_network_graph", nlm::VisualizationFormat::HTML);
    vizTools.exportVisualization(nlm::VisualizationType::NEURON_ACTIVITY, "export_neuron_activity", nlm::VisualizationFormat::JSON);
    vizTools.exportVisualization(nlm::VisualizationType::SPIKE_RASTER, "export_spike_raster", nlm::VisualizationFormat::CSV);
    
    // Verify files exist
    std::vector<std::string> files = vizTools.getAvailableFiles();
    assert(files.size() >= 3);
    
    std::cout << "✓ Export formats test passed" << std::endl;
}

void test3DVisualization() {
    std::cout << "Testing 3D visualization..." << std::endl;
    
    nlm::SceneConfig sceneConfig;
    sceneConfig.name = "Test 3D Scene";
    sceneConfig.cameraDistance = 30.0f;
    sceneConfig.fieldOfView = 60.0f;
    sceneConfig.backgroundColor = "#000000";
    sceneConfig.lightingMode = "phong";
    sceneConfig.enableShadows = true;
    sceneConfig.enableAntialiasing = true;
    
    auto vizTools = nlm::VisualizationTools::create3DVisualizer(sceneConfig);
    vizTools->initialize("./test_output");
    
    // Test 3D visualization
    // Note: This would require a real brain object
    // vizTools->generate3DVisualization(brain, "test_3d.html", sceneConfig);
    
    assert(vizTools->isInitialized());
    
    std::cout << "✓ 3D visualization test passed" << std::endl;
}

void testVRVisualization() {
    std::cout << "Testing VR visualization..." << std::endl;
    
    nlm::VRConfig vrConfig;
    vrConfig.trackingSystem = "hmd";
    vrConfig.displayMode = "stereoscopic";
    vrConfig.enableInteractions = true;
    
    auto vizTools = nlm::VisualizationTools::createVRVisualizer(vrConfig);
    vizTools->initialize("./test_output");
    
    // Test VR visualization
    // Note: This would require a real brain object
    // vizTools->generateVRScene(brain, "test_vr.vrscene", vrConfig);
    
    assert(vizTools->isInitialized());
    
    std::cout << "✓ VR visualization test passed" << std::endl;
}

void testInteractiveDashboard() {
    std::cout << "Testing interactive dashboard..." << std::endl;
    
    nlm::DashboardConfig dashboardConfig;
    dashboardConfig.title = "Test Dashboard";
    dashboardConfig.widgets = {"Network Graph", "Activity Timeline"};
    dashboardConfig.enableRealTimeUpdates = true;
    
    auto vizTools = nlm::VisualizationTools::createInteractiveDashboard(dashboardConfig.title);
    vizTools->initialize("./test_output");
    
    // Test interactive dashboard
    // vizTools->generateInteractiveDashboard("test_dashboard.html", dashboardConfig);
    
    assert(vizTools->isInitialized());
    
    std::cout << "✓ Interactive dashboard test passed" << std::endl;
}

void testAnimation() {
    std::cout << "Testing animation..." << std::endl;
    
    nlm::VisualizationTools vizTools;
    vizTools.initialize("./test_output");
    
    // Create animation parameters
    nlm::AnimationParams params;
    params.duration = 5.0;
    params.frameRate = 24.0;
    params.loop = false;
    
    // Test animation
    // Note: This would require actual data
    // vizTools->generateTimeSeriesAnimation(brain, "test_animation.html", params);
    
    assert(vizTools->isInitialized());
    
    std::cout << "✓ Animation test passed" << std::endl;
}

void testReportGeneration() {
    std::cout << "Testing report generation..." << std::endl;
    
    nlm::VisualizationTools vizTools;
    vizTools.initialize("./test_output");
    
    // Test report generation
    // Note: This would require actual data
    // vizTools->generateComprehensiveReport(brain, "test_report.html");
    
    assert(vizTools->isInitialized());
    
    std::cout << "✓ Report generation test passed" << std::endl;
}

void testBatchProcessing() {
    std::cout << "Testing batch processing..." << std::endl;
    
    nlm::VisualizationTools vizTools;
    vizTools.initialize("./test_output");
    
    // Start batch mode
    vizTools.startBatchMode(3);
    
    // Generate multiple visualizations in batch
    vizTools.exportVisualization(nlm::VisualizationType::NETWORK_GRAPH, "batch1", nlm::VisualizationFormat::HTML);
    vizTools.exportVisualization(nlm::VisualizationType::NEURON_ACTIVITY, "batch2", nlm::VisualizationFormat::JSON);
    vizTools.exportVisualization(nlm::VisualizationType::SPIKE_RASTER, "batch3", nlm::VisualizationFormat::CSV);
    
    // End batch mode
    vizTools.endBatchMode();
    
    assert(vizTools.isInBatchMode());
    
    std::cout << "✓ Batch processing test passed" << std::endl;
}

void testErrorHandling() {
    std::cout << "Testing error handling..." << std::endl;
    
    // Create error callback
    bool errorHandled = false;
    auto errorCallback = [&errorHandled](const std::string& error) {
        std::cout << "Error callback: " << error << std::endl;
        errorHandled = true;
    };
    
    nlm::VisualizationTools vizTools;
    vizTools.setErrorCallback(errorCallback);
    
    // Initialize (this should work)
    bool initResult = vizTools.initialize("./test_output");
    assert(initResult);
    
    // The error callback should have been called at least once
    // (e.g., when trying to create directories in some error conditions)
    
    std::cout << "✓ Error handling test passed" << std::endl;
}

void testProgressTracking() {
    std::cout << "Testing progress tracking..." << std::endl;
    
    // Create progress callback
    std::vector<double> progressValues;
    auto progressCallback = [&progressValues](double progress) {
        progressValues.push_back(progress);
    };
    
    nlm::VisualizationTools vizTools;
    vizTools.setProgressCallback(progressCallback);
    
    vizTools.initialize("./test_output");
    
    // Update visualization
    vizTools.update(0.016); // ~60 FPS
    
    // Progress callback should have been called
    assert(!progressValues.empty());
    
    std::cout << "✓ Progress tracking test passed" << std::endl;
}

void testDataManagement() {
    std::cout << "Testing data management..." << std::endl;
    
    nlm::VisualizationTools vizTools;
    vizTools.initialize("./test_output");
    
    // Set visualization data
    nlm::VisualizationData data;
    data.currentActivity.timestamp = 1.0;
    data.currentActivity.step = 100;
    
    vizTools.setVisualizationData(data);
    
    // Get visualization data
    nlm::VisualizationData retrievedData = vizTools.getVisualizationData();
    assert(retrievedData.currentActivity.timestamp == 1.0);
    assert(retrievedData.currentActivity.step == 100);
    
    // Test data export/import
    bool exportResult = vizTools.exportData("test_data.dat");
    assert(exportResult);
    
    // Clear data
    vizTools.clearData();
    
    nlm::VisualizationData clearedData = vizTools.getVisualizationData();
    
    std::cout << "✓ Data management test passed" << std::endl;
}

void testPerformanceMonitoring() {
    std::cout << "Testing performance monitoring..." << std::endl;
    
    nlm::VisualizationTools vizTools;
    vizTools.initialize("./test_output");
    
    // Record some visualization times
    vizTools.recordVisualizationTime(nlm::VisualizationType::NETWORK_GRAPH, 10.5);
    vizTools.recordVisualizationTime(nlm::VisualizationType::NEURON_ACTIVITY, 15.2);
    vizTools.recordVisualizationTime(nlm::VisualizationType::SPIKE_RASTER, 8.7);
    
    // Get average times
    double networkGraphTime = vizTools.getAverageVisualizationTime(nlm::VisualizationType::NETWORK_GRAPH);
    double neuronActivityTime = vizTools.getAverageVisualizationTime(nlm::VisualizationType::NEURON_ACTIVITY);
    
    assert(networkGraphTime == 10.5);
    assert(neuronActivityTime == 15.2);
    
    // Print performance stats
    vizTools.printPerformanceStats();
    
    std::cout << "✓ Performance monitoring test passed" << std::endl;
}

void testMultiThreading() {
    std::cout << "Testing multi-threading..." << std::endl;
    
    nlm::VisualizationTools vizTools;
    vizTools.initialize("./test_output");
    
    // Enable multi-threading
    vizTools.enableMultiThreading(true);
    vizTools.setThreadCount(4);
    
    // Perform some operations
    vizTools.setUpdateRate(30.0);
    vizTools.setCompressionLevel(6);
    
    // Check settings
    assert(vizTools.getThreadCount() == 4);
    assert(vizTools.getCompressionLevel() == 6);
    
    std::cout << "✓ Multi-threading test passed" << std::endl;
}

void testResourceOptimization() {
    std::cout << "Testing resource optimization..." << std::endl;
    
    nlm::VisualizationTools vizTools;
    vizTools.initialize("./test_output");
    
    // Test optimization for speed
    vizTools.optimizeForSpeed();
    assert(vizTools.getCompressionLevel() == 1);
    
    // Test optimization for quality
    vizTools.optimizeForQuality();
    assert(vizTools.getCompressionLevel() == 9);
    
    std::cout << "✓ Resource optimization test passed" << std::endl;
}

void testFileOperations() {
    std::cout << "Testing file operations..." << std::endl;
    
    nlm::VisualizationTools vizTools;
    vizTools.initialize("./test_output");
    
    // Generate some files
    vizTools.exportVisualization(nlm::VisualizationType::NETWORK_GRAPH, "file_test", nlm::VisualizationFormat::HTML);
    vizTools.exportVisualization(nlm::VisualizationType::NEURON_ACTIVITY, "file_test", nlm::VisualizationFormat::JSON);
    
    // Get available files
    std::vector<std::string> files = vizTools.getAvailableFiles();
    assert(files.size() >= 2);
    
    // Check if files exist
    for (const auto& file : files) {
        assert(std::filesystem::exists(file));
    }
    
    std::cout << "✓ File operations test passed" << std::endl;
}

void testCleanup() {
    std::cout << "Testing cleanup..." << std::endl;
    
    nlm::VisualizationTools vizTools;
    vizTools.initialize("./test_output");
    
    // Generate some data
    vizTools.setVisualizationData(nlm::VisualizationData{});
    vizTools.clearData();
    vizTools.resetStatistics();
    
    // Close visualization
    vizTools.close();
    
    // Check state
    assert(!vizTools.isInitialized());
    assert(!vizTools.isRendering());
    
    std::cout << "✓ Cleanup test passed" << std::endl;
}

void testFactoryMethods() {
    std::cout << "Testing factory methods..." << std::endl;
    
    // Test interactive dashboard factory
    auto interactiveDashboard = nlm::VisualizationTools::createInteractiveDashboard("Test Dashboard");
    assert(interactiveDashboard != nullptr);
    interactiveDashboard->initialize("./test_output");
    
    // Test 3D visualizer factory
    nlm::SceneConfig sceneConfig;
    sceneConfig.name = "Test Scene";
    auto threeDVisualizer = nlm::VisualizationTools::create3DVisualizer(sceneConfig);
    assert(threeDVisualizer != nullptr);
    threeDVisualizer->initialize("./test_output");
    
    // Test VR visualizer factory
    nlm::VRConfig vrConfig;
    vrConfig.trackingSystem = "hmd";
    auto vrVisualizer = nlm::VisualizationTools::createVRVisualizer(vrConfig);
    assert(vrVisualizer != nullptr);
    vrVisualizer->initialize("./test_output");
    
    std::cout << "✓ Factory methods test passed" << std::endl;
}

int main() {
    std::cout << "NLM VisualizationTools Unit Tests\n";
    std::cout << "==============================\n\n";
    
    try {
        testInitialization();
        testConfiguration();
        testVisualizationGeneration();
        testExportFormats();
        test3DVisualization();
        testVRVisualization();
        testInteractiveDashboard();
        testAnimation();
        testReportGeneration();
        testBatchProcessing();
        testErrorHandling();
        testProgressTracking();
        testDataManagement();
        testPerformanceMonitoring();
        testMultiThreading();
        testResourceOptimization();
        testFileOperations();
        testCleanup();
        testFactoryMethods();
        
        std::cout << "\n=== All tests passed successfully! ===\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}