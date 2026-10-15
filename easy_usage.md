# NLM - Advanced Usage Guide for Experienced Users

This guide builds upon the easy usage patterns and provides advanced techniques for getting the most out of NLM.

## Advanced Configuration Management

### Custom Configuration Files

```python
import pynlm
import json

# Create a complex configuration
config = pynlm.createDefaultConfig()

# Configure brain parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("brain.connection_probability", 0.08)
config.set("brain.v_thresh", -45.0)  # Higher threshold for lower excitability
config.set("brain.v_rest", -65.0)
config.set("brain.tau_mem", 25.0)   # Slower membrane time constant

# Configure STDP parameters for specific learning patterns
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.stdp.tau_plus", 30.0)   # Longer LTP duration
config.set("plasticity.stdp.tau_minus", 35.0)  # Longer LTD duration

# Configure Hebbian learning
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.001)

# Configure structural plasticity
config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.synaptogenesis_rate", 0.0002)
config.set("plasticity.structural.pruning_rate", 0.00005)

# Configure neuromodulation
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Configure development
config.set("development.enable", True)
config.set("development.stage_transition_speed", 50.0)

# Save configuration for reuse
config.saveToFile("advanced_config.json")
```

### Environment Configuration

```python
import pynlm

# Create world with complex terrain
world = pynlm.createSimpleWorld()
world.configure(
    width=50,
    height=50,
    visionWidth=16,
    visionHeight=16
)

# Add different types of objects
world.addObject(pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Resource, value=2.0))
world.addObject(pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Resource, value=1.5))
world.addObject(pynlm.WorldObject(20, 20, pynlm.WorldObjectType.Hazard, value=-1.0))
world.addObject(pynlm.WorldObject(40, 40, pynlm.WorldObjectType.Marker, value=0.0))

# Set agent starting position
world.setAgentStart(25.0, 25.0)

# Configure world dynamics
world.setMaxEnergy(1000.0)
world.setRestitution(0.6)
world.setFriction(0.3)

# Configure vision parameters
world.configureVision(
    fov=90.0,                     # Field of view in degrees
    blurAmount=0.1,               # Vision blur for realism
    noiseLevel=0.05,              # Visual noise
    contrast=1.2,                 # Contrast enhancement
    brightness=0.95               # Brightness adjustment
)
```

## Advanced Agent Control

### Custom Action Strategies

```python
import pynlm

def customActionStrategy(agent, brain, world):
    """Advanced action selection strategy with behavioral control."""
    
    # Get basic action from agent
    action = agent.decodeMotorCommand()
    
    # Implement custom strategies based on context
    if action == pynlm.ActionType.MoveForward:
        # Check if moving forward is safe
        percept = world.getSensoryPercept()
        vision = percept.getVision()
        
        # Find forward direction in vision (center of field of view)
        center_x = world.getVisionWidth() // 2
        center_y = world.getVisionHeight() // 2
        
        # Check for hazards in path
        hazard_detected = False
        for y in range(center_y - 2, center_y + 3):
            if y >= 0 and y < world.getVisionHeight():
                for x in range(center_x - 2, center_x + 3):
                    if x >= 0 and x < world.getVisionWidth():
                        if percept.getObjectType(x, y) == pynlm.WorldObjectType.Hazard:
                            hazard_detected = True
                            break
                if hazard_detected:
                    break
        
        # Modify action based on safety
        if hazard_detected:
            # Move to safer position instead
            action = selectSafeAlternative(agent)
    
    # Implement curiosity-driven exploration
    elif action == pynlm.ActionType.Wait:
        # Explore when curiosity is high
        curiosity = agent.getCuriosityLevel()
        if curiosity > 0.7:
            action = exploreRandomly(agent)
    
    # Implement reward-based action modification
    elif action == pynlm.ActionType.Interact:
        # Check if interaction is likely to be beneficial
        percept = world.getSensoryPercept()
        interaction_value = calculateInteractionValue(percept)
        
        if interaction_value < 0.1:  # Low expected benefit
            action = pynlm.ActionType.Wait
    
    return action

def exploreRandomly(agent):
    """Explore randomly when curiosity is high."""
    choices = [
        pynlm.ActionType.MoveForward,
        pynlm.ActionType.MoveBackward,
        pynlm.ActionType.TurnLeft,
        pynlm.ActionType.TurnRight,
        pynlm.ActionType.LookLeft,
        pynlm.ActionType.LookRight
    ]
    
    import random
    return random.choice(choices)

def selectSafeAlternative(agent):
    """Select a safer action when forward movement is risky."""
    # Implement avoidance behavior
    options = [
        pynlm.ActionType.TurnLeft,
        pynlm.ActionType.TurnRight,
        pynlm.ActionType.MoveBackward,
        pynlm.ActionType.LookLeft,
        pynlm.ActionType.LookRight
    ]
    
    # Choose the option with least visible hazards
    percept = world.getSensoryPercept()
    best_option = pynlm.ActionType.Wait
    best_score = float('inf')
    
    for option in options:
        # Simulate action and calculate hazard score
        score = calculateHazardScore(option, percept)
        if score < best_score:
            best_score = score
            best_option = option
    
    return best_option
```

### Memory-Enhanced Decision Making

```python
import pynlm

def memoryEnhancedDecisionMaking(agent, brain, world):
    """Use memory systems to inform action selection."""
    
    # Access working memory
    working_memory = brain.getWorkingMemory()
    if working_memory:
        active_traces = working_memory.getActiveTraces()
        
        # Check for recent successful actions
        successful_actions = []
        for trace in active_traces:
            if trace.effectiveness > 0.7:  # High effectiveness
                successful_actions.append(trace.actionType)
        
        # Bias action selection toward previously successful actions
        if successful_actions:
            recent_success = max(successful_actions, key=lambda x: countSuccesses(x))
            # Increase probability of selecting this action
            modifyActionProbability(agent, recent_success, 1.5)
    
    # Access episodic memory for contextual decisions
    episodic_memory = brain.getEpisodicMemory()
    if episodic_memory:
        recent_episodes = episodic_memory.getRecentEpisodes(10)
        
        # Look for patterns in recent experiences
        for episode in recent_episodes:
            if episode.reward > 0.5:  # Positive reward
                # Record successful action patterns
                recordSuccessfulPattern(episode, agent)
            elif episode.reward < -0.5:  # Negative reward
                # Record unsuccessful action patterns
                recordUnsuccessfulPattern(episode, agent)
    
    # Use predictive memory for future planning
    prediction_system = brain.getPredictionSystem()
    if prediction_system:
        # Make predictions about different actions
        action_predictions = predictActionOutcomes(agent, world)
        
        # Choose action with best predicted outcome
        best_prediction = max(action_predictions, key=lambda x: x.outcome)
        biasAgentActionSelection(agent, best_prediction.action, best_prediction.confidence)
```

## Advanced Learning Experiments

### Custom Learning Metrics

```python
import pynlm

class CustomLearningMetrics:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.metrics_history = []
    
    def calculatePlasticityIndex(self):
        """Calculate a composite measure of network plasticity."""
        # Get various plasticity indicators
        stdp_weight = self.brain.getSTDP().getLTPWeight() if self.brain.getSTDP() else 0.0
        hebbian_weight = self.brain.getHebbian().getLearningRate() if self.brain.getHebbian() else 0.0
        structural_rate = self.brain.getStructuralPlasticity().getSynaptogenesisRate() if self.brain.getStructuralPlasticity() else 0.0
        
        # Composite plasticity index
        plasticity = (stdp_weight * 0.4 + hebbian_weight * 0.3 + structural_rate * 1000.0 * 0.3)
        return plasticity
    
    def calculateNetworkCohesion(self):
        """Measure how well the network is organized."""
        cohesion = 0.0
        regions = self.brain.getRegions()
        
        if len(regions) >= 2:
            for i in range(len(regions)):
                for j in range(i + 1, len(regions)):
                    # Calculate connectivity between regions
                    inter_region_connections = self.brain.getInterRegionConnections()
                    total_connections = 0;
                    
                    for conn in inter_region_connections:
                        if (conn.sourceRegion == i + 1 && conn.targetRegion == j + 1) ||
                           (conn.sourceRegion == j + 1 && conn.targetRegion == i + 1) {
                            total_connections++;
                        }
                    
                    if (i + 1) * (j + 1) > 0 {
                        cohesion += static_cast<float>(total_connections) / static_cast<float>((i + 1) * (j + 1));
                    }
        }
        
        return cohesion / (regions.size() * (regions.size() - 1) / 2.0);
    
    def calculateBehavioralComplexity(self):
        """Measure diversity of behavioral outputs."""
        # Get recent actions from agent
        percept = self.world.getSensoryPercept()
        if percept and percept.getInternal().size() > 0 {
            return percept.getInternal()[0];  // Use novelty as complexity measure
        }
        return 0.0;
    
    def calculateAdaptability(self):
        """Measure how quickly the network adapts to changes."""
        # This would require tracking performance over time
        # For now, return a simple heuristic based on current firing patterns
        firing_rate = self.brain.getAverageFiringRate()
        return firing_rate / 10.0;  // Normalize
    
    def computeAllMetrics(self):
        """Compute all metrics and return them."""
        metrics = {
            'plasticity': self.calculatePlasticityIndex(),
            'cohesion': self.calculateNetworkCohesion(),
            'complexity': self.calculateBehavioralComplexity(),
            'adaptability': self.calculateAdaptability(),
            'spike_count': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'memory_load': self.brain.getWorkingMemory().getActiveTraces() if self.brain.getWorkingMemory() else 0
        };
        
        self.metrics_history.append(metrics);
        return metrics;
    
    def getMetricsTrend(self, metric_name, window=10):
        """Get trend of a metric over recent history."""
        if len(self.metrics_history) < 2) return 0.0;
        
        recent_metrics = self.metrics_history.end();
        std::advance(recent_metrics, -std::min(window, len(self.metrics_history)));
        
        values = [];
        for &metric : recent_metrics) {
            values.push_back(metric[metric_name]);
        }
        
        if (values.size() < 2) return 0.0;
        
        // Calculate linear trend
        double sum_x = 0.0, sum_y = 0.0, sum_xy = 0.0, sum_x2 = 0.0;
        int n = values.size();
        
        for (int i = 0; i < n; ++i) {
            double x = i;
            double y = values[i];
            sum_x += x;
            sum_y += y;
            sum_xy += x * y;
            sum_x2 += x * x;
        }
        
        double slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x);
        return slope;
    }
```

### Advanced Simulation Control

```python
import pynlm
import time
import threading

class AdvancedSimulationController:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.simulation_time = 0.0
        self.frame_time = 0.1
        self.is_running = False
        self.pause_simulation = False
    
    def runEpisode(self, max_steps=1000, target_performance=0.8):
        """Run a simulation episode with adaptive control."""
        start_time = time.time()
        steps = 0;
        
        // Enable learning systems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        // Initial performance baseline
        baseline_performance = self.getPerformanceMetric();
        
        while (steps < max_steps && self.is_running) {
            if (!self.pause_simulation) {
                // Update simulation
                self.world.update(self.frame_time);
                
                // Process sensory input
                percept = self.world.getSensoryPercept();
                self.agent.processSensoryInput(percept);
                
                // Run brain step
                self.brain.step(steps, self.simulation_time);
                
                // Get action and apply it
                action = self.agent.decodeMotorCommand();
                self.world.applyMotorCommand(action, self.simulation_time);
                
                // Calculate performance
                performance = self.getPerformanceMetric();
                
                // Adaptive control based on performance
                if (performance < target_performance * 0.8) {
                    // Performance is low - increase exploration
                    self.agent.enableCuriosity(True);
                    self.agent.setCuriosityLevel(min(1.0, self.agent.getCuriosityLevel() * 1.2));
                } else if (performance > target_performance * 1.2) {
                    // Performance is high - increase exploitation
                    self.agent.enableCuriosity(False);
                    self.agent.setCuriosityLevel(max(0.0, self.agent.getCuriosityLevel() * 0.8));
                }
                
                // Update development
                self.agent.updateDevelopment(self.frame_time);
                
                // Advance time
                self.simulation_time += self.frame_time;
                steps++;
                
                // Log progress periodically
                if (steps % 100 == 0) {
                    self.logProgress(steps, performance, start_time);
                }
            }
            
            // Control frame rate
            this_thread::sleep_for(chrono::milliseconds(int(this.frame_time * 1000)));
        }
        
        // Final performance analysis
        final_performance = this.getPerformanceMetric();
        this.analyzeEpisodeResults(steps, start_time, final_performance, baseline_performance);
        
        return {
            'steps': steps,
            'duration': time.time() - start_time,
            'final_performance': final_performance,
            'performance_change': final_performance - baseline_performance,
            'spike_count': brain.getTotalSpikeCount()
        };
    }
    
    def getPerformanceMetric(self):
        """Calculate a composite performance metric."""
        // Combine multiple metrics for overall performance
        working_memory = brain.getWorkingMemory();
        episodic_memory = brain.getEpisodicMemory();
        
        metrics = {
            'memory_efficiency': (working_memory ? working_memory.getActiveTraces() : 0) / 100.0,
            'memory_cohesion': calculateMemoryCohesion(working_memory, episodic_memory),
            'learning_rate': calculateLearningRate(),
            'behavioral_complexity': calculateBehavioralComplexity(),
            'firing_stability': calculateFiringStability()
        };
        
        // Normalize and combine
        double performance = 0.0;
        double weight_sum = 0.0;
        
        for (const auto& &pair : metrics) {
            double normalized = pair.second;
            performance += normalized * pair.first;  // Weight by importance
            weight_sum += pair.first;
        }
        
        return weight_sum > 0 ? performance / weight_sum : 0.0;
    }
```

## Performance Optimization Tips

### For Large Networks

```python
import pynlm

# Optimize configuration for large networks (1000+ neurons)
large_config = pynlm.createDefaultConfig()

// Reduce synaptic density for large networks
large_config.set("brain.synapse_density", 0.05)  // Half the density
large_config.set("brain.connection_probability", 0.05)

// Optimize neuron parameters for stability
large_config.set("brain.v_thresh", -50.0)     // Higher threshold
large_config.set("brain.tau_mem", 30.0)      // Slower membrane dynamics
large_config.set("brain.tau_ref", 5.0)       // Longer refractory period

// Configure efficient plasticity
large_config.set("plasticity.stdp.enable", True)
large_config.set("plasticity.stdp.learning_rate", 0.001)  // Lower rate
large_config.set("plasticity.structural.enable", False)   // Disable structural plasticity for large networks

// Configure memory systems efficiently
large_config.set("working_memory.capacity", 500)  // Limit working memory
large_config.set("episodic_memory.max_episodes", 100)  // Limit episodic memory

// Create and initialize brain with optimized config
large_brain = pynlm.createBrain(large_config)
large_brain.initialize()
```

### For Small Networks

```python
import pynlm

// Optimize configuration for small networks (10-100 neurons)
small_config = pynlm.createDefaultConfig()

// Higher synaptic density for small networks
large_config.set("brain.synapse_density", 0.3)     // Much higher density
large_config.set("brain.connection_probability", 0.2)

// Faster dynamics for small networks
large_config.set("brain.v_thresh", -55.0)       // Lower threshold
large_config.set("brain.tau_mem", 15.0)        // Faster membrane dynamics
large_config.set("brain.tau_ref", 2.0)         // Shorter refractory period

// Richer plasticity for small networks
large_config.set("plasticity.stdp.enable", True)
large_config.set("plasticity.stdp.learning_rate", 0.005)  // Higher rate
large_config.set("plasticity.structural.enable", True)   // Enable structural plasticity

// Rich memory systems for small networks
large_config.set("working_memory.capacity", 50)   // Reasonable capacity
large_config.set("episodic_memory.max_episodes", 50)   // Moderate memory

// Create and initialize with small config
small_brain = pynlm.createBrain(small_config)
small_brain.initialize()
```

### Memory Management

```python
import pynlm

def optimizeMemoryUsage(brain, world, agent):
    """Optimize memory usage for performance."""
    
    // Access memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    if (working_memory) {
        // Adjust working memory capacity based on network size
        neuron_count = brain.getTotalNeuronCount();
        
        if (neuron_count > 1000) {
            working_memory.setCapacity(neuron_count / 20);  // Smaller capacity for large networks
        } else {
            working_memory.setCapacity(neuron_count / 10);   // Larger capacity for small networks
        }
    }
    
    if (episodic_memory) {
        // Adjust episodic memory based on simulation length
        if (neuron_count > 1000) {
            episodic_memory.setMaxEpisodes(100);  // Less memory for large networks
        } else {
            episodic_memory.setMaxEpisodes(1000);  // More memory for small networks
        }
    }
    
    // Optimize attention system
    attention = brain.getAttention();
    if (attention) {
        // Adjust attention strength based on network complexity
        float attention_strength = neuron_count > 1000 ? 0.3f : 0.7f;
        attention.setInhibitionStrength(attention_strength);
        attention.setExcitationStrength(1.5f - attention_strength);
    }
}
```

## Error Handling and Recovery

### Robust Simulation Loop

```python
import pynlm
import logging

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

def robustSimulationLoop(brain, world, agent, max_steps=1000):
    """Robust simulation loop with error handling and recovery."""
    
    steps = 0;
    errors = 0;
    last_error_step = -1;
    
    // Initialize with error handling
    try {
        brain.initialize();
        agent.initialize(world);
    } catch (const std::exception& e) {
        logger.error(std::string("Initialization failed: ") + e.what());
        return;
    }
    
    while (steps < max_steps) {
        try {
            // Update world
            world.update(0.1);
            
            // Process sensory input
            percept = world.getSensoryPercept();
            if (!percept) {
                throw std::runtime_error("Invalid sensory percept");
            }
            
            agent.processSensoryInput(percept);
            
            // Run brain step with error checking
            brain.step(steps, steps * 0.1);
            
            // Get and apply action
            action = agent.decodeMotorCommand();
            if (action) {
                world.applyMotorCommand(action, steps * 0.1);
            }
            
            // Update development
            agent.updateDevelopment(0.1);
            
            steps++;
            
            // Check for recovery conditions
            if (checkRecoveryConditions(brain, world, agent)) {
                performRecovery(brain, world, agent);
            }
            
        } catch (const std::exception& e) {
            errors++;
            last_error_step = steps;
            logger.warning(std::string("Error at step ") + std::to_string(steps) + ": " + e.what());
            
            // Attempt recovery
            if (!attemptRecovery(brain, world, agent, steps)) {
                logger.error(std::string("Recovery failed. Stopping simulation at step ") + std::to_string(steps));
                break;
            }
        }
    }
    
    logger.info(std::string("Simulation completed: ") + std::to_string(steps) + " steps, " + std::to_string(errors) + " errors");
}

bool checkRecoveryConditions(Brain& brain, World& world, AgentBrain& agent) {
    """Check if recovery is needed."""
    
    // Check for unstable network states
    float firing_rate = brain.getAverageFiringRate();
    if (firing_rate > 100.0) {  // Extremely high firing rate
        return true;
    }
    
    // Check for memory system overload
    WorkingMemory* working_memory = brain.getWorkingMemory();
    if (working_memory && working_memory.getActiveTraces() > working_memory.getCapacity() * 0.9) {
        return true;
    }
    
    // Check for developmental instability
    DevelopmentalStage stage = brain.getDevelopmentalStage();
    if (stage == DevelopmentalStage::Maturation && !isDevelopmentStable(brain)) {
        return true;
    }
    
    return false;
}

void performRecovery(Brain& brain, World& world, AgentBrain& agent) {
    """Perform recovery from unstable state."""
    logger.info("Performing recovery from unstable state...");
    
    // Reset brain to stable state
    brain.reset();
    brain.initialize();
    
    // Reset memory systems
    WorkingMemory* working_memory = brain.getWorkingMemory();
    if (working_memory) {
        working_memory.clear();
    }
    
    EpisodicMemory* episodic_memory = brain.getEpisodicMemory();
    if (episodic_memory) {
        episodic_memory.clear();
    }
    
    // Reset agent state
    agent.reset();
    
    // Reinitialize agent with current world
    agent.initialize(world);
    
    logger.info("Recovery completed successfully.");
}

bool attemptRecovery(Brain& brain, World& world, AgentBrain& agent, int current_step) {
    """Attempt recovery from error."""
    
    // Simple recovery strategy: reset and retry
    try {
        brain.reset();
        brain.initialize();
        
        agent.reset();
        agent.initialize(world);
        
        // Reset world to stable state
        world.reset();
        
        logger.info(std::string("Recovery successful at step ") + std::to_string(current_step));
        return true;
        
    } catch (const std::exception& e) {
        logger.error(std::string("Recovery failed: ") + e.what());
        return false;
    }
}
```

## Advanced Integration Examples

### Complete Agent Environment Setup

```python
import pynlm

def setupAdvancedAgentEnvironment(config_file="configs/advanced.json") {
    """Set up a complete advanced agent environment."""
    
    // Load or create advanced configuration
    config = pynlm.createDefaultConfig();
    
    if (config.loadFromFile(config_file)) {
        std::cout << "Loaded advanced configuration from " << config_file << std::endl;
    } else {
        // Create advanced configuration programmatically
        createAdvancedConfiguration(config);
    }
    
    // Create brain with advanced configuration
    brain = pynlm.createBrain(config);
    brain.initialize();
    
    // Create agent brain interface
    agent = pynlm.createAgentBrain(brain);
    
    // Create complex world
    world = pynlm.createSimpleWorld();
    configureAdvancedWorld(world, config);
    
    // Initialize agent with world
    agent.initialize(world);
    
    // Enable all advanced subsystems
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    agent.enableDevelopment(true);
    agent.enableStructuralPlasticity(true);
    
    // Configure neuromodulation levels
    agent.setNeuromodulationLevel(pynlm.NeuromodulatorType::Dopamine, 0.5);
    agent.setNeuromodulationLevel(pynlm.NeuromodulatorType::Novelty, 0.8);
    
    return {brain, agent, world};
}

def createAdvancedConfiguration(Config& config) {
    """Create advanced configuration programmatically."""
    
    // High-performance brain configuration
    config.set("brain.neuron_count", 2000);
    config.set("brain.synapse_density", 0.12);
    config.set("brain.connection_probability", 0.1);
    config.set("brain.v_thresh", -50.0);
    config.set("brain.v_rest", -65.0);
    config.set("brain.tau_mem", 20.0);
    config.set("brain.tau_ref", 3.0);
    
    // Advanced plasticity configuration
    config.set("plasticity.stdp.enable", true);
    config.set("plasticity.stdp.learning_rate", 0.002);
    config.set("plasticity.stdp.tau_plus", 25.0);
    config.set("plasticity.stdp.tau_minus", 30.0);
    config.set("plasticity.hebbian.enable", true);
    config.set("plasticity.hebbian.learning_rate", 0.001);
    config.set("plasticity.structural.enable", true);
    config.set("plasticity.structural.synaptogenesis_rate", 0.0001);
    config.set("plasticity.structural.pruning_rate", 0.00001);
    
    // Advanced neuromodulation configuration
    config.set("neuromod.dopamine.scale", 1.2);
    config.set("neuromod.curiosity.enable", true);
    config.set("neuromod.curiosity.exploration_rate", 0.3);
    config.set("neuromod.novelty.enable", true);
    config.set("neuromod.novelty.sensitivity", 0.8);
    
    // Advanced development configuration
    config.set("development.enable", true);
    config.set("development.stage_transition_speed", 30.0);
    config.set("development.plasticity_factor", 0.8);
    
    // Advanced memory configuration
    config.set("working_memory.capacity", 500);
    config.set("working_memory.decay_rate", 0.95);
    config.set("episodic_memory.max_episodes", 500);
    config.set("episodic_memory.consolidation_threshold", 0.7);
    
    // Advanced prediction system
    config.set("prediction.system.enable", true);
    config.set("prediction.confidence_threshold", 0.6);
    config.set("prediction.horizon", 10);
    
    // Advanced cognition
    config.set("cognition.planner.enabled", true);
    config.set("cognition.planner.planning_depth", 5);
    config.set("cognition.attention.enabled", true);
    config.set("cognition.attention.inhibition_strength", 0.5);
    config.set("cognition.attention.excitation_strength", 1.5);
}

def configureAdvancedWorld(SimpleWorld& world, const Config& config) {
    """Configure advanced world with complex dynamics."""
    
    // Create complex terrain
    world.configure(
        width=config.getOr<int64_t>("world.width", 100),
        height=config.getOr<int64_t>("world.height", 100),
        visionWidth=config.getOr<int64_t>("world.vision_width", 16),
        visionHeight=config.getOr<int64_t>("world.vision_height", 16)
    );
    
    // Add resources at varying densities
    int resource_density = config.getOr<int64_t>("world.resource_density", 5);
    int hazard_density = config.getOr<int64_t>("world.hazard_density", 3);
    int marker_density = config.getOr<int64_t>("world.marker_density", 2);
    
    // Add resources
    for (int i = 0; i < resource_density; ++i) {
        world.addObject(pynlm.WorldObject(
            rand() % world.getWidth(),
            rand() % world.getHeight(),
            pynlm.WorldObjectType::Resource,
            value=1.0 + (rand() % 10) / 10.0
        ));
    }
    
    // Add hazards
    for (int i = 0; i < hazard_density; ++i) {
        world.addObject(pynlm.WorldObject(
            rand() % world.getWidth(),
            rand() % world.getHeight(),
            pynlm.WorldObjectType::Hazard,
            value=-1.0
        ));
    }
    
    // Add markers
    for (int i = 0; i < marker_density; ++i) {
        world.addObject(pynlm.WorldObject(
            rand() % world.getWidth(),
            rand() % world.getHeight(),
            pynlm.WorldObjectType::Marker,
            value=0.0
        ));
    }
    
    // Configure world dynamics
    world.setMaxEnergy(config.getOr<double>("world.max_energy", 1000.0));
    world.setRestitution(config.getOr<double>("world.restitution", 0.6));
    world.setFriction(config.getOr<double>("world.friction", 0.3));
    
    // Configure vision system
    configureAdvancedVision(world, config);
}

def configureAdvancedVision(SimpleWorld& world, const Config& config) {
    """Configure advanced vision system."""
    
    // Basic vision configuration
    world.configureVision(
        fov=config.getOr<double>("vision.fov", 90.0),
        blurAmount=config.getOr<double>("vision.blur", 0.1),
        noiseLevel=config.getOr<double>("vision.noise", 0.05),
        contrast=config.getOr<double>("vision.contrast", 1.2),
        brightness=config.getOr<double>("vision.brightness", 0.95)
    );
    
    // Advanced vision features
    bool enable_edge_detection = config.getOr<bool>("vision.edge_detection", true);
    bool enable_depth_perception = config.getOr<bool>("vision.depth_perception", true);
    bool enable_color_vision = config.getOr<bool>("vision.color_vision", false);  // Monochrome for simplicity
    
    if (enable_edge_detection) {
        world.enableEdgeDetection(true);
    }
    
    if (enable_depth_perception) {
        world.enableDepthPerception(true);
    }
    
    if (enable_color_vision) {
        world.enableColorVision(true);
    }
}
```

## Performance Monitoring and Analysis

### Real-time Monitoring

```python
import pynlm
import time
import threading

class PerformanceMonitor:
    public:
        PerformanceMonitor(Brain& brain, World& world, AgentBrain& agent) :
            brain_(brain), world_(world), agent_(agent), 
            monitoring_(false), monitor_thread_(nullptr) {}
        
        ~PerformanceMonitor() {
            stopMonitoring();
        }
        
        void startMonitoring(double interval = 1.0) {
            monitoring_ = true;
            monitor_thread_ = new std::thread(&PerformanceMonitor::monitoringLoop, this, interval);
        }
        
        void stopMonitoring() {
            monitoring_ = false;
            if (monitor_thread_) {
                monitor_thread_->join();
                delete monitor_thread_;
                monitor_thread_ = nullptr;
            }
        }
        
        void addPerformanceMetric(const std::string& name, std::function<double()> metric) {
            metrics_[name] = metric;
        }
        
        std::unordered_map<std::string, double> getCurrentMetrics() {
            std::unordered_map<std::string, double> current_metrics;
            for (const auto& &pair : metrics_) {
                current_metrics[pair.first] = pair.second();
            }
            return current_metrics;
        }
        
        void exportMetrics(const std::string& filename) {
            std::ofstream file(filename);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open file for writing: " + filename);
            }
            
            file << "Timestamp,Step,Metrics\n";
            file << std::chrono::system_clock::now().time_since_epoch().count() << ",";
            file << brain_.getCurrentStep() << ",";
            
            auto current_metrics = getCurrentMetrics();
            bool first = true;
            for (const auto& &pair : current_metrics) {
                if (!first) file << ",";
                file << pair.first << ":" << pair.second;
                first = false;
            }
            file << "\n";
        }
    
    private:
        void monitoringLoop(double interval) {
            while (monitoring_) {
                try {
                    // Collect metrics
                    std::unordered_map<std::string, double> metrics = this->getCurrentMetrics();
                    
                    // Log metrics
                    logMetrics(metrics);
                    
                    // Save metrics periodically
                    if (metrics_history_.size() > 100) {
                        exportMetrics("performance_metrics.csv");
                        metrics_history_.clear();
                    }
                    
                    // Check for performance issues
                    checkPerformanceIssues(metrics);
                    
                } catch (const std::exception& e) {
                    logger.error(std::string("Monitoring error: ") + e.what());
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(int(interval * 1000)));
            }
        }
        
        void logMetrics(const std::unordered_map<std::string, double>& metrics) {
            std::stringstream ss;
            ss << "Performance Metrics:";
            for (const auto& &pair : metrics) {
                ss << " " << pair.first << "=" << pair.second << ";";
            }
            logger.info(ss.str());
        }
        
        void checkPerformanceIssues(const std::unordered_map<std::string, double>& metrics) {
            // Check for potential performance issues
            if (metrics.count("firing_rate") && metrics.at("firing_rate") > 50.0) {
                logger.warning("High firing rate detected: " + std::to_string(metrics.at("firing_rate")));
            }
            
            if (metrics.count("memory_load") && metrics.at("memory_load") > 0.8) {
                logger.warning("High memory load detected: " + std::to_string(metrics.at("memory_load")));
            }
            
            if (metrics.count("plasticity") && metrics.at("plasticity") < 0.1) {
                logger.warning("Low plasticity detected: " + std::to_string(metrics.at("plasticity")));
            }
        }
    
    private:
        Brain& brain_;
        World& world_;
        AgentBrain& agent_;
        bool monitoring_;
        std::thread* monitor_thread_;
        std::unordered_map<std::string, std::function<double()>> metrics_;
        std::vector<std::unordered_map<std::string, double>> metrics_history_;
};
```

## Key Takeaways for Advanced Users

1. **Configuration Management**: Use JSON configuration files for complex setups and programmatical configuration for dynamic scenarios.

2. **Performance Optimization**: Choose appropriate configuration based on network size and requirements (small vs large networks).

3. **Memory Management**: Balance memory capacity with network complexity to ensure stable performance.

4. **Error Handling**: Implement robust error handling and recovery mechanisms for production systems.

5. **Monitoring**: Use performance monitoring to track system health and identify issues proactively.

6. **Advanced Learning**: Leverage the integrated memory, neuromodulation, and prediction systems for sophisticated learning behaviors.

7. **Custom Actions**: Implement custom action selection strategies that incorporate memory, prediction, and environmental context.

These advanced techniques enable users to build sophisticated neural systems that can learn, adapt, and interact with complex environments in realistic ways.