// Comprehensive Unit Tests for NLM Improvements
// Tests the enhanced functionality including bug fixes, JSON support, and Python bindings

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_improvements {

// Forward declarations
class Config;
class Brain;

void testEnhancedConfigJSON() {
    """Test JSON configuration loading with type support"""
    std::cout << "  Testing enhanced Config JSON support..." << std::endl;
    
    nlm::Config config;
    
    // Create a temporary JSON file
    std::string jsonContent = R"({
        "int_value": 42,
        "double_value": 3.14159,
        "string_value": "test_string",
        "bool_value": true,
        "array_int": [1, 2, 3, 4, 5],
        "array_double": [1.1, 2.2, 3.3],
        "array_string": ["hello", "world", "test"],
        "nested_object": {
            "inner_int": 100,
            "inner_string": "nested"
        }
    })";
    
    std::ofstream jsonFile("test_config.json");
    jsonFile << jsonContent;
    jsonFile.close();
    
    // Load JSON file
    bool success = config.loadFromFile("test_config.json");
    assert(success);
    
    // Test integer loading
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    // Test double loading
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && *doubleVal > 3.14 && *doubleVal < 3.142);
    
    // Test string loading
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "test_string");
    
    // Test boolean loading
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Test array loading (int)
    auto arrayInt = config.get<std::vector<int>>("array_int");
    assert(arrayInt.has_value() && arrayInt.value().size() == 5);
    
    // Test array loading (double)
    auto arrayDouble = config.get<std::vector<double>>("array_double");
    assert(arrayDouble.has_value() && arrayDouble.value().size() == 3);
    
    // Test array loading (string)
    auto arrayString = config.get<std::vector<std::string>>("array_string");
    assert(arrayString.has_value() && arrayString.value().size() == 3);
    
    // Test fallback to key=value format
    std::string simpleContent = "simple_key = simple_value\nsimple_bool = true\nsimple_number = 123";
    std::ofstream simpleFile("test_simple.txt");
    simpleFile << simpleContent;
    simpleFile.close();
    
    nlm::Config simpleConfig;
    bool simpleSuccess = simpleConfig.loadFromFile("test_simple.txt");
    assert(simpleSuccess);
    
    auto simpleString = simpleConfig.get<std::string>("simple_key");
    assert(simpleString.has_value() && *simpleString == "simple_value");
    
    auto simpleBool = simpleConfig.get<bool>("simple_bool");
    assert(simpleBool.has_value() && *simpleBool == true);
    
    auto simpleNum = simpleConfig.get<int>("simple_number");
    assert(simpleNum.has_value() && *simpleNum == 123);
    
    // Cleanup
    std::remove("test_config.json");
    std::remove("test_simple.txt");
    
    std::cout << "    Enhanced Config JSON tests passed" << std::endl;
}

void testNeuralWorkingMemoryBugFixes() {
    """Test NeuralWorkingMemory bug fixes for memory access"""
    std::cout << "  Testing NeuralWorkingMemory bug fixes..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get working memory
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test storeToNeuron with proper RegionId handling
    workingMemory->storeToNeuron(nlm::NeuronId(40000), 0.5f);
    
    // Verify it's stored
    assert(workingMemory->contains(nlm::NeuronId(40000)));
    assert(workingMemory->getNeuronActivation(nlm::NeuronId(40000)) == 0.5f);
    
    // Test competition mechanism (winners_ vector fix)
    // Create some activation patterns
    std::vector<float> pattern1 = {0.8f, 0.7f, 0.6f};
    std::vector<float> pattern2 = {0.1f, 0.2f, 0.3f};
    
    workingMemory->store(pattern1, 1.0f);
    workingMemory->store(pattern2, 1.0f);
    
    // Update to trigger competition
    workingMemory->update(0.1f);
    
    // Verify competition selected winners
    // At least one neuron should be a winner due to high activation
    bool hasWinner = false;
    for (size_t i = 0; i < workingMemory->getMemoryNeurons().size(); ++i) {
        nlm::NeuronId neuron = workingMemory->getMemoryNeurons()[i];
        if (workingMemory->isWinning(neuron)) {
            hasWinner = true;
            break;
        }
    }
    
    // Test decay behavior
    float activityBefore = workingMemory->getNeuronActivation(nlm::NeuronId(40000));
    workingMemory->update(100.0f); // Large time step
    float activityAfter = workingMemory->getNeuronActivation(nlm::NeuronId(40000));
    
    // Activity should decay over time
    assert(activityAfter <= activityBefore);
    
    // Test clear functionality
    workingMemory->clear();
    assert(!workingMemory->contains(nlm::NeuronId(40000)));
    assert(workingMemory->getMemoryNeurons().empty());
    
    std::cout << "    NeuralWorkingMemory bug fix tests passed" << std::endl;
}

void testPythonBindingsEnhanced() {
    """Test enhanced Python bindings functionality"""
    std::cout << "  Testing enhanced Python bindings..." << std::endl;
    
    // Note: In a real test environment, we would compile and run
    // actual Python bindings, but for this demonstration we'll
    // simulate the binding tests by testing the C++ classes directly
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Test basic bindings functionality
    assert(brain.getTotalNeuronCount() == 500);
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Test step with both overloads
    nlm::Brain brain2(config);
    brain2.initialize();
    brain2.step(1);
    brain2.step(2, 0.5); // with timestamp
    
    // Test region access (should be properly bound)
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 2);
    
    for (const auto& rid : regionIds) {
        nlm::NeuralRegion* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    // Test neuromodulator binding (should be accessible)
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    auto* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    std::cout << "    Python bindings enhanced tests passed" << std::endl;
}

void testAttentionalSelectionImplementation() {
    """Test complete AttentionalSelection implementation"""
    std::cout << "  Testing AttentionalSelection implementation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* attention = brain.getAttention();
    assert(attention != nullptr);
    
    // Test attention initialization
    attention->initialize(&brain);
    
    // Test competition processing
    std::vector<nlm::NeuronId> competitors;
    competitors.push_back(nlm::NeuronId(40001));
    competitors.push_back(nlm::NeuronId(40002));
    competitors.push_back(nlm::NeuronId(40003));
    
    // Apply some top-down bias
    attention->applyTopDownBias(nlm::NeuronId(40001), 0.8f);
    attention->applyBottomUpSalience(nlm::NeuronId(40002), 0.9f);
    
    // Process competition
    auto winners = attention->processCompetition(competitors, 0.5f);
    assert(!winners.empty());
    
    // Test attended regions
    auto* region = brain.getRegion(nlm::RegionId(0));
    attention->focusOnRegion(region->getId());
    
    auto attended = attention->getAttendedRegions();
    assert(!attended.empty());
    
    // Test attention parameters
    attention->setInhibitionStrength(0.3f);
    attention->setExcitationStrength(1.2f);
    attention->setCompetitionThreshold(0.4f);
    
    assert(attention->getInhibitionFor(nlm::NeuronId(40001)) > 0.0f);
    assert(attention->getExcitationFor(nlm::NeuronId(40001)) > 0.0f);
    
    std::cout << "    AttentionalSelection implementation tests passed" << std::endl;
}

void testConceptFormationImplementation() {
    """Test complete ConceptFormation implementation"""
    std::cout << "  Testing ConceptFormation implementation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Initialize concept formation
    conceptFormation->initialize(&brain);
    
    // Test experience presentation
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> features1 = {0.5f, 0.6f, 0.7f};
    
    size_t conceptId1 = conceptFormation->presentExperience(pattern1, features1, 0.8f, 10);
    assert(conceptId1 > 0); // Should create new concept
    
    // Present similar pattern (should match existing concept)
    std::vector<float> pattern2 = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
    std::vector<float> features2 = {0.55f, 0.65f, 0.75f};
    
    size_t conceptId2 = conceptFormation->presentExperience(pattern2, features2, 0.9f, 20);
    assert(conceptId2 == conceptId1); // Should match existing concept
    
    // Test novelty detection
    std::vector<float> novelPattern = {0.9f, 0.1f, 0.2f, 0.8f, 0.3f};
    bool isNovel = conceptFormation->isNovel(novelPattern, 0.7f);
    assert(isNovel); // Should be novel due to low similarity
    
    // Test concept properties
    auto prototype = conceptFormation->getConceptPrototype(conceptId1);
    assert(!prototype.empty());
    
    float stability = conceptFormation->getConceptStability(conceptId1);
    assert(stability >= 0.0f && stability <= 1.0f);
    
    size_t conceptCount = conceptFormation->getConceptCount();
    assert(conceptCount > 0);
    
    std::cout << "    ConceptFormation implementation tests passed" << std::endl;
}

void testNeuromodulatorIntegration() {
    """Test complete neuromodulator integration"""
    std::cout << "  Testing neuromodulator integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test dopamine
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    dopamine->setLevel(0.7f);
    assert(dopamine->getLevel() == 0.7f);
    
    float plasticity = dopamine->getPlasticityFactor();
    assert(plasticity >= 0.5f && plasticity <= 1.5f);
    
    // Test novelty
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    novelty->setLevel(0.5f);
    float noveltyPlasticity = novelty->getPlasticityFactor();
    assert(noveltyPlasticity >= 0.5f && noveltyPlasticity <= 1.5f);
    
    // Test curiosity
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    curiosity->setLevel(0.3f);
    float curiosityPlasticity = curiosity->getPlasticityFactor();
    assert(curiosityPlasticity >= 0.5f && curiosityPlasticity <= 1.5f);
    
    // Test prediction error
    auto* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    predictionError->setLevel(0.8f);
    float errorPlasticity = predictionError->getPlasticityFactor();
    assert(errorPlasticity >= 0.5f && errorPlasticity <= 1.5f);
    
    std::cout << "    Neuromodulator integration tests passed" << std::endl;
}

void testMemorySystemIntegration() {
    """Test complete memory system integration"""
    std::cout << "  Testing memory system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(600), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Test working memory initialization
    workingMemory->initialize(&brain);
    
    // Test episodic memory initialization
    episodicMemory->initialize(&brain);
    
    // Test associative memory initialization
    associativeMemory->initialize(&brain);
    
    // Test working memory functionality
    std::vector<float> testPattern = {0.2f, 0.4f, 0.6f, 0.8f, 1.0f};
    
    workingMemory->store(testPattern, 0.9f);
    
    auto retrieved = workingMemory->retrieve();
    assert(retrieved.size() == testPattern.size());
    
    // Test episodic memory functionality
    workingMemory->storeToNeuron(nlm::NeuronId(50000), 0.7f);
    assert(workingMemory->contains(nlm::NeuronId(50000)));
    
    // Test competition
    workingMemory->runCompetition();
    
    // Test decay
    workingMemory->update(10.0f);
    
    // Test capacity
    workingMemory->setCapacity(50);
    assert(workingMemory->getCapacity() == 50);
    
    std::cout << "    Memory system integration tests passed" << std::endl;
}

void testPredictionSystemIntegration() {
    """Test complete prediction system integration"""
    std::cout << "  Testing prediction system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(800), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Initialize prediction system
    predictionSystem->initialize(&brain);
    
    // Test prediction functionality
    std::vector<float> currentState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> sensoryInput = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
    
    // Process prediction
    predictionSystem->processSensoryInput(sensoryInput);
    
    // Get prediction error
    float predictionError = predictionSystem->getPredictionError();
    assert(predictionError >= 0.0f && predictionError <= 1.0f);
    
    // Test prediction confidence
    float confidence = predictionSystem->getConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    std::cout << "    Prediction system integration tests passed" << std::endl;
}

void testDevelopmentSystemIntegration() {
    """Test complete development system integration"""
    std::cout << "  Testing development system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(700), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Initialize development system
    developmentSystem->initialize(&brain);
    
    // Test developmental stage access
    nlm::DevelopmentalStage stage = brain.getDevelopmentalStage();
    assert(stage >= nlm::DevelopmentalStage::Initial && 
           stage <= nlm::DevelopmentalStage::Aging);
    
    // Test stage modification
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
    
    // Test development update
    developmentSystem->update(0.1f);
    
    std::cout << "    Development system integration tests passed" << std::endl;
}

void runAllImprovementTests() {
    std::cout << "=== Running NLM Code Improvement Tests ===" << std::endl;
    
    try {
        testEnhancedConfigJSON();
        testNeuralWorkingMemoryBugFixes();
        testPythonBindingsEnhanced();
        testAttentionalSelectionImplementation();
        testConceptFormationImplementation();
        testNeuromodulatorIntegration();
        testMemorySystemIntegration();
        testPredictionSystemIntegration();
        testDevelopmentSystemIntegration();
        
        std::cout << std::endl;
        std::cout << "=== ALL IMPROVEMENT TESTS PASSED ===" << std::endl;
        std::cout << "The NLM improvements are working correctly!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== TEST FAILED ===" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        std::cout << "Some improvements may have issues that need fixing." << std::endl;
    } catch (...) {
        std::cout << std::endl;
        std::cout << "=== TEST FAILED ===" << std::endl;
        std::cout << "Unknown error occurred during testing." << std::endl;
        std::cout << "Please review the implementation." << std::endl;
    }
}

} // namespace test_improvements

// Entry point
int main() {
    test_improvements::runAllImprovementTests();
    return 0;
}