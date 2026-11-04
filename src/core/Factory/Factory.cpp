#include "Factory/Factory.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Reward.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/Memory.hpp"
#include "development/DevelopmentSystem.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "development/Maturation.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/Attention.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/SelfModel.hpp"
#include "sensory/Vision.hpp"
#include "sensory/Audio.hpp"
#include "sensory/InternalSignals.hpp"
#include "motor/MotorSystem.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "environment/Observation.hpp"
#include "experiments/Experiment.hpp"
#include "experiments/Metrics.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "performance/CheckpointSystem.hpp"
#include <memory>

namespace nlm {

// Factory implementation
DefaultFactory::DefaultFactory() = default;

// NLMPFactory implementation
NLMPFactory::NLMPFactory() : factory_(std::make_unique<DefaultFactory>()) {}

void NLMPFactory::registerDefaults() {
    // Core components with dependencies
    factory_->registerComponent("Config", []() -> std::shared_ptr<void> {
        auto config = std::make_shared<Config>();
        config->set("random_seed", 42LL, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", 1000LL, ConfigSource::Default);
        config->set("region_count", 1LL, ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        return std::shared_ptr<void>(config);
    });
    
    factory_->registerComponent("Logger", []() -> std::shared_ptr<void> {
        auto logger = std::make_shared<Logger>();
        logger->setLevel(LogLevel::Info);
        return std::shared_ptr<void>(logger);
    });
    
    factory_->registerComponent("RandomGenerator", []() -> std::shared_ptr<void> {
        return std::shared_ptr<void>(new RandomGenerator());
    });
    
    factory_->registerComponent("SimulationClock", []() -> std::shared_ptr<void> {
        return std::shared_ptr<void>(new SimulationClock(0.001));
    });
    
    // Brain with dependencies
    factory_->registerComponent("Brain", []() -> std::shared_ptr<void> {
        auto config = std::make_shared<Config>();
        auto logger = std::make_shared<Logger>();
        auto randomGen = std::make_shared<RandomGenerator>();
        auto clock = std::make_shared<SimulationClock>(0.001);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return std::shared_ptr<void>(brain);
    });
    
    // Plasticity rules
    factory_->registerComponent("STDP", []() -> std::shared_ptr<void> {
        auto plasticity = std::make_shared<STDP>();
        plasticity->setLearningRate(0.02f);
        return std::shared_ptr<void>(plasticity);
    });
    
    factory_->registerComponent("Hebbian", []() -> std::shared_ptr<void> {
        auto plasticity = std::make_shared<Hebbian>();
        plasticity->setLearningRate(0.01f);
        return std::shared_ptr<void>(plasticity);
    });
    
    factory_->registerComponent("StructuralPlasticity", []() -> std::shared_ptr<void> {
        auto plasticity = std::make_shared<StructuralPlasticity>();
        plasticity->setRate(0.0001f);
        return std::shared_ptr<void>(plasticity);
    });
    
    // Neuromodulators
    factory_->registerComponent("Dopamine", []() -> std::shared_ptr<void> {
        auto neuromod = std::make_shared<Dopamine>();
        neuromod->setBaseline(0.0f);
        neuromod->setPeak(1.0f);
        return std::shared_ptr<void>(neuromod);
    });
    
    factory_->registerComponent("Curiosity", []() -> std::shared_ptr<void> {
        auto neuromod = std::make_shared<Curiosity>();
        neuromod->setBaseline(0.0f);
        neuromod->setThreshold(0.5f);
        return std::shared_ptr<void>(neuromod);
    });
    
    factory_->registerComponent("Novelty", []() -> std::shared_ptr<void> {
        auto neuromod = std::make_shared<Novelty>();
        neuromod->setSensitivity(0.5f);
        neuromod->setMemorySize(1000);
        return std::shared_ptr<void>(neuromod);
    });
    
    factory_->registerComponent("PredictionError", []() -> std::shared_ptr<void> {
        auto neuromod = std::make_shared<PredictionError>();
        neuromod->setLearningRate(0.01f);
        return std::shared_ptr<void>(neuromod);
    });
    
    factory_->registerComponent("Reward", []() -> std::shared_ptr<void> {
        auto neuromod = std::make_shared<Reward>();
        neuromod->setSensitivity(1.0f);
        neuromod->setDecay(0.99f);
        return std::shared_ptr<void>(neuromod);
    });
    
    // Memory systems
    factory_->registerComponent("WorkingMemory", []() -> std::shared_ptr<void> {
        auto memory = std::make_shared<WorkingMemory>();
        memory->setCapacity(100);
        memory->setDecayRate(0.95f);
        return std::shared_ptr<void>(memory);
    });
    
    factory_->registerComponent("EpisodicMemory", []() -> std::shared_ptr<void> {
        auto memory = std::make_shared<EpisodicMemory>();
        memory->setMaxEpisodes(1000);
        memory->setReplayProbability(0.1f);
        return std::shared_ptr<void>(memory);
    });
    
    // Development systems
    factory_->registerComponent("DevelopmentSystem", []() -> std::shared_ptr<void> {
        auto dev = std::make_shared<DevelopmentSystem>();
        dev->setMaturationRate(0.001f);
        return std::shared_ptr<void>(dev);
    });
    
    factory_->registerComponent("Synaptogenesis", []() -> std::shared_ptr<void> {
        auto dev = std::make_shared<Synaptogenesis>();
        dev->setRate(0.0001f);
        return std::shared_ptr<void>(dev);
    });
    
    factory_->registerComponent("Pruning", []() -> std::shared_ptr<void> {
        auto dev = std::make_shared<Pruning>();
        dev->setRate(0.00001f);
        return std::shared_ptr<void>(dev);
    });
    
    factory_->registerComponent("Maturation", []() -> std::shared_ptr<void> {
        auto dev = std::make_shared<Maturation>();
        dev->setMaturationRate(0.5f);
        return std::shared_ptr<void>(dev);
    });
    
    // Prediction systems
    factory_->registerComponent("NeuralPrediction", []() -> std::shared_ptr<void> {
        auto prediction = std::make_shared<NeuralPrediction>();
        prediction->setPredictionAccuracy(0.8f);
        return std::shared_ptr<void>(prediction);
    });
    
    factory_->registerComponent("PredictionSystem", []() -> std::shared_ptr<void> {
        auto prediction = std::make_shared<PredictionSystem>();
        prediction->setPredictionHorizon(10.0f);
        prediction->setConfidenceThreshold(0.7f);
        return std::shared_ptr<void>(prediction);
    });
    
    // Cognition systems
    factory_->registerComponent("Attention", []() -> std::shared_ptr<void> {
        auto cognition = std::make_shared<Attention>();
        cognition->setSelectionStrength(0.5f);
        return std::shared_ptr<void>(cognition);
    });
    
    factory_->registerComponent("ConceptFormation", []() -> std::shared_ptr<void> {
        auto cognition = std::make_shared<ConceptFormation>();
        cognition->setSimilarityThreshold(0.7f);
        cognition->setLearningRate(0.01f);
        return std::shared_ptr<void>(cognition);
    });
    
    factory_->registerComponent("NeuralPlanner", []() -> std::shared_ptr<void> {
        auto cognition = std::make_shared<NeuralPlanner>();
        cognition->setActionSelectionStrength(0.8f);
        return std::shared_ptr<void>(cognition);
    });
    
    factory_->registerComponent("SelfModel", []() -> std::shared_ptr<void> {
        auto cognition = std::make_shared<SelfModel>();
        cognition->setSelfAwareness(0.5f);
        return std::shared_ptr<void>(cognition);
    });
    
    // Sensory systems
    factory_->registerComponent("Vision", []() -> std::shared_ptr<void> {
        auto sensory = std::make_shared<Vision>(nullptr);
        sensory->setResolution(640);
        sensory->setFieldOfView(90.0f);
        return std::shared_ptr<void>(sensory);
    });
    
    factory_->registerComponent("Audio", []() -> std::shared_ptr<void> {
        auto sensory = std::make_shared<Audio>(nullptr);
        sensory->setFrequencyRange(20.0f, 20000.0f);
        sensory->setVolumeThreshold(0.1f);
        return std::shared_ptr<void>(sensory);
    });
    
    factory_->registerComponent("InternalSignals", []() -> std::shared_ptr<void> {
        auto sensory = std::make_shared<InternalSignals>();
        sensory->setSensoryUpdateRate(0.1f);
        return std::shared_ptr<void>(sensory);
    });
    
    // Motor system
    factory_->registerComponent("MotorSystem", []() -> std::shared_ptr<void> {
        auto motor = std::make_shared<MotorSystem>();
        motor->setActionSelectionStrength(0.8f);
        motor->setLearningRate(0.01f);
        return std::shared_ptr<void>(motor);
    });
    
    factory_->registerComponent("Action", []() -> std::shared_ptr<void> {
        auto action = std::make_shared<Action>();
        action->setActionDuration(1.0f);
        action->setTerminationThreshold(0.5f);
        return std::shared_ptr<void>(action);
    });
    
    // Environment
    factory_->registerComponent("Environment", []() -> std::shared_ptr<void> {
        auto env = std::make_shared<Environment>(nullptr);
        env->setWidth(10);
        env->setHeight(10);
        env->setResetOnEpisodeEnd(true);
        return std::shared_ptr<void>(env);
    });
    
    factory_->registerComponent("Observation", []() -> std::shared_ptr<void> {
        auto observation = std::make_shared<Observation>();
        observation->setObservationHistorySize(100);
        observation->setFeatureExtraction(true);
        return std::shared_ptr<void>(observation);
    });
    
    // Experiments
    factory_->registerComponent("Experiment", []() -> std::shared_ptr<void> {
        auto experiment = std::make_shared<Experiment>();
        experiment->setSeed(42);
        return std::shared_ptr<void>(experiment);
    });
    
    factory_->registerComponent("Phase6IntegratedExperiment", []() -> std::shared_ptr<void> {
        auto experiment = std::make_shared<Phase6IntegratedExperiment>();
        return std::shared_ptr<void>(experiment);
    });
    
    // Performance systems
    factory_->registerComponent("CheckpointSystem", []() -> std::shared_ptr<void> {
        auto checkpoint = std::make_shared<CheckpointSystem>();
        checkpoint->setCheckpointInterval(100);
        checkpoint->setMaxCheckpoints(10);
        return std::shared_ptr<void>(checkpoint);
    });
    
    factory_->registerComponent("Metrics", []() -> std::shared_ptr<void> {
        auto metrics = std::make_shared<Metrics>();
        metrics->setMetricCollectionInterval(0.1f);
        metrics->setMetricHistorySize(1000);
        return std::shared_ptr<void>(metrics);
    });
    
    factory_->registerComponent("ExperimentRunner", []() -> std::shared_ptr<void> {
        auto runner = std::make_shared<ExperimentRunner>();
        runner->setMaxConcurrentExperiments(4);
        runner->setExperimentTimeout(3600.0f); // 1 hour
        return std::shared_ptr<void>(runner);
    });
}

// NLMPFactory component creation methods
std::shared_ptr<void> NLMPFactory::createComponent(const std::string& name, const Config& config) {
    // For now, use the default factory
    return factory_->create(name);
}

// Convenience functions implementation
namespace FactoryUtils {

std::shared_ptr<Brain> createBrain() {
    auto& container = NLMContainer::getInstance();
    auto config = std::make_shared<Config>();
    config->set("random_seed", 42LL, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", 1000LL, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    return brain;
}

std::shared_ptr<Brain> createBrain(const Config& config) {
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    return brain;
}

std::shared_ptr<Logger> getLogger() {
    auto& container = NLMContainer::getInstance();
    return container.getComponent<Logger>("Logger");
}

std::shared_ptr<Config> getConfig() {
    auto& container = NLMContainer::getInstance();
    return container.getComponent<Config>("Config");
}

} // namespace FactoryUtils

} // namespace nlm