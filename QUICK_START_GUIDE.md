# AgentBrain Quick Start Guide

## Welcome to the Enhanced AgentBrain!

This guide will help you get started with the new, enhanced AgentBrain system. All improvements maintain full backward compatibility with existing code.

## Installation and Setup

The AgentBrain improvements are integrated directly into the existing codebase. No installation is required - just include the enhanced headers in your project.

## Quick Start Examples

### 1. Basic Usage (Unchanged)

This is how you used AgentBrain before - it still works exactly the same:

```cpp
#include "src/agent/AgentBrain.hpp"

int main() {
    // Create brain and agent brain
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Initialize with world
    agentBrain->initialize(world);
    
    // Main simulation loop
    while (simulationRunning) {
        agentBrain->processSensoryInput(percept);
        agentBrain->applyRewardModulation(reward, predictedReward);
        agentBrain->updateDevelopment(timestep);
        
        MotorCommand command = agentBrain->decodeMotorCommand();
        // ... send command to world
    }
    
    return 0;
}
```

### 2. Enhanced Usage with Error Handling and Profiling

For new users or when you want comprehensive functionality:

```cpp
#include "src/agent/EnhancedAgentBrain.hpp"
#include "src/agent/AdvancedAgentBrainExamples.hpp"

int main() {
    // Create base agent brain
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Wrap with enhanced functionality
    auto enhancedAgentBrain = std::make_shared<nlm::EnhancedAgentBrain>(agentBrain);
    
    // Enable performance monitoring
    enhancedAgentBrain->setProfilingEnabled(true);
    enhancedAgentBrain->setErrorReportingEnabled(true);
    
    // Add custom behavior tracking listener
    auto behaviorTracker = std::make_shared<BehaviorTrackingListener>();
    enhancedAgentBrain->addStateChangeListener(behaviorTracker);
    
    // Initialize enhanced agent
    enhancedAgentBrain->initialize(world);
    
    // Main loop with enhanced features
    while (simulationRunning) {
        enhancedAgentBrain->processSensoryInput(percept);
        enhancedAgentBrain->applyRewardModulation(reward, predictedReward);
        enhancedAgentBrain->updateDevelopment(timestep);
        
        MotorCommand command = enhancedAgentBrain->decodeMotorCommand();
        
        // Record performance periodically
        enhancedAgentBrain->recordPerformanceSample();
        
        // Send command to world
        // ...
    }
    
    // Export debug information for analysis
    std::string debugInfo = enhancedAgentBrain->exportDebugInfo();
    std::string behaviorReport = behaviorTracker->generateBehaviorReport();
    
    return 0;
}
```

### 3. Advanced Configuration Management

For environment-specific behavior or custom parameters:

```cpp
#include "src/agent/AdvancedAgentBrainExamples.hpp"

int main() {
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Create configuration manager
    CustomAgentBrainManager manager(agentBrain);
    
    // Apply environment-specific configuration
    auto envConfig = CustomAgentBrainManager::AdvancedConfiguration::createEnvironmentSpecificConfig("open_field");
    manager.applyAdvancedConfiguration(envConfig);
    
    // Or create completely custom configuration
    CustomAgentBrainManager::AdvancedConfiguration customConfig;
    customConfig.learning.weightLearningRate = 0.05f;
    customConfig.learning.predictionErrorSensitivity = 1.2f;
    customConfig.behavior.curiosityThreshold = 0.2f;
    customConfig.behavior.explorationProbability = 0.3f;
    
    manager.applyAdvancedConfiguration(customConfig);
    
    // Initialize with custom configuration
    agentBrain->initialize(world);
    
    return 0;
}
```

### 4. Custom Training Strategies

For advanced users who want custom learning behavior:

```cpp
#include "src/agent/AdvancedAgentBrainExamples.hpp"

int main() {
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    auto enhancedAgentBrain = std::make_shared<nlm::EnhancedAgentBrain>(agentBrain);
    
    // Create training manager
    TrainingManager trainer(enhancedAgentBrain);
    
    // Configure training strategy
    TrainingManager::TrainingStrategy strategy;
    strategy.type = TrainingManager::TrainingStrategy::Type::ExplorationFocused;
    strategy.learningRate = 0.02f;
    strategy.explorationRate = 0.25f;
    strategy.targetNoveltyLevel = 0.6f;
    strategy.useRewardPrediction = true;
    strategy.adaptToEnvironment = true;
    
    trainer.setStrategy(strategy);
    
    // Main simulation loop
    while (simulationRunning) {
        enhancedAgentBrain->processSensoryInput(percept);
        enhancedAgentBrain->applyRewardModulation(reward, predictedReward);
        enhancedAgentBrain->updateDevelopment(timestep);
        
        MotorCommand command = enhancedAgentBrain->decodeMotorCommand();
        
        // Execute training step
        trainer.executeTrainingStep(timestep);
        
        // Send command to world
        // ...
    }
    
    // Generate training report
    std::string trainingReport = trainer.generateTrainingReport();
    
    return 0;
}
```

## Configuration Customization

### Runtime Configuration Changes

You can modify AgentBrain configuration at runtime without recompiling:

```cpp
auto agentBrain = ...; // Your AgentBrain instance

// Access the configuration system
auto& config = agentBrain->getConfig();

// Modify parameters
config.visionScaleFactor = 8.0f;
config.touchScaleFactor = 12.0f;
config.explorationBaseProbability = 0.35f;
config.curiosityThreshold = 0.25f;

// Add custom parameters
agentBrain->addCustomParameter("customRewardFactor", 1.5f);

// Get parameter values
float customValue = agentBrain->getCustomParameter("customRewardFactor", 1.0f);

// Export configuration for persistence
std::string configStr = agentBrain->exportConfig();

// Import configuration
agentBrain->importConfig(configStr);
```

### Environment-Specific Configurations

The system includes predefined configurations for different environments:

```cpp
// Environment types available:
// - "open_field" (encourage exploration)
// - "maze" (systematic exploration)
// - "hazardous" (conservative approach)
// - "resource_rich" (balanced exploration/exploitation)

auto envConfig = CustomAgentBrainManager::createEnvironmentSpecificConfig("maze");
manager.applyAdvancedConfiguration(envConfig);
```

## Performance Monitoring

### Basic Performance Monitoring

Enable performance monitoring with just one line:

```cpp
auto enhancedAgentBrain = std::make_shared<nlm::EnhancedAgentBrain>(agentBrain);
enhancedAgentBrain->setProfilingEnabled(true);
```

### Detailed Performance Analysis

The system provides comprehensive performance metrics:

```cpp
// During simulation
enhancedAgentBrain->recordPerformanceSample();

// Get performance statistics
auto& profiler = enhancedAgentBrain->getProfiler();
auto metrics = profiler.getCurrentMetrics();

// Generate performance report
std::string performanceReport = profiler.generateReport();

// Get error information
auto& errorHandler = enhancedAgentBrain->getErrorHandler();
std::string errorReport = errorHandler.exportErrors();

// Combined debug information
std::string debugInfo = enhancedAgentBrain->exportDebugInfo();
```

## Error Handling and Recovery

### Automatic Error Recovery

The enhanced system automatically handles errors:

```cpp
auto enhancedAgentBrain = std::make_shared<nlm::EnhancedAgentBrain>(agentBrain);

// Error reporting is enabled by default
// The system will:
// - Report errors with source location
// - Categorize errors by type
// - Apply recovery mechanisms
// - Log errors for debugging
// - Prevent error flooding
```

### Custom Error Handling

For advanced error handling:

```cpp
// Create custom error handler
class CustomErrorHandler : public AgentBrainErrorHandler {
public:
    void handleError(const ErrorInfo& error) override {
        // Custom error processing
        // Log to database, send alerts, etc.
        std::cout << "ERROR: " << error.message << std::endl;
    }
};
```

## Migration Guide

### From Old to New

**If you're already using AgentBrain:**

1. **No changes required!** - All existing code continues to work
2. **Gradual upgrade path** - Start with basic usage, then add enhancements
3. **Benefits are additive** - New features are opt-in

**Recommended upgrade path:**

```cpp
// OLD CODE (still works)
auto brain = std::make_shared<nlm::Brain>(config);
auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);

// NEW CODE (with enhancements)
auto brain = std::make_shared<nlm::Brain>(config);
auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
auto enhancedAgentBrain = std::make_shared<nlm::EnhancedAgentBrain>(agentBrain);
enhancedAgentBrain->setProfilingEnabled(true);
```

### For Advanced Users

**New capabilities added:**
- Runtime configuration management
- Comprehensive error handling
- Performance monitoring and profiling
- Extensible listener system
- Custom training strategies
- Environment-specific configurations
- Advanced state management
- Optimized processing algorithms

## Documentation Resources

### Available Documentation

1. **AgentBrain Improvements** - `AGENT_BRAIN_IMPROVEMENTS.md` - Comprehensive technical documentation
2. **Quick Start Guide** - This file - Getting started examples
3. **Source Code Headers** - Inline comments and documentation in header files

### Additional Resources

- **Examples Directory**: Look for example projects in the examples/ folder
- **Tests Directory**: Review unit tests for best practices
- **Documentation Directory**: Additional technical specifications

## Getting Help

### Common Issues and Solutions

1. **Configuration problems?** - Check parameter validation
2. **Performance issues?** - Review profiling reports
3. **Listener not working?** - Verify listener registration
4. **Memory usage high?** - Check state management settings

### Support Channels

- **Documentation**: Review AGENT_BRAIN_IMPROVEMENTS.md
- **Examples**: Run existing example projects
- **Code Comments**: Check header file comments
- **Community**: Join the NLM user community

## Next Steps

### For Simple Use Cases
1. Use basic AgentBrain as before
2. Gradually add enhanced features as needed
3. Start with EnhancedAgentBrain for comprehensive functionality

### For Advanced Use Cases
1. Implement custom listeners for behavior tracking
2. Use CustomAgentBrainManager for environment-specific behavior
3. Configure TrainingManager for custom learning strategies
4. Optimize performance with OptimizedAgentBrain classes

### For Research and Development
1. Use advanced configuration system for parameter tuning
2. Implement custom error handling and recovery mechanisms
3. Add performance profiling for algorithm optimization
4. Extend the extensibility framework with custom components

## Conclusion

The Enhanced AgentBrain provides:

- **Backward Compatibility**: All existing code continues to work unchanged
- **Progressive Enhancement**: New features are opt-in and additive
- **Production Ready**: Comprehensive error handling, performance monitoring, and extensibility
- **Research Friendly**: Extensive configuration options and customization capabilities
- **Performance Optimized**: Advanced algorithms and memory management

Start with the basic examples, and as you become more familiar with the system, explore the advanced features for even greater capabilities.

Enjoy implementing your enhanced AgentBrain!