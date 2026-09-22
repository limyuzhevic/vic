# NLM User Guide

## Overview

NLM (熙然) is an experimental computational brain project that simulates biological neural systems using spiking neural network (SNN) technology. This guide provides practical examples and step-by-step instructions for using NLM effectively.

## Prerequisites

Before using NLM, you'll need:

- **Python** 3.8+ installed
- **C++ compiler** (GCC 10+, Clang 12+, or MSVC 2019+)
- **CMake** 3.16+ for building from source
- Basic understanding of Python programming

## Quick Installation and Setup

### Option 1: Install from PyPI (Recommended for most users)

```bash
pip install nlm-neural-learning-machine
```

### Option 2: Build from Source (Development/Users)

```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Create build directory and compile
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install Python bindings
pip install .
```

## Basic Usage: Creating Your First Brain

### Example 1: Minimal Working Brain

```python
import pynlm

# Create a brain with default settings
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize the brain (required before use)
brain.initialize()

# Run simulation for 100 time steps
for step in range(100):
    brain.step(step)

# Check how active the brain is
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Neurons currently firing: {brain.getFiringNeuronCount()}")
```

### Example 2: Brain with Sensory Input and Motor Output

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create a simple world (environment)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent that connects brain to world
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation loop
for step in range(200):
    # Update world (environment changes)
    world.update(0.1)
    
    # Get what the agent perceives (sensory input)
    percept = world.getSensoryPercept()
    
    # Tell brain what it perceives (sensory transduction)
    agent.processSensoryInput(percept)
    
    # Brain processes information and learns
    brain.step(step)
    
    # Brain decides what action to take
    action = agent.decodeMotorCommand()
    
    # Execute action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Provide reward signal for learning
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update brain development
    agent.updateDevelopment(0.1)
    
    # Print progress every 50 steps
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Agent position: {world.getAgentBody().getPosition()}")
```

## Advanced Usage: Configuring Your Brain

### Example 3: Custom Brain Configuration

```python
import pynlm

# Create and configure custom brain config
config = pynlm.createDefaultConfig()

# Modify configuration parameters
config.set("brain.neuron_count", 5000)  # More neurons
config.set("brain.synapse_density", 0.05)  # Fewer connections
config.set("brain.v_thresh", -45.0)  # Higher firing threshold
config.set("plasticity.stdp.enable", true)  # Enable STDP
config.set("plasticity.hebbian.enable", true)  # Enable Hebbian learning
config.set("neuromod.dopamine.scale", 0.5)  # Modulate dopamine effect
config.set("neuromod.curiosity.enable", true)  # Enable curiosity

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Save configuration for later use
config.saveToFile("my_brain_config.json")
```

### Example 4: Complex Agent with All Systems

```python
import pynlm

class AdvancedAgent:
    def __init__(self, brain_config=None):
        # Configure brain with optimal settings
        self.config = brain_config or pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 3000)
        self.config.set("working_memory_capacity", 500)
        self.config.set("prediction_horizon", 5)
        
        # Create all components
        self.brain = pynlm.createBrain(self.config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        
        # Configure world for complex tasks
        self.world.configure(
            width=30, height=30,
            visionWidth=16, visionHeight=16,
            visionAngle=180,
            maxEnergy=1000
        )
        
        # Enable all subsystems
        self._enable_all_systems()
        
        # Initialize components
        self.brain.initialize()
        self.world.reset()
        self.agent.initialize(self.world)
    
    def _enable_all_systems(self):
        """Enable all brain subsystems for maximum capability"""
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        self.agent.enablePrediction(True)
        self.agent.enableMemory(True)
        self.agent.enablePlanning(True)
        self.agent.enableSocialLearning(True)
    
    def run_episode(self, max_steps=1000):
        """Run a complete agent episode"""
        for step in range(max_steps):
            # World dynamics
            self.world.update(0.1)
            
            # Agent perception and action loop
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            
            # Apply action and get reward
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            reward = self._calculate_reward()
            predicted_reward = self.agent.getPredictionError()
            
            # Learning and development
            self.agent.applyRewardModulation(reward, predicted_reward)
            self.agent.updateDevelopment(0.1)
            
            # Check termination conditions
            if self._check_termination():
                break
        
        return self._get_episode_stats()
    
    def _calculate_reward(self):
        """Calculate reward based on agent's performance"""
        percept = self.world.getSensoryPercept()
        
        # Example: reward for reaching targets, avoiding hazards
        if percept.isObjectType(pynlm.WorldObjectType.Resource):
            return percept.getValue() * 10.0
        elif percept.isObjectType(pynlm.WorldObjectType.Hazard):
            return -100.0
        elif percept.isObjectType(pynlm.WorldObjectType.Wall):
            return -1.0
        
        return 0.1  # Small exploration bonus
    
    def _check_termination(self):
        """Check if episode should end"""
        # Episode ends if agent energy runs out
        if self.world.getAgentBody().getEnergy() <= 0:
            return True
        
        # Or if maximum steps reached (handled by loop)
        return False
    
    def _get_episode_stats(self):
        """Collect episode statistics"""
        return {
            'final_curiosity': self.agent.getCuriosityLevel(),
            'final_novelty': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'development_stage': self.agent.getDevelopmentalStage(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'final_position': self.world.getAgentBody().getPosition()
        }

# Usage example
advanced_agent = AdvancedAgent()
episode_stats = advanced_agent.run_episode(max_steps=500)
print("Episode completed with stats:", episode_stats)
```

## Development Workflow

### Building and Running Demos

```bash
# Build all targets
cd build
make -j$(nproc)

# Run Phase 6 integration demo
./nlm_phase6_demo

# Run Phase 4 cognitive experiments
./nlm_phase4_demo 100

# Run unit tests
./nlm_test
```

### Running Python Examples

```bash
# Navigate to your project directory
# Create examples.py with the code from this guide
# Run it
python examples.py

# For interactive development
python -c "import pynlm; brain = pynlm.createBrain(pynlm.createDefaultConfig()); brain.initialize(); print('Brain ready!')"
```

## Performance Optimization Tips

### Example 5: Optimized Simulation Setup

```python
import pynlm
import time

def optimized_simulation(config_overrides=None):
    """Optimized simulation setup for better performance"""
    config = pynlm.createDefaultConfig()
    
    # Apply optimizations based on task requirements
    if config_overrides:
        for key, value in config_overrides.items():
            config.set(key, value)
    
    # Create brain with performance settings
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Use agent for better performance than direct brain calls
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Performance monitoring
    start_time = time.time()
    steps_per_second = 0
    
    for step in range(1000):
        world.update(0.1)
        
        # Batch sensory processing for multiple steps
        if step % 10 == 0:
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
        
        # Periodic updates
        if step % 5 == 0:
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Track performance
        if step % 100 == 0:
            current_time = time.time()
            steps_per_second = 100 / (current_time - start_time)
            start_time = current_time
            print(f"Performance: {steps_per_second:.1f} steps/sec")
    
    return {
        'brain': brain,
        'agent': agent,
        'world': world,
        'performance': steps_per_second,
        'total_time': time.time() - start_time
    }

# Run optimized simulation
results = optimized_simulation({
    "brain.neuron_count": 2000,
    "working_memory_capacity": 200
})
print(f"Simulation completed with {results['performance']:.1f} steps/sec")
```

## Error Handling and Debugging

### Example 6: Robust Simulation with Error Handling

```python
import pynlm

def safe_simulation_run(max_steps=1000):
    """Simulation with comprehensive error handling"""
    try:
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Track simulation state
        simulation_errors = 0
        warnings = []
        
        for step in range(max_steps):
            try:
                # Validate brain state before step
                if brain.getFiringNeuronCount() > brain.getTotalNeuronCount():
                    warnings.append(f"Step {step}: Invalid neuron count")
                    continue
                
                # Run simulation with error checking
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Save checkpoint periodically
                if step % 100 == 0:
                    brain.save(f"checkpoint_{step}.bin")
                    
            except Exception as e:
                simulation_errors += 1
                warnings.append(f"Step {step} error: {str(e)}")
                
                # Attempt recovery
                if simulation_errors > 5:
                    print(f"Too many errors ({simulation_errors}), attempting recovery...")
                    brain.reset()
                    agent.reset()
                    world.reset()
                    brain.initialize()
                    agent.initialize(world)
                    simulation_errors = 0
        
        # Report results
        print(f"Simulation completed with {simulation_errors} errors")
        if warnings:
            print(f"Encountered {len(warnings)} warnings")
            for warning in warnings[:5]:  # Show first 5 warnings
                print(f"  - {warning}")
        
        return brain, agent, world
        
    except Exception as e:
        print(f"Critical error: {str(e)}")
        raise
    
    finally:
        # Cleanup
        print("Simulation resources cleaned up")

# Run safe simulation
brain, agent, world = safe_simulation_run(500)
```

## Common Patterns and Best Practices

### Pattern 1: Agent Lifecycle Management

```python
import pynlm

def create_agent_for_task(task_config):
    """Create agent configured for specific task"""
    config = pynlm.createDefaultConfig()
    
    # Task-specific configuration
    if task_config.get("learning_needed", True):
        config.set("plasticity.stdp.enable", true)
        config.set("plasticity.hebbian.enable", true)
    
    if task_config.get("memory_needed", True):
        config.set("working_memory_capacity", 500)
    
    if task_config.get("curiosity_needed", True):
        config.set("neuromod.curiosity.enable", true)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Enable features based on task
    for feature in task_config.get("features", []):
        if feature == "reward_modulation":
            agent.enableRewardModulation(True)
        elif feature == "structural_plasticity":
            agent.enableStructuralPlasticity(True)
        elif feature == "development":
            agent.enableDevelopment(True)
        elif feature == "curiosity":
            agent.enableCuriosity(True)
    
    return brain, agent

def run_task(brain, agent, task_spec, max_steps=1000):
    """Run a task with given agent"""
    world = pynlm.createSimpleWorld()
    world.configure(**task_spec.get("world_config", {}))
    world.reset()
    agent.initialize(world)
    
    for step in range(max_steps):
        # Execute task-specific behavior
        action = execute_task_logic(agent, step, task_spec)
        
        # Environment interaction
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update and check for task completion
        if is_task_complete(agent, world):
            break
    
    return brain, agent, world
```

### Pattern 2: State Management and Saving

```python
import pynlm
import json

class AgentStateManager:
    """Manage agent state across sessions"""
    
    def __init__(self, agent_name="default"):
        self.agent_name = agent_name
        self.checkpoint_dir = f"checkpoints/{agent_name}/"
        
        # Create checkpoint directory if it doesn't exist
        import os
        os.makedirs(self.checkpoint_dir, exist_ok=True)
    
    def save_state(self, brain, agent, world, episode):
        """Save agent state to disk"""
        # Save brain state
        brain.save(f"{self.checkpoint_dir}/brain_{episode}.bin")
        
        # Save agent statistics
        agent_stats = {
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'development_stage': agent.getDevelopmentalStage(),
            'neuromodulation_level': agent.getNeuromodulationLevel()
        }
        
        with open(f"{self.checkpoint_dir}/agent_stats_{episode}.json", 'w') as f:
            json.dump(agent_stats, f, indent=2)
        
        print(f"State saved at episode {episode}")
    
    def load_state(self, brain, agent, world, episode):
        """Load agent state from disk"""
        try:
            brain.load(f"{self.checkpoint_dir}/brain_{episode}.bin")
            
            # Load agent stats
            with open(f"{self.checkpoint_dir}/agent_stats_{episode}.json", 'r') as f:
                agent_stats = json.load(f)
            
            print(f"State loaded from episode {episode}")
            return True
            
        except Exception as e:
            print(f"Failed to load state: {e}")
            return False
    
    def save_config(self, config):
        """Save agent configuration"""
        config.saveToFile(f"{self.checkpoint_dir}/config.json")
    
    def load_config(self, config):
        """Load agent configuration"""
        config.loadFromFile(f"{self.checkpoint_dir}/config.json")

# Usage example
state_manager = AgentStateManager("my_agent")

# Later in simulation
if should_save_state:
    state_manager.save_state(brain, agent, world, current_episode)

if should_load_state:
    state_manager.load_state(brain, agent, world, saved_episode)
```

### Pattern 3: Performance Monitoring

```python
import pynlm
import time

class PerformanceMonitor:
    """Monitor and analyze simulation performance"""
    
    def __init__(self):
        self.metrics = {
            'steps': [],
            'firing_rates': [],
            'curiosity_levels': [],
            'prediction_errors': [],
            'times': []
        }
        self.start_time = None
    
    def start_monitoring(self):
        """Start performance monitoring"""
        self.start_time = time.time()
    
    def record_step(self, brain, agent, step_time):
        """Record metrics for a single step"""
        self.metrics['steps'].append(len(self.metrics['steps']))
        self.metrics['firing_rates'].append(brain.getFiringNeuronCount())
        self.metrics['curiosity_levels'].append(agent.getCuriosityLevel())
        self.metrics['prediction_errors'].append(agent.getPredictionError())
        self.metrics['times'].append(step_time)
    
    def get_summary(self):
        """Get performance summary"""
        if not self.metrics['steps']:
            return None
        
        total_time = self.metrics['times'][-1] if self.metrics['times'] else 0
        steps_per_second = len(self.metrics['steps']) / total_time if total_time > 0 else 0
        
        return {
            'total_steps': len(self.metrics['steps']),
            'total_time': total_time,
            'steps_per_second': steps_per_second,
            'avg_firing_rate': sum(self.metrics['firing_rates']) / len(self.metrics['firing_rates']),
            'avg_curiosity': sum(self.metrics['curiosity_levels']) / len(self.metrics['curiosity_levels']),
            'avg_prediction_error': sum(self.metrics['prediction_errors']) / len(self.metrics['prediction_errors'])
        }

# Usage in simulation
monitor = PerformanceMonitor()

# Start monitoring
monitor.start_monitoring()

# In simulation loop
for step in range(1000):
    step_start = time.time()
    
    # Run simulation step
    brain.step(step)
    # ... other simulation code ...
    
    step_time = time.time() - step_start
    monitor.record_step(brain, agent, step_time)

# Get performance summary
perf_summary = monitor.get_summary()
print(f"Performance: {perf_summary['steps_per_second']:.1f} steps/sec")
```

## Next Steps

After mastering the basics, explore:

1. **Advanced Configuration**: Modify neural parameters, plasticity rules, and neuromodulation
2. **Complex Tasks**: Design agents for specific challenges (navigation, learning, decision making)
3. **Experiment Framework**: Use NLM's built-in experiment system for scientific studies
4. **Performance Optimization**: Implement parallel processing, memory pools, and SIMD optimizations
5. **Custom Components**: Extend NLM with new neuron types, plasticity rules, or neuromodulators
6. **Phase 4 Experiments**: Test cognitive capabilities like prediction, planning, and social learning

This User Guide provides a solid foundation for working with NLM. For more detailed information, refer to the API Reference, Configuration Documentation, and Experiment Documentation sections.

For help with specific issues, check the Troubleshooting Guide or visit the NLM documentation for the latest updates and examples.
