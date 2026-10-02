# NLM Enhanced Agent Brain Features

This document describes the advanced agent commands and capabilities added to the NLM (Neural Learning Machine) system.

## Overview

The enhanced AgentBrain now provides comprehensive control over neural learning and behavior, making the system more user-friendly and powerful while maintaining backward compatibility.

## Key Features

### 1. Advanced Agent Control
- **Start/Pause/Resume/Stop**: Complete control over agent execution
- **State Monitoring**: Real-time status tracking of agent operations
- **Execution Flow Management**: Pause and resume agent processing

### 2. Learning Configuration
- **Learning Enable/Disable**: Turn learning on or off as needed
- **Learning Rate Adjustment**: Fine-tune learning speed (0.0 to 1.0)
- **Exploration/Exploitation Control**: Balance between exploration and exploitation

### 3. Behavior Modification
- **Curiosity Threshold**: Control when curiosity drives exploration
- **Adaptation Speed**: Adjust how quickly the agent adapts to new stimuli
- **Behavioral Tweaks**: Modify exploration strategies

### 4. State Management
- **Checkpoint Saving**: Save agent state at any point
- **State Restoration**: Restore from checkpoints
- **Persistent Storage**: Save/load agent configurations

### 5. Performance Monitoring and Optimization
- **Real-time Monitoring**: Track agent performance metrics
- **Performance Alerts**: Get notified when performance drops below threshold
- **Statistics Collection**: Comprehensive performance metrics

## Command Line Interface

### Mode Selection
- `mode=agent`: Enable agent mode for brain control
- `mode=simulation`: Enable simulation mode (default)

### Agent Commands (in agent mode)
- `agent command [arg]`: Execute agent command (see 'help agent')
- `status`: Show system status
- `help [topic]`: Show help for topic
- `run`: Start agent or simulation
- `stop`: Stop agent or simulation
- `pause`: Pause agent or simulation
- `resume`: Resume agent or simulation

### Simulation Commands
- `run`: Run the simulation (default)
- `stop`: Stop simulation
- `pause`: Pause simulation
- `resume`: Resume simulation
- `verbose [on|off]`: Toggle verbose logging
- `status`: Show system status

## Configuration

### Configuration File
Create a configuration file (e.g., `agent_brain.cfg`) in the `configs/` directory:

```ini
[agent_brain]
agent_learning_enabled = true
agent_learning_rate = 0.05
agent_exploration_rate = 0.15
agent_performance_monitoring = true
agent_curiosity_threshold = 0.3
agent_adaptation_speed = 0.02
agent_max_memory_size = 2000000
```

### Command Line Overrides
```bash
./nlm mode=agent agent learning on
./nlm mode=agent agent lrate 0.1
./nlm mode=agent agent explore 0.2
./nlm mode=agent agent checkpoint /path/to/checkpoint
./nlm mode=agent agent restore /path/to/checkpoint
```

## Agent Brain Methods

### Core Control Methods
- `start()`: Start the agent (resume from pause)
- `stop()`: Stop the agent (save state)
- `pause()`: Pause the agent (save state)
- `resume()`: Resume the agent
- `isRunning()`: Check if agent is running
- `isPaused()`: Check if agent is paused
- `isStopped()`: Check if agent is stopped

### Configuration Methods
- `setLearningEnabled(bool)`: Enable/disable learning
- `setLearningRate(float)`: Set learning rate (0.0-1.0)
- `setExplorationRate(float)`: Set exploration rate (0.0-1.0)
- `setMaxMemorySize(size_t)`: Set maximum memory size
- `setCuriosityThreshold(float)`: Set curiosity threshold
- `setAdaptationSpeed(float)`: Set adaptation speed

### Statistics and Monitoring
- `collectStats()`: Collect comprehensive agent statistics
- `getPerformanceMetrics()`: Get formatted performance metrics
- `getUptime()`: Get agent uptime in seconds
- `getTotalTime()`: Get total time in seconds
- `getStepCount()`: Get number of steps processed
- `getAverageFiringRate()`: Get average firing rate
- `getSynapticStability()`: Get synaptic stability score
- `getWeightStatistics()`: Get weight statistics
- `checkPerformance()`: Check if performance meets threshold
- `getPerformanceScore()`: Get performance score

### State Management
- `checkpoint(const std::string&)`: Save checkpoint
- `restore(const std::string&)`: Restore checkpoint
- `saveState(const std::string&)`: Save agent state
- `loadState(const std::string&)`: Load agent state
- `reset()`: Reset agent to initial state

### Learning Control
- `enableLearning(bool)`: Enable/disable learning
- `adjustLearningRate(float)`: Adjust learning rate by factor
- `setExplorationExploitation(float)`: Set exploration-exploitation balance
- `resetAdaptation()`: Reset adaptation accumulator

## Examples

### Basic Agent Control
```bash
# Start agent
./nlm mode=agent agent start

# Check status
./nlm mode=agent agent status

# Pause agent
./nlm mode=agent agent pause

# Resume agent
./nlm mode=agent agent resume

# Stop agent
./nlm mode=agent agent stop
```

### Learning Configuration
```bash
# Enable learning
./nlm mode=agent agent learning on

# Set learning rate
./nlm mode=agent agent lrate 0.05

# Set exploration rate
./nlm mode=agent agent explore 0.2

# Disable learning
./nlm mode=agent agent learning off
```

### State Management
```bash
# Save checkpoint
./nlm mode=agent agent checkpoint /tmp/agent_checkpoint

# Restore checkpoint
./nlm mode=agent agent restore /tmp/agent_checkpoint

# Save agent state
./nlm mode=agent agent save /tmp/agent_state.dat

# Load agent state
./nlm mode=agent agent load /tmp/agent_state.dat
```

### Performance Monitoring
```bash
# Enable performance monitoring
./nlm mode=agent agent monitor on

# Get statistics
./nlm mode=agent agent stats

# Get performance metrics
./nlm mode=agent agent perf
```

## Usage Notes

### Backward Compatibility
- The enhanced AgentBrain maintains full backward compatibility with existing code
- All original methods work as before
- New methods are additive and do not break existing functionality

### Error Handling
- All new methods include proper error handling
- Configuration validation prevents invalid settings
- Graceful degradation if files cannot be read/written

### Performance Considerations
- Performance monitoring can be disabled for better performance
- State management operations are optimized for minimal overhead
- Learning rate adjustments can be used to control computation time

## Testing

Run the test suite to verify new features:

```bash
cd /path/to/project
./build/tests/test_agent_brain
```

## Troubleshooting

### Common Issues
1. **Agent commands not recognized**: Ensure you're in agent mode (`mode=agent`)
2. **Learning not working**: Check `agent_learning_enabled` configuration
3. **Performance monitoring not working**: Verify `agent_performance_monitoring` setting
4. **Checkpoints not saving**: Check file permissions and directory access

### Getting Help
- `help agent`: Show all agent commands
- `help simulation`: Show simulation commands
- `agent status`: Show current agent status
- `agent perf`: Show performance metrics

## Future Enhancements

The enhanced AgentBrain provides a foundation for future extensions:
- Integration with reinforcement learning algorithms
- Advanced neural architecture search
- Real-time adaptation to environment changes
- Multi-agent coordination capabilities

## References

- [NLM Phase 2 Documentation](README.md)
- [AgentBrain Class Reference](src/agent/AgentBrain.hpp)
- [Configuration Reference](configs/agent_brain.cfg)
- [Test Suite](tests/test_agent_brain.cpp)