#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include "../brain/Brain.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../plasticity/STDP.hpp"

namespace nlm {

/**
 * Configuration structure for agent brain behavior
 */
struct AgentConfiguration {
    bool learningEnabled;
    bool plasticityEnabled;
    bool developmentEnabled;
    bool curiosityEnabled;
    float learningRate;
    float plasticityRate;
    float developmentRate;
    
    AgentConfiguration()
        : learningEnabled(true)
        , plasticityEnabled(true)
        , developmentEnabled(true)
        , curiosityEnabled(true)
        , learningRate(1.0f)
        , plasticityRate(1.0f)
        , developmentRate(1.0f) {}
    
    bool isLearningEnabled() const { return learningEnabled; }
    bool isPlasticityEnabled() const { return plasticityEnabled; }
    bool isDevelopmentEnabled() const { return developmentEnabled; }
    bool isCuriosityEnabled() const { return curiosityEnabled; }
    
    void enableLearning(bool enable) { learningEnabled = enable; }
    void enablePlasticity(bool enable) { plasticityEnabled = enable; }
    void enableDevelopment(bool enable) { developmentEnabled = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled = enable; }
    
    void setLearningRate(float rate) { learningRate = rate; }
    void setPlasticityRate(float rate) { plasticityRate = rate; }
    void setDevelopmentRate(float rate) { developmentRate = rate; }
    
    float getLearningRate() const { return learningRate; }
    float getPlasticityRate() const { return plasticityRate; }
    float getDevelopmentRate() const { return developmentRate; }
};

/**
 * Helper class for visualizing NLM brain state
 */
class VisualizationHelper {
public:
    VisualizationHelper() = default;
    ~VisualizationHelper() = default;
    
    /**
     * Create neuron spike raster visualization data
     */
    std::vector<std::pair<float, float>> createNeuronRaster(
        std::shared_ptr<Brain> brain, size_t steps) {
        std::vector<std::pair<float, float>> spikes;
        // Simplified implementation for visualization
        // In a real implementation, this would create proper visualization data
        return spikes;
    }
    
    /**
     * Calculate network-level metrics for visualization
     */
    std::unordered_map<std::string, float> calculateNetworkMetrics(
        std::shared_ptr<Brain> brain) {
        std::unordered_map<std::string, float> metrics;
        metrics["avg_firing_rate"] = brain->getAverageFiringRate();
        metrics["excitatory_inhibition_ratio"] = 
            brain->getExcitationInhibitionRatio();
        metrics["total_neurons"] = brain->getTotalNeuronCount();
        metrics["total_synapses"] = brain->getTotalSynapseCount();
        return metrics;
    }
};

/**
 * Agent body state for complete agent system
 */
struct AgentBodyState {
    float x;
    float y;
    float orientation;
    float velocityX;
    float velocityY;
    float angularVelocity;
    float energy;
    float health;
    float age;
    bool isMoving;
    bool isTurning;
    double lastActionTime;
    
    AgentBodyState()
        : x(0.0f), y(0.0f), orientation(0.0f), velocityX(0.0f), velocityY(0.0f)
        , angularVelocity(0.0f), energy(100.0f), health(100.0f), age(0.0f)
        , isMoving(false), isTurning(false), lastActionTime(0.0) {}
    
    void reset() {
        x = y = orientation = velocityX = velocityY = angularVelocity = 0.0f;
        energy = health = 100.0f;
        age = 0.0f;
        isMoving = isTurning = false;
        lastActionTime = 0.0;
    }
};

} // namespace nlm
