#include "brain/plasticity/BrainPlasticity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <ctime>

namespace nlm {

BrainPlasticity::BrainPlasticity(std::shared_ptr<Config> config)
    : config_(config)
    , activeSynapses_(0)
    , averageSynapticWeight_(0.0f)
    , plasticityStrength_(1.0f)
    , learningRate_(0.01f)
    , stdpLearningRate_(0.01f)
    , hebbianLearningRate_(0.01f)
    , structuralLearningRate_(0.0001f)
    , weightChangeThreshold_(0.01f)
    , potentiationThreshold_(0.1f)
    , depressionThreshold_(-0.1f)
{
    if (!config_) {
        NLM_LOG_ERROR("Configuration is null in BrainPlasticity constructor");
    }
    
    // Initialize random generator with seed from config
    uint64_t seed = 42;  // Default seed
    if (auto seedOpt = config_->get<uint64_t>("random_seed")) {
        seed = *seedOpt;
    }
    rng_ = std::make_unique<RandomGenerator>(seed);
    
    NLM_LOG_INFO("BrainPlasticity initialized");
}

BrainPlasticity::~BrainPlasticity() = default;

bool BrainPlasticity::initialize(const std::vector<Neuron*>& neurons) {
    NLM_LOG_INFO("Initializing brain plasticity systems...");
    
    // Initialize plasticity systems
    spikeSystem_ = std::make_unique<SpikeSystem>();
    stdp_ = std::make_unique<STDP>();
    hebbian_ = std::make_unique<Hebbian>();
    structuralPlasticity_ = std::make_unique<StructuralPlasticity>();
    
    // Initialize neuromodulation systems for plasticity
    dopamine_ = std::make_unique<Dopamine>();
    curiosity_ = std::make_unique<Curiosity>();
    novelty_ = std::make_unique<Novelty>();
    
    // Initialize plasticity systems
    if (spikeSystem_) {
        spikeSystem_->initialize();
    }
    
    // Configure STDP
    if (stdp_) {
        float ltpWeight = config_->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config_->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config_->getOr<float>("stdp_tau", 20.0f);
        stdp_->configure(ltpWeight, ltdWeight, tau);
    }
    
    // Configure Hebbian
    if (hebbian_) {
        // Hebbian configuration would go here
    }
    
    // Configure structural plasticity
    if (structuralPlasticity_) {
        float synaptogenesisRate = config_->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config_->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity_->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity_->setPruningRate(pruningRate);
    }
    
    // Initialize neuromodulation systems
    if (dopamine_) {
        dopamine_->initialize(nullptr);  // Would pass brain reference
    }
    
    if (curiosity_) {
        curiosity_->initialize(nullptr);  // Would pass brain reference
    }
    
    if (novelty_) {
        novelty_->initialize(nullptr);  // Would pass brain reference
    }
    
    // Register spike handlers for plasticity
    if (spikeSystem_) {
        spikeSystem_->registerHandler([this](const DetailedSpikeEvent& event) {
            onSpikeEvent(event);
        });
        
        spikeSystem_->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
            onDelayedSpikeEvent(event);
        });
    }
    
    // Validate configuration
    if (!validatePlasticityConfig(config_)) {
        NLM_LOG_ERROR("Plasticity system configuration validation failed");
        return false;
    }
    
    // Calculate initial statistics
    calculateStatistics(neurons);
    
    NLM_LOG_INFO("Brain plasticity systems initialized");
    return true;
}

void BrainPlasticity::reset() {
    NLM_LOG_INFO("Resetting brain plasticity systems...");
    
    eligibilityTraces_.clear();
    recentPlasticityEvents_.clear();
    
    activeSynapses_ = 0;
    averageSynapticWeight_ = 0.0f;
    plasticityStrength_ = 1.0f;
    
    learningRate_ = 0.01f;
    stdpLearningRate_ = 0.01f;
    hebbianLearningRate_ = 0.01f;
    structuralLearningRate_ = 0.0001f;
    
    // Reset plasticity systems
    if (spikeSystem_) {
        spikeSystem_->reset();
    }
    
    if (stdp_) {
        stdp_->reset();
    }
    
    if (hebbian_) {
        hebbian_->reset();
    }
    
    if (structuralPlasticity_) {
        structuralPlasticity_->reset();
    }
    
    // Reset neuromodulation systems
    if (dopamine_) {
        dopamine_->setLevel(0.0f);
    }
    
    NLM_LOG_INFO("Brain plasticity systems reset");
}

void BrainPlasticity::updatePlasticity(double timestep) {
    NLM_LOG_INFO("Updating brain plasticity systems...");
    
    // Update learning rates
    updateLearningRates(timestep);
    
    // Apply spike timing dependent plasticity
    applySpikeTimingDependentPlasticity(timestep);
    
    // Apply Hebbian learning
    applyHebbianLearning(timestep);
    
    // Apply structural plasticity
    applyStructuralPlasticity(timestep);
    
    // Update eligibility traces
    // This would be integrated with the brain's neuron spike events
    
    // Update statistics
    // This would be called from brain implementation
    
    // Log plasticity activity
    logPlasticityActivity();
}

void BrainPlasticity::applySynapticPlasticity(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) {
        return;
    }
    
    // Apply synaptic plasticity to all neurons
    // This includes STDP, Hebbian, and other plasticity rules
    
    size_t synapseCount = 0;
    float totalWeightChange = 0.0f;
    
    for (const auto* neuron : neurons) {
        // Get connected synapses
        auto synapses = neuron->getOutgoingSynapses();
        
        for (auto* syn : synapses) {
            // Apply plasticity rules
            applyPlasticityRule(syn, neuron, timestep_);
            
            synapseCount++;
            
            // Track weight change for statistics
            float weightChange = syn->getWeightChange();
            totalWeightChange += std::abs(weightChange);
        }
    }
    
    activeSynapses_ = synapseCount;
    averageSynapticWeight_ = totalWeightChange / std::max<size_t>(synapseCount, 1);
}

void BrainPlasticity::applySpikeTimingDependentPlasticity(double timestep) {
    if (!spikeSystem_ || !stdp_) {
        return;
    }
    
    // Get pending spike events
    auto pendingSpikes = spikeSystem_->getPendingSpikeEvents();
    
    for (const auto& spike : pendingSpikes) {
        // Find post-synaptic neurons and apply STDP
        // This would involve finding neurons that received the spike
        // and calculating timing-dependent weight changes
        
        // For now, use simplified STDP update
        float weightChange = calculateSTDPWeightChange(spike, timestep);
        
        // Record plasticity event
        recordPlasticityEvent("STDP", spike.source_neuron, spike.destination_neuron, weightChange, "dopamine");
    }
}

void BrainPlasticity::applyHebbianLearning(double timestep) {
    if (!hebbian_) {
        return;
    }
    
    // Apply Hebbian learning rules
    // This would involve finding co-activated neurons and strengthening connections
    
    // Simplified Hebbian learning for demonstration
    float weightChange = calculateHebbianWeightChange(timestep);
    
    if (weightChange != 0.0f) {
        recordPlasticityEvent("Hebbian", 0, 0, weightChange, "curiosity");
    }
}

void BrainPlasticity::applyStructuralPlasticity(double timestep) {
    if (!structuralPlasticity_) {
        return;
    }
    
    // Apply structural plasticity rules
    // This includes synaptogenesis (formation of new synapses) and pruning (removal of weak synapses)
    
    structuralPlasticity_->update(nullptr, *rng_, timestep * 1000);  // Would pass brain reference
    
    // Record structural plasticity events
    // This would be handled by structuralPlasticity system
}

void BrainPlasticity::modulateLearningWithNeuromodulation(float dopamine, float curiosity, float novelty) {
    // Update plasticity strength based on neuromodulation
    float modulationFactor = 1.0f;
    
    if (dopamine > 0.0f) {
        modulationFactor *= 1.0f + dopamine * 0.5f;  // Positive dopamine increases learning
    }
    
    if (curiosity > 0.5f) {
        modulationFactor *= 1.2f;  // High curiosity increases exploration and learning
    }
    
    if (novelty > 0.8f) {
        modulationFactor *= 1.3f;  // High novelty increases learning
    }
    
    plasticityStrength_ = std::clamp(modulationFactor, 0.1f, 3.0f);
    
    // Update learning rates
    learningRate_ *= plasticityStrength_;
    stdpLearningRate_ *= plasticityStrength_;
    hebbianLearningRate_ *= plasticityStrength_;
}

void BrainPlasticity::updateEligibilityTraces(const std::vector<Neuron*>& neurons, double timestep) {
    if (neurons.empty()) {
        return;
    }
    
    // Update eligibility traces for all neurons
    for (const auto* neuron : neurons) {
        size_t neuronId = neuron->getId();
        
        // Update eligibility trace (decays over time)
        float& eligibility = eligibilityTraces_[neuronId];
        eligibility *= (1.0f - 0.01f * timestep);  // Decay factor
        
        // Reset if neuron just spiked
        // This would be called from brain spike handling
    }
}

void BrainPlasticity::simulatePlasticity(double timestep, const std::vector<Neuron*>& neurons) {
    // Complete plasticity simulation for one timestep
    updateLearningRates(timestep);
    applySynapticPlasticity(neurons);
    modulateLearningWithNeuromodulation(
        dopamine_ ? dopamine_->getLevel() : 0.0f,
        curiosity_ ? curiosity_->getLevel() : 0.0f,
        novelty_ ? novelty_->getLevel() : 0.0f
    );
}

void BrainPlasticity::updateLearningRates(double timestep) {
    // Update learning rates based on plasticity strength and neuromodulation
    
    // Time-dependent learning rate decay
    learningRate_ *= (1.0f - 0.001f * timestep);
    stdpLearningRate_ *= (1.0f - 0.001f * timestep);
    hebbianLearningRate_ *= (1.0f - 0.001f * timestep);
    structuralLearningRate_ *= (1.0f - 0.001f * timestep);
    
    // Clip learning rates to reasonable bounds
    learningRate_ = std::clamp(learningRate_, 0.001f, 0.1f);
    stdpLearningRate_ = std::clamp(stdpLearningRate_, 0.001f, 0.1f);
    hebbianLearningRate_ = std::clamp(hebbianLearningRate_, 0.0001f, 0.01f);
    structuralLearningRate_ = std::clamp(structuralLearningRate_, 0.00001f, 0.001f);
}

void BrainPlasticity::adaptLearningRatesBasedOnNeuromodulation(float dopamine, float curiosity) {
    // Adapt learning rates based on neuromodulation signals
    
    // Dopamine-based learning rate adaptation
    if (dopamine > 0.0f) {
        // Positive prediction error: increase learning rate
        learningRate_ *= (1.0f + dopamine * 0.1f);
        stdpLearningRate_ *= (1.0f + dopamine * 0.1f);
    } else {
        // Negative prediction error: decrease learning rate
        learningRate_ *= (1.0f + dopamine * 0.05f);
        stdpLearningRate_ *= (1.0f + dopamine * 0.05f);
    }
    
    // Curiosity-based exploration learning rate
    if (curiosity > 0.5f) {
        // High curiosity: increase exploration learning rate
        hebbianLearningRate_ *= 1.2f;
    }
}

void BrainPlasticity::calculateStatistics(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) {
        return;
    }
    
    // Calculate statistics based on current network state
    size_t totalSynapses = 0;
    float totalWeight = 0.0f;
    
    for (const auto* neuron : neurons) {
        auto synapses = neuron->getOutgoingSynapses();
        totalSynapses += synapses.size();
        
        for (const auto* syn : synapses) {
            totalWeight += std::abs(syn->getWeight());
        }
    }
    
    activeSynapses_ = totalSynapses;
    averageSynapticWeight_ = totalSynapses > 0 ? totalWeight / totalSynapses : 0.0f;
}

void BrainPlasticity::applyPlasticityRule(Synapse* syn, const Neuron* neuron, double timestep) {
    if (!syn || !neuron) {
        return;
    }
    
    // Apply plasticity rules based on recent activity
    float weightChange = 0.0f;
    
    // Simplified STDP for demonstration
    float currentTime = static_cast<float>(timestep);
    auto& preSpikeHistory = syn->getPreSpikeHistory();
    auto& postSpikeHistory = syn->getPostSpikeHistory();
    
    if (!preSpikeHistory.empty() && !postSpikeHistory.empty()) {
        // Calculate STDP weight change
        float maxPreTime = preSpikeHistory.back();
        float maxPostTime = postSpikeHistory.back();
        
        if (maxPostTime > maxPreTime) {
            float delta_t = maxPostTime - maxPreTime;
            if (delta_t < 20.0f) {  // LTD window
                weightChange = -0.01f * exp(-delta_t / 10.0f);
            } else {
                weightChange = 0.01f * exp(-delta_t / 20.0f);  // LTP window
            }
        }
    }
    
    // Apply weight change with learning rate modulation
    weightChange *= plasticityStrength_ * learningRate_;
    
    // Clamp weight change to threshold
    weightChange = std::clamp(weightChange, depressionThreshold_, potentiationThreshold_);
    
    if (std::abs(weightChange) > weightChangeThreshold_) {
        syn->addToWeight(weightChange);
    }
}

float BrainPlasticity::calculateSTDPWeightChange(const DetailedSpikeEvent& spike, double timestep) const {
    // Calculate STDP weight change based on spike timing
    float currentTime = static_cast<float>(timestep);
    float delta_t = currentTime - spike.timestamp;
    
    if (delta_t < 0.0f) {
        // Pre-synaptic spike before post-synaptic spike: LTP
        return 0.01f * exp(-delta_t / 20.0f);
    } else {
        // Post-synaptic spike before pre-synaptic spike: LTD
        return -0.01f * exp(-delta_t / 10.0f);
    }
}

float BrainPlasticity::calculateHebbianWeightChange(double timestep) const {
    // Calculate Hebbian weight change based on co-activation
    // Simplified: random weight change for demonstration
    float randomChange = (rng_->uniformReal(0.0f, 1.0f) - 0.5f) * 0.02f;
    return randomChange * plasticityStrength_;
}

void BrainPlasticity::recordPlasticityEvent(const std::string& type, size_t source, size_t destination, 
                                           float weightChange, const std::string& sourceSignal) {
    PlasticityEvent event;
    event.timestamp = std::chrono::steady_clock::now();
    event.type = type;
    event.synapseCount = 1;
    event.weightChange = weightChange;
    event.source = sourceSignal;
    
    recentPlasticityEvents_.push_back(event);
    
    // Limit stored events
    if (recentPlasticityEvents_.size() > 1000) {
        recentPlasticityEvents_.erase(recentPlasticityEvents_.begin());
    }
    
    // Log plasticity event
    NLM_LOG_INFO("Plasticity event: " + type + " (change: " + std::to_string(weightChange) + ") " +
                 "source: " + sourceSignal);
}

void BrainPlasticity::logPlasticityActivity() {
    if (recentPlasticityEvents_.empty()) {
        return;
    }
    
    // Log recent plasticity activity
    NLM_LOG_INFO("Plasticity activity: " + std::to_string(recentPlasticityEvents_.size()) + 
                 " events in last timestep, average strength: " + std::to_string(plasticityStrength_));
}

void BrainPlasticity::onSpikeEvent(const DetailedSpikeEvent& event) {
    // Handle spike events for plasticity
    // This would update eligibility traces and trigger immediate plasticity updates
    
    // Update eligibility trace
    size_t neuronId = event.source_neuron;
    float& eligibility = eligibilityTraces_[neuronId];
    eligibility += 1.0f;  // Spike strengthens eligibility
    
    // Apply STDP for this spike
    float weightChange = calculateSTDPWeightChange(event, event.timestamp);
    recordPlasticityEvent("SpikeTriggered", event.source_neuron, event.destination_neuron, weightChange, "internal");
}

void BrainPlasticity::onDelayedSpikeEvent(const DelayedSpikeEvent& event) {
    // Handle delayed spike events for plasticity
    // This would be used for long-range connectivity plasticity
    
    // Calculate weight change based on delayed spike
    float weightChange = event.weight * 0.01f;  // Simplified weight update
    recordPlasticityEvent("DelayedSpike", event.source_neuron, event.destination_neuron, weightChange, "structural");
}

bool BrainPlasticity::validatePlasticityConfig(const std::shared_ptr<const Config>& config) {
    if (!config) {
        return false;
    }
    
    // Validate plasticity-related configuration
    auto learningRate = config->getOr<float>("learning_rate", 0.01f);
    if (learningRate < 0.001f || learningRate > 0.1f) {
        NLM_LOG_ERROR("Invalid learning_rate: " + std::to_string(learningRate));
        return false;
    }
    
    auto stdpLearningRate = config->getOr<float>("stdp_learning_rate", 0.01f);
    if (stdpLearningRate < 0.001f || stdpLearningRate > 0.1f) {
        NLM_LOG_ERROR("Invalid stdp_learning_rate: " + std::to_string(stdpLearningRate));
        return false;
    }
    
    auto hebbianLearningRate = config->getOr<float>("hebbian_learning_rate", 0.001f);
    if (hebbianLearningRate < 0.0001f || hebbianLearningRate > 0.01f) {
        NLM_LOG_ERROR("Invalid hebbian_learning_rate: " + std::to_string(hebbianLearningRate));
        return false;
    }
    
    return true;
}

void BrainPlasticity::applyDefaultPlasticitySettings(std::shared_ptr<Config> config) {
    if (!config) {
        return;
    }
    
    // Apply default plasticity settings
    config->set("learning_rate", 0.01f);
    config->set("stdp_learning_rate", 0.01f);
    config->set("hebbian_learning_rate", 0.001f);
    config->set("structural_learning_rate", 0.0001f);
    config->set("plasticity_strength", 1.0f);
    config->set("plasticity_enabled", true);
}

} // namespace nlm
