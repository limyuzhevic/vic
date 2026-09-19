// ICuriosity.hpp
// Abstract interface for curiosity neuromodulator
// Implements exploration motivation and intrinsic reward signals
// Curiosity drives exploration of novel or challenging states

#pragma once

#include "INeurosystem.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <map>

namespace nlm {

class ICuriosity : public INeurosystem {
public:
    virtual ~ICuriosity() = default;

    // Initialize curiosity system with brain reference
    virtual bool initialize(Brain* brain) override = 0;

    // Get neuromodulator name
    virtual const char* getName() const override { return "Curiosity"; }

    // Get current curiosity level
    virtual float getLevel() const override = 0;
    virtual void setLevel(float level) override = 0;

    // Update curiosity state (called each timestep)
    virtual void update(TimestepDuration dt) override = 0;

    // Apply curiosity effect to plasticity
    virtual float getPlasticityFactor() const override = 0;

    // Signal state exploration or novelty detection
    virtual void signalExploration(NeuronId exploringNeuron, float explorationStrength = 1.0f) = 0;

    // Signal curiosity-driven learning
    virtual void signalLearningOpportunity(const std::vector<float>& stateFeatures,
                                          float learningPotential = 1.0f) = 0;

    // Get curiosity history
    virtual const std::vector<float>& getExplorationHistory() const = 0;
    virtual const std::vector<float>& getLearningOpportunityHistory() const = 0;
    virtual void clearHistory() = 0;

    // Calculate curiosity based on prediction error
    virtual float calculateCuriosityFromPredictionError(float predictionError,
                                                       float confidence) const = 0;

    // Calculate curiosity based on state novelty
    virtual float calculateCuriosityFromNovelty(const std::vector<float>& state) const = 0;

    // Set/Get curiosity baseline
    virtual float getBaselineLevel() const = 0;
    virtual void setBaselineLevel(float level) = 0;

    // Get/Get curiosity modulation strength
    virtual float getModulationStrength() const = 0;
    virtual void setModulationStrength(float strength) = 0;

    // Get current exploration drive
    virtual float getExplorationDrive() const = 0;
    virtual void setExplorationDrive(float drive) = 0;

    // Reset curiosity system to initial state
    virtual void reset() override = 0;

    // Interface information override
    virtual NeuromodulatorType getType() const override { return NeuromodulatorType::Novelty; }

    // Error handling override
    virtual bool hasError() const override = 0;
    virtual std::string getErrorMessage() const override = 0;
    virtual void clearError() override = 0;

    // Callback pattern for curiosity events
    using ExplorationSignaledCallback = std::function<void(NeuronId neuron, float strength)>;
    virtual void setExplorationSignaledCallback(const ExplorationSignaledCallback& callback) = 0;

    using LearningOpportunitySignaledCallback = std::function<void(const std::vector<float>& features,
                                                                 float potential)>;
    virtual void setLearningOpportunitySignaledCallback(const LearningOpportunitySignaledCallback& callback) = 0;

    // Debug information override
    virtual std::string getDebugInfo() const override = 0;

    // Performance metrics
    virtual uint64_t getTotalExplorationsSignaled() const = 0;
    virtual uint64_t getTotalLearningOpportunitiesSignaled() const = 0;
    virtual float getExplorationEfficiency() const = 0;  // Exploration per unit time

protected:
    ICuriosity() = default;
};

} // namespace nlm
