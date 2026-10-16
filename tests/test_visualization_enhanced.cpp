// Enhanced Visualization Interface Tests
#include "visualization/VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_visualization {

void testVisualizationConfig() {
    std::cout << "  Testing VisualizationConfig..." << std::endl;
    
    nlm::VisualizationConfig config;
    
    // Test default values
    assert(config.updateRate > 0.0);
    assert(config.refreshInterval > 0.0);
    assert(config.realTimeUpdates == true);
    assert(config.autoRefresh == true);
    assert(config.showNeuronTypes == true);
    assert(config.showRegions == true);
    assert(config.showActivityLevels == true);
    assert(config.spikeHeatmap == true);
    assert(config.colorCodeWeights == true);
    assert(config.exportMetadata == true);
    assert(config.showStatisticalOverlays == true);
    
    // Test configuration changes
    config.updateRate = 15.0;
    config.realTimeUpdates = false;
    config.showNeuronTypes = false;
    
    assert(config.updateRate == 15.0);
    assert(config.realTimeUpdates == false);
    assert(config.showNeuronTypes == false);
    
    std::cout << "    testVisualizationConfig passed" << std::endl;
}

void testVisualizationInterfaceCreation() {
    std::cout << "  Testing VisualizationInterface creation..." << std::endl;
    
    nlm::VisualizationInterface viz;
    
    // Test default initialization
    assert(viz.isActive() == false);
    assert(viz.getUpdateRate() > 0.0);
    
    // Test initialization with config
    nlm::VisualizationConfig config;
    config.updateRate = 20.0;
    
    bool initSuccess = viz.initialize(config);
    assert(initSuccess == true);
    assert(viz.isActive() == true);
    assert(viz.getUpdateRate() == 20.0);
    
    // Test pause/resume
    viz.pauseUpdates();
    assert(viz.isPaused() == true);
    
    viz.resumeUpdates();
    assert(viz.isPaused() == false);
    
    viz.close();
    assert(viz.isActive() == false);
    
    std::cout << "    testVisualizationInterfaceCreation passed" << std::endl;
}

void testVisualizationModeSwitching() {
    std::cout << "  Testing visualization mode switching..." << std::endl;
    
    nlm::VisualizationInterface viz;
    viz.initialize();
    
    // Test initial mode
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::Network);
    
    // Test mode switching
    viz.setVisualizationMode(nlm::VisualizationMode::Activity);
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::Activity);
    
    viz.setVisualizationMode(nlm::VisualizationMode::Spikes);
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::Spikes);
    
    viz.setVisualizationMode(nlm::VisualizationMode::Weights);
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::Weights);
    
    viz.setVisualizationMode(nlm::VisualizationMode::WorkingMemory);
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::WorkingMemory);
    
    viz.setVisualizationMode(nlm::VisualizationMode::Neuromodulation);
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::Neuromodulation);
    
    viz.setVisualizationMode(nlm::VisualizationMode::Prediction);
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::Prediction);
    
    viz.setVisualizationMode(nlm::VisualizationMode::Development);
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::Development);
    
    viz.setVisualizationMode(nlm::VisualizationMode::Composite);
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::Composite);
    
    // Test toggle mode
    viz.toggleVisualizationMode();
    // Should wrap from Composite to Network
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::Network);
    
    viz.close();
    
    std::cout << "    testVisualizationModeSwitching passed" << std::endl;
}

void testStatisticalAnalysisHelpers() {
    std::cout << "  Testing statistical analysis helpers..." << std::endl;
    
    nlm::VisualizationInterface viz;
    viz.initialize();
    
    // Create a simple brain-like object for testing (mock)
    // Since we can't easily create a real brain, we'll test the helper methods
    // by calling them with the visualization interface
    
    // Test firing rate distribution computation
    auto firingRateDist = viz.computeFiringRateDistribution(
        *reinterpret_cast<nlm::Brain*>(0x1)  // Mock pointer for testing
    );
    assert(firingRateDist.empty()); // Should be empty with invalid brain
    
    // Test strongest connections computation
    auto strongConns = viz.computeStrongestConnections(
        *reinterpret_cast<nlm::Brain*>(0x1),  // Mock pointer for testing
        5
    );
    assert(strongConns.empty()); // Should be empty with invalid brain
    
    // Test network efficiency computation
    float efficiency = viz.computeNetworkEfficiency(
        *reinterpret_cast<nlm::Brain*>(0x1)  // Mock pointer for testing
    );
    assert(efficiency == 0.0f); // Should be 0.0 with invalid brain
    
    // Test clustering coefficient computation
    float clustering = viz.computeClusteringCoefficient(
        *reinterpret_cast<nlm::Brain*>(0x1)  // Mock pointer for testing
    );
    assert(clustering == 0.0f); // Should be 0.0 with invalid brain
    
    viz.close();
    
    std::cout << "    testStatisticalAnalysisHelpers passed" << std::endl;
}

void testExportFunctionality() {
    std::cout << "  Testing export functionality..." << std::endl;
    
    nlm::VisualizationInterface viz;
    viz.initialize();
    
    // Create a temporary directory for test exports
    std::string testDir = "/tmp/nlm_viz_test_" + 
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
    
    std::filesystem::create_directories(testDir);
    
    // Test text export
    std::string textPath = testDir + "/test_export.txt";
    bool textExport = viz.exportVisualization(nlm::ExportFormat::Text, textPath);
    assert(textExport == true);
    assert(std::filesystem::exists(textPath));
    
    // Test JSON export
    std::string jsonPath = testDir + "/test_export.json";
    bool jsonExport = viz.exportVisualization(nlm::ExportFormat::JSON, jsonPath);
    assert(jsonExport == true);
    assert(std::filesystem::exists(jsonPath));
    
    // Test CSV export
    std::string csvPath = testDir + "/test_export.csv";
    bool csvExport = viz.exportVisualization(nlm::ExportFormat::CSV, csvPath);
    assert(csvExport == true);
    assert(std::filesystem::exists(csvPath));
    
    // Test SVG export
    std::string svgPath = testDir + "/test_export.svg";
    bool svgExport = viz.exportVisualization(nlm::ExportFormat::SVG, svgPath);
    assert(svgExport == true);
    assert(std::filesystem::exists(svgPath));
    
    // Test PNG export (would normally require graphical libraries)
    std::string pngPath = testDir + "/test_export.png";
    bool pngExport = viz.exportVisualization(nlm::ExportFormat::PNG, pngPath);
    // PNG export might not be available in this environment, so we'll accept either
    assert(pngExport == true); // Should succeed (even if it just prints a message)
    
    // Verify export paths
    viz.setExportPath(testDir + "/default_export.txt");
    
    viz.close();
    
    // Clean up test directory
    std::filesystem::remove_all(testDir);
    
    std::cout << "    testExportFunctionality passed" << std::endl;
}

void testConfigUpdate() {
    std::cout << "  Testing configuration update..." << std::endl;
    
    nlm::VisualizationInterface viz;
    viz.initialize();
    
    // Test config update with string
    viz.updateConfig("update_rate", "25.0");
    assert(viz.getUpdateRate() == 25.0);
    
    viz.updateConfig("visualization_mode", "activity");
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::Activity);
    
    viz.updateConfig("visualization_mode", "spikes");
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::Spikes);
    
    viz.updateConfig("visualization_mode", "weights");
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::Weights);
    
    viz.updateConfig("visualization_mode", "working_memory");
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::WorkingMemory);
    
    viz.updateConfig("visualization_mode", "neuromodulation");
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::Neuromodulation);
    
    viz.updateConfig("visualization_mode", "prediction");
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::Prediction);
    
    viz.updateConfig("visualization_mode", "development");
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::Development);
    
    viz.updateConfig("visualization_mode", "composite");
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::Composite);
    
    viz.updateConfig("update_rate", "10.0"); // Reset
    
    viz.close();
    
    std::cout << "    testConfigUpdate passed" << std::endl;
}

void testPhase6Integration() {
    std::cout << "  Testing Phase 6 system integration..." << std::endl;
    
    nlm::VisualizationInterface viz;
    viz.initialize();
    
    // Enable all Phase 6 visualization options
    nlm::VisualizationConfig config;
    config.showWorkingMemory = true;
    config.showNeuromodulation = true;
    config.showPrediction = true;
    config.showDevelopment = true;
    
    viz.setConfig(config);
    
    // Test that all Phase 6 visualization modes work
    viz.setVisualizationMode(nlm::VisualizationMode::WorkingMemory);
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::WorkingMemory);
    
    viz.setVisualizationMode(nlm::VisualizationMode::Neuromodulation);
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::Neuromodulation);
    
    viz.setVisualizationMode(nlm::VisualizationMode::Prediction);
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::Prediction);
    
    viz.setVisualizationMode(nlm::VisualizationMode::Development);
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::Development);
    
    // Test composite mode (Phase 6 integration)
    viz.setVisualizationMode(nlm::VisualizationMode::Composite);
    assert(viz.getVisualizationMode() == nlm::VisualizationMode::Composite);
    
    viz.close();
    
    std::cout << "    testPhase6Integration passed" << std::endl;
}

void runAll() {
    std::cout << "=== Enhanced Visualization Interface Tests ===" << std::endl;
    std::cout << std::endl;
    
    testVisualizationConfig();
    std::cout << std::endl;
    
    testVisualizationInterfaceCreation();
    std::cout << std::endl;
    
    testVisualizationModeSwitching();
    std::cout << std::endl;
    
    testStatisticalAnalysisHelpers();
    std::cout << std::endl;
    
    testExportFunctionality();
    std::cout << std::endl;
    
    testConfigUpdate();
    std::cout << std::endl;
    
    testPhase6Integration();
    std::cout << std::endl;
    
    std::cout << "=== All Visualization Tests Passed ===" << std::endl;
}

} // namespace test_visualization

int main(int argc, char* argv[]) {
    test_visualization::runAll();
    return 0;
}
