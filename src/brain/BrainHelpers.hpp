// Brain Helper Functions Header
// Refactored helper functions for Brain.cpp

#ifndef BRAIN_HELPERS_HPP
#define BRAIN_HELPERS_HPP

#include "Brain.hpp"
#include "../brain/BrainInitialization.hpp"
#include "../brain/BrainIntegration.hpp"
#include "../brain/BrainCoreStep.hpp"
#include "../brain/BrainCheckpoint.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <string>
#include <limits>

namespace nlm {

class BrainHelpers {
public:
    // Configuration and initialization helpers
    static void logConfiguration(const Brain::Impl& pImpl, 
                                 size_t neuronCount, 
                                 size_t regionCount, 
                                 float connectionProbability);
    static void createRegions(Brain::Impl& pImpl, size_t regionCount);
    static void createNeuronsAndPopulateNeurons(Brain::Impl& pImpl, 
                                                size_t neuronCount, 
                                                size_t regionCount, 
                                                float connectionProbability,
                                                bool isNewInitialization);
    static void collectSensoryMotorNeurons(Brain::Impl& pImpl);
    static void logInitializationStats(const Brain::Impl& pImpl);
    
    // Step helper methods
    static void registerSpikeHandlers(Brain::Impl& pImpl);
    static void applyHomeostaticScaling(Brain::Impl& pImpl, SimulationStep currentStep);
    static void updateDevelopmentSystem(Brain::Impl& pImpl, SimulationStep currentStep);
    
    // Action and behavior helpers
    static void calculateMotorNeuronActivity(const Brain::Impl& pImpl, 
                                             size_t& firingMotor);
    static ActionType selectAction(size_t firingMotor);
    static void applyNeuromodulation(Dopamine& dopamine, float& plasticityMod);
    
    // Reset and state helpers
    static void resetBrain(Brain::Impl& pImpl);
    
    // Sensory and motor helpers
    static void injectSensoryInput(Brain::Impl& pImpl, const SensoryInput& input);
    static void produceAction(const Brain::Impl& pImpl, ActionType& actionType);
    
    // Statistics helpers
    static void updateStatistics(Brain::Impl& pImpl);
    
    // Development helpers
    static void applyDevelopmentEffects(Brain::Impl& pImpl, SimulationStep currentStep);
    
    // Memory helpers
    static void updateEpisodicMemory(Brain::Impl& pImpl, SimulationStep currentStep);
    static void replayMemories(Brain::Impl& pImpl, SimulationStep currentStep);
    static void consolidateMemories(Brain::Impl& pImpl, SimulationStep currentStep);
    
    // Configuration helpers
    static void loadConfiguration(Brain::Impl& pImpl);
    
    // Utility functions
    static std::string formatLogMessage(const std::string& prefix, const std::string& message);
    static float calculateExcitationInhibitionRatio(const Brain::Impl& pImpl);
};

} // namespace nlm

#endif // BRAIN_HELPERS_HPP
