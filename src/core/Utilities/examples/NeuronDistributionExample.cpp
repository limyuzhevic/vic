// ============================================================================
// NEURON DISTRIBUTION UTILITIES EXAMPLE
// ============================================================================

#include <iostream>
#include <vector>
#include "Utilities.hpp"

int main() {
    std::cout << "=== Neuron Distribution Utilities Example ===\n";
    
    // Example 1: Distribute neurons across motor groups
    size_t totalMotorNeurons = 100;
    auto motorDistribution = nlm::distributeMotorNeurons(totalMotorNeurons);
    
    std::cout << "Motor neuron distribution (total: " << totalMotorNeurons << "):\n";
    for (size_t i = 0; i < motorDistribution.size(); ++i) {
        std::cout << "  Group " << i << ": " << motorDistribution[i] << " neurons\n";
    }
    
    // Example 2: Distribute sensory neurons
    size_t totalSensoryNeurons = 42;
    auto sensoryDistribution = nlm::distributeSensoryNeurons(totalSensoryNeurons);
    
    std::cout << "\nSensory neuron distribution (total: " << totalSensoryNeurons << "):\n";
    for (size_t i = 0; i < sensoryDistribution.size(); ++i) {
        std::cout << "  Modality " << i << ": " << sensoryDistribution[i] << " neurons\n";
    }
    
    // Example 3: Custom group distribution with start index
    auto customDistribution = nlm::distributeToGroups(25, 5, 2); // Start from group 2
    
    std::cout << "\nCustom distribution (25 neurons, 5 groups, start=2):\n";
    for (size_t i = 0; i < customDistribution.size(); ++i) {
        std::cout << "  Group " << i << ": " << customDistribution[i] << " neurons\n";
    }
    
    return 0;
}
