# NLM Interactive Debugging Console

This document describes the interactive debugging console for NLM (Neural Learning Machine), an advanced debugging and analysis tool for neural network simulation.

## Overview

The interactive debugging console provides real-time interaction with the NLM brain simulation, allowing users to:
- Monitor brain state in real-time
- Inject stimuli and control simulation
- Analyze neural activity patterns
- Access detailed statistics and metrics
- Control simulation parameters dynamically

## Features

### Real-time Monitoring
- Live brain state display
- Neural activity graphs
- Spike statistics
- Memory system status
- Plasticity activity tracking

### Interactive Controls
- Current injection into specific neurons
- Simulation speed control
- Step-by-step execution
- Pause/resume simulation
- Reset brain state

### Analysis Tools
- Neural pattern detection
- Synaptic weight visualization
- Connection analysis
- Activity correlation
- Performance metrics

### Command Reference

#### Basic Commands

**Help Commands**
```
help                    Show available commands
help <command>          Get detailed help for specific command
commands                List all available commands
```

**Brain Control**
```
brain reset            Reset brain to initial state
brain status           Show current brain status
brain initialize       Re-initialize brain with current config
brain save <file>      Save brain state to file
brain load <file>      Load brain state from file
```

**Simulation Control**
```
simulate <steps>       Run simulation for specified steps
simulate step          Run one simulation step
pause                  Pause simulation
simulate resume         Resume paused simulation
speed <factor>         Set simulation speed (0.1x to 10x)
duration <seconds>     Set simulation duration
time <timestamp>       Set current simulation time
```

**Neuron Control**
```
inject <neuron_id> <current> Inject current into specific neuron
inject <type> <current>    Inject current into all neurons of type
neurons                 List all neurons with basic info
neurons <neuron_id>      Show detailed neuron information
spikes <count>          Get recent spikes
spike_history <neuron_id> Show spike history for neuron
```

**Event System**
```
events                 Show recent events
events <filter>         Filter events by criteria
event_system status     Show event system status
event_system enable     Enable event processing
event_system disable    Disable event processing
subscribe <filter>      Subscribe to events matching filter
unsubscribe <callback> Remove event subscription
```

**Configuration**
```
config                 Show current configuration
config set <key> <value> Set configuration value
config get <key>        Get configuration value
config save <file>      Save configuration to file
config load <file>      Load configuration from file
config keys            List all configuration keys
```

**Statistics**
```
stats                  Show comprehensive statistics
stats neurons          Show neuron statistics
stats synapses         Show synapse statistics
stats memory           Show memory system statistics
stats performance      Show performance metrics
```

**Analysis**
```
analysis spike_patterns Detect spike patterns
analysis connectivity  Analyze neural connectivity
analysis plasticity    Analyze plasticity activity
analysis activity      Analyze overall neural activity
analysis anomalies     Detect anomalies in simulation
```

**File Operations**
```
log save <file>        Save log to file
log append <file>       Append to log file
log clear              Clear current log
file list <path>       List files in directory
file read <path>       Read file contents
file write <path> <content> Write file contents
```

**System**
```
exit                   Exit interactive console
shutdown               Gracefully shutdown simulation
help                   Show help menu
screen                 Toggle screen clearing
refresh <seconds>      Auto-refresh interval
```

## Event System in Interactive Console

The interactive console provides special commands for working with the neural event system:

### Event Monitoring
```
# Show recent events
events

# Filter events by type
events NeuronFired
events SynapticWeightChanged
events SpikePatternDetected

# Filter by timestamp
events --after 10.0 --before 20.0

# Show event system status
event_system status

# Enable/disable event processing
event_system enable
event_system disable
```

### Event Subscription
```
# Subscribe to specific event types
subscribe --type NeuronFired --callback neuron_fire_handler

subscribe --type SynapticWeightChanged --min_data 0.01
subscribe --type SpikePatternDetected --source_neuron 42

subscribe --filter "event.type == 'SpikePattern' && event.timestamp > 10.0"
```

### Event Analysis
```
# Analyze event patterns
events --analyze patterns

# Count events by type
events --count

# Show event statistics
event_system stats

# Export events to file
events --export events.json
```

## Command Examples

### Basic Usage
```
# Start interactive console
./nlm --mode=interactive

# Show help
help

# Reset brain and start fresh
brain reset
brain initialize

# Monitor brain status
brain status
stats
```

### Real-time Analysis
```
# Run simulation and monitor
simulate 1000

# Monitor spikes
events --filter "event.type == 'NeuronFired'"

# Check connectivity
analysis connectivity

# Monitor plasticity
events --filter "event.type == 'SynapticWeightChanged'"
```

### Interactive Debugging
```
# Inject current and observe
inject 0 50.0
simulate step
spikes 10

# Check neuron state
neurons 0

# Monitor events
event_system status

# Analyze patterns
events --analyze
```

## Event Filter Syntax

Event filters use a flexible query language:

### Basic Filters
```
--type NeuronFired           Filter by event type
--source 42                  Filter by source neuron ID
--target 100                 Filter by target neuron ID
--min_data 0.5               Filter by minimum data value
--max_data 1.0               Filter by maximum data value
--after 10.0                 Filter events after timestamp
--before 20.0               Filter events before timestamp
```

### Complex Filters
```
--type --any NeuronFired,SynapticWeightChanged
--source --range 0-100
--data --contains "learning"
--description "spike event"
```

### Event Callback Registration
```
# Register callbacks for specific event types
subscribe --type NeuronFired --callback on_neuron_fire
subscribe --type SynapticWeightChanged --callback on_weight_change

# Callback functions
on_neuron_fire(event, processed) {
    // Handle neuron fired event
    processed = true;
}

on_weight_change(event, processed) {
    // Handle synaptic weight change event
    processed = true;
}
```

## Integration with Other Features

### Event-Driven Learning
```
# Enable event-driven learning
config set learning.event_driven true
config set learning.event_threshold 0.1

# Subscribe to learning events
subscribe --type PredictionError --callback on_learning_error
```

### Real-time Adaptation
```
# Monitor environment events
events --type SensoryInputReceived

# Respond to events dynamically
subscribe --type RewardReceived --callback on_reward
```

### Performance Monitoring
```
# Track performance events
event_system status

# Analyze performance patterns
events --analyze performance
```

## Console Configuration

### Configuration File
```
# Interactive console configuration
[console]
input_buffer_size = 8192
command_history_size = 1000
auto_refresh_interval = 1.0
screen_clear_on_command = true
console_log_level = INFO

[event_system]
enabled = true
limit = 10000
priority_analysis = true

[monitoring]
neuron_refresh_interval = 0.5
stats_refresh_interval = 2.0
```

### Environment Variables
```
NLM_CONSOLE_MODE=interactive
NLM_EVENT_SYSTEM_ENABLED=1
NLM_AUTO_REFRESH=true
NLM_LOG_LEVEL=DEBUG
```

## Technical Implementation

### Architecture

The interactive debugging console is built with the following architecture:

1. **Command Parser**: Parses and validates user commands
2. **Command Handler**: Executes commands and manages brain interaction
3. **Event Monitor**: Monitors and displays neural events in real-time
4. **Statistics Engine**: Computes and displays brain statistics
5. **Event System Integration**: Integrates with the neural event system
6. **Input Handler**: Manages user input and console interaction
7. **Output Manager**: Manages console output and display

### Event System Integration

The console integrates with the neural event system through:

1. **Event Filtering**: Filters events based on user criteria
2. **Event Subscription**: Manages event subscriptions
3. **Event Processing**: Processes events in real-time
4. **Event Statistics**: Provides event statistics and analysis
5. **Event Export**: Exports events to files for analysis

### Performance Considerations

1. **Efficient Filtering**: Uses efficient event filtering algorithms
2. **Memory Management**: Manages memory usage for large event sets
3. **Real-time Processing**: Processes events in real-time with minimal latency
4. **Resource Management**: Efficient resource usage and cleanup

## Error Handling

### Common Errors

**Event System Errors**
```
Error: Event system not enabled
Solution: Enable event system with "event_system enable"

Error: Invalid event filter
Solution: Use valid event filter syntax

Error: Callback registration failed
Solution: Check callback function signature
```

**Command Errors**
```
Error: Command not found
Solution: Type "help" for available commands

Error: Invalid neuron ID
Solution: Use "neurons" to list valid neuron IDs

Error: Configuration key not found
Solution: Use "config keys" to list available keys
```

**Event Processing Errors**
```
Error: Event processing failed
Solution: Check event handler implementation

Error: Event filter malformed
Solution: Use proper filter syntax

Error: Event subscription conflict
Solution: Use different filter or callback
```

## Migration Guide

### From Basic to Interactive Console

If you're migrating from the basic command-line interface to the interactive console:

1. **Install Interactive Console**: Ensure NLM is built with interactive console support
2. **Start Interactive Console**: Use `./nlm --mode=interactive`
3. **Learn Basic Commands**: Start with basic commands like `help`, `brain reset`, `brain status`
4. **Explore Features**: Gradually explore advanced features like event system, statistics, analysis
5. **Customize Configuration**: Customize interactive console configuration

### Event System Integration

For users familiar with the event system:

1. **Subscribe to Events**: Use `subscribe` command to subscribe to events
2. **Filter Events**: Use event filters to select specific events
3. **Process Events**: Process events in real-time with custom callbacks
4. **Analyze Events**: Use event analysis commands for pattern detection
5. **Export Events**: Export events to files for offline analysis

## Conclusion

The NLM Interactive Debugging Console provides a comprehensive, user-friendly interface for interacting with and analyzing neural network simulations. It integrates seamlessly with the neural event system and provides powerful tools for real-time monitoring, analysis, and control.

The console is designed to be:
- **Intuitive**: Easy to learn and use
- **Powerful**: Provides comprehensive functionality
- **Flexible**: Supports a wide range of use cases
- **Efficient**: Performs well even with large simulations
- **Extensible**: Easy to extend with new features

This console significantly enhances the usability of NLM, making it accessible to users of all skill levels while providing advanced tools for expert users.