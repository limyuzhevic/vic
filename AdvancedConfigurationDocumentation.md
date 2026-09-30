# Advanced Configuration Options and Simulation Modes

This document provides comprehensive information about advanced configuration options and simulation modes available in the NLM framework.

## Advanced Configuration Options

### Configuration Schema

The NLM configuration system has been enhanced with the following advanced options:

```python
import pynlm

# Create advanced configuration with all options
config = pynlm.createDefaultConfig()

# === Advanced Brain Configuration ===

# Network topology and connectivity
config.set("brain.neuron_count", 5000)
config.set("brain.region_count", 6)
config.set("brain.synapse_density", 0.25)
config.set("brain.connection_probability", 0.08)
config.set("brain.initial_weight_mean", 0.5)
config.set("brain.initial_weight_std", 0.15)

# Neuron type distribution
config.set("region.cortex.excitatory_ratio", 0.8)
config.set("region.cortex.inhibitory_ratio", 0.2)
config.set("region.motor.excitatory_ratio", 0.9)
config.set("region.motor.inhibitory_ratio", 0.1)

# === Advanced Plasticity Configuration ===

# STDP (Spike-Timing-Dependent Plasticity) configuration
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.02)
config.set("plasticity.stdp.tau_plus", 30.0)  # Positive window (ms)
config.set("plasticity.stdp.tau_minus", 20.0) # Negative window (ms)
config.set("plasticity.stdp.max_weight", 1.5)
config.set("plasticity.stdp.min_weight", 0.1)
config.set("plasticity.stdp.learning_rate_decay", 0.8)  # Learning rate decay

# Hebbian learning configuration
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.005)
config.set("plasticity.hebbian.baseline_weight", 0.5)
config.set("plasticity.hebbian.max_weight_increase", 0.3)

# Structural plasticity configuration
config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.formation_rate", 0.005)
config.set("plasticity.structural.pruning_threshold", 0.3)
config.set("plasticity.structural.max_connections_per_neuron", 100)
config.set("plasticity.structural.formation_cost", 0.1)

# === Advanced Neuromodulation Configuration ===

# Dopamine system
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.dopamine.baseline", 0.1)
config.set("neuromod.dopamine.peak", 3.0)
config.set("neuromod.dopamine.decay_rate", 0.05)
config.set("neuromod.dopamine.release_rate", 1.0)
config.set("neuromod.dopamine.sensitivity_to_reward", 1.5)

# Novelty detection system
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.sensitivity", 0.9)
config.set("neuromod.novelty.adaptation_rate", 0.95)
config.set("neuromod.novelty.baseline", 0.05)

# Curiosity system
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.base_rate", 0.15)
config.set("neuromod.curiosity.learning_rate", 0.02)
config.set("neuromod.curiosity.exploration_bonus", 0.5)
config.set("neuromod.curiosity.exploration_decay", 0.9)

# Prediction error system
config.set("neuromod.prediction_error.scale", 1.5)
config.set("neuromod.prediction_error.decay_rate", 0.8)
config.set("neuromod.prediction_error.baseline", 0.01)

# === Advanced Development Configuration ===

# Developmental stages
config.set("development.enable", True)
config.set("development.stage_duration", 1000)
config.set("development.maturation_rate", 0.98)
config.set("development.pruning_strength", 0.6)
config.set("development.critical_period_length", 3000)

# Synaptogenesis (synapse formation)
config.set("development.synaptogenesis.enable", True)
config.set("development.synaptogenesis.baseline_rate", 0.01)
config.set("development.synaptogenesis.age_dependency", 0.9)
config.set("development.synaptogenesis.reactivity_dependent", True)

# Pruning (synapse elimination)
config.set("development.pruning.enable", True)
config.set("development.pruning.baseline_rate", 0.005)
config.set("development.pruning.activity_dependence", 0.8)
config.set("development.pruning.weak_connection_threshold", 0.2)

# Maturation (development of adult properties)
config.set("development.maturation.enable", True)
config.set("development.maturation.rate", 0.95)
config.set("development.maturation.threshold_stable", 0.9)

# === Advanced Memory Configuration ===

# Working memory
config.set("memory.working_memory.capacity", 500)
config.set("memory.working_memory.consolidation_rate", 0.9)
config.set("memory.working_memory.decay_rate", 0.95)
config.set("memory.working_memory.interference_strength", 0.3)

# Episodic memory
config.set("memory.episodic_memory.capacity", 200)
config.set("memory.episodic_memory.encoding_strength", 0.85)
config.set("memory.episodic_memory.recall_prob_decay", 0.98)
config.set("memory.episodic_memory.consoolidation_rate", 0.7)

# Semantic memory
config.set("memory.semantic_memory.capacity", 300)
config.set("memory.semantic_memory.association_strength", 0.6)
config.set("memory.semantic_memory.abstract_level", 0.7)

# Procedural memory
config.set("memory.procedural_memory.capacity", 150)
config.set("memory.procedural_memory.strength_decay", 0.9)
config.set("memory.procedural_memory.reinforcement_learning_rate", 0.05)

# === Advanced Prediction Configuration ===

# Prediction system
config.set("prediction.system.enable", True)
config.set("prediction.system.temporal_window", 1000)
config.set("prediction.system.accuracy_threshold", 0.7)
config.set("prediction.system.confidence_threshold", 0.8)

# Action consequence prediction
config.set("prediction.action_consequence.enable", True)
config.set("prediction.action_consequence.prediction_horizon", 500)
config.set("prediction.action_consequence.sampling_rate", 0.02)

# === Advanced Cognitive Configuration ===

# Attention system
config.set("cognition.attention.enable", True)
config.set("cognition.attention.competition_strength", 0.8)
config.set("cognition.attention.saliency_threshold", 0.3)
config.set("cognition.attention.top_down_bias", 0.4)

# Planning system
config.set("cognition.planning.enable", True)
config.set("cognition.planning.horizon_length", 200)
config.set("cognition.planning.exploration_rate", 0.2)
config.set("cognition.planning.exploitation_rate", 0.8)

# === Advanced Simulation Configuration ===

# Time and simulation parameters
config.set("simulation.time_step", 0.1)
config.set("simulation.max_steps", 5000)
config.set("simulation.save_interval", 100)
config.set("simulation.performance_monitoring", True)

# World parameters
config.set("world.width", 50)
config.set("world.height", 50)
config.set("world.vision_width", 8)
config.set("world.vision_height", 8)
config.set("world.max_energy", 100.0)
config.set("world.energy_decay_rate", 0.99)

# === Advanced Agent Configuration ===

# Agent behavior
config.set("agent.exploration_rate", 0.15)
config.set("agent.exploitation_rate", 0.85)
config.set("agent.noise_level", 0.1)
config.set("agent.epsilon", 0.1)

# Reward processing
config.set("agent.reward_scale", 1.0)
config.set("agent.reward_history_length", 100)
config.set("agent.prediction_error_weight", 0.5)

# === Advanced Features ===

# Feature toggles
config.set("features.reward_modulation", True)
config.set("features.structural_plasticity", True)
config.set("features.development", True)
config.set("features.curiosity", True)
config.set("features.novelty_detection", True)
config.set("features.prediction_learning", True)
config.set("features.attention_control", True)
config.set("features.planning_module", True)

# Performance optimization
config.set("performance.optimization_level", 2)
config.set("performance.enable_vectorization", True)
config.set("performance.enable_parallel_processing", True)
config.set("performance.enable_memory_pools", True)

# === Configuration Validation ===

# Validate configuration
def validate_config(config):
    """Validate configuration parameters"""
    errors = []
    
    # Check neuron count
    neuron_count = config.get("brain.neuron_count", 1000)
    if neuron_count < 100 or neuron_count > 100000:
        errors.append(f"Neuron count {neuron_count} is out of valid range [100, 100000]")
    
    # Check region count
    region_count = config.get("brain.region_count", 4)
    if region_count < 1 or region_count > 20:
        errors.append(f"Region count {region_count} is out of valid range [1, 20]")
    
    # Check synapse density
    synapse_density = config.get("brain.synapse_density", 0.1)
    if synapse_density < 0.0 or synapse_density > 1.0:
        errors.append(f"Synapse density {synapse_density} is out of valid range [0.0, 1.0]")
    
    # Check plasticity parameters
    if config.get("plasticity.stdp.enable", True):
        stdp_lr = config.get("plasticity.stdp.learning_rate", 0.01)
        if stdp_lr < 0.0 or stdp_lr > 1.0:
            errors.append(f"STDP learning rate {stdp_lr} is out of valid range [0.0, 1.0]")
    
    # Check neuromodulation parameters
    if config.get("neuromod.curiosity.enable", True):
        curiosity_base = config.get("neuromod.curiosity.base_rate", 0.1)
        if curiosity_base < 0.0 or curiosity_base > 1.0:
            errors.append(f"Curiosity base rate {curiosity_base} is out of valid range [0.0, 1.0]")
    
    return errors

# Configuration presets
class ConfigurationPresets:
    @staticmethod
    def createMinimal():
        """Create minimal configuration"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        config.set("brain.synapse_density", 0.05)
        config.set("plasticity.stdp.enable", False)
        config.set("plasticity.hebbian.enable", False)
        config.set("plasticity.structural.enable", False)
        config.set("development.enable", False)
        config.set("neuromod.curiosity.enable", False)
        config.set("neuromod.novelty.enable", False)
        config.set("memory.working_memory.capacity", 10)
        config.set("simulation.steps_per_episode", 50)
        return config
    
    @staticmethod
    def createBalanced():
        """Create balanced configuration"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.1)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.01)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("development.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("memory.working_memory.capacity", 100)
        config.set("simulation.steps_per_episode", 500)
        return config
    
    @staticmethod
    def createAdvanced():
        """Create advanced configuration"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 5000)
        config.set("brain.synapse_density", 0.2)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.02)
        config.set("plasticity.stdp.tau_plus", 30.0)
        config.set("plasticity.stdp.tau_minus", 20.0)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.005)
        config.set("plasticity.structural.enable", True)
        config.set("plasticity.structural.formation_rate", 0.005)
        config.set("development.enable", True)
        config.set("development.stage_duration", 1000)
        config.set("development.maturation_rate", 0.98)
        config.set("development.pruning_strength", 0.6)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.base_rate", 0.15)
        config.set("neuromod.curiosity.learning_rate", 0.02)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.novelty.sensitivity", 0.9)
        config.set("neuromod.dopamine.scale", 2.0)
        config.set("neuromod.dopamine.baseline", 0.1)
        config.set("neuromod.prediction_error.scale", 1.5)
        config.set("memory.working_memory.capacity", 300)
        config.set("memory.episodic_memory.capacity", 150)
        config.set("simulation.steps_per_episode", 2000)
        config.set("simulation.episodes", 10)
        return config
    
    @staticmethod
    def createResearch():
        """Create research configuration with maximum features"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 10000)
        config.set("brain.synapse_density", 0.15)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.01)
        config.set("plasticity.stdp.tau_plus", 50.0)
        config.set("plasticity.stdp.tau_minus", 30.0)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.001)
        config.set("plasticity.structural.enable", True)
        config.set("plasticity.structural.formation_rate", 0.001)
        config.set("development.enable", True)
        config.set("development.stage_duration", 2000)
        config.set("development.maturation_rate", 0.95)
        config.set("development.pruning_strength", 0.8)
        config.set("development.critical_period_length", 5000)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.base_rate", 0.05)
        config.set("neuromod.curiosity.learning_rate", 0.01)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.novelty.sensitivity", 0.95)
        config.set("neuromod.novelty.decay_rate", 0.99)
        config.set("neuromod.dopamine.scale", 3.0)
        config.set("neuromod.dopamine.baseline", 0.05)
        config.set("neuromod.dopamine.peak", 5.0)
        config.set("neuromod.prediction_error.scale", 2.0)
        config.set("neuromod.prediction_error.decay_rate", 0.9)
        config.set("memory.working_memory.capacity", 1000)
        config.set("memory.episodic_memory.capacity", 500)
        config.set("memory.episodic_memory.encoding_strength", 0.95)
        config.set("simulation.steps_per_episode", 10000)
        config.set("simulation.episodes", 100)
        return config

# Configuration file utilities
class ConfigFileManager:
    @staticmethod
    def save_config(config, filepath):
        """Save configuration to file"""
        config.saveToFile(filepath)
        
    @staticmethod
    def load_config(filepath):
        """Load configuration from file"""
        config = pynlm.createDefaultConfig()
        config.loadFromFile(filepath)
        return config
    
    @staticmethod
    def export_config_json(config, filepath):
        """Export configuration to JSON file"""
        import json
        config_dict = {}
        
        # Convert config to dictionary
        # (Implementation depends on the specific config interface)
        
        with open(filepath, 'w') as f:
            json.dump(config_dict, f, indent=2)
    
    @staticmethod
    def import_config_json(filepath):
        """Import configuration from JSON file"""
        import json
        
        with open(filepath, 'r') as f:
            config_dict = json.load(f)
        
        config = pynlm.createDefaultConfig()
        
        # Apply configuration from dictionary
        # (Implementation depends on the specific config interface)
        
        return config

# Configuration comparison utilities
class ConfigComparator:
    @staticmethod
    def compare_configs(config1, config2, config1_name="Config 1", config2_name="Config 2"):
        """Compare two configurations and return differences"""
        differences = []
        
        # Get all keys from both configs
        keys1 = config1.getKeys()
        keys2 = config2.getKeys()
        
        all_keys = set(keys1) | set(keys2)
        
        for key in sorted(all_keys):
            val1 = config1.get(key)
            val2 = config2.get(key)
            
            if val1 != val2:
                diff = {
                    'key': key,
                    'config1_value': val1,
                    'config2_value': val2,
                    'difference': f"{config1_name}.{key}={val1} != {config2_name}.{key}={val2}"
                }
                differences.append(diff)
        
        return differences
    
    @staticmethod
    def find_config_differences(configs, reference_config_index=0):
        """Find differences between multiple configurations and a reference"""
        if not configs:
            return {}
        
        reference = configs[reference_config_index]
        differences = {}
        
        for i, config in enumerate(configs):
            if i == reference_config_index:
                continue
                
            config_diffs = ConfigComparator.compare_configs(reference, config)
            if config_diffs:
                differences[f"Config_{i}"] = config_diffs
        
        return differences

# Configuration optimization utilities
class ConfigOptimizer:
    @staticmethod
    def optimize_for_performance(config, target_performance=None):
        """Optimize configuration for performance"""
        optimized_config = config
        
        # Adjust neuron count for performance
        neuron_count = optimized_config.get("brain.neuron_count", 1000)
        if neuron_count < 100:
            optimized_config.set("brain.neuron_count", 500)
        elif neuron_count > 10000:
            optimized_config.set("brain.neuron_count", 5000)
        
        # Enable vectorization
        optimized_config.set("performance.enable_vectorization", True)
        
        # Optimize plasticity
        optimized_config.set("plasticity.stdp.learning_rate", 0.01)
        optimized_config.set("plasticity.hebbian.learning_rate", 0.005)
        
        # Optimize neuromodulation
        optimized_config.set("neuromod.curiosity.base_rate", 0.1)
        optimized_config.set("neuromod.novelty.sensitivity", 0.7)
        
        return optimized_config
    
    @staticmethod
    def optimize_for_accuracy(config, target_accuracy=None):
        """Optimize configuration for accuracy"""
        optimized_config = config
        
        # Increase neuron count for accuracy
        neuron_count = optimized_config.get("brain.neuron_count", 1000)
        if neuron_count < 1000:
            optimized_config.set("brain.neuron_count", 2000)
        elif neuron_count < 5000:
            optimized_config.set("brain.neuron_count", 8000)
        
        # Enable all features
        optimized_config.set("plasticity.stdp.enable", True)
        optimized_config.set("plasticity.hebbian.enable", True)
        optimized_config.set("plasticity.structural.enable", True)
        optimized_config.set("development.enable", True)
        optimized_config.set("neuromod.curiosity.enable", True)
        optimized_config.set("neuromod.novelty.enable", True)
        optimized_config.set("neuromod.prediction_error.enable", True)
        
        # Increase memory capacity
        optimized_config.set("memory.working_memory.capacity", 500)
        optimized_config.set("memory.episodic_memory.capacity", 250)
        
        return optimized_config

# Advanced simulation modes
class SimulationModes:
    @staticmethod
    def createControlMode(config):
        """Create control mode simulation (deterministic)"""
        mode_config = config
        mode_config.set("simulation.mode", "control")
        mode_config.set("world.random_seed", 42)
        mode_config.set("brain.random_seed", 12345)
        mode_config.set("simulation.noise_level", 0.0)
        mode_config.set("agent.epsilon", 0.0)
        mode_config.set("agent.noise_level", 0.0)
        return mode_config
    
    @staticmethod
    def createExplorationMode(config):
        """Create exploration mode simulation (high exploration)"""
        mode_config = config
        mode_config.set("simulation.mode", "exploration")
        mode_config.set("agent.epsilon", 0.5)
        mode_config.set("agent.noise_level", 0.2)
        mode_config.set("neuromod.curiosity.base_rate", 0.3)
        mode_config.set("neuromod.novelty.sensitivity", 1.0)
        return mode_config
    
    @staticmethod
    def createExploitationMode(config):
        """Create exploitation mode simulation (focused on learned behavior)"""
        mode_config = config
        mode_config.set("simulation.mode", "exploitation")
        mode_config.set("agent.epsilon", 0.01)
        mode_config.set("agent.noise_level", 0.05)
        mode_config.set("neuromod.curiosity.base_rate", 0.05)
        mode_config.set("neuromod.novelty.sensitivity", 0.3)
        return mode_config
    
    @staticmethod
    def createDevelopmentMode(config):
        """Create development mode simulation (focused on learning)"""
        mode_config = config
        mode_config.set("simulation.mode", "development")
        mode_config.set("development.stage_duration", 2000)
        mode_config.set("development.maturation_rate", 0.99)
        mode_config.set("development.pruning_strength", 0.9)
        mode_config.set("plasticity.structural.formation_rate", 0.01)
        mode_config.set("plasticity.structural.pruning_threshold", 0.5)
        return mode_config
    
    @staticmethod
    def createReinforcementMode(config):
        """Create reinforcement learning mode simulation"""
        mode_config = config
        mode_config.set("simulation.mode", "reinforcement")
        mode_config.set("agent.reward_scale", 2.0)
        mode_config.set("neuromod.prediction_error.scale", 2.0)
        mode_config.set("neuromod.dopamine.scale", 2.5)
        mode_config.set("plasticity.stdp.learning_rate", 0.03)
        mode_config.set("neuromod.curiosity.base_rate", 0.2)
        return mode_config
    
    @staticmethod
    def createTransferMode(config):
        """Create transfer learning mode simulation"""
        mode_config = config
        mode_config.set("simulation.mode", "transfer")
        mode_config.set("neuromod.curiosity.enable", True)
        mode_config.set("neuromod.novelty.enable", True)
        mode_config.set("cognition.planning.exploration_rate", 0.3)
        mode_config.set("cognition.planning.exploitation_rate", 0.7)
        mode_config.set("memory.semantic_memory.abstract_level", 0.8)
        return mode_config

# Advanced simulation runner
class AdvancedSimulationRunner:
    def __init__(self):
        self.results = []
        self.monitoring = False
        
    def run_advanced_simulation(self, config, mode="balanced", num_episodes=10, steps_per_episode=500):
        """Run advanced simulation with monitoring and analysis"""
        
        print(f"Starting advanced simulation in {mode} mode...")
        print(f"Configuration: {config.get('brain.neuron_count', 'default')} neurons, "
              f"{config.get('brain.synapse_density', 'default')} synapse density")
        print(f"Episodes: {num_episodes}, Steps per episode: {steps_per_episode}")
        print("=" * 60)
        
        # Create brain and agent
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(
            width=config.get("world.width", 20),
            height=config.get("world.height", 20),
            visionWidth=config.get("world.visionWidth", 8),
            visionHeight=config.get("world.visionHeight", 8)
        )
        world.reset()
        agent.initialize(world)
        
        # Enable features based on config
        self._enable_features(agent, config)
        
        # Run simulation
        simulation_start = time.time()
        
        for episode in range(num_episodes):
            print(f"\nEpisode {episode + 1}/{num_episodes}:")
            
            # Reset environment
            world.reset()
            agent.reset()
            agent.updateDevelopment(0.1)
            
            # Episode tracking
            episode_start = time.time()
            episode_reward = 0.0
            episode_steps = 0
            max_curiosity = 0.0
            max_novelty = 0.0
            prediction_errors = []
            
            for step in range(steps_per_episode):
                # World update
                world.update(config.get("simulation.time_step", 0.1))
                
                # Sensorimotor loop
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Calculate and apply reward
                reward = self._calculate_advanced_reward(percept, agent, config)
                predicted_reward = agent.getPredictionError()
                agent.applyRewardModulation(reward, predicted_reward)
                
                episode_reward += reward
                max_curiosity = max(max_curiosity, agent.getCuriosityLevel())
                max_novelty = max(max_novelty, agent.getNoveltyLevel())
                prediction_errors.append(agent.getPredictionError())
                
                episode_steps += 1
                
                # Progress monitoring
                if (step + 1) % 100 == 0:
                    elapsed = time.time() - episode_start
                    print(f"  Step {step + 1:4d}: "
                          f"Reward={reward:6.2f}, "
                          f"Total={episode_reward:8.2f}, "
                          f"Curiosity={agent.getCuriosityLevel():5.3f}, "
                          f"Novelty={agent.getNoveltyLevel():5.3f}, "
                          f"Speed={step/elapsed:6.1f} steps/s")
            
            episode_duration = time.time() - episode_start
            
            # Store episode results
            episode_result = {
                'episode': episode + 1,
                'total_reward': episode_reward,
                'steps_completed': episode_steps,
                'duration_seconds': episode_duration,
                'steps_per_second': episode_steps / episode_duration if episode_duration > 0 else 0,
                'max_curiosity': max_curiosity,
                'max_novelty': max_novelty,
                'avg_prediction_error': sum(prediction_errors) / len(prediction_errors) if prediction_errors else 0,
                'avg_firing_rate': brain.getAverageFiringRate(),
                'final_development_stage': brain.getDevelopmentalStage().value,
                'final_plasticity_factor': agent.getNeuromodulationLevel()
            }
            
            self.results.append(episode_result)
            
            print(f"Episode {episode + 1} completed: "
                  f"Total Reward={episode_reward:8.2f}, "
                  f"Avg Reward/Sec={episode_reward/episode_duration:6.2f}, "
                  f"Firing Rate={brain.getAverageFiringRate():6.3f}")
            
            # Save checkpoint if performance is good
            if episode_reward > self._get_best_reward():
                self._save_checkpoint(brain, episode + 1, episode_reward)
        
        total_duration = time.time() - simulation_start
        
        # Print summary
        print("\n" + "=" * 60)
        print("SIMULATION SUMMARY")
        print("=" * 60)
        
        self._print_summary_results()
        
        # Save results
        self._save_results()
        
        print(f"\nAdvanced simulation completed successfully!")
        print(f"Total simulation time: {total_duration:.1f}s")
        print(f"Average episode duration: {total_duration/num_episodes:.1f}s")
        print(f"Results saved to simulation_results.json")
        
        return self.results
    
    def _enable_features(self, agent, config):
        """Enable features based on configuration"""
        if config.get("features.reward_modulation", True):
            agent.enableRewardModulation(True)
        if config.get("features.structural_plasticity", True):
            agent.enableStructuralPlasticity(True)
        if config.get("features.development", True):
            agent.enableDevelopment(True)
        if config.get("features.curiosity", True):
            agent.enableCuriosity(True)
        if config.get("features.novelty_detection", True):
            # Novelty is part of curiosity system
            pass
    
    def _calculate_advanced_reward(self, percept, agent, config):
        """Calculate advanced reward based on multiple factors"""
        reward = 0.0
        
        # Energy reward
        internal = percept.getInternal()
        if internal and len(internal) > 0:
            energy = internal[0]
            reward += energy * config.get("reward.energy_weight", 0.1)
        
        # Novelty reward
        novelty = agent.getNoveltyLevel()
        reward += novelty * config.get("reward.novelty_weight", 0.3)
        
        # Curiosity reward
        curiosity = agent.getCuriosityLevel()
        reward += curiosity * config.get("reward.curiosity_weight", 0.2)
        
        # Prediction error penalty
        pred_error = abs(agent.getPredictionError())
        reward -= pred_error * config.get("reward.prediction_error_penalty", 0.1)
        
        # Success bonus (positive prediction error)
        if agent.getPredictionError() > 0.5:
            reward += 0.5 * config.get("reward.success_bonus", 1.0)
        
        # Exploration bonus
        if curiosity > 0.7:
            reward += 0.3 * config.get("reward.exploration_bonus", 1.0)
        
        return reward
    
    def _get_best_reward(self):
        """Get the best reward achieved so far"""
        if not self.results:
            return 0.0
        return max(r['total_reward'] for r in self.results)
    
    def _save_checkpoint(self, brain, episode, reward):
        """Save brain checkpoint"""
        checkpoint_file = f"checkpoint_ep{episode:04d}_r{reward:06.2f}.bin"
        brain.save(checkpoint_file)
        print(f"  Checkpoint saved: {checkpoint_file}")
    
    def _print_summary_results(self):
        """Print summary of simulation results"""
        if not self.results:
            return
        
        total_reward = sum(r['total_reward'] for r in self.results)
        avg_reward = total_reward / len(self.results)
        avg_firing_rate = sum(r['avg_firing_rate'] for r in self.results) / len(self.results)
        max_curiosity = max(r['max_curiosity'] for r in self.results)
        max_novelty = max(r['max_novelty'] for r in self.results)
        
        print(f"Total Episodes: {len(self.results)}")
        print(f"Total Reward: {total_reward:10.2f}")
        print(f"Average Reward per Episode: {avg_reward:10.3f}")
        print(f"Average Firing Rate: {avg_firing_rate:10.3f}")
        print(f"Max Curiosity Achieved: {max_curiosity:10.3f}")
        print(f"Max Novelty Achieved: {max_novelty:10.3f}")
        print(f"Best Performance: Episode {max(r['episode'] for r in self.results)} "
              f"(Reward: {self._get_best_reward():8.2f})")
        
        # Learning progress analysis
        if len(self.results) > 1:
            early_reward = sum(r['total_reward'] for r in self.results[:len(self.results)//2]) / (len(self.results)//2)
            late_reward = sum(r['total_reward'] for r in self.results[len(self.results)//2:]) / (len(self.results)//2)
            reward_improvement = late_reward - early_reward
            
            print(f"\nLearning Analysis:")
            print(f"  Early Episode Average Reward: {early_reward:8.3f}")
            print(f"  Late Episode Average Reward: {late_reward:8.3f}")
            print(f"  Reward Improvement: {reward_improvement:8.3f}")
            print(f"  {'Improving' if reward_improvement > 0 else 'Declining'} performance")
        
        # Development stage analysis
        final_stage = self.results[-1]['final_development_stage']
        print(f"Final Development Stage: {final_stage}")
        
        # Plasticity factor analysis
        final_plasticity = self.results[-1]['final_plasticity_factor']
        print(f"Final Plasticity Factor: {final_plasticity:8.3f}")
    
    def _save_results(self):
        """Save simulation results to file"""
        import json
        
        results_data = {
            'simulation_parameters': {
                'mode': 'advanced',
                'num_episodes': len(self.results),
                'steps_per_episode': 500
            },
            'results': self.results,
            'summary': {
                'total_reward': sum(r['total_reward'] for r in self.results),
                'avg_reward_per_episode': sum(r['total_reward'] for r in self.results) / len(self.results),
                'avg_firing_rate': sum(r['avg_firing_rate'] for r in self.results) / len(self.results),
                'max_curiosity': max(r['max_curiosity'] for r in self.results),
                'max_novelty': max(r['max_novelty'] for r in self.results)
            }
        }
        
        with open("simulation_results.json", 'w') as f:
            json.dump(results_data, f, indent=2)

# Main function for advanced simulation
if __name__ == "__main__":
    print("=== Advanced NLM Simulation ===\n")
    
    # Create advanced configuration
    config = ConfigurationPresets.createAdvanced()
    
    # Create simulation runner
    runner = AdvancedSimulationRunner()
    
    # Run advanced simulation
    results = runner.run_advanced_simulation(
        config=config,
        mode="advanced",
        num_episodes=5,
        steps_per_episode=500
    )
    
    print("\nAdvanced simulation completed successfully!")