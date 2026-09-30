// Visualization Tests
// Tests for the enhanced visualization and analysis capabilities

#include "visualization/VisualizationInterface.hpp"
#include <iostream>
#include <cassert>

void testVisualizationInterfaceCreation() {
    std::cout << "Running visualization interface creation tests..." << std::endl;
    
    nlm::VisualizationInterface viz;
    
    // Test default initialization
    bool success = viz.initialize();
    assert(success);
    
    assert(viz.isActive());
    assert(viz.getUpdateRate() > 0.0);
    
    // Test shutdown
    viz.close();
    assert(!viz.isActive());
    
    std::cout << "Visualization interface creation tests passed!" << std::endl;
}

void testVisualizationInterfaceInitialization() {
    std::cout << "Running visualization interface initialization tests..." << std::endl;
    
    nlm::VisualizationInterface viz;
    
    // Test with custom options
    std::map<std::string, std::string> options;
    options["active"] = "true";
    options["update_rate"] = "60.0";
    options["output_directory"] = "./test_visualizations";
    options["default_backend"] = "plotly";
    
    bool success = viz.initialize(options);
    assert(success);
    
    assert(viz.isActive());
    assert(viz.getUpdateRate() == 60.0);
    assert(viz.getOutputDirectory() == "./test_visualizations");
    assert(viz.getDefaultBackend() == "plotly");
    
    // Test property setters
    viz.setUpdateRate(120.0);
    assert(viz.getUpdateRate() == 120.0);
    
    viz.setOutputDirectory("./custom_visualizations");
    assert(viz.getOutputDirectory() == "./custom_visualizations");
    
    viz.setDefaultBackend("matplotlib");
    assert(viz.getDefaultBackend() == "matplotlib");
    
    // Clean up
    viz.close();
    
    std::cout << "Visualization interface initialization tests passed!" << std::endl;
}

void testVisualizationBackendSelection() {
    std::cout << "Running visualization backend selection tests..." << std::endl;
    
    nlm::VisualizationInterface viz;
    
    // Test backend initialization with options
    std::map<std::string, std::string> options;
    options["output_directory"] = "./test_backends";
    
    bool success = viz.initialize(options);
    assert(success);
    
    // Test different backend names
    viz.setDefaultBackend("plotly");
    assert(viz.getDefaultBackend() == "plotly");
    
    viz.setDefaultBackend("matplotlib");
    assert(viz.getDefaultBackend() == "matplotlib");
    
    viz.setDefaultBackend("custom_backend");
    assert(viz.getDefaultBackend() == "custom_backend");
    
    // Clean up
    viz.close();
    
    std::cout << "Visualization backend selection tests passed!" << std::endl;
}

void testVisualizationOutputDirectory() {
    std::cout << "Running visualization output directory tests..." << std::endl;
    
    nlm::VisualizationInterface viz;
    
    // Test default output directory
    std::string defaultDir = viz.getOutputDirectory();
    assert(defaultDir == "./visualizations" || defaultDir == "./test_visualizations");
    
    // Test custom output directory
    viz.setOutputDirectory("./my_visualizations");
    assert(viz.getOutputDirectory() == "./my_visualizations");
    
    // Test that output directory changes don't affect other instances
    nlm::VisualizationInterface viz2;
    viz2.initialize();
    
    // viz2 should have its own output directory
    std::string viz2Dir = viz2.getOutputDirectory();
    assert(viz2Dir == "./visualizations" || viz2Dir == "./test_visualizations");
    
    // viz should still have its custom directory
    assert(viz.getOutputDirectory() == "./my_visualizations");
    
    // Clean up
    viz.close();
    viz2.close();
    
    std::cout << "Visualization output directory tests passed!" << std::endl;
}

void testVisualizationIntegration() {
    std::cout << "Running visualization integration tests..." << std::endl;
    
    nlm::VisualizationInterface viz;
    
    std::map<std::string, std::string> options;
    options["active"] = "true";
    options["output_directory"] = "./integration_tests";
    
    bool success = viz.initialize(options);
    assert(success);
    
    assert(viz.isActive());
    
    // Test that visualization interface can be used multiple times
    viz.setUpdateRate(30.0);
    viz.setUpdateRate(45.0);  // Should override
    
    assert(viz.getUpdateRate() == 45.0);
    
    // Test cleanup
    viz.close();
    assert(!viz.isActive());
    
    // Test re-initialization
    success = viz.initialize();
    assert(success);
    assert(viz.isActive());
    
    viz.close();
    
    std::cout << "Visualization integration tests passed!" << std::endl;
}

int main() {
    std::cout << "=== Visualization Tests ===" << std::endl << std::endl;
    
    testVisualizationInterfaceCreation();
    testVisualizationInterfaceInitialization();
    testVisualizationBackendSelection();
    testVisualizationOutputDirectory();
    testVisualizationIntegration();
    
    std::cout << std::endl;
    std::cout << "=== All Visualization Tests Passed Successfully ===" << std::endl;
    
    return 0;
}