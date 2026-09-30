# NLM - Enhanced Easy Usage Guide for Beginners

## Overview

This enhanced easy usage guide provides comprehensive examples and practical knowledge for using the NLM (Neural Learning Machine) Python API. It builds upon the basic concepts while introducing advanced patterns, error handling, and performance optimization techniques.

## Quick Navigation

- [Basic Concepts](#basic-concepts)
- [Essential Components](#essential-components)
- [Step-by-Step Examples](#step-by-step-examples)
- [Error Handling and Debugging](#error-handling-and-debugging)
- [Performance Optimization Tips](#performance-optimization-tips)
- [Advanced Configuration Options](#advanced-configuration-options)
- [Real-World Use Cases](#real-world-use-cases)

## Basic Concepts

### What is NLM?

NLM is a brain simulator that creates virtual brains capable of learning, memory, and decision-making. It models biological neurons and synapses to create artificial intelligence systems that learn from experience.

### Core Components

1. **Brain**: Virtual neural network with neurons and synapses
2. **World**: Environment where the brain operates
3. **Agent**: Interface between brain and world
4. **Sensory Input**: Information from the environment
5. **Motor Output**: Actions taken in the environment

## Essential Components

### Brain Creation and Initialization

```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize the brain
brain.initialize()

# Check brain status
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Brain initialized: {brain.isInitialized()}")
```

### World Setup

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=50,                    # World size
    height=50,
    visionWidth=10,             # Agent's visual field
    visionHeight=10,
    maxEnergy=100.0,            # Maximum energy
    timeStep=0.05              # Simulation timestep
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(25, 25)

# Check world status
print(f"World width: {world.getWidth()}")
print(f"World height: {world.getHeight()}")
print(f"Simulation time: {world.getSimulationTime():.2f}")
```

### Agent Creation

```python
import pynlm

# Create agent brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create agent
agent = pynlm.createAgentBrain(brain)

# Initialize agent with world
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableCuriosity(True)             # Drive exploration
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain maturation

print(f"Agent initialized: {agent.isInitialized()}")
print(f"Curiosity level: {agent.getCuriosityLevel():.2f}")
```

## Step-by-Step Examples

### Example 1: Silent Brain (Basic Neuron Activity)

```python
import pynlm
import time

def run_silent_brain_example():
    """Run a basic example showing neuron activity"""
    
    print("=== Silent Brain Example ===")
    
    # Create and initialize brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run simulation with increasing activity
    for step in range(100):
        # Brain processes one timestep
        brain.step(step)
        
        # Print periodic status
        if step % 25 == 0:
            firing_rate = brain.getAverageFiringRate()
            print(f"Step {step}: Neurons firing = {brain.getFiringNeuronCount()}, "
                  f"Firing rate = {firing_rate:.2f}")
    
    print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
    
    return brain

# Run the example
silent_brain = run_silent_brain_example()
```

### Example 2: Agent in a World (Full Integration)

```python
import pynlm
import random

def run_agent_world_example():
    """Run a complete example with agent in world"""
    
    print("\n=== Agent in World Example ===")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30,
        height=30,
        visionWidth=8,
        visionHeight=8,
        maxEnergy=100.0,
        timeStep=0.1
    )
    world.reset()
    
    # Create and initialize agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation loop
    for step in range(200):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input
        agent.processSensoryInput(percept)
        
        # 4. Brain computation
        brain.step(step)
        
        # 5. Decode action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Print periodic status
        if step % 50 == 0:
            body = world.getAgentBody()
            position = body.getPosition()
            energy = body.getEnergy()
            
            print(f"Step {step}:")
            print(f"  Position: {position}")
            print(f"  Energy: {energy:.1f}/{body.getMaxEnergy()}")
            print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.2f}")
            
            # Add some random resources for agent to find
            if step % 100 == 0:
                # Place resource in random location
                res_x = random.randint(0, world.getWidth() - 1)
                res_y = random.randint(0, world.getHeight() - 1)
                world.addObject(res_x, res_y, 
                              pynlm.WorldObjectType.Resource, value=10.0)
                print(f"  Resource placed at ({res_x}, {res_y})")
    
    print(f"\nSimulation complete!")
    print(f"Total steps: 200")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.2f}")
    
    return brain, world, agent

# Run the example
agent_brain, agent_world, agent_obj = run_agent_world_example()
```

### Example 3: Advanced Agent with Learning (Multi-Feature)

```python
import pynlm
import math

class AdvancedAgentExample:
    """Advanced agent example with multiple learning features"""
    
    def __init__(self):
        self.brain = None
        self.world = None
        self.agent = None
        self.step_count = 0
        self.performance_metrics = []
        
    def setup_advanced_agent(self):
        """Set up advanced agent with custom configuration"""
        
        # Create optimized configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000)
        config.set("learning.rate", 0.02)
        config.set("brain.excitability", 1.3)
        config.set("memory.working_memory_size", 300)
        config.set("neuromodulation.dopamine_strength", 1.2)
        config.set("neuromodulation.curiosity_strength", 1.5)
        
        # Create and initialize brain
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create and configure world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=40,
            height=40,
            visionWidth=12,
            visionHeight=12,
            maxEnergy=150.0,
            timeStep=0.03
        )
        self.world.reset()
        
        # Create and initialize agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable all advanced features
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        
        print("Advanced agent setup complete!")
        print(f"  Brain neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  World size: {self.world.getWidth()}x{self.world.getHeight()}")
        print(f"  Agent curiosity: {self.agent.getCuriosityLevel():.2f}")
        
    def run_advanced_simulation(self, steps):
        """Run advanced simulation with detailed monitoring"""
        
        for step in range(steps):
            # 1. Update world
            self.world.update(self.world.getSimulationTime() + 
                            self.world.getTimeStep())
            
            # 2. Get perception
            percept = self.world.getSensoryPercept()
            
            # 3. Process with agent
            self.agent.processSensoryInput(percept)
            
            # 4. Brain computation
            self.brain.step(step)
            
            # 5. Get action
            action = self.agent.decodeMotorCommand()
            
            # 6. Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # 7. Apply neuromodulation if enabled
            if self.agent.getRewardModulationEnabled():
                # Simple reward based on agent energy
                reward = self._calculate_reward(self.agent, self.world)
                predicted = self.agent.getPrediction()
                self.agent.applyRewardModulation(reward, predicted)
            
            # 8. Update development
            if self.agent.getDevelopmentEnabled():
                self.agent.updateDevelopment(step * self.world.getTimeStep())
            
            # 9. Record metrics
            self._record_metrics(step)
            
            # 10. Print progress
            if step % 100 == 0:
                self._print_progress(step)
        
        print(f"\nAdvanced simulation complete!")
        self._print_summary()
        
    def _calculate_reward(self, agent, world):
        """Calculate reward based on agent state"""
        
        # Energy-based reward
        energy = agent.getEnergy() if hasattr(agent, 'getEnergy') else 100.0
        energy_reward = (energy / 100.0) * 0.5
        
        # Curiosity-based reward
        curiosity = agent.getCuriosityLevel()
        curiosity_reward = curiosity * 0.3
        
        # Novelty-based reward
        novelty = agent.getNoveltyLevel()
        novelty_reward = novelty * 0.2
        
        return energy_reward + curiosity_reward + novelty_reward
        
    def _record_metrics(self, step):
        """Record simulation metrics"""
        
        metric = {
            'step': step,
            'firing_rate': self.brain.getAverageFiringRate(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'neuromodulation': self.agent.getNeuromodulationLevel()
        }
        
        self.performance_metrics.append(metric)
        
    def _print_progress(self, step):
        """Print simulation progress"""
        
        brain = self.brain
        agent = self.agent
        world = self.world
        
        print(f"Step {step}:")
        print(f"  Brain: {brain.getFiringNeuronCount()} neurons firing, "
              f"{brain.getAverageFiringRate():.2f} firing rate")
        print(f"  Agent: {agent.getCuriosityLevel():.2f} curiosity, "
              f"{agent.getNoveltyLevel():.2f} novelty")
        print(f"  World: Time {world.getSimulationTime():.2f}, "
              f"Position {world.getAgentPosition()}")
        
    def _print_summary(self):
        """Print simulation summary"""
        
        if not self.performance_metrics:
            return
            
        # Calculate statistics
        avg_firing_rate = sum(m['firing_rate'] for m in self.performance_metrics) / len(self.performance_metrics)
        max_firing_rate = max(m['firing_rate'] for m in self.performance_metrics)
        avg_curiosity = sum(m['curiosity'] for m in self.performance_metrics) / len(self.performance_metrics)
        
        print(f"\n=== Advanced Simulation Summary ===")
        print(f"Total steps: {len(self.performance_metrics)}")
        print(f"Average firing rate: {avg_firing_rate:.2f}")
        print(f"Maximum firing rate: {max_firing_rate:.2f}")
        print(f"Average curiosity level: {avg_curiosity:.2f}")
        print(f"Final developmental stage: {self.brain.getDevelopmentalStage()}")
        
        # Performance analysis
        self._analyze_performance()
        
    def _analyze_performance(self):
        """Analyze simulation performance"""
        
        if len(self.performance_metrics) < 2:
            return
            
        # Check for performance trends
        first_half = self.performance_metrics[:len(self.performance_metrics)//2]
        second_half = self.performance_metrics[len(self.performance_metrics)//2:]
        
        avg_firing_first = sum(m['firing_rate'] for m in first_half) / len(first_half)
        avg_firing_second = sum(m['firing_rate'] for m in second_half) / len(second_half)
        
        if avg_firing_second > avg_firing_first * 1.2:
            print(f"Performance trend: Increasing activity ({avg_firing_first:.2f} -> {avg_firing_second:.2f})")
        elif avg_firing_second < avg_firing_first * 0.8:
            print(f"Performance trend: Decreasing activity ({avg_firing_first:.2f} -> {avg_firing_second:.2f})")
        else:
            print(f"Performance trend: Stable activity ({avg_firing_first:.2f})")
            
        # Check for stability
        firing_variance = sum((m['firing_rate'] - avg_firing_rate)**2 for m in self.performance_metrics) / len(self.performance_metrics)
        firing_std = math.sqrt(firing_variance)
        
        if firing_std < 0.1:
            print(f"Firing rate stability: Excellent (std dev: {firing_std:.3f})")
        elif firing_std < 0.3:
            print(f"Firing rate stability: Good (std dev: {firing_std:.3f})")
        else:
            print(f"Firing rate stability: Variable (std dev: {firing_std:.3f})")

# Run advanced agent example
advanced_agent = AdvancedAgentExample()
advanced_agent.setup_advanced_agent()
advanced_agent.run_advanced_simulation(500)
```

### Example 4: Error Handling and Robust Simulation

```python
import pynlm
import time

class RobustSimulationExample:
    """Example showing robust error handling and simulation"""
    
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = None
        self.world = None
        self.agent = None
        self.checkpoints = []
        
    def safe_simulation_run(self, steps, checkpoint_interval=100):
        """Run simulation with comprehensive error handling"""
        
        try:
            # Initialize components
            print("Initializing simulation components...")
            self._initialize_components()
            
            # Run simulation with error handling
            for step in range(steps):
                try:
                    # Run single simulation step
                    self._run_step(step)
                    
                    # Save checkpoint periodically
                    if step % checkpoint_interval == 0:
                        self._save_checkpoint(step)
                        
                except KeyboardInterrupt:
                    print(f"\nSimulation interrupted by user at step {step}")
                    self._handle_interrupt(step)
                    break
                    
                except pynlm.RuntimeError as e:
                    print(f"Runtime error at step {step}: {e}")
                    self._handle_runtime_error(step, e)
                    
                except MemoryError as e:
                    print(f"Memory error at step {step}: {e}")
                    self._handle_memory_error(step, e)
                    
                except Exception as e:
                    print(f"Unexpected error at step {step}: {e}")
                    self._handle_general_error(step, e)
                    
            print(f"\nSimulation completed successfully!")
            
        except Exception as e:
            print(f"Fatal error during initialization: {e}")
            self._handle_fatal_error(e)
            raise
            
    def _initialize_components(self):
        """Initialize simulation components with error checking"""
        
        print("Creating brain...")
        self.brain = pynlm.createBrain(self.config)
        if not self.brain.isInitialized():
            self.brain.initialize()
        print(f"  Created brain with {self.brain.getTotalNeuronCount()} neurons")
        
        print("Creating world...")
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        self.world.reset()
        print(f"  Created world {self.world.getWidth()}x{self.world.getHeight()}")
        
        print("Creating agent...")
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        print("  Agent initialized successfully")
        
        # Enable all features
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
    def _run_step(self, step):
        """Run single simulation step with error checking"""
        
        # Performance monitoring
        step_start = time.time()
        
        try:
            # Update world
            self.world.update(0.1)
            
            # Get perception
            percept = self.world.getSensoryPercept()
            if percept is None:
                print(f"Step {step}: Warning - No perception available")
                
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(step)
            
            # Check for brain performance issues
            if self.brain.isOverloaded():
                print(f"Step {step}: Warning - Brain performance degraded")
                
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Performance monitoring
            step_time = time.time() - step_start
            
            # Log performance
            if step % 50 == 0:
                print(f"Step {step}: {step_time:.4f}s, "
                      f"Firing: {self.brain.getFiringNeuronCount()}, "
                      f"Energy: {self.agent.getEnergy() if hasattr(self.agent, 'getEnergy') else 100:.1f}")
                      
        except RuntimeError as e:
            raise
        except Exception as e:
            raise RuntimeError(f"Simulation step failed: {e}")
            
    def _save_checkpoint(self, step):
        """Save simulation checkpoint"""
        
        checkpoint_data = {
            'step': step,
            'brain_state': self._get_brain_state(),
            'world_state': self._get_world_state(),
            'agent_state': self._get_agent_state(),
            'timestamp': time.time()
        }
        
        self.checkpoints.append(checkpoint_data)
        print(f"  Checkpoint saved at step {step}")
        
    def _get_brain_state(self):
        """Get brain state for checkpoint"""
        
        return {
            'neuron_count': self.brain.getTotalNeuronCount(),
            'firing_count': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'developmental_stage': self.brain.getDevelopmentalStage()
        }
        
    def _get_world_state(self):
        """Get world state for checkpoint"""
        
        return {
            'width': self.world.getWidth(),
            'height': self.world.getHeight(),
            'simulation_time': self.world.getSimulationTime(),
            'agent_position': self.world.getAgentPosition(),
            'agent_energy': self.world.getAgentEnergy()
        }
        
    def _get_agent_state(self):
        """Get agent state for checkpoint"""
        
        return {
            'curiosity_level': self.agent.getCuriosityLevel(),
            'novelty_level': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'neuromodulation_level': self.agent.getNeuromodulationLevel()
        }
        
    def _handle_interrupt(self, step):
        """Handle user interrupt"""
        
        print(f"\nInterrupted at step {step}")
        
        # Save final checkpoint
        if self.checkpoints:
            self._save_checkpoint(step)
            print("Final checkpoint saved")
            
    def _handle_runtime_error(self, step, error):
        """Handle runtime error"""
        
        print(f"Runtime error at step {step}: {error}")
        
        # Try to continue
        try:
            # Reset world if needed
            self.world.reset()
            
            # Reinitialize agent
            self.agent.initialize(self.world)
            
            print("Recovered from runtime error")
            return True
            
        except Exception as recovery_error:
            print(f"Failed to recover: {recovery_error}")
            return False
            
    def _handle_memory_error(self, step, error):
        """Handle memory error"""
        
        print(f"Memory error at step {step}: {error}")
        
        # Try to free memory
        import gc
        gc.collect()
        
        # Reduce simulation size
        print("Attempting to reduce simulation size...")
        return True
        
    def _handle_general_error(self, step, error):
        """Handle general error"""
        
        print(f"General error at step {step}: {error}")
        
        # Log error for debugging
        with open(f"simulation_error_step_{step}.log", 'w') as f:
            f.write(f"Step: {step}\n")
            f.write(f"Error: {error}\n")
            f.write(f"Time: {time.time()}\n")
            
        # Try to continue
        return True
        
    def _handle_fatal_error(self, error):
        """Handle fatal error during initialization"""
        
        print(f"Fatal error during initialization: {error}")
        print("Cleaning up...")
        
        # Clean up
        if self.agent:
            del self.agent
        if self.brain:
            del self.brain
        if self.world:
            del self.world
            
        # Save error log
        with open("fatal_error.log", 'w') as f:
            f.write(f"Fatal error: {error}\n")
            f.write(f"Time: {time.time()}\n")

# Run robust simulation example
robust_sim = RobustSimulationExample()
robust_sim.safe_simulation_run(steps=500, checkpoint_interval=100)
```

### Example 5: Performance Monitoring and Optimization

```python
import pynlm
import time
import psutil
import statistics

class PerformanceMonitoringExample:
    """Example showing performance monitoring and optimization"""
    
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = None
        self.world = None
        self.agent = None
        self.performance_data = []
        self.memory_samples = []
        self.cpu_samples = []
        
    def run_monitored_simulation(self, steps):
        """Run simulation with comprehensive performance monitoring"""
        
        print("Starting performance-monitored simulation...")
        
        # Initialize components
        self._initialize_monitoring_components()
        
        # Start system monitoring
        process = psutil.Process()
        start_time = time.time()
        start_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Run simulation with detailed monitoring
        for step in range(steps):
            step_start = time.time()
            step_memory_start = process.memory_info().rss / 1024 / 1024
            
            # Record CPU usage
            cpu_percent = process.cpu_percent()
            self.cpu_samples.append(cpu_percent)
            
            try:
                # Run simulation step
                self._run_monitoring_step(step)
                
            except Exception as e:
                print(f"Error at step {step}: {e}")
                raise
                
            # Record performance metrics
            step_time = time.time() - step_start
            step_memory_end = process.memory_info().rss / 1024 / 1024
            
            self.performance_data.append({
                'step': step,
                'time': step_time,
                'memory_start': step_memory_start,
                'memory_end': step_memory_end,
                'memory_change': step_memory_end - step_memory_start,
                'cpu_percent': cpu_percent,
                'neuron_count': self.brain.getTotalNeuronCount(),
                'firing_count': self.brain.getFiringNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'firing_rate': self.brain.getAverageFiringRate()
            })
            
            # Print performance summary
            if step % 100 == 0:
                self._print_performance_summary(step, process)
                
            # Check for performance issues
            self._check_performance_issues(step)
        
        # Calculate final statistics
        end_time = time.time()
        end_memory = process.memory_info().rss / 1024 / 1024
        
        total_time = end_time - start_time
        memory_growth = end_memory - start_memory
        
        print(f"\n=== Performance Monitoring Summary ===")
        print(f"Total simulation time: {total_time:.2f}s")
        print(f"Average step time: {total_time/steps:.6f}s")
        print(f"Memory growth: {memory_growth:.2f} MB")
        print(f"Peak memory: {max(self.memory_samples):.2f} MB")
        print(f"Average CPU: {statistics.mean(self.cpu_samples):.1f}%")
        print(f"Max CPU: {max(self.cpu_samples):.1f}%")
        
        # Generate performance report
        self._generate_performance_report(total_time, memory_growth, steps)
        
    def _initialize_monitoring_components(self):
        """Initialize simulation components with monitoring"""
        
        print("Initializing simulation with monitoring...")
        
        # Configure for performance
        self.config.set("brain.neuron_count", 1000)
        self.config.set("simulation.time_step", 0.05)
        self.config.set("brain.excitability", 1.2)
        
        # Create components
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=40, height=40, visionWidth=10, visionHeight=10)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable features
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        print(f"  Brain: {self.brain.getTotalNeuronCount()} neurons")
        print(f"  World: {self.world.getWidth()}x{self.world.getHeight()}")
        print(f"  Agent: {self.agent.getCuriosityLevel():.2f} curiosity")
        
    def _run_monitoring_step(self, step):
        """Run single simulation step with monitoring"""
        
        # Update world
        self.world.update(0.1)
        
        # Get perception
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        
        # Brain computation
        self.brain.step(step)
        
        # Get action
        action = self.agent.decodeMotorCommand()
        
        # Apply action
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
    def _print_performance_summary(self, step, process):
        """Print performance summary"""
        
        current_data = self.performance_data[-1]
        
        print(f"\nStep {step}:")
        print(f"  Time: {current_data['time']:.4f}s")
        print(f"  Memory change: {current_data['memory_change']:+.2f} MB")
        print(f"  CPU: {current_data['cpu_percent']:.1f}%")
        print(f"  Firing rate: {current_data['firing_rate']:.2f}")
        print(f"  Total spikes: {current_data['total_spikes']}")
        
    def _check_performance_issues(self, step):
        """Check for performance issues"""
        
        if not self.performance_data:
            return
            
        current = self.performance_data[-1]
        
        # Check for slow steps
        if current['time'] > 0.2:  # > 200ms
            print(f"    WARNING: Slow step at {step}: {current['time']:.4f}s")
            
        # Check for memory issues
        if current['memory_change'] > 20:  # > 20MB increase
            print(f"    WARNING: Large memory increase at {step}: {current['memory_change']:.2f} MB")
            
        # Check for CPU issues
        if current['cpu_percent'] > 90:  # > 90% CPU
            print(f"    WARNING: High CPU usage at {step}: {current['cpu_percent']:.1f}%")
            
        # Check for firing rate issues
        if current['firing_rate'] > 100:  # > 100 Hz
            print(f"    WARNING: High firing rate at {step}: {current['firing_rate']:.2f} Hz")
            
    def _generate_performance_report(self, total_time, memory_growth, steps):
        """Generate comprehensive performance report"""
        
        print(f"\n=== Performance Analysis ===")
        
        if not self.performance_data:
            return
            
        # Time analysis
        avg_step_time = statistics.mean(d['time'] for d in self.performance_data)
        min_step_time = min(d['time'] for d in self.performance_data)
        max_step_time = max(d['time'] for d in self.performance_data)
        time_std = statistics.stdev(d['time'] for d in self.performance_data)
        
        print(f"\nTime Performance:")
        print(f"  Average step time: {avg_step_time:.6f}s")
        print(f"  Minimum step time: {min_step_time:.6f}s")
        print(f"  Maximum step time: {max_step_time:.6f}s")
        print(f"  Standard deviation: {time_std:.6f}s")
        print(f"  Performance consistency: {1 - (time_std / avg_step_time):.2%}")
        
        # Memory analysis
        memory_changes = [d['memory_change'] for d in self.performance_data]
        avg_memory_change = statistics.mean(memory_changes)
        max_memory_change = max(memory_changes)
        min_memory_change = min(memory_changes)
        
        print(f"\nMemory Performance:")
        print(f"  Average memory change: {avg_memory_change:.2f} MB")
        print(f"  Maximum memory increase: {max_memory_change:.2f} MB")
        print(f"  Maximum memory decrease: {min_memory_change:.2f} MB")
        
        # CPU analysis
        avg_cpu = statistics.mean(d['cpu_percent'] for d in self.performance_data)
        max_cpu = max(d['cpu_percent'] for d in self.performance_data)
        
        print(f"\nCPU Performance:")
        print(f"  Average CPU usage: {avg_cpu:.1f}%")
        print(f"  Maximum CPU usage: {max_cpu:.1f}%")
        
        # Neural activity analysis
        firing_rates = [d['firing_rate'] for d in self.performance_data]
        avg_firing_rate = statistics.mean(firing_rates)
        max_firing_rate = max(firing_rates)
        min_firing_rate = min(firing_rates)
        
        print(f"\nNeural Activity:")
        print(f"  Average firing rate: {avg_firing_rate:.2f} Hz")
        print(f"  Maximum firing rate: {max_firing_rate:.2f} Hz")
        print(f"  Minimum firing rate: {min_firing_rate:.2f} Hz")
        
        # Simulation efficiency
        steps_per_second = steps / total_time
        print(f"\nSimulation Efficiency:")
        print(f"  Steps per second: {steps_per_second:.1f}")
        print(f"  Seconds per 1000 steps: {1000/steps_per_second:.2f}")
        
        # Recommendations
        print(f"\nPerformance Recommendations:")
        if avg_step_time > 0.1:
            print(f"  - Consider increasing simulation timestep for speed")
            
        if max_memory_change > 50:
            print(f"  - Consider memory optimization for large simulations")
            
        if max_cpu > 80:
            print(f"  - Consider parallel processing for CPU-bound workloads")
            
        if max_firing_rate > 150:
            print(f"  - Consider reducing neuron count for stable firing rates")
            
        if time_std / avg_step_time > 0.2:
            print(f"  - Performance is inconsistent, consider optimization")
            
        print(f"\nPerformance monitoring complete!")

# Run performance monitoring example
perf_monitor = PerformanceMonitoringExample()
perf_monitor.run_monitored_simulation(500)
```

## Error Handling and Debugging

### Common Errors and Solutions

#### 1. Brain Initialization Errors

**Problem:** Brain fails to initialize properly.

**Solution:**

```python
def safe_brain_initialization():
    try:
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        print("Brain initialized successfully")
        return brain
    except RuntimeError as e:
        print(f"Brain initialization failed: {e}")
        
        # Try alternative configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)  # Reduce neuron count
        config.set("simulation.time_step", 0.1)  # Increase timestep
        
        try:
            brain = pynlm.createBrain(config)
            brain.initialize()
            print("Brain initialized with alternative configuration")
            return brain
        except Exception as e2:
            print(f"Alternative configuration failed: {e2}")
            raise
```

#### 2. World Configuration Errors

**Problem:** World configuration fails.

**Solution:**

```python
def safe_world_configuration(width=50, height=50):
    try:
        world = pynlm.createSimpleWorld()
        world.configure(
            width=width,
            height=height,
            visionWidth=min(10, width//5),
            visionHeight=min(10, height//5),
            maxEnergy=100.0,
            timeStep=0.1
        )
        world.reset()
        print(f"World configured successfully: {width}x{height}")
        return world
    except RuntimeError as e:
        print(f"World configuration failed: {e}")
        
        # Try default configuration
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20)
        world.reset()
        print("World configured with default settings")
        return world
```

#### 3. Agent Initialization Errors

**Problem:** Agent fails to initialize.

**Solution:**

```python
def safe_agent_initialization(brain, world):
    try:
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        print("Agent initialized successfully")
        return agent
    except RuntimeError as e:
        print(f"Agent initialization failed: {e}")
        
        # Try with minimal configuration
        try:
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Disable advanced features
            agent.enableRewardModulation(False)
            agent.enableCuriosity(False)
            
            print("Agent initialized with minimal configuration")
            return agent
        except Exception as e2:
            print(f"Minimal configuration failed: {e2}")
            raise
```

### Debugging Techniques

#### 1. Step-by-Step Debugging

```python
def debug_simulation_step_by_step(brain, world, agent, max_steps=10):
    """Debug simulation step by step"""
    
    for step in range(max_steps):
        print(f"\n=== Step {step} ===")
        
        # 1. Update world
        print("Updating world...")
        world.update(0.1)
        print(f"  World time: {world.getSimulationTime():.2f}")
        
        # 2. Get perception
        print("Getting sensory perception...")
        percept = world.getSensoryPercept()
        if percept is None:
            print("  Warning: No perception available")
        else:
            print(f"  Perception received: {type(percept).__name__}")
        
        # 3. Process sensory input
        print("Processing sensory input...")
        agent.processSensoryInput(percept)
        print(f"  Curiosity level: {agent.getCuriosityLevel():.2f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.2f}")
        
        # 4. Brain computation
        print("Running brain computation...")
        brain.step(step)
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
        
        # 5. Get action
        print("Decoding motor command...")
        action = agent.decodeMotorCommand()
        print(f"  Action: {action}")
        
        # 6. Apply action
        print("Applying motor command...")
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Check agent state
        print("Checking agent state...")
        body = world.getAgentBody()
        print(f"  Agent position: {body.getPosition()}")
        print(f"  Agent energy: {body.getEnergy()}")
        
    print(f"\n=== Debug Complete ===")
```

#### 2. State Inspection

```python
def inspect_simulation_state(brain, world, agent):
    """Inspect simulation state for debugging"""
    
    print("\n=== Simulation State Inspection ===")
    
    # Brain state
    print("\n--- Brain State ---")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Is initialized: {brain.isInitialized()}")
    
    # Memory state
    print("\n--- Memory State ---")
    working_memory = brain.getWorkingMemory()
    print(f"Working memory size: {working_memory.getSize()}")
    print(f"Working memory capacity: {working_memory.getCapacity()}")
    print(f"Working memory full: {working_memory.isFull()}")
    
    # Agent state
    print("\n--- Agent State ---")
    print(f"Curiosity level: {agent.getCuriosityLevel():.2f}")
    print(f"Novelty level: {agent.getNoveltyLevel():.2f}")
    print(f"Prediction error: {agent.getPredictionError():.2f}")
    print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.2f}")
    
    # World state
    print("\n--- World State ---")
    print(f"World dimensions: {world.getWidth()} x {world.getHeight()}")
    print(f"Simulation time: {world.getSimulationTime():.2f}")
    print(f"Agent position: {world.getAgentPosition()}")
    print(f"Max energy: {world.getMaxEnergy():.1f}")
    
    # Configuration state
    print("\n--- Configuration State ---")
    config = brain.getConfig()
    config_keys = config.getKeys()
    for key in config_keys:
        if key.startswith("brain.") or key.startswith("learning."):
            print(f"{key}: {config.get(key)}")
```

### Performance Optimization Tips

#### 1. Memory Optimization

```python
import pynlm
import gc

def optimize_memory_usage():
    """Optimize memory usage for long simulations"""
    
    # Use memory-efficient configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)  # Reduce neuron count
    config.set("memory.working_memory_size", 100)  # Limit working memory
    config.set("memory.episodic_capacity", 500)  # Limit episodic memory
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Force garbage collection
    gc.collect()
    
    # Monitor memory usage
    import psutil
    import os
    process = psutil.Process(os.getpid())
    
    print(f"Initial memory: {process.memory_info().rss / 1024 / 1024:.2f} MB")
    
    for step in range(1000):
        brain.step(step)
        
        if step % 100 == 0:
            memory = process.memory_info().rss / 1024 / 1024
            print(f"Step {step}: Memory = {memory:.2f} MB")
            
            # Force cleanup
            if step % 300 == 0:
                gc.collect()
                print(f"  Forced garbage collection")
                
    print(f"Final memory: {process.memory_info().rss / 1024 / 1024:.2f} MB")
```

#### 2. Time Optimization

```python
import pynlm
import time

def optimize_simulation_speed():
    """Optimize simulation for speed"""
    
    # Use speed-optimized configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("simulation.time_step", 0.05)  # Smaller timestep
    config.set("brain.excitability", 1.0)    # Standard excitability
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Monitor performance
    times = []
    
    for step in range(1000):
        step_start = time.time()
        
        brain.step(step)
        
        step_time = time.time() - step_start
        times.append(step_time)
        
        # Check performance
        if step % 100 == 0:
            avg_time = sum(times[-100:]) / len(times[-100:])
            print(f"Step {step}: Avg time = {avg_time:.4f}s, "
                  f"Speed = {1/avg_time:.1f} steps/sec")
            
            # Optimize if needed
            if avg_time > 0.1:
                print(f"  Optimizing configuration...")
                config.set("brain.neuron_count", 500)  # Reduce neurons
                # Recreate brain with new config
                brain = pynlm.createBrain(config)
                brain.initialize()
                
    # Calculate final performance
    avg_time = sum(times) / len(times)
    min_time = min(times)
    max_time = max(times)
    
    print(f"\nPerformance Summary:")
    print(f"  Average step time: {avg_time:.6f}s")
    print(f"  Minimum step time: {min_time:.6f}s")
    print(f"  Maximum step time: {max_time:.6f}s")
    print(f"  Steps per second: {1/avg_time:.1f}")
```

## Real-World Use Cases

### Use Case 1: Robotic Control

```python
import pynlm
import numpy as np

class RoboticController:
    """Robotic controller using NLM"""
    
    def __init__(self):
        # Create neural controller
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1500)
        config.set("learning.rate", 0.02)
        config.set("brain.excitability", 1.5)
        
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create environment
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=100,
            height=100,
            visionWidth=15,
            visionHeight=15,
            maxEnergy=200.0
        )
        self.world.reset()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable all features
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
    def run_control_loop(self, steps):
        """Run control loop for robot"""
        
        for step in range(steps):
            # Get sensor data (vision, touch, etc.)
            sensors = self._get_sensor_data()
            
            # Convert sensor data to perception
            percept = self._sensors_to_perception(sensors)
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Generate motor command
            action = self.agent.decodeMotorCommand()
            
            # Convert action to robot commands
            robot_command = self._action_to_robot_command(action)
            
            # Execute command
            self._execute_robot_command(robot_command)
            
            # Update environment
            self.world.update(0.1)
            
            # Update robot state based on action
            self._update_robot_state(action)
            
            # Provide reward
            reward = self._calculate_control_reward()
            self.agent.applyRewardModulation(reward, self.agent.getPrediction())
            
    def _get_sensor_data(self):
        """Get sensor data from robot"""
        
        # Simulate sensor readings
        vision = np.random.random((15, 15)) * 0.5  # Simulated vision
        touch = np.random.random(6) * 0.3  # Simulated touch
        proximity = np.random.random(4) * 0.8  # Simulated proximity
        
        return {
            'vision': vision,
            'touch': touch,
            'proximity': proximity,
            'battery': np.random.random() * 100
        }
        
    def _sensors_to_perception(self, sensors):
        """Convert sensor data to NLM perception"""
        
        # Create perception object
        percept = {
            'vision_data': sensors['vision'],
            'touch_data': sensors['touch'],
            'proximity_data': sensors['proximity'],
            'battery_level': sensors['battery'],
            'timestamp': time.time()
        }
        
        return percept
        
    def _action_to_robot_command(self, action):
        """Convert NLM action to robot command"""
        
        # Map NLM action to robot commands
        if action.type == 'move_forward':
            return {'type': 'motor_control', 'command': 'forward', 'speed': 1.0}
        elif action.type == 'turn_left':
            return {'type': 'motor_control', 'command': 'turn_left', 'angle': 30}
        elif action.type == 'turn_right':
            return {'type': 'motor_control', 'command': 'turn_right', 'angle': 30}
        elif action.type == 'stop':
            return {'type': 'motor_control', 'command': 'stop', 'speed': 0.0}
        elif action.type == 'grasp':
            return {'type': 'gripper_control', 'command': 'close', 'force': 50}
        elif action.type == 'release':
            return {'type': 'gripper_control', 'command': 'open', 'force': 0}
        else:
            return {'type': 'idle', 'command': 'hold'}
            
    def _execute_robot_command(self, command):
        """Execute robot command"""
        
        # Simulate robot command execution
        print(f"Executing robot command: {command}")
        
        # In real implementation, this would control robot motors
        # For simulation, we just track the command
        
    def _update_robot_state(self, action):
        """Update robot state based on action"""
        
        # Simulate state updates
        # This would include battery consumption, wear, etc.
        
    def _calculate_control_reward(self):
        """Calculate reward for control task"""
        
        # Simple reward function
        reward = 0.0
        
        # Positive reward for successful actions
        if hasattr(self, 'task_completed') and self.task_completed:
            reward += 10.0
            
        # Negative reward for failures
        if hasattr(self, 'task_failed') and self.task_failed:
            reward -= 5.0
            
        # Negative reward for battery consumption
        battery = self._get_robot_battery()
        reward -= (100 - battery) * 0.1
        
        return reward
        
    def _get_robot_battery(self):
        """Get robot battery level"""
        return 100.0  # Placeholder

# Use robotic controller
controller = RoboticController()
controller.run_control_loop(500)
```

### Use Case 2: Game AI

```python
import pynlm
import random

class GameAI:
    """Game AI using NLM"""
    
    def __init__(self):
        # Create AI brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000)
        config.set("learning.rate", 0.03)
        config.set("brain.excitability", 1.2)
        
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create game world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=80,
            height=60,
            visionWidth=12,
            visionHeight=10,
            maxEnergy=100.0
        )
        self.world.reset()
        
        # Create game agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable learning features
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        # Initialize game state
        self.game_state = self._initialize_game_state()
        
    def _initialize_game_state(self):
        """Initialize game state"""
        
        return {
            'player_position': (40, 30),
            'player_score': 0,
            'enemies': [],
            'power_ups': [],
            'level': 1,
            'game_over': False,
            'current_action': None
        }
        
    def run_game_simulation(self, steps):
        """Run game simulation"""
        
        for step in range(steps):
            if self.game_state['game_over']:
                print("Game over!")
                break
                
            # Update game world
            self._update_game_world(step)
            
            # Get AI perception
            percept = self._get_ai_perception()
            
            # Process perception
            self.agent.processSensoryInput(percept)
            
            # Generate AI action
            action = self.agent.decodeMotorCommand()
            
            # Apply AI action
            self._apply_ai_action(action)
            
            # Update game state
            self._update_game_state(action)
            
            # Calculate game reward
            reward = self._calculate_game_reward()
            self.agent.applyRewardModulation(reward, self.agent.getPrediction())
            
            # Print game status
            if step % 50 == 0:
                self._print_game_status(step)
                
    def _update_game_world(self, step):
        """Update game world state"""
        
        # Update world simulation
        self.world.update(0.1)
        
        # Generate new obstacles/enemies
        if step % 100 == 0:
            self._generate_new_content()
            
    def _get_ai_perception(self):
        """Get AI perception of game state"""
        
        # Convert game state to perception
        vision_data = self._get_vision_data()
        audio_data = self._get_audio_data()
        game_state_data = self._get_game_state_data()
        
        percept = {
            'vision': vision_data,
            'audio': audio_data,
            'game_state': game_state_data,
            'player_stats': self._get_player_stats()
        }
        
        return percept
        
    def _get_vision_data(self):
        """Get vision data for AI"""
        
        # Simulate AI vision
        vision = []
        
        for x in range(-6, 7):
            for y in range(-5, 6):
                world_x = self.game_state['player_position'][0] + x
                world_y = self.game_state['player_position'][1] + y
                
                if 0 <= world_x < self.world.getWidth() and 0 <= world_y < self.world.getHeight():
                    # Check what's at this position
                    cell_type = self._get_cell_type(world_x, world_y)
                    vision.append({
                        'position': (world_x, world_y),
                        'type': cell_type,
                        'distance': (x**2 + y**2)**0.5
                    })
                    
        return vision
        
    def _get_audio_data(self):
        """Get audio data for AI"""
        
        # Simulate audio cues
        audio = []
        
        # Add random audio events
        if random.random() < 0.1:
            audio.append({
                'type': 'enemy_sighted',
                'location': self._find_nearest_enemy(),
                'distance': random.uniform(10, 50)
            })
            
        if random.random() < 0.05:
            audio.append({
                'type': 'power_up_found',
                'type': random.choice(['health', 'ammo', 'speed']),
                'location': self._find_nearest_power_up()
            })
            
        return audio
        
    def _get_game_state_data(self):
        """Get game state data for AI"""
        
        return {
            'player_health': self._get_player_health(),
            'enemies_count': len(self.game_state['enemies']),
            'power_ups_count': len(self.game_state['power_ups']),
            'level': self.game_state['level'],
            'time_remaining': max(0, 100 - self.game_state['level'] * 10)
        }
        
    def _get_player_stats(self):
        """Get player statistics"""
        
        return {
            'position': self.game_state['player_position'],
            'score': self.game_state['player_score'],
            'health': self._get_player_health(),
            'inventory': self._get_player_inventory()
        }
        
    def _get_cell_type(self, x, y):
        """Get type of cell at position"""
        
        # Check for different cell types
        if self.game_state['player_position'] == (x, y):
            return 'player'
            
        for enemy in self.game_state['enemies']:
            if enemy['position'] == (x, y):
                return 'enemy'
                
        for power_up in self.game_state['power_ups']:
            if power_up['position'] == (x, y):
                return 'power_up'
                
        # Check for walls/obstacles
        if x == 0 or x == self.world.getWidth() - 1 or y == 0 or y == self.world.getHeight() - 1:
            return 'wall'
            
        return 'empty'
        
    def _find_nearest_enemy(self):
        """Find nearest enemy"""
        
        if not self.game_state['enemies']:
            return None
            
        player_pos = self.game_state['player_position']
        nearest = min(self.game_state['enemies'],
                     key=lambda e: ((e['position'][0] - player_pos[0])**2 +
                                   (e['position'][1] - player_pos[1])**2)**0.5)
        
        return nearest['position']
        
    def _find_nearest_power_up(self):
        """Find nearest power-up"""
        
        if not self.game_state['power_ups']:
            return None
            
        player_pos = self.game_state['player_position']
        nearest = min(self.game_state['power_ups'],
                     key=lambda p: ((p['position'][0] - player_pos[0])**2 +
                                   (p['position'][1] - player_pos[1])**2)**0.5)
        
        return nearest['position']
        
    def _get_player_health(self):
        """Get player health"""
        
        return 100  # Placeholder
        
    def _get_player_inventory(self):
        """Get player inventory"""
        
        return []  # Placeholder
        
    def _generate_new_content(self):
        """Generate new game content"""
        
        # Generate new enemy
        if random.random() < 0.3:
            enemy_x = random.randint(10, self.world.getWidth() - 10)
            enemy_y = random.randint(10, self.world.getHeight() - 10)
            
            if self._is_valid_position(enemy_x, enemy_y):
                self.game_state['enemies'].append({
                    'position': (enemy_x, enemy_y),
                    'type': random.choice(['easy', 'medium', 'hard']),
                    'health': random.randint(50, 100)
                })
                
        # Generate power-up
        if random.random() < 0.2:
            power_x = random.randint(10, self.world.getWidth() - 10)
            power_y = random.randint(10, self.world.getHeight() - 10)
            
            if self._is_valid_position(power_x, power_y):
                self.game_state['power_ups'].append({
                    'position': (power_x, power_y),
                    'type': random.choice(['health', 'ammo', 'speed'])
                })
                
    def _is_valid_position(self, x, y):
        """Check if position is valid"""
        
        return (0 <= x < self.world.getWidth() and 
                0 <= y < self.world.getHeight() and
                not any(e['position'] == (x, y) for e in self.game_state['enemies']) and
                not any(p['position'] == (x, y) for p in self.game_state['power_ups']))
        
    def _apply_ai_action(self, action):
        """Apply AI action to game"""
        
        # Convert action to game behavior
        if action.type == 'move_forward':
            self._move_player('forward')
        elif action.type == 'turn_left':
            self._rotate_player('left')
        elif action.type == 'turn_right':
            self._rotate_player('right')
        elif action.type == 'stop':
            self._stop_player()
            
    def _move_player(self, direction):
        """Move player in direction"""
        
        x, y = self.game_state['player_position']
        
        if direction == 'forward':
            new_x, new_y = x + 1, y
        else:
            # Handle other directions
            new_x, new_y = x, y
            
        if self._is_valid_position(new_x, new_y):
            self.game_state['player_position'] = (new_x, new_y)
            
    def _rotate_player(self, direction):
        """Rotate player"""
        
        # In 2D grid, rotation is not applicable
        # This would be for 3D games
        pass
        
    def _stop_player(self):
        """Stop player movement"""
        
        # Player just doesn't move
        pass
        
    def _update_game_state(self, action):
        """Update game state based on action"""
        
        # Check for collisions
        self._check_collisions()
        
        # Check for power-ups
        self._check_power_ups()
        
        # Check for enemies
        self._check_enemies()
        
        # Update score
        if action.type == 'collect':
            self.game_state['player_score'] += 10
            
    def _check_collisions(self):
        """Check for collisions with obstacles"""
        
        x, y = self.game_state['player_position']
        
        # Check for wall collision
        if x == 0 or x == self.world.getWidth() - 1 or y == 0 or y == self.world.getHeight() - 1:
            print("Wall collision!")
            
        # Check for enemy collision
        for enemy in self.game_state['enemies']:
            if enemy['position'] == (x, y):
                print(f"Enemy collision with {enemy['type']} enemy!")
                # Handle enemy collision
                break
                
    def _check_power_ups(self):
        """Check for power-up collection"""
        
        x, y = self.game_state['player_position']
        
        for power_up in self.game_state['power_ups']:
            if power_up['position'] == (x, y):
                # Apply power-up effect
                self._apply_power_up(power_up['type'])
                # Remove power-up
                self.game_state['power_ups'].remove(power_up)
                break
                
    def _apply_power_up(self, power_up_type):
        """Apply power-up effect"""
        
        if power_up_type == 'health':
            print("Health power-up collected!")
            # Restore health
        elif power_up_type == 'ammo':
            print("Ammo power-up collected!")
            # Add ammo
        elif power_up_type == 'speed':
            print("Speed power-up collected!")
            # Increase speed
            
    def _check_enemies(self):
        """Check for enemy encounters"""
        
        x, y = self.game_state['player_position']
        
        for enemy in self.game_state['enemies']:
            if enemy['position'] == (x, y):
                print(f"Enemy encounter with {enemy['type']} enemy!")
                # Handle enemy encounter
                break
                
    def _calculate_game_reward(self):
        """Calculate reward for game action"""
        
        reward = 0.0
        
        # Positive reward for collecting power-ups
        if hasattr(self, 'last_action') and self.last_action == 'collect':
            reward += 5.0
            
        # Negative reward for collisions
        if hasattr(self, 'last_collision') and self.last_collision:
            reward -= 10.0
            
        # Positive reward for killing enemies
        if hasattr(self, 'enemies_killed') and self.enemies_killed > 0:
            reward += self.enemies_killed * 20.0
            
        # Positive reward for score
        reward += self.game_state['player_score'] * 0.1
        
        return reward
        
    def _print_game_status(self, step):
        """Print game status"""
        
        print(f"\n=== Game Status - Step {step} ===")
        print(f"Player: {self.game_state['player_position']}")
        print(f"Score: {self.game_state['player_score']}")
        print(f"Level: {self.game_state['level']}")
        print(f"Enemies: {len(self.game_state['enemies'])}")
        print(f"Power-ups: {len(self.game_state['power_ups'])}")
        print(f"Action: {self.game_state.get('current_action', 'None')}")

# Use game AI
ai = GameAI()
ai.run_game_simulation(500)
```

## Advanced Configuration Options

### Neuron Configuration

```python
import pynlm

class AdvancedNeuronConfig:
    """Advanced neuron configuration options"""
    
    @staticmethod
    def create_highly_excitatory():
        """Create highly excitatory neuron configuration"""
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000)
        config.set("brain.excitability", 2.0)  # High excitability
        config.set("learning.rate", 0.05)    # High learning rate
        config.set("simulation.time_step", 0.02)  # Small timestep
        
        return config
        
    @staticmethod
    def create_stable_inhibitory():
        """Create stable inhibitory neuron configuration"""
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1500)
        config.set("brain.excitability", 0.5)   # Low excitability
        config.set("learning.rate", 0.01)       # Low learning rate
        config.set("simulation.time_step", 0.1) # Larger timestep
        
        return config
        
    @staticmethod
    def create_balanced():
        """Create balanced neuron configuration"""
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("brain.excitability", 1.0)   # Standard excitability
        config.set("learning.rate", 0.02)       # Standard learning rate
        config.set("simulation.time_step", 0.05) # Standard timestep
        
        return config

# Use advanced neuron configurations
highly_excitatory_config = AdvancedNeuronConfig.create_highly_excitatory()
stable_inhibitory_config = AdvancedNeuronConfig.create_stable_inhibitory()
balanced_config = AdvancedNeuronConfig.create_balanced()
```

### Memory Configuration

```python
import pynlm

class AdvancedMemoryConfig:
    """Advanced memory configuration options"""
    
    @staticmethod
    def create_large_memory():
        """Create large memory configuration"""
        
        config = pynlm.createDefaultConfig()
        config.set("memory.working_memory_size", 1000)    # Large working memory
        config.set("memory.episodic_capacity", 5000)      # Large episodic memory
        config.set("memory.associative_capacity", 2000)   # Large associative memory
        
        return config
        
    @staticmethod
    def create_efficient_memory():
        """Create efficient memory configuration"""
        
        config = pynlm.createDefaultConfig()
        config.set("memory.working_memory_size", 100)     # Small working memory
        config.set("memory.episodic_capacity", 500)       # Small episodic memory
        config.set("memory.associative_capacity", 200)    # Small associative memory
        
        return config
        
    @staticmethod
    def create_focused_memory():
        """Create focused memory configuration"""
        
        config = pynlm.createDefaultConfig()
        config.set("memory.working_memory_size", 500)     # Medium working memory
        config.set("memory.episodic_capacity", 2000)     # Medium episodic memory
        config.set("memory.associative_capacity", 800)   # Medium associative memory
        
        return config

# Use advanced memory configurations
large_memory_config = AdvancedMemoryConfig.create_large_memory()
efficient_memory_config = AdvancedMemoryConfig.create_efficient_memory()
focused_memory_config = AdvancedMemoryConfig.create_focused_memory()
```

### Neuromodulation Configuration

```python
import pynlm

class AdvancedNeuromodulationConfig:
    """Advanced neuromodulation configuration options"""
    
    @staticmethod
    def create_high_curiosity():
        """Create high curiosity configuration"""
        
        config = pynlm.createDefaultConfig()
        config.set("neuromodulation.curiosity_strength", 2.0)  # High curiosity
        config.set("neuromodulation.novelty_threshold", 0.3)   # Low novelty threshold
        config.set("neuromodulation.enable_curiosity", True)
        config.set("neuromodulation.enable_reward_modulation", True)
        
        return config
        
    @staticmethod
    def create_high_stability():
        """Create high stability configuration"""
        
        config = pynlm.createDefaultConfig()
        config.set("neuromodulation.curiosity_strength", 0.3)  # Low curiosity
        config.set("neuromodulation.novelty_threshold", 0.7)   # High novelty threshold
        config.set("neuromodulation.enable_curiosity", False)
        config.set("neuromodulation.enable_reward_modulation", True)
        
        return config
        
    @staticmethod
    def create_balanced_neuromodulation():
        """Create balanced neuromodulation configuration"""
        
        config = pynlm.createDefaultConfig()
        config.set("neuromodulation.curiosity_strength", 1.0)  # Standard curiosity
        config.set("neuromodulation.novelty_threshold", 0.5)   # Standard novelty threshold
        config.set("neuromodulation.enable_curiosity", True)
        config.set("neuromodulation.enable_reward_modulation", True)
        config.set("neuromodulation.enable_structural_plasticity", True)
        config.set("neuromodulation.enable_development", True)
        
        return config

# Use advanced neuromodulation configurations
high_curiosity_config = AdvancedNeuromodulationConfig.create_high_curiosity()
high_stability_config = AdvancedNeuromodulationConfig.create_high_stability()
balanced_neuromodulation_config = AdvancedNeuromodulationConfig.create_balanced_neuromodulation()
```

### Simulation Configuration

```python
import pynlm

class AdvancedSimulationConfig:
    """Advanced simulation configuration options"""
    
    @staticmethod
    def create_fast_simulation():
        """Create fast simulation configuration"""
        
        config = pynlm.createDefaultConfig()
        config.set("simulation.time_step", 0.01)    # Small timestep
        config.set("brain.excitability", 1.2)       # Optimal excitability
        config.set("learning.rate", 0.03)          # High learning rate
        
        return config
        
    @staticmethod
    def create_stable_simulation():
        """Create stable simulation configuration"""
        
        config = pynlm.createDefaultConfig()
        config.set("simulation.time_step", 0.1)     # Larger timestep
        config.set("brain.excitability", 0.8)      # Moderate excitability
        config.set("learning.rate", 0.01)          # Low learning rate
        
        return config
        
    @staticmethod
    def create_precise_simulation():
        """Create precise simulation configuration"""
        
        config = pynlm.createDefaultConfig()
        config.set("simulation.time_step", 0.001)  # Very small timestep
        config.set("brain.excitability", 1.0)      # Standard excitability
        config.set("learning.rate", 0.02)          # Standard learning rate
        
        return config

# Use advanced simulation configurations
fast_config = AdvancedSimulationConfig.create_fast_simulation()
stable_config = AdvancedSimulationConfig.create_stable_simulation()
precise_config = AdvancedSimulationConfig.create_precise_simulation()
```

## Conclusion

This enhanced easy usage guide provides comprehensive knowledge and practical examples for using the NLM Python API. It covers:

- **Basic Concepts**: Fundamental understanding of NLM components
- **Step-by-Step Examples**: Practical demonstrations of NLM usage
- **Error Handling**: Robust error handling and debugging techniques
- **Performance Optimization**: Tips for optimizing simulation performance
- **Advanced Configuration**: Advanced configuration options for fine-tuned control
- **Real-World Use Cases**: Practical applications in robotics, gaming, and more

Use this guide as a comprehensive resource for learning and implementing NLM simulations. Start with the basic examples and gradually explore more advanced features as you become more comfortable with the API.

## See Also

- [Comprehensive Usage Guide](../HOW_TO_USE.md)
- [Python API Reference](../src/docs/python_api.md)
- [Performance Tuning Guide](../src/docs/python_performance_tuning.md)
- [Troubleshooting Guide](../src/docs/python_troubleshooting.md)
- [Advanced Usage Guide](../src/docs/python_advanced_usage.md)
- [Architecture Documentation](../docs/ARCHITECTURE.md)
- [Phase 6 Final Audit](../docs/PHASE6_FINAL_AUDIT.md)
