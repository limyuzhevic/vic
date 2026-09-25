# API Reference

## Overview
This document provides complete API documentation for all NLM classes and methods, organized by module.

## API Structure

### Core Module
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Configuration management system
- **Random**: Deterministic random number generator
- **Logger**: Structured logging system
- **SimulationClock**: Time management for simulation

### Brain Module
- **Neuron**: Individual neuron representation (LIF dynamics)
- **Synapse**: Synaptic connection representation
- **NeuralPopulation**: Group of neurons with shared properties
- **NeuralRegion**: Brain region with multiple populations
- **Brain**: Central coordinator

### Dynamics Module
- **NeuralDynamics**: Abstract base for neuron dynamics
- **SpikeSystem**: Event-driven spike processing

### Plasticity Module
- **PlasticityRule**: Abstract base for plasticity rules
- **Hebbian**: Hebbian learning rule
- **STDP**: Spike-Timing-Dependent Plasticity
- **StructuralPlasticity**: Synapse formation/removal

### Development Module
- **DevelopmentSystem**: Orchestrates development
- **Synaptogenesis**: New synapse formation
- **Pruning**: Weak synapse removal
- **Maturation**: Adult property development

### Neuromodulation Module
- **Neuromodulator**: Abstract base for neuromodulators
- **Dopamine**: Reward and reinforcement
- **Novelty**: Novelty detection
- **Curiosity**: Exploration motivation
- **PredictionError**: Error signals

### Memory Module
- **NeuralWorkingMemory**: Persistent activity for temporary storage
- **NeuralEpisodicMemory**: Experience encoding in neural patterns
- **NeuralAssociativeMemory**: Experience-based associations

### Prediction Module
- **NeuralPrediction**: Temporal sequence learning
- **PredictionSystem**: Action effect prediction

### Cognition Module
- **ConceptFormation**: Pattern discovery without labels
- **NeuralPlanner**: Predictive action selection

### Sensory Module
- **SensoryInput**: Base class for sensory data
- **Vision**: Visual input processing
- **Audio**: Audio input processing
- **InternalSignals**: Internal state signals

### Motor Module
- **Action**: Action representation
- **MotorSystem**: Neural-to-action conversion

### Environment Module
- **Environment**: Abstract environment interface
- **Observation**: Sensory observation wrapper

### Experiments Module
- **Experiment**: Single experiment recording
- **ExperimentRunner**: Experiment execution
- **Metrics**: Data analysis

## API Documentation Index

### Brain Module
- [Brain](#brain-module)
- [Neuron](#neuron)
- [Synapse](#synapse)
- [NeuralPopulation](#neuralpopulation)
- [NeuralRegion](#neuralregion)

### Core Module
- [Types](#types)
- [Config](#config)
- [Random](#random)
- [Logger](#logger)
- [SimulationClock](#simulationclock)

### Plasticity Module
- [PlasticityRule](#plasticityrule)
- [Hebbian](#hebbian)
- [STDP](#stdp)
- [StructuralPlasticity](#structuralplasticity)

### Development Module
- [DevelopmentSystem](#developmentsystem)
- [Synaptogenesis](#synaptogenesis)
- [Pruning](#pruning)
- [Maturation](#maturation)

### Neuromodulation Module
- [Neuromodulator](#neuromodulator)
- [Dopamine](#dopamine)
- [Novelty](#novelty)
- [Curiosity](#curiosity)
- [PredictionError](#predictionerror)

### Memory Module
- [NeuralWorkingMemory](#neuralworkingmemory)
- [NeuralEpisodicMemory](#neuralepisodicmemory)
- [Memory](#memory)

### Prediction Module
- [NeuralPrediction](#neuralprediction)
- [PredictionSystem](#predictionsystem)

### Cognition Module
- [ConceptFormation](#conceptformation)
- [NeuralPlanner](#neuralplanner)

### Sensory Module
- [SensoryInput](#sensoryinput)
- [Vision](#vision)
- [Audio](#audio)
- [InternalSignals](#internalsignals)

### Motor Module
- [Action](#action)
- [MotorSystem](#motorsystem)

### Environment Module
- [Environment](#environment)
- [Observation](#observation)

### Experiments Module
- [Experiment](#experiment)
- [ExperimentRunner](#experimentrunner)
- [Metrics](#metrics)

## Class Documentation

### Brain Module

#### Brain
The central coordinator of all neural processes, managing neurons, synapses, plasticity rules, development, and neuromodulation.

**Header**: `src/brain/Brain.hpp`
**Methods**:
- `Brain(NeuronCount neuronCount)` - Constructor
- `~Brain()` - Destructor
- `void addNeuron(std::unique_ptr<Neuron> neuron)` - Add a neuron
- `void addSynapse(std::unique_ptr<Synapse> synapse)` - Add a synapse
- `void step(SimulationStep step)` - Advance simulation by one step
- `void enablePlasticity(bool enable)` - Enable/disable plasticity
- `void enableDevelopment(bool enable)` - Enable/disable development
- `void enableNeuromodulation(bool enable)` - Enable/disable neuromodulation

#### Neuron
Individual neuron representing Leaky Integrate-and-Fire (LIF) dynamics with refractory periods and adaptation.

**Header**: `src/brain/Neuron.hpp`
**Constructor**: `Neuron(NeuronId id)`
**Key Properties**:
- Membrane potential (mV)
- Firing threshold (mV)
- Refractory period (steps)
- Firing rate (Hz)
- Spike history

**Key Methods**:
- `void updateMembranePotential(float delta)` - Update membrane potential
- `bool checkThreshold()` - Check if neuron should fire
- `void fire()` - Handle spike event
- `void resetRefractory()` - Reset refractory period
- `void addToRefractoryPeriod(uint32_t steps)` - Extend refractory period

#### Synapse
Synaptic connection between neurons with weight, delay, and plasticity capabilities.

**Header**: `src/brain/Synapse.hpp`
**Constructor**: `Synapse(SynapseId id, NeuronId source, NeuronId destination)`
**Key Properties**:
- Weight (SynapticWeight)
- Delay (in simulation steps)
- Type (excitatory/inhibitory)
- Pre/post spike history
- Plasticity flags

**Key Methods**:
- `void update(Timestamp currentTime)` - Update synaptic dynamics
- `void applyPlasticity()` - Apply plasticity rules
- `void recordPreSpike(Timestamp timestamp)` - Record pre-synaptic spike
- `void recordPostSpike(Timestamp timestamp)` - Record post-synaptic spike
- `float getEfficacy()` - Get synaptic efficacy

#### NeuralPopulation
Group of neurons with shared properties and collective dynamics.

**Header**: `src/brain/NeuralPopulation.hpp`
**Constructor**: `NeuralPopulation(PopulationId id, const NeuronConfig& config)`
**Key Properties**:
- Population ID
- Neuron type
- Connectivity pattern
- Collective firing rate

**Key Methods**:
- `void addNeuron(std::unique_ptr<Neuron> neuron)` - Add neuron to population
- `void removeNeuron(NeuronId id)` - Remove neuron from population
- `void updateConnections()` - Update connectivity
- `float getCollectiveFiringRate()` - Get population firing rate
- `void broadcastSpikes()` - Broadcast spikes to other populations

#### NeuralRegion
Brain region containing multiple neural populations with regional interactions.

**Header**: `src/brain/NeuralRegion.hpp`
**Constructor**: `NeuralRegion(RegionId id)`
**Key Properties**:
- Region ID
- List of populations
- Regional coupling strengths
- Regional neuromodulation

**Key Methods**:
- `void addPopulation(std::unique_ptr<NeuralPopulation> population)` - Add population
- `void updateRegionalInteractions()` - Update regional coupling
- `void applyRegionalModulation()` - Apply regional neuromodulation
- `void synchronizePopulations()` - Synchronize population activity

### Core Module

#### Types
Strongly-typed identifiers for safety and debugging.

**Header**: `src/core/Types/Types.hpp`
**Key Types**:
- `NeuronId` - Neuron identifier
- `SynapseId` - Synapse identifier
- `RegionId` - Region identifier
- `PopulationId` - Population identifier
- `SimulationStep` - Step counter
- `Timestamp` - Time in seconds
- `TimestepDuration` - Time step duration
- `SynapticWeight` - Synaptic weight type
- `MembranePotential` - Membrane potential type
- `FiringRate` - Firing rate type

#### Config
Configuration management system with file I/O, validation, and schema checking.

**Header**: `src/core/Config/Config.hpp`
**Key Methods**:
- `bool loadFromFile(const std::string& filepath)` - Load from JSON file
- `bool saveToFile(const std::string& filepath) const` - Save to JSON file
- `std::optional<T> get<T>(const std::string& key) const` - Get typed value
- `void set(const std::string& key, const ConfigValue& value)` - Set value
- `void validate() const` - Validate configuration
- `std::vector<std::string> getValidationErrors() const` - Get validation errors

#### Random
Deterministic random number generator with seed management.

**Header**: `src/core/Random/Random.hpp`
**Key Methods**:
- `RandomGenerator()` - Default constructor
- `RandomGenerator(uint64_t seed)` - Constructor with seed
- `void setSeed(uint64_t seed)` - Set seed
- `float uniform()` - Uniform random number [0, 1)
- `float normal(float mean, float stddev)` - Normal random number
- `int integer(int min, int max)` - Integer random number
- `bool Bernoulli(float probability)` - Bernoulli trial

#### Logger
Structured logging system with configurable levels and output.

**Header**: `src/core/Logger/Logger.hpp`
**Key Methods**:
- `void setLevel(LogLevel level)` - Set logging level
- `void info(const std::string& message)` - Log info message
- `void warn(const std::string& message)` - Log warning
- `void error(const std::string& message)` - Log error
- `void debug(const std::string& message)` - Log debug message
- `bool isLevelEnabled(LogLevel level) const` - Check if level is enabled
- `void setOutput(std::unique_ptr<LogOutput> output)` - Set log output

#### SimulationClock
Time management for simulation with high precision.

**Header**: `src/core/SimulationClock/SimulationClock.hpp`
**Key Methods**:
- `SimulationClock(TimestepDuration timestep)` - Constructor
- `~SimulationClock()` - Destructor
- `void advance()` - Advance simulation time
- `Timestamp getTime() const` - Get current time
- `SimulationStep getStep() const` - Get current step
- `TimestepDuration getTimestep() const` - Get timestep
- `void setTimestep(TimestepDuration timestep)` - Set timestep

### Plasticity Module

#### PlasticityRule
Abstract base class for all plasticity rules.

**Header**: `src/plasticity/PlasticityRule.hpp`
**Abstract Methods**:
- `virtual void update(Synapse* synapse, const std::vector<Timestamp>& preSpikes, const std::vector<Timestamp>& postSpikes, TimestepDuration dt) = 0`
- `virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0`
- `virtual const char* getName() const = 0`

**Key Methods**:
- `bool isEnabled() const` - Check if rule is enabled
- `void setEnabled(bool enabled)` - Enable/disable rule

#### Hebbian
Hebbian learning rule implementing "neurons that fire together, wire together".

**Header**: `src/plasticity/Hebbian.hpp`
**Key Methods**:
- `Hebbian(float learningRate)` - Constructor with learning rate
- `void setLearningRate(float rate)` - Set learning rate
- `float getLearningRate() const` - Get learning rate

#### STDP
Spike-Timing-Dependent Plasticity rule with LTP/LTD mechanisms.

**Header**: `src/plasticity/STDP.hpp`
**Key Methods**:
- `STDP(float ltpWeight, float ltdWeight, float timeConstant)` - Constructor
- `void setLTPWeight(float weight)` - Set LTP weight
- `void setLTDWeight(float weight)` - Set LTD weight
- `void setTimeConstant(float tau)` - Set time constant
- `void configure(float ltpWeight, float ltdWeight, float tau)` - Configure from parameters

#### StructuralPlasticity
Manages synapse formation and removal (synaptogenesis and pruning).

**Header**: `src/plasticity/StructuralPlasticity.hpp`
**Key Methods**:
- `void setFormationRate(float rate)` - Set synapse formation rate
- `void setPruningRate(float rate)` - Set synapse pruning rate
- `void setCriticalPeriod(float start, float end)` - Set critical period
- `void update(SimulationStep step)` - Update structural changes

### Development Module

#### DevelopmentSystem
Orchestrates developmental processes including synaptogenesis, pruning, and maturation.

**Header**: `src/development/DevelopmentSystem.hpp`
**Key Methods**:
- `void advanceDevelopmentalStage()` - Advance to next developmental stage
- `float getDevelopmentalProgress() const` - Get developmental progress
- `void setPlasticityRate(float rate)` - Set plasticity rate
- `void enableCriticalPeriod(bool enable)` - Enable critical period

#### Synaptogenesis
Manages formation of new synapses during development.

**Header**: `src/development/Synaptogenesis.hpp`
**Key Methods**:
- `void setFormationProbability(float probability)` - Set formation probability
- `void setMinimumReceptorDensity(float density)` - Set minimum receptor density
- `void update(SimulationStep step)` - Update synaptogenesis

#### Pruning
Manages removal of weak synapses during development.

**Header**: `src/development/Pruning.hpp`
**Key Methods**:
- `void setPruningThreshold(float threshold)` - Set pruning threshold
- `void setActivityDependent(bool activityDependent)` - Set activity-dependent pruning
- `void update(SimulationStep step)` - Update pruning

#### Maturation
Manages development of adult-level properties.

**Header**: `src/development/Maturation.hpp`
**Key Methods**:
- `void setMaturationRate(float rate)` - Set maturation rate
- `void setMaturationThreshold(float threshold)` - Set maturation threshold
- `bool isMature() const` - Check if mature
- `void update(SimulationStep step)` - Update maturation

### Neuromodulation Module

#### Neuromodulator
Abstract base class for neuromodulatory signals.

**Header**: `src/neuromodulation/Neuromodulator.hpp`
**Abstract Methods**:
- `virtual const char* getName() const = 0`
- `virtual float getLevel() const = 0`
- `virtual void setLevel(float level) = 0`
- `virtual float getPlasticityFactor() const = 0`
- `virtual void update(TimestepDuration dt) = 0`

#### Dopamine
Reward and reinforcement learning signal.

**Header**: `src/neuromodulation/Dopamine.hpp`
**Key Methods**:
- `void signalReward(float reward)` - Signal reward
- `void signalPredictionError(float error)` - Signal prediction error
- `void setBaselineLevel(float level)` - Set baseline level

#### Novelty
Detects novel events and triggers exploration.

**Header**: `src/neuromodulation/Novelty.hpp`
**Key Methods**:
- `void setDetectionThreshold(float threshold)` - Set detection threshold
- `void setAdaptationRate(float rate)` - Set adaptation rate
- `bool isNovel(const std::vector<float>& input)` - Check if input is novel

#### Curiosity
Exploration motivation based on novelty and prediction error.

**Header**: `src/neuromodulation/Curiosity.hpp`
**Key Methods**:
- `void setExplorationStrength(float strength)` - Set exploration strength
- `void setExploitationStrength(float strength)` - Set exploitation strength
- `float getMotivation() const` - Get exploration motivation

#### PredictionError
Error signals for predictive processing.

**Header**: `src/neuromodulation/PredictionError.hpp`
**Key Methods**:
- `void setErrorThreshold(float threshold)` - Set error threshold
- `void setLearningRate(float rate)` - Set learning rate
- `float computeError(float predicted, float actual)` - Compute prediction error

### Memory Module

#### NeuralWorkingMemory
Persistent activity for temporary information storage.

**Header**: `src/memory/NeuralWorkingMemory.hpp`
**Key Methods**:
- `void store(const std::vector<float>& pattern)` - Store pattern
- `std::vector<float> retrieve()` - Retrieve pattern
- `void updateDecay(float decayRate)` - Update decay rate
- `void activate()` - Activate working memory
- `void deactivate()` - Deactivate working memory

#### NeuralEpisodicMemory
Experience encoding in long-term neural patterns.

**Header**: `src/memory/NeuralEpisodicMemory.hpp`
**Key Methods**:
- `void encode(const std::vector<float>& experience)` - Encode experience
- `std::vector<float> retrieve(float timestamp)` - Retrieve by timestamp
- `void replay(float replayRate)` - Replay experiences
- `void consolidate()` - Consolidate into long-term memory

#### Memory
High-level memory management interface.

**Header**: `src/memory/Memory.hpp`
**Key Methods**:
- `void store(const std::string& key, const std::vector<float>& value)` - Store value
- `std::optional<std::vector<float>> retrieve(const std::string& key)` - Retrieve value
- `void clear(const std::string& key)` - Clear value
- `void clearAll()` - Clear all values
- `void update(float dt)` - Update memory systems

### Prediction Module

#### NeuralPrediction
Temporal sequence learning and prediction.

**Header**: `src/prediction/NeuralPrediction.hpp`
**Key Methods**:
- `void learn(const std::vector<float>& sequence)` - Learn sequence
- `std::vector<float> predict(const std::vector<float>& state)` - Predict next state
- `void setPredictionHorizon(int horizon)` - Set prediction horizon
- `void setConfidenceThreshold(float threshold)` - Set confidence threshold

#### PredictionSystem
Action effect prediction and consequence modeling.

**Header**: `src/prediction/PredictionSystem.hpp`
**Key Methods**:
- `void learnActionEffect(const std::vector<float>& action, const std::vector<float>& effect)` - Learn action-effect pairing
- `std::vector<float> predictConsequence(const std::vector<float>& action)` - Predict consequence
- `float getPredictionConfidence(const std::vector<float>& action)` - Get prediction confidence

### Cognition Module

#### ConceptFormation
Pattern discovery and abstraction from experiences.

**Header**: `src/cognition/ConceptFormation.hpp`
**Key Methods**:
- `void learn(const std::vector<float>& experience)` - Learn from experience
- `std::vector<float> getConcept(const std::vector<float>& input)` - Get concept representation
- `void setClusteringThreshold(float threshold)` - Set clustering threshold
- `void setConceptStability(float stability)` - Set concept stability

#### NeuralPlanner
Predictive action selection and planning.

**Header**: `src/cognition/NeuralPlanner.hpp`
**Key Methods**:
- `void setPlanningHorizon(int horizon)` - Set planning horizon
- `std::vector<float> plan(const std::vector<float>& currentState)` - Generate plan
- `void setActionSelectionStrategy(ActionSelectionStrategy strategy)` - Set strategy
- `float evaluateAction(const std::vector<float>& action)` - Evaluate action

### Sensory Module

#### SensoryInput
Base class for all sensory inputs.

**Header**: `src/sensory/SensoryInput.hpp`
**Abstract Methods**:
- `virtual void update(TimestepDuration dt) = 0`
- `virtual std::vector<float> getInput() const = 0`
- `virtual bool isActive() const = 0`

#### Vision
Visual input processing and feature extraction.

**Header**: `src/sensory/Vision.hpp`
**Key Methods**:
- `void setResolution(int width, int height)` - Set resolution
- `void setFieldOfView(float fov)` - Set field of view
- `std::vector<float> getFeatures() const` - Get visual features

#### Audio
Audio input processing and sound analysis.

**Header**: `src/sensory/Audio.hpp`
**Key Methods**:
- `void setSampleRate(int sampleRate)` - Set sample rate
- `void setFrequencyRange(float min, float max)` - Set frequency range
- `std::vector<float> getFeatures() const` - Get audio features

#### InternalSignals
Internal state signals and neuromodulatory information.

**Header**: `src/sensory/InternalSignals.hpp`
**Key Methods**:
- `void setNeuronActivity(const std::vector<float>& activity)` - Set neuron activity
- `void setModulationSignals(const std::vector<float>& signals)` - Set modulation signals
- `std::vector<float> getInternalState() const` - Get internal state

### Motor Module

#### Action
Action representation and motor planning.

**Header**: `src/motor/Action.hpp`
**Key Methods**:
- `void setActionParameters(const std::vector<float>& params)` - Set action parameters
- `std::vector<float> getActionVector() const` - Get action vector
- `void setActionStrength(float strength)` - Set action strength

#### MotorSystem
Neural-to-action conversion and motor output generation.

**Header**: `src/motor/MotorSystem.hpp`
**Key Methods**:
- `void setActionSelection(NeuralPlanner* planner)` - Set action planner
- `std::vector<float> generateAction(const std::vector<float>& neuralState)` - Generate action
- `void setActionConstraints(const std::vector<float>& constraints)` - Set action constraints

### Environment Module

#### Environment
Abstract environment interface for simulation environments.

**Header**: `src/environment/Environment.hpp`
**Abstract Methods**:
- `virtual void update(TimestepDuration dt) = 0`
- `virtual std::vector<float> getObservation() const = 0`
- `virtual bool isEpisodeDone() const = 0`
- `virtual void reset() = 0`

#### Observation
Sensory observation wrapper with preprocessing.

**Header**: `src/environment/Observation.hpp`
**Key Methods**:
- `void setSensoryInput(SensoryInput* input)` - Set sensory input
- `std::vector<float> getObservation() const` - Get processed observation
- `void setObservationSize(size_t size)` - Set observation size
- `void normalize(float mean, float stddev)` - Normalize observation

### Experiments Module

#### Experiment
Single experiment recording and data collection.

**Header**: `src/experiments/Experiment.hpp`
**Key Methods**:
- `void start()` - Start experiment
- `void stop()` - Stop experiment
- `void record(const std::string& name, float value)` - Record metric
- `std::vector<Measurement> getResults() const` - Get experiment results
- `void reset() - Reset experiment data

#### ExperimentRunner
Experiment execution and orchestration.

**Header**: `src/experiments/ExperimentRunner.hpp`
**Key Methods**:
- `void addExperiment(std::unique_ptr<Experiment> experiment)` - Add experiment
- `void runAll()` - Run all experiments
- `void runExperiment(const std::string& name)` - Run specific experiment
- `void setConcurrencyLevel(int level)` - Set concurrency level
- `void saveResults(const std::string& filepath)` - Save results to file

#### Metrics
Data analysis and statistical computation.

**Header**: `src/experiments/Metrics.hpp`
**Key Methods**:
- `void addMeasurement(const Measurement& measurement)` - Add measurement
- `float computeMean() const` - Compute mean
- `float computeStddev() const` - Compute standard deviation
- `float computeMin() const` - Compute minimum
- `float computeMax() const` - Compute maximum

## API Usage Examples

### Basic Brain Simulation
```cpp
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"

int main() {
    // Create and configure brain
    auto config = std::make_unique<Config>();
    config->set("brain.neuron_count", 1000);
    config->set("brain.region_count", 1);
    
    auto brain = std::make_unique<Brain>(std::move(config));
    
    // Run simulation
    for (SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step);
    }
    
    return 0;
}
```

### Memory System Usage
```cpp
#include "src/memory/Memory.hpp"

int main() {
    Memory memory;
    
    // Store and retrieve data
    std::vector<float> data = {0.1f, 0.5f, 0.9f, 0.3f};
    memory.store("example_data", data);
    
    auto retrieved = memory.retrieve("example_data");
    if (retrieved) {
        // Use retrieved data
        for (float value : *retrieved) {
            std::cout << value << std::endl;
        }
    }
    
    return 0;
}
```

### Plasticity System Usage
```cpp
#include "src/plasticity/STDP.hpp"
#include "src/plasticity/Hebbian.hpp"

int main() {
    // Create plasticity rules
    auto stdpRule = std::make_unique<STDP>(0.01f, 0.015f, 20.0f);
    auto hebbianRule = std::make_unique<Hebbian>(0.001f);
    
    // Enable plasticity in brain
    // brain->enablePlasticity(true);
    
    return 0;
}
```

### Development System Usage
```cpp
#include "src/development/DevelopmentSystem.hpp"

int main() {
    DevelopmentSystem devSystem;
    
    // Enable development
    devSystem.enableDevelopment(true);
    
    // Advance developmental stages
    for (int stage = 0; stage < 10; ++stage) {
        devSystem.advanceDevelopmentalStage();
        // Adjust plasticity based on developmental stage
        float plasticityRate = devSystem.getDevelopmentalProgress() * 0.1f;
        // Apply plasticity rate
    }
    
    return 0;
}
```

### Neuromodulation Usage
```cpp
#include "src/neuromodulation/Dopamine.hpp"
#include "src/neuromodulation/Curiosity.hpp"

int main() {
    Dopamine dopamine;
    Curiosity curiosity;
    
    // Configure neuromodulators
    dopamine.setBaselineLevel(0.5f);
    curiosity.setExplorationStrength(0.7f);
    curiosity.setExploitationStrength(0.3f);
    
    // Simulation loop
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Update neuromodulators
        dopamine.update(0.01f);
        curiosity.update(0.01f);
        
        // Check for reward and update dopamine
        if (/* reward condition */) {
            dopamine.signalReward(1.0f);
        }
        
        // Check for novelty and update curiosity
        if (/* novelty condition */) {
            // Curiosity system will automatically increase exploration
        }
    }
    
    return 0;
}
```

### Prediction System Usage
```cpp
#include "src/prediction/NeuralPrediction.hpp"
#include "src/prediction/PredictionSystem.hpp"

int main() {
    NeuralPrediction prediction;
    PredictionSystem predictionSystem;
    
    // Configure prediction systems
    prediction.setPredictionHorizon(20);
    prediction.setConfidenceThreshold(0.8f);
    
    predictionSystem.setPredictionHorizon(20);
    
    // Learn sequences
    std::vector<float> sequence = {0.1f, 0.3f, 0.7f, 0.9f, 0.5f};
    prediction.learn(sequence);
    
    // Make predictions
    std::vector<float> currentState = {0.2f, 0.4f, 0.6f};
    std::vector<float> predictedNext = prediction.predict(currentState);
    
    return 0;
}
```

### Cognition System Usage
```cpp
#include "src/cognition/NeuralPlanner.hpp"
#include "src/cognition/ConceptFormation.hpp"

int main() {
    NeuralPlanner planner;
    ConceptFormation conceptFormation;
    
    // Configure cognition systems
    planner.setPlanningHorizon(10);
    planner.setActionSelectionStrategy(ActionSelectionStrategy::EPISODIC);
    
    conceptFormation.setClusteringThreshold(0.5f);
    conceptFormation.setConceptStability(0.9f);
    
    // Learn concepts from experiences
    std::vector<float> experience = {0.1f, 0.5f, 0.9f, 0.3f, 0.7f};
    conceptFormation.learn(experience);
    
    // Generate action plans
    std::vector<float> currentState = {0.3f, 0.6f, 0.2f};
    std::vector<float> plan = planner.plan(currentState);
    
    return 0;
}
```

### Sensory System Usage
```cpp
#include "src/sensory/Vision.hpp"
#include "src/sensory/Audio.hpp"

int main() {
    Vision vision;
    Audio audio;
    
    // Configure sensory systems
    vision.setResolution(640, 480);
    vision.setFieldOfView(60.0f);
    
    audio.setSampleRate(44100);
    audio.setFrequencyRange(20.0f, 20000.0f);
    
    // Simulation loop
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Update sensory inputs
        vision.update(0.01f);
        audio.update(0.01f);
        
        // Get sensory features
        std::vector<float> visualFeatures = vision.getFeatures();
        std::vector<float> audioFeatures = audio.getFeatures();
        
        // Process sensory data
        // (e.g., feed to brain system)
    }
    
    return 0;
}
```

### Motor System Usage
```cpp
#include "src/motor/MotorSystem.hpp"
#include "src/motor/Action.hpp"

int main() {
    MotorSystem motorSystem;
    Action action;
    
    // Configure motor system
    motorSystem.setActionSelection(/* neural planner */);
    motorSystem.setActionConstraints({/* constraint values */});
    
    // Generate actions
    std::vector<float> neuralState = {0.1f, 0.5f, 0.9f};
    std::vector<float> motorAction = motorSystem.generateAction(neuralState);
    
    // Use action
    action.setActionParameters(motorAction);
    action.setActionStrength(1.0f);
    
    // Execute action
    // (send to environment)
    
    return 0;
}
```

### Environment System Usage
```cpp
#include "src/environment/SimpleWorld.hpp"

int main() {
    SimpleWorld environment;
    
    // Configure environment
    environment.setWidth(100);
    environment.setHeight(100);
    environment.setObstacles(/* obstacle list */);
    
    // Reset environment
    environment.reset();
    
    // Simulation loop
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Update environment
        environment.update(0.01f);
        
        // Get observations
        std::vector<float> observation = environment.getObservation();
        
        // Check if episode is done
        if (environment.isEpisodeDone()) {
            environment.reset();
        }
    }
    
    return 0;
}
```

### Complete Experiment Usage
```cpp
#include "src/experiments/ExperimentRunner.hpp"
#include "src/experiments/Metrics.hpp"

int main() {
    ExperimentRunner runner;
    
    // Add metrics calculator
    auto metrics = std::make_unique<Metrics>();
    runner.addMetricCalculator(std::move(metrics));
    
    // Add experiments
    for (int i = 0; i < 5; ++i) {
        auto experiment = std::make_unique<Experiment>("Experiment_" + std::to_string(i));
        // Configure experiment
        runner.addExperiment(std::move(experiment));
    }
    
    // Run all experiments
    runner.runAll();
    
    // Save results
    runner.saveResults("experiment_results.json");
    
    return 0;
}
```

## API Summary

This API reference provides comprehensive documentation for all NLM classes and methods. The API is organized into the following modules:

1. **Core**: Basic utilities and configuration
2. **Brain**: Neural computation and dynamics
3. **Plasticity**: Learning rules and synaptic changes
4. **Development**: Developmental processes
5. **Neuromodulation**: Modulatory signals
6. **Memory**: Working and episodic memory
7. **Prediction**: Predictive processing
8. **Cognition**: Higher-level cognitive processes
9. **Sensory**: Sensory input processing
10. **Motor**: Motor output generation
11. **Environment**: Simulation environments
12. **Experiments**: Experiment framework

The API is designed to be modular, extensible, and follows object-oriented principles. All classes use smart pointers for memory management and provide clean interfaces for neural simulation.