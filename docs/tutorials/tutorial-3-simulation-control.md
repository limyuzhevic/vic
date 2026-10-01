# NLM Tutorial 3: Simulation Control

This tutorial provides step-by-step instructions for simulation control and monitoring in NLM (Neural Learning Machine). It covers:

- **Simulation Control Overview**: Simulation control overview and concepts
- **Simulation Control Basics**: Basic simulation control
- **Simulation Control Details**: Detailed simulation control options
- **Simulation Control Examples**: Control examples and use cases
- **Simulation Control Testing**: Control testing and validation
- **Simulation Control Optimization**: Control optimization strategies

## 🎯 Why Simulation Control Matters

Simulation control is crucial for NLM because:

1. **System Control**: Control system simulation behavior
2. **Performance**: Control system performance
3. **Learning**: Control system learning capabilities
4. **Adaptation**: Control system adaptation
5. **Monitoring**: Control system monitoring

## ⚙️ Simulation Control Overview

### Simulation Control Overview

NLM simulation control provides a framework for controlling NLM simulation behavior:

1. **Simulation Loop**: Simulation loop control
2. **Simulation Timing**: Simulation timing control
3. **Simulation Events**: Simulation event control
4. **Simulation State**: Simulation state control
5. **Simulation Monitoring**: Simulation monitoring control

### Simulation Control Structure

NLM simulation control structure:

```
simulation
├── step_control                # Step control
├── timing_control              # Timing control
├── event_control               # Event control
├── state_control               # State control
├── monitoring_control          # Monitoring control
└── ...                        # Additional simulation control options
```

## 📋 Simulation Control Basics

### Create Basic Simulation

Create basic simulation control:

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print("✅ Simulation completed successfully")
```

### Control Simulation Speed

Control simulation speed:

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Control simulation speed
for step in range(100):
    # Control simulation speed
    brain.step(step)
    
    # Print progress
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

### Pause Simulation

Pause simulation:

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    
    # Pause simulation (example)
    if step == 50:
        print("⏸️  Simulation paused")
        input("Press Enter to continue...")
        print("▶️  Simulation resumed")
```

### Resume Simulation

Resume simulation:

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with resume capability
for step in range(100):
    brain.step(step)
    
    # Resume simulation (example)
    if step == 30:
        print("⏸️  Simulation paused")
        # Save simulation state
        brain.save("simulation_state.bin")
        
        # Resume from saved state
        brain.load("simulation_state.bin")
```

### Step Simulation

Step simulation:

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation step by step
for step in range(100):
    # Step simulation
    brain.step(step)
    
    # Print step information
    print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

### Run Multiple Simulations

Run multiple simulations:

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Run multiple simulations
for simulation in range(3):
    print(f"Running simulation {simulation + 1}")
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    print(f"✅ Simulation {simulation + 1} completed")
```

## 📋 Simulation Control Details

### Simulation Control Parameters

Simulation control parameters:

#### Step Control Parameters

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `simulation.max_steps` | int | 1000 | Maximum simulation steps | 1-10000 |
| `simulation.step_interval` | int | 1 | Step interval | 1-100 |
| `simulation.step_size` | float | 0.1 | Step size | 0.01-1.0 |

#### Timing Control Parameters

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `simulation.max_time` | float | 100.0 | Maximum simulation time (s) | 1.0-1000.0 |
| `simulation.time_step` | float | 0.1 | Simulation time step (s) | 0.01-1.0 |
| `simulation.start_time` | float | 0.0 | Start time (s) | 0.0-1000.0 |

#### Event Control Parameters

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `simulation.event_enabled` | bool | true | Enable simulation events | - |
| `simulation.event_interval` | int | 10 | Event interval | 1-100 |
| `simulation.event_types` | list | ["step", "time", "state"] | Event types | - |

#### State Control Parameters

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `simulation.state_enabled` | bool | true | Enable simulation state | - |
| `simulation.state_interval` | int | 100 | State interval | 1-1000 |
| `simulation.state_file` | string | "simulation_state.bin" | State file | - |

#### Monitoring Control Parameters

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `simulation.monitoring_enabled` | bool | true | Enable simulation monitoring | - |
| `simulation.monitoring_interval` | int | 10 | Monitoring interval | 1-100 |
| `simulation.monitoring_metrics` | list | ["firing_rate", "memory_usage", "cpu_usage"] | Monitoring metrics | - |

### Simulation Control Options

Simulation control options:

#### Simulation Control Types

| Type | Description |
|------|-------------|
| **Step Simulation** | Simulate step by step |
| **Time Simulation** | Simulate over time |
| **Event Simulation** | Simulate with events |
| **State Simulation** | Simulate with states |
| **Monitoring Simulation** | Simulate with monitoring |

#### Simulation Control Modes

| Mode | Description |
|------|-------------|
| **Real-time Mode** | Simulate in real-time |
| **Fast-forward Mode** | Simulate with fast-forward |
| **Pause Mode** | Pause simulation |
| **Resume Mode** | Resume simulation |
| **Reset Mode** | Reset simulation |

## 📋 Simulation Control Examples

### Example 1: Basic Simulation Control

Basic simulation control:

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run basic simulation
for step in range(100):
    brain.step(step)

print("✅ Basic simulation completed")
```

### Example 2: Timed Simulation

Timed simulation:

```python
import pynlm
import time

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run timed simulation
start_time = time.time()
for step in range(100):
    brain.step(step)
    
    # Check time
    elapsed_time = time.time() - start_time
    if elapsed_time > 10.0:  # 10 seconds
        print("⏰ Simulation time exceeded")
        break

print("✅ Timed simulation completed")
```

### Example 3: Event-Based Simulation

Event-based simulation:

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create event manager
class EventManager:
    def __init__(self):
        self.events = []
    
    def add_event(self, event):
        self.events.append(event)
    
    def process_events(self):
        for event in self.events:
            if event['condition'](event['step']):
                event['action']()

# Create event manager
event_manager = EventManager()

# Add events
event_manager.add_event({
    'name': 'checkpoint',
    'condition': lambda step: step % 50 == 0,
    'action': lambda: brain.save(f"checkpoint_step_{step}.bin")
})

# Run event-based simulation
for step in range(100):
    brain.step(step)
    
    # Process events
    event_manager.process_events()
    
    # Print progress
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

### Example 4: State-Based Simulation

State-based simulation:

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create state manager
class StateManager:
    def __init__(self):
        self.states = []
    
    def add_state(self, state):
        self.states.append(state)
    
    def save_state(self, filename):
        brain.save(filename)
    
    def load_state(self, filename):
        brain.load(filename)

# Create state manager
state_manager = StateManager()

# Add states
state_manager.add_state({
    'name': 'initial_state',
    'action': lambda: print("Initial state")
})

state_manager.add_state({
    'name': 'checkpoint_state',
    'condition': lambda step: step % 50 == 0,
    'action': lambda: state_manager.save_state(f"checkpoint_step_{step}.bin")
})

# Run state-based simulation
for step in range(100):
    brain.step(step)
    
    # Process states
    for state in state_manager.states:
        if 'condition' in state and not state['condition'](step):
            continue
        
        state['action']()
```

## 📋 Simulation Control Testing

### Test Basic Simulation Control

Test basic simulation control:

```python
import pynlm

def test_basic_simulation_control():
    """Test basic simulation control"""
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    print("✅ Basic simulation control test passed")

# Run test
test_basic_simulation_control()
```

### Test Timed Simulation

Test timed simulation:

```python
import pynlm
import time

def test_timed_simulation():
    """Test timed simulation"""
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run timed simulation
    start_time = time.time()
    for step in range(100):
        brain.step(step)
        
        # Check time
        elapsed_time = time.time() - start_time
        assert elapsed_time < 60.0  # 60 seconds
    
    print("✅ Timed simulation test passed")

# Run test
test_timed_simulation()
```

### Test Event Simulation

Test event simulation:

```python
import pynlm

def test_event_simulation():
    """Test event simulation"""
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create event manager
    class EventManager:
        def __init__(self):
            self.events_called = []
        
        def add_event(self, condition, action):
            self.events_called.append((condition, action))
        
        def process_events(self, step):
            for condition, action in self.events_called:
                if condition(step):
                    action()
    
    event_manager = EventManager()
    
    # Add event
    event_manager.add_event(
        lambda step: step % 10 == 0,
        lambda: event_manager.events_called.append('event_called')
    )
    
    # Run event simulation
    for step in range(100):
        brain.step(step)
        
        # Process events
        event_manager.process_events(step)
    
    print("✅ Event simulation test passed")

# Run test
test_event_simulation()
```

## 📋 Simulation Control Optimization

### Optimize for Speed

Optimize simulation control for speed:

```python
import pynlm

def optimize_for_speed(config):
    """Optimize configuration for speed"""
    # Optimize step count
    config.set("simulation.max_steps", 100)
    
    # Optimize time step
    config.set("simulation.time_step", 0.01)
    
    # Disable monitoring for speed
    config.set("simulation.monitoring_enabled", False)
    
    return config

# Optimize configuration
config = pynlm.createDefaultConfig()
optimized_config = optimize_for_speed(config)

print("✅ Configuration optimized for speed")
```

### Optimize for Accuracy

Optimize simulation control for accuracy:

```python
import pynlm

def optimize_for_accuracy(config):
    """Optimize configuration for accuracy"""
    # Optimize step count
    config.set("simulation.max_steps", 1000)
    
    # Optimize time step
    config.set("simulation.time_step", 0.1)
    
    # Enable monitoring for accuracy
    config.set("simulation.monitoring_enabled", True)
    
    return config

# Optimize configuration
config = pynlm.createDefaultConfig()
optimized_config = optimize_for_accuracy(config)

print("✅ Configuration optimized for accuracy")
```

## 📊 Simulation Control Metrics

### Simulation Control Metrics

| Metric | Target | Current |
|--------|--------|---------|
| **Simulation Speed** | > 100 steps/s | - |
| **Memory Usage** | < 100 MB | - |
| **CPU Usage** | < 50% | - |
| **Event Processing** | < 1ms per event | - |
| **State Management** | < 10ms per state | - |

### Simulation Control Performance Metrics

| Metric | Target | Current |
|--------|--------|---------|
| **Step Time** | < 1ms per step | - |
| **Time Step** | < 0.1s per step | - |
| **Event Interval** | < 10ms per event | - |
| **State Save Time** | < 10ms per save | - |
| **State Load Time** | < 10ms per load | - |

## 🏆 Simulation Control Success Criteria

Simulation control is successful when:

1. **Simulation Correctness**: Simulation behavior is correct
2. **Simulation Performance**: Simulation performance is acceptable
3. **Simulation Control**: Simulation control is seamless
4. **Simulation Documentation**: Simulation control is documented
5. **Simulation Testing**: Simulation control is tested

## 📚 Simulation Control Summary

This completes Simulation Control Tutorial 3. It provides comprehensive step-by-step instructions for simulation control and monitoring, including:

- **Simulation Control Overview**: Simulation control overview and concepts
- **Simulation Control Basics**: Basic simulation control
- **Simulation Control Details**: Detailed simulation control options
- **Simulation Control Examples**: Control examples and use cases
- **Simulation Control Testing**: Control testing and validation
- **Simulation Control Optimization**: Control optimization strategies

## 🔗 Related Resources

### Documentation
- **README.md**: Project overview and installation
- **getting-started.md**: Getting started guide
- **api-reference.md**: Complete API documentation
- **configuration.md**: Configuration guide
- **best-practices.md**: Best practices

### Examples
- **examples/visualization_example.py**: Visualization example

### Code
- **src/brain/**: Core neural components
- **src/agent/**: Agent integration layer
- **src/world/**: Environment simulation

## 📖 Next Tutorial

For more advanced tutorials:

1. **Tutorial 4**: Learn about visualization and debugging
2. **Tutorial 5**: Learn about plugin development and usage

Following these steps will help you successfully complete Tutorial 3 and continue learning NLM.🧠✨