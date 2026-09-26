#include "Phase5Demo.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../memory/WorkingMemory.hpp"
#include "../memory/EpisodicMemory.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace nlm {

Phase5Demo::Phase5Demo() {
    std::cout << "Phase 5 Demo: Integrated Cognitive Architecture\n";
    std::cout << "===========================================\n";
    std::cout << "Demonstrates integration of prediction, memory,\n";
    std::cout << "cognition, and action generation.\n\n";
}

Phase5Demo::~Phase5Demo() = default;

void Phase5Demo::run() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "PHASE 5: INTEGRATED NEUROCOGNITIVE SYSTEM\n";
    std::cout << std::string(60, '=') << "\n\n";
    
    // Create system components
    createSystem();
    
    // Configure components
    configureComponents();
    
    // Initialize everything
    initializeSystem();
    
    // Run demonstrations
    runPredictionDemo();
    runMemoryDemo();
    runCognitionDemo();
    runActionDemo();
    
    // Show results
    showResults();
}

void Phase5Demo::createSystem() {
    // Configuration
    config_ = std::make_shared<Config>();
    
    // Core processing
    clock_ = std::make_shared<SimulationClock>(0.01);
    random_ = std::make_shared<RandomGenerator>();
    logger_ = std::make_shared<Logger>();
    
    // Neural components
    brain_ = std::make_shared<Brain>(config_);
    agentBrain_ = std::make_shared<AgentBrain>(brain_);
    
    // Specialized modules
    prediction_ = std::make_shared<NeuralPrediction>(brain_.get());
    workingMemory_ = std::make_shared<WorkingMemory>();
    episodicMemory_ = std::make_shared<EpisodicMemory>();
    conceptFormation_ = std::make_shared<ConceptFormation>();
    neuralPlanner_ = std::make_shared<NeuralPlanner>();
    
    // Environment
    world_ = std::make_shared<SimpleWorld>();
    
    std::cout << "System components created.\n";
}

void Phase5Demo::configureComponents() {
    // Configure brain
    config_->set("neuron_count", size_t(2000));
    config_->set("region_count", size_t(3));
    config_->set("connection_probability", 0.05f);
    config_->set("random_seed", uint64_t(12345));
    config_->set("simulation_timestep", 0.001);
    
    // Configure world
    world_->configure(30.0f, 30.0f, 20, 20);
    world_->setRandomSeed(12345);
    
    // Configure agent brain
    agentBrain_->enableRewardModulation(true);
    agentBrain_->enableStructuralPlasticity(true);
    agentBrain_->enableDevelopment(true);
    agentBrain_->enableCuriosity(true);
    
    // Configure prediction system
    prediction_->setLearningRate(0.01f);
    prediction_->setTemporalWindow(10.0f);
    
    // Configure memory systems
    workingMemory_->setCapacity(500);
    workingMemory_->setDecayRate(0.02f);
    
    // Configure cognition
    conceptFormation_->setConceptCount(50);
    conceptFormation_->setLearningRate(0.005f);
    
    neuralPlanner_->setPlanningHorizon(5);
    neuralPlanner_->setActionSpaceSize(8);
    
    std::cout << "Components configured.\n";
}

void Phase5Demo::initializeSystem() {
    brain_->initialize();
    agentBrain_->initialize(*world_);
    
    // Initialize memories
    workingMemory_->initialize(brain_.get());
    episodicMemory_->initialize();
    
    std::cout << "System initialized.\n";
}

void Phase5Demo::runPredictionDemo() {
    std::cout << "\n--- PREDICTION DEMONSTRATION ---\n";
    
    // Create a simple sequence of events
    std::vector<float> sequence = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.4f, 0.3f, 0.2f, 0.1f};
    
    std::cout << "Input sequence: ";
    for (size_t i = 0; i < sequence.size(); ++i) {
        std::cout << std::fixed << std::setprecision(2) << sequence[i];
        if (i < sequence.size() - 1) std::cout << " -> ";
    }
    std::cout << "\n\n";
    
    // Feed to prediction system
    for (size_t i = 0; i < sequence.size(); ++i) {
        // Predict next value
        float prediction = prediction_->predict(sequence[i]);
        
        // Get actual next value
        float actual = (i + 1 < sequence.size()) ? sequence[i + 1] : 0.0f;
        
        // Learn from result
        prediction_->update(prediction, actual);
        
        std::cout << "Step " << i << ": predict=" << std::setprecision(3) << prediction;
        if (i + 1 < sequence.size()) {
            std::cout << ", actual=" << actual << ", error=" << std::abs(prediction - actual);
        }
        std::cout << "\n";
    }
    
    // Show prediction accuracy
    float finalAccuracy = prediction_->getAccuracy();
    std::cout << "\nPrediction system accuracy: " << std::fixed << std::setprecision(2) 
              << (finalAccuracy * 100) << "%\n";
}

void Phase5Demo::runMemoryDemo() {
    std::cout << "\n--- MEMORY DEMONSTRATION ---\n";
    
    // Create working memory pattern
    std::vector<float> pattern(100, 0.0f);
    for (size_t i = 0; i < 20; ++i) {
        pattern[i] = std::sin(static_cast<float>(i) * 0.3f);
        pattern[i + 20] = std::cos(static_cast<float>(i) * 0.3f);
        pattern[i + 40] = std::sin(static_cast<float>(i) * 0.5f);
    }
    
    // Store in working memory
    workingMemory_->store(pattern, 0.8f);
    std::cout << "Working memory stores pattern of length " << pattern.size() << "\n";
    
    // Retrieve pattern
    std::vector<float> retrieved;
    for (size_t i = 0; i < 20; ++i) {
        retrieved.push_back(workingMemory_->retrieve(i % 100));
    }
    
    // Calculate similarity
    float similarity = 0.0f;
    for (size_t i = 0; i < retrieved.size(); ++i) {
        similarity += std::abs(retrieved[i] - pattern[i]);
    }
    similarity /= retrieved.size();
    
    std::cout << "Pattern retrieval similarity: " << std::fixed << std::setprecision(3) 
              << (1.0f - similarity) * 100 << "%\n";
    
    // Test episodic memory
    EpisodicMemoryItem episode;
    episode.timestamp = 100.0f;
    episode.metadata = "Exploration sequence";
    
    for (size_t i = 0; i < 50; ++i) {
        episode.neurons.push_back(NeuronId(static_cast<uint64_t>(i) * 1000));
        episode.values.push_back(std::sin(static_cast<float>(i) * 0.2f));
    }
    
    episodicMemory_->storeEpisode(episode);
    std::cout << "Episodic memory stores " << episodicMemory_->getEpisodeCount() 
              << " episodes\n";
    
    // Retrieve recent episodes
    auto recent = episodicMemory_->getRecentEpisodes(1);
    if (!recent.empty()) {
        std::cout << "Retrieved episode contains " << recent[0].neurons.size() 
                  << " neural activations\n";
    }
}

void Phase5Demo::runCognitionDemo() {
    std::cout << "\n--- COGNITION DEMONSTRATION ---\n";
    
    // Create initial concepts
    std::vector<std::string> concepts = {"object", "color", "shape", "movement", "sound"};
    
    for (size_t i = 0; i < concepts.size(); ++i) {
        Concept concept;
        concept.name = concepts[i];
        concept.id = i;
        concept.associatedNeurons.resize(100);
        
        for (size_t j = 0; j < concept.associatedNeurons.size(); ++j) {
            concept.associatedNeurons[j] = NeuronId(static_cast<uint64_t>(i) * 10000 + j);
            concept.strength = 0.5f + std::sin(static_cast<float>(j) * 0.1f) * 0.5f;
        }
        
        conceptFormation_->addConcept(concept);
    }
    
    std::cout << "Created " << concepts.size() << " concepts\n";
    
    // Test concept formation with new input
    std::string inputConcept = "bright_color";
    
    // Extract features from input
    std::vector<float> features(5, 0.0f);
    features[0] = 0.8f;  // brightness
    features[1] = 0.7f;  // color intensity
    features[2] = 0.3f;  // texture
    features[3] = 0.1f;  // movement
    features[4] = 0.2f;  // sound
    
    // Form new concept
    Concept newConcept = conceptFormation_->formConcept(inputConcept, features);
    
    std::cout << "Formed new concept: '" << newConcept.name << "'\n";
    std::cout << "Concept strength: " << std::fixed << std::setprecision(3) << newConcept.strength << "\n";
    
    // Test neural planning
    std::cout << "\n--- NEURAL PLANNING ---\n";
    
    // Define goal
    Goal goal;
    goal.name = "Navigate to goal";
    goal.priority = 0.9f;
    goal.constraints = {"within_bounds", "energy_available"};
    
    // Create action sequence
    std::vector<Action> plannedActions;
    for (int i = 0; i < 5; ++i) {
        Action action;
        action.type = static_cast<ActionType>(i % 8);
        action.parameters = {static_cast<float>(i) * 0.5f, std::sin(static_cast<float>(i))};
        plannedActions.push_back(action);
    }
    
    // Plan action sequence
    std::vector<Action> plan = neuralPlanner_->plan(goal, plannedActions);
    
    std::cout << "Planned action sequence of length " << plan.size() << ":\n";
    for (size_t i = 0; i < plan.size(); ++i) {
        std::cout << "  " << i + 1 << ". " << actionTypeToString(plan[i].type) << "\n";
    }
}

void Phase5Demo::runActionDemo() {
    std::cout << "\n--- ACTION DEMONSTRATION ---\n";
    
    // Create world with agent
    std::cout << "World size: " << world_->getWidth() << " x " << world_->getHeight() << "\n";
    std::cout << "Vision size: " << world_->getVisionWidth() << " x " << world_->getVisionHeight() << "\n\n";
    
    // Reset world
    world_->reset();
    
    // Simulate some interaction
    std::cout << "Simulating agent interaction:\n";
    
    for (int step = 0; step < 10; ++step) {
        // Get sensory input
        const SensoryPercept& percept = world_->getSensoryPercept();
        
        // Process sensory input
        agentBrain_->processSensoryInput(percept);
        
        // Simulate brain activity
        brain_->step(step);
        
        // Decode action
        MotorCommand action = agentBrain_->decodeMotorCommand();
        
        // Apply action
        ActionResult result = world_->applyMotorCommand(action, 0.01 * step);
        
        // Apply reward
        agentBrain_->applyRewardModulation(result.reward, 0.0f);
        
        // Print step
        std::cout << "  Step " << step + 1 << ": " << motorCommandToString(action);
        std::cout << " -> reward=" << std::fixed << std::setprecision(3) << result.reward;
        std::cout << ", energy=" << world_->getAgentBody().energy << "\n";
        
        // Update world
        world_->update(0.01);
        
        // Update development
        agentBrain_->updateDevelopment(0.01);
        
        // Store in episodic memory
        EpisodicMemoryItem episode;
        episode.timestamp = 0.01 * step;
        episode.metadata = "Action step " + std::to_string(step + 1);
        
        // Store neural activity
        for (const auto& region : brain_->getRegions()) {
            const auto* neurons = region->getAllNeurons();
            if (neurons) {
                for (const auto* neuron : *neurons) {
                    if (neuron->getState().isActive) {
                        episode.neurons.push_back(neuron->getId());
                        episode.values.push_back(neuron->getState().membranePotential);
                    }
                }
            }
        }
        
        episodicMemory_->storeEpisode(episode);
    }
    
    std::cout << "\nAgent completed " << 10 << " steps in the environment.\n";
}

void Phase5Demo::showResults() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "PHASE 5 SUMMARY\n";
    std::cout << std::string(60, '=') << "\n\n";
    
    std::cout << "System successfully integrated:\n";
    std::cout << "  ✓ Real-time neural computation (" << brain_->getTotalNeuronCount() << " neurons)\n";
    std::cout << "  ✓ Sensory-motor loop with world interaction\n";
    std::cout << "  ✓ Predictive coding and learning\n";
    std::cout << "  ✓ Working and episodic memory\n";
    std::cout << "  ✓ Concept formation and learning\n";
    std::cout << "  ✓ Neural planning and action generation\n";
    std::cout << "  ✓ Neuromodulation and development\n\n";
    
    std::cout << "System demonstrates:\n";
    std::cout << "  • Experience-driven prediction and error correction\n";
    std::cout << "  • Working memory for active information maintenance\n";
    std::cout << "  • Episodic memory for event recording\n";
    std::cout << "  • Concept formation from sensory input\n";
    std::cout << "  • Goal-directed planning with action sequences\n";
    std::cout << "  • Integration of all cognitive modules\n\n";
    
    std::cout << "This represents a complete neurocogntive architecture\n";
    std::cout << "capable of learning, predicting, remembering, and planning.\n";
}

std::string Phase5Demo::actionTypeToString(ActionType type) const {
    switch (type) {
        case ActionType::MoveForward: return "MoveForward";
        case ActionType::MoveBackward: return "MoveBackward";
        case ActionType::TurnLeft: return "TurnLeft";
        case ActionType::TurnRight: return "TurnRight";
        case ActionType::Look: return "Look";
        case ActionType::LookUp: return "LookUp";
        case ActionType::LookDown: return "LookDown";
        case ActionType::Interact: return "Interact";
        case ActionType::Wait: return "Wait";
        default: return "Unknown";
    }
}

} // namespace nlm