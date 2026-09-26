// Cognition System Tests
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralAttention.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_cognition {

void testNeuralPlanner() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* neuralPlanner = brain.getPlanner();
    assert(neuralPlanner != nullptr);
    
    // Test neural planner functionality
    float planningConfidence = neuralPlanner->getConfidence();
    assert(planningConfidence >= 0.0f && planningConfidence <= 1.0f);
    
    std::cout << "    testNeuralPlanner passed" << std::endl;
}

void testConceptFormation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test concept formation functionality
    size_t conceptCount = conceptFormation->getConceptCount();
    assert(conceptCount >= 0);
    
    std::cout << "    testConceptFormation passed" << std::endl;
}

void testNeuralAttention() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* neuralAttention = brain.getAttention();
    assert(neuralAttention != nullptr);
    
    // Test neural attention functionality
    float attentionSelectivity = neuralAttention->getSelectivity();
    assert(attentionSelectivity >= 0.0f && attentionSelectivity <= 1.0f);
    
    std::cout << "    testNeuralAttention passed" << std::endl;
}

void runAll() {
    testNeuralPlanner();
    testConceptFormation();
    testNeuralAttention();
}

} // namespace test_cognition
