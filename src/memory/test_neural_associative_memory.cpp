#ifdef TEST_COMPILE
// Test compilation of NeuralAssociativeMemory implementation

#include "src/memory/NeuralAssociativeMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/brain/Brain.hpp"
#include <iostream>

namespace nlm {

// Test if the implementation is complete
void testNeuralAssociativeMemory() {
    std::cout << "Testing NeuralAssociativeMemory compilation..." << std::endl;
    
    // Create a mock brain reference for testing
    class MockBrain {
    public:
        void injectCurrent(NeuronId neuron, MembranePotential current) {}
    };
    
    MockBrain mockBrain;
    NeuralAssociativeMemory associativeMemory;
    associativeMemory.initialize(&mockBrain);
    
    // Test basic functionality
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> pattern2 = {0.4f, 0.5f, 0.6f};
    
    associativeMemory.associate(pattern1, pattern2, 0.8f);
    
    float strength = associativeMemory.getAssociationStrength(pattern1, pattern2);
    std::cout << "Association strength: " << strength << std::endl;
    
    // Test spread activation
    auto retrieved = associativeMemory.retrieve(pattern1, 3);
    std::cout << "Retrieved " << retrieved.size() << " patterns" << std::endl;
    
    std::cout << "NeuralAssociativeMemory compilation test completed successfully!" << std::endl;
}

} // namespace nlm

#endif // TEST_COMPILE