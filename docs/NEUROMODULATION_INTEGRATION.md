# Neuromodulation Integration Documentation

## Overview

Neuromodulation is a critical component of Phase 6 integration, providing the regulatory signals that modulate neural excitability, plasticity, and behavior. The neuromodulation systems work together to create adaptive, goal-directed behavior.

### Key Components

#### Dopamine System
- **Purpose**: Reward prediction error and behavioral vigor
- **Mechanism**: Phasic and tonic signaling
- **Integration**: Modulates neural excitability and plasticity rates

#### Curiosity System  
- **Purpose**: Exploration drive and novelty seeking
- **Mechanism**: Combines novelty detection with prediction error
- **Integration**: Influences action selection and exploration decisions

#### Novelty System
- **Purpose**: Detects and responds to new sensory patterns
- **Mechanism**: Comparison with stored patterns, decay over time
- **Integration**: Provides input to curiosity system for exploration

#### Prediction Error System
- **Purpose**: Computes prediction error for learning
- **Mechanism**: Compares expected vs. actual outcomes
- **Integration**: Drives curiosity and modulates plasticity

## Integration Details

### Neuromodulation Flow
```
Reward/Prediction Error → Dopamine → Neural Excitability
Novelty → Curiosity → Exploration Decisions
Prediction Error → Neuromodulation → Plasticity
```

### System Interactions
- **Dopamine**: Modulates neural excitability, scales STDP learning rates
- **Curiosity**: Uses prediction error to drive exploration
- **Novelty**: Provides exploration signal based on sensory changes
- **Prediction Error**: Updates neuromodulation levels

## API Reference

```cpp
// Dopamine System
brain->getDopamine()->update(timestep);                    // Update each step
float dopamine = brain->getDopamine()->getLevel();          // Get current level
float plasticityMod = brain->getDopamine()->getPlasticityFactor(); // Get plasticity modulation

// Curiosity System
brain->getCuriosity()->update(timestep);                    // Update each step
brain->getCuriosity()->setExplorationDrive(errorMagnitude); // Set exploration drive
float curiosity = brain->getCuriosity()->getExplorationDrive(); // Get curiosity level

// Novelty System
brain->getNovelty()->update(timestep);                      // Update each step
float novelty = brain->getNovelty()->getLevel();              // Get novelty level

// Prediction Error System
brain->getPredictionError()->computeError(actual, predicted); // Compute error
float error = brain->getPredictionError()->getError();        // Get error
float magnitude = brain->getPredictionError()->getMagnitude(); // Get magnitude

// Integration Access
brain->getDopamine()         // Returns dopamine system
brain->getCuriosity()        // Returns curiosity system
brain->getNovelty()          // Returns novelty system
brain->getPredictionError()  // Returns prediction error system
```

## Integration Verification

```cpp
bool testNeuromodulationIntegration() {
    if (!brain->getDopamine()) {
        NLM_LOG_ERROR("Dopamine system not integrated");
        return false;
    }
    
    if (!brain->getCuriosity()) {
        NLM_LOG_ERROR("Curiosity system not integrated");
        return false;
    }
    
    if (!brain->getNovelty()) {
        NLM_LOG_ERROR("Novelty system not integrated");
        return false;
    }
    
    if (!brain->getPredictionError()) {
        NLM_LOG_ERROR("Prediction error system not integrated");
        return false;
    }
    
    // Test neuromodulation effects on neural excitability
    auto* dopamine = brain->getDopamine();
    float initialLevel = dopamine->getLevel();
    
    for (int i = 0; i < 50; ++i) {
        // Inject some sensory activity to trigger neuromodulation
        brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
        brain->step(i, i * 0.001);
        
        // Check if neuromodulation is affecting neural activity
        float dopamineLevel = dopamine->getLevel();
        if (dopamineLevel != initialLevel) {
            NLM_LOG_INFO("Dopamine modulated neural activity: " + std::to_string(initialLevel) + " -> " + std::to_string(dopamineLevel));
            break;
        }
    }
    
    return true;
}
```

## Python API

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Check neuromodulation integration
if brain.getDopamine():
    print("Dopamine: INTEGRATED")
    print(f"Level: {brain.getDopamine().getLevel():.3f}")

if brain.getCuriosity():
    print("Curiosity: INTEGRATED")
    print(f"Exploration Drive: {brain.getCuriosity().getExplorationDrive():.3f}")

if brain.getNovelty():
    print("Novelty: INTEGRATED")
    print(f"Novelty Level: {brain.getNovelty().getLevel():.3f}")

if brain.getPredictionError():
    print("Prediction Error: INTEGRATED")
    print(f"Error: {brain.getPredictionError().getError():.3f}")
```

## Configuration

```cpp
// Neuromodulation system configuration
config->set("brain.neuromod.dopamine.scale", 1.0f);           // Dopamine scaling
config->set("brain.neuromod.curiosity.enable", true);         // Enable curiosity
config->set("brain.neuromod.novelty.enable", true);           // Enable novelty
config->set("brain.prediction.history_size", 100);            // Prediction history
config->set("brain.prediction.confidence_threshold", 0.5f);   // Confidence threshold
```

## Usage Examples

### Basic Neuromodulation Usage
```cpp
// Run simulation with neuromodulation
for (uint64_t step = 0; step < 1000; ++step) {
    // Inject sensory activity
    brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
    
    // Run brain loop (neuromodulation updates)
    brain->step(step, step * 0.001);
    
    if (step % 100 == 0) {
        auto* dopamine = brain->getDopamine();
        auto* curiosity = brain->getCuriosity();
        
        NLM_LOG_INFO("Step " + std::to_string(step) + ": Dopamine=" + std::to_string(dopamine->getLevel()) + 
                    ", Curiosity=" + std::to_string(curiosity->getExplorationDrive()));
    }
}
```

### Neuromodulation with Agent
```cpp
// Agent with neuromodulation integration
agent->enableRewardModulation(true);     // Dopamine learning
agent->enableCuriosity(true);           // Exploration drive

for (uint64_t step = 0; step < 2000; ++step) {
    // Process world interaction
    world->update(0.1f);
    agent->processSensoryInput(world->observe());
    
    // Run brain loop (neuromodulation integrated)
    brain->step(step, step * 0.001);
    
    // Decode action (neuromodulation influences action selection)
    action = agent->decodeMotorCommand();
    world->applyAction(action);
    
    // Apply reward (dopamine computation)
    reward = world->computeReward();
    agent->applyRewardModulation(reward, 0.0f);
    
    if (step % 200 == 0) {
        NLM_LOG_INFO("Step " + std::to_string(step) + ": "
                    "Dopamine=" + std::to_string(agent->getNeuromodulationLevel()) +
                    ", Curiosity=" + std::to_string(agent->getCuriosityLevel()) +
                    ", Novelty=" + std::to_string(agent->getNoveltyLevel()));
    }
}
```

## Integration Summary

Neuromodulation integration in Phase 6 provides:
- **Dopamine-driven learning** through reward prediction error
- **Curiosity-based exploration** for discovering novel experiences
- **Novelty detection** for identifying interesting events
- **Prediction error computation** for adaptive learning

These systems work together to create adaptive behavior that balances exploration and exploitation while learning from experience.
