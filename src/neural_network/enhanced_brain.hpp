#pragma once

#include "../brain/Brain.hpp"
#include "../neural_network/neurons/neuron_base.hpp"
#include "../neural_network/neurons/lif_neuron.hpp"
#include "../neural_network/neurons/adaptive_neuron.hpp"
#include "../neural_network/neurons/stochastic_neuron.hpp"
#include "../neural_network/neurons/dendrite.hpp"
#include "../neural_network/synapses/synapse_base.hpp"
#include "../neural_network/synapses/stdp.hpp"
#include "../neural_network/synapses/hebbian.hpp"
#include "../neural_network/synapses/neuromodulated_synapse.hpp"
#include "../neural_network/synapses/structural_plasticity.hpp"
#include "../neural_network/circuits/network_topology.hpp"
#include "../neural_network/circuits/connection_patterns.hpp"
#include "../neural_network/circuits/network_initializer.hpp"
#include "../neural_network/performance/simd_optimization.hpp"
#include "../neural_network/performance/memory_pool.hpp"
#include "../neural_network/performance/event_queue.hpp"
#include "../neural_network/research/meta_plasticity.hpp"
#include "../neural_network/research/homeostatic_plasticity.hpp"
#include "../neural_network/research/specialized_populations.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace nlm {

/**
 * Enhanced Brain - Main neural system coordinator with advanced features
 * 
 * Extends the basic Brain class with comprehensive neural network capabilities:
 * - Enhanced neuron models (LIF, Adaptive, Stochastic, Dendritic)
 * - Advanced plasticity rules (STDP, Hebbian, Neuromodulated, Structural)
 * - Neural circuit architecture (Topology, Connection Patterns, Initialization)
 * - Performance optimizations (SIMD, Memory Pool, Event-Driven Processing)
 * - Research extensions (Meta-plasticity, Homeostatic Plasticity, Specialized Populations)
 * 
 * Maintains full backward compatibility with existing Brain interface
 */
class EnhancedBrain : public Brain {
public:
    EnhancedBrain(std::shared_ptr<Config> config);
    ~EnhancedBrain() override;

    // ===== INITIALIZATION =====

    // Initialize brain with configuration
    bool initialize() override;

    // Enable/disable advanced features
    void enableEnhancedNeuronModels(bool enable) { enhancedNeuronModelsEnabled_ = enable; }
    void enableAdvancedPlasticity(bool enable) { advancedPlasticityEnabled_ = enable; }
    void enableNeuralCircuitArchitecture(bool enable) { neuralCircuitArchitectureEnabled_ = enable; }
    void enablePerformanceOptimizations(bool enable) { performanceOptimizationsEnabled_ = enable; }
    void enableResearchExtensions(bool enable) { researchExtensionsEnabled_ = enable; }

    // ===== ENHANCED NEURON MODELS =====

    // Enable specific enhanced neuron models
    void enableLIFNeurons(bool enable) { lifNeuronsEnabled_ = enable; }
    void enableAdaptiveNeurons(bool enable) { adaptiveNeuronsEnabled_ = enable; }
    void enableStochasticNeurons(bool enable) { stochasticNeuronsEnabled_ = enable; }
    void enableDendriticProcessing(bool enable) { dendriticProcessingEnabled_ = enable; }

    // Set neuron model configuration
    void setLIFNeuronParameters(const LIFNeuron::LIFParameters& params) {
        lifNeuronParams_ = params;
    }
    void setAdaptiveNeuronParameters(const AdaptiveNeuron::AdExParameters& params) {
        adaptiveNeuronParams_ = params;
    }
    void setStochasticNeuronParameters(const StochasticNeuron::StochasticParameters& params) {
        stochasticNeuronParams_ = params;
    }
    void setDendriticParameters(const Dendrite::DendriticParameters& params) {
        dendriticParams_ = params;
    }

    // ===== ADVANCED PLASTICITY =====

    // Enable specific plasticity rules
    void enableAdvancedSTDP(bool enable) { advancedSTDPEnabled_ = enable; }
    void enableAdvancedHebbian(bool enable) { advancedHebbianEnabled_ = enable; }
    void enableNeuromodulatedPlasticity(bool enable) { neuromodulatedPlasticityEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }

    // Set plasticity rule configuration
    void setAdvancedSTDPParameters(const AdvancedSTDP::TripletSTDPParams& params) {
        advancedSTDPParams_ = params;
    }
    void setAdvancedHebbianParameters(const AdvancedHebbian::HebbianParameters& params) {
        advancedHebbianParams_ = params;
    }

    // ===== NEURAL CIRCUIT ARCHITECTURE =====

    // Set circuit topology
    void setCircuitTopology(const NeuralCircuitTopology::GridTopology& gridParams) {
        circuitTopology_.setGridTopology(gridParams);
    }
    void setCircuitTopology(const NeuralCircuitTopology::MapTopology& mapParams) {
        circuitTopology_.setMapTopology(mapParams);
    }
    void setCircuitTopology(const NeuralCircuitTopology::HierarchicalTopology& hierarchicalParams) {
        circuitTopology_.setHierarchicalTopology(hierarchicalParams);
    }
    void setCircuitTopology(const NeuralCircuitTopology::SmallWorldTopology& smallWorldParams) {
        circuitTopology_.setSmallWorldTopology(smallWorldParams);
    }

    // Set connection patterns
    void setConnectionPattern(const ConnectionPatterns::LocalConnectivity& localParams) {
        connectionPatterns_.setLocalConnectivity(localParams);
    }
    void setConnectionPattern(const ConnectionPatterns::GlobalConnectivity& globalParams) {
        connectionPatterns_.setGlobalConnectivity(globalParams);
    }
    void setConnectionPattern(const ConnectionPatterns::DistanceDependentConnectivity& distanceParams) {
        connectionPatterns_.setDistanceDependentConnectivity(distanceParams);
    }
    void setConnectionPattern(const ConnectionPatterns::ActivityDependentConnectivity& activityParams) {
        connectionPatterns_.setActivityDependentConnectivity(activityParams);
    }

    // ===== PERFORMANCE OPTIMIZATIONS =====

    // Enable performance optimizations
    void enableSIMDVectorization(bool enable) { simdVectorizationEnabled_ = enable; }
    void enableMemoryPooling(bool enable) { memoryPoolingEnabled_ = enable; }
    void enableEventDrivenProcessing(bool enable) { eventDrivenProcessingEnabled_ = enable; }

    // Set performance parameters
    void setSIMDVectorSize(size_t size) { simdVectorSize_ = size; }
    void setMemoryPoolSize(size_t neurons, size_t synapses) {
        memoryPoolNeurons_ = neurons;
        memoryPoolSynapses_ = synapses;
    }
    void setEventDrivenTimeBucketSize(TimestepDuration bucketSize) {
        eventDrivenTimeBucketSize_ = bucketSize;
    }

    // ===== RESEARCH EXTENSIONS =====

    // Enable research extensions
    void enableMetaPlasticity(bool enable) { metaPlasticityEnabled_ = enable; }
    void enableHomeostaticPlasticity(bool enable) { homeostaticPlasticityEnabled_ = enable; }
    void enableSpecializedPopulations(bool enable) { specializedPopulationsEnabled_ = enable; }

    // Set research extension parameters
    void setMetaPlasticityParameters(const MetaPlasticity::AdaptiveMetaPlasticity& params) {
        metaPlasticityParams_ = params;
    }
    void setHomeostaticPlasticityParameters(const HomeostaticPlasticity::SynapticScaling& params) {
        homeostaticPlasticityParams_ = params;
    }

    // ===== CONFIGURATION =====

    // Load configuration from config file
    bool loadConfig(const std::string& configFilePath);

    // Save configuration to config file
    bool saveConfig(const std::string& configFilePath) const;

    // ===== MAIN SIMULATION STEP =====

    // Main simulation step (overrides Brain::step)
    void step(SimulationStep currentStep) override;

    // Main simulation step with time (overrides Brain::step)
    void step(SimulationStep currentStep, Timestamp currentTime) override;

    // ===== NEURAL NETWORK SPECIFIC METHODS =====

    // Generate neural network with specified configuration
    bool generateNeuralNetwork(const std::string& networkType,
                              const std::map<std::string, float>& parameters);

    // Configure neural network for specific task
    bool configureForTask(const std::string& taskType,
                         const std::map<std::string, float>& taskParameters);

    // Get neural network statistics
    struct NeuralNetworkStats {
        // Neuron statistics
        size_t totalNeurons;
        size_t activeNeurons;
        size_t firingNeurons;
        float averageFiringRate;
        float averageMembranePotential;
        float averageThreshold;
        
        // Synapse statistics
        size_t totalSynapses;
        size_t activeSynapses;
        float averageSynapticWeight;
        float synapticWeightVariance;
        
        // Plasticity statistics
        float totalPlasticityChange;
        float averageSTDPChange;
        float averageHebbianChange;
        float averageHomeostaticChange;
        
        // Performance statistics
        double simulationEfficiency;
        double memoryUsage;
        double computationTime;
        
        // Research statistics
        float metaPlasticityLevel;
        float homeostaticPlasticityLevel;
        float networkComplexity;
        float networkStability;
    };

    NeuralNetworkStats getNeuralNetworkStats() const;

    // ===== UTILITY =====

    // Check if enhanced brain is properly initialized
    bool isInitialized() const { return isInitialized_; }

    // Get current configuration
    std::shared_ptr<const Config> getConfig() const override { return config_; }

    // ===== OVERRIDE =====

    // Override Brain methods for enhanced functionality
    void receiveSensoryInput(const class SensoryInput& input) override;
    void injectCurrent(NeuronId neuron, MembranePotential current) override;
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) override;
    SpikeSystem* getSpikeSystem() override;
    const SpikeSystem* getSpikeSystem() const override;
    STDP* getSTDP() override;
    Hebbian* getHebbian() override;
    StructuralPlasticity* getStructuralPlasticity() override;
    void applyNeuromodulation(const class Neuromodulator& signal) override;
    void updatePlasticity() override;
    void develop() override;
    void reset() override;
    bool save(const std::string& filepath) const override;
    bool load(const std::string& filepath) override;

    // Get enhanced neural network components
    std::shared_ptr<NeuralCircuitTopology> getNeuralCircuitTopology() const { return circuitTopology_; }
    std::shared_ptr<ConnectionPatterns> getConnectionPatterns() const { return connectionPatterns_; }
    std::shared_ptr<NetworkInitializer> getNetworkInitializer() const { return networkInitializer_; }
    std::shared_ptr<SIMDVectorization> getSIMDVectorization() const { return simdVectorization_; }
    std::shared_ptr<MemoryPool> getMemoryPool() const { return memoryPool_; }
    std::shared_ptr<EventDrivenProcessing> getEventDrivenProcessing() const { return eventDrivenProcessing_; }
    std::shared_ptr<MetaPlasticity> getMetaPlasticity() const { return metaPlasticity_; }
    std::shared_ptr<HomeostaticPlasticity> getHomeostaticPlasticity() const { return homeostaticPlasticity_; }
    std::shared_ptr<SpecializedNeuronPopulations> getSpecializedNeuronPopulations() const { return specializedPopulations_; }

private:
    // Enhanced neural network components
    std::shared_ptr<NeuralCircuitTopology> circuitTopology_;
    std::shared_ptr<ConnectionPatterns> connectionPatterns_;
    std::shared_ptr<NetworkInitializer> networkInitializer_;
    std::shared_ptr<SIMDVectorization> simdVectorization_;
    std::shared_ptr<MemoryPool> memoryPool_;
    std::shared_ptr<EventDrivenProcessing> eventDrivenProcessing_;
    std::shared_ptr<MetaPlasticity> metaPlasticity_;
    std::shared_ptr<HomeostaticPlasticity> homeostaticPlasticity_;
    std::shared_ptr<SpecializedNeuronPopulations> specializedPopulations_;

    // Neuron models
    std::shared_ptr<LIFNeuron> lifNeuron_;
    std::shared_ptr<AdaptiveNeuron> adaptiveNeuron_;
    std::shared_ptr<StochasticNeuron> stochasticNeuron_;
    std::shared_ptr<Dendrite> dendrite_;

    // Plasticity rules
    std::shared_ptr<AdvancedSTDP> advancedSTDP_;
    std::shared_ptr<AdvancedHebbian> advancedHebbian_;
    std::shared_ptr<NeuromodulatedSynapse> neuromodulatedSynapse_;
    std::shared_ptr<StructuralPlasticity> structuralPlasticity_;

    // Configuration
    std::shared_ptr<Config> config_;
    bool isInitialized_;

    // Feature flags
    bool enhancedNeuronModelsEnabled_;
    bool advancedPlasticityEnabled_;
    bool neuralCircuitArchitectureEnabled_;
    bool performanceOptimizationsEnabled_;
    bool researchExtensionsEnabled_;

    // Neuron model flags
    bool lifNeuronsEnabled_;
    bool adaptiveNeuronsEnabled_;
    bool stochasticNeuronsEnabled_;
    bool dendriticProcessingEnabled_;

    // Plasticity rule flags
    bool advancedSTDPEnabled_;
    bool advancedHebbianEnabled_;
    bool neuromodulatedPlasticityEnabled_;
    bool structuralPlasticityEnabled_;

    // Performance optimization flags
    bool simdVectorizationEnabled_;
    bool memoryPoolingEnabled_;
    bool eventDrivenProcessingEnabled_;

    // Research extension flags
    bool metaPlasticityEnabled_;
    bool homeostaticPlasticityEnabled_;
    bool specializedPopulationsEnabled_;

    // Parameters
    LIFNeuron::LIFParameters lifNeuronParams_;
    AdaptiveNeuron::AdExParameters adaptiveNeuronParams_;
    StochasticNeuron::StochasticParameters stochasticNeuronParams_;
    Dendrite::DendriticParameters dendriticParams_;

    AdvancedSTDP::TripletSTDPParams advancedSTDPParams_;
    AdvancedHebbian::HebbianParameters advancedHebbianParams_;

    MetaPlasticity::AdaptiveMetaPlasticity metaPlasticityParams_;
    HomeostaticPlasticity::SynapticScaling homeostaticPlasticityParams_;

    // Performance parameters
    size_t simdVectorSize_;
    size_t memoryPoolNeurons_;
    size_t memoryPoolSynapses_;
    TimestepDuration eventDrivenTimeBucketSize_;

    // Statistics
    NeuralNetworkStats currentStats_;

    // Helper methods
    void initializeEnhancedNeuronModels();
    void initializeAdvancedPlasticity();
    void initializeNeuralCircuitArchitecture();
    void initializePerformanceOptimizations();
    void initializeResearchExtensions();

    void updateEnhancedNeuronModels(TimestepDuration dt);
    void updateAdvancedPlasticity(TimestepDuration dt);
    void updateNeuralCircuitArchitecture(TimestepDuration dt);
    void updatePerformanceOptimizations(TimestepDuration dt);
    void updateResearchExtensions(TimestepDuration dt);

    void generateCircuitTopology();
    void generateConnectionPatterns();
    void initializeNeuralNetwork();
    void applyPerformanceOptimizations();
    void applyResearchExtensions();

    void updateNeuronModels(TimestepDuration dt);
    void updatePlasticityRules(TimestepDuration dt);
    void updateCircuitArchitecture(TimestepDuration dt);
    void updatePerformanceOptimizations(TimestepDuration dt);
    void updateResearchExtensions(TimestepDuration dt);

    void calculateNeuralNetworkStats() const;
    void updateStatistics(TimestepDuration dt);
    bool validateConfiguration() const;
    void logNeuralNetworkStatus() const;
};

} // namespace nlm
