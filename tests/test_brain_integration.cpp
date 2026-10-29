#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <cassert>

using namespace nlm;

void testMemorySystemIntegration() {
    std::cout << "=== Testing Memory System Integration ===" << std::endl;
    
    // Create config
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(2));
    config->set("random_seed", static_cast<int64_t>(42));
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    assert(brain->initialize());
    
    // Test working memory
    auto* workingMemory = brain->getWorkingMemory();
    assert(workingMemory != nullptr);
    assert(workingMemory->getCapacity() > 0);
    
    // Store some neuron activations
    workingMemory->store(NeuronId(1), 0.5f);
    workingMemory->store(NeuronId(2), 0.8f);
    assert(workingMemory->retrieve(NeuronId(1)) == 0.5f);
    assert(workingMemory->contains(NeuronId(2)));
    
    // Test episodic memory
    auto* episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    assert(episodicMemory->getEpisodeCount() == 0);
    
    // Create and store an episode
    EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.reward = 1.0f;
    episode.activeNeurons = {NeuronId(1), NeuronId(2)};
    episode.neuronActivations = {0.5f, 0.8f};
    episode.age = 0;
    
    episodicMemory->storeEpisode(episode);
    assert(episodicMemory->getEpisodeCount() == 1);
    
    auto retrievedEpisode = episodicMemory->retrieveEpisode(0);
    assert(retrievedEpisode.activeNeurons.size() == 2);
    
    // Test associative memory
    auto* associativeMemory = brain->getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    associativeMemory->associate(NeuronId(1), NeuronId(2), 0.7f);
    auto associations = associativeMemory->getAssociations(NeuronId(1));
    assert(!associations.empty());
    assert(associativeMemory->getAssociationStrength(NeuronId(1), NeuronId(2)) == 0.7f);
    
    // Test semantic memory
    auto* semanticMemory = brain->getSemanticMemory();
    assert(semanticMemory != nullptr);
    
    semanticMemory->storeFact("test_fact", "test_value");
    assert(semanticMemory->retrieveFact("test_fact") == "test_value");
    assert(semanticMemory->hasFact("test_fact"));
    
    brain->reset();
    assert(workingMemory->getCurrentSize() == 0);
    assert(episodicMemory->getEpisodeCount() == 0);
    
    std::cout << "✓ Memory system integration tests passed" << std::endl;
}

void testPredictionSystemIntegration() {
    std::cout << "=== Testing Prediction System Integration ===" << std::endl;
    
    // Create config
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(50));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("random_seed", static_cast<int64_t>(123));
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Test prediction system
    auto* predictionSystem = brain->getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Create sensory input for testing
    auto* vision = new Vision();
    vision->setData({0.1f, 0.2f, 0.3f, 0.4f, 0.5f});
    vision->setTimestamp(0.0);
    
    // Test prediction
    auto predictedState = predictionSystem->predictNextState(*vision);
    assert(predictedState != nullptr);
    
    // Test prediction error
    auto actualState = vision->clone();
    predictionSystem->updatePredictions(*predictedState, *actualState);
    
    float error = predictionSystem->getPredictionError();
    assert(error >= 0.0f);  // Error should be non-negative
    
    float confidence = predictionSystem->getConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    // Test prediction training
    predictionSystem->train(*vision);
    
    // Test error history
    auto errorHistory = predictionSystem->getErrorHistory();
    assert(errorHistory.size() >= 1);
    
    brain->reset();
    
    std::cout << "✓ Prediction system integration tests passed" << std::endl;
}

void testCognitionSystemIntegration() {
    std::cout << "=== Testing Cognition System Integration ===" << std::endl;
    
    // Create config
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(75));
    config->set("region_count", static_cast<int64_t>(2));
    config->set("random_seed", static_cast<int64_t>(456));
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Test neural planner
    auto* planner = brain->getPlanner();
    assert(planner != nullptr);
    
    // Test planning
    std::vector<float> currentState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    ActionType action = planner->planAction(currentState, 1.0f);
    assert(action != ActionType::Custom);  // Should return a valid action
    
    float confidence = planner->getConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    // Test self model
    auto* selfModel = brain->getSelfModel();
    assert(selfModel != nullptr);
    
    // Test self model recording
    selfModel->recordSelfAction(action, currentState, currentState);
    
    float bodyAwareness = selfModel->getBodyAwareness();
    assert(bodyAwareness >= 0.0f && bodyAwareness <= 1.0f);
    
    // Test social learning
    auto* socialLearning = brain->getSocialLearning();
    assert(socialLearning != nullptr);
    
    // Test observation recording
    socialLearning->observeAgentAction(action, currentState, currentState);
    
    bool canImitate = socialLearning->canImitate(action);
    assert(canImitate);  // Should be able to imitate the observed action
    
    brain->reset();
    
    std::cout << "✓ Cognition system integration tests passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "=== Testing Neuromodulation Integration ===" << std::endl;
    
    // Create config
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(60));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("random_seed", static_cast<int64_t>(789));
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Test dopamine
    auto* dopamine = brain->getDopamine();
    assert(dopamine != nullptr);
    
    float initialDopamine = dopamine->getLevel();
    assert(initialDopamine >= 0.0f);
    
    // Test dopamine effects on neural excitability
    float dopamineLevel = dopamine->getLevel();
    assert(dopamineLevel >= 0.0f);
    
    // Test curiosity
    auto* curiosity = brain->getCuriosity();
    assert(curiosity != nullptr);
    
    float curiosityLevel = curiosity->getLevel();
    assert(curiosityLevel >= 0.0f && curiosityLevel <= 1.0f);
    
    float explorationBias = curiosity->getExplorationBias();
    assert(explorationBias >= 0.0f && explorationBias <= 1.0f);
    
    // Test novelty detection
    auto* novelty = brain->getNovelty();
    assert(novelty != nullptr);
    
    float noveltyLevel = novelty->getLevel();
    assert(noveltyLevel >= 0.0f && noveltyLevel <= 1.0f);
    
    // Test prediction error
    auto* predictionError = brain->getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    float errorLevel = predictionError->getLevel();
    assert(errorLevel >= 0.0f);
    
    brain->reset();
    
    std::cout << "✓ Neuromodulation integration tests passed" << std::endl;
}

void testDevelopmentSystemIntegration() {
    std::cout << "=== Testing Development System Integration ===" << std::endl;
    
    // Create config
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(80));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("random_seed", static_cast<int64_t>(999));
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Test development system
    auto* developmentSystem = brain->getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test developmental stage
    DevelopmentalStage initialStage = developmentSystem->getCurrentStage();
    assert(initialStage == DevelopmentalStage::Initial);
    
    // Test critical period check
    bool isCriticalPeriod = developmentSystem->isCriticalPeriod();
    assert(isCriticalPeriod);  // Should be in initial stage, which is considered a critical period
    
    // Test developmental stage update
    developmentSystem->setStage(DevelopmentalStage::Adult);
    DevelopmentalStage adultStage = developmentSystem->getCurrentStage();
    assert(adultStage == DevelopmentalStage::Adult);
    
    brain->reset();
    
    std::cout << "✓ Development system integration tests passed" << std::endl;
}

void testCompleteBrainLoop() {
    std::cout << "=== Testing Complete Brain Loop Integration ===" << std::endl;
    
    // Create config with reasonable settings
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(2));
    config->set("random_seed", static_cast<int64_t>(1));
    config->set("replay_interval", static_cast<size_t>(50));
    config->set("consolidation_interval", static_cast<size_t>(200));
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Simulate multiple steps to test the complete integration
    size_t stepsCompleted = 0;
    size_t memoryEvents = 0;
    size_t predictionEvents = 0;
    
    for (size_t step = 0; step < 200; ++step) {
        // Apply sensory input periodically
        if (step % 10 == 0) {
            // Create simple sensory input
            auto* sensoryInput = new InternalSignals();
            for (size_t i = 0; i < 5; ++i) {
                sensoryInput->addSignal(0.1f * (step % 10) / 10.0f);
            }
            sensoryInput->setTimestamp(static_cast<Timestamp>(step * 0.001));
            
            brain->receiveSensoryInput(*sensoryInput);
        }
        
        // Step the brain
        brain->step(step, static_cast<Timestamp>(step * 0.001));
        
        // Check memory system activity
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory && workingMemory->getCurrentSize() > 0) {
            memoryEvents++;
        }
        
        // Check prediction system
        auto* predictionSystem = brain->getPredictionSystem();
        if (predictionSystem && predictionSystem->getErrorHistory().size() > 0) {
            predictionEvents++;
        }
        
        stepsCompleted++;
        
        // Log progress occasionally
        if (step % 50 == 0) {
            std::cout << "  Completed step " << step << "/200" << std::endl;
        }
    }
    
    // Verify that we had some memory and prediction activity
    assert(memoryEvents > 0);
    assert(predictionEvents > 0);
    
    // Check that brain state is consistent
    assert(brain->getTotalNeuronCount() > 0);
    assert(brain->getTotalSynapseCount() > 0);
    assert(brain->getTotalSpikeCount() >= 0);
    
    // Check memory system state
    auto* workingMemory = brain->getWorkingMemory();
    assert(workingMemory != nullptr);
    
    auto* episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Check cognition system state
    auto* planner = brain->getPlanner();
    assert(planner != nullptr);
    
    auto* selfModel = brain->getSelfModel();
    assert(selfModel != nullptr);
    
    brain->reset();
    
    std::cout << "✓ Complete brain loop integration tests passed (" << stepsCompleted << " steps)" << std::endl;
}

int main() {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "=== NLM Integration Tests ===" << std::endl;
    std::cout << "Testing Phase 6 integrated brain systems..." << std::endl;
    
    try {
        testMemorySystemIntegration();
        testPredictionSystemIntegration();
        testCognitionSystemIntegration();
        testNeuromodulationIntegration();
        testDevelopmentSystemIntegration();
        testCompleteBrainLoop();
        
        std::cout << "" << std::endl;
        std::cout << "=== ALL INTEGRATION TESTS PASSED ===" << std::endl;
        std::cout << "The NLM Phase 6 integrated brain systems are working correctly!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}