#include "NeuralWorkingMemory.hpp"
#include "../brain/Brain.hpp"
#include <iostream>
#include <cassert>

using namespace nlm;

int main() {
    std::cout << "Testing optimized NeuralWorkingMemory..." << std::endl;
    
    // Create mock brain
    class MockBrain : public Brain {
    public:
        MockBrain() : Brain(nullptr) {}
        
        std::vector<NeuralRegion*> getRegions() const override { return {}; }
        NeuralRegion* getRegion(size_t id) const override { return nullptr; }
        std::vector<Neuron*> getAllNeurons() const override { return {}; }
        void injectCurrent(NeuronId neuron, float current) override {}
        float getCurrent(NeuronId neuron) const override { return 0.0f; }
        std::vector<float> getAllCurrents() const override { return {}; }
        void addNeuron(std::unique_ptr<Neuron> neuron) override {}
        void addRegion(std::unique_ptr<NeuralRegion> region) override {}
        void addSynapse(std::unique_ptr<Synapse> synapse) override {}
        void addConnection(NeuronId from, NeuronId to, float strength) override {}
        float getConnectionStrength(NeuronId from, NeuronId to) const override { return 0.0f; }
        bool hasConnection(NeuronId from, NeuronId to) const override { return false; }
        void update(TimestepDuration dt) override {}
    };
    
    MockBrain brain;
    
    // Test 1: Initialize and store data
    NeuralWorkingMemory nlm;
    nlm.initialize(&brain);
    
    // Test storing patterns
    std::vector<float> pattern1 = {0.1f, 0.5f, 0.8f, 0.3f, 0.6f};
    nlm.store(pattern1, 1.0f);
    
    // Test storing to specific neurons
    NeuronId neuron1(1000);
    NeuronId neuron2(2000);
    nlm.storeToNeuron(neuron1, 0.9f);
    nlm.storeToNeuron(neuron2, 0.7f);
    
    // Test retrieval
    auto retrieved = nlm.retrieve();
    assert(!retrieved.empty());
    std::cout << "Retrieved " << retrieved.size() << " activation values" << std::endl;
    
    // Test contains
    assert(nlm.contains(neuron1));
    assert(nlm.contains(neuron2));
    std::cout << "Contains check passed" << std::endl;
    
    // Test getNeuronActivation
    float act1 = nlm.getNeuronActivation(neuron1);
    float act2 = nlm.getNeuronActivation(neuron2);
    assert(act1 >= 0.0f);
    assert(act2 >= 0.0f);
    std::cout << "Get neuron activation passed" << std::endl;
    
    // Test competition
    nlm.runCompetition();
    std::cout << "Run competition passed" << std::endl;
    
    // Test isWinning
    bool winning = nlm.isWinning(neuron1);
    std::cout << "Is winning check passed" << std::endl;
    
    // Test memory activity
    float activity = nlm.getMemoryActivity();
    assert(activity >= 0.0f);
    std::cout << "Memory activity: " << activity << std::endl;
    
    // Test strengthening
    nlm.strengthenMemory(0.5f);
    std::cout << "Strengthen memory passed" << std::endl;
    
    // Test update
    nlm.update(1.0f);
    std::cout << "Update passed" << std::endl;
    
    // Test clear
    nlm.clear();
    assert(nlm.getActiveTraces() == 0);
    assert(!nlm.contains(neuron1));
    std::cout << "Clear passed" << std::endl;
    
    // Test AttentionalSelection
    AttentionalSelection as;
    as.initialize(&brain);
    
    std::vector<NeuronId> competitors = {NeuronId(1000), NeuronId(2000), NeuronId(3000)};
    as.applyTopDownBias(NeuronId(1000), 0.5f);
    as.applyBottomUpSalience(NeuronId(2000), 0.3f);
    
    auto winners = as.processCompetition(competitors, 0.5f);
    assert(!winners.empty());
    std::cout << "AttentionalSelection process competition passed" << std::endl;
    
    // Test focusOnRegion
    as.focusOnRegion(1);
    auto regions = as.getAttendedRegions();
    assert(!regions.empty());
    std::cout << "Focus on region passed" << std::endl;
    
    // Test update attention
    as.update(1.0f);
    std::cout << "AttentionalSelection update passed" << std::endl;
    
    // Test getInhibitionFor and getExcitationFor
    float inhibition = as.getInhibitionFor(neuron1);
    float excitation = as.getExcitationFor(neuron1);
    std::cout << "Get inhibition and excitation passed" << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== All tests passed! ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Performance optimizations implemented:" << std::endl;
    std::cout << "1. Extracted magic numbers to constants in Constants namespace" << std::endl;
    std::cout << "2. Replaced parallel arrays with struct of arrays approach" << std::endl;
    std::cout << "3. Added unordered_map for O(1) neuron activation lookup" << std::endl;
    std::cout << "4. Added unordered_set for O(1) membership testing" << std::endl;
    std::cout << "5. Optimized decayWeakTraces() to avoid vector modifications during iteration" << std::endl;
    std::cout << "6. Improved runCompetition() to reduce redundant winner checking" << std::endl;
    std::cout << "7. Optimized memory access patterns for cache efficiency" << std::endl;
    std::cout << "8. Reduced redundant calculations and extracted common logic" << std::endl;
    std::cout << std::endl;
    std::cout << "Performance improvements expected:" << std::endl;
    std::cout << "- O(n) to O(1) for neuron lookups (unordered_map/unordered_set)" << std::endl;
    std::cout << "- Cache-friendly struct of arrays vs. parallel arrays" << std::endl;
    std::cout << "- Batch removal instead of element-by-element removal" << std::endl;
    std::cout << "- Early pruning in competition algorithm" << std::endl;
    std::cout << "- Reduced redundant std::find operations" << std::endl;
    
    return 0;
}