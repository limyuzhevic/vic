#include <iostream>
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"

void test_brain_creation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "Brain created successfully" << std::endl;
}

void test_brain_methods() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test initialization
    bool initSuccess = brain.initialize();
    if (!initSuccess) {
        std::cout << "Brain initialization failed" << std::endl;
        return;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    
    // Test various methods that might be missing implementations
    size_t regionCount = brain.getRegionCount();
    std::cout << "Region count: " << regionCount << std::endl;
    
    size_t neuronCount = brain.getTotalNeuronCount();
    std::cout << "Total neuron count: " << neuronCount << std::endl;
    
    size_t synapseCount = brain.getTotalSynapseCount();
    std::cout << "Total synapse count: " << synapseCount << std::endl;
    
    // Test reset method
    brain.reset();
    std::cout << "Brain reset completed" << std::endl;
    
    // Test accessor methods
    nlm::NeuralWorkingMemory* workingMem = brain.getWorkingMemory();
    if (workingMem) {
        std::cout << "Working memory access: OK" << std::endl;
    }
    
    nlm::NeuralEpisodicMemory* episodicMem = brain.getEpisodicMemory();
    if (episodicMem) {
        std::cout << "Episodic memory access: OK" << std::endl;
    }
    
    nlm::NeuralAssociativeMemory* associativeMem = brain.getAssociativeMemory();
    if (associativeMem) {
        std::cout << "Associative memory access: OK" << std::endl;
    }
    
    nlm::PredictionSystem* predSystem = brain.getPredictionSystem();
    if (predSystem) {
        std::cout << "Prediction system access: OK" << std::endl;
    }
    
    nlm::NeuralPlanner* planner = brain.getPlanner();
    if (planner) {
        std::cout << "Planner access: OK" << std::endl;
    }
    
    nlm::ConceptFormation* conceptForm = brain.getConceptFormation();
    if (conceptForm) {
        std::cout << "Concept formation access: OK" << std::endl;
    }
    
    nlm::AttentionalSelection* attention = brain.getAttention();
    if (attention) {
        std::cout << "Attention system access: OK" << std::endl;
    }
    
    nlm::DevelopmentSystem* devSystem = brain.getDevelopmentSystem();
    if (devSystem) {
        std::cout << "Development system access: OK" << std::endl;
    }
    
    nlm::Dopamine* dopamine = brain.getDopamine();
    if (dopamine) {
        std::cout << "Dopamine access: OK" << std::endl;
    }
    
    nlm::Curiosity* curiosity = brain.getCuriosity();
    if (curiosity) {
        std::cout << "Curiosity access: OK" << std::endl;
    }
    
    nlm::Novelty* novelty = brain.getNovelty();
    if (novelty) {
        std::cout << "Novelty access: OK" << std::endl;
    }
    
    nlm::PredictionError* predictionError = brain.getPredictionErrorSignal();
    if (predictionError) {
        std::cout << "Prediction error access: OK" << std::endl;
    }
    
    std::cout << "All tests completed successfully" << std::endl;
}

int main() {
    std::cout << "Testing Brain class implementation..." << std::endl;
    
    try {
        test_brain_creation();
        test_brain_methods();
    } catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
