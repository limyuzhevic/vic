# NLM Training Module

## Overview

The NLM Training Module is a comprehensive, extensible system for training NLM (Neural Learning Machine) brains for various tasks while maintaining biological plausibility and performance of the underlying simulation.

## Installation

### From Source
```bash
cmake -B build -S .
cmake --build build
```

### Python Package
The training module is distributed as part of the NLM Python package:

```bash
pip install pynlm[training]
```

Or install all development dependencies:

```bash
pip install pynlm[dev]
```

## Getting Started

### Basic Usage

#### Creating a Training Setup
```cpp
#include "nlm/training/TrainingModule.hpp"

// Create training setup
auto [trainer, brain, world, agent] = nlm::createTrainingSetup();
```

#### Configuring Training
```cpp
nlm::TrainingConfig config;
config.maxEpisodes = 1000;
config.learningRate = 0.001;
config.discountFactor = 0.99;
config.explorationRate = 0.1;
```

#### Running Training
```cpp
bool success = trainer.trainAgent(brain, world, agent, config);
```

### Python Usage
```python
import pynlm as nlm

# Create training setup
trainer, brain, world, agent = nlm.create_training_setup()

# Configure training
training_config = nlm.TrainingConfig()
training_config.maxEpisodes = 1000
training_config.learningRate = 0.001

# Run training
success = trainer.trainAgent(brain, world, agent, training_config)
```

## Training Examples

### 1. Basic Reinforcement Learning
See `examples/basic_training.cpp` for a complete example.

### 2. Advanced Training with Features
See `examples/advanced_training.cpp` for a complete example.

### 3. Hyperparameter Optimization
See `examples/hyperparameter_optimization.cpp` for a complete example.

### 4. Experiment Tracking
See `examples/experiment_tracking.cpp` for a complete example.

## API Reference

### Core Classes

#### TrainingCore
**Header**: `nlm/training/TrainingCore.hpp`

**Methods**:
- `bool trainAgent(Brain*, SimpleWorld*, AgentBrain*, const TrainingConfig&)`
- `float evaluateAgent(Brain*, SimpleWorld*, AgentBrain*, const TrainingConfig&, int)`
- `std::vector<std::pair<std::string, float>> hyperparameterOptimization(...)`
- `bool saveTrainingSession(...)`
- `bool loadTrainingSession(...)`

#### TrainingConfig
**Header**: `nlm/training/TrainingComponents.hpp`

**Members**:
- `int maxEpisodes`
- `float learningRate`
- `float discountFactor`
- `float explorationRate`
- `RewardConfig rewardConfig`
- `EnvironmentConfig environmentConfig`
- `ValidationConfig validationConfig`
- `std::string checkpointPath`
- `uint64_t randomSeed`

#### TrainingCallback
**Header**: `nlm/training/TrainingComponents.hpp`

**Pure Virtual Methods**:
- `virtual void afterEpisode(int, float, bool)`
- `virtual void afterStep(int, int, float, float)`
- `virtual void onError(const std::string&)`
- `virtual void onTrainingStart()`
- `virtual void onTrainingEnd()`
- `virtual void onCheckpointSaved(const std::string&)`

#### SimpleTrainingCallback
**Header**: `nlm/training/TrainingComponents.hpp`

**Default Implementation** of TrainingCallback with basic logging.

#### TrainingMonitor
**Header**: `nlm/training/TrainingComponents.hpp`

**Methods**:
- `void recordEpisodePerformance(int, float, float, float)`
- `float getAverageReward(int = 100) const`
- `float getAverageLoss(int = 100) const`
- `float getCurrentReward() const`
- `float getCurrentLoss() const`
- `int getEpisodesCompleted() const`
- `std::string getPerformanceSummary() const`

#### AdvancedTrainingFeatures
**Header**: `nlm/training/TrainingComponents.hpp`

**Features**:
- **Curiosity-driven Exploration**: Intrinsic motivation for exploration
- **Curriculum Learning**: Progressive task difficulty
- **Transfer Learning**: Knowledge transfer between tasks
- **Meta-learning**: Learning how to learn

#### CommonTrainingPatterns
**Header**: `nlm/training/TrainingComponents.hpp`

**Static Methods**:
- `static bool runReinforcementLearningPattern(...)`
- `static bool runSupervisedLearningPattern(...)`
- `static bool runUnsupervisedLearningPattern(...)`
- `static bool runHybridTrainingPattern(...)`

#### TrainingPersistence
**Header**: `nlm/training/TrainingComponents.hpp`

**Static Methods**:
- `static bool saveTrainingSession(...)`
- `static bool loadTrainingSession(...)`

#### TrainingModule
**Header**: `nlm/training/TrainingModule.hpp`

**Convenience Methods**:
- `std::shared_ptr<TrainingCore> createTrainingCore()`
- `std::shared_ptr<TrainingConfig> createTrainingConfig()`
- `std::shared_ptr<AdvancedTrainingFeatures> createAdvancedFeatures()`
- `std::shared_ptr<SimpleTrainingCallback> createTrainingCallback()`
- `std::shared_ptr<CommonTrainingPatterns> createTrainingPatterns()`
- `std::shared_ptr<TrainingPersistence> createTrainingPersistence()`

### Reward Configuration

#### RewardConfig
**Header**: `nlm/training/TrainingComponents.hpp`

**Members**:
- `enum Type { SIMPLE, WEIGHTED, EPISODIC } type`
- `float simpleReward`
- `Weighted weighted`
- `float episodicWindow`
- `float discountFactor`

#### RewardConfig::Weighted
**Members**:
- `float stepCompletion`
- `float novelty`
- `float survival`
- `float efficiency`

### Environment Configuration

#### EnvironmentConfig
**Header**: `nlm/training/TrainingComponents.hpp`

**Members**:
- `bool enableDebugOutput`
- `float timeStep`
- `float maxTimePerEpisode`
- `int maxStepsPerEpisode`
- `bool recordVideo`
- `std::string videoOutputPath`

### Validation Configuration

#### ValidationConfig
**Header**: `nlm/training/TrainingComponents.hpp`

**Members**:
- `bool enableValidation`
- `float validationFrequency`
- `bool earlyStopping`
- `float patience`
- `float improvementThreshold`
- `float targetPerformance`

### Hyperparameter Optimization

#### OptimizationStrategy
**Header**: `nlm/training/TrainingComponents.hpp`

**Enum Values**:
- `GRID_SEARCH`
- `RANDOM_SEARCH`
- `BAYESIAN_OPTIMIZATION`
- `HILL_CLIMBING`
- `ADAM`
- `RMSPROP`

#### Hyperparameter
**Header**: `nlm/training/TrainingComponents.hpp`

**Members**:
- `std::string name`
- `float value`
- `float min`
- `float max`
- `bool discrete`

### Performance Monitoring

#### TrainingMonitor
**Header**: `nlm/training/TrainingComponents.hpp`

**Performance Metrics**:
- **Episode Rewards**: Collection of rewards per episode
- **Episode Losses**: Collection of losses per episode
- **Episode Durations**: Collection of durations per episode
- **Running Statistics**: Average values over episodes
- **Success Rate**: Ratio of successful episodes
- **Training Time**: Total time spent training

## Configuration Examples

### Basic Configuration
```cpp
nlm::TrainingConfig config;
config.maxEpisodes = 1000;
config.learningRate = 0.001;
config.discountFactor = 0.99;
config.explorationRate = 0.1;
config.rewardConfig.type = nlm::RewardConfig::Type::SIMPLE;
config.rewardConfig.simpleReward = 1.0f;
```

### Advanced Configuration
```cpp
nlm::TrainingConfig config;
config.maxEpisodes = 5000;
config.learningRate = 0.0005;
config.discountFactor = 0.995;
config.explorationRate = 0.05;
config.rewardConfig.type = nlm::RewardConfig::Type::WEIGHTED;
config.rewardConfig.weighted.stepCompletion = 1.0f;
config.rewardConfig.weighted.novelty = 0.5f;
config.rewardConfig.weighted.survival = 0.8f;
config.rewardConfig.weighted.efficiency = 1.0f;
config.environmentConfig.enableDebugOutput = true;
config.environmentConfig.timeStep = 0.05f;
config.validationConfig.enableValidation = true;
config.validationConfig.validationFrequency = 50.0f;
config.validationConfig.earlyStopping = true;
config.validationConfig.patience = 20;
config.saveCheckpoints = true;
config.checkpointFrequency = 100;
```

### Hyperparameter Configuration
```cpp
std::vector<nlm::Hyperparameter> paramSpace = {
    {"learning_rate", 0.001f, 0.0001f, 0.01f},
    {"discount_factor", 0.99f, 0.9f, 0.999f},
    {"exploration_rate", 0.1f, 0.01f, 0.5f},
    {"batch_size", 32, 16, 128, true},
    {"hidden_size", 256, 128, 1024, true},
};
```

## Callbacks

### SimpleTrainingCallback
A basic callback implementation that provides logging and progress reporting.

### Custom Callbacks
Create custom callbacks by inheriting from `TrainingCallback` and overriding the virtual methods.

```cpp
class CustomCallback : public nlm::TrainingCallback {
public:
    void afterEpisode(int episode, float reward, bool success) override {
        std::cout << "Episode " << episode << ": Reward = " << reward 
                  << ", Success = " << (success ? "Yes" : "No") << std::endl;
    }
    
    void afterStep(int episode, int step, float reward, float loss) override {
        if (step % 100 == 0) {
            std::cout << "Episode " << episode << ", Step " << step 
                      << ", Reward: " << reward << ", Loss: " << loss << std::endl;
        }
    }
};
```

## Performance Monitoring

### TrainingMonitor Usage
```cpp
nlm::TrainingMonitor monitor;

// Record performance
monitor.recordEpisodePerformance(episode, reward, loss, duration);

// Get performance metrics
float avg_reward = monitor.getAverageReward();
float avg_loss = monitor.getAverageLoss();
int episodes_completed = monitor.getEpisodesCompleted();

// Get performance summary
std::string summary = monitor.getPerformanceSummary();
```

## Advanced Features

### Curiosity-driven Exploration
```cpp
nlm::AdvancedTrainingFeatures features;
features.enableCuriosityDrivenExploration();
features.curiosityConfig.explorationBonus = 0.1f;
features.curiosityConfig.enableIntrinsicMotivation = true;
features.curiosityConfig.noveltyThreshold = 0.5f;
```

### Curriculum Learning
```cpp
nlm::AdvancedTrainingFeatures features;
features.enableCurriculumLearning();
features.curriculumConfig.enableCurriculum = true;
features.curriculumConfig.initialDifficulty = 1;
features.curriculumConfig.maxDifficulty = 10;
features.curriculumConfig.successThreshold = 0.8f;
```

### Transfer Learning
```cpp
nlm::AdvancedTrainingFeatures features;
features.enableTransferLearning("source_task");
features.transferConfig.enableTransfer = true;
features.transferConfig.transferWeight = 0.5f;
```

### Meta-learning
```cpp
nlm::AdvancedTrainingFeatures features;
features.enableMetaLearning();
features.metaLearningConfig.enableMetaLearning = true;
features.metaLearningConfig.metaSteps = 10;
```

## Training Patterns

### Reinforcement Learning Pattern
```cpp
bool success = nlm::CommonTrainingPatterns::runReinforcementLearningPattern(
    trainer, brain, world, agent, rl_config, callback
);
```

### Supervised Learning Pattern
```cpp
bool success = nlm::CommonTrainingPatterns::runSupervisedLearningPattern(
    trainer, brain, world, agent, sl_config, callback
);
```

### Unsupervised Learning Pattern
```cpp
bool success = nlm::CommonTrainingPatterns::runUnsupervisedLearningPattern(
    trainer, brain, world, agent, us_config, callback
);
```

### Hybrid Training Pattern
```cpp
bool success = nlm::CommonTrainingPatterns::runHybridTrainingPattern(
    trainer, brain, world, agent, hybrid_config, callback
);
```

## Persistence and Experiment Tracking

### Save Training Session
```cpp
bool success = trainer.saveTrainingSession(
    "experiment_name.nlm",
    brain, agent, episode
);
```

### Load Training Session
```cpp
bool success = trainer.loadTrainingSession(
    "experiment_name.nlm",
    brain, agent, episode
);
```

## Hyperparameter Optimization

### Random Search
```cpp
auto results = optimizer.optimize(
    paramSpace,
    evaluate_training_params,
    nlm::OptimizationStrategy::RANDOM_SEARCH,
    maxIterations = 100
);
```

### Grid Search
```cpp
auto results = optimizer.optimize(
    paramSpace,
    evaluate_training_params,
    nlm::OptimizationStrategy::GRID_SEARCH,
    maxIterations = 100
);
```

### Hill Climbing
```cpp
auto results = optimizer.optimize(
    paramSpace,
    evaluate_training_params,
    nlm::OptimizationStrategy::HILL_CLIMBING,
    maxIterations = 100
);
```

## Testing

### Running Tests
```bash
cd build
./training_test
```

### Test Coverage
The training module includes comprehensive unit tests:
- TrainingCore functionality
- TrainingConfig functionality
- Callback system
- Performance monitoring
- Hyperparameter optimization
- Save/load persistence

## Building and Installation

### Building the Training Module
```bash
cmake -B build -S .
cmake --build build
```

### Installing the Training Module
```bash
cmake --install build --prefix /usr/local
```

### Running Tests
```bash
cd build
ctest
```

## Troubleshooting

### "Training is taking too long"
- Reduce maxEpisodes or increase stopping criteria
- Use hyperparameter optimization to find better configurations

### "Callbacks not being called"
- Ensure callbacks are properly registered
- Check that training configuration enables the features callbacks expect

### "Saving training sessions fails"
- Ensure sufficient disk space
- Check file permissions
- Verify that required dependencies are installed

### "Advanced features not working"
- Verify that the agent supports the required features
- Check that the world simulation supports the advanced feature modules
- Ensure feature flags are properly enabled

## Migration Guide

### From Old Training API to New Training Module

#### Old API (Simplified)
```cpp
// Old way
void trainAgent(Brain* brain, World* world, Agent* agent, int episodes);
```

#### New API (TrainingModule)
```cpp
// New way
auto [trainer, brain, world, agent] = nlm::createTrainingSetup();
nlm::TrainingConfig config;
config.maxEpisodes = episodes;
bool success = trainer.trainAgent(brain, world, agent, config);
```

#### Old API (Basic Callbacks)
```cpp
// Old way
void onEpisodeComplete(int episode, float reward);
```

#### New API (TrainingCallback)
```cpp
// New way
class MyCallback : public nlm::TrainingCallback {
public:
    void afterEpisode(int episode, float reward, bool success) override {
        // Custom implementation
    }
};
```

## Future Enhancements

### Planned Features
1. **Distributed Training**: Integration with Ray for distributed training
2. **Advanced Visualization**: Real-time training visualization
3. **Automated Experiment Design**: Automated generation of training experiments
4. **Model Compression**: Support for model compression and quantization
5. **Online Learning**: Support for continuous, online learning

### Research Integration
- Integration with neural science research
- Support for hypothesis testing
- Reproducible research workflows

## License

The NLM Training Module is licensed under the MIT License.

## Copyright

Copyright (c) 2026 NLM Authors

## Acknowledgements

The NLM Training Module builds upon the extensive research and development of the NLM project, including contributions from:

- Neural Systems Research Team
- Computational Neuroscience Community
- Open Source Contributors

## Contact

For questions, issues, or contributions:
- Website: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Issues: https://github.com/nlm-project/nlm/issues

## Version

Version: 1.0.0

## Release Notes

### Version 1.0.0
- Initial release of the NLM Training Module
- Comprehensive training infrastructure
- Advanced features and patterns
- Full persistence and experiment tracking
- Comprehensive testing suite

## Conclusion

The NLM Training Module provides a comprehensive, extensible system for training NLM brains for various tasks. It maintains biological plausibility while offering advanced features for modern machine learning applications. The module is designed to be flexible, efficient, and easy to use, making it suitable for both research and production environments.

Key strengths:
- **Comprehensive**: Covers all aspects of training NLM brains
- **Extensible**: Easy to add new features and patterns
- **Performant**: Optimized for efficiency
- **Well-documented**: Complete documentation and examples
- **Tested**: Comprehensive test coverage
- **Reproducible**: Strong support for reproducible research

The training module represents a significant step forward in the field of neural machine learning, providing researchers and practitioners with the tools they need to train effective, biologically plausible neural systems for a wide range of applications.
