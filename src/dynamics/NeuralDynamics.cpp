#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>
#include <map>
#include <limits>
#include "NLMErrors.hpp"
#include "ErrorUtils.hpp"

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    float membraneCapacitance;    // nF
    float restingPotential;       // mV
    float threshold;              // mV
    float resetPotential;          // mV
    float adaptationCurrent;       // pA (for spike-frequency adaptation)
    float adaptationTimeConstant;  // ms
    std::map<SynapseHandle, float> synapticConductances;  // G_syn for each synapse
    float totalSynapticConductance;  // Total G_syn
    std::map<SynapseHandle, float> alphaX;  // For alpha synapse model (x variable)
    std::map<SynapseHandle, float> alphaY;  // For alpha synapse model (y variable)
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f), 
             membraneCapacitance(1.0f), restingPotential(-70.0f), 
             threshold(-55.0f), resetPotential(-65.0f),
             adaptationCurrent(0.0f), adaptationTimeConstant(50.0f),
             totalSynapticConductance(0.0f) {
        validateParameters();
    }
    
    void validateParameters() {
        NLM_VALIDATE(membraneTimeConstant > 0.0f, "Membrane time constant must be positive");
        NLM_VALIDATE(membraneResistance > 0.0f, "Membrane resistance must be positive");
        NLM_VALIDATE(membraneCapacitance > 0.0f, "Membrane capacitance must be positive");
        NLM_VALIDATE(restingPotential < threshold, "Resting potential must be less than threshold");
        NLM_VALIDATE(resetPotential < restingPotential, "Reset potential must be less than resting potential");
        NLM_VALIDATE(resetPotential < threshold, "Reset potential must be less than threshold");
        NLM_VALIDATE(adaptationTimeConstant > 0.0f, "Adaptation time constant must be positive");
        NLM_VALIDATE(adaptationCurrent >= 0.0f, "Adaptation current must be non-negative");
    }
    
    void validateMembranePotential(float V) {
        NLM_VALIDATE(std::isfinite(V), "Membrane potential must be finite");
        NLM_VALIDATE(V >= -1000.0f && V <= 1000.0f, 
                   "Membrane potential out of reasonable bounds [-1000, 1000] mV");
    }
    
    void validateCurrent(float I) {
        NLM_VALIDATE(std::isfinite(I), "Current must be finite");
        NLM_VALIDATE(std::abs(I) < 1000000.0f, "Current magnitude exceeds reasonable bounds");
    }
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    try {
        NLM_VALIDATE(neuron != nullptr, "Neuron pointer cannot be null");
        NLM_VALIDATE(dt > 0.0, "Timestep must be positive");
        
        // Validate timestep for numerical stability
        NLM_VALIDATE(dt < 0.1, "Timestep too large, may cause numerical instability");
        
        const auto& state = neuron->getState();
        
        // Validate neuron state
        NLM_VALIDATE(std::isfinite(state.membranePotential), "Neuron membrane potential must be finite");
        NLM_VALIDATE(std::isfinite(state.restingPotential), "Neuron resting potential must be finite");
        NLM_VALIDATE(std::isfinite(state.threshold), "Neuron threshold must be finite");
        NLM_VALIDATE(std::isfinite(state.resetPotential), "Neuron reset potential must be finite");
        
        float V = neuron->getMembranePotential();
        float V_rest = state.restingPotential;
        float V_reset = state.resetPotential;
        float threshold = state.threshold;
        float I_total = neuron->getTotalCurrent();
        
        // Validate currents
        pImpl->validateCurrent(I_total);
        pImpl->validateMembranePotential(V);
        
        // Handle refractory period with validation
        if (neuron->isRefractory()) {
            neuron->setMembranePotential(V_reset);
            neuron->decrementRefractory();
            neuron->clearTotalCurrent();
            return;
        }
        
        // Leaky integrate-and-fire with Euler-Cromer integration and validation
        float dV = (-(V - V_rest) / pImpl->membraneTimeConstant + I_total / pImpl->membraneResistance) * static_cast<float>(dt);
        
        // Check for numerical overflow in dV
        NLM_VALIDATE(std::isfinite(dV), "Voltage change (dV) is not finite");
        NLM_VALIDATE(std::abs(dV) < 100.0f, "Voltage change too large, may cause instability");
        
        V += dV;
        
        // Validate membrane potential after update
        pImpl->validateMembranePotential(V);
        
        // Apply spike-frequency adaptation with validation
        if (pImpl->adaptationCurrent > 0.0f) {
            float decayFactor = static_cast<float>(dt) * 1000.0f * pImpl->adaptationCurrent / pImpl->adaptationTimeConstant;
            NLM_VALIDATE(decayFactor < 1.0f, "Adaptation decay factor too large");
            
            pImpl->adaptationCurrent = std::max(0.0f, pImpl->adaptationCurrent - decayFactor);
        }
        
        // Validate adaptation current
        NLM_VALIDATE(std::isfinite(pImpl->adaptationCurrent), "Adaptation current must be finite");
        NLM_VALIDATE(pImpl->adaptationCurrent < 1000.0f, "Adaptation current exceeds maximum");
        
        // Update adaptation from spike history
        const auto& spikeHistory = neuron->getSpikeHistory();
        if (!spikeHistory.empty()) {
            float recentSpikeCount = 0;
            float timeWindow = 100.0f;
            for (auto it = spikeHistory.rbegin(); it != spikeHistory.rend(); ++it) {
                if (0.0f <= (*it) && (*it) <= timeWindow) {
                    recentSpikeCount += 1.0f;
                } else {
                    break;
                }
            }
            if (recentSpikeCount > 0) {
                float spikeContribution = 100.0f * recentSpikeCount / timeWindow;
                NLM_VALIDATE(spikeContribution > 0.0f, "Spike contribution must be positive");
                
                pImpl->adaptationCurrent += spikeContribution;
                pImpl->adaptationCurrent = std::min(pImpl->adaptationCurrent, 1000.0f);
            }
        }
        
        // Check for spike with adaptive threshold
        float adaptiveThreshold = threshold + pImpl->adaptationCurrent * 0.1f;
        
        // Validate thresholds
        NLM_VALIDATE(std::isfinite(adaptiveThreshold), "Adaptive threshold must be finite");
        NLM_VALIDATE(adaptiveThreshold > 0.0f, "Adaptive threshold must be positive");
        
        if (V >= adaptiveThreshold) {
            neuron->setFiringState(FiringState::Active);
            neuron->recordSpike(0.0);
            
            // Reset membrane potential
            V = V_reset;
            neuron->setRefractoryPeriod(static_cast<uint32_t>(pImpl->membraneTimeConstant * 0.8f));
            neuron->setFiringState(FiringState::Refractory);
            
            // Update adaptation current
            pImpl->adaptationCurrent += 200.0f;
            pImpl->adaptationCurrent = std::min(pImpl->adaptationCurrent, 1000.0f);
            
        } else {
            neuron->setFiringState(FiringState::Active);
        }
        
        // Validate final membrane potential
        pImpl->validateMembranePotential(V);
        
        neuron->setMembranePotential(V);
        neuron->clearTotalCurrent();
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR(std::string("Failed to update neuron in IntegrateAndFireDynamics: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error updating neuron: ") + e.what());
        throw RuntimeError("Neuron update failed: " + std::string(e.what()), "NeuralDynamics");
    }
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    try {
        NLM_VALIDATE(synapse != nullptr, "Synapse pointer cannot be null");
        NLM_VALIDATE(dt > 0.0, "Timestep must be positive");
        
        SynapseHandle handle = synapse->getId().value;
        
        // Validate synapse state
        float weight = synapse->getWeight();
        float efficacy = synapse->getEfficacy();
        
        NLM_VALIDATE(std::isfinite(weight), "Synaptic weight must be finite");
        NLM_VALIDATE(std::isfinite(efficacy), "Synaptic efficacy must be finite");
        NLM_VALIDATE(weight > -1000.0f && weight < 1000.0f, "Synaptic weight out of reasonable bounds");
        NLM_VALIDATE(efficacy >= 0.0f, "Synaptic efficacy must be non-negative");
        
        // Alpha synapse model for synaptic dynamics
        if (pImpl->alphaX.find(handle) != pImpl->alphaX.end()) {
            float x = pImpl->alphaX[handle];
            float y = pImpl->alphaY[handle];
            
            // Update synaptic variables with validation
            float tau_x = 5.0f;
            float tau_y = 100.0f;
            
            float dx = static_cast<float>(dt) * 1000.0f * (-x / tau_x);
            float dy = static_cast<float>(dt) * 1000.0f * (-y / tau_y);
            
            // Validate changes
            NLM_VALIDATE(std::isfinite(dx) && std::isfinite(dy), "Synaptic variable changes not finite");
            NLM_VALIDATE(std::abs(dx) < 1.0f && std::abs(dy) < 1.0f, "Synaptic variable changes too large");
            
            x += dx;
            y += dy;
            
            pImpl->alphaX[handle] = x;
            pImpl->alphaY[handle] = y;
            
            // Validate alpha variables
            NLM_VALIDATE(x >= 0.0f && x <= 1.0f, "Alpha X variable out of bounds [0, 1]");
            NLM_VALIDATE(y >= 0.0f && y <= 1.0f, "Alpha Y variable out of bounds [0, 1]");
            
            // Calculate synaptic conductance
            float peak_conductance = weight * efficacy;
            float synapticConductance = peak_conductance * x * y;
            
            // Validate synaptic conductance
            NLM_VALIDATE(std::isfinite(synapticConductance), "Synaptic conductance must be finite");
            NLM_VALIDATE(synapticConductance >= 0.0f, "Synaptic conductance must be non-negative");
            
            pImpl->synapticConductances[handle] = synapticConductance;
            pImpl->totalSynapticConductance = 0.0f;
            for (const auto& pair : pImpl->synapticConductances) {
                pImpl->totalSynapticConductance += pair.second;
            }
            
            // Validate total synaptic conductance
            NLM_VALIDATE(std::isfinite(pImpl->totalSynapticConductance), 
                       "Total synaptic conductance must be finite");
        }
        
        // Decay eligibility trace with validation
        float eligibility = synapse->getEligibilityTrace();
        float decayFactor = (1.0f - static_cast<float>(dt) * 1000.0f * 0.01f);
        
        // Validate decay factor
        NLM_VALIDATE(decayFactor > 0.0f, "Decay factor must be positive");
        
        eligibility *= decayFactor;
        
        if (std::abs(eligibility) < 0.001f) {
            eligibility = 0.0f;
        }
        
        // Validate eligibility
        NLM_VALIDATE(std::isfinite(eligibility), "Eligibility trace must be finite");
        
        synapse->setEligibilityTrace(eligibility);
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR(std::string("Failed to update synapse in IntegrateAndFireDynamics: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error updating synapse: ") + e.what());
        throw RuntimeError("Synapse update failed: " + std::string(e.what()), "NeuralDynamics");
    }
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    try {
        NLM_VALIDATE(neuron != nullptr, "Neuron pointer cannot be null");
        NLM_VALIDATE(synapse != nullptr, "Synapse pointer cannot be null");
        
        SynapseHandle handle = synapse->getId().value;
        SynapticWeight weight = synapse->getWeight();
        SynapseType type = synapse->getType();
        bool isExcitatory = synapse->isExcitatory();
        
        // Validate synaptic properties
        NLM_VALIDATE(std::isfinite(weight), "Synaptic weight must be finite");
        NLM_VALIDATE(weight > -1000.0f && weight < 1000.0f, "Synaptic weight out of reasonable bounds");
        
        // Alpha synapse model
        float tau_x = 5.0f;
        float tau_y = 100.0f;
        float peakConductance = std::abs(weight) * synapse->getEfficacy();
        
        // Validate peak conductance
        NLM_VALIDATE(std::isfinite(peakConductance), "Peak conductance must be finite");
        NLM_VALIDATE(peakConductance >= 0.0f, "Peak conductance must be non-negative");
        
        // Initialize synaptic variables
        if (pImpl->alphaX.find(handle) == pImpl->alphaX.end()) {
            pImpl->alphaX[handle] = 0.0f;
            pImpl->alphaY[handle] = 0.0f;
        }
        
        // Update synaptic variables on spike
        float x = pImpl->alphaX[handle];
        float y = pImpl->alphaY[handle];
        
        x = std::min(1.0f, x + 0.1f);
        y = 1.0f;
        
        // Validate synaptic variable updates
        NLM_VALIDATE(x >= 0.0f && x <= 1.0f, "Alpha X variable after spike out of bounds [0, 1]");
        NLM_VALIDATE(y >= 0.0f && y <= 1.0f, "Alpha Y variable after spike out of bounds [0, 1]");
        
        pImpl->alphaX[handle] = x;
        pImpl->alphaY[handle] = y;
        
        // Calculate synaptic current
        float synapticConductance = peakConductance * x * y;
        float E_syn = isExcitatory ? 0.0f : -80.0f;
        float V_m = neuron->getMembranePotential();
        float drivingForce = V_m - E_syn;
        float synapticCurrent = synapticConductance * drivingForce;
        
        // Validate synaptic current
        NLM_VALIDATE(std::isfinite(synapticCurrent), "Synaptic current must be finite");
        NLM_VALIDATE(std::abs(synapticCurrent) < 1000000.0f, "Synaptic current magnitude exceeds reasonable bounds");
        
        if (isExcitatory) {
            neuron->receiveExcitatoryInput(synapticCurrent);
        } else {
            neuron->receiveInhibitoryInput(synapticCurrent);
        }
        
        pImpl->synapticConductances[handle] = synapticConductance;
        pImpl->totalSynapticConductance += synapticConductance;
        
        // Validate total synaptic conductance
        NLM_VALIDATE(std::isfinite(pImpl->totalSynapticConductance), 
                   "Total synaptic conductance must be finite");
        
        // Update eligibility trace - need to cast away const for setting
        float eligibility = synapse->getEligibilityTrace() + 1.0f;
        
        // Validate eligibility
        NLM_VALIDATE(std::isfinite(eligibility), "Eligibility trace must be finite");
        
        const_cast<Synapse*>(synapse)->setEligibilityTrace(eligibility);
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR(std::string("Failed to apply spike input in IntegrateAndFireDynamics: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error applying spike input: ") + e.what());
        throw RuntimeError("Spike input application failed: " + std::string(e.what()), "NeuralDynamics");
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    try {
        NLM_VALIDATE(neuron != nullptr, "Neuron pointer cannot be null");
        
        const auto& state = neuron->getState();
        float V = neuron->getMembranePotential();
        float threshold = state.threshold;
        
        // Validate parameters
        NLM_VALIDATE(std::isfinite(V), "Membrane potential must be finite");
        NLM_VALIDATE(std::isfinite(threshold), "Threshold must be finite");
        NLM_VALIDATE(threshold > 0.0f, "Threshold must be positive");
        
        return V >= threshold;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error checking if neuron should fire: ") + e.what());
        throw RuntimeError("Should fire check failed: " + std::string(e.what()), "NeuralDynamics");
    }
}

void IntegrateAndFireDynamics::reset() {
    try {
        pImpl->adaptationCurrent = 0.0f;
        pImpl->totalSynapticConductance = 0.0f;
        pImpl->synapticConductances.clear();
        pImpl->alphaX.clear();
        pImpl->alphaY.clear();
        
        // Validate reset state
        NLM_VALIDATE(pImpl->adaptationCurrent == 0.0f, "Adaptation current should be zero after reset");
        NLM_VALIDATE(pImpl->totalSynapticConductance == 0.0f, "Total synaptic conductance should be zero after reset");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error resetting IntegrateAndFireDynamics: ") + e.what());
        throw RuntimeError("Reset failed: " + std::string(e.what()), "NeuralDynamics");
    }
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    try {
        NLM_VALIDATE(tau > 0.0f, "Membrane time constant must be positive");
        NLM_VALIDATE(tau < 1000.0f, "Membrane time constant must be less than 1000 ms");
        
        pImpl->membraneTimeConstant = tau;
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR(std::string("Failed to set membrane time constant: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error setting membrane time constant: ") + e.what());
        throw RuntimeError("Set membrane time constant failed: " + std::string(e.what()), "NeuralDynamics");
    }
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    try {
        return pImpl->membraneTimeConstant;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error getting membrane time constant: ") + e.what());
        throw RuntimeError("Get membrane time constant failed: " + std::string(e.what()), "NeuralDynamics");
    }
}

void IntegrateAndFireDynamics::setMembraneResistance(float R) {
    try {
        NLM_VALIDATE(R > 0.0f, "Membrane resistance must be positive");
        NLM_VALIDATE(R < 100000.0f, "Membrane resistance must be less than 100 MOhm");
        
        pImpl->membraneResistance = R;
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR(std::string("Failed to set membrane resistance: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error setting membrane resistance: ") + e.what());
        throw RuntimeError("Set membrane resistance failed: " + std::string(e.what()), "NeuralDynamics");
    }
}

float IntegrateAndFireDynamics::getMembraneResistance() const {
    try {
        return pImpl->membraneResistance;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error getting membrane resistance: ") + e.what());
        throw RuntimeError("Get membrane resistance failed: " + std::string(e.what()), "NeuralDynamics");
    }
}

void IntegrateAndFireDynamics::setMembraneCapacitance(float C) {
    try {
        NLM_VALIDATE(C > 0.0f, "Membrane capacitance must be positive");
        NLM_VALIDATE(C < 100.0f, "Membrane capacitance must be less than 100 nF");
        
        pImpl->membraneCapacitance = C;
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR(std::string("Failed to set membrane capacitance: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error setting membrane capacitance: ") + e.what());
        throw RuntimeError("Set membrane capacitance failed: " + std::string(e.what()), "NeuralDynamics");
    }
}

float IntegrateAndFireDynamics::getMembraneCapacitance() const {
    try {
        return pImpl->membraneCapacitance;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error getting membrane capacitance: ") + e.what());
        throw RuntimeError("Get membrane capacitance failed: " + std::string(e.what()), "NeuralDynamics");
    }
}

void IntegrateAndFireDynamics::setRestingPotential(float V_rest) {
    try {
        NLM_VALIDATE(V_rest > -100.0f, "Resting potential must be greater than -100 mV");
        NLM_VALIDATE(V_rest < 0.0f, "Resting potential must be less than 0 mV");
        
        pImpl->restingPotential = V_rest;
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR(std::string("Failed to set resting potential: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error setting resting potential: ") + e.what());
        throw RuntimeError("Set resting potential failed: " + std::string(e.what()), "NeuralDynamics");
    }
}

float IntegrateAndFireDynamics::getRestingPotential() const {
    try {
        return pImpl->restingPotential;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error getting resting potential: ") + e.what());
        throw RuntimeError("Get resting potential failed: " + std::string(e.what()), "NeuralDynamics");
    }
}

void IntegrateAndFireDynamics::setThreshold(float threshold) {
    try {
        NLM_VALIDATE(threshold > -50.0f, "Threshold must be greater than -50 mV");
        NLM_VALIDATE(threshold < 50.0f, "Threshold must be less than 50 mV");
        NLM_VALIDATE(threshold > pImpl->restingPotential, "Threshold must be greater than resting potential");
        
        pImpl->threshold = threshold;
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR(std::string("Failed to set threshold: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error setting threshold: ") + e.what());
        throw RuntimeError("Set threshold failed: " + std::string(e.what()), "NeuralDynamics");
    }
}

float IntegrateAndFireDynamics::getThreshold() const {
    try {
        return pImpl->threshold;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error getting threshold: ") + e.what());
        throw RuntimeError("Get threshold failed: " + std::string(e.what()), "NeuralDynamics");
    }
}

void IntegrateAndFireDynamics::setResetPotential(float V_reset) {
    try {
        NLM_VALIDATE(V_reset > -80.0f, "Reset potential must be greater than -80 mV");
        NLM_VALIDATE(V_reset < 0.0f, "Reset potential must be less than 0 mV");
        NLM_VALIDATE(V_reset < pImpl->restingPotential, "Reset potential must be less than resting potential");
        NLM_VALIDATE(V_reset < pImpl->threshold, "Reset potential must be less than threshold");
        
        pImpl->resetPotential = V_reset;
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR(std::string("Failed to set reset potential: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error setting reset potential: ") + e.what());
        throw RuntimeError("Set reset potential failed: " + std::string(e.what()), "NeuralDynamics");
    }
}

float IntegrateAndFireDynamics::getResetPotential() const {
    try {
        return pImpl->resetPotential;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error getting reset potential: ") + e.what());
        throw RuntimeError("Get reset potential failed: " + std::string(e.what()), "NeuralDynamics");
    }
}

void IntegrateAndFireDynamics::setAdaptationTimeConstant(float tau_adapt) {
    try {
        NLM_VALIDATE(tau_adapt > 0.0f, "Adaptation time constant must be positive");
        NLM_VALIDATE(tau_adapt < 1000.0f, "Adaptation time constant must be less than 1000 ms");
        
        pImpl->adaptationTimeConstant = tau_adapt;
        
    } catch (const NLMError& e) {
        NLM_LOG_ERROR(std::string("Failed to set adaptation time constant: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error setting adaptation time constant: ") + e.what());
        throw RuntimeError("Set adaptation time constant failed: " + std::string(e.what()), "NeuralDynamics");
    }
}

float IntegrateAndFireDynamics::getAdaptationTimeConstant() const {
    try {
        return pImpl->adaptationTimeConstant;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error getting adaptation time constant: ") + e.what());
        throw RuntimeError("Get adaptation time constant failed: " + std::string(e.what()), "NeuralDynamics");
    }
}

} // namespace nlm
