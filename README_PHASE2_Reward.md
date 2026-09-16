# Phase 2 Reward System Implementation

## Overview
This file documents the real reward computation implementation in Reward.cpp that addresses all requirements from Phase 2:

### 1. Real-time Reward Computation
- **computeReward(const Observation& observation)**: Main entry point that calculates actual reward values
- **Reward shaping**: Includes goal achievement, resource collection, hazard penalties, and step penalties
- **Reward function implementation**:
  - Goal achievement: Up to +10.0f reward (configurable)
  - Resource collection: Up to +1.0f reward per resource found
  - Hazard penalties: -5.0f penalty for dangerous conditions
  - Step penalties: Small negative reward for time passage

### 2. Observation-Based Reward Calculation
- **Visual features**: Processes RGB data from world observations to detect goals, hazards, resources
- **Touch/sensor data**: Evaluates collision sensors and collision penalties
- **Internal state rewards**: Includes proprioception and homeostatic signal rewards
- **World objects and agent state**: Integrated calculation based on all observable components

### 3. Temporal Reward Mechanisms
- **Reward decay**: Configurable time decay rate (0.01f default) applied each timestep
- **Accumulated reward tracking**: Tracks episode total and step-by-step rewards
- **Reward prediction errors**: Framework for TD learning integration
- **Near-miss tolerance**: Bonus rewards for progress that doesn't yet reach goal threshold

### 4. Configuration and Parameters
- **Config.hpp integration**: Rewards can be configured via nlm::Config system
- **Configurable weights**:
  - reward_weight_visual: Visual feature importance (1.0f default)
  - reward_weight_sensory: Sensory input importance (1.0f default)
  - reward_weight_internal: Internal state importance (1.0f default)
  - reward_weight_temporal: Time decay importance (0.5f default)
- **Exploration rewards**: Configurable exploration drive parameter
- **Goal-based rewards**: Configurable thresholds and bonuses

### 5. Advanced Features
- **Sparse vs Dense**: Toggle between sparse (goal-threshold based) and dense (continuous) reward systems
- **Shaping rewards**: Intermediate goal progress rewards in dense mode
- **Negative rewards**: Configurable penalties for hazards and time
- **Episode statistics**: Tracks goal progress, hazard risk, and total episode rewards

## Implementation Details

### Reward.cpp Features
```cpp
// Core reward computation
float computeReward(const Observation& observation) const;

// Component rewards
float computeVisualReward(const Observation& observation) const;
float computeSensoryReward(const Observation& observation) const;
float computeInternalReward(const Observation& observation) const;

// State tracking
void initialize(std::shared_ptr<Config> config);
void updateTemporalState();
void resetEpisodeState();

// Statistics and accessors
float getGoalProgress() const;
float getHazardRisk() const;
float getLastStepReward() const;
float getEpisodeTotalReward() const;
```

### Key Methods
- **initialize()**: Links reward system to configuration
- **computeReward()**: Main reward calculation with shaping
- **add()**: Accumulates rewards and updates history
- **reset()**: Clears current and accumulated rewards

### Reward Components

1. **Goal Detection**: Bright pixels (value > 0.9f) indicate goal proximity
2. **Hazard Detection**: High red channel values (> 0.8f) indicate danger
3. **Resource Detection**: Medium brightness values (0.5f-0.8f) indicate resources
4. **Sensory Processing**: Touch, vision, and audio signals contribute to rewards
5. **Internal State**: Homeostatic balance and metabolic efficiency bonuses

### Configuration Parameters

Common reward settings:
- `reward_sparse`: true/false for sparse/dense reward systems
- `reward_goal_threshold`: Goal achievement threshold (default 0.9f)
- `reward_hazard_threshold`: Hazard trigger threshold (default 0.7f)
- `reward_near_miss_bonus`: Near-miss reward bonus (default 0.5f)
- `reward_exploration_rate`: Exploration drive rate (default 0.1f)

### Testing

The implementation includes comprehensive tests in tests/test_reward.cpp covering:
- Reward initialization and configuration
- Reward parameter configuration
- Reward accumulation and history
- Reward reset functionality
- Sparse and dense reward modes
- Temporal reward decay
- Goal progress calculation
- Hazard risk assessment
- Resource availability
- Near-miss rewards
- Complete integration testing

### Integration with Neuromodulation

The Reward class is designed to integrate with the dopamine neuromodulator:
- Rewards can signal reward prediction errors
- Used by reward-modulated plasticity rules
- Supports temporal difference (TD) learning
- Connects to exploration drive via curiosity system

### Performance Considerations

- **Efficiency**: O(n) observation processing where n is observation size
- **History management**: Limited to 1000 entries to prevent memory issues
- **Modular design**: Separate components for different reward types
- **Configurable**: Weights and thresholds can be adjusted per environment

### Future Enhancements

Potential improvements:
- More sophisticated feature extraction from observations
- Machine learning-based reward shaping
- Multi-agent reward coordination
- Real-time adaptation of reward parameters
- Integration with advanced reinforcement learning algorithms

