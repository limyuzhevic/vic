#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

// Initialize AgentBrain with validation and error checking
AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
{
    try {
        // Validate input brain pointer
        if (!brain_) {
            NLM_LOG_ERROR("AgentBrain initialized with null brain reference");
            // Allow null brain for flexibility, but log the issue
        }
        
        // Initialize motor and sensory neuron groups
        initializeNeuronGroups();
        
        // Initialize development state
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        NLM_LOG_INFO("AgentBrain initialized successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain initialization failed: " + std::string(e.what()));
        throw;
    }
}

// Initialize neuron groups with error checking
void AgentBrain::initializeNeuronGroups() {
    if (!brain_) {
        return;
    }
    
    try {
        for (const auto& region : brain_->getRegions()) {
            if (!region) {
                NLM_LOG_WARNING("Null region encountered during AgentBrain initialization");
                continue;
            }
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) {
                    NLM_LOG_WARNING("Null population encountered during AgentBrain initialization");
                    continue;
                }
                
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (!n) {
                            NLM_LOG_WARNING("Null neuron encountered during AgentBrain initialization");
                            continue;
                        }
                        
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (!n) {
                            NLM_LOG_WARNING("Null neuron encountered during AgentBrain initialization");
                            continue;
                        }
                        
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Neuron group initialization failed: " + std::string(e.what()));
        throw;
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    try {
        previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        NLM_LOG_INFO("AgentBrain initialized with " + 
                     std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                     std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                     std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain initialization failed: " + std::string(e.what()));
        throw;
    }
}