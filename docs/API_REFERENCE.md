# NLM API Reference

## Overview

This document provides comprehensive API documentation for the NLM (熙然) brain simulation framework, including all major classes, methods, and their parameters.

## Core Brain Architecture

### Brain Class (`src/brain/Brain.hpp`)

The `Brain` class serves as the central coordinator of the neural system, implementing real spiking neural computation with event-driven dynamics and integrated memory, prediction, cognition, and neuromodulation systems.

#### Constructors
- `explicit Brain(std::shared_ptr<Config> config)` - Constructs a brain with configuration
- `Brain(Brain&& other) noexcept` - Move constructor
- `Brain& operator=(Brain&& other) noexcept` - Move assignment operator

#### Core Methods
- `bool initialize()` - Initializes the brain with all neural components
- `void step(SimulationStep currentStep)` - Performs simulation step with step number
- `void step(SimulationStep currentStep, Timestamp currentTime)` - Performs simulation step with time
- `void receiveSensoryInput(const SensoryInput& input)` - Receives sensory input from environment
- `void injectCurrent(NeuronId neuron, MembranePotential current)` - Injects current into specific neuron
- `void injectCurrentToNeurons(NeuronType type, MembranePotential current)` - Injects current into neurons of specific type

#### Accessor Methods
- `SpikeSystem* getSpikeSystem()` - Access to spike system
- `STDP* getSTDP()` - Access to STDP plasticity system
- `Hebbian* getHebbian()` - Access to Hebbian plasticity system
- `StructuralPlasticity* getStructuralPlasticity()` - Access to structural plasticity system
- `float getExcitationInhibitionRatio() const` - Computes excitation/inhibition balance
- `size_t getTotalSpikeCount() const` - Gets total spike count
- `size_t getPendingSpikeEventCount() const` - Gets pending spike events

#### Memory Systems
- `NeuralWorkingMemory* getWorkingMemory()` - Access to working memory system
- `NeuralEpisodicMemory* getEpisodicMemory()` - Access to episodic memory system
- `NeuralAssociativeMemory* getAssociativeMemory()` - Access to associative memory system

#### Prediction System
- `PredictionSystem* getPredictionSystem()` - Access to prediction system

#### Cognition Systems
- `NeuralPlanner* getPlanner()` - Access to neural planner
- `ConceptFormation* getConceptFormation()` - Access to concept formation system
- `AttentionalSelection* getAttention()` - Access to attentional selection system

#### Development System
- `DevelopmentSystem* getDevelopmentSystem()` - Access to development system
- `DevelopmentalStage getDevelopmentalStage() const` - Gets current developmental stage
- `void setDevelopmentalStage(DevelopmentalStage stage)` - Sets developmental stage

#### Neuromodulation Systems
- `Dopamine* getDopamine()` - Access to dopamine system
- `Curiosity* getCuriosity()` - Access to curiosity system
- `Novelty* getNovelty()` - Access to novelty system
- `PredictionError* getPredictionErrorSignal()` - Access to prediction error system

#### Output and Control
- `std::unique_ptr<Action> produceAction()` - Produces motor/action output
- `void applyNeuromodulation(const Neuromodulator& signal)` - Applies neuromodulatory signals
- `void updatePlasticity()` - Updates all plasticity rules
- `void develop()` - Applies developmental changes
- `void reset()` - Resets brain state
- `bool save(const std::string& filepath) const` - Saves brain state to file
- `bool load(const std::string& filepath)` - Loads brain state from file

#### Statistics
- `size_t getTotalNeuronCount() const` - Total neuron count
- `size_t getTotalSynapseCount() const` - Total synapse count
- `size_t getActiveNeuronCount() const` - Active neuron count
- `size_t getFiringNeuronCount() const` - Firing neuron count
- `float getAverageFiringRate() const` - Average firing rate

#### Configuration Access
- `std::shared_ptr<const Config> getConfig() const` - Gets configuration object
- `RandomGenerator* getRandomGenerator()` - Access to random generator
- `void logStatus() const` - Logs brain status

### Neuron Class (`src/brain/Neuron.hpp`)

The `Neuron` class represents individual neurons implementing Leaky Integrate-and-Fire (LIF) dynamics.

#### NeuronState Struct
Contains complete neuron state for efficient storage:
- `MembranePotential membranePotential` - Current membrane potential (mV)
- `MembranePotential restingPotential` - Resting potential (mV)
- `MembranePotential threshold` - Firing threshold (mV)
- `MembranePotential resetPotential` - Post-spike reset value (mV)
- `MembranePotential leakConductance` - Leak conductance (nS)
- `MembranePotential synapseConductance` - Synaptic conductance (nS)
- `FiringRate firingRate` - Current firing rate (Hz)
- `FiringState firingState` - Current firing state
- `uint32_t refractoryRemaining` - Steps remaining in refractory period
- `uint32_t refractoryPeriod` - Total refractory period (steps)
- `float adaptationVariable` - For spike-frequency adaptation
- `float lastSpikeTime` - Timestamp of last spike

#### Constructor
- `explicit Neuron(NeuronId id)` - Constructs neuron with unique ID

#### Core Methods
- `NeuronId getId() const` - Gets neuron ID
- `NeuronType getType() const` - Gets neuron type
- `void setType(NeuronType type)` - Sets neuron type
- `const NeuronState& getState() const` - Gets neuron state (const)
- `NeuronState& getState()` - Gets neuron state (mutable)

#### Membrane Potential
- `MembranePotential getMembranePotential() const` - Gets membrane potential
- `void setMembranePotential(MembranePotential potential)` - Sets membrane potential
- `void addToMembranePotential(MembranePotential delta)` - Adds to membrane potential

#### Firing Threshold
- `MembranePotential getThreshold() const` - Gets firing threshold
- `void setThreshold(MembranePotential threshold)` - Sets firing threshold

#### Firing State
- `bool isFiring() const` - Checks if neuron is firing
- `bool isRefractory() const` - Checks if neuron is in refractory period
- `void setFiringState(FiringState state)` - Sets firing state
- `void setRefractoryPeriod(uint32_t steps)` - Sets refractory period
- `void decrementRefractory()` - Decrements refractory time

#### Rate and Parameters
- `FiringRate getFiringRate() const` - Gets firing rate
- `void setFiringRate(FiringRate rate)` - Sets firing rate
- `MembranePotential getLeakConductance() const` - Gets leak conductance
- `uint32_t getRefractoryPeriod() const` - Gets refractory period

#### State Variables
- `void setRestingPotential(MembranePotential potential)` - Sets resting potential
- `MembranePotential getRestingPotential() const` - Gets resting potential
- `void setResetPotential(MembranePotential potential)` - Sets reset potential
- `MembranePotential getResetPotential() const` - Gets reset potential

#### Spike Detection
- `bool checkThreshold() const` - Checks if neuron should fire
- `float getLastSpikeTime() const` - Gets last spike timestamp

#### Dynamics
- `bool stepLIF(Timestamp currentTime, TimestepDuration dt)` - Performs LIF dynamics
- `void step(Timestamp currentTime)` - Updates neuron for simulation step

#### Inputs
- `void receiveExcitatoryInput(MembranePotential amplitude)` - Receives excitatory input
- `void receiveInhibitoryInput(MembranePotential amplitude)` - Receives inhibitory input
- `void receiveModulatoryInput(MembranePotential amplitude)` - Receives modulatory input
- `void injectCurrent(MembranePotential current)` - Injects external current
- `MembranePotential getTotalCurrent() const` - Gets total current
- `void clearTotalCurrent()` - Clears total current

#### Spike History
- `void recordSpike(Timestamp timestamp)` - Records spike event
- `const std::vector<Timestamp>& getSpikeHistory() const` - Gets spike history
- `void clearSpikeHistory()` - Clears spike history

#### Synapse Management
- `void addIncomingSynapse(SynapseHandle handle)` - Adds incoming synapse
- `void addOutgoingSynapse(SynapseHandle handle)` - Adds outgoing synapse
- `const std::vector<SynapseHandle>& getIncomingSynapses() const` - Gets incoming synapses
- `const std::vector<SynapseHandle>& getOutgoingSynapses() const` - Gets outgoing synapses

#### Plasticity
- `const PlasticityFlags& getPlasticityFlags() const` - Gets plasticity flags (const)
- `PlasticityFlags& getPlasticityFlags()` - Gets plasticity flags (mutable)
- `void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated)` - Enables plasticity rules

#### Region Membership
- `void setRegionId(RegionId region)` - Sets region ID
- `RegionId getRegionId() const` - Gets region ID
- `void setPopulationId(PopulationId population)` - Sets population ID
- `PopulationId getPopulationId() const` - Gets population ID

### Synapse Class (`src/brain/Synapse.hpp`)

The `Synapse` class represents connections between neurons with delays and plasticity.

#### Constructor
- `Synapse(SynapseId id, NeuronId source, NeuronId destination)` - Constructs synapse

#### Identity
- `SynapseId getId() const` - Gets synapse ID
- `NeuronId getSourceNeuron() const` - Gets source neuron
- `NeuronId getDestinationNeuron() const` - Gets destination neuron

#### Weight
- `SynapticWeight getWeight() const` - Gets synaptic weight
- `void setWeight(SynapticWeight weight)` - Sets synaptic weight
- `void addToWeight(SynapticWeight delta)` - Adjusts synaptic weight

#### Delay
- `Delay getDelay() const` - Gets synaptic delay
- `void setDelay(Delay delay)` - Sets synaptic delay

#### Type and Polarity
- `SynapseType getType() const` - Gets synapse type
- `void setType(SynapseType type)` - Sets synapse type
- `bool isExcitatory() const` - Checks if excitatory
- `bool isInhibitory() const` - Checks if inhibitory

#### Spike History
- `void recordPreSpike(Timestamp timestamp)` - Records presynaptic spike
- `void recordPostSpike(Timestamp timestamp)` - Records postsynaptic spike
- `const std::vector<Timestamp>& getPreSpikeHistory() const` - Gets presynaptic history
- `const std::vector<Timestamp>& getPostSpikeHistory() const` - Gets postsynaptic history
- `void clearHistory()` - Clears spike history

#### Plasticity
- `const PlasticityFlags& getPlasticityFlags() const` - Gets plasticity flags (const)
- `PlasticityFlags& getPlasticityFlags()` - Gets plasticity flags (mutable)
- `void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated)` - Enables plasticity rules

#### Eligibility Trace
- `float getEligibilityTrace() const` - Gets eligibility trace
- `void setEligibilityTrace(float trace)` - Sets eligibility trace
- `void decayEligibilityTrace(float decayRate)` - Decays eligibility trace

#### Efficacy
- `float getEfficacy() const` - Gets synaptic efficacy
- `void setEfficacy(float efficacy)` - Sets synaptic efficacy

#### Dynamics
- `void step(Timestamp currentTime)` - Updates synapse for simulation step

#### State Management
- `void reset()` - Resets synapse state
- `void initializeRandom(RandomGenerator& rng)` - Initializes with random parameters

### Config Class (`src/core/Config/Config.hpp`)

The `Config` class provides centralized configuration management.

#### Configuration Sources
- `ConfigSource::Default` - Built-in defaults
- `ConfigSource::File` - File-based configuration
- `ConfigSource::CommandLine` - Command-line arguments
- `ConfigSource::Runtime` - Runtime overrides

#### Value Types
- `int`, `int64_t`, `double`, `bool` - Primitive types
- `std::string` - String type
- `std::vector<int>`, `std::vector<double>`, `std::vector<std::string>` - Array types

#### Core Methods
- `Config()` - Default constructor
- `bool loadFromFile(const std::string& filepath)` - Loads from file
- `bool loadFromArgs(int argc, char** argv)` - Loads from command line
- `bool saveToFile(const std::string& filepath) const` - Saves to file
- `std::optional<T> get(const std::string& key) const` - Gets typed value
- `T getOr(const std::string& key, const T& defaultValue) const` - Gets with default
- `void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime)` - Sets value
- `bool has(const std::string& key) const` - Checks key existence
- `void remove(const std::string& key)` - Removes key
- `std::vector<std::string> getKeys() const` - Gets all keys
- `void clear()` - Clears all configuration
- `std::string summary() const` - Gets configuration summary

### NeuralDynamics Classes

#### NeuralDynamics (Abstract Base)
- `virtual void updateNeuron(Neuron* neuron, TimestepDuration dt) = 0` - Updates neuron
- `virtual void updateSynapse(Synapse* synapse, TimestepDuration dt) = 0` - Updates synapse
- `virtual void applySpikeInput(Neuron* neuron, const Synapse* synapse) = 0` - Applies spike input
- `virtual bool shouldFire(const Neuron* neuron) const = 0` - Checks if should fire
- `virtual void reset() = 0` - Resets dynamics

#### IntegrateAndFireDynamics
- `IntegrateAndFireDynamics()` - Constructor
- `void setMembraneTimeConstant(float tau)` - Sets time constant
- `float getMembraneTimeConstant() const` - Gets time constant

#### HodgkinHuxleyDynamics
- Placeholder for detailed biophysical modeling

#### FitzHughNagumoDynamics
- Simplified excitable system model

### PlasticityRule Classes

#### PlasticityRule (Abstract Base)
- `virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0` - Applies weight change
- `virtual const char* getName() const = 0` - Gets rule name
- `bool isEnabled() const` - Checks if enabled
- `void setEnabled(bool enabled)` - Sets enabled state

#### HebbianRule
- `HebbianRule()` - Constructor
- `void setLearningRate(float rate)` - Sets learning rate
- `float getLearningRate() const` - Gets learning rate

#### AntiHebbianRule
- Implements anti-Hebbian learning

#### BCMRule
- Implements Bienenstock-Cooper-Munro learning rule

## File Structure Summary

### Core Components
- `src/brain/` - Neural computation core
- `src/dynamics/` - Neural dynamics models
- `src/plasticity/` - Plasticity rules
- `src/core/` - Core utilities (Config, Random, Logger)

### Integrated Systems
- `src/memory/` - Memory systems
- `src/prediction/` - Prediction modules
- `src/cognition/` - Cognitive functions
- `src/neuromodulation/` - Neuromodulation signals
- `src/development/` - Developmental processes
- `src/environment/` - Environmental interaction
- `src/sensory/` - Sensory processing
- `src/motor/` - Motor control

## Configuration Parameters

### Neural Parameters
- `neuron_count` - Number of neurons
- `region_count` - Number of brain regions
- `connection_probability` - Synapse creation probability

### Plasticity Parameters
- `stdp_ltp_weight` - STDP long-term potentiation weight
- `stdp_ltd_weight` - STDP long-term depression weight
- `stdp_tau` - STDP time constant
- `synaptogenesis_rate` - Synapse formation rate
- `pruning_rate` - Synapse elimination rate

### Simulation Parameters
- `simulation_timestep` - Time step duration
- `replay_interval` - Memory replay interval
- `consolidation_interval` - Memory consolidation interval

### Randomization
- `random_seed` - Seed for random number generator

## References

- Dayan & Abbott (2001) "Theoretical Neuroscience"
- Maass et al. (2002) "Computational models of spiking neurons"
- Sporns (2011) "Network Biology"
- Sporns (2018) "The complex brain"