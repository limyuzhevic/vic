#include "MotorDecoder.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>

namespace nlm {

MotorDecoder::MotorDecoder(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , world_(nullptr)
    , currentForwardActivity_(0.0f)
    , currentBackwardActivity_(0.0f)
    , currentLeftActivity_(0.0f)
    , currentRightActivity_(0.0f)
    , currentInteractActivity_(0.0f)
    , currentWaitActivity_(0.0f)
    , lastMaxActivity_(0.0f) {
    
    // Initialize with default configuration
    config_.enableDebugOutput_ = false;
    
    // Set default strategy
    strategy_ = std::make_unique<ThresholdStrategy>();
    
    NLM_LOG_INFO("MotorDecoder created with default configuration");
}

void MotorDecoder::configure(const MotorConfig& config) {
    config_ = config;
    
    // Validate configuration
    if (config_.enableExploration_ && config_.explorationThreshold_ < 0.0f || 
        config_.explorationThreshold_ > 1.0f) {
        NLM_LOG_WARNING("Exploration threshold must be between 0.0 and 1.0, using default");
        config_.explorationThreshold_ = 0.3f;
    }
    
    if (config_.activityThreshold_ < 0.0f || config_.activityThreshold_ > 1.0f) {
        NLM_LOG_WARNING("Activity threshold must be between 0.0 and 1.0, using default");
        config_.activityThreshold_ = 0.5f;
    }
    
    NLM_LOG_INFO("MotorDecoder configured successfully");
}

void MotorDecoder::setSensoryNeuronGroups(
    const std::vector<Neuron*>& vision,
    const std::vector<Neuron*>& touch,
    const std::vector<Neuron*>& internal,
    const std::vector<Neuron*>& proprioception) {
    
    sensoryVision_ = vision;
    sensoryTouch_ = touch;
    sensoryInternal_ = internal;
    sensoryProprioception_ = proprioception;
    
    // Log group sizes
    size_t totalVision = 0;
    size_t totalTouch = 0;
    size_t totalInternal = 0;
    size_t totalProprioception = 0;
    
    for (const auto& neuron : vision) {
        totalVision++;
    }
    for (const auto& neuron : touch) {
        totalTouch++;
    }
    for (const auto& neuron : internal) {
        totalInternal++;
    }
    for (const auto& neuron : proprioception) {
        totalProprioception++;
    }
    
    NLM_LOG_INFO("MotorDecoder sensory groups configured: "
                 "Vision=" + std::to_string(totalVision) +
                 " Touch=" + std::to_string(totalTouch) +
                 " Internal=" + std::to_string(totalInternal) +
                 " Proprioception=" + std::to_string(totalProprioception));
}

void MotorDecoder::setStrategy(std::unique_ptr<MotorDecodingStrategy> strategy) {
    if (strategy) {
        strategy_ = std::move(strategy);
        NLM_LOG_INFO("MotorDecoder strategy changed to: " + strategy_->getName());
    }
}

MotorCommand MotorDecoder::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot decode motor command: brain is null");
        return MotorCommand::Wait;
    }
    
    // Calculate current activity levels
    updateActivityLevels();
    
    // Prepare decoding context
    MotorDecodingContext context;
    context.brain = brain_;
    context.world = world_;
    context.forwardActivity = currentForwardActivity_;
    context.backwardActivity = currentBackwardActivity_;
    context.leftActivity = currentLeftActivity_;
    context.rightActivity = currentRightActivity_;
    context.interactActivity = currentInteractActivity_;
    context.waitActivity = currentWaitActivity_;
    context.curiosityLevel = 0.0f; // Will be set by AgentBrain
    context.noveltyLevel = 0.0f;   // Will be set by AgentBrain
    context.predictionError = 0.0f; // Will be set by AgentBrain
    context.dopamineLevel = 0.0f;  // Will be set by AgentBrain
    context.plasticityModifier = 0.0f; // Will be set by AgentBrain
    
    // Use selected strategy for decoding
    MotorCommand decoded = strategy_->decode(context);
    
    // Apply curiosity-based exploration if enabled
    if (config_.enableExploration_) {
        decoded = exploreWithCuriosity(decoded);
    }
    
    if (config_.enableDebugOutput_) {
        NLM_LOG_INFO("Motor decoding: "
                     "Forward=" + std::to_string(currentForwardActivity_) +
                     " Backward=" + std::to_string(currentBackwardActivity_) +
                     " Left=" + std::to_string(currentLeftActivity_) +
                     " Right=" + std::to_string(currentRightActivity_) +
                     " Interact=" + std::to_string(currentInteractActivity_) +
                     " Wait=" + std::to_string(currentWaitActivity_) +
                     " Result=" + std::to_string(static_cast<int>(decoded)));
    }
    
    return decoded;
}

void MotorDecoder::updateActivityWeights(float forward, float backward, float left, float right,
                                          float interact, float wait) {
    currentForwardActivity_ = forward;
    currentBackwardActivity_ = backward;
    currentLeftActivity_ = left;
    currentRightActivity_ = right;
    currentInteractActivity_ = interact;
    currentWaitActivity_ = wait;
    
    // Update maximum activity for statistics
    lastMaxActivity_ = std::max({forward, backward, left, right, interact, wait});
}

void MotorDecoder::updateActivityLevels() {
    // Calculate motor neuron activities based on membrane potential deviations
    auto calculateActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        
        float totalActivity = 0.0f;
        size_t validNeurons = 0;
        
        for (Neuron* neuron : neurons) {
            if (neuron) {
                float activity = std::abs(neuron->getMembranePotential() - neuron->getRestingPotential());
                totalActivity += activity;
                validNeurons++;
            }
        }
        
        return validNeurons > 0 ? totalActivity / validNeurons : 0.0f;
    };
    
    float forwardAct = calculateActivity(motorForward_);
    float backwardAct = calculateActivity(motorBackward_);
    float leftAct = calculateActivity(motorTurnLeft_);
    float rightAct = calculateActivity(motorTurnRight_);
    float interactAct = calculateActivity(motorInteract_);
    float waitAct = calculateActivity(motorWait_);
    
    updateActivityWeights(forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct);
}

MotorCommand MotorDecoder::exploreWithCuriosity(MotorCommand defaultCmd) {
    // Random exploration based on curiosity level
    // This is a simplified implementation - AgentBrain will provide actual curiosity levels
    float exploreChance = 0.1f; // Default exploration probability
    
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    if (r < exploreChance) {
        // Random motor command
        int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
        switch (choice) {
            case 0: return MotorCommand::MoveForward;
            case 1: return MotorCommand::MoveBackward;
            case 2: return MotorCommand::TurnLeft;
            case 3: return MotorCommand::TurnRight;
            case 4: return MotorCommand::LookLeft;
            case 5: return MotorCommand::LookRight;
            case 6: return MotorCommand::Interact;
            default: return MotorCommand::Wait;
        }
    }
    
    return defaultCmd;
}

float MotorDecoder::getAverageActivity() const {
    if (!brain_) return 0.0f;
    
    float activities[] = {
        currentForwardActivity_,
        currentBackwardActivity_,
        currentLeftActivity_,
        currentRightActivity_,
        currentInteractActivity_,
        currentWaitActivity_
    };
    
    float sum = std::accumulate(std::begin(activities), std::end(activities), 0.0f);
    return std::accumulate(std::begin(activities), std::end(activities), 0.0f) / 6.0f;
}

float MotorDecoder::getMaxActivity() const {
    return lastMaxActivity_;
}

size_t MotorDecoder::getActiveMotorNeurons() const {
    size_t totalActive = 0;
    
    auto countActive = [](const std::vector<Neuron*>& neurons) -> size_t {
        size_t count = 0;
        for (Neuron* neuron : neurons) {
            if (neuron && std::abs(neuron->getMembranePotential() - neuron->getRestingPotential()) > 0.1f) {
                count++;
            }
        }
        return count;
    };
    
    totalActive += countActive(motorForward_);
    totalActive += countActive(motorBackward_);
    totalActive += countActive(motorTurnLeft_);
    totalActive += countActive(motorTurnRight_);
    totalActive += countActive(motorInteract_);
    totalActive += countActive(motorWait_);
    
    return totalActive;
}

// MotorDecoder strategy implementations

MotorCommand MotorDecoder::ThresholdStrategy::decode(const MotorDecodingContext& context) {
    float maxActivity = std::max({
        context.forwardActivity,
        context.backwardActivity,
        context.leftActivity,
        context.rightActivity,
        context.interactActivity,
        context.waitActivity
    });
    
    // Only act if there's meaningful activity
    if (maxActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    // Find the command with maximum activity
    struct CommandWithActivity {
        MotorCommand cmd;
        float activity;
    };
    
    CommandWithActivity commands[] = {
        {MotorCommand::MoveForward, context.forwardActivity},
        {MotorCommand::MoveBackward, context.backwardActivity},
        {MotorCommand::TurnLeft, context.leftActivity},
        {MotorCommand::TurnRight, context.rightActivity},
        {MotorCommand::Interact, context.interactActivity},
        {MotorCommand::Wait, context.waitActivity}
    };
    
    MotorCommand bestCmd = MotorCommand::Wait;
    float bestActivity = context.waitActivity; // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            bestCmd = c.cmd;
        }
    }
    
    return bestCmd;
}

MotorCommand MotorDecoder::WeightedVotingStrategy::decode(const MotorDecodingContext& context) {
    struct CommandWithWeightedActivity {
        MotorCommand cmd;
        float weightedActivity;
    };
    
    CommandWithWeightedActivity commands[] = {
        {MotorCommand::MoveForward, context.forwardActivity * 1.0f},
        {MotorCommand::MoveBackward, context.backwardActivity * 1.0f},
        {MotorCommand::TurnLeft, context.leftActivity * 1.0f},
        {MotorCommand::TurnRight, context.rightActivity * 1.0f},
        {MotorCommand::Interact, context.interactActivity * 1.0f},
        {MotorCommand::Wait, context.waitActivity * 1.0f}
    };
    
    MotorCommand bestCmd = MotorCommand::Wait;
    float bestWeightedActivity = 0.0f;
    
    for (const auto& c : commands) {
        if (c.weightedActivity > bestWeightedActivity) {
            bestWeightedActivity = c.weightedActivity;
            bestCmd = c.cmd;
        }
    }
    
    // Apply threshold if configured
    if (bestWeightedActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return bestCmd;
}

MotorCommand MotorDecoder::TopNStrategy::decode(const MotorDecodingContext& context) {
    // Collect all commands with their activities
    std::vector<std::pair<MotorCommand, float>> commandActivities;
    
    commandActivities.emplace_back(MotorCommand::MoveForward, context.forwardActivity);
    commandActivities.emplace_back(MotorCommand::MoveBackward, context.backwardActivity);
    commandActivities.emplace_back(MotorCommand::TurnLeft, context.leftActivity);
    commandActivities.emplace_back(MotorCommand::TurnRight, context.rightActivity);
    commandActivities.emplace_back(MotorCommand::Interact, context.interactActivity);
    commandActivities.emplace_back(MotorCommand::Wait, context.waitActivity);
    
    // Sort by activity (descending)
    std::sort(commandActivities.begin(), commandActivities.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Select top N commands
    int numCommands = std::min(config_.topN_, static_cast<int>(commandActivities.size()));
    
    // Choose among top N with weighted random selection
    if (numCommands > 1) {
        float totalActivity = 0.0f;
        for (int i = 0; i < numCommands; ++i) {
            totalActivity += commandActivities[i].second;
        }
        
        if (totalActivity > 0.0f) {
            float r = context.brain->getRandomGenerator()->uniformReal(0.0f, totalActivity);
            float accumulated = 0.0f;
            
            for (int i = 0; i < numCommands; ++i) {
                accumulated += commandActivities[i].second;
                if (r <= accumulated) {
                    return commandActivities[i].first;
                }
            }
        }
    }
    
    // If only one command or selection fails, return the top command
    if (!commandActivities.empty()) {
        return commandActivities[0].first;
    }
    
    return MotorCommand::Wait;
}

MotorCommand MotorDecoder::AdaptiveThresholdStrategy::decode(const MotorDecodingContext& context) {
    // Calculate adaptive threshold based on overall activity
    float maxActivity = std::max({
        context.forwardActivity,
        context.backwardActivity,
        context.leftActivity,
        context.rightActivity,
        context.interactActivity,
        context.waitActivity
    });
    
    // Adaptive threshold: base + modulation based on overall activity
    float threshold = config_.adaptiveThresholdBase_ + 
                      config_.adaptiveThresholdMod_ * maxActivity;
    
    // Clamp threshold to reasonable range
    threshold = std::clamp(threshold, 0.1f, 0.9f);
    
    // Find commands above threshold
    struct CommandWithActivity {
        MotorCommand cmd;
        float activity;
    };
    
    CommandWithActivity commands[] = {
        {MotorCommand::MoveForward, context.forwardActivity},
        {MotorCommand::MoveBackward, context.backwardActivity},
        {MotorCommand::TurnLeft, context.leftActivity},
        {MotorCommand::TurnRight, context.rightActivity},
        {MotorCommand::Interact, context.interactActivity},
        {MotorCommand::Wait, context.waitActivity}
    };
    
    // Find the command with maximum activity
    MotorCommand bestCmd = MotorCommand::Wait;
    float bestActivity = context.waitActivity;
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity && c.activity >= threshold) {
            bestActivity = c.activity;
            bestCmd = c.cmd;
        }
    }
    
    return bestCmd;
}

} // namespace nlm
