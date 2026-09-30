#pragma once

#include "Brain.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <any>

namespace nlm {

class BrainMemoryManager;
class BrainCognitionManager;
class BrainNeuromodulationManager;
class BrainPlasticityManager;

class Brain {
public:
    // Configuration structure for brain initialization
    struct Config {
        size_t neuron_count = 1000;
        size_t region_count = 1;
        float connection_probability = 0.1f;
        double timestep = 0.001;
        size_t replay_interval = 100;
        size_t consolidation_interval = 1000;
        size_t max_episodes = 1000;
        float synaptogenesis_rate = 0.0001f;
        float pruning_rate = 0.00001f;
        uint64_t random_seed = 42;
        std::string checkpoint_dir = "./checkpoints";
        bool enable_reward_modulation = true;
        bool enable_structural_plasticity = true;
        bool enable_development = true;
        bool enable_curiosity = true;
        
        // Extension points for custom configurations
        std::unordered_map<std::string, std::any> custom_params;
    };

    explicit Brain(const Config& config = Config());
    ~Brain();

    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;

    // Initialize brain with comprehensive error handling
    bool initialize(const std::string& config_path = "");

    // Main simulation step with timing
    void step(size_t step_count, double time = -1.0);

    // Enhanced sensory input with validation
    bool receiveSensoryInput(const SensoryInput& input, size_t input_id = 0);

    // Enhanced motor action with confidence and metadata
    std::pair<MotorCommand, float> produceActionWithConfidence();

    // Configuration management
    bool setConfig(const std::string& key, const std::any& value);
    std::any getConfig(const std::string& key) const;
    const Config& getBrainConfig() const { return config_; }

    // Memory system access
    class NeuralWorkingMemory* getWorkingMemory() const;
    class NeuralEpisodicMemory* getEpisodicMemory() const;
    class NeuralAssociativeMemory* getAssociativeMemory() const;

    // Prediction system access
    class PredictionSystem* getPredictionSystem() const;

    // Cognitive system access
    class NeuralPlanner* getPlanner() const;
    class ConceptFormation* getConceptFormation() const;
    class AttentionalSelection* getAttention() const;

    // Neuromodulation system access
    class Dopamine* getDopamine() const;
    class Curiosity* getCuriosity() const;
    class Novelty* getNovelty() const;
    class PredictionError* getPredictionError() const;

    // Development system access
    class DevelopmentSystem* getDevelopmentSystem() const;
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);

    // Plasticity system access
    class STDP* getSTDP() const;
    class Hebbian* getHebbian() const;
    class StructuralPlasticity* getStructuralPlasticity() const;

    // Spike system access
    class SpikeSystem* getSpikeSystem() const;

    // Statistics and monitoring
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;

    // Motor output
    std::unique_ptr<Action> produceAction() override;

    // Neuromodulation
    void applyNeuromodulation(const Neuromodulator& signal) override;

    // Plasticity
    void updatePlasticity() override;

    // Development
    void develop() override;

    // Reset and lifecycle
    void reset() override;
    bool save(const std::string& filepath) const override;
    bool load(const std::string& filepath) override;

    // Region management
    RegionId addRegion(const std::string& name = "") override;
    NeuralRegion* getRegion(RegionId id) override;
    const NeuralRegion* getRegion(RegionId id) const override;
    size_t getRegionCount() const override;
    std::vector<RegionId> getRegionIds() const override;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const override;

    // Inter-region connections
    void addInterRegionConnection(RegionId source, RegionId target,
                                 float weight = 0.0f, Delay delay = 1) override;
    void removeInterRegionConnection(RegionId source, RegionId target) override;

    // Integration managers
    bool setupIntegration(const Config& config);
    void cleanupIntegration();

    // Statistical analysis and debugging
    void logStatus() const override;
    std::string getIntegrationReport() const;
    bool validateSystemIntegrity() const;

    // Performance optimization hints
    void optimizeMemoryUsage();
    void enableSparseConnectivity(bool enable = true);
    void enableSIMD(bool enable = true);

private:
    // Enhanced implementation with pImpl pattern
    struct Impl;
    std::unique_ptr<Impl> pImpl_;

    // Configuration
    Config config_;

    // Integration managers
    std::unique_ptr<BrainMemoryManager> memoryManager_;
    std::unique_ptr<BrainCognitionManager> cognitionManager_;
    std::unique_ptr<BrainNeuromodulationManager> neuromodulationManager_;
    std::unique_ptr<BrainPlasticityManager> plasticityManager_;

    // Private helper methods
    bool validateConfig() const;
    void setupIntegrationSystems();
    void cleanupIntegrationSystems();
    void logConfigurationSummary() const;
};

} // namespace nlm
