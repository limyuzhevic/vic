# NLM Neuromodulation API Reference

## Overview

The Neuromodulation API provides access to NLM's comprehensive neuromodulation systems. Neuromodulation is a crucial component of NLM that enables:

1. **Reward Learning**: Use rewards to guide behavior
2. **Curiosity-Driven Exploration**: Seek novel and interesting experiences
3. **Novelty Detection**: Identify and respond to new situations
4. **Attention Regulation**: Focus processing on relevant information
5. **Arousal Modulation**: Adjust overall system responsiveness
6. **Motivation Control**: Balance exploration vs. exploitation

## Dopamine System API

### DopamineModulator Class

The `DopamineModulator` class (in `src/neuromodulation/DopamineModulator.hpp` and `src/neuromodulation/DopamineModulator.cpp`) provides dopamine-based reward prediction error signaling.

#### DopamineModulator(std::shared_ptr<Brain> brain)

**Description**: Creates DopamineModulator with brain reference.

**Parameters**:
- `brain`: Reference to brain instance

#### float getDopamineLevel() const

**Description**: Gets current dopamine level.

**Returns**: Dopamine level (0.0-1.0, where higher values indicate stronger reward signals).

**Example**:
```cpp
float dopamine = modulator->getDopamineLevel();
```

#### void updateDopamine(float reward, float predicted_reward)

**Description**: Updates dopamine level based on reward prediction error.

**Parameters**:
- `reward`: Actual received reward
- `predicted_reward`: Predicted reward

**Example**:
```cpp
modulator->updateDopamine(actual_reward, predicted_reward);
```

#### float getPredictionError() const

**Description**: Gets current prediction error (dopamine encodes this).

**Returns**: Prediction error (0.0-1.0).

**Example**:
```cpp
float error = modulator->getPredictionError();
```

#### void enableDopamineLearning(bool enable)

**Description**: Enables or disables dopamine-based learning.

**Parameters**:
- `enable`: Whether to enable dopamine learning

**Example**:
```cpp
modulator->enableDopamineLearning(true);
```

#### void setDopamineScale(float scale)

**Description**: Sets dopamine scale factor.

**Parameters**:
- `scale`: Dopamine scale (typically 0.5-2.0)

**Example**:
```cpp
modulator->setDopamineScale(1.5f);
```

### DopamineModulatorStats Structure

```cpp
struct DopamineModulatorStats {
    float current_level;                // Current dopamine level (0.0-1.0)
    float prediction_error;             // Current prediction error
    float learning_efficiency;          // Learning efficiency (0.0-1.0)
    float reward_response;              // Reward responsiveness (0.0-1.0)
    float adaptation_speed;             // Speed of adaptation (0.0-1.0)
    float confidence_influence;         // Influence of confidence on dopamine
};
```

## Curiosity System API

### CuriosityModulator Class

The `CuriosityModulator` class (in `src/neuromodulation/CuriosityModulator.hpp` and `src/neuromodulation/CuriosityModulator.cpp`) provides curiosity-driven exploration.

#### CuriosityModulator(std::shared_ptr<Brain> brain)

**Description**: Creates CuriosityModulator with brain reference.

**Parameters**:
- `brain`: Reference to brain instance

#### float getCuriosityLevel() const

**Description**: Gets current curiosity level.

**Returns**: Curiosity level (0.0-1.0, where higher values indicate stronger exploration drive).

**Example**:
```cpp
float curiosity = modulator->getCuriosityLevel();
```

#### void updateCuriosity(float entropy, float novelty)

**Description**: Updates curiosity level based on environmental uncertainty and novelty.

**Parameters**:
- `entropy`: Environmental entropy (uncertainty)
- `novelty`: Environmental novelty

**Example**:
```cpp
modulator->updateCuriosity(0.7f, 0.8f);
```

#### float getExplorationDrive() const

**Description**: Gets current exploration drive.

**Returns**: Exploration drive (0.0-1.0).

**Example**:
```cpp
float exploration = modulator->getExplorationDrive();
```

#### void enableCuriosity(bool enable)

**Description**: Enables or disables curiosity system.

**Parameters**:
- `enable`: Whether to enable curiosity

**Example**:
```cpp
modulator->enableCuriosity(true);
```

#### void setCuriosityThreshold(float threshold)

**Description**: Sets curiosity activation threshold.

**Parameters**:
- `threshold`: Minimum curiosity level for activation (0.0-1.0)

**Example**:
```cpp
modulator->setCuriosityThreshold(0.5f);
```

#### void setCuriosityEntropyWeight(float weight)

**Description**: Sets entropy weight for curiosity calculation.

**Parameters**:
- `weight`: Entropy weight (typically 0.5-1.0)

**Example**:
```cpp
modulator->setCuriosityEntropyWeight(0.8f);
```

### CuriosityModulatorStats Structure

```cpp
struct CuriosityModulatorStats {
    float current_level;                // Current curiosity level (0.0-1.0)
    float exploration_drive;            // Current exploration drive (0.0-1.0)
    float entropy_sensitivity;          // Sensitivity to entropy (0.0-1.0)
    float novelty_sensitivity;           // Sensitivity to novelty (0.0-1.0)
    float adaptation_efficiency;         // Adaptation efficiency (0.0-1.0)
    float information_gain;             // Information gain from exploration (0.0-1.0)
};
```

## Novelty Detection System API

### NoveltyDetector Class

The `NoveltyDetector` class (in `src/neuromodulation/NoveltyDetector.hpp` and `src/neuromodulation/NoveltyDetector.cpp`) provides novelty detection and surprise signaling.

#### NoveltyDetector(std::shared_ptr<Brain> brain)

**Description**: Creates NoveltyDetector with brain reference.

**Parameters**:
- `brain`: Reference to brain instance

#### float detectNovelty(const std::vector<float>& input) const

**Description**: Detects novelty in input pattern.

**Parameters**:
- `input`: Input pattern to analyze

**Returns**: Novelty score (0.0-1.0, higher values indicate more novelty).

**Example**:
```cpp
std::vector<float> input = {0.5f, 0.7f, 0.3f, 0.8f, 0.4f};
float novelty = detector->detectNovelty(input);
```

#### void updateNovelty(float novelty, float surprise_threshold)

**Description**: Updates novelty detection based on surprise threshold.

**Parameters**:
- `novelty`: Novelty score
- `surprise_threshold`: Threshold for surprise response

**Example**:
```cpp
modulator->updateNovelty(novelty_score, 0.7f);
```

#### float getNoveltyLevel() const

**Description**: Gets current novelty level.

**Returns**: Novelty level (0.0-1.0).

**Example**:
```cpp
float novelty = detector->getNoveltyLevel();
```

#### void enableNoveltyDetection(bool enable)

**Description**: Enables or disables novelty detection.

**Parameters**:
- `enable`: Whether to enable novelty detection

**Example**:
```cpp
modulator->enableNoveltyDetection(true);
```

#### void setNoveltySensitivity(float sensitivity)

**Description**: Sets novelty sensitivity.

**Parameters**:
- `sensitivity`: Novelty sensitivity (0.0-1.0)

**Example**:
```cpp
modulator->setNoveltySensitivity(0.9f);
```

#### void setSurpriseThreshold(float threshold)

**Description**: Sets surprise threshold.

**Parameters**:
- `threshold`: Surprise threshold (0.0-1.0)

**Example**:
```cpp
modulator->setSurpriseThreshold(0.6f);
```

### NoveltyDetectorStats Structure

```cpp
struct NoveltyDetectorStats {
    float current_level;                // Current novelty level (0.0-1.0)
    float surprise_level;               // Current surprise level (0.0-1.0)
    float detection_accuracy;           // Detection accuracy (0.0-1.0)
    float false_positive_rate;          // False positive rate (0.0-1.0)
    float adaptation_speed;             // Adaptation speed (0.0-1.0)
    float learning_efficiency;          // Learning efficiency (0.0-1.0)
};
```

## Attention System API

### AttentionModulator Class

The `AttentionModulator` class (in `src/neuromodulation/AttentionModulator.hpp` and `src/neuromodulation/AttentionModulator.cpp`) provides attention allocation and selection.

#### AttentionModulator(std::shared_ptr<Brain> brain)

**Description**: Creates AttentionModulator with brain reference.

**Parameters**:
- `brain`: Reference to brain instance

#### float getAttentionLevel() const

**Description**: Gets current attention level.

**Returns**: Attention level (0.0-1.0, where higher values indicate focused attention).

**Example**:
```cpp
float attention = modulator->getAttentionLevel();
```

#### std::vector<float> allocateAttention(const std::vector<float>& stimuli) const

**Description**: Allocates attention across stimuli.

**Parameters**:
- `stimuli`: List of stimulus values

**Returns**: Attention allocation vector (same length as stimuli).

**Example**:
```cpp
std::vector<float> stimuli = {0.8f, 0.3f, 0.6f, 0.2f};
std::vector<float> allocation = modulator->allocateAttention(stimuli);
```

#### float getSelectionStrength() const

**Description**: Gets attention selection strength.

**Returns**: Selection strength (0.0-1.0).

**Example**:
```cpp
float selection = modulator->getSelectionStrength();
```

#### float getInhibitionStrength() const

**Description**: Gets attention inhibition strength.

**Returns**: Inhibition strength (0.0-1.0).

**Example**:
```cpp
float inhibition = modulator->getInhibitionStrength();
```

#### void enableAttention(bool enable)

**Description**: Enables or disables attention system.

**Parameters**:
- `enable`: Whether to enable attention

**Example**:
```cpp
modulator->enableAttention(true);
```

#### void setSelectionStrength(float strength)

**Description**: Sets attention selection strength.

**Parameters**:
- `strength`: Selection strength (0.0-1.0)

**Example**:
```cpp
modulator->setSelectionStrength(0.8f);
```

#### void setInhibitionStrength(float strength)

**Description**: Sets attention inhibition strength.

**Parameters**:
- `strength`: Inhibition strength (0.0-1.0)

**Example**:
```cpp
modulator->setInhibitionStrength(0.6f);
```

### AttentionModulatorStats Structure

```cpp
struct AttentionModulatorStats {
    float current_level;                // Current attention level (0.0-1.0)
    float selection_strength;           // Selection strength (0.0-1.0)
    float inhibition_strength;          // Inhibition strength (0.0-1.0)
    float allocation_efficiency;         // Allocation efficiency (0.0-1.0)
    float focus_stability;              // Focus stability (0.0-1.0)
    float distraction_resistance;        // Distraction resistance (0.0-1.0)
};
```

## Neuromodulation Integration API

### NeuromodulationManager Class

The `NeuromodulationManager` class (in `src/neuromodulation/NeuromodulationManager.hpp` and `src/neuromodulation/NeuromodulationManager.cpp`) provides integrated neuromodulation management.

#### NeuromodulationManager(std::shared_ptr<Brain> brain)

**Description**: Creates NeuromodulationManager with brain reference.

**Parameters**:
- `brain`: Reference to brain instance

#### float getNeuromodulationLevel(const std::string& neuromodulator) const

**Description**: Gets level of specific neuromodulator.

**Parameters**:
- `neuromodulator`: Neuromodulator name ("dopamine", "curiosity", "novelty", "attention")

**Returns**: Neuromodulator level (0.0-1.0).

**Example**:
```cpp
float dopamine = manager->getNeuromodulationLevel("dopamine");
float curiosity = manager->getNeuromodulationLevel("curiosity");
```

#### void applyRewardModulation(float reward, float predicted_reward)

**Description**: Applies reward modulation through integrated system.

**Parameters**:
- `reward`: Actual received reward
- `predicted_reward`: Predicted reward

**Example**:
```cpp
manager->applyRewardModulation(actual_reward, predicted_reward);
```

#### void enableNeuromodulation(const std::string& neuromodulator, bool enable)

**Description**: Enables or disables specific neuromodulator.

**Parameters**:
- `neuromodulator`: Neuromodulator name
- `enable`: Whether to enable neuromodulator

**Example**:
```cpp
manager->enableNeuromodulation("curiosity", true);
manager->enableNeuromodulation("novelty", true);
```

#### NeuromodulationStats getNeuromodulationStats() const

**Description**: Gets comprehensive neuromodulation statistics.

**Returns**: Neuromodulation statistics.

**Example**:
```cpp
NeuromodulationStats stats = manager->getNeuromodulationStats();
```

#### float getDopamineEffectOnMemory() const

**Description**: Gets dopamine effect on memory systems.

**Returns**: Dopamine effect on memory (0.0-1.0).

**Example**:
```cpp
float effect = manager->getDopamineEffectOnMemory();
```

#### float getCuriosityEffectOnMemory() const

**Description**: Gets curiosity effect on memory systems.

**Returns**: Curiosity effect on memory (0.0-1.0).

**Example**:
```cpp
float effect = manager->getCuriosityEffectOnMemory();
```

### NeuromodulationStats Structure

```cpp
struct NeuromodulationStats {
    float dopamine_level;               // Dopamine level (0.0-1.0)
    float curiosity_level;              // Curiosity level (0.0-1.0)
    float novelty_level;                // Novelty level (0.0-1.0)
    float attention_level;              // Attention level (0.0-1.0)
    float reward_effectiveness;         // Reward effectiveness (0.0-1.0)
    float exploration_effectiveness;    // Exploration effectiveness (0.0-1.0)
    float adaptation_efficiency;         // Overall adaptation efficiency (0.0-1.0)
    float learning_coherence;           // Learning coherence (0.0-1.0)
};
```

## Python Neuromodulation API

The Python neuromodulation API provides simplified access to NLM neuromodulation systems:

### DopamineModulator

```python
class DopamineModulator:
    def __init__(self, brain):
        self.brain = brain
        
    def get_dopamine_level(self):
        # Get current dopamine level
        
    def update_dopamine(self, reward, predicted_reward):
        # Update dopamine based on reward prediction error
        
    def get_prediction_error(self):
        # Get prediction error
        
    def enable_dopamine_learning(self, enable):
        # Enable or disable dopamine learning
```

### CuriosityModulator

```python
class CuriosityModulator:
    def __init__(self, brain):
        self.brain = brain
        
    def get_curiosity_level(self):
        # Get current curiosity level
        
    def update_curiosity(self, entropy, novelty):
        # Update curiosity based on entropy and novelty
        
    def get_exploration_drive(self):
        # Get exploration drive
        
    def enable_curiosity(self, enable):
        # Enable or disable curiosity system
```

### NoveltyDetector

```python
class NoveltyDetector:
    def __init__(self, brain):
        self.brain = brain
        
    def detect_novelty(self, input):
        # Detect novelty in input pattern
        
    def get_novelty_level(self):
        # Get current novelty level
        
    def enable_novelty_detection(self, enable):
        # Enable or disable novelty detection
```

### AttentionModulator

```python
class AttentionModulator:
    def __init__(self, brain):
        self.brain = brain
        
    def get_attention_level(self):
        # Get current attention level
        
    def allocate_attention(self, stimuli):
        # Allocate attention across stimuli
        
    def get_selection_strength(self):
        # Get attention selection strength
        
    def get_inhibition_strength(self):
        # Get attention inhibition strength
```

### NeuromodulationManager

```python
class NeuromodulationManager:
    def __init__(self, brain):
        self.brain = brain
        self.dopamine_modulator = DopamineModulator(brain)
        self.curiosity_modulator = CuriosityModulator(brain)
        self.novelty_detector = NoveltyDetector(brain)
        self.attention_modulator = AttentionModulator(brain)
        
    def get_neuromodulation_level(self, neuromodulator):
        # Get level of specific neuromodulator
        
    def apply_reward_modulation(self, reward, predicted_reward):
        # Apply reward modulation through integrated system
        
    def enable_neuromodulation(self, neuromodulator, enable):
        # Enable or disable specific neuromodulator
        
    def get_neuromodulation_stats(self):
        # Get neuromodulation statistics
```

## Neuromodulation Configuration API

### NeuromodulationConfig Structure

```cpp
struct NeuromodulationConfig {
    bool enable_dopamine;                    // Enable dopamine system
    float dopamine_scale;                    // Dopamine scale factor
    float dopamine_learning_rate;            // Dopamine learning rate
    bool enable_curiosity;                   // Enable curiosity system
    float curiosity_threshold;               // Curiosity activation threshold
    float curiosity_entropy_weight;          // Entropy weight for curiosity
    bool enable_novelty_detection;            // Enable novelty detection
    float novelty_sensitivity;               // Novelty detection sensitivity
    float novelty_surprise_threshold;         // Surprise threshold
    bool enable_attention;                    // Enable attention system
    float attention_selection_strength;      // Attention selection strength
    float attention_inhibition_strength;     // Attention inhibition strength
    bool enable_reward_modulation;           // Enable reward modulation
    bool enable_exploration_modulation;       // Enable exploration modulation
};
```

### Configuration Operations

#### NeuromodulationConfig createDefaultNeuromodulationConfig()

**Description**: Creates default neuromodulation configuration.

**Returns**: Default neuromodulation configuration.

#### NeuromodulationConfig createResearchNeuromodulationConfig()

**Description**: Creates neuromodulation configuration optimized for research.

**Returns**: Research-oriented neuromodulation configuration.

#### void applyNeuromodulationConfig(const NeuromodulationConfig& config)

**Description**: Applies neuromodulation configuration to brain.

**Parameters**:
- `config`: Neuromodulation configuration

## Neuromodulation Example Programs

### Example 1: Dopamine-Based Learning

```cpp
// dopamine_learning_example.cpp
#include <iostream>
#include <vector>
#include "brain/Brain.hpp"
#include "neuromodulation/DopamineModulator.hpp"

int main() {
    // Create brain and dopamine modulator
    auto config = std::make_shared<Config>();
    config->set("neuromod.dopamine.enable", true);
    config->set("neuromod.dopamine.scale", 1.5f);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    auto dopamineModulator = std::make_shared<DopamineModulator>(brain);
    
    // Demonstrate dopamine-based learning
    std::cout << "Dopamine-Based Learning Example" << std::endl;
    std::cout << "==============================" << std::endl;
    
    // Simulate learning trials
    for (int trial = 0; trial < 10; ++trial) {
        // Generate random reward and prediction
        float reward = (trial % 3 == 0) ? 1.0f : 0.2f;  // Occasional large rewards
        float predicted_reward = 0.5f;
        
        // Update dopamine based on prediction error
        float error = reward - predicted_reward;
        dopamineModulator->updateDopamine(reward, predicted_reward);
        
        std::cout << "Trial " << trial << ": Reward=" << reward 
                  << ", Prediction=" << predicted_reward 
                  << ", Error=" << error << std::endl;
        
        float dopamine = dopamineModulator->getDopamineLevel();
        float prediction_error = dopamineModulator->getPredictionError();
        
        std::cout << "  Dopamine level: " << dopamine << std::endl;
        std::cout << "  Prediction error: " << prediction_error << std::endl;
        
        if (error > 0) {
            std::cout << "  → Positive prediction error - dopamine burst!" << std::endl;
        } else if (error < 0) {
            std::cout << "  → Negative prediction error - dopamine dip!" << std::endl;
        } else {
            std::cout << "  → No prediction error - baseline dopamine" << std::endl;
        }
    }
    
    return 0;
}
```

### Example 2: Curiosity-Driven Exploration

```cpp
// curiosity_exploration_example.cpp
#include <iostream>
#include <vector>
#include <random>
#include "brain/Brain.hpp"
#include "neuromodulation/CuriosityModulator.hpp"

int main() {
    // Create brain and curiosity modulator
    auto config = std::make_shared<Config>();
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.curiosity.threshold", 0.4f);
    config->set("neuromod.curiosity.entropy_weight", 0.8f);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    auto curiosityModulator = std::make_shared<CuriosityModulator>(brain);
    
    // Demonstrate curiosity-driven exploration
    std::cout << "Curiosity-Driven Exploration Example" << std::endl;
    std::cout << "====================================" << std::endl;
    
    // Simulate exploration of different environments
    std::vector<std::string> environments = {
        "simple",   // Low entropy, low novelty
        "moderate", // Medium entropy, medium novelty
        "complex",  // High entropy, high novelty
        "chaotic",  // Very high entropy, very high novelty
    };
    
    for (int step = 0; step < 20; ++step) {
        // Select environment
        std::string env = environments[step % environments.size()];
        
        // Calculate environment characteristics
        float entropy, novelty;
        if (env == "simple") {
            entropy = 0.2f; novelty = 0.1f;
        } else if (env == "moderate") {
            entropy = 0.5f; novelty = 0.4f;
        } else if (env == "complex") {
            entropy = 0.8f; novelty = 0.7f;
        } else {
            entropy = 1.0f; novelty = 0.9f;
        }
        
        // Update curiosity
        curiosityModulator->updateCuriosity(entropy, novelty);
        
        // Get exploration behavior
        float curiosity = curiosityModulator->getCuriosityLevel();
        float exploration = curiosityModulator->getExplorationDrive();
        
        std::cout << "Step " << step << ": Environment " << env 
                  << " (entropy: " << entropy << ", novelty: " << novelty << ")" << std::endl;
        std::cout << "  Curiosity level: " << curiosity << std::endl;
        std::cout << "  Exploration drive: " << exploration << std::endl;
        
        // Determine behavior
        if (entropy > 0.5f || novelty > 0.5f) {
            std::cout << "  → Exploration behavior (high curiosity)" << std::endl;
        } else {
            std::cout << "  → Exploitation behavior (low curiosity)" << std::endl;
        }
    }
    
    return 0;
}
```

### Example 3: Novelty Detection

```cpp
// novelty_detection_example.cpp
#include <iostream>
#include <vector>
#include <cmath>
#include "brain/Brain.hpp"
#include "neuromodulation/NoveltyDetector.hpp"

int main() {
    // Create brain and novelty detector
    auto config = std::make_shared<Config>();
    config->set("neuromod.novelty.enable", true);
    config->set("neuromod.novelty.sensitivity", 1.0f);
    config->set("neuromod.novelty.surprise_threshold", 0.7f);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    auto noveltyDetector = std::make_shared<NoveltyDetector>(brain);
    
    // Demonstrate novelty detection
    std::cout << "Novelty Detection Example" << std::endl;
    std::cout << "==========================" << std::endl;
    
    // Store known patterns
    std::vector<std::vector<float>> known_patterns = {
        {0.8f, 0.2f, 0.1f},  // Pattern 1
        {0.3f, 0.7f, 0.2f},  // Pattern 2
        {0.6f, 0.1f, 0.9f},  // Pattern 3
    };
    
    for (int step = 0; step < 15; ++step) {
        // Generate input (with some noise)
        std::vector<float> base_pattern = known_patterns[step % known_patterns.size()];
        std::vector<float> input = base_pattern;
        
        // Add noise to simulate real-world variability
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-0.1f, 0.1f);
        
        for (size_t i = 0; i < input.size(); ++i) {
            input[i] += dis(gen);
        }
        
        // Detect novelty
        float novelty = noveltyDetector->detectNovelty(input);
        
        // Calculate similarities
        float min_similarity = 1.0f;
        for (const auto& pattern : known_patterns) {
            float similarity = 0.0f;
            for (size_t i = 0; i < input.size(); ++i) {
                similarity += std::abs(input[i] - pattern[i]);
            }
            similarity = 1.0f - similarity / input.size();
            min_similarity = std::min(min_similarity, similarity);
        }
        
        std::cout << "Step " << step << ": Input [";
        for (size_t i = 0; i < input.size(); ++i) {
            std::cout << input[i];
            if (i < input.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
        
        std::cout << "  Similarity to known patterns: [";
        for (const auto& pattern : known_patterns) {
            float similarity = 0.0f;
            for (size_t i = 0; i < input.size(); ++i) {
                similarity += std::abs(input[i] - pattern[i]);
            }
            similarity = 1.0f - similarity / input.size();
            std::cout << similarity;
            if (&pattern != &known_patterns.back()) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
        
        std::cout << "  Minimum similarity: " << min_similarity << std::endl;
        
        if (novelty > 0.7f) {
            std::cout << "  → High novelty detected! "
                      << "Surprise level: " << (1.0f - min_similarity) << std::endl;
        } else {
            std::cout << "  → Familiar input" << std::endl;
        }
        
        float novelty_level = noveltyDetector->getNoveltyLevel();
        std::cout << "  Novelty level: " << novelty_level << std::endl;
    }
    
    return 0;
}
```

## Neuromodulation API Reference Summary

| Category | Class | Key Methods |
|----------|-------|-------------|
| Dopamine | `DopamineModulator` | `getDopamineLevel()`, `updateDopamine()`, `getPredictionError()` |
| Curiosity | `CuriosityModulator` | `getCuriosityLevel()`, `updateCuriosity()`, `getExplorationDrive()` |
| Novelty Detection | `NoveltyDetector` | `detectNovelty()`, `getNoveltyLevel()`, `updateNovelty()` |
| Attention | `AttentionModulator` | `getAttentionLevel()`, `allocateAttention()`, `getSelectionStrength()` |
| Integration | `NeuromodulationManager` | `getNeuromodulationLevel()`, `applyRewardModulation()`, `getNeuromodulationStats()` |

## Python Neuromodulation API Reference Summary

| Category | Class | Key Methods |
|----------|-------|-------------|
| Dopamine | `DopamineModulator` | `get_dopamine_level()`, `update_dopamine()`, `get_prediction_error()` |
| Curiosity | `CuriosityModulator` | `get_curiosity_level()`, `update_curiosity()`, `get_exploration_drive()` |
| Novelty Detection | `NoveltyDetector` | `detect_novelty()`, `get_novelty_level()`, `update_novelty()` |
| Attention | `AttentionModulator` | `get_attention_level()`, `allocate_attention()`, `get_selection_strength()` |
| Integration | `NeuromodulationManager` | `get_neuromodulation_level()`, `apply_reward_modulation()`, `get_neuromodulation_stats()` |

## Next Steps

1. **Get Started**: Try the basic examples in `docs/examples/neuromodulation.cpp`
2. **Explore Documentation**: Read `configuration.md` for neuromodulation configuration details
3. **Run Demos**: Execute neuromodulation-specific demo programs
4. **Create Experiments**: Design neuromodulation-specific NLM experiments
5. **Extend Neuromodulation**: Add new neuromodulation features to NLM

The Neuromodulation API provides comprehensive access to NLM's neuromodulation systems. Whether you're interested in dopamine-based reward learning, curiosity-driven exploration, novelty detection, attention allocation, or integrated neuromodulation, the API provides the tools you need to create sophisticated neuromodulation experiments.

---

*Neuromodulation System API Reference*
- **Location**: `docs/api/neuromodulation_api.md`
- **Source**: Various neuromodulation system headers in `src/neuromodulation/`
- **Python Bindings**: Neuromodulation API in Python bindings
- **Documentation**: Complete neuromodulation API reference with examples

*Ready to start using the Neuromodulation API?*

1. **Read the examples**: Check `docs/examples/neuromodulation.cpp`
2. **Try Python API**: Use `configuration.md` for Python examples
3. **Run demo programs**: Execute the built-in demo programs
4. **Create your own**: Design custom neuromodulation NLM experiments
5. **Contribute**: Share your neuromodulation API usage with the community

The Neuromodulation API is essential for NLM's intelligent behavior. It provides access to all neuromodulation systems that enable NLM to:
- **Learn from Rewards**: Use dopamine for reinforcement learning
- **Explore Actively**: Use curiosity for intrinsic motivation
- **Adapt to Novelty**: Use novelty detection for adaptation
- **Focus Attention**: Use attention modulation for selective processing
- **Balance Behavior**: Use multiple neuromodulators for optimal behavior

Your neuromodulation API research will help advance our understanding of how biological neuromodulators support intelligent behavior!

---

*Neuromodulation API enables NLM to:
• Learn from rewards (dopamine)
• Explore actively (curiosity)
• Detect novelty (surprise detection)
• Focus attention (attention modulation)
• Balance behavior (multiple neuromodulators)

Your neuromodulation API research contributions will help advance neural system research!

---