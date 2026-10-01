# WorldAdvanced Documentation

## Overview

This document provides comprehensive documentation for the WorldAdvanced class, which offers advanced Python APIs for the SimpleWorld class, including world metrics, environment metrics, and simulation statistics APIs.

## Introduction

The WorldAdvanced class provides enhanced access to SimpleWorld functionality through Python-friendly APIs, including comprehensive world metrics, environment metrics, and simulation statistics capabilities.

## Key Features

### World Metrics
- **World Statistics**: Monitor world dimensions, agent position, object count, and simulation time
- **Agent Metrics**: Track agent position, orientation, velocity, energy, health, and age
- **Environment Metrics**: Analyze world size, complexity, density, stability, and resource availability
- **Simulation Statistics**: Monitor simulation progress, performance, and efficiency

### Environment Metrics
- **Environmental Complexity**: Measure world complexity based on dimensions and objects
- **Environmental Stability**: Track environmental stability over time
- **Resource Availability**: Monitor resource availability (food, water, shelter, energy, oxygen)
- **Challenge Level**: Calculate challenge level based on complexity and stability

### Performance Metrics
- **Performance Statistics**: Monitor frame rate, simulation efficiency, memory usage, CPU usage, and network usage
- **System Resources**: Get system resource information (CPU, memory, disk, network)
- **World Load**: Calculate world load based on dimensions and objects

## API Reference

### Constructor
```python
def __init__(self, world):
    """Initialize WorldAdvanced with a SimpleWorld object
    
    Args:
        world: The SimpleWorld object to wrap
    """
```

### Basic World Operations

#### Core Operations

**`configure(width, height, vision_width, vision_height)`**
```python
def configure(self, width, height, vision_width, vision_height):
    """Configure the world
    
    Args:
        width (int): World width
        height (int): World height
        vision_width (int): Vision width
        vision_height (int): Vision height
    """
```

**`update(timestep)`**
```python
def update(self, timestep):
    """Update world state
    
    Args:
        timestep (float): Time step for simulation
    """
```

**`reset()`**
```python
def reset(self):
    """Reset world state
    
    Returns:
        bool: True if reset successful, False otherwise
    """
```

**`setAgentStart(x, y)`**
```python
def setAgentStart(self, x, y):
    """Set agent start position
    
    Args:
        x (float): X coordinate
        y (float): Y coordinate
    """
```

**`applyMotorCommand(cmd, current_time)`**
```python
def applyMotorCommand(self, cmd, current_time):
    """Apply motor command
    
    Args:
        cmd: Motor command
        current_time (float): Current simulation time
    """
```

**`getSensoryPercept()`**
```python
def getSensoryPercept(self):
    """Get sensory percept
    
    Returns:
        SensoryPercept: Sensory percept
    """
```

**`getAgentBody()`**
```python
def getAgentBody(self):
    """Get agent body
    
    Returns:
        AgentBody: Agent body
    """
```

**`addObject(obj)`**
```python
def addObject(self, obj):
    """Add world object
    
    Args:
        obj: World object
    """
```

**`removeObject(x, y)`**
```python
def removeObject(self, x, y):
    """Remove world object at position
    
    Args:
        x (float): X coordinate
        y (float): Y coordinate
    """
```

**`isValidPosition(x, y)`**
```python
def isValidPosition(self, x, y):
    """Check if position is valid
    
    Args:
        x (float): X coordinate
        y (float): Y coordinate
    
    Returns:
        bool: True if position is valid, False otherwise
    """
```

### World Information APIs

**`getWidth()`**
```python
def getWidth(self):
    """Get world width
    
    Returns:
        int: World width
    """
```

**`getHeight()`**
```python
def getHeight(self):
    """Get world height
    
    Returns:
        int: World height
    """
```

**`getMaxEnergy()`**
```python
def getMaxEnergy(self):
    """Get maximum energy
    
    Returns:
        float: Maximum energy
    """
```

**`setMaxEnergy(e)`**
```python
def setMaxEnergy(self, e):
    """Set maximum energy
    
    Args:
        e (float): Maximum energy
    """
```

**`getEnergyDecayRate()`**
```python
def getEnergyDecayRate(self):
    """Get energy decay rate
    
    Returns:
        float: Energy decay rate
    """
```

**`setEnergyDecayRate(r)`**
```python
def setEnergyDecayRate(self, r):
    """Set energy decay rate
    
    Args:
        r (float): Energy decay rate
    """
```

**`getSimulationTime()`**
```python
def getSimulationTime(self):
    """Get simulation time
    
    Returns:
        float: Simulation time
    """
```

**`setRandomSeed(seed)`**
```python
def setRandomSeed(self, seed):
    """Set random seed
    
    Args:
        seed (int): Random seed
    """
```

**`getRandomSeed()`**
```python
def getRandomSeed(self):
    """Get random seed
    
    Returns:
        int: Random seed
    """
```

### World Metrics APIs

#### World Metrics

**`getWorldMetrics()`**
```python
def getWorldMetrics(self):
    """Get world metrics
    
    Returns:
        dict: Dictionary with world metrics:
            - dimensions (dict): World dimensions (width, height)
            - agent_position (dict): Agent position (x, y)
            - object_count (int): Number of active objects
            - simulation_time (float): Simulation time
            - world_complexity (float): World complexity (0.0 to 1.0)
    """
```

**`getAgentMetrics()`**
```python
def getAgentMetrics(self):
    """Get agent metrics
    
    Returns:
        dict: Dictionary with agent metrics:
            - position (dict): Agent position (x, y)
            - orientation (float): Agent orientation
            - velocity (dict): Agent velocity (x, y)
            - energy (float): Agent energy
            - health (float): Agent health (0.0 to 1.0)
            - age (float): Agent age
            - is_moving (bool): Whether agent is moving
            - is_turning (bool): Whether agent is turning
            - speed (float): Agent speed
            - efficiency (float): Agent efficiency
    """
```

**`getEnvironmentMetrics()`**
```python
def getEnvironmentMetrics(self):
    """Get environment metrics
    
    Returns:
        dict: Dictionary with environment metrics:
            - size (dict): World size (width, height)
            - area (float): World area
            - complexity (float): Environmental complexity (0.0 to 1.0)
            - density (float): Environmental density (0.0 to 1.0)
            - stability (float): Environmental stability (0.0 to 1.0)
            - resources (dict): Resource availability
    """
```

**`getSimulationStats()`**
```python
def getSimulationStats(self):
    """Get simulation statistics
    
    Returns:
        dict: Dictionary with simulation statistics:
            - steps_completed (int): Number of steps completed
            - objects_active (int): Number of active objects
            - simulation_uptime (float): Simulation uptime
            - performance_metrics (dict): Performance metrics
    """
```

**`getPerformanceStats()`**
```python
def getPerformanceStats(self):
    """Get performance statistics
    
    Returns:
        dict: Dictionary with performance statistics:
            - frame_rate (float): Frame rate (Hz)
            - simulation_efficiency (float): Simulation efficiency (0.0 to 1.0)
            - memory_usage (float): Memory usage
            - cpu_usage (float): CPU usage (0.0 to 1.0)
            - gpu_usage (float): GPU usage (0.0 to 1.0)
            - bandwidth_usage (float): Bandwidth usage (0.0 to 1.0)
    """
```

### Environment APIs

**`getEnvironmentalComplexity()`**
```python
def getEnvironmentalComplexity(self):
    """Get environmental complexity
    
    Returns:
        float: Environmental complexity (0.0 to 1.0)
    """
```

**`getEnvironmentalStability()`**
```python
def getEnvironmentalStability(self):
    """Get environmental stability
    
    Returns:
        float: Environmental stability (0.0 to 1.0)
    """
```

**`getResourceAvailability()`**
```python
def getResourceAvailability(self):
    """Get resource availability
    
    Returns:
        dict: Dictionary with resource availability:
            - food (float): Food availability (0.0 to 1.0)
            - water (float): Water availability (0.0 to 1.0)
            - shelter (float): Shelter availability (0.0 to 1.0)
            - energy (float): Energy availability (0.0 to 1.0)
            - oxygen (float): Oxygen availability (0.0 to 1.0)
    """
```

**`getChallengeLevel()`**
```python
def getChallengeLevel(self):
    """Get challenge level
    
    Returns:
        float: Challenge level (0.0 to 1.0)
    """
```

### Performance APIs

**`getPerformanceMetrics()`**
```python
def getPerformanceMetrics(self):
    """Get performance metrics
    
    Returns:
        dict: Dictionary with performance metrics:
            - frame_rate (float): Frame rate (Hz)
            - simulation_speed (float): Simulation speed (0.0 to 1.0)
            - update_time (float): Update time (seconds)
            - memory_usage (float): Memory usage
            - cpu_usage (float): CPU usage (0.0 to 1.0)
            - gpu_usage (float): GPU usage (0.0 to 1.0)
            - network_usage (float): Network usage (0.0 to 1.0)
            - disk_usage (float): Disk usage (0.0 to 1.0)
    """
```

**`getFrameRate()`**
```python
def getFrameRate(self):
    """Get frame rate
    
    Returns:
        float: Frame rate (Hz)
    """
```

**`getSimulationEfficiency()`**
```python
def getSimulationEfficiency(self):
    """Get simulation efficiency
    
    Returns:
        float: Simulation efficiency (0.0 to 1.0)
    """
```

**`getMemoryUsage()`**
```python
def getMemoryUsage(self):
    """Get memory usage
    
    Returns:
        float: Memory usage
    """
```

### Error APIs

**`validateWorldState()`**
```python
def validateWorldState(self):
    """Validate world state
    
    Returns:
        dict: Dictionary with validation results:
            - valid (bool): Whether world state is valid
            - errors (list): List of validation errors
            - warnings (list): List of validation warnings
    """
```

**`getWorldErrors()`**
```python
def getWorldErrors(self):
    """Get world errors
    
    Returns:
        list: List of world errors
    """
```

**`clearWorldErrors()`**
```python
def clearWorldErrors(self):
    """Clear world errors
    
    Returns:
        bool: True if errors cleared successfully, False otherwise
    """
```

### Diagnostic APIs

**`diagnoseWorld()`**
```python
def diagnoseWorld(self):
    """Comprehensive world diagnosis
    
    Returns:
        dict: Dictionary with diagnosis results:
            - world_id (str): World identifier
            - dimensions (dict): World dimensions (width, height)
            - agent_status (dict): Agent status
            - environment_status (dict): Environment status
            - performance_status (dict): Performance status
            - errors (list): List of errors
            - health_status (str): Health status (healthy, needs_attention)
    """
```

### Tool APIs

**`executeTool(tool_name, parameters)`**
```python
def executeTool(self, tool_name, parameters):
    """Execute a tool
    
    Args:
        tool_name (str): Name of the tool to execute
        parameters (dict): Parameters for the tool
    
    Returns:
        dict: Tool execution result
    """
```

**`runToolPipeline(tools, parameters)`**
```python
def runToolPipeline(self, tools, parameters):
    """Run a tool pipeline
    
    Args:
        tools (list): List of tool names to execute
        parameters (dict): Parameters for the tools
    
    Returns:
        dict: Pipeline execution result
    """
```

### Python Integration APIs

#### Version and Information APIs

**`getPythonBindingsVersion()`**
```python
def getPythonBindingsVersion(self):
    """Get Python bindings version
    
    Returns:
        str: Python bindings version
    """
```

**`getPythonBindingsInfo()`**
```python
def getPythonBindingsInfo(self):
    """Get Python bindings information
    
    Returns:
        dict: Python bindings information including version, platform, python version, and features
    """
```

### Resource Monitoring APIs

**`getSystemResources()`**
```python
def getSystemResources(self):
    """Get system resources
    
    Returns:
        dict: Dictionary with system resources:
            - cpu_percent (float): CPU usage (0.0 to 1.0)
            - memory_percent (float): Memory usage (0.0 to 1.0)
            - disk_percent (float): Disk usage (0.0 to 1.0)
            - network_io (dict): Network I/O counters
    """
```

**`getWorldLoad()`**
```python
def getWorldLoad(self):
    """Get world load
    
    Returns:
        float: World load (0.0 to 1.0)
    """
```

## Usage Examples

### Basic World Usage

```python
from nlm import WorldAdvanced

# Create and initialize world
world = WorldAdvanced(world)

# Configure world
world.configure(100, 100, 20, 20)

# Set agent start position
world.setAgentStart(50, 50)

# Reset world
world.reset()

# Get world information
print(f"World Width: {world.getWidth()}")
print(f"World Height: {world.getHeight()}")
print(f"Agent Start: {world.getAgentStart()}")
```

### World Metrics Usage

```python
world = WorldAdvanced(world)

# Configure world
world.configure(100, 100, 20, 20)

# Get world metrics
world_metrics = world.getWorldMetrics()
print("World Metrics:")
print(f"  Dimensions: {world_metrics['dimensions']}")
print(f"  Agent Position: {world_metrics['agent_position']}")
print(f"  Object Count: {world_metrics['object_count']}")
print(f"  Simulation Time: {world_metrics['simulation_time']}")
print(f"  World Complexity: {world_metrics['world_complexity']:.3f}")

# Get agent metrics
agent_metrics = world.getAgentMetrics()
print(f"\nAgent Metrics:")
print(f"  Position: {agent_metrics['position']}")
print(f"  Orientation: {agent_metrics['orientation']:.3f}")
print(f"  Velocity: {agent_metrics['velocity']}")
print(f"  Energy: {agent_metrics['energy']:.3f}")
print(f"  Health: {agent_metrics['health']:.3f}")
print(f"  Age: {agent_metrics['age']:.3f}")
print(f"  Speed: {agent_metrics['speed']:.3f}")
print(f"  Efficiency: {agent_metrics['efficiency']:.3f}")
```

### Environment Metrics Usage

```python
world = WorldAdvanced(world)

# Configure world
world.configure(100, 100, 20, 20)

# Get environment metrics
env_metrics = world.getEnvironmentMetrics()
print("Environment Metrics:")
print(f"  Size: {env_metrics['size']}")
print(f"  Area: {env_metrics['area']}")
print(f"  Complexity: {env_metrics['complexity']:.3f}")
print(f"  Density: {env_metrics['density']:.3f}")
print(f"  Stability: {env_metrics['stability']:.3f}")

# Get resource availability
resources = world.getResourceAvailability()
print(f"\nResource Availability:")
for resource, availability in resources.items():
    print(f"  {resource}: {availability:.3f}")

# Get environmental complexity and stability
complexity = world.getEnvironmentalComplexity()
stability = world.getEnvironmentalStability()
print(f"\nEnvironmental Status:")
print(f"  Complexity: {complexity:.3f}")
print(f"  Stability: {stability:.3f}")

# Get challenge level
challenge = world.getChallengeLevel()
print(f"  Challenge Level: {challenge:.3f}")
```

### Performance Metrics Usage

```python
world = WorldAdvanced(world)

# Configure world
world.configure(100, 100, 20, 20)

# Get performance metrics
perf_metrics = world.getPerformanceMetrics()
print("Performance Metrics:")
print(f"  Frame Rate: {perf_metrics['frame_rate']:.3f} Hz")
print(f"  Simulation Speed: {perf_metrics['simulation_speed']:.3f}")
print(f"  Update Time: {perf_metrics['update_time']:.3f} s")
print(f"  Memory Usage: {perf_metrics['memory_usage']}")
print(f"  CPU Usage: {perf_metrics['cpu_usage']:.3f}%")
print(f"  GPU Usage: {perf_metrics['gpu_usage']:.3f}%")

# Get system resources
system_resources = world.getSystemResources()
print(f"\nSystem Resources:")
print(f"  CPU Usage: {system_resources['cpu_percent']:.3f}%")
print(f"  Memory Usage: {system_resources['memory_percent']:.3f}%")
print(f"  Disk Usage: {system_resources['disk_percent']:.3f}%")
```

### Simulation Statistics Usage

```python
world = WorldAdvanced(world)

# Configure world
world.configure(100, 100, 20, 20)

# Get simulation statistics
sim_stats = world.getSimulationStats()
print("Simulation Statistics:")
print(f"  Steps Completed: {sim_stats['steps_completed']}")
print(f"  Objects Active: {sim_stats['objects_active']}")
print(f"  Simulation Uptime: {sim_stats['simulation_uptime']:.3f}")
print(f"  Performance Metrics: {sim_stats['performance_metrics']}")

# Get world load
world_load = world.getWorldLoad()
print(f"\nWorld Load: {world_load:.3f}")
```

### Error Handling Usage

```python
world = WorldAdvanced(world)

# Validate world state
validation_result = world.validateWorldState()
if not validation_result['valid']:
    print(f"Validation Errors: {validation_result['errors']}")
    print(f"Validation Warnings: {validation_result['warnings']}")
else:
    print("World state is valid.")

# Get and clear world errors
errors = world.getWorldErrors()
if errors:
    print(f"World Errors: {errors}")
    world.clearWorldErrors()
    print("World errors cleared.")
```

### Diagnostic APIs Usage

```python
world = WorldAdvanced(world)

# Configure world
world.configure(100, 100, 20, 20)

# Get comprehensive diagnosis
diagnosis = world.diagnoseWorld()
print("World Diagnosis:")
print(f"  World ID: {diagnosis['world_id']}")
print(f"  Dimensions: {diagnosis['dimensions']}")
print(f"  Agent Status: {diagnosis['agent_status']}")
print(f"  Environment Status: {diagnosis['environment_status']}")
print(f"  Performance Status: {diagnosis['performance_status']}")
print(f"  Health Status: {diagnosis['health_status']}")

# Handle errors if any
if diagnosis['errors']:
    print(f"Errors found: {diagnosis['errors']}")
```

### Tool APIs Usage

```python
world = WorldAdvanced(world)

# Configure world
world.configure(100, 100, 20, 20)

# Get available tools
tools = world.getAvailableTools()
print(f"Available Tools:")
for tool in tools:
    print(f"  - {tool['name']}: {tool['description']}")

# Execute a specific tool
tool_name = "world_analyzer"
parameters = {
    'world_id': id(world),
    'analysis_type': 'performance'
}
result = world.executeTool(tool_name, parameters)
print(f"\nTool Execution Result:")
print(f"  Status: {result['status']}")
print(f"  Execution Time: {result['execution_time']:.3f}s")

# Run tool pipeline
pipeline_tools = ['world_analyzer', 'environment_analyzer', 'performance_profiler']
pipeline_result = world.runToolPipeline(pipeline_tools, parameters)
print(f"\nPipeline Results:")
print(f"  Tools Executed: {pipeline_result['tools_executed']}")
print(f"  Pipeline Success: {pipeline_result['pipeline_success']}")
print(f"  Total Execution Time: {pipeline_result['total_execution_time']:.3f}s")
```

## Performance Tips

### 1. World Configuration Optimization

```python
world = WorldAdvanced(world)

# Optimize world configuration for performance
world.configure(100, 100, 20, 20)  # Reasonable dimensions

# Set appropriate energy decay rate
world.setEnergyDecayRate(0.01)  # Slow decay for stable environment
```

### 2. Environment Analysis Optimization

```python
world = WorldAdvanced(world)

# Analyze environment for optimization
env_metrics = world.getEnvironmentMetrics()
print(f"Environmental Complexity: {env_metrics['complexity']:.3f}")

# Adjust configuration based on environment
if env_metrics['complexity'] > 0.8:
    print("Environment is highly complex. Consider reducing world size or object count.")
```

### 3. Performance Monitoring Optimization

```python
world = WorldAdvanced(world)

# Monitor performance continuously
while True:
    # Get current performance metrics
    perf_metrics = world.getPerformanceMetrics()
    
    # Check for performance issues
    if perf_metrics['cpu_usage'] > 0.8:
        print("High CPU usage detected.")
    
    if perf_metrics['memory_usage'] > 100 * 1024 * 1024:  # 100 MB
        print("High memory usage detected.")
    
    # Wait for next monitoring cycle
    import time
    time.sleep(1.0)
```

## Best Practices

### 1. Error Handling Best Practices

```python
world = WorldAdvanced(world)

# Always handle errors gracefully
if not world.validateWorldState()['valid']:
    errors = world.getWorldErrors()
    print(f"Validation Errors: {errors}")
    
    # Clear errors if any
    world.clearWorldErrors()
    print("World errors cleared.")
```

### 2. Performance Monitoring Best Practices

```python
world = WorldAdvanced(world)

# Monitor performance continuously
while True:
    # Get performance metrics
    perf_metrics = world.getPerformanceMetrics()
    
    # Check for performance issues
    if perf_metrics['frame_rate'] < 30.0:
        print("Frame rate below optimal.")
    
    # Optimize if needed
    if perf_metrics['cpu_usage'] > 0.9:
        print("High CPU usage. Consider optimization.")
    
    # Wait for next monitoring cycle
    import time
    time.sleep(1.0)
```

### 3. World Configuration Best Practices

```python
world = WorldAdvanced(world)

# Always configure world before use
world.configure(100, 100, 20, 20)

# Set appropriate agent start position
world.setAgentStart(50, 50)

# Validate world state
if not world.validateWorldState()['valid']:
    print("World configuration validation failed.")
    print("Errors:", world.getWorldErrors())
```

### 4. Tool Usage Best Practices

```python
world = WorldAdvanced(world)

# Validate tool parameters before execution
tool_name = "world_analyzer"
parameters = {'world_id': id(world)}

# Check tool information
tool_info = world.getToolInfo(tool_name)
if tool_info:
    # Validate parameters
    required_params = tool_info.get('parameters', [])
    provided_params = parameters.keys()
    
    if set(required_params).issubset(set(provided_params)):
        result = world.executeTool(tool_name, parameters)
        print(f"Tool execution successful: {result['status']}")
    else:
        print(f"Missing required parameters: {set(required_params) - set(provided_params)}")
```

## Troubleshooting

### Common Issues and Solutions

1. **World Configuration Issues**
   - **Issue**: World configuration fails
   - **Solution**: Use `world.configure()` to set appropriate world dimensions

2. **Environment Analysis Issues**
   - **Issue**: Environment analysis fails
   - **Solution**: Use `world.getEnvironmentMetrics()` to analyze environment

3. **Performance Issues**
   - **Issue**: Performance metrics are poor
   - **Solution**: Use `world.getPerformanceMetrics()` to identify performance issues

4. **Error Handling Issues**
   - **Issue**: Error handling fails
   - **Solution**: Use `world.validateWorldState()` to validate world state

5. **Tool Execution Issues**
   - **Issue**: Tool execution fails
   - **Solution**: Use `world.getToolInfo()` to validate tool availability and parameters

### Debug Commands

```python
world = WorldAdvanced(world)

# Debug world configuration
print(f"World Dimensions: {world.getWidth()}x{world.getHeight()}")
print(f"Agent Start: {world.getAgentStart()}")

# Debug environment metrics
print(f"Environmental Complexity: {world.getEnvironmentalComplexity()}")
print(f"Environmental Stability: {world.getEnvironmentalStability()}")

# Debug performance metrics
perf_metrics = world.getPerformanceMetrics()
print(f"Frame Rate: {perf_metrics['frame_rate']}")
print(f"CPU Usage: {perf_metrics['cpu_usage']}")
print(f"Memory Usage: {perf_metrics['memory_usage']}")

# Debug world diagnosis
diagnosis = world.diagnoseWorld()
print(f"World Diagnosis: {diagnosis}")
```

## Migration Guide

### From Basic to Advanced API

If you're migrating from basic NLM Python bindings to advanced bindings, follow these steps:

1. **Upgrade World Usage**
   ```python
   # Basic
   world = SimpleWorld(config)
   
   # Advanced
   world = WorldAdvanced(world)
   
   # Add advanced features
   metrics = world.getWorldMetrics()
   world.executeTool("world_analyzer", {"world_id": world.getId()})
   ```

2. **Upgrade Configuration Usage**
   ```python
   # Basic
   config = Config(config_file)
   
   # Advanced
   config = ConfigAdvanced(config_object)
   
   # Add advanced features
   schema = config.getSchema()
   config.validate()
   ```

3. **Upgrade Agent Usage**
   ```python
   # Basic
   agent = AgentBrain(brain)
   
   # Advanced
   agent = AgentBrainAdvanced(brain)
   
   # Add advanced features
   cognitive_metrics = agent.getCognitiveMetrics()
   agent.executeTool("brain_analyzer", {"brain_id": brain.getBrainId()})
   ```

## Conclusion

The WorldAdvanced class provides comprehensive access to all world simulation systems through Python-friendly APIs. This documentation covers all major features including world metrics, environment metrics, performance metrics, simulation statistics, error handling, diagnostic APIs, and tool execution APIs.

By following the best practices outlined in this documentation, you can effectively use the WorldAdvanced class to build complex world simulation systems with high performance and reliability.

## See Also

- [NLM Python Bindings Documentation](https://nlm.org/docs/python-bindings) - General Python bindings documentation
- [BrainAdvanced Documentation](https://nlm.org/docs/brain-advanced) - BrainAdvanced documentation
- [AgentBrainAdvanced Documentation](https://nlm.org/docs/agent-advanced) - AgentBrainAdvanced documentation
- [ConfigAdvanced Documentation](https://nlm.org/docs/config-advanced) - ConfigAdvanced documentation
- [NLM Examples](https://nlm.org/examples) - Code examples and tutorials
- [NLM Community](https://nlm.org/community) - Community support and forums

---

*Last updated: 2025-01-01*  
*Version: 0.1.0-advanced*  
*Copyright: NLM Development Team*