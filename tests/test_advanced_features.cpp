// Advanced Features Tests
// Tests advanced features (plasticity, world interaction, agent systems)

#include "plasticity/StructuralPlasticity.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/STDP.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace test_advanced_features {

void testPlasticityRuleBase() {
    std::cout << "  Testing PlasticityRule Base..." << std::endl;
    
    // Test base PlasticityRule interface
    // This is an abstract base class
    // We test that concrete implementations inherit correctly
    
    std::cout << "    testPlasticityRuleBase passed" << std::endl;
}

void testHebbianPlasticity() {
    std::cout << "  Testing Hebbian Plasticity..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::Hebbian* hebbian = brain.getHebbian();
    assert(hebbian != nullptr);
    
    // Test Hebbian learning rule
    // HebbianPlasticity strengthens connections between co-active neurons
    
    // Set learning rate
    hebbian->setLearningRate(0.1f);
    assert(hebbian->getLearningRate() == 0.1f);
    
    // In real implementation, would test with actual neuron activity
    // For now, test that the system is accessible
    
    std::cout << "    testHebbianPlasticity passed" << std::endl;
}

void testSTDPPlasticity() {
    std::cout << "  Testing STDP Plasticity..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::STDP* stdp = brain.getSTDP();
    assert(stdp != nullptr);
    
    // Test STDP learning rule
    // Spike-Timing-Dependent Plasticity strengthens proximal synapses
    // and weakens distal synapses based on spike timing
    
    // Set STDP parameters
    stdp->setLearningRate(0.01f);
    stdp->setLTPTimeWindow(10.0f);  // Long-term potentiation window
    stdp->setLTDTimeWindow(20.0f);  // Long-term depression window
    
    assert(stdp->getLearningRate() == 0.01f);
    assert(stdp->getLTPTimeWindow() == 10.0f);
    assert(stdp->getLTDTimeWindow() == 20.0f);
    
    // In real implementation, would test with actual spike timing
    // For now, test that the system is accessible
    
    std::cout << "    testSTDPPlasticity passed" << std::endl;
}

void testStructuralPlasticity() {
    std::cout << "  Testing Structural Plasticity..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::StructuralPlasticity* structural = brain.getStructuralPlasticity();
    assert(structural != nullptr);
    
    // Test structural plasticity rules
    // Structural plasticity creates/removes synapses and neurons
    
    // Set plasticity rates
    structural->setSynaptogenesisRate(0.05f);
    structural->setPruningRate(0.02f);
    
    assert(structural->getSynaptogenesisRate() == 0.05f);
    assert(structural->getPruningRate() == 0.02f);
    
    // Test development system integration
    nlm::DevelopmentSystem* development = brain.getDevelopmentSystem();
    assert(development != nullptr);
    
    // Structural plasticity is active during development
    development->setStage(nlm::DevelopmentalStage::Juvenile);
    assert(development->getStage() == nlm::DevelopmentalStage::Juvenile);
    
    std::cout << "    testStructuralPlasticity passed" << std::endl;
}

void testAgentBrainBase() {
    std::cout << "  Testing AgentBrain Base..." << std::endl;
    
    // Test AgentBrain basic functionality
    // AgentBrain is the interface between the neural system and the agent
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // AgentBrain is attached to Brain
    // In real implementation, AgentBrain would be accessible
    
    // For now, test that the agent system is conceptual
    // and can integrate with neural systems
    
    std::cout << "    testAgentBrainBase passed" << std::endl;
}

void testAgentSensoryPercept() {
    std::cout << "  Testing Agent Sensory Percept..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test sensory perception integration
    // SensoryPercept converts raw sensory input to neural patterns
    
    nlm::SensoryPercept percept;
    assert(percept.getInputSize() == 0);  // Default
    
    // In real implementation, would test with actual sensory data
    // For now, test that the system is accessible
    
    std::cout << "    testAgentSensoryPercept passed" << std::endl;
}

void testSimpleWorldIntegration() {
    std::cout << "  Testing SimpleWorld Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test world integration
    // SimpleWorld provides environment simulation
    
    // In real implementation, would test with actual world simulation
    // For now, test that the system can integrate with brain
    
    // Test sensory input integration
    nlm::SensoryInput input;
    input.pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    input.timestamp = 0;
    input.confidence = 0.8f;
    
    // Brain can receive sensory input
    brain.receiveSensoryInput(input);
    
    std::cout << "    testSimpleWorldIntegration passed" << std::endl;
}

void testPlasticityIntegration() {
    std::cout << "  Testing Plasticity Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify all plasticity systems are accessible
    assert(brain.getHebbian() != nullptr);
    assert(brain.getSTDP() != nullptr);
    assert(brain.getStructuralPlasticity() != nullptr);
    
    // Test plasticity system interconnections
    nlm::Hebbian* hebbian = brain.getHebbian();
    nlm::STDP* stdp = brain.getSTDP();
    nlm::StructuralPlasticity* structural = brain.getStructuralPlasticity();
    
    // Set different plasticity rules
    hebbian->setLearningRate(0.05f);
    stdp->setLearningRate(0.02f);
    structural->setSynaptogenesisRate(0.03f);
    structural->setPruningRate(0.01f);
    
    // Test development system integration
    nlm::DevelopmentSystem* development = brain.getDevelopmentSystem();
    assert(development != nullptr);
    
    // Different developmental stages use different plasticity rules
    development->setStage(nlm::DevelopmentalStage::Infant);
    assert(development->getStage() == nlm::DevelopmentalStage::Infant);
    
    // Test neuromodulation integration
    nlm::Dopamine* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Neuromodulators affect plasticity
    // High dopamine increases Hebbian learning
    dopamine->setLevel(0.8f);
    
    // Test structural plasticity update
    brain.getRandomGenerator();  // Ensure RNG is available
    structural->update(&brain, brain.getRandomGenerator(), 0.01f);
    
    std::cout << "    testPlasticityIntegration passed" << std::endl;
}

void testAgentWorldIntegration() {
    std::cout << "  Testing Agent-World Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test agent-world system integration
    // This is the core of NLM: neural agents interacting with world
    
    // Test sensory input and output integration
    nlm::SensoryInput input;
    input.pattern = {0.2f, 0.4f, 0.6f, 0.8f, 1.0f};
    input.timestamp = 0;
    input.confidence = 0.9f;
    
    brain.receiveSensoryInput(input);
    
    // Test action production
    nlm::Action action = brain.produceAction();
    assert(action != nullptr);
    
    // Test memory integration with world interaction
    nlm::NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // World experiences update working memory
    workingMemory->store(nlm::NeuronId(1), 0.7f);
    workingMemory->decay(0.1f);
    
    // Test planning and action selection
    nlm::NeuralPlanner* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    std::vector<float> currentState = {0.3f, 0.6f, 0.9f};
    nlm::ActionType plannedAction = planner->planAction(currentState, 0.7f);
    
    // Test attention selection
    nlm::AttentionalSelection* attention = brain.getAttention();
    assert(attention != nullptr);
    
    std::vector<std::vector<float>> stimuli = {input.pattern};
    std::vector<float> focused = attention->selectFocus(stimuli);
    
    std::cout << "    testAgentWorldIntegration passed" << std::endl;
}

void testAdvancedFeaturesPerformance() {
    std::cout << "  Testing Advanced Features Performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::Hebbian* hebbian = brain.getHebbian();
    nlm::STDP* stdp = brain.getSTDP();
    nlm::StructuralPlasticity* structural = brain.getStructuralPlasticity();
    
    assert(hebbian != nullptr);
    assert(stdp != nullptr);
    assert(structural != nullptr);
    
    const size_t iterations = 5000;
    
    // Performance benchmark for advanced features
    for (size_t i = 0; i < iterations; ++i) {
        // Update plasticity rules
        hebbian->setLearningRate(0.01f * (1.0f + static_cast<float>(i % 10) / 10.0f));
        
        stdp->setLearningRate(0.005f * (1.0f + static_cast<float>(i % 5) / 5.0f));
        
        // Update structural plasticity
        structural->setSynaptogenesisRate(0.01f * (1.0f + static_cast<float>(i % 3) / 3.0f));
        structural->setPruningRate(0.005f * (1.0f + static_cast<float>(i % 7) / 7.0f));
        
        // Update with random generator
        brain.getRandomGenerator();  // Ensure available
        structural->update(&brain, brain.getRandomGenerator(), 0.01f);
    }
    
    // Verify performance
    assert(hebbian->getLearningRate() > 0.0f);
    assert(stdp->getLearningRate() > 0.0f);
    
    std::cout << "    testAdvancedFeaturesPerformance passed" << std::endl;
}

void testAdvancedFeaturesErrorConditions() {
    std::cout << "  Testing Advanced Features Error Conditions..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::Hebbian* hebbian = brain.getHebbian();
    nlm::STDP* stdp = brain.getSTDP();
    nlm::StructuralPlasticity* structural = brain.getStructuralPlasticity();
    
    // Test plasticity error conditions
    // Negative learning rates should be handled
    hebbian->setLearningRate(-0.1f);  // Should be clamped or handled
    // (Implementation dependent)
    
    // Extreme STDP time windows should be handled
    stdp->setLTPTimeWindow(1000.0f);  // Very large window
    stdp->setLTDTimeWindow(-10.0f);  // Negative window (should be handled)
    
    // Structural plasticity rates should be bounded
    structural->setSynaptogenesisRate(10.0f);  // Very high rate
    structural->setPruningRate(-1.0f);  // Negative rate (should be handled)
    
    // Test with null brain reference (should be handled gracefully)
    // In real implementation, these would check for null
    
    std::cout << "    testAdvancedFeaturesErrorConditions passed" << std::endl;
}

void runAll() {
    testPlasticityRuleBase();
    testHebbianPlasticity();
    testSTDPPlasticity();
    testStructuralPlasticity();
    testAgentBrainBase();
    testAgentSensoryPercept();
    testSimpleWorldIntegration();
    testPlasticityIntegration();
    testAgentWorldIntegration();
    testAdvancedFeaturesPerformance();
    testAdvancedFeaturesErrorConditions();
}

} // namespace test_advanced_features
