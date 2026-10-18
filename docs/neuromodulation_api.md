# NLM Neuromodulation API

## Overview

The Neuromodulation module implements the brain's chemical signaling systems that regulate neural activity, learning, and behavior. Neuromodulators act as global regulators, influencing how the brain processes information, learns from experience, and adapts its behavior based on environmental demands and internal states.

## Key Features

- **Global modulation**: Wide-ranging effects on neural processing across multiple brain regions
- **Learning regulation**: Control over synaptic plasticity and learning rates
- **Motivational control**: Generation and regulation of approach/avoidance behaviors
- **Attention modulation**: Allocation of cognitive resources and focus
- **State-dependent processing**: Adaptation of processing based on internal states
- **Context integration**: Modulation based on environmental and behavioral context
- **Homeostatic regulation**: Maintenance of system stability and balance
- **Adaptive plasticity**: Experience-dependent modulation of learning capabilities

## Core Classes

### NeuromodulatorBase Class

Abstract base class for all neuromodulators.

```cpp
class NeuromodulatorBase {
public:
    virtual ~NeuromodulatorBase() = default;
    
    // Core neuromodulation interface
    virtual void update(double timestep) = 0;
    virtual void applyModulation() = 0;
    
    // State management
    virtual void reset() = 0;
    virtual void saveState() = 0;
    virtual void restoreState() = 0;
    
    // Configuration
    virtual void setParameters(const NeuromodulatorParameters& params) = 0;
    virtual NeuromodulatorParameters getParameters() const = 0;
    
    // Statistics
    virtual double getModulationStrength() const = 0;
    virtual double getConcentration() const = 0;
    virtual Timestamp getLastUpdateTime() const = 0;
};
```

### Dopamine Class

Dopamine system regulates reward prediction, motivation, and learning.

```cpp
class Dopamine : public NeuromodulatorBase {
public:
    // Construction and lifecycle
    Dopamine();
    explicit Dopamine(const DopamineParameters& params);
    ~Dopamine() override;
    
    // Core neuromodulation
    void update(double timestep) override;
    void applyModulation() override;
    
    // Reward processing
    void processReward(float reward, float predictionError);
    float calculatePredictionError(float actualReward, float predictedReward);
    
    // Learning rate modulation
    float getLearningRate() const;
    void updateLearningRate(float predictionError);
    
    // Motivation regulation
    float getMotivationLevel() const;
    void setMotivationalValue(float value);
    void addMotivationalBias(float bias);
    
    // Action selection
    float evaluateActionValue(const Action& action) const;
    void updateActionValues(const std::vector<Action>& actions, float reward);
    
    // Neural modulation
    void modulateNeurons(const std::vector<NeuronId>& neurons);
    void modulateSynapticPlasticity(Synapse& synapse, float preActivity, float postActivity);
    
    // State management
    void reset() override;
    void saveState() override;
    void restoreState() override;
    
    // Configuration
    void setParameters(const DopamineParameters& params) override;
    DopamineParameters getParameters() const override;
    
    // Statistics
    double getModulationStrength() const override;
    double getConcentration() const override;
    Timestamp getLastUpdateTime() const override;
    
    // Activity tracking
    void recordActionOutcome(const Action& action, float reward);
    float getAverageReward(const Action& action) const;
    float getTemporalDifferenceError() const;
};
```

### Curiosity Class

Curiosity system drives exploration and information seeking.

```cpp
class Curiosity : public NeuromodulatorBase {
public:
    // Construction and lifecycle
    Curiosity();
    explicit Curiosity(const CuriosityParameters& params);
    ~Curiosity() override;
    
    // Core neuromodulation
    void update(double timestep) override;
    void applyModulation() override;
    
    // Novelty detection
    float detectNovelty(const std::vector<float>& sensoryInput);
    float calculateSurprise(const std::vector<float>& predictions, 
                           const std::vector<float>& observations);
    float calculateExpectedInformationGain(const std::vector<float>& stimulus);
    
    // Exploration regulation
    float getExplorationDrive() const;
    void updateExplorationDrive(float novelty);
    void setExplorationTarget(float target);
    
    // Information seeking
    std::vector<float> identifyInformativeStimuli(const std::vector<float>& currentInput);
    bool isStimulusWorthExploring(const std::vector<float>& stimulus);
    
    // Learning modulation
    void enhanceMemoryEncoding(const MemoryId& memoryId);
    void promoteGeneralization(const Concept& concept);
    
    // State management
    void reset() override;
    void saveState() override;
    void restoreState() override;
    
    // Configuration
    void setParameters(const CuriosityParameters& params) override;
    CuriosityParameters getParameters() const override;
    
    // Statistics
    double getModulationStrength() const override;
    double getConcentration() const override;
    Timestamp getLastUpdateTime() const override;
    
    // Activity tracking
    void recordNovelty(float novelty);
    float getAverageNovelty() const;
    size_t getExplorationCount() const;
};
```

### Novelty Class

Novelty system detects and responds to changes in the environment.

```cpp
class Novelty : public NeuromodulatorBase {
public:
    // Construction and lifecycle
    Novelty();
    explicit Novelty(const NoveltyParameters& params);
    ~Novelty() override;
    
    // Core neuromodulation
    void update(double timestep) override;
    void applyModulation() override;
    
    // Change detection
    float detectChange(const std::vector<float>& currentState, 
                      const std::vector<float>& expectedState);
    float detectAnomaly(const std::vector<float>& sensoryInput);
    float calculateContextualNovelty(const std::vector<float>& input, 
                                     const std::vector<float>& context);
    
    // Response regulation
    float getNoveltyResponse() const;
    void updateNoveltyResponse(float novelty);
    void setResponseThreshold(float threshold);
    
    // Adaptation
    void adaptToNoveltyLevel(float noveltyLevel);
    void calibrateResponseSensitivity(float sensitivity);
    
    // Integration
    void enhanceAttention(const std::vector<NeuronId>& neurons);
    void modifyBehavior(const Action& action, float novelty);
    
    // State management
    void reset() override;
    void saveState() override;
    void restoreState() override;
    
    // Configuration
    void setParameters(const NoveltyParameters& params) override;
    NoveltyParameters getParameters() const override;
    
    // Statistics
    double getModulationStrength() const override;
    double getConcentration() const override;
    Timestamp getLastUpdateTime() const override;
    
    // Activity tracking
    void recordNoveltyDetection(float novelty);
    float getAverageNovelty() const;
    size_t getNoveltyEvents() const;
};
```

### PredictionError Class

Prediction error system computes discrepancies between expected and actual outcomes.

```cpp
class PredictionError : public NeuromodulatorBase {
public:
    // Construction and lifecycle
    PredictionError();
    explicit PredictionError(const PredictionErrorParameters& params);
    ~PredictionError() override;
    
    // Core neuromodulation
    void update(double timestep) override;
    void applyModulation() override;
    
    // Error computation
    float computeActionPredictionError(const Action& action, float actualReward);
    float computeSensoryPredictionError(const std::vector<float>& predictedSensory,
                                      const std::vector<float>& actualSensory);
    float computeValuePredictionError(float predictedValue, float actualValue);
    
    // Error propagation
    void propagatePredictionError(const std::vector<NeuronId>& neurons);
    void distributeError(const Action& action, float error);
    void adjustPredictions(const std::vector<float>& predictions, float error);
    
    // Learning modulation
    void updateLearningBasedOnError(float error);
    void enhanceMemoryUpdatingBasedOnError(const MemoryId& memoryId, float error);
    
    // State management
    void reset() override;
    void saveState() override;
    void restoreState() override;
    
    // Configuration
    void setParameters(const PredictionErrorParameters& params) override;
    PredictionErrorParameters getParameters() const override;
    
    // Statistics
    double getModulationStrength() const override;
    double getConcentration() const override;
    Timestamp getLastUpdateTime() const override;
    
    // Activity tracking
    void recordPredictionError(float error);
    float getAveragePredictionError() const;
    float getPredictionErrorVariance() const;
};
```

### Reward Class

Reward system represents and processes reward information.

```cpp
class Reward : public NeuromodulatorBase {
public:
    // Construction and lifecycle
    Reward();
    explicit Reward(const RewardParameters& params);
    ~Reward() override;
    
    // Core neuromodulation
    void update(double timestep) override;
    void applyModulation() override;
    
    // Reward processing
    void processPrimaryReward(float reward);
    void processSecondaryReward(float reward, const Concept& concept);
    void processSocialReward(float reward, const SocialRelationship& relationship);
    
    // Value representation
    float getCurrentValue() const;
    void updateValue(float reward, float predictionError);
    float getExpectedValue(const Action& action) const;
    
    // Hedonic processing
    float getHedonicImpact() const;
    void setHedonicImpact(float impact);
    void applyAnhedonia(float anhedoniaFactor);
    
    // Motivational processing
    float getMotivationForAction(const Action& action) const;
    void updateActionMotivation(const Action& action, float reward);
    
    // State management
    void reset() override;
    void saveState() override;
    void restoreState() override;
    
    // Configuration
    void setParameters(const RewardParameters& params) override;
    RewardParameters getParameters() const override;
    
    // Statistics
    double getModulationStrength() const override;
    double getConcentration() const override;
    Timestamp getLastUpdateTime() const override;
    
    // Activity tracking
    void recordReward(float reward);
    float getAverageReward() const;
    float getRewardVariance() const;
};
```

## Neuromodulation Architecture

### Hierarchical Organization

Neuromodulation systems operate at multiple levels:

```
+-----------------------------------------------------------+
|                Global Neuromodulation                   |
|-----------------------------------------------------------|
|  +--------------------+--------------------+            |
|  |   Dopamine System  |   Curiosity System |            |
|  +--------------------+--------------------+            |
|  |   Novelty System   | Prediction Error   |            |
|  +--------------------+--------------------+            |
|  |     Reward System  |                    |            |
|  +--------------------+--------------------+            |
+-----------------------------------------------------------+
         |
         v
+-------------------------------+
|          Neural Modulation      |
| - Synaptic plasticity rules     |
| - Neuron excitability          |
| - Network dynamics             |
| - Memory consolidation         |
+-------------------------------+
         |
         v
+-------------------------------+
|         Behavioral Output       |
| - Action selection             |
| - Motor control               |
| - Attention focus             |
| - Motivational state           |
+-------------------------------+
```

### Cross-System Integration

Neuromodulation systems interact through multiple mechanisms:

1. **Direct Chemical Interactions**: Neuromodulators affect each other's release and effectiveness
2. **Receptor Level**: Shared receptor populations mediate cross-system effects
3. **Neural Circuit Integration**: Neuromodulatory systems converge on common neural targets
4. **Homeostatic Regulation**: Systems regulate each other's activity to maintain balance
5. **Contextual Modulation**: Environmental context influences neuromodulator effectiveness

## Neuromodulation Implementation

### Dopamine Implementation

```cpp
double Dopamine::getModulationStrength() const {
    // Combine tonic and phasic components
    double tonicStrength = calculateTonicConcentration();
    double phasicStrength = calculatePhasicStrength();
    
    return tonicStrength + phasicStrength;
}

double Dopamine::getConcentration() const {
    // Calculate overall dopamine concentration
    double baseline = getBaselineConcentration();
    double phasicRelease = getRecentPhasicRelease();
    
    return baseline + phasicRelease;
}

void Dopamine::applyModulation() {
    // Modulate synaptic plasticity
    for (auto& synapse : synapses_) {
        float modulation = calculateModulationForSynapse(synapse);
        synapse->modulatePlasticity(modulation);
    }
    
    // Modulate neuronal excitability
    for (auto& neuron : neurons_) {
        float modulation = calculateExcitabilityModulation(neuron);
        neuron->modulateExcitability(modulation);
    }
}
```

### Curiosity Implementation

```cpp
float Curiosity::getExplorationDrive() const {
    // Combine novelty sensitivity and intrinsic motivation
    float noveltySensitivity = getNoveltySensitivity();
    float intrinsicMotivation = getIntrinsicMotivation();
    
    return noveltySensitivity * intrinsicMotivation;
}

void Curiosity::updateExplorationDrive(float novelty) {
    // Update exploration drive based on novelty detection
    float noveltyResponse = calculateNoveltyResponse(novelty);
    
    // Adapt exploration drive with exponential smoothing
    explorationDrive_ = explorationDrive_ * 0.9 + noveltyResponse * 0.1;
}

std::vector<float> Curiosity::identifyInformativeStimuli(const std::vector<float>& currentInput) {
    // Find stimuli that would provide new information
    std::vector<float> informativeStimuli;
    
    for (const auto& stimulus : potentialStimuli_) {
        float informationGain = calculateInformationGain(currentInput, stimulus);
        if (informationGain > informationThreshold_) {
            informativeStimuli.push_back(stimulus);
        }
    }
    
    return informativeStimuli;
}
```

### Novelty Implementation

```cpp
float Novelty::detectChange(const std::vector<float>& currentState,
                           const std::vector<float>& expectedState) {
    // Calculate change detection using multiple measures
    float magnitudeChange = calculateMagnitudeChange(currentState, expectedState);
    float patternChange = calculatePatternChange(currentState, expectedState);
    float contextualChange = calculateContextualChange(currentState, expectedState);
    
    // Combine measures with weights
    float novelty = magnitudeChange * 0.4 + 
                   patternChange * 0.3 + 
                   contextualChange * 0.3;
    
    return std::min(novelty, 1.0f); // Cap at 1.0
}

void Novelty::adaptToNoveltyLevel(float noveltyLevel) {
    // Adapt response thresholds based on novelty level
    if (noveltyLevel > HIGH_NOVELTY_THRESHOLD) {
        // Reduce response sensitivity to prevent overload
        responseThreshold_ *= 1.5;
    } else if (noveltyLevel < LOW_NOVELTY_THRESHOLD) {
        // Increase response sensitivity to promote exploration
        responseThreshold_ *= 0.8;
    }
}
```

### PredictionError Implementation

```cpp
float PredictionError::computeActionPredictionError(const Action& action, float actualReward) {
    // Get predicted reward for action
    float predictedReward = getPredictedReward(action);
    
    // Calculate temporal difference error
    float predictionError = actualReward - predictedReward;
    
    // Apply reward decay for temporal difference
    predictionError *= std::exp(-timeConstant_ * timestep_);
    
    return predictionError;
}

void PredictionError::applyModulation() {
    // Modulate learning based on prediction error
    float errorMagnitude = std::abs(getCurrentPredictionError());
    
    if (errorMagnitude > THRESHOLD) {
        // Strong error - enhance learning
        enhanceLearningRate(errorMagnitude);
    } else if (errorMagnitude < SMALL_THRESHOLD) {
        // Weak error - reduce learning
        reduceLearningRate(errorMagnitude);
    }
    
    // Distribute error to relevant neural populations
    distributeErrorToNeurons(errorMagnitude);
}
```

### Reward Implementation

```cpp
float Reward::getCurrentValue() const {
    // Calculate current reward value based on history
    if (rewardHistory_.empty()) {
        return 0.0f;
    }
    
    // Use exponentially weighted average
    float totalWeight = 0.0f;
    float weightedSum = 0.0f;
    
    float weight = 1.0f;
    for (const auto& reward : rewardHistory_) {
        weightedSum += reward * weight;
        totalWeight += weight;
        weight *= rewardDecayRate_;
    }
    
    return weightedSum / totalWeight;
}

void Reward::processPrimaryReward(float reward) {
    // Process primary reward (survival-related)
    primaryRewardSum_ += reward;
    primaryRewardCount_++;
    
    // Update value function
    updateValueFunction(reward, 1.0f); // High learning rate for primary rewards
    
    // Record reward for statistics
    recordReward(reward);
}

void Reward::processSecondaryReward(float reward, const Concept& concept) {
    // Process secondary reward (learned associations)
    secondaryRewardSum_ += reward;
    secondaryRewardCount_++;
    
    // Update concept value
    updateConceptValue(concept, reward);
    
    // Record reward for statistics
    recordReward(reward);
}
```

## Integration with Other Modules

### Neural System Integration

```cpp
class Brain {
public:
    // Neuromodulation system access
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionError();
    Reward* getReward();
    
    // Neuromodulation coordination
    void coordinateNeuromodulation();
    void updateNeuromodulationBasedOnState();
    void applyNeuromodulationToNeuralSystems();
};
```

### Memory System Integration

```cpp
void Brain::coordinateNeuromodulation() {
    // Get neuromodulators
    auto dopamine = getDopamine();
    auto curiosity = getCuriosity();
    auto novelty = getNovelty();
    auto predictionError = getPredictionError();
    auto reward = getReward();
    
    // Integrate with memory systems
    auto workingMemory = getWorkingMemory();
    auto episodicMemory = getEpisodicMemory();
    auto semanticMemory = getSemanticMemory();
    
    // Dopamine enhances memory encoding
    dopamine->enhanceMemoryEncoding(workingMemory->getCurrentFocus());
    
    // Curiosity promotes exploration and novel encoding
    curiosity->promoteGeneralization(semanticMemory->getCurrentConcept());
    
    // Novelty enhances attention to new information
    novelty->enhanceAttention(workingMemory->getActiveNeurons());
    
    // Prediction error updates memory
    predictionError->enhanceMemoryUpdatingBasedOnError(workingMemory->getCurrentMemoryId());
    
    // Reward reinforces valued memories
    reward->enhanceMemoryRetention(workingMemory->getCurrentMemoryId());
}
```

### Cognitive System Integration

```cpp
void Brain::applyNeuromodulationToNeuralSystems() {
    // Get neuromodulators
    auto dopamine = getDopamine();
    auto curiosity = getCuriosity();
    auto novelty = getNovelty();
    auto predictionError = getPredictionError();
    auto reward = getReward();
    
    // Get neural systems
    auto neuralSystem = getNeuralSystem();
    auto cognitiveSystem = getCognitiveSystem();
    
    // Apply neuromodulation to neural systems
    dopamine->modulateNeurons(neuralSystem->getAllNeurons());
    dopamine->modulateSynapticPlasticity(neuralSystem->getSynapses());
    
    // Apply neuromodulation to cognitive systems
    curiosity->enhanceConceptFormation(cognitiveSystem->getConceptFormation());
    novelty->enhanceAttentionSelection(cognitiveSystem->getAttention());
    predictionError->enhancePlanning(cognitiveSystem->getPlanner());
    reward->enhanceDecisionMaking(cognitiveSystem->getDecisionMaker());
}
```

## Performance Optimization

### Computational Efficiency

```cpp
// Batch processing of neuromodulation
class NeuromodulationBatch {
public:
    // Process multiple neuromodulators efficiently
    static void processAllNeuromodulators(std::vector<NeuromodulatorBase*>& neuromodulators,
                                          double timestep);
    
    // Apply modulation to neural populations in batch
    static void applyModulationToNeurons(const std::vector<NeuronId>& neurons,
                                        std::vector<NeuromodulatorBase*>& neuromodulators);
    
    // Optimize modulation parameters
    static void optimizeModulationParameters(std::vector<NeuromodulatorBase*>& neuromodulators);
};
```

### Memory Management

```cpp
// Memory pools for neuromodulator states
class NeuromodulatorStatePool {
public:
    static NeuromodulatorState* allocate();
    static void deallocate(NeuromodulatorState* state);
    static void clear();
    static size_t getUsedCount();
};
```

### Parallel Processing

```cpp
// Thread-safe parallel neuromodulation processing
class ParallelNeuromodulationProcessor {
public:
    static void processNeuromodulators(std::vector<NeuromodulatorBase*>& neuromodulators,
                                       std::function<void(NeuromodulatorBase*)> processor);
    
    static void applyModulationToNeuralSystems(const std::vector<NeuronId>& neurons,
                                              std::vector<NeuromodulatorBase*>& neuromodulators);
};
```

## Thread Safety

### Read-Only Operations

```cpp
// Thread-safe read operations
float Dopamine::getLearningRate() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return learningRate_;
}

float Curiosity::getExplorationDrive() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return explorationDrive_;
}
```

### Write Operations

```cpp
// Write operations with proper synchronization
void Dopamine::updateLearningRate(float predictionError) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Calculate new learning rate based on prediction error
    float newLearningRate = learningRate_ * (1.0f + predictionError * learningRateModulation_);
    
    // Apply bounds
    newLearningRate = std::max(MIN_LEARNING_RATE, 
                              std::min(newLearningRate, MAX_LEARNING_RATE));
    
    learningRate_ = newLearningRate;
}
```

## Error Handling

### Neuromodulation Errors

```cpp
enum class NeuromodulationError {
    Success = 0,
    InvalidParameters = 1,
    NeuralSystemNotAvailable = 2,
    PlasticityUpdateFailed = 3,
    MemoryAccessFailed = 4,
    ConcentrationOutOfBounds = 5,
    IntegrationFailed = 6
};
```

### Error Handling Pattern

```cpp
void Dopamine::applyModulation() {
    try {
        // Validate neuromodulator state
        if (!isActive()) {
            throw NeuromodulationError::NeuralSystemNotAvailable;
        }
        
        // Check concentration bounds
        if (getConcentration() > MAX_CONCENTRATION) {
            throw NeuromodulationError::ConcentrationOutOfBounds;
        }
        
        // Apply neuromodulation
        modulateNeurons(neurons_);
        modulateSynapticPlasticity(synapses_);
        
    } catch (const NeuromodulationError& e) {
        // Handle error
        std::cerr << "Neuromodulation error: " << static_cast<int>(e) << std::endl;
        throw;
    }
}
```

## Usage Examples

### Basic Neuromodulation Setup

```cpp
#include <nlm/neuromodulation/Dopamine.hpp>
#include <nlm/neuromodulation/Curiosity.hpp>

int main() {
    // Create neuromodulation systems
    auto dopamine = std::make_shared<nlm::Dopamine>();
    auto curiosity = std::make_shared<nlm::Curiosity>();
    auto novelty = std::make_shared<nlm::Novelty>();
    auto predictionError = std::make_shared<nlm::PredictionError>();
    auto reward = std::make_shared<nlm::Reward>();
    
    // Configure parameters
    nlm::DopamineParameters dopamineParams;
    dopamineParams.baselineConcentration = 0.1f;
    dopamineParams.phasicRelease = 0.5f;
    dopamine->setParameters(dopamineParams);
    
    nlm::CuriosityParameters curiosityParams;
    curiosityParams.noveltySensitivity = 0.8f;
    curiosityParams.explorationTarget = 0.3f;
    curiosity->setParameters(curiosityParams);
    
    return 0;
}
```

### Neuromodulation in Simulation

```cpp
#include <nlm/brain/Brain.hpp>

int main() {
    // Create brain with neuromodulation
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Run simulation with neuromodulation
    for (int step = 0; step < 1000; ++step) {
        // Get neuromodulators
        auto dopamine = brain->getDopamine();
        auto curiosity = brain->getCuriosity();
        auto novelty = brain->getNovelty();
        auto predictionError = brain->getPredictionError();
        auto reward = brain->getReward();
        
        // Update neuromodulators
        dopamine->update(0.001);
        curiosity->update(0.001);
        novelty->update(0.001);
        predictionError->update(0.001);
        reward->update(0.001);
        
        // Apply neuromodulation
        dopamine->applyModulation();
        curiosity->applyModulation();
        novelty->applyModulation();
        predictionError->applyModulation();
        reward->applyModulation();
        
        // Run neural simulation
        brain->step(step, step * 0.001);
        
        // Check for reward signals
        if (step % 100 == 0) {
            float rewardValue = reward->getCurrentValue();
            std::cout << "Simulation step " << step 
                      << ", Reward: " << rewardValue << std::endl;
        }
    }
    
    return 0;
}
```

### Integration with Learning

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/neuromodulation/Dopamine.hpp>

void learnFromExperience(Brain& brain, const Experience& experience) {
    // Get dopamine system
    auto dopamine = brain.getDopamine();
    
    // Calculate reward prediction error
    float predictedReward = dopamine->getPredictedReward(experience.action);
    float predictionError = experience.reward - predictedReward;
    
    // Update dopamine system
    dopamine->processReward(experience.reward, predictionError);
    
    // Update action values
    dopamine->updateActionValues(brain.getAvailableActions(), experience.reward);
}
```

## Testing and Validation

### Unit Testing

```cpp
#include <nlm/neuromodulation/Dopamine.hpp>
#include <gtest/gtest.h>

TEST(DopamineTest, LearningRateUpdate) {
    auto dopamine = std::make_shared<nlm::Dopamine>();
    
    // Set initial learning rate
    float initialLearningRate = dopamine->getLearningRate();
    
    // Process reward with prediction error
    dopamine->processReward(1.0f, 0.5f);
    
    // Check updated learning rate
    float updatedLearningRate = dopamine->getLearningRate();
    
    EXPECT_NE(initialLearningRate, updatedLearningRate);
}

TEST(CuriosityTest, ExplorationDriveUpdate) {
    auto curiosity = std::make_shared<nlm::Curiosity>();
    
    // Set initial exploration drive
    float initialExploration = curiosity->getExplorationDrive();
    
    // Detect novelty
    std::vector<float> sensoryInput = {0.5f, 0.8f, 0.3f};
    float novelty = curiosity->detectNovelty(sensoryInput);
    
    // Update exploration drive
    curiosity->updateExplorationDrive(novelty);
    
    // Check updated exploration drive
    float updatedExploration = curiosity->getExplorationDrive();
    
    EXPECT_NE(initialExploration, updatedExploration);
}
```

### Performance Testing

```bash
// Run neuromodulation performance tests
./benchmark --neuromodulation --test dopamine

// Profile neuromodulation overhead
./profiler --target neuromodulation --operation update
```

## Migration Guide

### From Earlier Versions

Neuromodulation API changes:

- **Version 0.1.0**: Basic reward system only
- **Version 0.2.0**: Added dopamine and curiosity systems
- **Version 0.3.0**: Added novelty and prediction error systems
- **Version 0.4.0**: Enhanced reward system with multiple reward types
- **Version 0.5.0**: Improved neuromodulation integration and error handling
- **Version 0.6.0**: Enhanced performance optimization and parallel processing

### API Changes

- **System architecture**: Changed from single neuromodulator to multiple integrated systems
- **Modulation mechanisms**: Expanded from reward-only to comprehensive neuromodulation
- **Integration**: Enhanced integration with neural and cognitive systems
- **Performance optimization**: Added batch processing and parallel processing
- **Thread safety**: Added proper synchronization for concurrent access
- **Error handling**: Enhanced error handling and validation

## References

1. [Dopamine in Reinforcement Learning](https://en.wikipedia.org/wiki/Dopamine_in_reinforcement_learning)
2. [Curiosity-Driven Learning](https://en.wikipedia.org/wiki/Curiosity-driven_learning)
3. [Novelty Detection](https://en.wikipedia.org/wiki/Novelty_detection)
4. [Prediction Error](https://en.wikipedia.org/wiki/Prediction_error)
5. [Reward Processing](https://en.wikipedia.org/wiki/Reward_system_(neuroscience)

## See Also

- [Brain API](brain_api.md) - Brain integration with neuromodulation
- [Memory API](memory_api.md) - Memory system integration
- [Cognition API](cognition_api.md) - Cognitive system integration
- [Development API](development_api.md) - Development system integration
- [Config API](config_api.md) - Configuration for neuromodulation
- [Logging API](logging_api.md) - Logging system integration

**Last updated:** October 18, 2026
**Version:** 0.6.0