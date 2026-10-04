#include "MotorSystem.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"

#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    
    // Action preferences for exploration/exploitation
    std::unordered_map<ActionType, float> actionPreferences;
    
    // Neural integration
    Brain* brain;
    Dopamine* dopamine;
    Curiosity* curiosity;
    PredictionError* predictionError;
    NeuralWorkingMemory* workingMemory;
    
    // Random generator for exploration
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    
    Impl() : brain(nullptr), dopamine(nullptr), curiosity(nullptr), 
             predictionError(nullptr), workingMemory(nullptr),
             rng(std::random_device{}()), uniformDist(0.0f, 1.0f) {
        availableActions = {
            ActionType::MoveForward,
            ActionType::MoveBackward,
            ActionType::MoveLeft,
            ActionType::MoveRight,
            ActionType::TurnLeft,
            ActionType::TurnRight,
            ActionType::Look,
            ActionType::LookUp,
            ActionType::LookDown,
            ActionType::Interact,
            ActionType::Eat,
            ActionType::Drink,
            ActionType::Rest,
            ActionType::Wait,
            ActionType::Custom
        };
        
        // Initialize preferences with small noise
        for (ActionType action : availableActions) {
            // Start with uniform preference with small random noise
            float basePreference = 1.0f / availableActions.size();
            float noise = uniformDist(rng) * 0.5f - 0.25f;
            actionPreferences[action] = std::max(0.01f, basePreference + noise);
        }
    }
    
    ~Impl() {
        // Clear action history
        for (auto* action : actionHistory) {
            delete action;
        }
        actionHistory.clear();
    }
    
    void initialize(Brain* brainPtr) {
        brain = brainPtr;
        
        // Get neuromodulation systems
        if (brain) {
            dopamine = brain->getDopamine();
            curiosity = brain->getCuriosity();
            predictionError = brain->getPredictionErrorSignal();
            workingMemory = brain->getWorkingMemory();
        }
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

void MotorSystem::initialize(Brain* brain) {
    pImpl->initialize(brain);
}

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    if (pImpl->availableActions.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Compute action selection scores based on multiple factors
    std::vector<std::pair<ActionType, float>> actionScores;
    
    // Get neural activity from motor region populations
    std::vector<float> populationActivations;
    const std::vector<NeuralPopulation*>& allPopulations = const_cast<NeuralRegion&>(motorRegion).getAllPopulations();
    for (size_t i = 0; i < allPopulations.size(); ++i) {
        const NeuralPopulation* population = allPopulations[i];
        if (population) {
            populationActivations.push_back(population->getAverageFiringRate());
        }
    }
    
    // Calculate baseline action preferences based on neural activity
    // Each population might have a preferred action type
    for (ActionType action : pImpl->availableActions) {
        float score = pImpl->actionPreferences[action];
        
        // Adjust score based on neuromodulation
        if (pImpl->dopamine) {
            float dopamineLevel = pImpl->dopamine->getLevel();
            // Dopamine enhances actions with positive prediction errors
            if (pImpl->predictionError) {
                float predictionError = pImpl->predictionError->getError();
                // Reward prediction error modulates dopamine effect
                score += dopamineLevel * predictionError * 0.5f;
            }
        }
        
        // Curiosity encourages exploration of novel actions
        if (pImpl->curiosity) {
            float curiosityLevel = pImpl->curiosity->getLevel();
            // Increase score for less frequently chosen actions
            int actionCount = std::count_if(pImpl->actionHistory.begin(), pImpl->actionHistory.end(),
                [action](const Action* a) { return a->getType() == action; });
            float novelty = 1.0f / (1.0f + static_cast<float>(actionCount));
            score += curiosityLevel * novelty * 0.3f;
        }
        
        // Integrate with working memory content
        if (pImpl->workingMemory) {
            const std::vector<NeuronId>& memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            // Check if action-related neurons are in working memory
            for (NeuronId neuronId : memoryNeurons) {
                const std::vector<NeuralPopulation*>& allPopulations = const_cast<NeuralRegion&>(motorRegion).getAllPopulations();
                if (!allPopulations.empty()) {
                    size_t populationIndex = neuronId.index() % allPopulations.size();
                    const NeuralPopulation* population = allPopulations[populationIndex];
                    if (population && population->getAverageFiringRate() > 0.5f) {
                        // Strong working memory activation boosts action probability
                        score *= 1.2f;
                        break;
                    }
                }
            }
        }
        
        // Ensure score is non-negative
        score = std::max(0.0f, score);
        actionScores.push_back({action, score});
    }
    
    // Normalize scores to probabilities
    float totalScore = 0.0f;
    for (const auto& pair : actionScores) {
        totalScore += pair.second;
    }
    
    if (totalScore <= 0.0f) {
        // Fallback to uniform random selection
        int index = std::uniform_int_distribution<int>(0, pImpl->availableActions.size() - 1)(pImpl->rng);
        auto action = std::make_unique<Action>(pImpl->availableActions[index]);
        pImpl->actionHistory.push_back(action.get());
        return action;
    }
    
    // Convert scores to probabilities
    std::vector<float> probabilities;
    for (const auto& pair : actionScores) {
        probabilities.push_back(pair.second / totalScore);
    }
    
    // Cumulative distribution for weighted random selection
    std::vector<float> cumulativeProbabilities;
    float cumsum = 0.0f;
    for (float prob : probabilities) {
        cumsum += prob;
        cumulativeProbabilities.push_back(cumsum);
    }
    
    float randomValue = pImpl->uniformDist(pImpl->rng);
    
    // Select action based on weighted probabilities
    ActionType selectedAction;
    for (size_t i = 0; i < actionScores.size(); ++i) {
        if (randomValue <= cumulativeProbabilities[i]) {
            selectedAction = actionScores[i].first;
            break;
        }
        if (i == actionScores.size() - 1) {
            selectedAction = actionScores.back().first;
        }
    }
    
    // Record selected action in history
    auto newAction = std::make_unique<Action>(selectedAction);
    pImpl->actionHistory.push_back(newAction.get());
    
    return newAction;
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Validate action
    bool isValidAction = false;
    for (ActionType availableAction : pImpl->availableActions) {
        if (availableAction == action) {
            isValidAction = true;
            break;
        }
    }
    
    if (isValidAction) {
        // Clamp preference to valid range [0.0, 10.0]
        float clampedPreference = std::max(0.0f, std::min(preference, 10.0f));
        pImpl->actionPreferences[action] = clampedPreference;
    }
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

void MotorSystem::clearHistory() {
    for (auto* action : pImpl->actionHistory) {
        delete action;
    }
    pImpl->actionHistory.clear();
}

} // namespace nlm