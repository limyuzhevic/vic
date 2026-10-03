# NLM - Advanced Usage Guide

## Advanced Configuration and Customization

This guide covers advanced usage patterns, performance optimization, and deep customization of the NLM framework for experienced users.

## Deep Configuration Customization

### Custom Brain Configuration

```python
import pynlm
import json

# Create a custom configuration with advanced settings
config = pynlm.createAdvancedConfig(
    numNeurons=500,              # More neurons for complex tasks
    learningRate=0.001,          # Lower learning rate for stability
    enableDevelopment=True,      # Enable developmental processes
    enableRewardModulation=True, # Enable reward-based learning
    enableCuriosity=True,        # Enable exploration behavior
    enableStructuralPlasticity=True  # Allow new connections
)

# Add custom settings
config.set("developmental_rate", 0.05)
config.set("plasticity_threshold", 0.3)
config.set("reward_decay", 0.95)
config.set("novelty_threshold", 0.7)
config.set("prediction_horizon", 10)

# Save advanced configuration
config.saveToFile("advanced_config.json")

# Load and modify configuration
default_config = pynlm.createDefaultConfig()
default_config.set("num_neurons", 1000)
default_config.set("learning_rate", 0.0001)
brain = pynlm.createBrain(default_config)
brain.initialize()
```

### World Environment Customization

```python
import pynlm

# Create complex environment with specific features
world = pynlm.createWorldWithResources(width=50, height=50, resourceDensity=0.15)
world.configure(width=50, height=50, visionWidth=16, visionHeight=16)

# Add obstacles and hazards
obstacles = [
    (5, 5, pynlm.WorldObjectType.Wall, 0.0f, 1.0f),
    (45, 45, pynlm.WorldObjectType.Wall, 0.0f, 1.0f),
    (25, 10, pynlm.WorldObjectType.Hazard, -10.0f, 0.8f),
    (10, 25, pynlm.WorldObjectType.Resource, 20.0f, 0.6f),
]

for x, y, obj_type, value, radius in obstacles:
    world.addObject(pynlm.WorldObject(x, y, obj_type, value, radius))

# Add moving objects
import random
for i in range(10):
    obj = pynlm.WorldObject(
        random.uniform(5, 45), random.uniform(5, 45),
        pynlm.WorldObjectType.Resource,
        random.uniform(5, 15),
        random.uniform(0.3, 0.8)
    )
    world.addObject(obj)

# Set agent starting position
world.setAgentStart(2, 2)
world.reset()
```

## Advanced Simulation Techniques

### Batch Processing and Multi-Agent Simulation

```python
import pynlm

def runMultiAgentSimulation(num_agents=5, steps_per_agent=100):
    """Run multiple agent simulations in parallel"""
    all_results = []
    
    for agent_id in range(num_agents):
        # Create unique brain for each agent
        config = pynlm.createAdvancedConfig(
            numNeurons=200 + agent_id * 50,  # Varied brain sizes
            learningRate=0.01 / (agent_id + 1),  # Different learning rates
            enableDevelopment=True,
            enableRewardModulation=True
        )
        
        # Create environment with agent-specific characteristics
        world = pynlm.createWorldWithResources(
            width=30, height=30, 
            resourceDensity=0.1 + agent_id * 0.02
        )
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all features
        agent.enableAllFeatures()
        
        # Run simulation for this agent
        agent_results = {
            "agent_id": agent_id,
            "steps_completed": 0,
            "final_firing_rate": 0.0,
            "total_spikes": 0,
            "learning_metrics": {},
            "performance_score": 0.0
        }
        
        for step in range(steps_per_agent):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Collect metrics
            agent_results["steps_completed"] += 1
            agent_results["final_firing_rate"] = brain.getAverageFiringRate()
            agent_results["total_spikes"] = brain.getTotalSpikeCount()
        
        all_results.append(agent_results)
    
    return all_results

# Run multi-agent simulation
results = runMultiAgentSimulation(num_agents=3, steps_per_agent=50)
for result in results:
    print(f"Agent {result['agent_id']}: {result['steps_completed']} steps, "
          f"final firing rate: {result['final_firing_rate']:.3f}")
```

### Performance Optimization

```python
import pynlm
import time
import psutil
import os

def optimizeForPerformance(brain, world, agent, target_fps=30):
    """Optimize simulation performance for target FPS"""
    timestep = 1.0 / target_fps
    
    # Performance monitoring
    process = psutil.Process(os.getpid())
    memory_samples = []
    fps_samples = []
    
    start_time = time.time()
    frame_count = 0
    
    # Phase 1: Warm-up and optimization
    for warm_step in range(10):
        world.update(timestep)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(warm_step)
    
    # Phase 2: Performance measurement
    for step in range(100):
        step_start = time.time()
        
        world.update(timestep)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Measure performance
        step_end = time.time()
        actual_fps = 1.0 / (step_end - step_start)
        fps_samples.append(actual_fps)
        
        memory_samples.append(process.memory_info().rss / 1024 / 1024)  # MB
        
        frame_count += 1
    
    end_time = time.time()
    total_time = end_time - start_time
    
    # Calculate statistics
    avg_fps = frame_count / total_time
    min_fps = min(fps_samples)
    max_fps = max(fps_samples)
    avg_memory = sum(memory_samples) / len(memory_samples)
    
    # Optimization suggestions
    suggestions = []
    if avg_fps < target_fps:
        suggestions.append(f"Increase timestep to {timestep * 1.1:.3f} to reach {target_fps} FPS")
    
    if avg_memory > 500:  # 500MB threshold
        suggestions.append("Reduce brain size or increase GC frequency")
    
    if max_fps < target_fps * 0.8:
        suggestions.append("Consider reducing neural complexity or using SIMD optimizations")
    
    return {
        "avg_fps": avg_fps,
        "min_fps": min_fps,
        "max_fps": max_fps,
        "avg_memory_mb": avg_memory,
        "total_steps": frame_count,
        "optimization_suggestions": suggestions
    }

# Use optimization
results = optimizeForPerformance(brain, world, agent, target_fps=30)
print(f"Performance Results: {results}")
```

## Advanced Learning and Development

### Developmental Psychology Simulation

```python
import pynlm
import math

class DevelopmentalBrain:
    def __init__(self):
        self.config = pynlm.createAdvancedConfig(
            numNeurons=300,
            learningRate=0.02,
            enableDevelopment=True,
            enableCuriosity=True,
            enableRewardModulation=True,
            enableStructuralPlasticity=True
        )
        self.brain = pynlm.createBrain(self.config)
        self.agent = None
        self.world = pynlm.createWorldWithResources(20, 20, 0.12)
        
    def runDevelopmentalSimulation(self, stages=5):
        """Simulate brain development across multiple stages"""
        self.brain.initialize()
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        developmental_history = {
            "stage": [],
            "firing_rate": [],
            "plasticity": [],
            "exploration": [],
            "learning": []
        }
        
        # Define stage characteristics
        stage_params = {
            "Initial": {"plasticity": 0.8, "exploration": 0.6, "neurongrowth": 0.5},
            "CriticalPeriod": {"plasticity": 0.6, "exploration": 0.4, "neurongrowth": 0.7},
            "Maturation": {"plasticity": 0.4, "exploration": 0.3, "neurongrowth": 0.9},
            "Adult": {"plasticity": 0.2, "exploration": 0.2, "neurongrowth": 1.0},
            "Aging": {"plasticity": 0.1, "exploration": 0.1, "neurongrowth": 0.8}
        }
        
        for stage_idx in range(stages):
            stage = list(stage_params.keys())[stage_idx]
            
            # Set developmental stage
            self.brain.setDevelopmentalStage(
                pynlm.DevelopmentalStage(list(pynlm.DevelopmentalStage).index(
                    getattr(pynlm.DevelopmentalStage, stage)
                ))
            )
            
            # Run stage simulation
            stage_results = self._runDevelopmentalStage(
                stage, stage_params[stage], 50
            )
            
            # Record history
            developmental_history["stage"].append(stage)
            developmental_history["firing_rate"].append(stage_results["avg_firing_rate"])
            developmental_history["plasticity"].append(stage_results["avg_plasticity"])
            developmental_history["exploration"].append(stage_results["avg_exploration"])
            developmental_history["learning"].append(stage_results["learning_rate"])
            
            print(f"Completed {stage} stage: FR={stage_results['avg_firing_rate']:.3f}, "
                  f"Plasticity={stage_results['avg_plasticity']:.3f}")
        
        return developmental_history
    
    def _runDevelopmentalStage(self, stage, params, steps):
        """Run a single developmental stage"""
        firing_rates = []
        plasticity_values = []
        exploration_values = []
        learning_rates = []
        
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(params["exploration"] > 0.3)
        self.agent.enableRewardModulation(True)
        
        for step in range(steps):
            self.world.update(0.1)
            self.agent.processSensoryInput(self.world.getSensoryPercept())
            self.brain.step(step)
            
            # Record metrics
            firing_rates.append(self.brain.getAverageFiringRate())
            plasticity_values.append(self.agent.getNeuromodulationLevel())
            exploration_values.append(self.agent.getCuriosityLevel())
            learning_rates.append(1.0 - self.brain.getPredictionError())
            
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply developmental effects
            if stage == "Initial":
                # Initial synaptogenesis
                if step % 10 == 0:
                    self.brain.addRegion(f"Region_{step}")
            elif stage == "CriticalPeriod":
                # Pruning and myelination
                if step % 15 == 0:
                    # Simulate synaptic pruning
                    pass
            elif stage == "Aging":
                # Reduced plasticity
                self.agent.enableStructuralPlasticity(params["neurongrowth"] < 1.0)
        
        return {
            "avg_firing_rate": sum(firing_rates) / len(firing_rates),
            "avg_plasticity": sum(plasticity_values) / len(plasticity_values),
            "avg_exploration": sum(exploration_values) / len(exploration_values),
            "learning_rate": sum(learning_rates) / len(learning_rates),
            "max_firing_rate": max(firing_rates),
            "min_firing_rate": min(firing_rates)
        }

# Run developmental simulation
dev_brain = DevelopmentalBrain()
history = dev_brain.runDevelopmentalSimulation()
```

## Advanced Debugging and Analysis

### Detailed Brain Analysis

```python
import pynlm
import json
import math

def analyzeBrainComplexity(brain):
    """Deep analysis of brain complexity and structure"""
    analysis = {}
    
    # Basic statistics
    analysis["basic_stats"] = {
        "total_neurons": brain.getTotalNeuronCount(),
        "total_synapses": brain.getTotalSynapseCount(),
        "active_neurons": brain.getActiveNeuronCount(),
        "firing_neurons": brain.getFiringNeuronCount(),
        "total_spikes": brain.getTotalSpikeCount(),
        "developmental_stage": int(brain.getDevelopmentalStage()),
        "stage_name": str(brain.getDevelopmentalStage())
    }
    
    # Network properties
    neurons = brain.getTotalNeuronCount()
    synapses = brain.getTotalSynapseCount()
    analysis["network_properties"] = {
        "neuron_density": synapses / max(neurons, 1),
        "connection_density": synapses / max(neurons * neurons, 1),
        "firing_efficiency": brain.getTotalSpikeCount() / max(neurons, 1),
        "excitatory_inhibition_ratio": brain.getExcitationInhibitionRatio(),
        "average_firing_rate": brain.getAverageFiringRate()
    }
    
    # Regional analysis
    regions = brain.getRegions()
    region_stats = {}
    for region in regions:
        # Note: This assumes NeuralRegion has these methods
        region_stats[region.getName()] = {
            "neuron_count": region.getNeuronCount(),
            "synapse_count": region.getSynapseCount(),
            "firing_count": region.getFiringCount()
        }
    analysis["regional_stats"] = region_stats
    
    # Performance indicators
    analysis["performance_indicators"] = {
        "processing_speed": "fast" if brain.getTotalSpikeCount() > 100 else "moderate",
        "memory_efficiency": "high" if brain.getTotalNeuronCount() < 500 else "low",
        "learning_capacity": "high" if brain.getPredictionError() < 0.3 else "low",
        "stability": "stable" if brain.getAverageFiringRate() > 0.1 else "unstable"
    }
    
    # Developmental indicators
    stage = brain.getDevelopmentalStage()
    analysis["developmental_indicators"] = {
        "stage_progress": min(1.0, stage.value / 4.0),
        "critical_period_active": stage == pynlm.DevelopmentalStage.CriticalPeriod,
        "plasticity_level": 1.0 - (stage.value * 0.2),  # Decreasing plasticity
        "maturation_score": stage.value / 4.0
    }
    
    return analysis

def debugSensoryProcessing(agent, world, steps=20):
    """Debug and analyze sensory processing pipeline"""
    debug_info = {
        "vision_analysis": [],
        "sensory_weights": {},
        "processing_latency": [],
        "feature_extraction": {}
    }
    
    # Get initial state
    initial_percept = world.getSensoryPercept()
    
    for step in range(steps):
        step_start = time.time()
        
        # Process sensory input
        agent.processSensoryInput(world.getSensoryPercept())
        
        step_end = time.time()
        debug_info["processing_latency"].append((step_end - step_start) * 1000)  # ms
        
        # Analyze vision
        vision_data = agent.getBrain().getSpikeSystem().getSensoryNeurons()
        debug_info["vision_analysis"].append({
            "step": step,
            "neuron_activity": len(vision_data),
            "average_activity": sum(vision_data) / max(len(vision_data), 1)
        })
        
        # World update
        world.update(0.1)
    
    # Calculate average processing latency
    avg_latency = sum(debug_info["processing_latency"]) / len(debug_info["processing_latency"])
    
    debug_info["sensory_weights"] = {
        "vision": 0.5,
        "touch": 0.2,
        "internal": 0.2,
        "proprioception": 0.1,
        "audio": 0.0
    }
    
    debug_info["feature_extraction"] = {
        "edges_detected": True,
        "colors_analyzed": True,
        "depth_perception": True,
        "motion_detected": True,
        "object_recognition": False  # Simplified
    }
    
    return debug_info

# Use analysis functions
brain_stats = analyzeBrainComplexity(brain)
print(json.dumps(brain_stats, indent=2))

sensory_debug = debugSensoryProcessing(agent, world)
print(f"Average processing latency: {sensory_debug['avg_latency']:.2f}ms")
```

### Advanced Error Handling and Recovery

```python
import pynlm
import traceback

class SafeBrainSimulator:
    def __init__(self, brain, world=None, agent=None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.error_count = 0
        self.recovery_actions = []
        
    def safe_step(self, step, time=0.0, max_retries=3):
        """Safely execute a brain step with error recovery"""
        last_error = None
        
        for attempt in range(max_retries):
            try:
                # Validate brain state
                if not pynlm.validateBrainState(self.brain):
                    self._handleStateError("Invalid brain state")
                    return False
                
                # Validate configuration
                config_errors = pynlm.checkConfigValidity(self.brain.getConfig())
                if config_errors:
                    self._handleConfigError(config_errors)
                    return False
                
                # Execute step
                if time == 0.0:
                    self.brain.step(step)
                else:
                    self.brain.step(step, time)
                
                # World interaction if available
                if self.world and self.agent:
                    self.world.update(0.1)
                    self.agent.processSensoryInput(self.world.getSensoryPercept())
                    
                    action = self.agent.decodeMotorCommand()
                    self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                return True
                
            except Exception as e:
                last_error = e
                self.error_count += 1
                self.recovery_actions.append({
                    "step": step,
                    "attempt": attempt,
                    "error": str(e),
                    "timestamp": time.time()
                })
                
                # Recovery strategies
                if "memory" in str(e).lower():
                    self._recoverMemoryIssue()
                elif "configuration" in str(e).lower():
                    self._recoverConfigIssue()
                elif "neural" in str(e).lower():
                    self._recoverNeuralIssue()
                else:
                    self._recoverGenericIssue()
                
                if attempt == max_retries - 1:
                    print(f"Failed to execute step {step} after {max_retries} attempts: {e}")
                    traceback.print_exc()
                    return False
                
                # Brief pause before retry
                import time
                time.sleep(0.01)
        
        return False
    
    def _handleStateError(self, error_msg):
        print(f"State error: {error_msg}")
        # Attempt recovery
        if self.brain:
            try:
                self.brain.reset()
            except:
                pass
    
    def _handleConfigError(self, errors):
        print(f"Configuration errors: {errors}")
        # Log configuration issues
    
    def _recoverMemoryIssue(self):
        """Recover from memory-related errors"""
        print("Attempting memory recovery...")
        # Force garbage collection
        import gc
        gc.collect()
    
    def _recoverConfigIssue(self):
        """Recover from configuration errors"""
        print("Attempting configuration recovery...")
        # Reset to safe defaults
        if self.agent:
            self.agent.disableAllFeatures()
            self.agent.enableRewardModulation(True)
    
    def _recoverNeuralIssue(self):
        """Recover from neural computation errors"""
        print("Attempting neural recovery...")
        # Reduce neural complexity temporarily
        if self.agent:
            self.agent.enableDevelopment(False)
    
    def _recoverGenericIssue(self):
        """Generic recovery strategy"""
        print("Attempting generic recovery...")
        # Basic reset
        if self.brain:
            try:
                self.brain.reset()
            except:
                pass
    
    def runSafeSimulation(self, steps, timestep=0.1):
        """Run complete simulation with safe stepping"""
        success_count = 0
        failure_count = 0
        
        for step in range(steps):
            if self.safe_step(step, timestep):
                success_count += 1
            else:
                failure_count += 1
        
        return {
            "total_steps": steps,
            "successful_steps": success_count,
            "failed_steps": failure_count,
            "success_rate": success_count / max(steps, 1),
            "error_count": self.error_count,
            "recovery_actions": len(self.recovery_actions)
        }

# Use safe simulator
simulator = SafeBrainSimulator(brain, world, agent)
results = simulator.runSafeSimulation(100)
print(f"Simulation Results: {results}")
```

## Advanced Features and Extensions

### Custom Action and World Types

```python
import pynlm
from typing import Dict, Any, Callable

class AdvancedAction(Action):
    """Extended action class with additional features"""
    
    def __init__(self, action_type: ActionType, parameters: Dict[str, Any] = None):
        super().__init__(action_type, parameters or {})
        self.timestamp = 0.0
        self.reward_prediction = 0.0
        self.success_probability = 1.0
        self.sequences = []  # Action sequences for complex behaviors
    
    def addToSequence(self, action: 'AdvancedAction'):
        """Add action to execution sequence"""
        self.sequences.append(action)
    
    def getSequenceValue(self) -> float:
        """Calculate sequence value based on action synergy"""
        total_value = 0.0
        for action in self.sequences:
            total_value += self._getActionValue(action)
        return total_value / max(len(self.sequences), 1)
    
    def _getActionValue(self, action: 'AdvancedAction') -> float:
        """Calculate individual action value"""
        value_map = {
            ActionType.MoveForward: 1.0,
            ActionType.TurnLeft: 0.8,
            ActionType.TurnRight: 0.8,
            ActionType.Look: 0.6,
            ActionType.Interact: 0.9,
            ActionType.Eat: 0.7,
            ActionType.Wait: 0.3,
            ActionType.Custom: 0.5
        }
        return value_map.get(action.getType(), 0.5)

class DynamicWorld(SimpleWorld):
    """Extended world class with dynamic elements"""
    
    def __init__(self):
        super().__init__()
        self.weather_system = {}
        self.time_of_day = 0.0
        self.dynamic_objects = []
        self.environmental_factors = {}
    
    def updateEnvironmentalFactors(self):
        """Update world environmental factors"""
        # Time of day affects visibility
        visibility_factor = math.cos(self.time_of_day * math.pi / 12.0) * 0.5 + 0.5
        
        # Weather simulation
        if "weather" in self.environmental_factors:
            weather = self.environmental_factors["weather"]
            if weather == "rain":
                visibility_factor *= 0.7
            elif weather == "fog":
                visibility_factor *= 0.5
            elif weather == "clear":
                visibility_factor *= 1.2
        
        # Update vision configuration
        self.configure(self.width_, self.height_, 
                      int(self.visionWidth_ * visibility_factor),
                      int(self.visionHeight_ * visibility_factor))
    
    def addDynamicObject(self, obj: WorldObject, behavior: Callable):
        """Add dynamically behaving object"""
        self.dynamic_objects.append({
            "object": obj,
            "behavior": behavior,
            "last_update": self.getSimulationTime()
        })
    
    def updateDynamicObjects(self):
        """Update dynamic objects based on their behaviors"""
        current_time = self.getSimulationTime()
        
        for dynamic_obj in self.dynamic_objects[:]:  # Copy for safe iteration
            if current_time - dynamic_obj["last_update"] > 1.0:  # Update every second
                behavior = dynamic_obj["behavior"]
                try:
                    new_position = behavior(dynamic_obj["object"], current_time)
                    if new_position:
                        # Remove old object
                        self.objects_.remove(dynamic_obj["object"])
                        # Add updated object
                        new_obj = WorldObject(
                            new_position[0], new_position[1],
                            dynamic_obj["object"].type,
                            dynamic_obj["object"].value,
                            dynamic_obj["object"].radius
                        )
                        self.objects_.append(new_obj)
                        dynamic_obj["last_update"] = current_time
                except Exception as e:
                    print(f"Error updating dynamic object: {e}")
                    # Keep object in place if update fails
    
    def setTimeOfDay(self, hour: float):
        """Set time of day (0-24)"""
        self.time_of_day = hour % 24.0
        self.updateEnvironmentalFactors()
    
    def setWeather(self, weather_type: str, intensity: float = 1.0):
        """Set weather conditions"""
        self.environmental_factors["weather"] = weather_type
        self.updateEnvironmentalFactors()
    
    def getLighting(self) -> float:
        """Get current lighting level based on time and weather"""
        base_lighting = (math.sin(self.time_of_day * math.pi / 12.0) + 1.0) * 0.5
        
        if "weather" in self.environmental_factors:
            weather = self.environmental_factors["weather"]
            if weather == "night":
                base_lighting *= 0.3
            elif weather == "fog":
                base_lighting *= 0.6
            elif weather == "overcast":
                base_lighting *= 0.8
        
        return base_lighting

# Use advanced classes
advanced_world = DynamicWorld()
advanced_world.configure(30, 30, 12, 12)
advanced_world.setTimeOfDay(14.0)  # Afternoon
advanced_world.setWeather("clear", 1.0)

# Add moving object with behavior
def wanderingBehavior(obj: WorldObject, current_time: float):
    """Behavior for wandering object"""
    # Simple wandering: move in random direction
    import random
    new_x = obj.x + random.uniform(-1, 1)
    new_y = obj.y + random.uniform(-1, 1)
    
    # Keep within bounds
    new_x = max(0, min(new_x, 30))
    new_y = max(0, min(new_y, 30))
    
    return (new_x, new_y)

advanced_world.addDynamicObject(
    WorldObject(5, 5, WorldObjectType.Resource, 10.0, 0.5),
    wanderingBehavior
)
```

## Integration and Deployment

### Building and Distribution

```bash
# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install
make install

# Test installation
python3 -c "import pynlm; print('NLM version:', pynlm.getVersion())"

# Run demo
./nlm_phase6_demo
```

### Performance Benchmarking

```python
import pynlm
import time
import json
from typing import Dict, List

class PerformanceBenchmark:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.results = {}
    
    def benchmarkStepPerformance(self, steps: int = 100) -> Dict:
        """Benchmark individual step performance"""
        times = []
        
        for step in range(steps):
            step_start = time.perf_counter()
            
            # Execute one simulation step
            self.world.update(0.1)
            self.agent.processSensoryInput(self.world.getSensoryPercept())
            self.brain.step(step)
            self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(
                self.agent.decodeMotorCommand(), 
                self.world.getSimulationTime()
            )
            
            step_end = time.perf_counter()
            times.append((step_end - step_start) * 1000)  # Convert to ms
        
        return {
            "steps": steps,
            "total_time_ms": sum(times),
            "avg_time_ms": sum(times) / len(times),
            "min_time_ms": min(times),
            "max_time_ms": max(times),
            "std_time_ms": (sum((t - sum(times)/len(times))**2 for t in times) / len(times))**0.5,
            "steps_per_second": steps / (sum(times) / 1000)
        }
    
    def benchmarkMemoryUsage(self, steps: int = 50) -> Dict:
        """Benchmark memory usage over time"""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        memory_samples = []
        
        # Warm-up
        for step in range(10):
            self.safeBenchmarkStep(step)
        
        # Benchmark
        for step in range(steps):
            self.safeBenchmarkStep(step)
            memory_samples.append(process.memory_info().rss / 1024 / 1024)  # MB
        
        return {
            "steps": steps,
            "peak_memory_mb": max(memory_samples),
            "avg_memory_mb": sum(memory_samples) / len(memory_samples),
            "memory_growth_mb": memory_samples[-1] - memory_samples[0],
            "memory_std_mb": (sum((m - sum(memory_samples)/len(memory_samples))**2 for m in memory_samples) / len(memory_samples))**0.5
        }
    
    def safeBenchmarkStep(self, step: int):
        """Safe step execution for benchmarking"""
        try:
            self.world.update(0.1)
            self.agent.processSensoryInput(self.world.getSensoryPercept())
            self.brain.step(step)
            
            # Collect metrics without side effects
            _ = self.brain.getAverageFiringRate()
            _ = self.brain.getTotalSpikeCount()
            _ = self.agent.getCuriosityLevel()
            
        except Exception as e:
            print(f"Benchmark step {step} failed: {e}")
            raise
    
    def runCompleteBenchmark(self) -> Dict:
        """Run complete performance benchmark suite"""
        print("Running performance benchmarks...")
        
        step_results = self.benchmarkStepPerformance(100)
        memory_results = self.benchmarkMemoryUsage(50)
        
        # Functional correctness test
        functional_score = self._benchmarkFunctionalCorrectness()
        
        # Learning performance test
        learning_results = self.benchmarkLearningPerformance()
        
        return {
            "step_performance": step_results,
            "memory_usage": memory_results,
            "functional_correctness": functional_score,
            "learning_performance": learning_results,
            "timestamp": time.time(),
            "system_info": self._getSystemInfo()
        }
    
    def _benchmarkFunctionalCorrectness(self) -> Dict:
        """Test functional correctness of the system"""
        errors = []
        
        # Test basic operations
        if self.brain.getTotalNeuronCount() <= 0:
            errors.append("Invalid neuron count")
        
        if self.brain.getTotalSynapseCount() < 0:
            errors.append("Invalid synapse count")
        
        if self.brain.getAverageFiringRate() < 0 or self.brain.getAverageFiringRate() > 1000:
            errors.append("Invalid firing rate")
        
        # Test developmental stages
        if not hasattr(self.brain.getDevelopmentalStage(), 'value'):
            errors.append("Invalid developmental stage")
        
        return {
            "passed": len(errors) == 0,
            "errors": errors,
            "confidence": 1.0 - (len(errors) * 0.1)
        }
    
    def benchmarkLearningPerformance(self) -> Dict:
        """Benchmark learning and adaptation performance"""
        initial_state = self._getBrainState()
        
        # Run learning phase
        for step in range(200):
            self.world.update(0.1)
            self.agent.processSensoryInput(self.world.getSensoryPercept())
            self.brain.step(step)
            
            # Apply some learning
            if step % 20 == 0:
                reward = 1.0 if step % 40 == 0 else 0.0
                self.agent.applyRewardModulation(reward, 0.0)
        
        final_state = self._getBrainState()
        
        return {
            "initial_neurons": initial_state["neurons"],
            "final_neurons": final_state["neurons"],
            "neuron_growth": final_state["neurons"] - initial_state["neurons"],
            "learning_steps": 200,
            "adaptation_score": self._calculateAdaptationScore(initial_state, final_state)
        }
    
    def _getBrainState(self) -> Dict:
        """Get current brain state"""
        return {
            "neurons": self.brain.getTotalNeuronCount(),
            "spikes": self.brain.getTotalSpikeCount(),
            "firing_rate": self.brain.getAverageFiringRate(),
            "developmental_stage": int(self.brain.getDevelopmentalStage()),
            "neuromodulation": self.agent.getNeuromodulationLevel(),
            "curiosity": self.agent.getCuriosityLevel()
        }
    
    def _calculateAdaptationScore(self, initial: Dict, final: Dict) -> float:
        """Calculate adaptation/learning score"""
        # Simple adaptation formula
        neuron_change = (final["neurons"] - initial["neurons"]) / max(initial["neurons"], 1)
        firing_change = (final["firing_rate"] - initial["firing_rate"]) / max(initial["firing_rate"], 0.01)
        neuromodulation_change = final["neuromodulation"] - initial["neuromodulation"]
        curiosity_change = final["curiosity"] - initial["curiosity"]
        
        return min(1.0, (neuron_change * 0.3 + firing_change * 0.2 + 
                        neuromodulation_change * 0.3 + curiosity_change * 0.2))
    
    def _getSystemInfo(self) -> Dict:
        """Get system information"""
        import platform
        import sys
        
        return {
            "platform": platform.system(),
            "python_version": platform.python_version(),
            "architecture": platform.machine(),
            "processor_count": platform.processor(),
            "available_memory": platform.virtual_memory().total / (1024**3),  # GB
            "cpu_count": platform.cpu_count(),
            "compiler": sys.version.split()[0]
        }

# Run comprehensive benchmark
benchmark = PerformanceBenchmark(brain, world, agent)
results = benchmark.runCompleteBenchmark()

# Save results
with open("benchmark_results.json", "w") as f:
    json.dump(results, f, indent=2, default=str)

print("Benchmark completed. Results saved to benchmark_results.json")
```

## Troubleshooting and Common Issues

### Advanced Troubleshooting

```python
import pynlm
import traceback
import sys

def diagnoseBrainIssues(brain, world=None, agent=None):
    """Diagnose common brain issues and provide solutions"""
    issues = []
    solutions = []
    
    # Check brain initialization
    if not hasattr(brain, '_initialized') or not brain._initialized:
        issues.append("Brain not properly initialized")
        solutions.append("Call brain.initialize() before use")
    
    # Check neuron counts
    neuron_count = brain.getTotalNeuronCount()
    if neuron_count <= 0:
        issues.append("Zero or negative neuron count")
        solutions.append("Check brain configuration and initialization")
    
    if neuron_count > 10000:
        issues.append("Excessive neuron count may cause performance issues")
        solutions.append("Reduce brain size or increase simulation timestep")
    
    # Check synaptic connections
    synapse_count = brain.getTotalSynapseCount()
    if synapse_count < neuron_count * 0.1:  # Less than 10% connectivity
        issues.append("Low synaptic connectivity")
        solutions.append("Enable structural plasticity or increase neuron density")
    
    # Check firing stability
    firing_rate = brain.getAverageFiringRate()
    if firing_rate == 0:
        issues.append("Zero firing rate - brain not active")
        solutions.append("Check sensory input connection or increase neuron excitability")
    
    if firing_rate > 1000:  # Unusually high firing rate
        issues.append("Excessive firing rate - potential instability")
        solutions.append("Reduce neuron parameters or check for excitation/inhibition imbalance")
    
    # Check developmental stage
    stage = brain.getDevelopmentalStage()
    if stage == pynlm.DevelopmentalStage.Aging and neuron_count < 100:
        issues.append("Aging stage with insufficient neurons")
        solutions.append("Start with larger brain or modify developmental parameters")
    
    # Check agent configuration
    if agent:
        if agent.getSensoryInputSize() == 0:
            issues.append("Agent has zero sensory input size")
            solutions.append("Check agent initialization and world configuration")
        
        if agent.getMotorOutputSize() == 0:
            issues.append("Agent has zero motor output size")
            solutions.append("Configure motor neuron groups or brain regions")
    
    # Check world configuration
    if world:
        if world.getWidth() <= 0 or world.getHeight() <= 0:
            issues.append("Invalid world dimensions")
            solutions.append("Set positive world dimensions")
        
        if world.getAgentEnergy() <= 0:
            issues.append("Zero or negative agent energy")
            solutions.append("Check world configuration or increase agent parameters")
    
    return {
        "issues": issues,
        "solutions": solutions,
        "severity": "high" if len(issues) > 3 else "medium" if len(issues) > 1 else "low",
        "recommendations": _generateRecommendations(issues)
    }

def _generateRecommendations(issues):
    """Generate specific recommendations based on issues"""
    recommendations = []
    
    issue_categories = {
        "initialization": ["Call brain.initialize() before use"],
        "neuron_count": ["Check config['num_neurons'] setting", "Increase neuron density"],
        "connectivity": ["Enable structural_plasticity", "Add inter-region connections"],
        "stability": ["Check excitation/inhibition balance", "Add inhibitory neurons"],
        "performance": ["Reduce brain size", "Increase simulation timestep", "Enable SIMD optimizations"]
    }
    
    for issue in issues:
        issue_lower = issue.lower()
        for category, recs in issue_categories.items():
            if category in issue_lower:
                recommendations.extend(recs)
                break
    
    return list(set(recommendations))  # Remove duplicates

def debugPerformance_bottlenecks(brain, world, agent, steps=50):
    """Debug performance bottlenecks in the simulation"""
    bottlenecks = []
    
    # Profile different components
    component_times = {}
    
    # World update
    world_times = []
    for i in range(steps):
        start = time.time()
        world.update(0.1)
        end = time.time()
        world_times.append((end - start) * 1000)
    
    component_times["world_update"] = {
        "avg_ms": sum(world_times) / len(world_times),
        "min_ms": min(world_times),
        "max_ms": max(world_times),
        "std_ms": (sum((t - sum(world_times)/len(world_times))**2 for t in world_times) / len(world_times))**0.5
    }
    
    if component_times["world_update"]["avg_ms"] > 10:  # More than 10ms per update
        bottlenecks.append("World update is slow - consider optimizing collision detection")
    
    # Brain step
    brain_times = []
    for i in range(steps):
        start = time.time()
        brain.step(i)
        end = time.time()
        brain_times.append((end - start) * 1000)
    
    component_times["brain_step"] = {
        "avg_ms": sum(brain_times) / len(brain_times),
        "min_ms": min(brain_times),
        "max_ms": max(brain_times),
        "std_ms": (sum((t - sum(brain_times)/len(brain_times))**2 for t in brain_times) / len(brain_times))**0.5
    }
    
    if component_times["brain_step"]["avg_ms"] > 5:  # More than 5ms per step
        bottlenecks.append("Brain step is slow - consider reducing neuron count or enabling SIMD")
    
    # Agent processing
    agent_times = []
    for i in range(steps):
        start = time.time()
        agent.processSensoryInput(world.getSensoryPercept())
        end = time.time()
        agent_times.append((end - start) * 1000)
    
    component_times["agent_processing"] = {
        "avg_ms": sum(agent_times) / len(agent_times),
        "min_ms": min(agent_times),
        "max_ms": max(agent_times),
        "std_ms": (sum((t - sum(agent_times)/len(agent_times))**2 for t in agent_times) / len(agent_times))**0.5
    }
    
    if component_times["agent_processing"]["avg_ms"] > 2:  # More than 2ms per process
        bottlenecks.append("Agent processing is slow - consider optimizing motor decoding")
    
    # Memory usage
    import psutil
    process = psutil.Process()
    memory_samples = []
    
    for i in range(steps):
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
        memory_samples.append(process.memory_info().rss / 1024 / 1024)  # MB
    
    component_times["memory_usage"] = {
        "peak_mb": max(memory_samples),
        "avg_mb": sum(memory_samples) / len(memory_samples),
        "growth_mb": memory_samples[-1] - memory_samples[0]
    }
    
    if component_times["memory_usage"]["peak_mb"] > 1000:  # More than 1GB
        bottlenecks.append("High memory usage - consider using memory pools")
    
    if component_times["memory_usage"]["growth_mb"] > 100:  # Growing more than 100MB
        bottlenecks.append("Memory leak detected - implement proper cleanup")
    
    return {
        "component_times": component_times,
        "bottlenecks": bottlenecks,
        "optimization_potential": _calculateOptimizationPotential(component_times),
        "recommendations": _getPerformanceRecommendations(bottlenecks)
    }

def _calculateOptimizationPotential(component_times):
    """Calculate optimization potential for each component"""
    potential = {}
    
    for component, times in component_times.items():
        if "avg_ms" in times:
            avg_time = times["avg_ms"]
            if avg_time > 10:
                potential[component] = "high"
            elif avg_time > 5:
                potential[component] = "medium"
            else:
                potential[component] = "low"
    
    return potential

def _getPerformanceRecommendations(bottlenecks):
    """Get performance recommendations based on bottlenecks"""
    recommendations = []
    
    for bottleneck in bottlenecks:
        if "World update" in bottleneck:
            recommendations.extend([
                "Use spatial partitioning for collision detection",
                "Reduce number of dynamic objects",
                "Optimize collision detection algorithms"
            ])
        elif "Brain step" in bottleneck:
            recommendations.extend([
                "Reduce neural population size",
                "Enable SIMD optimizations",
                "Use neural pruning for complex simulations",
                "Implement spike compression"
            ])
        elif "Agent processing" in bottleneck:
            recommendations.extend([
                "Cache sensory processing results",
                "Optimize motor decoding algorithm",
                "Use parallel processing for agent functions"
            ])
        elif "memory" in bottleneck:
            recommendations.extend([
                "Implement custom memory pools",
                "Use weak references for temporary objects",
                "Enable garbage collection optimization"
            ])
    
    return list(set(recommendations))

# Use advanced troubleshooting
print("=== Advanced Brain Diagnosis ===")
diagnosis = diagnoseBrainIssues(brain, world, agent)
print(f"Issues found: {len(diagnosis['issues'])}")
print(f"Severity: {diagnosis['severity']}")
print("Solutions:")
for i, solution in enumerate(diagnosis['solutions'], 1):
    print(f"  {i}. {solution}")

print("\n=== Performance Analysis ===")
performance = debugPerformanceBottlenecks(brain, world, agent)
print("Component Performance:")
for component, times in performance['component_times'].items():
    if "avg_ms" in times:
        print(f"  {component}: {times['avg_ms']:.2f}ms avg")
    elif "peak_mb" in times:
        print(f"  {component}: {times['peak_mb']:.1f}MB peak")

print("\nBottlenecks found:", len(performance['bottlenecks']))
for i, bottleneck in enumerate(performance['bottlenecks'], 1):
    print(f"  {i}. {bottleneck}")
```