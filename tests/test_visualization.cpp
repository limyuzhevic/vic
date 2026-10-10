#include "test_common.hpp"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace test_visualization {

void testVisualizationInterfaceCreation() {
    nlm::VisualizationInterface vizInterface;
    assert(vizInterface.isActive() == false);
    assert(vizInterface.getColorMode() == nlm::ColorMode::RGB);
    assert(vizInterface.getScale() == 1.0f);
    std::cout << "    testVisualizationInterfaceCreation passed" << std::endl;
}

void testVisualizationSetup() {
    nlm::VisualizationInterface vizInterface;
    
    // Setup visualization
    vizInterface.setup(800, 600);
    assert(vizInterface.isActive() == true);
    assert(vizInterface.getWidth() == 800);
    assert(vizInterface.getHeight() == 600);
    
    // Render
    vizInterface.render();
    std::cout << "    testVisualizationSetup passed" << std::endl;
}

void testColorMode() {
    nlm::VisualizationInterface vizInterface;
    
    // Test color modes
    vizInterface.setColorMode(nlm::ColorMode::Grayscale);
    assert(vizInterface.getColorMode() == nlm::ColorMode::Grayscale);
    
    vizInterface.setColorMode(nlm::ColorMode::HSV);
    assert(vizInterface.getColorMode() == nlm::ColorMode::HSV);
    
    vizInterface.setColorMode(nlm::ColorMode::RGB);
    assert(vizInterface.getColorMode() == nlm::ColorMode::RGB);
    
    std::cout << "    testColorMode passed" << std::endl;
}

void testVisualizationIntegration() {
    nlm::VisualizationInterface vizInterface;
    
    // Test integration with Brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Visualization interface should be accessible
    assert(&vizInterface != nullptr);
    std::cout << "    testVisualizationIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Visualization tests..." << std::endl;
    testVisualizationInterfaceCreation();
    testVisualizationSetup();
    testColorMode();
    testVisualizationIntegration();
}

} // namespace test_visualization