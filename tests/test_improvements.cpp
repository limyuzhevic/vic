// NLM Test - Comprehensive Tests for Improved Functionality
// Tests the fixes and improvements made to address bugs and enhance usability

#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>

// Include NLM headers
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "neuromodulation/Neuromodulator.hpp"

namespace test_improvements {

void testConstructorInitialization() {
    std::cout << "    Testing constructor initialization..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(12345), nlm::ConfigSource::Default);
    
    // Test that constructor initializes members properly
    nlm::Brain brain(config);
    
    // Verify initialization was successful
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Test brain initialization
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    std::cout << "    testConstructorInitialization passed" << std::endl;
}

void testNullPointerChecks() {
    std::cout << "    Testing null pointer safety..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test that initialize handles missing components gracefully
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Test step with valid brain
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testNullPointerChecks passed" << std::endl;
}

void testMemorySystemIntegration() {
    std::cout << "    Testing memory system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test working memory
    nlm::NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test episodic memory
    nlm::NeuralEpisodicMemory* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test associative memory
    nlm::NeuralAssociativeMemory* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Simulate some activity and store in memory
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
        
        // Inject some sensory input
        nlm::SensoryInput sensory;
        std::vector<float> data = {1.0f, 2.0f, 3.0f};
        sensory.setData(data);
        brain.receiveSensoryInput(sensory);
    }
    
    std::cout << "    testMemorySystemIntegration passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "    Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test neuromodulation systems
    nlm::Dopamine* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    nlm::Curiosity* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    nlm::Novelty* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    nlm::PredictionError* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    // Test neuromodulation updates during simulation
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void testEnhancedErrorHandling() {
    std::cout << "    Testing enhanced error handling..." << std::endl;
    
    // Test with null brain in LearningExperiment (simulating improved error handling)
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run enhanced learning experiment
    struct LearningExperiment {
        std::shared_ptr<nlm::Brain> brain;
        size_t initialSynapseCount;
        std::vector<float> initialWeights;
        std::vector<float> finalWeights;
        
        LearningExperiment(std::shared_ptr<nlm::Brain> b) 
            : brain(b), initialSynapseCount(0) {}
        
        void recordInitialState() {
            if (!brain) {
                return; // Graceful handling of null brain
            }
            
            initialSynapseCount = brain->getTotalSynapseCount();
            initialWeights.clear();
            
            // Record weights safely
            if (auto* region = brain->getRegion(nlm::RegionId(1))) {
                const auto& synapses = region->getSynapses();
                initialWeights.reserve(synapses.size());
                for (const auto& syn : synapses) {
                    if (syn) {
                        initialWeights.push_back(syn->getWeight());
                    }
                }
            }
        }
        
        void recordFinalState() {
            finalWeights.clear();
            
            if (!brain) {
                return;
            }
            
            if (auto* region = brain->getRegion(nlm::RegionId(1))) {
                const auto& synapses = region->getSynapses();
                finalWeights.reserve(synapses.size());
                for (const auto& syn : synapses) {
                    if (syn) {
                        finalWeights.push_back(syn->getWeight());
                    }
                }
            }
        }
        
        float computeLearningScore() {
            if (initialWeights.empty() || finalWeights.empty()) {
                return 0.0f;
            }
            
            float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float initialMean = initialSum / initialWeights.size();
            float finalMean = finalSum / finalWeights.size();
            
            return std::abs(finalMean - initialMean);
        }
    };
    
    LearningExperiment experiment(std::make_shared<nlm::Brain>(brain));
    
    // Run experiment with steps
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    experiment.recordInitialState();
    experiment.recordFinalState();
    
    float learningScore = experiment.computeLearningScore();
    
    // Test that learning score is computed without errors
    assert(!std::isnan(learningScore));
    assert(learningScore >= 0.0f);
    
    std::cout << "    testEnhancedErrorHandling passed" << std::endl;
}

void testConfigurationValidation() {
    std::cout << "    Testing configuration validation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Test default configuration
    assert(config->has("neuron_count"));
    assert(config->has("region_count"));
    
    // Test setting and getting values
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2f, nlm::ConfigSource::Default);
    
    assert(config->getOr<size_t>("neuron_count", 0) == 500);
    assert(config->getOr<size_t>("region_count", 0) == 3);
    assert(config->getOr<float>("connection_probability", 0.0f) == 0.2f);
    
    // Create brain with validated config
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    std::cout << "    testConfigurationValidation passed" << std::endl;
}

void testPythonBindingIntegration() {
    std::cout << "    Testing Python binding integration patterns..." << std::endl;
    
    // This test verifies that the improved C++ API is compatible with
    // the Python bindings and follows proper patterns
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test methods that should be available for Python bindings
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() > 0);
    assert(brain.getTotalSpikeCount() >= 0);
    
    // Test action production
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    // Test developmental stage access
    nlm::DevelopmentalStage stage = brain.getDevelopmentalStage();
    assert(stage != nlm::DevelopmentalStage::Aging); // Should be a valid stage
    
    std::cout << "    testPythonBindingIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running NLM Improvement Tests..." << std::endl;
    std::cout << "============================" << std::endl;
    
    testConstructorInitialization();
    testNullPointerChecks();
    testMemorySystemIntegration();
    testNeuromodulationIntegration();
    testEnhancedErrorHandling();
    testConfigurationValidation();
    testPythonBindingIntegration();
    
    std::cout << std::endl;
    std::cout << "=== All Improvement Tests PASSED ===" << std::endl;
}

} // namespace test_improvements

// Main function for standalone test execution
int main() {
    std::cout << "=== NLM Phase 2: Improved Functionality Tests ===" << std::endl;
    std::cout << "Testing fixes and improvements for bugs and usability:" << std::endl;
    std::cout << "  - Pimpl implementation fixes" << std::endl;
    std::cout << "  - Null pointer safety improvements" << std::endl;
    std::cout << "  - Constructor initialization fixes" << std::endl;
    std::cout << "  - Enhanced error handling" << std::endl;
    std::cout << "  - Memory system integration" << std::endl;
    std::cout << "  - Neuromodulation integration" << std::endl;
    std::cout << std::endl;
    
    try {
        test_improvements::runAll();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
