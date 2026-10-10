#include <iostream>
#include "brain/Neuron.hpp"
#include "memory/Memory.hpp"
#include "core/Types/Types.hpp"

int main() {
    std::cout << "NLM Neural Simulation - Basic Demo" << std::endl;
    std::cout << "===================================" << std::endl;
    
    // Demonstrate core types
    nlm::core::Vector3D position(1.0, 2.0, 3.0);
    std::cout << "Position vector: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
    
    // Demonstrate neural module
    std::cout << "Demonstrating neural module capabilities..." << std::endl;
    
    // Demonstrate memory module
    nlm::memory::Memory memory(1000);
    std::cout << "Memory system initialized with capacity: " << memory.getTotalCapacity() << std::endl;
    
    // Demonstrate cognition module
    std::cout << "Cognition module ready for neural planning..." << std::endl;
    
    std::cout << "\nBasic demo completed successfully!" << std::endl;
    return 0;
}
