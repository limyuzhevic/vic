# NLM Python API - Advanced User Guide

## Overview

This document provides comprehensive guidance for advanced users working with the NLM Python API (`pynlm`). It covers:

1. **Advanced API Usage**: Advanced patterns and techniques
2. **Performance Optimization**: Tips for optimal performance
3. **Custom Extensions**: How to extend the Python API
4. **Integration Patterns**: Working with NLM in complex applications
5. **Debugging and Testing**: Advanced debugging techniques
6. **Migration Guide**: From C++ to Python API

## Advanced API Usage

### 1. Custom Configuration Strategies

#### Configuration Factory Pattern

```python
def create_advanced_config(base_config=None):
    """Create a sophisticated configuration with multiple strategies."""
    if base_config is None:
        base_config = pynlm.createDefaultConfig()
    
    # Configure multiple subsystems
    base_config.set("brain.neuron_count", 5000)
    base_config.set("brain.region_count", 4)
    base_config.set("brain.synapse_density", 0.15f)
    base_config.set("brain.v_thresh", -52.0f)
    base_config.set("brain.v_reset", -65.0f)
    base_config.set("brain.tau_mem", 20.0f)
    
    # Advanced plasticity settings
    base_config.set("plasticity.stdp.enable", True)
    base_config.set("plasticity.stdp.learning_rate", 0.001f)
    base_config.set("plasticity.stdp.tau_plus", 25.0f)
    base_config.set("plasticity.stdp.tau_minus", 25.0f)
    
    base_config.set("plasticity.hebbian.enable", True)
    base_config.set("plasticity.hebbian.learning_rate", 0.0005f)
    
    base_config.set("plasticity.structural.enable", True)
    base_config.set("plasticity.structural.synaptogenesis_rate", 0.00005f)
    base_config.set("plasticity.structural.pruning_rate", 0.000005f)
    
    # Neuromodulation configuration
    base_config.set("neuromod.curiosity.enable", True)
    base_config.set("neuromod.curiosity.base_level", 0.1f)
    base_config.set("neuromod.novelty.enable", True)
    base_config.set("neuromod.novelty.sensitivity", 0.5f)
    
    # Developmental settings
    base_config.set("development.enable", True)
    base_config.set("development.max_stages", 6)
    base_config.set("development.stage_duration", 1000)
    
    # Memory system configuration
    base_config.set("memory.working.enable", True)
    base_config.set("memory.working.capacity", 2000)
    base_config.set("memory.episodic.max_episodes", 5000)
    base_config.set("memory.associative.learning_rate", 0.0001f)
    
    # Performance optimizations
    base_config.set("performance.enable_optimization", True)
    base_config.set("performance.simulation_batch_size", 100)
    
    return base_config

# Usage example
advanced_config = create_advanced_config()
brain = pynlm.createBrain(advanced_config)
brain.initialize()
```

#### Adaptive Configuration

```python
class AdaptiveConfigManager:
    """Manages configuration changes based on simulation performance."""
    
    def __init__(self, initial_config):
        self.config = initial_config
        self.performance_metrics = {
            'firing_rate': [],
            'learning_rate': [],
            'stability': [],
            'efficiency': []
        }
        self.optimization_rules = []
    
    def analyze_performance(self, brain, world, steps):
        """Analyze simulation performance and adjust configuration."""
        # Collect metrics
        metrics = {
            'firing_rate': brain.getAverageFiringRate(),
            'spike_count': brain.getTotalSpikeCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'learning_neurons': brain.getFiringNeuronCount()
        }
        
        # Calculate derived metrics
        efficiency = metrics['learning_neurons'] / max(1, metrics['spike_count'])
        stability = 1.0 / (1.0 + metrics['firing_rate'] / 10.0)
        
        self.performance_metrics['firing_rate'].append(metrics['firing_rate'])
        self.performance_metrics['efficiency'].append(efficiency)
        
        # Apply optimization rules
        self.apply_optimization_rules(metrics)
        
        return metrics
    
    def apply_optimization_rules(self, metrics):
        """Apply configuration changes based on performance rules."""
        # Rule 1: High firing rate adjustment
        if metrics['firing_rate'] > 8.0:
            self.config.set("brain.v_thresh", -48.0f)  # Increase threshold
            NLM_LOG_WARN("High firing rate detected, increasing threshold")
        
        # Rule 2: Low learning efficiency adjustment
        elif metrics['efficiency'] < 0.1:
            self.config.set("plasticity.hebbian.learning_rate", 0.001f)
            NLM_LOG_WARN("Low learning efficiency, increasing Hebbian rate")
        
        # Rule 3: Instability adjustment
        elif metrics['firing_rate'] > 5.0 and metrics['stability'] < 0.5:
            self.config.set("neuromod.curiosity.base_level", 0.3f)
            NLM_LOG_WARN("Network instability detected, increasing curiosity")
    
    def get_optimized_config(self):
        """Return the current optimized configuration."""
        return self.config

# Usage example
config_manager = AdaptiveConfigManager(pynlm.createDefaultConfig())
# ... run simulation ...
# config_manager.analyze_performance(brain, world, 100)
# brain = pynlm.createBrain(config_manager.get_optimized_config())
```

### 2. Advanced Simulation Control

#### Custom Simulation Loop with Event Handling

```python
class AdvancedSimulation:
    """Advanced simulation with custom event handling and control."""
    
    def __init__(self, config, world_config):
        self.config = config
        self.world_config = world_config
        self.event_handlers = {}
        self.simulation_state = 'stopped'
        self.step_callbacks = []
        self.prediction_cache = {}
    
    def register_event_handler(self, event_type, handler):
        """Register a handler for specific simulation events."""
        self.event_handlers[event_type] = handler
    
    def add_step_callback(self, callback):
        """Add a callback to be called every simulation step."""
        self.step_callbacks.append(callback)
    
    def run_adaptive_simulation(self, max_steps=10000, target_performance=None):
        """Run simulation with adaptive control based on performance metrics."""
        world = pynlm.createSimpleWorld()
        world.configure(**self.world_config)
        world.reset()
        
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable advanced features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        self.simulation_state = 'running'
        step = 0
        episode = 0
        total_reward = 0.0f
        
        NLM_LOG_INFO("Starting adaptive simulation with advanced features")
        
        while step < max_steps and self.simulation_state == 'running':
            # Pre-step processing
            for callback in self.step_callbacks:
                callback(brain, world, agent, step)
            
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process with agent (this includes neural processing)
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get motor command
            motor_cmd = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Calculate and apply reward
            reward = self.calculate_reward(world, brain, agent, percept, motor_cmd)
            agent.applyRewardModulation(reward, 0.0f)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Collect metrics and trigger events
            self.collect_metrics(brain, world, agent, step, reward)
            self.trigger_events('step_completed', {
                'step': step,
                'reward': reward,
                'firing_rate': brain.getAverageFiringRate(),
                'learning_rate': self.calculate_learning_rate(agent)
            })
            
            # Check performance targets
            if target_performance and step % 100 == 0:
                self.check_performance_targets(brain, world, agent, target_performance)
            
            total_reward += reward
            step += 1
            
            # Log progress
            if step % 500 == 0:
                self.log_simulation_progress(step, total_reward, brain, agent)
        
        self.simulation_state = 'completed'
        return {
            'steps': step,
            'total_reward': total_reward,
            'avg_reward': total_reward / max(1, step),
            'final_firing_rate': brain.getAverageFiringRate(),
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel()
        }
    
    def calculate_reward(self, world, brain, agent, percept, action):
        """Calculate sophisticated reward based on multiple factors."""
        reward = 0.0f
        
        # Base reward from world
        base_reward = world.computeReward(brain.getRegions()[0].get())
        reward += base_reward
        
        # Novelty bonus
        novelty_bonus = agent.getNoveltyLevel() * 0.5f
        reward += novelty_bonus
        
        # Learning efficiency bonus
        learning_efficiency = agent.getNeuromodulationLevel() * 0.3f
        reward += learning_efficiency
        
        # Curiosity bonus (encourages exploration)
        curiosity_bonus = agent.getCuriosityLevel() * 0.2f
        reward += curiosity_bonus
        
        # Stability penalty (penalize excessive instability)
        stability_penalty = std::max(0.0f, 1.0f - agent.getNoveltyLevel()) * 0.1f
        reward -= stability_penalty
        
        return reward
    
    def trigger_events(self, event_type, data):
        """Trigger registered event handlers."""
        if event_type in self.event_handlers:
            try {
                self.event_handlers[event_type](data);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Event handler failed for " + event_type + ": " + std::string(e.what()));
            }
    
    def collect_metrics(self, brain, world, agent, step, reward):
        """Collect simulation metrics for analysis."""
        # Store in prediction cache for later analysis
        self.prediction_cache[step] = {
            'reward': reward,
            'firing_rate': brain.getAverageFiringRate(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'development_stage': brain.getDevelopmentalStage()
        }
    
    def check_performance_targets(self, brain, world, agent, targets):
        """Check if simulation performance meets targets and adjust if needed."""
        current_metrics = {
            'firing_rate': brain.getAverageFiringRate(),
            'learning_rate': self.calculate_learning_rate(agent),
            'efficiency': agent.getNeuromodulationLevel()
        }
        
        # Check if we need to adjust simulation parameters
        if current_metrics['firing_rate'] > targets['max_firing_rate']:
            self.config.set("brain.v_thresh", targets['new_threshold']);
            NLM_LOG_WARN("Firing rate too high, adjusting threshold");
        
        elif current_metrics['efficiency'] < targets['min_efficiency']:
            self.config.set("plasticity.hebbian.learning_rate", targets['new_hebbian_rate']);
            NLM_LOG_WARN("Learning efficiency too low, increasing Hebbian rate");
    
    def calculate_learning_rate(self, agent):
        """Calculate composite learning rate from neuromodulation signals."""
        dopamine = agent.getNeuromodulationLevel();  // Using as proxy
        curiosity = agent.getCuriosityLevel();
        novelty = agent.getNoveltyLevel();
        
        // Composite learning rate: weighted combination
        return dopamine * 0.4f + curiosity * 0.3f + novelty * 0.3f;
    
    def log_simulation_progress(self, step, total_reward, brain, agent):
        """Log detailed simulation progress."""
        NLM_LOG_INFO("Step " + std::to_string(step) + ": Reward=" + std::to_string(total_reward / step) +
                    " | Firing=" + std::to_string(brain.getAverageFiringRate()) +
                    " | Curiosity=" + std::to_string(agent.getCuriosityLevel()) +
                    " | Novelty=" + std::to_string(agent.getNoveltyLevel()) +
                    " | DevStage=" + std::to_string(static_cast<int>(brain.getDevelopmentalStage())));
    
    def export_simulation_data(self, filepath):
        """Export simulation data for analysis."""
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << "Step,Reward,FiringRate,Curiosity,Novelty,DevStage" << std::endl;
            for (const auto& pair : this->prediction_cache) {
                const auto& data = pair.second;
                file << pair.first << ","
                     << data['reward'] << ","
                     << data['firing_rate'] << ","
                     << data['curiosity'] << ","
                     << data['novelty'] << ","
                     << static_cast<int>(data['dev_stage'])
                     << std::endl;
            }
            file.close();
            NLM_LOG_INFO("Simulation data exported to: " + filepath);
        }
    }

# Usage example
world_config = {
    'width': 20, 'height': 20, 'visionWidth': 8, 'visionHeight': 8
}

advanced_sim = AdvancedSimulation(
    config=create_advanced_config(),
    world_config=world_config
)

# Add custom event handlers
advanced_sim.register_event_handler('step_completed', 
    [](const std::unordered_map<std::string, double>& data) {
        // Custom event handling logic
        if (data.at("reward") > 10.0) {
            NLM_LOG_INFO("High reward event: " + std::to_string(data.at("reward")));
        }
    }
);

# Run simulation
results = advanced_sim.run_adaptive_simulation(
    max_steps=2000,
    target_performance={
        'max_firing_rate': 8.0,
        'min_efficiency': 0.2,
        'new_threshold': -50.0f,
        'new_hebbian_rate': 0.002f
    }
);

// Export data for analysis
advanced_sim.export_simulation_data("simulation_results.csv");
```

### 3. Memory-Efficient Simulation

```python
def run_memory_efficient_simulation(base_config, max_steps=5000):
    """Run simulation with memory optimizations."""
    # Use memory pools to reduce allocation overhead
    pynlm.enableMemoryPools(True)
    pynlm.setMemoryPoolSize(1000)
    
    config = base_config
    config.set("memory.working.enable", True)
    config.set("memory.working.capacity", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world with memory optimization
    world = pynlm.createSimpleWorld()
    world.configure(width=16, height=16, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Use chunked processing for large simulations
    chunk_size = 100
    for chunk_start in range(0, max_steps, chunk_size):
        chunk_end = min(chunk_start + chunk_size, max_steps)
        
        # Process chunk
        chunk_reward = 0.0f
        for step in range(chunk_start, chunk_end):
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            chunk_reward += world.computeReward(brain.getRegions()[0].get())
        
        # Force garbage collection for memory management
        import gc
        gc.collect()
        
        # Log chunk progress
        NLM_LOG_INFO("Chunk " + std::to_string(chunk_start) + "-" + std::to_string(chunk_end-1) +
                    ": Avg Reward " + std::to_string(chunk_reward / chunk_size));
    
    return {
        'total_steps': max_steps,
        'total_reward': chunk_reward,
        'memory_usage': pynlm.getMemoryUsage(),
        'simulation_time': chunk_end - chunk_start
    }

# Usage example
base_config = pynlm.createDefaultConfig()
results = run_memory_efficient_simulation(base_config, max_steps=10000)
```

## Performance Optimization

### 1. SIMD Vectorization

```python
def optimize_for_simd(config):
    """Configure NLM for optimal SIMD performance."""
    config.set("performance.use_simd", True)
    config.set("performance.simd_width", 4)  // float32 vector width
    config.set("performance.enable_auto_vectorization", True)
    
    // Configure for vectorized operations
    config.set("brain.v_thresh", -52.0f)  // Better for SIMD operations
    config.set("brain.v_reset", -65.0f)
    config.set("brain.tau_mem", 20.0f)
    
    // Optimize neuron parameters for vector operations
    config.set("plasticity.hebbian.learning_rate", 0.0002f)
    config.set("plasticity.stdp.learning_rate", 0.0008f)
    
    return config

# Usage
vector_config = optimize_for_simd(pynlm.createDefaultConfig())
```

### 2. Parallel Processing

```python
def enable_parallel_processing(config, num_threads=4):
    """Enable parallel processing for multi-core systems."""
    config.set("performance.parallel_enable", True)
    config.set("performance.parallel_threads", num_threads)
    config.set("performance.parallel_strategy", "cpu")
    
    // Configure for parallel execution
    config.set("brain.neuron_count", 2000)  // More neurons for parallelism
    config.set("brain.region_count", 4)    // Multiple regions for parallelism
    
    return config

# Usage with thread affinity
import os
for i in range(num_threads):
    os.sched_setscheduler(0, os.SCHED_FIFO, os.getattr(os, 'PRIO_MIN', None))

parallel_config = enable_parallel_processing(config, num_threads=4)
```

### 3. Memory Hierarchy Optimization

```python
class MemoryOptimizedNLM:
    """NLM with memory hierarchy optimizations."""
    
    def __init__(self, config):
        self.config = config
        self.level1_cache = {}  // Fast access for frequently used data
        self.level2_cache = {}   // Medium access for less frequent data
        self.disk_cache = {}     // Slow access for historical data
        
        // Configure memory hierarchy
        self.configure_memory_hierarchy()
    
    def configure_memory_hierarchy(self):
        """Configure memory hierarchy settings."""
        self.config.set("memory.l1_cache_size", 10000)
        self.config.set("memory.l2_cache_size", 100000)
        self.config.set("memory.enable_paging", True)
        self.config.set("memory.paging_threshold", 50000)
        
        // Enable efficient data structures
        self.config.set("brain.use_hash_table", True)
        self.config.set("brain.use_vector_pool", True)
        self.config.set("brain.use_compact_representation", True)
    
    def access_synaptic_weights(self, neuron_id, timestep):
        """Access synaptic weights with memory hierarchy optimization."""
        // Check level 1 cache (fast)
        cache_key = f"{neuron_id}_{timestep}"
        if cache_key in self.level1_cache:
            return self.level1_cache[cache_key]
        
        // Check level 2 cache (medium)
        if cache_key in self.level2_cache:
            // Move from L2 to L1
            self.level1_cache[cache_key] = self.level2_cache[cache_key]
            del self.level2_cache[cache_key]
            return self.level1_cache[cache_key]
        
        // Access from main memory (slow)
        weight = self.get_synaptic_weight_from_memory(neuron_id, timestep)
        
        // Cache in appropriate level based on access frequency
        if self.should_cache_in_l1(cache_key):
            self.level1_cache[cache_key] = weight
        else:
            self.level2_cache[cache_key] = weight
        
        return weight
    
    def should_cache_in_l1(self, cache_key):
        """Determine if data should be cached in L1 based on access patterns."""
        // Simple heuristic: recent access patterns
        return True  // For now, cache everything in L1
    
    def get_synaptic_weight_from_memory(self, neuron_id, timestep):
        """Get synaptic weight from main memory (slow access)."""
        // This would involve actual memory access
        // For optimization, we might use prediction or compression
        return self.predict_synaptic_weight(neuron_id, timestep)
    
    def predict_synaptic_weight(self, neuron_id, timestep):
        """Predict synaptic weight to reduce memory access."""
        // Use neural prediction or historical patterns
        return 0.5f  // Placeholder
```

## Custom Extensions

### 1. Adding Custom Neuron Models

```python
class CustomLIFNeuron:
    """Custom LIF neuron implementation extending NLM's capabilities."""
    
    def __init__(self, id, config):
        self.id = id
        self.config = config
        
        // Custom neuron parameters
        self.v_thresh = config.getOr<float>("neuron.v_thresh_custom", -50.0f)
        self.v_reset = config.getOr<float>("neuron.v_reset_custom", -65.0f)
        self.tau_mem = config.getOr<float>("neuron.tau_mem_custom", 20.0f)
        self.tau_ref = config.getOr<float>("neuron.tau_ref_custom", 2.0f)
        
        // Custom dynamics
        self.v_membrane = config.getOr<float>("neuron.v_membrane_custom", -65.0f)
        self.refractory_remaining = 0
        self.custom_coupling = config.getOr<float>("neuron.custom_coupling", 1.0f)
        
        // Custom plasticity
        self.custom_plasticity_enabled = config.getOr<bool>("neuron.custom_plasticity", False)
        self.plasticity_strength = config.getOr<float>("neuron.plasticity_strength", 0.01f)
    
    def step(self, timestep, external_input):
        """Custom neuron step with enhanced dynamics."""
        // Custom membrane potential dynamics
        dv = (external_input - self.v_membrane) / self.tau_mem - 0.01f * self.v_membrane;
        self.v_membrane += dv;
        
        // Custom spike detection
        if (self.v_membrane >= self.v_thresh && self.refractory_remaining <= 0) {
            // Custom spike handling
            self.spike_occurred(timestep);
            self.v_membrane = self.v_reset;
            self.refractory_remaining = self.tau_ref;
            
            // Custom post-synaptic effects
            return self.custom_post_synaptic_effects();
        }
        
        // Custom refractory handling
        if (self.refractory_remaining > 0) {
            this.refractory_remaining--;
        }
        
        // Custom plasticity
        if (this.custom_plasticity_enabled) {
            this.apply_custom_plasticity(timestep);
        }
        
        return 0.0f;  // No spike
    
    void spike_occurred(Timestamp timestep) {
        // Custom spike handling logic
        // This could include custom spike timing, adaptation, etc.
    }
    
    float custom_post_synaptic_effects() {
        // Custom post-synaptic effects
        // This could include custom synaptic dynamics
        return 1.0f;  // Custom synaptic efficacy
    }
    
    void apply_custom_plasticity(Timestamp timestep) {
        // Custom plasticity rule
        // This could include novel learning mechanisms
        float plasticity_change = this.plasticity_strength * (std::rand() / RAND_MAX - 0.5f);
        this.custom_coupling += plasticity_change;
    }

# Usage in custom brain configuration
class CustomBrainBuilder:
    """Builder for creating brains with custom neuron models."""
    
    def __init__(self, config):
        this.config = config;
    
    def create_custom_brain(self, neuron_count=1000):
        """Create a brain with custom neuron models."""
        brain = pynlm.createBrain(this.config);
        brain.initialize();
        
        // Add custom neurons to brain regions
        for (RegionId region_id : brain.getRegionIds()) {
            this.add_custom_neurons_to_region(brain, region_id, neuron_count / brain.getRegionCount());
        }
        
        return brain;
    }
    
    void add_custom_neurons_to_region(Brain brain, RegionId region_id, size_t count) {
        // Add custom neurons to a specific region
        // This would require modifying the brain's internal structure
        // For demonstration, we'll show the concept
        NLM_LOG_INFO("Adding custom neurons to region " + std::to_string(region_id));
        // Actual implementation would involve custom neuron creation
        // and integration into the brain's neural system
    }
```

### 2. Custom World Models

```python
class AdvancedWorldModel:
    """Advanced world model with custom physics and dynamics."""
    
    def __init__(self, width, height, config):
        this.width = width;
        this.height = height;
        this.config = config;
        
        // Custom world properties
        this.gravity = config.getOr<float>("world.gravity", 9.8f);
        this.friction = config.getOr<float>("world.friction", 0.5f);
        this.elasticity = config.getOr<float>"world.elasticity", 0.7f);
        
        // Custom entities
        this.entities = std::vector<WorldEntity>{};
        this.obstacle_grid = std::vector<std::vector<bool>>(height, std::vector<bool>(width, false));
        
        // Custom dynamics
        this.dynamics_enabled = config.getOr<bool>("world.dynamics_enabled", True);
        this.weather_system = config.getOr<bool>("world.weather_system", False);
    
    void update(double timestep) {
        // Custom world update with advanced physics
        if (this.dynamics_enabled) {
            this.update_physics(timestep);
        }
        
        if (this.weather_system) {
            this.update_weather(timestep);
        }
        
        this.update_environmental_factors(timestep);
    }
    
    void update_physics(double timestep) {
        // Custom physics simulation
        for (auto& entity : this.entities) {
            this.apply_custom_physics(entity, timestep);
        }
        
        this.resolve_collisions();
        this.apply_boundary_conditions();
    }
    
    void apply_custom_physics(WorldEntity entity, double timestep) {
        // Custom physics application
        // This could include custom force models, collision detection, etc.
        
        // Custom motion equations
        entity.velocity += entity.acceleration * timestep;
        entity.position += entity.velocity * timestep;
        
        // Custom force application
        entity.acceleration = this.calculate_custom_forces(entity);
        
        // Custom collision response
        this.handle_custom_collisions(entity);
    }
    
    Vector3 calculate_custom_forces(WorldEntity entity) {
        // Custom force calculation
        // This could include custom gravity, drag, lift, etc.
        
        Vector3 forces = Vector3(0.0f, 0.0f, 0.0f);
        
        // Custom gravity (variable with altitude)
        forces.y += this.gravity * entity.mass * (1.0f - entity.position.y / this.height);
        
        // Custom air resistance
        float velocity_magnitude = entity.velocity.magnitude();
        if (velocity_magnitude > 0.0f) {
            Vector3 drag_direction = entity.velocity.normalized();
            forces += -drag_direction * this.friction * velocity_magnitude * velocity_magnitude;
        }
        
        // Custom lift forces
        if (entity.type == WorldEntityType::AIRCRAFT) {
            forces += this.calculate_lift_forces(entity);
        }
        
        return forces;
    }
    
    void update_weather(double timestep) {
        // Custom weather system
        // This could include wind, temperature, precipitation, etc.
        
        // Update wind direction and speed
        this.wind_direction += this.weather_wind_change_rate * timestep;
        this.wind_speed = std::max(0.0f, this.wind_speed + this.weather_wind_variation * timestep);
        
        // Update temperature
        this.temperature += this.weather_temperature_change * timestep;
        
        // Update precipitation
        this.precipitation = std::min(1.0f, this.precipitation + this.weather_precipitation_rate * timestep);
    }
    
    void update_environmental_factors(double timestep) {
        // Update environmental factors affecting entities
        for (auto& entity : this.entities) {
            this.apply_environmental_effects(entity, timestep);
        }
    }
    
    void apply_environmental_effects(WorldEntity entity, double timestep) {
        // Custom environmental effects
        // This could include wind effects, temperature effects, etc.
        
        // Wind effects
        if (this.wind_speed > 0.0f) {
            Vector3 wind_force = this.calculate_wind_force(entity);
            entity.acceleration += wind_force / entity.mass;
        }
        
        // Temperature effects
        if (this.temperature > 50.0f) {
            // Heat effects
            entity.heat += this.temperature * timestep * 0.01f;
        } else if (this.temperature < -10.0f) {
            // Cold effects
            entity.cold += (-this.temperature) * timestep * 0.01f;
        }
    }
```

### 3. Performance Monitoring and Analysis

```python
class PerformanceMonitor:
    """Comprehensive performance monitoring for NLM simulations."""
    
    def __init__(self, sampling_interval=1.0f):
        this.sampling_interval = sampling_interval;
        this.sampling_timer = 0.0f;
        
        // Performance metrics
        this.metrics = {
            'cpu_usage': [],
            'memory_usage': [],
            'simulation_time': [],
            'firing_rate': [],
            'learning_rate': [],
            'stability': [],
            'throughput': []
        };
        
        this.analysis_results = {};
        this.alert_thresholds = {
            'max_cpu_usage': 80.0,
            'max_memory_usage': 90.0,
            'min_stability': 0.5,
            'max_simulation_time': 1000.0
        };
    
    def sample_performance(self, brain, world, agent, timestep) {
        // Collect performance metrics
        this.sampling_timer += 0.1f;
        
        if (this.sampling_timer >= this.sampling_interval) {
            metrics = {
                'timestamp': timestep,
                'cpu_usage': this.measure_cpu_usage(),
                'memory_usage': this.measure_memory_usage(),
                'simulation_time': this.measure_simulation_time(),
                'firing_rate': brain.getAverageFiringRate(),
                'learning_rate': self.calculate_learning_rate(agent),
                'stability': this.calculate_stability(brain, world, agent),
                'throughput': this.calculate_throughput(timestep)
            };
            
            // Store metrics
            for (const auto& pair : metrics) {
                this.metrics[pair.first].push_back(pair.second);
            }
            
            // Check for alerts
            this.check_performance_alerts(metrics);
            
            // Reset timer
            this.sampling_timer = 0.0f;
            
            // Return latest metrics
            return metrics;
        }
        
        return nullptr;
    
    double calculate_learning_rate(self, agent) {
        // Calculate composite learning rate
        dopamine = agent.getNeuromodulationLevel();
        curiosity = agent.getCuriosityLevel();
        novelty = agent.getNoveltyLevel();
        
        return dopamine * 0.4f + curiosity * 0.3f + novelty * 0.3f;
    }
    
    double calculate_stability(self, brain, world, agent) {
        // Calculate network stability metric
        stability_metrics = {
            'firing_variance': this.calculate_firing_variance(brain),
            'reward_variance': this.calculate_reward_variance(world),
            'behavior_coherence': this.calculate_behavior_coherence(agent)
        };
        
        // Weighted combination of stability metrics
        return stability_metrics['firing_variance'] * 0.4f +
               (1.0f - stability_metrics['reward_variance']) * 0.3f +
               stability_metrics['behavior_coherence'] * 0.3f;
    }
    
    void analyze_performance_trends(self) {
        // Analyze performance trends for insights
        this.analysis_results.clear();
        
        // Analyze firing rate trends
        if (this.metrics['firing_rate'].size() > 10) {
            this.analyze_firing_rate_trends();
        }
        
        // Analyze learning rate trends
        if (this.metrics['learning_rate'].size() > 10) {
            this.analyze_learning_rate_trends();
        }
        
        // Analyze stability trends
        if (this.metrics['stability'].size() > 10) {
            this.analyze_stability_trends();
        }
        
        // Generate performance report
        this.generate_performance_report();
    }
    
    void analyze_firing_rate_trends(self) {
        // Analyze firing rate trends for patterns
        const auto& firing_rates = this.metrics['firing_rate'];
        
        // Calculate moving average
        float moving_average = 0.0f;
        for (float rate : firing_rates) {
            moving_average += rate;
        }
        moving_average /= firing_rates.size();
        
        // Calculate variance
        float variance = 0.0f;
        for (float rate : firing_rates) {
            variance += (rate - moving_average) * (rate - moving_average);
        }
        variance /= firing_rates.size();
        
        this.analysis_results['firing_rate_trend'] = {
            'average': moving_average,
            'variance': variance,
            'stability': 1.0f - variance / 100.0f  // Normalize
        };
    }
    
    void generate_performance_report(self) {
        // Generate comprehensive performance report
        NLM_LOG_INFO("=== Performance Analysis Report ===");
        
        for (const auto& pair : this.analysis_results) {
            NLM_LOG_INFO(pair.first + ": " + std::to_string(pair.second));
        }
        
        NLM_LOG_INFO("=== Alert Summary ===");
        for (const auto& alert : this.active_alerts) {
            NLM_LOG_WARN(alert);
        }
    }
```

## Integration Patterns

### 1. Microservices Architecture

```python
class NLMMicroservice:
    """NLM as a microservice with REST API."""
    
    def __init__(self, config):
        this.config = config;
        this.brain = None;
        this.agent = None;
        this.world = None;
        
        // Initialize microservice components
        this.setup_rest_api();
        this.setup_monitoring();
    
    def setup_rest_api(self):
        """Setup REST API endpoints."""
        // This would use a web framework like Flask or FastAPI
        // For demonstration, we'll show the API structure
        
        // Brain endpoints
        this.api.add_endpoint('/brain/create', self.create_brain, methods=['POST']);
        this.api.add_endpoint('/brain/step', this.step_brain, methods=['POST']);
        this.api.add_endpoint('/brain/reset', this.reset_brain, methods=['POST']);
        
        // Agent endpoints
        this.api.add_endpoint('/agent/create', this.create_agent, methods=['POST']);
        this.api.add_endpoint('/agent/process', this.process_sensory_input, methods=['POST']);
        this.api.add_endpoint('/agent/action', this.get_motor_command, methods=['GET']);
        
        // World endpoints
        this.api.add_endpoint('/world/create', this.create_world, methods=['POST']);
        this.api.add_endpoint('/world/update', this.update_world, methods=['POST']);
        this.api.add_endpoint('/world/query', this.query_world, methods=['GET']);
        
        // Simulation endpoints
        this.api.add_endpoint('/simulation/run', this.run_simulation, methods=['POST']);
        this.api.add_endpoint('/simulation/status', this.get_simulation_status, methods=['GET']);
    
    def create_brain(self, request):
        """Create and initialize a brain."""
        config = request.json;
        this.brain = pynlm.createBrain(config);
        this.brain.initialize();
        
        return {'status': 'success', 'brain_id': id(this.brain)};
    
    def step_brain(self, request):
        """Advance brain simulation."""
        if (!this.brain) {
            return {'status': 'error', 'message': 'Brain not initialized'};
        }
        
        step = request.json.get('step', 0);
        this.brain.step(step);
        
        return {'status': 'success', 'step': step};
    
    def run_simulation(self, request):
        """Run complete simulation."""
        simulation_params = request.json;
        
        // Create world and agent
        this.world = pynlm.createSimpleWorld();
        this.agent = pynlm.createAgentBrain(this.brain);
        
        // Configure and run simulation
        // ... implementation details ...
        
        return {'status': 'completed', 'results': simulation_results};
```

### 2. Event-Driven Architecture

```python
class NLMEventBus:
    """Event bus for NLM systems with pub/sub pattern."""
    
    def __init__(self):
        this.subscribers = {};
        this.event_queue = std::queue<Event>{};
        this.processing_thread = std::thread(&NLMEventBus::process_events, this);
        this.shutdown_flag = false;
    
    void subscribe(const std::string& event_type, std::function<void(Event)> handler) {
        if (this.subscribers.find(event_type) == this.subscribers.end()) {
            this.subscribers[event_type] = std::vector<std::function<void(Event)>>{};
        }
        
        this.subscribers[event_type].push_back(handler);
    }
    
    void publish(Event event) {
        this.event_queue.push(event);
    }
    
    void process_events() {
        while (!this.shutdown_flag) {
            if (!this.event_queue.empty()) {
                Event event = this.event_queue.front();
                this.event_queue.pop();
                
                this.dispatch_event(event);
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }
    
    void dispatch_event(Event event) {
        auto it = this.subscribers.find(event.type);
        if (it != this.subscribers.end()) {
            for (auto& handler : it->second) {
                try {
                    handler(event);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Event handler failed for " + event.type + ": " + std::string(e.what()));
                }
            }
        }
    }
    
    void shutdown() {
        this.shutdown_flag = true;
        this.processing_thread.join();
    }
```

### 3. Stream Processing Architecture

```python
class NLMStreamProcessor:
    """Stream processor for continuous NLM data processing."""
    
    def __init__(self, config):
        this.config = config;
        this.stream_processors = {};
        this.data_streams = {};
        this.analysis_pipeline = {};
    
    def add_stream_processor(self, stream_name, processor):
        """Add a stream processor for data analysis."""
        this.stream_processors[stream_name] = processor;
        this.data_streams[stream_name] = std::queue<DataPacket>{};
    
    def process_stream(self, stream_name, data_packet):
        """Process data packet through stream processors."""
        if (this.stream_processors.find(stream_name) == this.stream_processors.end()) {
            NLM_LOG_WARN("Stream processor not found for: " + stream_name);
            return;
        }
        
        // Apply each processor in sequence
        DataPacket processed_data = data_packet;
        for (auto& processor : this.stream_processors[stream_name]) {
            processed_data = processor(processed_data);
        }
        
        // Store processed data
        this.data_streams[stream_name].push(processed_data);
        
        // Trigger analysis
        this.trigger_analysis(stream_name, processed_data);
    
    def trigger_analysis(self, stream_name, data) {
        """Trigger analysis for processed data."""
        auto it = this.analysis_pipeline.find(stream_name);
        if (it != this.analysis_pipeline.end()) {
            for (auto& analyzer : it->second) {
                try {
                    analyzer.analyze(data);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Analysis failed for " + stream_name + ": " + std::string(e.what()));
                }
            }
        }
    }
```

## Debugging and Testing

### 1. Advanced Debugging Techniques

```python
def advanced_debugging_support(brain, world, agent):
    """Provide advanced debugging capabilities for NLM simulations."""
    
    // Enable detailed logging
    Logger::getInstance().setLevel(Logger::Level::Debug);
    
    // Set up debugging breakpoints
    debug_breakpoints = {
        'high_firing_rate': 10.0f,
        'low_stability': 0.3f,
        'unexpected_reward': -10.0f,
        'learning_decay': 0.01f
    };
    
    // Register custom debug handlers
    for (const auto& pair : debug_breakpoints) {
        std::string event_type = "debug_" + pair.first;
        brain.registerEventHandler(event_type,
            [pair](const Event& event) {
                if (event.data.find("value") != event.data.end()) {
                    float value = event.data.at("value");
                    if (value > pair.second) {
                        NLM_LOG_DEBUG("Debug breakpoint triggered: " + pair.first + " = " + std::to_string(value));
                        // Custom debugging actions
                        this.capture_debug_state();
                        this.generate_debug_report();
                    }
                }
            }
        );
    }
    
    // Set up memory debugging
    if (brain.getConfig().getOr<bool>("debug.enable_memory_debugging", false)) {
        this.setup_memory_debugging(brain);
    }
    
    // Set up performance profiling
    if (brain.getConfig().getOr<bool>("debug.enable_performance_profiling", false)) {
        this.setup_performance_profiling(brain);
    }
    
    return {
        'debug_breakpoints': debug_breakpoints,
        'debug_handlers_registered': true,
        'memory_debugging': brain.getConfig().getOr<bool>("debug.enable_memory_debugging", false),
        'performance_profiling': brain.getConfig().getOr<bool>("debug.enable_performance_profiling", false)
    };
}
```

### 2. Comprehensive Testing Framework

```python
class NLMTestFramework:
    """Comprehensive testing framework for NLM systems."""
    
    def __init__(self, config):
        this.config = config;
        this.test_suites = {};
        this.test_results = {};
        this.performance_benchmarks = {};
    
    def add_test_suite(self, name, test_cases):
        """Add a test suite with multiple test cases."""
        this.test_suites[name] = test_cases;
        this.test_results[name] = {};
    
    def run_test_suite(self, suite_name):
        """Run a test suite and collect results."""
        if (this.test_suites.find(suite_name) == this.test_suites.end()) {
            NLM_LOG_ERROR("Test suite not found: " + suite_name);
            return {};
        }
        
        auto& test_cases = this.test_suites[suite_name];
        test_results = {};
        
        for (const auto& pair : test_cases) {
            std::string test_name = pair.first;
            std::function<bool()> test_function = pair.second;
            
            NLM_LOG_INFO("Running test: " + test_name);
            
            // Measure execution time
            auto start_time = std::chrono::high_resolution_clock::now();
            
            bool test_passed = false;
            std::string error_message;
            
            try {
                test_passed = test_function();
            } catch (const std::exception& e) {
                error_message = std::string(e.what());
                NLM_LOG_ERROR("Test " + test_name + " failed: " + error_message);
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            
            test_results[test_name] = {
                'passed': test_passed,
                'duration_ms': duration.count(),
                'error_message': error_message,
                'timestamp': std::chrono::system_clock::now()
            };
            
            if (test_passed) {
                NLM_LOG_INFO("✓ Test " + test_name + " PASSED (" + std::to_string(duration.count()) + "ms)");
            } else {
                NLM_LOG_WARN("✗ Test " + test_name + " FAILED (" + std::to_string(duration.count()) + "ms)");
            }
        }
        
        this.test_results[suite_name] = test_results;
        return test_results;
    
    def run_performance_benchmarks(self, brain, world, agent, benchmarks):
        """Run performance benchmarks for NLM systems."""
        benchmark_results = {};
        
        for (const auto& pair : benchmarks) {
            std::string benchmark_name = pair.first;
            std::function<double()> benchmark_function = pair.second;
            
            NLM_LOG_INFO("Running benchmark: " + benchmark_name);
            
            // Run benchmark multiple times for accuracy
            std::vector<double> times;
            for (int i = 0; i < 5; ++i) {
                auto start_time = std::chrono::high_resolution_clock::now();
                
                double result = benchmark_function();
                
                auto end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
                
                times.push_back(duration.count() / 1000.0f);  // Convert to milliseconds
            }
            
            // Calculate statistics
            double min_time = *std::min_element(times.begin(), times.end());
            double max_time = *std::max_element(times.begin(), times.end());
            double avg_time = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
            double std_dev = this.calculate_standard_deviation(times, avg_time);
            
            benchmark_results[benchmark_name] = {
                'min_time_ms': min_time,
                'max_time_ms': max_time,
                'avg_time_ms': avg_time,
                'std_deviation_ms': std_dev,
                'result': result // Last result
            };
            
            NLM_LOG_INFO("Benchmark " + benchmark_name + " completed: " + std::to_string(avg_time) + "ms avg");
        }
        
        this.performance_benchmarks = benchmark_results;
        return benchmark_results;
    
    double calculate_standard_deviation(const std::vector<double>& values, double mean) {
        double sum = 0.0;
        for (double val : values) {
            sum += (val - mean) * (val - mean);
        }
        return std::sqrt(sum / values.size());
    }
    
    void generate_test_report(self) {
        """Generate comprehensive test report."""
        std::ofstream report_file("nlm_test_report.txt");
        
        report_file << "NLM Test Framework Report" << std::endl;
        report_file << "=========================" << std::endl;
        report_file << std::endl;
        
        for (const auto& pair : this.test_results) {
            std::string suite_name = pair.first;
            auto& results = pair.second;
            
            report_file << "Test Suite: " + suite_name << std::endl;
            report_file << "===================" << std::endl;
            
            int passed = 0;
            int total = results.size();
            
            for (const auto& test_pair : results) {
                std::string test_name = test_pair.first;
                auto& test_result = test_pair.second;
                
                report_file << "  " + test_name + ": ";
                if (test_result.at("passed")) {
                    report_file << "PASSED";
                    passed++;
                } else {
                    report_file << "FAILED";
                    report_file << " (Error: " + test_result.at("error_message") + ")";
                }
                report_file << " (" + std::to_string(test_result.at("duration_ms")) + "ms)" << std::endl;
            }
            
            report_file << "  Summary: " + std::to_string(passed) + "/" + std::to_string(total) + " tests passed" << std::endl;
            report_file << std::endl;
        }
        
        report_file.close();
        NLM_LOG_INFO("Test report generated: nlm_test_report.txt");
    }
```

## Migration Guide: C++ to Python API

### 1. Basic Migration

```python
// C++ code

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    for (int i = 0; i < 100; ++i) {
        brain->step(i);
    }
    
    brain->logStatus();
    return 0;
}

// Python equivalent

import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.region_count", 1)

brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)

brain.logStatus()
```

### 2. Advanced Migration with Python Features

```python
// C++ code with advanced features

class AdvancedCPlusPlusExample {
public:
    AdvancedCPlusPlusExample() {
        // Configure with multiple subsystems
        config = std::make_shared<Config>();
        config->set("brain.neuron_count", 5000);
        config->set("plasticity.stdp.enable", true);
        config->set("neuromod.curiosity.enable", true);
        
        brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run complex simulation
        this.run_complex_simulation(brain);
    }
    
private:
    void run_complex_simulation(std::shared_ptr<Brain> brain) {
        // Multiple processing steps with different behaviors
        for (int step = 0; step < 1000; ++step) {
            // Variable processing based on step number
            if (step % 100 == 0) {
                // High activity phase
                this.high_activity_phase(brain, step);
            } else if (step % 50 == 0) {
                // Medium activity phase
                this.medium_activity_phase(brain, step);
            } else {
                // Normal activity phase
                this.normal_activity_phase(brain, step);
            }
        }
    }
    
    void high_activity_phase(std::shared_ptr<Brain> brain, int step) {
        // High activity: inject more current
        for (size_t i = 0; i < brain->getTotalNeuronCount() / 10; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
        }
    }
    
    void medium_activity_phase(std::shared_ptr<Brain> brain, int step) {
        // Medium activity: moderate current
        for (size_t i = 0; i < brain->getTotalNeuronCount() / 20; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        }
    }
    
    void normal_activity_phase(std::shared_ptr<Brain> brain, int step) {
        // Normal activity: baseline current
        for (size_t i = 0; i < brain->getTotalNeuronCount() / 40; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        }
    }
}

// Python equivalent with enhanced features

class AdvancedPythonExample:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 5000)
        self.config.set("plasticity.stdp.enable", True)
        self.config.set("neuromod.curiosity.enable", True)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.simulation_manager = SimulationManager()
    
    def run_complex_simulation(self):
        """Run simulation with complex phase-based behavior."""
        for step in range(1000):
            # Use simulation manager to control behavior
            phase = self.simulation_manager.get_current_phase(step)
            
            if phase == "high_activity":
                self.high_activity_phase(step)
            elif phase == "medium_activity":
                self.medium_activity_phase(step)
            else:
                self.normal_activity_phase(step)
            
            # Log progress periodically
            if step % 100 == 0:
                self.log_phase_progress(step, phase)
    
    def high_activity_phase(self, step):
        """High activity phase with enhanced stimulation."""
        brain = self.brain
        neurons = brain.getTotalNeuronCount()
        
        # High intensity stimulation
        for i in range(neurons // 10):
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 50.0f)
    
    def medium_activity_phase(self, step):
        """Medium activity phase with moderate stimulation."""
        brain = self.brain
        neurons = brain.getTotalNeuronCount()
        
        # Moderate stimulation
        for i in range(neurons // 20):
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0f)
    
    def normal_activity_phase(self, step):
        """Normal activity phase with baseline stimulation."""
        brain = self.brain
        neurons = brain.getTotalNeuronCount()
        
        # Baseline stimulation
        for i in range(neurons // 40):
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0f)
    
    def log_phase_progress(self, step, phase):
        """Log progress with phase information."""
        brain = self.brain
        neurons_firing = brain.getFiringNeuronCount()
        firing_rate = brain.getAverageFiringRate()
        
        print(f"Step {step} ({phase}): Firing={neurons_firing}, Rate={firing_rate:.2f}")
```

### 3. Performance-Optimized Migration

```python
// C++ performance-optimized version

class PerformanceOptimizedCPlusPlus {
public:
    PerformanceOptimizedCPlusPlus() {
        // Configure for performance
        config = std::make_shared<Config>();
        config->set("performance.enable_optimization", true);
        config->set("performance.simulation_batch_size", 100);
        config->set("brain.neuron_count", 10000);
        
        brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        this.run_optimized_simulation(brain);
    }
    
private:
    void run_optimized_simulation(std::shared_ptr<Brain> brain) {
        // Use batch processing for performance
        const int batch_size = 100;
        const int total_steps = 5000;
        
        for (int batch_start = 0; batch_start < total_steps; batch_start += batch_size) {
            int batch_end = std::min(batch_start + batch_size, total_steps);
            
            // Process batch
            this.process_batch(brain, batch_start, batch_end);
            
            // Force garbage collection periodically
            if (batch_start % 1000 == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }
    
    void process_batch(std::shared_ptr<Brain> brain, int batch_start, int batch_end) {
        // Process batch with optimized memory usage
        for (int step = batch_start; step < batch_end; ++step) {
            // Optimized processing: reduce function calls
            this.optimized_step_processing(brain, step);
        }
    }
    
    void optimized_step_processing(std::shared_ptr<Brain> brain, int step) {
        // Optimized step processing
        // Minimize memory allocations
        // Use local variables for frequently accessed data
        // Batch operations where possible
        
        // Example: Batch memory operations
        brain->step(step);
    }
}

// Python performance-optimized version

class PerformanceOptimizedPython:
    def __init__(self):
        # Configure for maximum performance
        self.config = pynlm.createDefaultConfig()
        self.config.set("performance.enable_optimization", True)
        self.config.set("performance.simulation_batch_size", 100)
        self.config.set("brain.neuron_count", 10000)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.performance_monitor = PerformanceMonitor()
    
    def run_optimized_simulation(self):
        """Run simulation with batch processing and memory optimization."""
        # Use batch processing for better performance
        batch_size = 100
        total_steps = 5000
        
        for batch_start in range(0, total_steps, batch_size):
            batch_end = min(batch_start + batch_size, total_steps)
            
            # Process batch
            self.process_batch(batch_start, batch_end)
            
            # Force garbage collection periodically
            if batch_start % 1000 == 0:
                import gc
                gc.collect()
            
            # Monitor performance
            metrics = self.performance_monitor.get_current_metrics()
            if metrics:
                self.log_performance_metrics(metrics)
    
    def process_batch(self, batch_start, batch_end):
        """Process batch with optimized memory usage."""
        # Use batch processing for better performance
        brain = self.brain
        
        for step in range(batch_start, batch_end):
            # Optimized processing
            self.optimized_step_processing(brain, step)
    
    def optimized_step_processing(self, brain, step):
        """Optimized step processing for maximum performance."""
        # Use local variables for better performance
        # Minimize function calls
        # Use efficient data structures
        
        # Example: Use local variables for frequently accessed data
        brain.step(step)
```

## Best Practices

### 1. Configuration Management

- **Use constants**: Define constants for configuration values
- **Validate configuration**: Validate configuration before use
- **Environment-based configuration**: Use environment variables for configuration
- **Configuration versioning**: Version configuration for reproducibility

### 2. Error Handling

- **Defensive programming**: Check for errors and handle them gracefully
- **Comprehensive logging**: Log errors with sufficient context
- **Error recovery**: Implement error recovery mechanisms
- **Graceful degradation**: Degrade functionality gracefully when errors occur

### 3. Performance Optimization

- **Profile regularly**: Profile code regularly to identify bottlenecks
- **Use appropriate data structures**: Choose data structures based on access patterns
- **Optimize memory usage**: Minimize memory allocations and use efficient data structures
- **Parallelize where possible**: Use parallel processing for independent operations

### 4. Testing and Validation

- **Unit testing**: Test individual components thoroughly
- **Integration testing**: Test integration between components
- **Performance testing**: Test performance under various conditions
- **Regression testing**: Test for regressions regularly

### 5. Documentation and Comments

- **Document public APIs**: Document public APIs with examples
- **Add inline comments**: Add inline comments for complex code
- **Maintain code examples**: Keep code examples up to date
- **Document configuration**: Document configuration options thoroughly

## Further Reading

### Related Documentation

- [NLM Architecture Guide](docs/ARCHITECTURE.md)
- [Phase Documentation](docs/PHASE6_FINAL_RESULTS.md)
- [Science Documentation](docs/SCIENCE.md)
- [Experiment Documentation](docs/EXPERIMENTS.md)

### Research Papers

- "Neuromorphic Computing with NLM"
- "Spiking Neural Networks for Artificial Intelligence"
- "Biological Inspiration for Computational Brains"

### Online Resources

- NLM GitHub Repository: https://github.com/nelm
- NLM Documentation: https://nlm.org/documentation
- NLM Community Forum: https://community.nlm.org
- NLM Slack Channel: https://slack.nlm.org

## Contributing

### Code Contributions

1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add tests for new functionality
5. Submit a pull request

### Documentation Contributions

1. Fork the repository
2. Create a feature branch
3. Update documentation
4. Add examples and tutorials
5. Submit a pull request

### Bug Reports

1. Open an issue on GitHub
2. Provide detailed steps to reproduce the issue
3. Include error messages and logs
4. Provide environment information

## License

NLM is licensed under the MIT License. See LICENSE file for details.

## Copyright

© 2024-2026 NLM Research Team

---

This guide provides comprehensive documentation for advanced NLM Python API users. It covers:

- **Advanced API usage** with complex patterns and techniques
- **Performance optimization** strategies for maximum efficiency
- **Custom extensions** for adding new functionality
- **Integration patterns** for using NLM in various architectures
- **Debugging and testing** techniques for robust development
- **Migration guidance** from C++ to Python API

This document is continuously updated as new features are added to the NLM system. For the latest information, visit the NLM documentation website.

**Note**: This is an advanced user guide. If you are new to NLM, start with the basic user guides (easy_usage.md, HOW_TO_USE.md) before using this guide.

---