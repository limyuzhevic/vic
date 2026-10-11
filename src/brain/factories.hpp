// Factory implementations for creating brain components
// 
// This file provides concrete factory classes for creating
// brain components following the dependency inversion principle.

#ifndef BRAIN_FACTORIES_HPP
#define BRAIN_FACTORIES_HPP

#include "BrainAbstractions.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include "NeuralRegion.hpp"
#include "NeuralPopulation.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../environment/Environment.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Brain component factory implementation
class DefaultBrainComponentFactory : public BrainComponentFactory {
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<RandomGenerator> rng_;
    std::shared_ptr<SimulationClock> clock_;
    std::shared_ptr<Logger> logger_;
    
    // Default parameters for component creation
    struct NeuronParameters {
        NeuronType type = NeuronType::Internal;
        RegionId regionId = INVALID_REGION_ID;
        PopulationId populationId = INVALID_POPULATION_ID;
        float restingPotential = -70.0f;
        float threshold = -55.0f;
        float leakConductance = 10.0f;
        uint32_t refractoryPeriod = 5;
    };
    
    struct SynapseParameters {
        float weight = 1.0f;
        Delay delay = 1;
        SynapseType type = SynapseType::Excitatory;
        bool enablePlasticity = true;
        PlasticityFlags plasticityFlags;
    };
    
public:
    DefaultBrainComponentFactory(
        std::shared_ptr<Config> config,
        std::shared_ptr<RandomGenerator> rng,
        std::shared_ptr<SimulationClock> clock,
        std::shared_ptr<Logger> logger
    ) : config_(config), rng_(rng), clock_(clock), logger_(logger) {}
    
    // BrainUnit factory method
    std::unique_ptr<BrainUnit> createBrainUnit(const std::string& type) override {
        if (type == "Neuron") {
            // Create default neuron
            NeuronId id = generateNeuronId();
            return createNeuron(id);
        } else if (type == "Synapse") {
            return createSynapse();
        } else {
            logger_->error("Unknown brain unit type: " + type);
            return nullptr;
        }
    }
    
    // Neuron factory with configurable parameters
    std::unique_ptr<Neuron> createNeuron(NeuronId id) override {
        auto neuron = std::make_unique<Neuron>(id);
        
        // Configure with default parameters
        neuron->setType(NeuronType::Internal);
        neuron->setRestingPotential(-70.0f);
        neuron->setThreshold(-55.0f);
        neuron->setLeakConductance(10.0f);
        neuron->setRefractoryPeriod(5);
        
        // Initialize with random parameters if RNG is available
        if (rng_) {
            neuron->initializeRandom(*rng_);
        }
        
        return neuron;
    }
    
    // Synapse factory with configurable parameters
    std::unique_ptr<Synapse> createSynapse() override {
        auto synapse = std::make_unique<Synapse>();
        
        // Configure with default parameters
        SynapseParameters params;
        synapse->configure(params);
        
        return synapse;
    }
    
    // NeuralRegion factory
    std::unique_ptr<NeuralRegion> createRegion(const std::string& name = "") override {
        RegionId id = generateRegionId();
        auto region = std::make_unique<NeuralRegion>(id, name);
        
        // Create default populations
        createDefaultPopulations(*region);
        
        return region;
    }
    
    // NeuralPopulation factory
    std::unique_ptr<NeuralPopulation> createPopulation() override {
        PopulationId id = generatePopulationId();
        auto population = std::make_unique<NeuralPopulation>(id);
        
        // Configure population parameters
        population->configure(neuronParameters_);
        
        return population;
    }
    
    // Plasticity system factories
    std::unique_ptr<IPlasticitySystem> createSTDP() {
        auto stdp = std::make_unique<STDP>();
        
        // Configure STDP with parameters from config
        float ltpWeight = config_->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config_->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config_->getOr<float>("stdp_tau", 20.0f);
        
        stdp->configure(ltpWeight, ltdWeight, tau);
        return stdp;
    }
    
    std::unique_ptr<IPlasticitySystem> createHebbian() {
        auto hebbian = std::make_unique<Hebbian>();
        
        // Configure Hebbian learning
        float learningRate = config_->getOr<float>("hebbian_learning_rate", 0.01f);
        hebbian->configure(learningRate);
        
        return hebbian;
    }
    
    std::unique_ptr<IPlasticitySystem> createStructuralPlasticity() {
        auto structural = std::make_unique<StructuralPlasticity>();
        
        // Configure structural plasticity
        float synaptogenesisRate = config_->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config_->getOr<float>("pruning_rate", 0.00001f);
        
        structural->setSynaptogenesisRate(synaptogenesisRate);
        structural->setPruningRate(pruningRate);
        
        return structural;
    }
    
    // Neuromodulation system factories
    std::unique_ptr<INeuromodulationSystem> createDopamine() {
        auto dopamine = std::make_unique<Dopamine>();
        
        // Configure dopamine system
        float baselineLevel = config_->getOr<float>("dopamine_baseline", 0.1f);
        float responseAmplitude = config_->getOr<float>("dopamine_amplitude", 0.5f);
        
        dopamine->configure(baselineLevel, responseAmplitude);
        return dopamine;
    }
    
    std::unique_ptr<INeuromodulationSystem> createCuriosity() {
        auto curiosity = std::make_unique<Curiosity>();
        
        // Configure curiosity system
        float sensitivity = config_->getOr<float>("curiosity_sensitivity", 0.5f);
        float explorationBias = config_->getOr<float>("curiosity_bias", 0.3f);
        
        curiosity->configure(sensitivity, explorationBias);
        return curiosity;
    }
    
    std::unique_ptr<INeuromodulationSystem> createNovelty() {
        auto novelty = std::make_unique<Novelty>();
        
        // Configure novelty detection
        float threshold = config_->getOr<float>("novelty_threshold", 0.7f);
        float decayRate = config_->getOr<float>("novelty_decay", 0.01f);
        
        novelty->configure(threshold, decayRate);
        return novelty;
    }
    
    std::unique_ptr<INeuromodulationSystem> createPredictionError() {
        auto predictionError = std::make_unique<PredictionError>();
        
        // Configure prediction error system
        float learningRate = config_->getOr<float>("prediction_error_learning_rate", 0.01f);
        float errorVariance = config_->getOr<float>("prediction_error_variance", 0.1f);
        
        predictionError->configure(learningRate, errorVariance);
        return predictionError;
    }
    
    // Memory system factories
    std::unique_ptr<IMemorySystem> createWorkingMemory() {
        auto workingMemory = std::make_unique<NeuralWorkingMemory>();
        
        // Configure working memory
        size_t capacity = config_->getOr<size_t>("working_memory_capacity", 100);
        float decayRate = config_->getOr<float>("working_memory_decay", 0.01f);
        
        workingMemory->initialize(this, capacity, decayRate);
        return workingMemory;
    }
    
    std::unique_ptr<IMemorySystem> createEpisodicMemory() {
        auto episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        
        // Configure episodic memory
        size_t maxEpisodes = config_->getOr<size_t>("episodic_memory_capacity", 1000);
        float consolidationThreshold = config_->getOr<float>("consolidation_threshold", 0.8f);
        
        episodicMemory->initialize(this, maxEpisodes, consolidationThreshold);
        return episodicMemory;
    }
    
    std::unique_ptr<IMemorySystem> createAssociativeMemory() {
        auto associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        // Configure associative memory
        size_t maxAssociations = config_->getOr<size_t>("associative_memory_capacity", 500);
        float completionThreshold = config_->getOr<float>("completion_threshold", 0.6f);
        
        associativeMemory->initialize(this, maxAssociations, completionThreshold);
        return associativeMemory;
    }
    
    // Cognitive system factories
    std::unique_ptr<ICognitionSystem> createNeuralPlanner() {
        auto planner = std::make_unique<NeuralPlanner>();
        
        // Configure neural planner
        size_t planningDepth = config_->getOr<size_t>("planning_depth", 5);
        float uncertaintyTolerance = config_->getOr<float>("uncertainty_tolerance", 0.5f);
        
        planner->initialize(this, planningDepth, uncertaintyTolerance);
        return planner;
    }
    
    std::unique_ptr<ICognitionSystem> createConceptFormation() {
        auto conceptFormation = std::make_unique<ConceptFormation>();
        
        // Configure concept formation
        float abstractionThreshold = config_->getOr<float>("abstraction_threshold", 0.7f);
        size_t maxConcepts = config_->getOr<size_t>("max_concepts", 100);
        
        conceptFormation->initialize(this, abstractionThreshold, maxConcepts);
        return conceptFormation;
    }
    
    // Sensory system factories
    std::unique_ptr<ISensorySystem> createSensoryInput() {
        auto sensoryInput = std::make_unique<SensoryInput>();
        
        // Configure sensory input
        float inputRange = config_->getOr<float>("input_range", 10.0f);
        float noiseLevel = config_->getOr<float>("input_noise", 0.1f);
        
        sensoryInput->configure(inputRange, noiseLevel);
        return sensoryInput;
    }
    
    // Motor system factories
    std::unique_ptr<IMotorSystem> createMotorSystem() {
        auto motorSystem = std::make_unique<MotorSystem>();
        
        // Configure motor system
        float actionSelectionThreshold = config_->getOr<float>("action_threshold", 0.5f);
        float explorationRate = config_->getOr<float>("exploration_rate", 0.1f);
        
        motorSystem->configure(actionSelectionThreshold, explorationRate);
        return motorSystem;
    }
    
    // Prediction system factories
    std::unique_ptr<IPredictionSystem> createPredictionSystem() {
        auto predictionSystem = std::make_unique<PredictionSystem>();
        
        // Configure prediction system
        float predictionAccuracy = config_->getOr<float>("prediction_accuracy", 0.8f);
        float errorSensitivity = config_->getOr<float>("error_sensitivity", 0.5f);
        
        predictionSystem->configure(predictionAccuracy, errorSensitivity);
        return predictionSystem;
    }
    
    // Environment factory
    std::unique_ptr<IEnvironment> createEnvironment() {
        auto environment = std::make_unique<Environment>();
        
        // Configure environment
        size_t width = config_->getOr<size_t>("environment_width", 10);
        size_t height = config_->getOr<size_t>("environment_height", 10);
        std::string type = config_->getOr<std::string>("environment_type", "grid");
        
        environment->initialize(this, width, height, type);
        return environment;
    }
    
    // Development system factory
    std::unique_ptr<IDevelopmentSystem> createDevelopmentSystem() {
        auto developmentSystem = std::make_unique<DevelopmentSystem>();
        
        // Configure development system
        float synaptogenesisRate = config_->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config_->getOr<float>("pruning_rate", 0.00001f);
        
        developmentSystem->configure(synaptogenesisRate, pruningRate);
        return developmentSystem;
    }
    
    // Spike system factory
    std::unique_ptr<ISpikeSystem> createSpikeSystem() {
        auto spikeSystem = std::make_unique<SpikeSystem>();
        
        // Configure spike system
        size_t maxNeurons = config_->getOr<size_t>("max_neurons", 1000);
        float refractoryPeriod = config_->getOr<float>("refractory_period", 5.0f);
        
        spikeSystem->initialize(this, maxNeurons, refractoryPeriod);
        return spikeSystem;
    }
    
    // Complete brain integration factory
    std::unique_ptr<IBrainImplementation> createBrainImplementation(
        std::shared_ptr<Config> config
    ) {
        auto brainImpl = std::make_unique<DefaultBrainImplementation>(config);
        return brainImpl;
    }
    
    // Configuration helper methods
    void setNeuronParameters(const NeuronParameters& params) {
        neuronParameters_ = params;
    }
    
    void setSynapseParameters(const SynapseParameters& params) {
        synapseParameters_ = params;
    }
    
    // ID generation utilities
    NeuronId generateNeuronId() {
        static std::atomic<NeuronId::Index> idCounter{1};
        return NeuronId(idCounter++);
    }
    
    RegionId generateRegionId() {
        static std::atomic<RegionId::Index> idCounter{1};
        return RegionId(idCounter++);
    }
    
    PopulationId generatePopulationId() {
        static std::atomic<PopulationId::Index> idCounter{1};
        return PopulationId(idCounter++);
    }
    
private:
    // Default parameters for component creation
    NeuronParameters neuronParameters_;
    SynapseParameters synapseParameters_;
    
    // Create default populations in a region
    void createDefaultPopulations(NeuralRegion& region) {
        // Create sensory, internal, and motor populations
        size_t neuronsPerRegion = 100;  // Default
        
        auto sensoryPopId = region.addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
        auto internalPopId = region.addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
        auto motorPopId = region.addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
        
        // Store population IDs for reference
        // (Implementation would depend on NeuralRegion interface)
    }
};

// Component registry for dependency injection
class ComponentRegistry {
public:
    static ComponentRegistry& getInstance() {
        static ComponentRegistry instance;
        return instance;
    }
    
    // Register a factory for a specific component type
    template<typename T>
    void registerFactory(std::function<std::unique_ptr<T>()> factory) {
        factories_[typeid(T).name()] = [factory]() -> std::unique_ptr<void> {
            return std::unique_ptr<void>(factory().release());
        };
    }
    
    // Create a component of the specified type
    template<typename T>
    std::unique_ptr<T> create() {
        auto it = factories_.find(typeid(T).name());
        if (it != factories_.end()) {
            auto ptr = std::unique_ptr<T>(static_cast<T*>(it->second().release()));
            return ptr;
        }
        return nullptr;
    }
    
    // Unregister a factory
    template<typename T>
    void unregister() {
        factories_.erase(typeid(T).name());
    }
    
private:
    ComponentRegistry() = default;
    ~ComponentRegistry() = default;
    ComponentRegistry(const ComponentRegistry&) = delete;
    ComponentRegistry& operator=(const ComponentRegistry&) = delete;
    
    std::unordered_map<std::string, std::function<std::unique_ptr<void>()>> factories_;
};

// Convenience function to create a fully configured brain
std::unique_ptr<IBrainImplementation> createIntegratedBrain(
    std::shared_ptr<Config> config,
    std::shared_ptr<RandomGenerator> rng,
    std::shared_ptr<SimulationClock> clock,
    std::shared_ptr<Logger> logger
) {
    // Create factory with dependencies
    auto factory = std::make_unique<DefaultBrainComponentFactory>(config, rng, clock, logger);
    
    // Create brain implementation using the factory
    auto brainImpl = factory->createBrainImplementation(config);
    
    return brainImpl;
}

} // namespace nlm

#endif // BRAIN_FACTORIES_HPP