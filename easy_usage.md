# NLM - Enhanced Easy Usage Guide for Beginners

## What is Enhanced NLM?

Enhanced NLM is a **brain simulator** for computers with advanced features, comprehensive monitoring, and powerful utilities. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works! The enhanced version adds:

- **Comprehensive monitoring** with real-time statistics
- **Error handling** with automatic recovery
- **Performance optimization** with benchmarking
- **Advanced debugging** with detailed analysis
- **Experiment management** with parameter sweeps
- **Session management** with checkpointing and restoration

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells  
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks (with enhanced monitoring and error handling)
2. **World** - The environment the brain lives in (with enhanced simulation)
3. **Agent** - The bridge connecting brain to world (with advanced interface)

---

## Quick Start with Enhanced Features (Copy & Paste)

### Example 1: Simplest Brain with Enhanced Monitoring

```python
import pynlm
from nlm_utils import (
    AdvancedBrainSession, SessionConfigAdvanced,
    AdvancedDebugLogger, Benchmark, MemoryMonitor
)

# Step 1: Create enhanced configuration with monitoring
config = SessionConfigAdvanced(
    name="enhanced_brain_demo",
    brain_config={
        "brain.neuron_count": 1000,
        "performance.monitoring": True,
        "error_handling.recovery": True,
        "debugging.enable": True
    },
    checkpoint_interval=100,
    auto_save=True
)

# Step 2: Create enhanced session
session = AdvancedBrainSession(config)

# Step 3: Set up monitoring
logger = AdvancedDebugLogger()
benchmark = Benchmark("enhanced_brain_benchmark")
monitor = MemoryMonitor("brain_memory_monitor")

@benchmark
@monitor
def enhanced_brain_simulation(session, steps=100):
    """Enhanced brain simulation with comprehensive monitoring."""
    for step in range(steps):
        session.brain.step(step)
        
        if step % 20 == 0:
            # Get comprehensive metrics
            metrics = session.get_step_metrics(step)
            logger.log_step(step, metrics)
    
    return {
        'total_spikes': session.brain.getTotalSpikeCount(),
        'firing_neurons': session.brain.getFiringNeuronCount(),
        'average_firing_rate': session.brain.getAverageFiringRate(),
        'benchmark_results': benchmark.get_results(),
        'memory_stats': monitor.get_summary()
    }

# Run enhanced simulation
results = enhanced_brain_simulation(session, 100)
print("Enhanced simulation complete!")
print(f"Total spikes: {results['total_spikes']}")
print(f"Firing neurons: {results['firing_neurons']}")
print(f"Average firing rate: {results['average_firing_rate']:.2f}")
print(f"Benchmark results: {results['benchmark_results']}")
```

### Example 2: Enhanced Brain in a Simple World

```python
import pynlm
from nlm_utils import (
    AdvancedBrainSession, SessionConfigAdvanced,
    AdvancedDebugLogger, MemoryMonitor
)

# Create enhanced configuration
config = SessionConfigAdvanced(
    name="enhanced_world_demo",
    brain_config={
        "brain.neuron_count": 500,
        "plasticity.stdp.enable": True,
        "performance.monitoring": True,
        "error_handling.recovery": True
    },
    world_config={
        "width": 20,
        "height": 20,
        "visionWidth": 8,
        "visionHeight": 8
    },
    checkpoint_interval=50,
    auto_save=True
)

# Create enhanced session
session = AdvancedBrainSession(config)

# Set up monitoring
logger = AdvancedDebugLogger()
monitor = MemoryMonitor("world_simulation_memory")

@monitor
def enhanced_world_simulation(session, steps=50):
    """Enhanced world simulation with comprehensive monitoring."""
    for step in range(steps):
        try:
            # 1. Update world with timing
            world_update_start = time.time()
            session.world.update(0.1)
            world_update_time = time.time() - world_update_start
            
            # 2. Get what the agent sees
            percept = session.world.getSensoryPercept()
            
            # 3. Tell the brain with error handling
            session.agent.processSensoryInput(percept)
            
            # 4. Brain thinks with monitoring
            brain_step_start = time.time()
            session.brain.step(step)
            brain_step_time = time.time() - brain_step_start
            
            # 5. Get action from brain
            action = session.agent.decodeMotorCommand()
            
            # 6. Do action in world with validation
            session.world.applyMotorCommand(action, session.world.getSimulationTime())
            
            # Log comprehensive metrics
            metrics = session.get_step_metrics(step)
            logger.log_step(step, metrics)
            
        except Exception as e:
            logger.log_error(step, str(e))
            session.recover_from_error(step, e)
            continue
    
    return {
        'steps_completed': steps,
        'memory_stats': monitor.get_summary(),
        'final_spikes': session.brain.getTotalSpikeCount(),
        'final_firing': session.brain.getFiringNeuronCount()
    }

# Run enhanced world simulation
import time
results = enhanced_world_simulation(session, 50)
print("Enhanced world simulation complete!")
print(f"Steps completed: {results['steps_completed']}")
print(f"Final spikes: {results['final_spikes']}")
print(f"Final firing neurons: {results['final_firing']}")
print(f"Memory usage: {results['memory_stats']}")
```

---

## Simple Analogy with Enhanced Features

| Real World | Enhanced NLM Code |
|------------|-------------------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it with monitoring | `session.agent.processSensoryInput(percept)` |
| You decide to act with error handling | `action = session.agent.decodeMotorCommand()` |
| You do the action with validation | `session.world.applyMotorCommand(action, time)` |
| Your brain learns with recovery | `session.agent.applyRewardModulation(reward, predicted)` |
| Your brain is monitored in real-time | `logger.log_step(step, metrics)` |
| Your simulation is benchmarked | `benchmark.run(simulation, steps)` |

---

## The Simplest Possible Enhanced Example

```python
import pynlm
from nlm_utils import (
    AdvancedBrainSession, SessionConfigAdvanced,
    AdvancedDebugLogger
)

# One line to create an enhanced virtual brain with monitoring
config = SessionConfigAdvanced(
    name="simple_enhanced_demo",
    brain_config={
        "brain.neuron_count": 500,
        "performance.monitoring": True,
        "error_handling.recovery": True
    },
    checkpoint_interval=100
)

session = AdvancedBrainSession(config)
logger = AdvancedDebugLogger()

# Initialize it with monitoring
session.brain.initialize()
logger.log_initialization(session.brain, config.brain_config)

# Make it active with monitoring
for step in range(10):
    session.brain.step(step)
    if step % 5 == 0:
        metrics = session.get_step_metrics(step)
        logger.log_step(step, metrics)

print("Enhanced simple brain test complete!")
print(f"Your brain has {session.brain.getTotalNeuronCount()} neurons!")
print(f"Total spikes: {session.brain.getTotalSpikeCount()}")
print(f"Firing neurons: {session.brain.getFiringNeuronCount()}")
```

---

## Enhanced Common Patterns

### Pattern 1: Enhanced Simulation with Comprehensive Monitoring

```python
from nlm_utils import (
    AdvancedBrainSession, SessionConfigAdvanced,
    AdvancedDebugLogger, MemoryMonitor, Benchmark
)

def run_enhanced_simulation(brain_config, world_config, steps):
    """Enhanced simulation with comprehensive monitoring and error handling."""
    
    # Create enhanced configuration
    config = SessionConfigAdvanced(
        name="enhanced_simulation",
        brain_config=brain_config,
        world_config=world_config,
        checkpoint_interval=min(100, steps),
        auto_save=True,
        performance_monitoring=True,
        error_recovery=True
    )
    
    session = AdvancedBrainSession(config)
    logger = AdvancedDebugLogger()
    monitor = MemoryMonitor("enhanced_simulation_memory")
    benchmark = Benchmark("enhanced_simulation_benchmark")
    
    @benchmark
    @monitor
    def enhanced_run(session, steps):
        """Run enhanced simulation."""
        for step in range(steps):
            try:
                # 1. Update world
                session.world.update(0.1)
                
                # 2. Get what the agent sees
                percept = session.world.getSensoryPercept()
                
                # 3. Tell the brain
                session.agent.processSensoryInput(percept)
                
                # 4. Brain thinks
                session.brain.step(step)
                
                # 5. Get action from brain
                action = session.agent.decodeMotorCommand()
                
                # 6. Do action in world
                session.world.applyMotorCommand(action, session.world.getSimulationTime())
                
                # Log comprehensive metrics
                metrics = session.get_step_metrics(step)
                logger.log_step(step, metrics)
                
            except Exception as e:
                logger.log_error(step, str(e))
                session.recover_from_error(step, e)
                continue
        
        return {
            'steps_completed': steps,
            'memory_stats': monitor.get_summary(),
            'benchmark_results': benchmark.get_results()
        }
    
    # Run enhanced simulation
    results = enhanced_run(session, steps)
    
    # Save session
    session.save(f"enhanced_simulation_{steps}")
    
    return results

# Use it like this:
brain_config = {
    "brain.neuron_count": 1000,
    "plasticity.stdp.enable": True,
    "performance.monitoring": True
}

world_config = {
    "width": 20,
    "height": 20,
    "visionWidth": 8,
    "visionHeight": 8
}

results = run_enhanced_simulation(brain_config, world_config, 1000)
print(f"Enhanced simulation complete: {results['steps_completed']} steps")
```

### Pattern 2: Enhanced Brain Stats with Real-time Monitoring

```python
from nlm_utils import AdvancedDebugLogger

def monitor_brain_stats(session, steps, output_interval=50):
    """Monitor brain stats with real-time logging."""
    
    logger = AdvancedDebugLogger()
    
    for step in range(steps):
        # Get comprehensive brain stats
        metrics = session.get_step_metrics(step)
        
        # Log enhanced metrics
        logger.log_step(step, metrics)
        
        if step % output_interval == 0:
            print(f"Step {step}:")
            print(f"  Neurons firing: {metrics['firing_neurons']}")
            print(f"  Total spikes: {metrics['total_spikes']}")
            print(f"  Average firing rate: {metrics['average_firing_rate']:.2f}")
            print(f"  Memory activity: {metrics['memory_activity']:.3f}")
            print(f"  Development stage: {metrics['development_stage']}")
            print(f"  World update time: {metrics['world_update_time']:.4f}s")
            print(f"  Brain step time: {metrics['brain_step_time']:.4f}s")
            print(f"  Reward: {metrics['reward']:.2f}")
```

### Pattern 3: Enhanced Brain Features with Error Recovery

```python
from nlm_utils import (
    AdvancedBrainSession, SessionConfigAdvanced,
    AdvancedDebugLogger, ErrorRecoveryManager
)

def run_enhanced_features(session):
    """Run simulation with enhanced features and error recovery."""
    
    logger = AdvancedDebugLogger()
    recovery_manager = ErrorRecoveryManager("enhanced_features")
    
    # These are all optional enhanced features:
    session.agent.enableRewardModulation(True)      # Learn from rewards with recovery
    session.agent.enableStructuralPlasticity(True)  # Grow new connections with monitoring
    session.agent.enableDevelopment(True)           # Brain matures over time with logging
    session.agent.enableCuriosity(True)             # Explore new things with recovery
    
    for step in range(100):
        try:
            # Run simulation with enhanced error recovery
            session.world.update(0.1)
            percept = session.world.getSensoryPercept()
            session.agent.processSensoryInput(percept)
            session.brain.step(step)
            action = session.agent.decodeMotorCommand()
            session.world.applyMotorCommand(action, session.world.getSimulationTime())
            
            # Apply enhanced neuromodulation
            reward = session.get_reward()
            session.agent.applyRewardModulation(reward, 0.0)
            
            # Enhanced development
            session.agent.updateDevelopment(0.1)
            
            # Log comprehensive metrics
            metrics = session.get_step_metrics(step)
            logger.log_step(step, metrics)
            
        except Exception as e:
            # Enhanced error recovery
            recovery_action = recovery_manager.recover_from_error(
                e, step, get_context=session.get_current_context()
            )
            logger.log_error(step, f"Recovered with action: {recovery_action}")
            session.recover_from_error(step, e)
            continue
    
    return session.get_simulation_summary()

# Create session and run enhanced features
config = SessionConfigAdvanced(
    name="enhanced_features_demo",
    brain_config={
        "brain.neuron_count": 500,
        "performance.monitoring": True,
        "error_handling.recovery": True
    }
)

session = AdvancedBrainSession(config)
results = run_enhanced_features(session)
print(f"Enhanced features simulation complete!")
print(f"Steps completed: {results['steps_completed']}")
print(f"Recovery actions: {results['recovery_actions']}")
print(f"Error count: {results['error_count']}")
```

---

## What Each Enhanced Component Does

### `AdvancedBrainSession(config)`
Creates an enhanced brain session with comprehensive monitoring and error recovery.
- `config` = Enhanced session configuration (includes monitoring, checkpointing, recovery)

### `AdvancedBrainSession.run_step(step)`
Enhanced step execution with error handling, monitoring, and recovery.

### `AdvancedBrainSession.get_step_metrics(step)`
Get comprehensive step metrics including performance and error information.

### `AdvancedBrainSession.checkpoint(name)`
Save session state with enhanced checkpointing.

### `AdvancedDebugLogger(config)`
Advanced debugging logger with structured logging and anomaly detection.

### `Benchmark(name, warmup_steps)`
Performance benchmarking with comprehensive metrics.

### `MemoryMonitor(name, interval)`
Memory usage monitoring with alerts and leak detection.

### `ErrorRecoveryManager(name)`
Automatic error recovery with intelligent recovery actions.

---

## Enhanced Mini Projects

### Project 1: Silent Brain with Enhanced Monitoring

```python
import pynlm
from nlm_utils import (
    AdvancedBrainSession, SessionConfigAdvanced,
    AdvancedDebugLogger, Benchmark, MemoryMonitor
)

# Create enhanced silent brain session
config = SessionConfigAdvanced(
    name="enhanced_silent_brain",
    brain_config={
        "brain.neuron_count": 1000,
        "performance.monitoring": True,
        "error_handling.recovery": True
    },
    checkpoint_interval=50,
    auto_save=True
)

session = AdvancedBrainSession(config)
logger = AdvancedDebugLogger()
benchmark = Benchmark("silent_brain_benchmark")
monitor = MemoryMonitor("silent_brain_memory")

@benchmark
@monitor
def enhanced_silent_brain_simulation(session, steps=10):
    """Enhanced silent brain simulation."""
    for step in range(steps):
        # Enhanced step execution with monitoring
        try:
            session.brain.step(step)
            
            # Log comprehensive metrics
            metrics = session.get_step_metrics(step)
            logger.log_step(step, metrics)
            
        except Exception as e:
            logger.log_error(step, str(e))
            session.recover_from_error(step, e)
            continue
    
    return {
        'steps_completed': steps,
        'benchmark_results': benchmark.get_results(),
        'memory_stats': monitor.get_summary(),
        'total_spikes': session.brain.getTotalSpikeCount(),
        'final_firing': session.brain.getFiringNeuronCount()
    }

# Run enhanced silent brain test
results = enhanced_silent_brain_simulation(session, 10)
session.save("enhanced_silent_brain")

print("Enhanced silent brain test complete!")
print(f"Total spikes: {results['total_spikes']}")
print(f"Firing neurons: {results['final_firing']}")
print(f"Benchmark: {results['benchmark_results']}")
```

### Project 2: Enhanced Brain Watching an Enhanced World

```python
import pynlm
from nlm_utils import (
    AdvancedBrainSession, SessionConfigAdvanced,
    AdvancedDebugLogger, MemoryMonitor
)

# Create enhanced world session
config = SessionConfigAdvanced(
    name="enhanced_world_observer",
    brain_config={
        "brain.neuron_count": 500,
        "performance.monitoring": True,
        "error_handling.recovery": True
    },
    world_config={
        "width": 30,
        "height": 30,
        "visionWidth": 12,
        "visionHeight": 12
    },
    checkpoint_interval=30,
    auto_save=True
)

session = AdvancedBrainSession(config)
logger = AdvancedDebugLogger()
monitor = MemoryMonitor("world_observer_memory")

@monitor
def enhanced_world_observer_simulation(session, steps=30):
    """Enhanced world observer simulation."""
    for step in range(steps):
        try:
            # Enhanced world update with timing
            world_start = time.time()
            session.world.update(0.1)
            world_time = time.time() - world_start
            
            # Enhanced sensory processing
            percept = session.world.getSensoryPercept()
            session.agent.processSensoryInput(percept)
            
            # Enhanced brain processing with monitoring
            brain_start = time.time()
            session.brain.step(step)
            brain_time = time.time() - brain_start
            
            # Enhanced motor control
            action = session.agent.decodeMotorCommand()
            session.world.applyMotorCommand(action, session.world.getSimulationTime())
            
            # Log comprehensive metrics
            metrics = session.get_step_metrics(step)
            metrics.update({
                'world_update_time': world_time,
                'brain_step_time': brain_time,
                'step_duration': world_time + brain_time
            })
            logger.log_step(step, metrics)
            
        except Exception as e:
            logger.log_error(step, str(e))
            session.recover_from_error(step, e)
            continue
    
    return {
        'steps_completed': steps,
        'memory_stats': monitor.get_summary(),
        'world_metrics': session.get_world_metrics(),
        'brain_metrics': session.get_brain_metrics()
    }

# Run enhanced world observer
import time
results = enhanced_world_observer_simulation(session, 30)
session.save("enhanced_world_observer")

print("Enhanced world observer complete!")
print(f"Steps completed: {results['steps_completed']}")
print(f"Memory usage: {results['memory_stats']}")
print(f"World metrics: {results['world_metrics']}")
print(f"Brain metrics: {results['brain_metrics']}")
```

### Project 3: Enhanced Complete Agent

```python
import pynlm
from nlm_utils import (
    AdvancedBrainSession, SessionConfigAdvanced,
    AdvancedDebugLogger, MemoryMonitor
)

# Create enhanced complete agent session
config = SessionConfigAdvanced(
    name="enhanced_complete_agent",
    brain_config={
        "brain.neuron_count": 800,
        "plasticity.stdp.enable": True,
        "performance.monitoring": True,
        "error_handling.recovery": True
    },
    world_config={
        "width": 25,
        "height": 25,
        "visionWidth": 10,
        "visionHeight": 10
    },
    checkpoint_interval=50,
    auto_save=True
)

session = AdvancedBrainSession(config)
logger = AdvancedDebugLogger()
monitor = MemoryMonitor("complete_agent_memory")

@monitor
def enhanced_complete_agent_simulation(session, steps=100):
    """Enhanced complete agent simulation."""
    for step in range(steps):
        try:
            # Setup enhanced simulation step
            session.world.update(0.1)
            percept = session.world.getSensoryPercept()
            session.agent.processSensoryInput(percept)
            session.brain.step(step)
            action = session.agent.decodeMotorCommand()
            session.world.applyMotorCommand(action, session.world.getSimulationTime())
            
            # Enhanced neuromodulation
            reward = session.get_reward()
            session.agent.applyRewardModulation(reward, 0.0)
            
            # Enhanced development
            session.agent.updateDevelopment(0.1)
            
            # Log comprehensive metrics
            metrics = session.get_step_metrics(step)
            metrics.update({
                'reward': reward,
                'curiosity': session.agent.getCuriosityLevel(),
                'novelty': session.agent.getNoveltyLevel(),
                'development_stage': session.brain.getDevelopmentalStage()
            })
            logger.log_step(step, metrics)
            
        except Exception as e:
            logger.log_error(step, str(e))
            session.recover_from_error(step, e)
            continue
        
        # Print enhanced progress
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {metrics['firing_neurons']}")
            print(f"  Total spikes: {metrics['total_spikes']}")
            print(f"  Average firing rate: {metrics['average_firing_rate']:.2f}")
            print(f"  Memory activity: {metrics['memory_activity']:.3f}")
            print(f"  Reward: {metrics['reward']:.2f}")
            print(f"  Curiosity: {metrics['curiosity']:.3f}")
            print(f"  Development stage: {metrics['development_stage']}")
    
    return {
        'steps_completed': steps,
        'memory_stats': monitor.get_summary(),
        'final_spikes': session.brain.getTotalSpikeCount(),
        'final_firing': session.brain.getFiringNeuronCount(),
        'final_reward': session.get_reward(),
        'final_curiosity': session.agent.getCuriosityLevel(),
        'final_novelty': session.agent.getNoveltyLevel()
    }

# Run enhanced complete agent
results = enhanced_complete_agent_simulation(session, 100)
session.save("enhanced_complete_agent")

print("\nEnhanced complete agent simulation complete!")
print(f"Steps completed: {results['steps_completed']}")
print(f"Final spikes: {results['final_spikes']}")
print(f"Final firing neurons: {results['final_firing']}")
print(f"Final reward: {results['final_reward']:.2f}")
print(f"Final curiosity: {results['final_curiosity']:.3f}")
print(f"Final novelty: {results['final_novelty']:.3f}")
```

---

## Enhanced Quick Reference

| What you want | Enhanced Code |
|--------------|---------------|
| Create enhanced brain | `AdvancedBrainSession(config)` |
| Start enhanced brain | `session.brain.initialize()` |
| Make enhanced brain think | `session.run_step(step)` |
| Create enhanced world | `AdvancedBrainSession(config)` |
| Create enhanced agent | `AdvancedBrainSession(config)` |
| Get enhanced brain stats | `session.get_step_metrics(step)` |
| See enhanced world | `session.world.getSensoryPercept()` |
| Make enhanced action | `session.agent.decodeMotorCommand()` |
| Monitor performance | `AdvancedDebugLogger()`, `Benchmark()`, `MemoryMonitor()` |
| Recover from errors | `session.recover_from_error(step, e)` |
| Save simulation | `session.save("name")` |

---

## Enhanced Troubleshooting

**"My enhanced brain isn't doing anything"**
- Did you call `session.brain.initialize()`?
- Check the enhanced logs for errors
- Try increasing the number of steps
- Monitor memory usage for potential leaks

**"The enhanced agent isn't moving"**
- Did you call `session.world.applyMotorCommand()`?
- Check that `session.world.update()` is being called
- Monitor timing metrics for performance issues
- Check error logs for recovery actions

**"Everything is 0"**
- Enhanced brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Monitor memory usage for potential issues
- Check for performance bottlenecks

**"Performance is slow"**
- Monitor memory usage with `MemoryMonitor()`
- Check timing metrics with `AdvancedDebugLogger()`
- Enable performance benchmarking with `Benchmark()`
- Look for recovery actions in logs

**"Memory usage is high"**
- Monitor memory usage with `MemoryMonitor()`
- Enable memory alerts and leak detection
- Check for memory issues in logs
- Consider reducing simulation complexity

---

## Enhanced Next Steps

When you're comfortable with enhanced NLM:

1. **Read `HOW_TO_USE.md`** for advanced features and comprehensive API documentation
2. **Read `docs/ARCHITECTURE.md`** to understand how all the enhanced components work together
3. **Experiment with different configurations** using the enhanced monitoring and debugging tools
4. **Run enhanced experiments** using the `MultiObjectiveExperiment` and `ParameterSweeper` utilities
5. **Manage complex sessions** using the advanced `SessionManagerAdvanced` and `ParallelSessionManager` tools
6. **Profile performance** using `PerformanceProfiler` and `Profiler` utilities
7. **Debug complex issues** using `BrainStateAnalyzer` and `AnomalyDetector` tools

---

## That's It! You're Now Ready to Use Enhanced NLM!

You now have access to the enhanced NLM with:

- **Comprehensive monitoring** with real-time statistics
- **Error handling** with automatic recovery
- **Performance optimization** with benchmarking
- **Advanced debugging** with detailed analysis
- **Experiment management** with parameter sweeps
- **Session management** with checkpointing and restoration

The enhanced NLM makes it easy to run powerful simulations with comprehensive monitoring and error recovery. Whether you're running simple demonstrations or complex experiments, the enhanced features help you track performance, debug issues, and get the most out of your NLM simulations.

Enjoy experimenting with the enhanced NLM!
