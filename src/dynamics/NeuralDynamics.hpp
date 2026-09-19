#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <string>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Config;

/**
 * @brief Enumeration of available integrate-and-fire parameter keys
 * 
 * These configuration keys are used to configure the IntegrateAndFireDynamics
 * class through the Config system. All parameters have biologically realistic
 * defaults that can be overridden via configuration file or command line.
 */
enum class IAParameterKey {
    // Neuron membrane parameters
    MembraneTimeConstant,      // Membrane time constant (ms)
    MembraneResistance,         // Membrane resistance (MOhm)
    RestingPotential,           // Resting membrane potential (mV)
    ThresholdPotential,         // Firing threshold (mV)
    ResetPotential,             // Post-spike reset potential (mV)
    Capacitance,                // Membrane capacitance (nF)
    
    // Synaptic reversal potentials
    ReversalPotentialExc,       // Excitatory reversal potential (mV)
    ReversalPotentialInh,       // Inhibitory reversal potential (mV)
    
    // Spike-frequency adaptation
    AdaptationConductance,      // AHP conductance (nS)
    AdaptationTimeConstant,     // Adaptation time constant (ms)
    AdaptationSlope,            // Adaptation slope factor (mV/nS)
    
    // Refractory period
    RefractoryPeriod,           // Refractory period duration (steps)
    PostSpikeAdaptation,       // Post-spike hyperpolarization (mV)
    
    // Short-term plasticity
    STPFacilitationTime,       // STP facilitation time constant (ms)
    STPDepressionTime,         // STP depression time constant (ms)
    STPUmax,                    // Maximum utilization probability
    
    // Configuration
    ConfigFile,                 // Configuration file path
    EnableParameterValidation    // Enable parameter validation checks
};

/**
 * @brief Abstract base class for neural dynamics implementations
 * 
 * This class defines the interface for all neural dynamics models.
 * Concrete implementations include IntegrateAndFireDynamics, HodgkinHuxleyDynamics,
 * and FitzHughNagumoDynamics.
 * 
 * The NeuralDynamics class provides the fundamental operations required for
 * neural simulation:
 * - Updating neuron state based on dynamics
 * - Updating synapse state based on plasticity rules
 * - Applying synaptic inputs to postsynaptic neurons
 * - Detecting and handling spikes
 * - Resetting dynamics state
 * 
 * @tparam TimestepDuration Type representing timestep duration in seconds
 * 
 * @see IntegrateAndFireDynamics For the primary LIF implementation
 * @see HodgkinHuxleyDynamics For detailed ionic current model
 * @see FitzHughNagumoDynamics For simplified excitation model
 */
class NeuralDynamics {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~NeuralDynamics() = default;
    
    /**
     * @brief Update neuron state for one timestep
     * 
     * This is the main dynamics update function that computes the new state
     * of a neuron based on its current state and inputs.
     * 
     * @param neuron Pointer to neuron to update
     * @param dt Timestep duration in seconds
     * 
     * @pre neuron != nullptr
     * @post Neuron state updated according to dynamics model
     */
    virtual void updateNeuron(Neuron* neuron, TimestepDuration dt) = 0;
    
    /**
     * @brief Update synapse state for one timestep
     * 
     * Updates synaptic properties including short-term plasticity,
     * eligibility traces, and other use-dependent dynamics.
     * 
     * @param synapse Pointer to synapse to update
     * @param dt Timestep duration in seconds
     * 
     * @pre synapse != nullptr
     * @post Synapse state updated according to dynamics model
     */
    virtual void updateSynapse(Synapse* synapse, TimestepDuration dt) = 0;
    
    /**
     * @brief Apply synaptic input from spike
     * 
     * Translates spike events into synaptic currents or conductances
     * at postsynaptic neurons. Implements conductance-based models
     * with proper reversal potential handling.
     * 
     * @param neuron Pointer to postsynaptic neuron
     * @param synapse Pointer to synapse delivering input
     * 
     * @pre neuron != nullptr and synapse != nullptr
     * @post Postsynaptic neuron receives appropriate input
     */
    virtual void applySpikeInput(Neuron* neuron, const Synapse* synapse) = 0;
    
    /**
     * @brief Check if neuron should fire
     * 
     * Determines whether the neuron has reached firing threshold
     * and is not in refractory period. This check is typically called
     * after updating neuron membrane potential.
     * 
     * @param neuron Pointer to neuron to check
     * @return true if neuron should fire, false otherwise
     * 
     * @pre neuron != nullptr
     */
    virtual bool shouldFire(const Neuron* neuron) const = 0;
    
    /**
     * @brief Reset dynamics state
     * 
     * Resets any internal state variables to their initial values.
     * This is called at the beginning of simulations or when restarting.
     */
    virtual void reset() = 0;
    
    /**
     * @brief Configure dynamics from configuration object
     * 
     * Loads parameters from a Config object, allowing runtime
     * configuration of all dynamics parameters. Validates parameters
     * and provides warnings for invalid or missing values.
     * 
     * @param config Shared pointer to configuration object
     * @return true if configuration successful, false otherwise
     */
    virtual bool configureFromConfig(const std::shared_ptr<Config>& config) = 0;
    
    /**
     * @brief Get parameter value by key
     * 
     * Retrieves a parameter value by its configuration key.
     * This allows runtime inspection of dynamics parameters.
     * 
     * @param key Configuration key
     * @return Current parameter value as string, empty string if not found
     */
    virtual std::string getParameter(IAParameterKey key) const = 0;
    
    /**
     * @brief Set parameter value by key
     * 
     * Sets a parameter value by its configuration key.
     * Performs parameter validation before applying changes.
     * 
     * @param key Configuration key
     * @param value String representation of parameter value
     * @return true if parameter set successfully, false otherwise
     */
    virtual bool setParameter(IAParameterKey key, const std::string& value) = 0;
    
    /**
     * @brief Get configuration summary
     * 
     * Returns a string summary of all current parameters and their values.
     * Useful for debugging and logging.
     * 
     * @return Configuration summary string
     */
    virtual std::string getConfigSummary() const = 0;
};

/**
 * @brief Integrate-and-fire dynamics (simple model) with full biological realism
 * 
 * This class implements a comprehensive Leaky Integrate-and-Fire (LIF) neuron model
 * enhanced with biological features including:
 * - Exponential Euler integration for stable membrane potential dynamics
 * - Spike detection with refractory period and reset
 * - Conductance-based synaptic integration with reversal potentials
 * - Spike-frequency adaptation via adaptive conductance
 * - Short-term plasticity (Tsodyks-Markram model) for synaptic dynamics
 * - Delay-based synaptic transmission
 * - Configuration-driven parameters with validation
 * 
 * @tparam TimestepDuration Type representing timestep duration in seconds
 * 
 * @see https://en.wikipedia.org/wiki/Leaky_integrate-and-fire_model
 * @see Tsodyks, A. V., Markram, H., & Tsodyks, E. (1998). The neural code. Biased nets.
 */
class IntegrateAndFireDynamics : public NeuralDynamics {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an IntegrateAndFireDynamics instance with biologically realistic
     * default parameters.
     */
    IntegrateAndFireDynamics();
    
    /**
     * @brief Destructor
     */
    ~IntegrateAndFireDynamics() override;
    
    /**
     * @brief Disable copying
     */
    IntegrateAndFireDynamics(const IntegrateAndFireDynamics&) = delete;
    IntegrateAndFireDynamics& operator=(const IntegrateAndFireDynamics&) = delete;
    
    /**
     * @brief Enable move construction
     */
    IntegrateAndFireDynamics(IntegrateAndFireDynamics&&) noexcept;
    
    /**
     * @brief Enable move assignment
     */
    IntegrateAndFireDynamics& operator=(IntegrateAndFireDynamics&&) noexcept;
    
    // NeuralDynamics interface implementation
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    bool configureFromConfig(const std::shared_ptr<Config>& config) override;
    std::string getParameter(IAParameterKey key) const override;
    bool setParameter(IAParameterKey key, const std::string& value) override;
    std::string getConfigSummary() const override;
    
    // Legacy interface methods (for backward compatibility)
    void setMembraneTimeConstant(float tau);
    float getMembraneTimeConstant() const;
    
    // Parameter validation and safety methods
    bool validateParameters() const;
    void enforceParameterBounds();
    bool isParameterValid(IAParameterKey key, const std::string& value) const;
    
    // Error handling
    std::string getLastError() const;
    void clearError();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Hodgkin-Huxley dynamics (detailed model)
// TODO PHASE 2: Implement Hodgkin-Huxley
class HodgkinHuxleyDynamics : public NeuralDynamics {
public:
    // PLACEHOLDER - Phase 2 will implement HH dynamics
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override {}
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override {}
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override {}
    bool shouldFire(const Neuron* neuron) const override { return false; }
    void reset() override {}
    bool configureFromConfig(const std::shared_ptr<Config>& config) override { return false; }
    std::string getParameter(IAParameterKey key) const override { return ""; }
    bool setParameter(IAParameterKey key, const std::string& value) override { return false; }
    std::string getConfigSummary() const override { return ""; }
};

// FitzHugh-Nagumo dynamics (simplified excitation model)
// TODO PHASE 2: Implement FHN dynamics
class FitzHughNagumoDynamics : public NeuralDynamics {
public:
    // PLACEHOLDER - Phase 2 will implement FHN dynamics
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override {}
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override {}
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override {}
    bool shouldFire(const Neuron* neuron) const override { return false; }
    void reset() override {}
    bool configureFromConfig(const std::shared_ptr<Config>& config) override { return false; }
    std::string getParameter(IAParameterKey key) const override { return ""; }
    bool setParameter(IAParameterKey key, const std::string& value) override { return false; }
    std::string getConfigSummary() const override { return ""; }
};

} // namespace nlm
