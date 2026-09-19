// IDopamine.hpp
// Abstract interface for dopamine neuromodulator
// Implements reward prediction and reinforcement learning signals
// Dopamine modulates plasticity for reward-associated behaviors

#pragma once

#include "INeurosystem.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <map>

namespace nlm {

class IDopamine : public INeurosystem {
public:
    virtual ~IDopamine() = default;

    // Initialize dopamine system with brain reference
    virtual bool initialize(Brain* brain) override = 0;

    // Get neuromodulator name
    virtual const char* getName() const override { return "Dopamine"; }

    // Get current dopamine level
    virtual float getLevel() const override = 0;
    virtual void setLevel(float level) override = 0;

    // Update dopamine state (called each timestep)
    virtual void update(TimestepDuration dt) override = 0;

    // Apply dopamine effect to plasticity
    virtual float getPlasticityFactor() const override = 0;

    // Signal reward observation
    virtual void signalReward(float reward, SimulationStep timestamp) = 0;

    // Signal reward prediction error
    virtual void signalRewardPredictionError(float error, SimulationStep timestamp) = 0;

    // Get reward history
    virtual const std::vector<float>& getRewardHistory() const = 0;
    virtual const std::vector<float>& getPredictionErrorHistory() const = 0;
    virtual void clearHistory() = 0;

    // Get reward statistics
    virtual float getAverageReward() const = 0;
    virtual float getAveragePredictionError() const = 0;
    virtual float getDiscountedReward(SimulationStep stepsBack = 100) const = 0;

    // Set/Get dopamine baseline
    virtual float getBaselineLevel() const = 0;
    virtual void setBaselineLevel(float level) = 0;

    // Get/Get dopamine modulation strength
    virtual float getModulationStrength() const = 0;
    virtual void setModulationStrength(float strength) = 0;

    // Get reward anticipation (predicted future reward)
    virtual float getExpectedReward() const = 0;
    virtual void setExpectedReward(float reward) = 0;

    // Reset dopamine system to initial state
    virtual void reset() override = 0;

    // Interface information override
    virtual NeuromodulatorType getType() const override { return NeuromodulatorType::Reward; }

    // Error handling override
    virtual bool hasError() const override = 0;
    virtual std::string getErrorMessage() const override = 0;
    virtual void clearError() override = 0;

    // Callback pattern for dopamine events
    using RewardSignaledCallback = std::function<void(float reward, SimulationStep timestamp)>;
    virtual void setRewardSignaledCallback(const RewardSignaledCallback& callback) = 0;

    using PredictionErrorSignaledCallback = std::function<void(float error, SimulationStep timestamp)>;
    virtual void setPredictionErrorSignaledCallback(const PredictionErrorSignaledCallback& callback) = 0;

    // Debug information override
    virtual std::string getDebugInfo() const override = 0;

    // Performance metrics
    virtual uint64_t getTotalRewardsSignaled() const = 0;
    virtual uint64_t getTotalPredictionErrorsSignaled() const = 0;
    virtual float getRewardPredictionAccuracy() const = 0;

protected:
    IDopamine() = default;
};

} // namespace nlm
