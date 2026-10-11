#include "AdvancedNeuronTests.hpp"
#include <iostream>

int main() {
    std::cout << "NLM Advanced Neuron Model Tests" << std::endl;
    std::cout << "===============================" << std::endl;
    
    try {
        nlm::AdvancedNeuronTests::runAllTests();
        
        std::cout << std::endl;
        std::cout << "All tests completed successfully!" << std::endl;
        std::cout << "Phase 5 (Advanced Neural Network Models and Algorithms) - Neuron Models Module:" << std::endl;
        std::cout << "  ✓ Hodgkin-Huxley Neuron Implementation" << std::endl;
        std::cout << "  ✓ Izhikevich Neuron Implementation" << std::endl;
        std::cout << "  ✓ Adaptive Exponential Integrate-and-Fire Neuron Implementation" << std::endl;
        std::cout << "  ✓ Theta Neuron Implementation" << std::endl;
        std::cout << "  ✓ Morris-Lecar Neuron Implementation" << std::endl;
        std::cout << "  ✓ Advanced Neuron Factory" << std::endl;
        std::cout << "  ✓ Neuron Model Configuration System" << std::endl;
        std::cout << "  ✓ Integration with Existing NLM Framework" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << std::endl;
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << std::endl;
        std::cerr << "Test failed with unknown exception!" << std::endl;
        return 1;
    }
}
