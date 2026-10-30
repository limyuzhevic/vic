#include "src/visualization/VisualizationInterface.hpp"
#include <string>
#include <iostream>

int main() {
    // Test that all methods exist and have correct signatures
    nlm::VisualizationInterface vis;
    
    // Test initialize
    bool initResult = vis.initialize();
    
    // Test isActive
    bool active = vis.isActive();
    
    // Test update rate
    vis.setUpdateRate(30.0);
    double rate = vis.getUpdateRate();
    
    // Test close
    vis.close();
    
    return 0;
}
