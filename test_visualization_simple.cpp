#include "src/visualization/VisualizationInterface.hpp"
#include <iostream>

int main() {
    nlm::VisualizationInterface vis;
    if (vis.initialize()) {
        std::cout << "Visualization initialized successfully" << std::endl;
        vis.update();
        vis.render();
        vis.close();
    }
    return 0;
}
