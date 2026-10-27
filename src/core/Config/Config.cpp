#include "NLMErrors.hpp"
#include <fstream>
#include <filesystem>
#include <chrono>

namespace nlm {

// Error handler implementation
ErrorHandler& ErrorHandler::getInstance() {
    static ErrorHandler instance;
    return instance;
}

// NLMException implementation
std::string NLMException::getCodeString() const {
    return getErrorCodeString(code_);
}

// Enhanced error handling in Neuron.cpp
template<typename T>
T safeNeuronAllocation(size_t count, const std::string& operation) {
    try {
        return T(count);
    } catch (const std::bad_alloc&) {
        throw NLMMemoryException("Neuron memory allocation failed", operation);
    }
}

// Enhanced Neuron constructor with error handling
Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    if (!id) {
        throw NLMException("Invalid neuron ID: 0");
    }
    
    try {
        pImpl->id = id;
        pImpl->type = NeuronType::Internal;
        pImpl->regionId = INVALID_REGION_ID;
        pImpl->populationId = INVALID_POPULATION_ID;
        pImpl->totalCurrent = 0.0f;
        
        // Validate constructor parameters
        if (std::isnan(pImpl->totalCurrent)) {
            throw NLMException("Invalid neuron initialization: NaN value");
        }
    } catch (const std::exception& e) {
        delete pImpl;
        pImpl = nullptr;
        throw NLMException("Failed to create neuron: " + std::string(e.what()));
    }
}

// Enhanced Neuron destructor with error handling
Neuron::~Neuron() {
    try {
        delete pImpl;
        pImpl = nullptr;
    } catch (const std::exception& e) {
        // Log error but don't throw from destructor
        std::cerr << "[NLM WARNING] Error in neuron destructor: " << e.what() << std::endl;
    }
}

// Enhanced Neuron copy constructor
Neuron::Neuron(const Neuron& other) : pImpl(new Impl) {
    try {
        // Deep copy implementation
        pImpl->id = other.pImpl->id;
        pImpl->type = other.pImpl->type;
        pImpl->state = other.pImpl->state;
        pImpl->regionId = other.pImpl->regionId;
        pImpl->populationId = other.pImpl->populationId;
        pImpl->totalCurrent = other.pImpl->totalCurrent;
        pImpl->synapticInput = other.pImpl->synapticInput;
        pImpl->spikeHistory = other.pImpl->spikeHistory;
        pImpl->plasticityFlags = other.pImpl->plasticityFlags;
    } catch (const std::exception& e) {
        delete pImpl;
        pImpl = nullptr;
        throw NLMException("Failed to copy neuron: " + std::string(e.what()));
    }
}

// Enhanced Neuron move constructor
Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    if (!pImpl) {
        throw NLMException("Move from null neuron");
    }
    other.pImpl = nullptr;
}

// Enhanced Neuron assignment operator
Neuron& Neuron::operator=(const Neuron& other) {
    if (this != &other) {
        try {
            // Self-assignment check
            if (this == &other) return *this;
            
            delete pImpl;
            pImpl = new Impl;
            
            // Deep copy implementation
            pImpl->id = other.pImpl->id;
            pImpl->type = other.pImpl->type;
            pImpl->state = other.pImpl->state;
            pImpl->regionId = other.pImpl->regionId;
            pImpl->populationId = other.pImpl->populationId;
            pImpl->totalCurrent = other.pImpl->totalCurrent;
            pImpl->synapticInput = other.pImpl->synapticInput;
            pImpl->spikeHistory = other.pImpl->spikeHistory;
            pImpl->plasticityFlags = other.pImpl->plasticityFlags;
        } catch (const std::exception& e) {
            delete pImpl;
            pImpl = nullptr;
            throw NLMException("Failed to assign neuron: " + std::string(e.what()));
        }
    }
    return *this;
}

// Enhanced Neuron move assignment operator
Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

// Enhanced Neuron methods with error handling
void Neuron::setType(NeuronType type) {
    if (pImpl) {
        pImpl->type = type;
    } else {
        throw NLMException("Cannot set type on null neuron");
    }
}

void Neuron::setMembranePotential(MembranePotential potential) {
    if (pImpl) {
        pImpl->state.membranePotential = potential;
        
        // Validate value
        if (std::isnan(pImpl->state.membranePotential) || 
            std::isinf(pImpl->state.membranePotential)) {
            throw NLMException("Invalid membrane potential: " + 
                             std::to_string(pImpl->state.membranePotential));
        }
    } else {
        throw NLMException("Cannot set membrane potential on null neuron");
    }
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    if (pImpl) {
        pImpl->state.membranePotential += delta;
        
        // Check for overflow/underflow
        if (pImpl->state.membranePotential > 100.0f || 
            pImpl->state.membranePotential < -100.0f) {
            throw NLMException("Membrane potential out of range after addition");
        }
    } else {
        throw NLMException("Cannot add to membrane potential on null neuron");
    }
}

MembranePotential Neuron::getMembranePotential() const {
    if (!pImpl) {
        throw NLMException("Cannot get membrane potential from null neuron");
    }
    return pImpl->state.membranePotential;
}

void Neuron::setThreshold(MembranePotential threshold) {
    if (pImpl) {
        pImpl->state.threshold = threshold;
        
        // Validate threshold
        if (std::isnan(pImpl->state.threshold) || 
            std::isinf(pImpl->state.threshold)) {
            throw NLMException("Invalid threshold: " + std::to_string(pImpl->state.threshold));
        }
    } else {
        throw NLMException("Cannot set threshold on null neuron");
    }
}

bool Neuron::checkThreshold() const {
    if (!pImpl) {
        throw NLMException("Cannot check threshold on null neuron");
    }
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

void Neuron::injectCurrent(MembranePotential current) {
    if (pImpl) {
        // Validate current
        if (std::isnan(current) || std::isinf(current)) {
            throw NLMException("Invalid current injection: " + std::to_string(current));
        }
        
        pImpl->synapticInput += current;
    } else {
        throw NLMException("Cannot inject current on null neuron");
    }
}

MembranePotential Neuron::getTotalCurrent() const {
    if (!pImpl) {
        throw NLMException("Cannot get total current from null neuron");
    }
    return pImpl->synapticInput;
}

void Neuron::clearTotalCurrent() {
    if (pImpl) {
        pImpl->synapticInput = 0.0f;
    } else {
        throw NLMException("Cannot clear current on null neuron");
    }
}

void Neuron::recordSpike(Timestamp timestamp) {
    if (pImpl) {
        // Validate timestamp
        if (timestamp < 0.0) {
            throw NLMException("Invalid spike timestamp: " + std::to_string(timestamp));
        }
        
        pImpl->spikeHistory.push_back(timestamp);
        
        // Check history size
        if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
        }
    } else {
        throw NLMException("Cannot record spike on null neuron");
    }
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    if (pImpl) {
        pImpl->plasticityFlags.hebbian = hebbian;
        pImpl->plasticityFlags.stdp = stdp;
        pImpl->plasticityFlags.reward_modulated = rewardModulated;
    } else {
        throw NLMException("Cannot enable plasticity on null neuron");
    }
}

void Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    if (!pImpl) {
        throw NLMException("Cannot step null neuron");
    }
    
    // Validate parameters
    if (currentTime < 0.0) {
        throw NLMException("Invalid current time: " + std::to_string(currentTime));
    }
    if (dt <= 0.0) {
        throw NLMException("Invalid timestep: " + std::to_string(dt));
    }
    
    try {
        // LIF integration with enhanced error handling
        MembranePotential& V = pImpl->state.membranePotential;
        MembranePotential V_rest = pImpl->state.restingPotential;
        MembranePotential V_reset = pImpl->state.resetPotential;
        float tau = Impl::TIME_CONSTANT;
        float C = Impl::MEMBRANE_CAPACITANCE;
        
        float synapticContribution = pImpl->synapticInput / C;
        float leakContribution = (V_rest - V) / tau;
        
        // Exponential Euler integration
        V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
        
        // Apply spike-frequency adaptation
        if (pImpl->state.adaptationVariable > 0.0f) {
            V -= pImpl->state.adaptationVariable * 0.01f;
            pImpl->state.adaptationVariable *= 0.95f;
        }
        
        // Clamp to prevent instability
        V = std::clamp(V, -100.0f, 50.0f);
        
        // Check for spike
        if (V >= pImpl->state.threshold) {
            pImpl->state.firingState = FiringState::Active;
            pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
            recordSpike(currentTime);
            V = pImpl->state.resetPotential;
            pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
            pImpl->state.adaptationVariable += 1.0f;
        } else {
            pImpl->state.firingState = FiringState::Active;
        }
        
        pImpl->synapticInput = 0.0f;
        
    } catch (const NLMException&) {
        throw;
    } catch (const std::exception& e) {
        throw NLMException("LIF integration failed: " + std::string(e.what()));
    }
}

void Neuron::step(Timestamp currentTime) {
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    if (pImpl) {
        pImpl->state = NeuronState();
        pImpl->synapticInput = 0.0f;
        pImpl->spikeHistory.clear();
    } else {
        throw NLMException("Cannot reset null neuron");
    }
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    if (!pImpl) {
        throw NLMException("Cannot initialize null neuron");
    }
    
    try {
        // Real random initialization with biological constraints
        pImpl->state.membranePotential = pImpl->state.restingPotential + 
                                         rng.uniformReal(-3.0f, 3.0f);
        
        pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
        pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
        pImpl->state.resetPotential = pImpl->state.restingPotential + 
                                       rng.uniformReal(0.0f, 5.0f);
        
        pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
        
        pImpl->state.firingState = FiringState::Resting;
        pImpl->state.refractoryRemaining = 0;
        pImpl->state.adaptationVariable = 0.0f;
        pImpl->state.lastSpikeTime = -1.0f;
        
        pImpl->synapticInput = 0.0f;
        pImpl->spikeHistory.clear();
        
    } catch (const NLMException&) {
        throw;
    } catch (const std::exception& e) {
        throw NLMException("Failed to initialize neuron randomly: " + std::string(e.what()));
    }
}

// Enhanced Synapse constructor with error handling
Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination) : pImpl(new Impl) {
    if (!id || !source || !destination) {
        delete pImpl;
        throw NLMException("Invalid synapse IDs: id=" + std::to_string(id) + 
                          " source=" + std::to_string(source) + 
                          " destination=" + std::to_string(destination));
    }
    
    try {
        pImpl->id = id;
        pImpl->sourceNeuron = source;
        pImpl->destinationNeuron = destination;
        pImpl->weight = 0.0f;
        pImpl->delay = 1;
        pImpl->type = SynapseType::Excitatory;
        pImpl->eligibilityTrace = 0.0f;
        pImpl->efficacy = 1.0f;
        pImpl->shortTermDepression = 1.0f;
        pImpl->shortTermFacilitation = 0.0f;
        pImpl->lastPreSpikeTime = -1.0f;
        pImpl->lastPostSpikeTime = -1.0f;
        
        // Validate initial values
        if (pImpl->weight < Impl::MIN_WEIGHT || pImpl->weight > Impl::MAX_WEIGHT) {
            throw NLMException("Invalid initial synapse weight");
        }
    } catch (const std::exception& e) {
        delete pImpl;
        pImpl = nullptr;
        throw NLMException("Failed to create synapse: " + std::string(e.what()));
    }
}

// Enhanced Synapse destructor with error handling
Synapse::~Synapse() {
    try {
        delete pImpl;
        pImpl = nullptr;
    } catch (const std::exception& e) {
        std::cerr << "[NLM WARNING] Error in synapse destructor: " << e.what() << std::endl;
    }
}

// Enhanced Synapse copy constructor
Synapse::Synapse(const Synapse& other) : pImpl(new Impl) {
    try {
        pImpl->id = other.pImpl->id;
        pImpl->sourceNeuron = other.pImpl->sourceNeuron;
        pImpl->destinationNeuron = other.pImpl->destinationNeuron;
        pImpl->weight = other.pImpl->weight;
        pImpl->delay = other.pImpl->delay;
        pImpl->type = other.pImpl->type;
        pImpl->preSpikeHistory = other.pImpl->preSpikeHistory;
        pImpl->postSpikeHistory = other.pImpl->postSpikeHistory;
        pImpl->plasticityFlags = other.pImpl->plasticityFlags;
        pImpl->shortTermDepression = other.pImpl->shortTermDepression;
        pImpl->shortTermFacilitation = other.pImpl->shortTermFacilitation;
        pImpl->lastPreSpikeTime = other.pImpl->lastPreSpikeTime;
        pImpl->lastPostSpikeTime = other.pImpl->lastPostSpikeTime;
        pImpl->eligibilityTrace = other.pImpl->eligibilityTrace;
        pImpl->efficacy = other.pImpl->efficacy;
    } catch (const std::exception& e) {
        delete pImpl;
        pImpl = nullptr;
        throw NLMException("Failed to copy synapse: " + std::string(e.what()));
    }
}

// Enhanced Synapse move constructor
Synapse::Synapse(Synapse&& other) noexcept : pImpl(other.pImpl) {
    if (!pImpl) {
        throw NLMException("Move from null synapse");
    }
    other.pImpl = nullptr;
}

// Enhanced Synapse assignment operator
Synapse& Synapse::operator=(const Synapse& other) {
    if (this != &other) {
        try {
            if (this == &other) return *this;
            
            delete pImpl;
            pImpl = new Impl;
            
            pImpl->id = other.pImpl->id;
            pImpl->sourceNeuron = other.pImpl->sourceNeuron;
            pImpl->destinationNeuron = other.pImpl->destinationNeuron;
            pImpl->weight = other.pImpl->weight;
            pImpl->delay = other.pImpl->delay;
            pImpl->type = other.pImpl->type;
            pImpl->preSpikeHistory = other.pImpl->preSpikeHistory;
            pImpl->postSpikeHistory = other.pImpl->postSpikeHistory;
            pImpl->plasticityFlags = other.pImpl->plasticityFlags;
            pImpl->shortTermDepression = other.pImpl->shortTermDepression;
            pImpl->shortTermFacilitation = other.pImpl->shortTermFacilitation;
            pImpl->lastPreSpikeTime = other.pImpl->lastPreSpikeTime;
            pImpl->lastPostSpikeTime = other.pImpl->lastPostSpikeTime;
            pImpl->eligibilityTrace = other.pImpl->eligibilityTrace;
            pImpl->efficacy = other.pImpl->efficacy;
        } catch (const std::exception& e) {
            delete pImpl;
            pImpl = nullptr;
            throw NLMException("Failed to assign synapse: " + std::string(e.what()));
        }
    }
    return *this;
}

// Enhanced Synapse move assignment operator
Synapse& Synapse::operator=(Synapse&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

// Enhanced Synapse methods with error handling
void Synapse::setWeight(SynapticWeight weight) {
    if (pImpl) {
        pImpl->weight = weight;
        
        if (pImpl->weight < Impl::MIN_WEIGHT || pImpl->weight > Impl::MAX_WEIGHT) {
            throw NLMException("Synapse weight out of bounds: " + std::to_string(pImpl->weight));
        }
    } else {
        throw NLMException("Cannot set weight on null synapse");
    }
}

void Synapse::addToWeight(SynapticWeight delta) {
    if (pImpl) {
        pImpl->weight += delta;
        
        if (pImpl->weight < Impl::MIN_WEIGHT || pImpl->weight > Impl::MAX_WEIGHT) {
            throw NLMException("Synapse weight out of bounds after addition");
        }
    } else {
        throw NLMException("Cannot add to weight on null synapse");
    }
}

void Synapse::setDelay(Delay delay) {
    if (pImpl) {
        if (delay < 0) {
            throw NLMException("Invalid synapse delay: " + std::to_string(delay));
        }
        pImpl->delay = delay;
    } else {
        throw NLMException("Cannot set delay on null synapse");
    }
}

void Synapse::setType(SynapseType type) {
    if (pImpl) {
        pImpl->type = type;
    } else {
        throw NLMException("Cannot set type on null synapse");
    }
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    if (pImpl) {
        if (timestamp < 0.0) {
            throw NLMException("Invalid pre-spike timestamp: " + std::to_string(timestamp));
        }
        
        pImpl->preSpikeHistory.push_back(timestamp);
        pImpl->lastPreSpikeTime = timestamp;
        
        if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
            pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
        }
    } else {
        throw NLMException("Cannot record pre-spike on null synapse");
    }
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    if (pImpl) {
        if (timestamp < 0.0) {
            throw NLMException("Invalid post-spike timestamp: " + std::to_string(timestamp));
        }
        
        pImpl->postSpikeHistory.push_back(timestamp);
        pImpl->lastPostSpikeTime = timestamp;
        
        if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
            pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
        }
    } else {
        throw NLMException("Cannot record post-spike on null synapse");
    }
}

void Synapse::decayEligibilityTrace(float decayRate) {
    if (pImpl) {
        if (decayRate < 0.0f || decayRate >= 1.0f) {
            throw NLMException("Invalid decay rate: " + std::to_string(decayRate));
        }
        
        pImpl->eligibilityTrace *= (1.0f - decayRate);
        
        if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
            pImpl->eligibilityTrace = 0.0f;
        }
    } else {
        throw NLMException("Cannot decay eligibility trace on null synapse");
    }
}

void Synapse::setEfficacy(float efficacy) {
    if (pImpl) {
        pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
    } else {
        throw NLMException("Cannot set efficacy on null synapse");
    }
}

void Synapse::step(Timestamp currentTime) {
    if (!pImpl) {
        throw NLMException("Cannot step null synapse");
    }
    
    if (currentTime < 0.0) {
        throw NLMException("Invalid current time: " + std::to_string(currentTime));
    }
    
    try {
        TimestepDuration dt = 0.001;
        
        // Decay short-term facilitation (Tsodyks-Markram model)
        if (pImpl->lastPreSpikeTime >= 0.0f) {
            float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
            pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
        }
        
        // Decay short-term depression
        if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
            float timeSinceActivity = std::max(
                pImpl->lastPostSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPostSpikeTime) : 0.0f,
                pImpl->lastPreSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPreSpikeTime) : 0.0f
            );
            
            // Recovery from depression toward 1.0
            pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * 
                                        (1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
        }
        
        // Decay eligibility trace for reward-modulated learning
        decayEligibilityTrace(0.001f);
        
        // Clamp weight bounds
        pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
        
    } catch (const NLMException&) {
        throw;
    } catch (const std::exception& e) {
        throw NLMException("Synapse step failed: " + std::string(e.what()));
    }
}

void Synapse::reset() {
    if (pImpl) {
        pImpl->weight = 0.0f;
        pImpl->preSpikeHistory.clear();
        pImpl->postSpikeHistory.clear();
        pImpl->eligibilityTrace = 0.0f;
        pImpl->efficacy = 1.0f;
    } else {
        throw NLMException("Cannot reset null synapse");
    }
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    if (!pImpl) {
        throw NLMException("Cannot initialize null synapse");
    }
    
    try {
        // Proper random initialization based on synapse type
        if (pImpl->type == SynapseType::Excitatory) {
            pImpl->weight = rng.uniformReal(0.1f, 0.4f);
            pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
        } else if (pImpl->type == SynapseType::Inhibitory) {
            pImpl->weight = -rng.uniformReal(0.1f, 0.4f);
            pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
        } else {
            pImpl->weight = rng.uniformReal(-0.1f, 0.1f);
            pImpl->efficacy = rng.uniformReal(0.9f, 1.0f);
        }
        
        pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
        
        // Initialize short-term plasticity state
        pImpl->shortTermDepression = 1.0f;
        pImpl->shortTermFacilitation = 0.0f;
        
        // Initialize eligibility trace to 0
        pImpl->eligibilityTrace = 0.0f;
        
        // Initialize last spike times
        pImpl->lastPreSpikeTime = -1.0f;
        pImpl->lastPostSpikeTime = -1.0f;
        
    } catch (const NLMException&) {
        throw;
    } catch (const std::exception& e) {
        throw NLMException("Failed to initialize synapse randomly: " + std::string(e.what()));
    }
}

} // namespace nlm
