# NLM - Enhanced Python Bindings Documentation

## Overview

NLM (Neural Learning Machine) Python Bindings provide an enhanced interface to the C++ neural simulation framework. This documentation covers the advanced features and improvements made to the Python API.

## Key Improvements

### Enhanced Brain Class
- Advanced simulation control with batch processing
- Comprehensive statistics and monitoring
- Memory system access (Phase 6)
- Development system integration
- Neuromodulation system access
- Enhanced region management
- Connectivity management
- Developmental stage controls

### Enhanced AgentBrain Class
- Advanced sensory/motor group access
- Enhanced neuromodulation controls
- Development state management
- Curiosity and exploration utilities
- Enhanced initialization and reset methods
- Motor decoding with curiosity bias

### Enhanced SimpleWorld Class
- Advanced object management (resources, hazards, walls)
- Enhanced validation and error handling
- Comprehensive statistics
- World boundary management
- Batch operations
- Callback support

### Enhanced Factory Functions
- Configuration profiles (development, exploration, learning)
- World presets (small, medium, large)
- Complete system factories
- Simulation setup utilities
- Random seed generation

### Enhanced Utilities
- System validation and health checks
- Performance monitoring
- Error handling and validation
- Debug and logging utilities
- Backup and restore capabilities
- Episode simulation helpers

## Quick Start (Enhanced)

### Basic Setup

```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrainWithDefaults()
brain.initialize()

# Run basic simulation
brain.runBatch(100)  # Run 100 steps efficiently
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete System Setup

```python
import pynlm

# Create complete system with learning enabled
brain, world, agent = pynlm.createSimulationSetup(
    enableLearning=True,
    enableDevelopment=True,
    enableCuriosity=True
)

# Run simulation with custom step logic
custom_step = lambda step, brain, world, agent: None  # Define your custom logic
brain.runAdaptiveEpisode(1000, custom_step)

# Get performance statistics
stats = pynlm.getPerformanceStats(brain, world)
print(f"Simulation stats: {stats}")
```

### Development-Focused Configuration

```python
import pynlm

# Create development-oriented configuration
development_config = pynlm.createDevelopmentConfig()
brain = pynlm.createInitializedBrain(development_config)

# Enable advanced features
agent = pynlm.createAgentBrain(brain)
agent.enableStructuralPlasticity(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

### Advanced World Management

```python
import pynlm

# Create medium-sized world
world = pynlm.createMediumWorld()

# Add world objects
world.addResource(10.0, 10.0, value=2.0)      # Resource
world.addHazard(20.0, 20.0, hazardValue=0.8)   # Hazard
world.addWall(5.0, 5.0, wallWidth=3.0)          # Wall

# Validate position before movement
if world.isPositionValid(15.0, 15.0):
    agent.applyMotorCommandWithValidation(
        pynlm.MotorCommand.MoveForward,
        world.getSimulationTime()
    )

# Get world statistics
world_stats = world.getWorldStats()
print(f"Agent position: ({world_stats['agent_x']}, {world_stats['agent_y']})")
```

## Advanced Examples

### Example 1: Learning Agent with Memory

```python
import pynlm

def learning_agent_simulation():
    # Create learning-focused system
    brain, world, agent = pynlm.createSimulationSetup(
        enableLearning=True,
        enableDevelopment=True,
        enableCuriosity=True
    )
    
    # Enable learning mechanisms
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    
    # Get memory systems for inspection
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    # Run learning simulation
    for episode in range(10):
        brain.reset()
        agent.reset()
        world.reset()
        
        # Run episode
        brain.runBatch(100)
        
        # Get learning statistics
        stats = brain.getSimulationStats()
        print(f"Episode {episode}: Spikes={stats['total_spikes']}, "
              f"Active={stats['active_neurons']}, Firing={stats['firing_neurons']}")
        
        # Apply reward-based learning
        reward = calculate_reward(world, agent)
        predicted_reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
    
    return brain, world, agent

def calculate_reward(world, agent):
    # Custom reward calculation
    agent_body = world.getAgentBody()
    return agent_body.energy * 0.1 - agent_body.health * 0.05
```

### Example 2: World Exploration with Curiosity

```python
import pynlm

def exploration_simulation():
    # Create exploration-oriented system
    brain, world, agent = pynlm.createSimulationSetup(
        enableLearning=False,
        enableDevelopment=True,
        enableCuriosity=True
    )
    
    # Configure exploration parameters
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Add exploration targets
    world.addResource(30.0, 30.0, value=5.0)  # High-value resource
    world.addHazard(50.0, 50.0, hazardValue=0.9)  # High hazard
    world.addWall(40.0, 40.0, wallWidth=2.0)   # Obstacle
    
    # Run exploration simulation
    exploration_steps = []
    for step in range(500):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes information
        brain.step(step)
        
        # Agent selects action with curiosity bias
        action = agent.decodeMotorCommand()
        
        # Apply action with validation
        if world.isPositionValid(world.getAgentBody().x, world.getAgentBody().y):
            world.applyMotorCommandWithValidation(action, world.getSimulationTime())
        
        # Track exploration
        exploration_steps.append({
            'step': step,
            'position': (world.getAgentBody().x, world.getAgentBody().y),
            'action': action,
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel()
        })
    
    return exploration_steps, brain, world, agent
```

### Example 3: Development and Maturation

```python
import pynlm

def development_simulation():
    # Create development-focused system
    development_config = pynlm.createDevelopmentConfig()
    brain = pynlm.createInitializedBrain(development_config)
    
    # Create agent
    world = pynlm.createMediumWorld()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Track development over time
    development_log = []
    
    for step in range(1000):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes information
        brain.step(step)
        
        # Log developmental stage
        developmental_stage = brain.getDevelopmentalStage()
        stage_name = brain.getDevelopmentalStageName()
        
        development_log.append({
            'step': step,
            'time': step * 0.1,
            'developmental_stage': developmental_stage,
            'stage_name': stage_name,
            'neuromodulation': agent.getNeuromodulationLevel(),
            'curiosity': agent.getCuriosityLevel(),
            'plasticity_modifier': agent.getPlasticityModifier()
        })
        
        # Change developmental stage for testing
        if step == 200:
            brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
        elif step == 500:
            brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
    
    return development_log, brain, world, agent
```

### Example 4: System Validation and Health Monitoring

```python
import pynlm

def system_health_check():
    # Create complete system
    brain, world, agent = pynlm.createCompleteSystem()
    
    # Validate individual components
    print("Validating system components...")
    
    # Check configuration
    config_valid = pynlm.validateConfig(brain.getConfig())
    print(f"Configuration valid: {config_valid}")
    
    # Check brain
    brain_valid = pynlm.validateBrain(brain)
    print(f"Brain valid: {brain_valid}")
    
    # Check world
    world_valid = pynlm.validateWorld(world)
    print(f"World valid: {world_valid}")
    
    # Check agent
    agent_valid = pynlm.validateAgent(agent)
    print(f"Agent valid: {agent_valid}")
    
    # Get comprehensive system status
    system_status = pynlm.validateSystem()
    print(f"System status: {system_status}")
    
    # Get performance statistics
    performance_stats = pynlm.getPerformanceStats(brain, world)
    print(f"Performance stats: {performance_stats}")
    
    # Run health check
    pynlm.logSystemStatus()
    
    return {
        'config_valid': config_valid,
        'brain_valid': brain_valid,
        'world_valid': world_valid,
        'agent_valid': agent_valid,
        'system_status': system_status,
        'performance_stats': performance_stats
    }
```

### Example 5: Batch Processing and Performance

```python
import pynlm
import time

def performance_benchmark():
    # Create system
    brain, world, agent = pynlm.createSimulationSetup()
    
    # Benchmark batch processing
    print("Benchmarking batch processing...")
    
    # Test batch processing with different sizes
    for batch_size in [100, 500, 1000]:
        start_time = time.time()
        
        brain.runBatch(batch_size)
        
        end_time = time.time()
        duration = end_time - start_time
        
        print(f"Batch size {batch_size}: {duration:.3f}s "
              f"({batch_size/duration:.0f} steps/second)")
    
    # Benchmark step with input
    print("Benchmarking step with input...")
    
    start_time = time.time()
    for step in range(100):
        brain.runStepWithInput(step, world.getSensoryPercept())
    end_time = time.time()
    
    print(f"100 steps with input: {end_time - start_time:.3f}s")
    
    # Get final statistics
    stats = brain.getSimulationStats()
    print(f"Final stats: {stats}")
    
    return stats
```

### Example 6: Advanced Configuration Management

```python
import pynlm

def advanced_configuration():
    # Create configuration from dictionary
    settings = {
        'development_enabled': True,
        'plasticity_enabled': True,
        'curiosity_enabled': True,
        'reward_modulation_enabled': True,
        'structural_plasticity_enabled': False,
        'prediction_enabled': True,
        'working_memory_enabled': True,
        'initial_neurons': 1000,
        'initial_regions': 5
    }
    
    config = pynlm.createConfig(settings)
    
    # Create brain with custom config
    brain = pynlm.createInitializedBrain(config)
    
    # Modify configuration at runtime
    brain.getConfig().set('learning_rate', 0.01)
    brain.getConfig().set('exploration_rate', 0.1)
    
    # Get configuration summary
    summary = brain.getConfig().summary()
    print(f"Configuration summary: {summary}")
    
    # Save and restore configuration
    pynlm.backupBrainState(brain, "backup.brain")
    
    # Run simulation
    for step in range(50):
        brain.step(step)
    
    # Restore state
    pynlm.restoreBrainState(brain, "backup.brain")
    
    # Verify restoration
    stats_after_restore = brain.getSimulationStats()
    print(f"Stats after restore: {stats_after_restore}")
    
    return config, brain
```

## API Reference

### Core Classes

#### Brain
- `createBrain(config)` - Create brain with configuration
- `createBrainWithDefaults()` - Create brain with default config
- `createInitializedBrain(config)` - Create and initialize brain
- `runBatch(numSteps, withTimestamps)` - Run batch processing
- `runStepWithInput(step, input, withTimestamp)` - Run step with input
- `getSimulationStats()` - Get comprehensive statistics
- `getWorkingMemory()` - Get working memory system
- `getEpisodicMemory()` - Get episodic memory system
- `getPredictionSystem()` - Get prediction system
- `getDevelopmentSystem()` - Get development system

#### AgentBrain
- `createAgentBrain(brain)` - Create agent brain
- `initializeAgent(world)` - Enhanced initialization
- `processStep(percept)` - Process step with readiness check
- `decodeAction(brain)` - Enhanced action decoding
- `enableDevelopment(enabled)` - Enable development
- `getDevelopmentalAge()` - Get developmental age

#### SimpleWorld
- `createSimpleWorld()` - Create world
- `createSmallWorld()` - Create small world
- `createMediumWorld()` - Create medium world
- `createLargeWorld()` - Create large world
- `resetWithAgent(x, y, orientation)` - Reset with agent
- `addResource(x, y, value, radius)` - Add resource
- `addHazard(x, y, hazardValue, radius)` - Add hazard
- `addWall(x, y, wallWidth, wallHeight)` - Add wall
- `isPositionValid(x, y)` - Enhanced validation
- `getWorldStats()` - Get comprehensive world stats

### Configuration

#### Factory Functions
- `createDefaultConfig()` - Create default config
- `createDevelopmentConfig()` - Create development config
- `createExplorationConfig()` - Create exploration config
- `createLearningConfig()` - Create learning config
- `createConfig(settings)` - Create config from dictionary

#### Configuration Methods
- `validateConfig(config)` - Validate configuration
- `backupBrainState(brain, filepath)` - Backup brain state
- `restoreBrainState(brain, filepath)` - Restore brain state

### System Utilities

#### Factory Functions
- `createCompleteSystem()` - Create complete system
- `createSimulationSetup()` - Create simulation setup

#### Monitoring and Validation
- `validateSystem()` - Validate complete system
- `getPerformanceStats(brain, world)` - Get performance statistics
- `logSystemStatus()` - Log system status
- `logBrainStatus(brain)` - Log brain status

### Episode Management

- `runEpisode(brain, world, agent, numSteps)` - Run episode
- `runAdaptiveEpisode(brain, world, agent, maxSteps, customStepLogic)` - Run adaptive episode

### Random Seed Management
- `createRandomSeed()` - Generate random seed
- `setRandomSeed(seed, world)` - Set random seed for world

## Best Practices

### 1. Use Factory Functions for Common Setups
```python
# Instead of manual setup
brain = pynlm.createBrainWithDefaults()
brain.initialize()

# Use enhanced factory
brain, world, agent = pynlm.createSimulationSetup(
    enableLearning=True,
    enableDevelopment=True
)
```

### 2. Leverage Batch Processing for Performance
```python
# Instead of individual steps
for i in range(1000):
    brain.step(i)

# Use batch processing
brain.runBatch(1000)
```

### 3. Use Enhanced Validation
```python
# Instead of manual validation
if world.isValidPosition(x, y):
    world.applyMotorCommand(action, time)

# Use enhanced validation
if world.isPositionValid(x, y):
    world.applyMotorCommandWithValidation(action, time)
```

### 4. Monitor System Health
```python
# Regular health checks
health = pynlm.validateSystem()
if health['status'] == 'ready':
    print("System is healthy and ready")
```

### 5. Use Configuration Profiles
```python
# Use appropriate configuration profiles
if learning_mode:
    config = pynlm.createLearningConfig()
elif exploration_mode:
    config = pynlm.createExplorationConfig()
else:
    config = pynlm.createDevelopmentConfig()
```

## Migration Guide

### From Basic to Enhanced API

#### Basic Setup
```python
# Old way
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)
```

#### Enhanced Setup
```python
# New way
brain, world, agent = pynlm.createSimulationSetup()
brain.runBatch(100)
```

#### Basic Statistics
```python
# Old way
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

#### Enhanced Statistics
```python
# New way
stats = brain.getSimulationStats()
print(f"Total spikes: {stats['total_spikes']}")
print(f"Active neurons: {stats['active_neurons']}")
```

## Error Handling

### Common Errors and Solutions

#### Configuration Errors
```python
# Problem: Invalid configuration
try:
    config = pynlm.createConfig(invalid_settings)
except Exception as e:
    print(f"Configuration error: {e}")
    config = pynlm.createDefaultConfig()
```

#### Validation Errors
```python
# Problem: Invalid world position
if not world.isPositionValid(x, y):
    print("Position is not valid")
    # Move agent to valid position
    world.setAgentStart(10.0, 10.0)
```

#### System Errors
```python
# Problem: System not ready
status = pynlm.validateSystem()
if status['status'] != 'ready':
    print("System is not ready")
    # Initialize system
    brain = pynlm.createInitializedBrain(pynlm.createDefaultConfig())
```

## Performance Tips

### 1. Use Batch Processing
- Use `runBatch()` for multiple simulation steps
- Reduces function call overhead

### 2. Minimize Object Creation
- Reuse objects where possible
- Use configuration profiles for common setups

### 3. Monitor Performance
- Use `getPerformanceStats()` regularly
- Profile batch processing vs individual steps

### 4. Use Appropriate Configuration
- Use development config for testing
- Use exploration config for exploration
- Use learning config for learning-focused tasks

## Troubleshooting

### Common Issues

#### Issue: Brain not processing input
```python
# Check if sensory input is connected
percept = world.getSensoryPercept()
if percept:
    agent.processSensoryInput(percept)
else:
    print("No sensory input available")
```

#### Issue: Agent not moving
```python
# Check world validation
if world.isPositionValid(world.getAgentBody().x, world.getAgentBody().y):
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
else:
    print("Agent position is invalid")
```

#### Issue: System validation failures
```python
# Check system status
status = pynlm.validateSystem()
print(f"System status: {status}")
if status['status'] != 'ready':
    print("System needs initialization")
```

## Conclusion

The enhanced NLM Python bindings provide a comprehensive and powerful interface for neural simulation. With advanced factory functions, batch processing, comprehensive validation, and extensive monitoring capabilities, developers can create sophisticated neural simulations efficiently and effectively.

Key advantages:
- Enhanced performance through batch processing
- Comprehensive validation and error handling
- Advanced configuration management
- Detailed monitoring and statistics
- Flexible factory functions for common setups
- Development-focused configuration profiles

This documentation provides the foundation for using the enhanced NLM Python bindings effectively in neural simulation projects.