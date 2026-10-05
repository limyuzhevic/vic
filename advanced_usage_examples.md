# NLM - Advanced Examples and Power User Guide

This document provides advanced examples and tutorials for experienced NLM users who want to get the most out of the framework.

## 1. Advanced Configuration Management

### Example 1: Dynamic Configuration Loading

```python
import pynlm
import json
import os

def create_advanced_config(config_file=None, base_config=None):
    """Create an advanced configuration with dynamic loading."""
    
    # Start with default configuration
    if base_config is None:
        config = pynlm.createDefaultConfig()
    else:
        config = base_config
    
    # Load environment-specific configuration
    if config_file and os.path.exists(config_file):
        try:
            # Parse JSON configuration
            with open(config_file, 'r') as f:
                env_config = json.load(f)
            
            # Apply environment-specific settings
            for section, settings in env_config.items():
                if section == "brain":
                    if "neuron_count" in settings:
                        config.set("brain.neuron_count", settings["neuron_count"])
                    if "region_count" in settings:
                        config.set("brain.region_count", settings["region_count"])
                    if "connection_probability" in settings:
                        config.set("brain.connection_probability", settings["connection_probability"])
                elif section == "plasticity":
                    if "stdp_learning_rate" in settings:
                        config.set("plasticity.stdp.learning_rate", settings["stdp_learning_rate"])
                    if "hebbian_learning_rate" in settings:
                        config.set("plasticity.hebbian.learning_rate", settings["hebbian_learning_rate"])
                elif section == "neuromodulation":
                    if "dopamine_baseline" in settings:
                        config.set("neuromod.dopamine_baseline", settings["dopamine_baseline"])
                    if "curiosity_scale" in settings:
                        config.set("neuromod.curiosity_scale", settings["curiosity_scale"])
        except Exception as e:
            print(f"Warning: Could not load configuration file: {e}")
    
    # Enable advanced features
    config.set("brain.enable_structural_plasticity", True)
    config.set("brain.enable_development", True)
    config.set("brain.enable_curiosity", True)
    config.set("brain.enable_reward_modulation", True)
    
    return config

# Usage example
advanced_config = create_advanced_config(
    config_file="production_config.json",
    base_config=pynlm.createDefaultConfig()
)

# Create brain with advanced configuration
brain = pynlm.createBrain(advanced_config)
brain.initialize()
```

### Example 2: Configuration Templates

```python
def get_development_stages(config):
    """Get developmental stages and their characteristics."""
    stages = {}
    
    # Stage 1: Initial (0-60s)
    if config.has("development.stage_1"):
        stage1_settings = config.get("development.stage_1")
        stages["Initial"] = {
            "age_range": (0, 60),
            "plasticity_modifier": stage1_settings.get("plasticity_modifier", 1.0),
            "synaptogenesis_rate": stage1_settings.get("synaptogenesis_rate", 0.001),
            "pruning_rate": stage1_settings.get("pruning_rate", 0.0001),
            "learning_rate": stage1_settings.get("learning_rate", 0.01)
        }
    
    # Stage 2: Critical Period (60-300s)
    if config.has("development.stage_2"):
        stage2_settings = config.get("development.stage_2")
        stages["CriticalPeriod"] = {
            "age_range": (60, 300),
            "plasticity_modifier": stage2_settings.get("plasticity_modifier", 0.8),
            "synaptogenesis_rate": stage2_settings.get("synaptogenesis_rate", 0.0005),
            "pruning_rate": stage2_settings.get("pruning_rate", 0.0002),
            "learning_rate": stage2_settings.get("learning_rate", 0.008)
        }
    
    # Stage 3: Maturation (300-900s)
    if config.has("development.stage_3"):
        stage3_settings = config.get("development.stage_3")
        stages["Maturation"] = {
            "age_range": (300, 900),
            "plasticity_modifier": stage3_settings.get("plasticity_modifier", 0.5),
            "synaptogenesis_rate": stage3_settings.get("synaptogenesis_rate", 0.0001),
            "pruning_rate": stage3_settings.get("pruning_rate", 0.0005),
            "learning_rate": stage3_settings.get("learning_rate", 0.005)
        }
    
    # Stage 4: Adult (900+s)
    if config.has("development.stage_4"):
        stage4_settings = config.get("development.stage_4")
        stages["Adult"] = {
            "age_range": (900, float('inf')),
            "plasticity_modifier": stage4_settings.get("plasticity_modifier", 0.2),
            "synaptogenesis_rate": stage4_settings.get("synaptogenesis_rate", 0.00001),
            "pruning_rate": stage4_settings.get("pruning_rate", 0.001),
            "learning_rate": stage4_settings.get("learning_rate", 0.001)
        }
    
    return stages

def apply_developmental_staging(config, stages):
    """Apply developmental staging configuration."""
    for stage_name, stage_settings in stages.items():
        config.set(f"development.{stage_name.lower()}_plasticity_modifier", 
                  stage_settings["plasticity_modifier"])
        config.set(f"development.{stage_name.lower()}_synaptogenesis_rate", 
                  stage_settings["synaptogenesis_rate"])
        config.set(f"development.{stage_name.lower()}_pruning_rate", 
                  stage_settings["pruning_rate"])
        config.set(f"development.{stage_name.lower()}_learning_rate", 
                  stage_settings["learning_rate"])
```

## 2. Advanced Agent Behaviors

### Example 3: Multi-Agent System

```python
import pynlm
import threading
import time
import queue

class MultiAgentSystem:
    """Advanced multi-agent system with inter-agent communication."""
    
    def __init__(self, num_agents=3):
        self.num_agents = num_agents
        self.agents = []
        self.world = pynlm.createSimpleWorld()
        self.message_queue = queue.Queue()
        self.running = False
        
        # Configure world for multi-agent interaction
        self.world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
        
        # Create agents with different configurations
        for i in range(num_agents):
            config = pynlm.createDefaultConfig()
            
            # Modify configuration for different agent types
            if i == 0:
                # Explorer agent
                config.set("brain.neuron_count", 500)
                config.set("brain.enable_curiosity", True)
                config.set("brain.enable_structural_plasticity", True)
            elif i == 1:
                # Social agent
                config.set("brain.neuron_count", 800)
                config.set("brain.enable_reward_modulation", True)
                config.set("brain.enable_development", True)
            else:
                # Predator agent
                config.set("brain.neuron_count", 600)
                config.set("brain.enable_prediction", True)
                config.set("brain.enable_novelty_detection", True)
            
            brain = pynlm.createBrain(config)
            agent = pynlm.createAgentBrain(brain)
            
            self.agents.append({
                'brain': brain,
                'agent': agent,
                'id': i,
                'position': (i * 10, i * 10)
            })
    
    def start_simulation(self, num_steps=1000):
        """Start multi-agent simulation."""
        self.running = True
        
        # Initialize agents
        for agent_info in self.agents:
            agent_info['agent'].initialize(self.world)
        
        # Set initial positions
        for i, agent_info in enumerate(self.agents):
            self.world.setAgentStart(agent_info['position'][0], agent_info['position'][1])
        
        # Run simulation
        for step in range(num_steps):
            if not self.running:
                break
                
            self.world.update(0.1)
            
            # Each agent perceives the world differently
            for agent_info in self.agents:
                self.process_agent_perception(agent_info, step)
                self.update_agent_behavior(agent_info, step)
                self.handle_inter_agent_communication(agent_info, step)
            
            # Check for agent interactions
            self.check_agent_interactions()
            
            if step % 100 == 0:
                print(f"Step {step}/{num_steps}")
        
        self.running = False
    
    def process_agent_perception(self, agent_info, step):
        """Process agent perception with advanced feature extraction."""
        brain = agent_info['brain']
        agent = agent_info['agent']
        
        # Get sensory input
        percept = self.world.getSensoryPercept()
        
        # Apply agent-specific perception processing
        if agent_info['id'] == 0:
            # Explorer: enhanced vision processing
            vision = percept.getVision()
            if vision:
                # Apply edge detection filter
                processed_vision = self.apply_edge_detection(vision)
                # Create enhanced percept
                from pynlm import Vision
                enhanced_vision = Vision(8, 8, 3)
                enhanced_vision.setData(processed_vision)
                percept.setVision(enhanced_vision)
        
        # Process sensory input
        agent.processSensoryInput(percept)
    
    def apply_edge_detection(self, vision_data):
        """Apply edge detection to vision data."""
        # Simple edge detection implementation
        height = len(vision_data)
        if height == 0:
            return vision_data
            
        width = len(vision_data[0])
        processed = [[0.0 for _ in range(width)] for _ in range(height)]
        
        for y in range(1, height - 1):
            for x in range(1, width - 1):
                # Sobel operator approximation
                center = vision_data[y][x]
                left = vision_data[y][x - 1]
                right = vision_data[y][x + 1]
                up = vision_data[y - 1][x]
                down = vision_data[y + 1][x]
                
                # Calculate gradient magnitude
                gradient_x = right - left
                gradient_y = down - up
                magnitude = ((gradient_x ** 2 + gradient_y ** 2) ** 0.5) / 2.0
                
                # Apply non-linear response
                processed[y][x] = min(1.0, magnitude * 2.0)
        
        return processed
    
    def update_agent_behavior(self, agent_info, step):
        """Update agent behavior based on internal state."""
        brain = agent_info['brain']
        agent = agent_info['agent']
        
        # Update brain
        brain.step(step)
        
        # Apply agent-specific behaviors
        if agent_info['id'] == 0:
            # Explorer: curiosity-driven exploration
            curiosity = agent.getCuriosityLevel()
            if curiosity > 0.7:
                # High curiosity leads to more random actions
                self encourage_exploration(agent)
            else:
                # Low curiosity leads to systematic exploration
                self encourage_systematic_exploration(agent)
        
        elif agent_info['id'] == 1:
            # Social agent: imitation learning
            self.apply_imitation_learning(agent, self.agents)
        
        elif agent_info['id'] == 2:
            # Predator: predictive behavior
            self.apply_predictive_behavior(agent, step)
        
        # Get action and apply it
        action = agent.decodeMotorCommand()
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Apply neuromodulation based on experience
        reward = self.calculate_agent_reward(agent_info)
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    def encourage_exploration(self, agent):
        """Encourage exploration behavior when curiosity is high."""
        curiosity = agent.getCuriosityLevel()
        explore_chance = curiosity * 0.3  # 30% max exploration chance
        
        if self.random() < explore_chance:
            # Force exploration action
            from pynlm import MotorCommand
            # This would typically involve injecting current into exploration neurons
            pass
    
    def encourage_systematic_exploration(self, agent):
        """Encourage systematic, non-random exploration."""
        # Use predictive coding for systematic exploration
        brain = agent.getBrain()
        prediction_error = agent.getPredictionError()
        if abs(prediction_error) > 0.5:
            # High prediction error -> explore unknown areas
            self.explore_unknown_areas(agent)
    
    def apply_imitation_learning(self, agent, other_agents):
        """Apply imitation learning from other agents."""
        # Find most successful agent
        best_agent = self.find_most_successful_agent(other_agents)
        if best_agent:
            # Copy successful behaviors
            self.copy_successful_behaviors(agent, best_agent)
    
    def find_most_successful_agent(self, agents):
        """Find the agent with highest fitness."""
        best_agent = None
        best_fitness = -float('inf')
        
        for agent_info in agents:
            brain = agent_info['brain']
            fitness = self.calculate_agent_fitness(agent_info)
            
            if fitness > best_fitness:
                best_fitness = fitness
                best_agent = agent_info
        
        return best_agent
    
    def calculate_agent_fitness(self, agent_info):
        """Calculate agent fitness based on performance metrics."""
        brain = agent_info['brain']
        agent = agent_info['agent']
        
        # Fitness based on firing rate (learning activity)
        firing_rate = brain.getAverageFiringRate()
        
        # Bonus for curiosity (exploration)
        curiosity = agent.getCuriosityLevel()
        fitness = firing_rate * (1.0 + curiosity * 0.5)
        
        # Penalty for low prediction accuracy
        prediction_error = agent.getPredictionError()
        fitness *= (1.0 - abs(prediction_error) * 0.1)
        
        return fitness
    
    def check_agent_interactions(self):
        """Check for interactions between agents."""
        for i, agent1_info in enumerate(self.agents):
            for j, agent2_info in enumerate(self.agents):
                if i != j:
                    self.handle_agent_interaction(agent1_info, agent2_info)
    
    def handle_agent_interaction(self, agent1_info, agent2_info):
        """Handle interaction between two agents."""
        # Check for proximity
        pos1 = self.get_agent_position(agent1_info)
        pos2 = self.get_agent_position(agent2_info)
        
        distance = ((pos1[0] - pos2[0]) ** 2 + (pos1[1] - pos2[1]) ** 2) ** 0.5
        
        if distance < 2.0:
            # Agents are close - social interaction
            self.trigger_social_interaction(agent1_info, agent2_info)
        
        # Check for collision
        if self.check_collision(agent1_info, agent2_info):
            self.handle_collision(agent1_info, agent2_info)
    
    def trigger_social_interaction(self, agent1_info, agent2_info):
        """Trigger social interaction between agents."""
        brain1 = agent1_info['brain']
        brain2 = agent2_info['brain']
        
        # Social learning: agent1 observes agent2
        self.observe_agent(agent1_info, agent2_info)
        
        # Social reinforcement: agent2 rewards agent1
        self.reward_for_social_behavior(agent1_info, agent2_info)
    
    def observe_agent(self, observing_agent, observed_agent):
        """Observe another agent's behavior."""
        observed_brain = observed_agent['brain']
        
        # Copy neural patterns (simplified imitation)
        # In practice, this would involve more sophisticated pattern matching
        observing_agent['brain'].set("imitation_copy_rate", 0.1)
    
    def reward_for_social_behavior(self, rewarded_agent, rewarding_agent):
        """Reward agent for social behavior."""
        # Transfer learning bonus
        reward = 0.1  # Small social learning bonus
        rewarding_agent['agent'].applyRewardModulation(reward, 0.0)
    
    def get_agent_position(self, agent_info):
        """Get agent position from world."""
        body = self.world.getAgentBody()
        return (body.x, body.y)
    
    def check_collision(self, agent1_info, agent2_info):
        """Check if two agents have collided."""
        pos1 = self.get_agent_position(agent1_info)
        pos2 = self.get_agent_position(agent2_info)
        
        distance = ((pos1[0] - pos2[0]) ** 2 + (pos1[1] - pos2[1]) ** 2) ** 0.5
        return distance < 1.5
    
    def handle_collision(self, agent1_info, agent2_info):
        """Handle collision between agents."""
        # Resolve collision
        pos1 = self.get_agent_position(agent1_info)
        pos2 = self.get_agent_position(agent2_info)
        
        # Simple repulsion
        distance = ((pos1[0] - pos2[0]) ** 2 + (pos1[1] - pos2[1]) ** 2) ** 0.5
        if distance < 0.1:
            # Push agents apart
            dx = (pos1[0] - pos2[0]) / distance
            dy = (pos1[1] - pos2[1]) / distance
            
            # Move agents apart
            agent1_info['position'] = (pos1[0] + dx * 0.1, pos1[1] + dy * 0.1)
            agent2_info['position'] = (pos2[0] - dx * 0.1, pos2[1] - dy * 0.1)
    
    def calculate_agent_reward(self, agent_info):
        """Calculate reward for agent based on performance."""
        # Multiple reward components
        brain = agent_info['brain']
        agent = agent_info['agent']
        
        # Activity-based reward
        firing_rate = brain.getAverageFiringRate()
        activity_reward = min(firing_rate * 10.0, 1.0)
        
        # Novelty-based reward
        novelty = agent.getNoveltyLevel()
        novelty_reward = novelty * 0.5
        
        # Prediction accuracy reward
        prediction_error = agent.getPredictionError()
        accuracy_reward = max(0.0, 1.0 - abs(prediction_error))
        
        # Curiosity reward
        curiosity = agent.getCuriosityLevel()
        curiosity_reward = curiosity * 0.3
        
        # Total reward
        total_reward = activity_reward + novelty_reward + accuracy_reward + curiosity_reward
        
        return total_reward
    
    def random(self):
        """Generate random number between 0 and 1."""
        import random
        return random.random()

# Usage example
if __name__ == "__main__":
    # Create and run multi-agent system
    multi_agent = MultiAgentSystem(num_agents=3)
    print("Starting multi-agent simulation...")
    multi_agent.start_simulation(num_steps=500)
    print("Multi-agent simulation completed!")
```

## 3. Performance Optimization Techniques

### Example 4: Memory-Optimized Brain Configuration

```python
def optimize_brain_memory(config, available_memory_mb=512):
    """Optimize brain configuration for memory constraints."""
    
    # Estimate memory usage
    estimated_memory = estimate_brain_memory(config)
    
    if estimated_memory > available_memory_mb:
        # Apply memory optimizations
        
        # 1. Reduce neuron count if necessary
        current_neuron_count = config.getInt("brain.neuron_count")
        if current_neuron_count > available_memory_mb * 10:  # Rough estimate
            new_neuron_count = available_memory_mb * 8
            config.set("brain.neuron_count", new_neuron_count)
        
        # 2. Reduce region count
        current_region_count = config.getInt("brain.region_count")
        if current_region_count > available_memory_mb / 50:
            new_region_count = max(1, int(available_memory_mb / 50))
            config.set("brain.region_count", new_region_count)
        
        # 3. Disable memory-intensive features
        config.set("brain.enable_structural_plasticity", False)
        config.set("brain.enable_development", False)
        config.set("brain.enable_curiosity", False)
        
        # 4. Reduce plasticity complexity
        config.set("plasticity.stdp.enable", False)
        config.set("plasticity.hebbian.enable", True)  # Simpler, less memory
        
        # 5. Use compressed synaptic representations
        config.set("brain.use_compressed_synapses", True)
        config.set("brain.synapse_compression_ratio", 0.5)
    
    return config

def estimate_brain_memory(config):
    """Estimate brain memory usage in MB."""
    # Estimates based on configuration
    neuron_count = config.getInt("brain.neuron_count", 1000)
    region_count = config.getInt("brain.region_count", 2)
    
    # Memory per neuron (approximate)
    memory_per_neuron = 0.01  # MB
    
    # Memory per region
    memory_per_region = 0.1  # MB
    
    # Memory for synaptic connections
    connection_prob = config.getDouble("brain.connection_probability", 0.1)
    synapse_memory = neuron_count * neuron_count * connection_prob * 0.001  # MB
    
    # Memory for plasticity rules
    plasticity_memory = 1.0  # MB
    
    # Memory for development system
    development_memory = 0.5  # MB
    
    total_memory = (neuron_count * memory_per_neuron + 
                   region_count * memory_per_region +
                   synapse_memory + plasticity_memory + development_memory)
    
    return total_memory

def create_streaming_brain(config):
    """Create a streaming brain for real-time processing."""
    
    # Configure for streaming processing
    config.set("brain.streaming_mode", True)
    config.set("brain.buffer_size", 1000)  # Number of steps to buffer
    config.set("brain.streaming_update_rate", 60.0)  # Hz
    config.set("brain.compression_enabled", True)
    
    # Optimize for streaming performance
    config.set("brain.use_event_driven", True)
    config.set("brain.use_sparse_updates", True)
    config.set("brain.use_batch_processing", True)
    
    # Reduce memory overhead
    config.set("brain.enable_structural_plasticity", False)
    config.set("brain.enable_development", False)
    config.set("brain.enable_curiosity", False)
    
    # Configure for real-time response
    config.set("brain.response_time_budget", 0.016)  # 16ms for 60Hz
    config.set("brain.max_lateney_ms", 10.0)
    
    return config
```

### Example 5: Advanced Error Handling and Recovery

```python
import pynlm
import traceback
import logging

class ResilientBrainWrapper:
    """Wrapper for brain with advanced error handling and recovery."""
    
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.brain = None
        self.world = None
        self.agent = None
        self.checkpoint_file = "brain_checkpoint.bin"
        self.max_retries = 3
        self.recovery_strategy = "restart"
        
        # Setup logging
        logging.basicConfig(level=logging.INFO)
        self.logger = logging.getLogger(__name__)
    
    def create_brain(self):
        """Create brain with error checking."""
        try:
            self.brain = pynlm.createBrain(self.config)
            
            # Validate configuration
            if not self.validate_configuration():
                raise ValueError("Configuration validation failed")
            
            # Initialize brain
            self.brain.initialize()
            self.logger.info("Brain created and initialized successfully")
            
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to create brain: {e}")
            traceback.print_exc()
            return False
    
    def validate_configuration(self):
        """Validate brain configuration."""
        try:
            # Check essential configuration
            if not self.config.has("brain.neuron_count"):
                self.logger.error("Missing brain.neuron_count configuration")
                return False
            
            neuron_count = self.config.getInt("brain.neuron_count")
            if neuron_count <= 0:
                self.logger.error("Invalid neuron count")
                return False
            
            # Check plasticity configuration
            if self.config.getBool("brain.enable_structural_plasticity"):
                if not self.config.has("plasticity.structural.plasticity_rate"):
                    self.logger.warning("Structural plasticity enabled but no rate specified")
            
            # Check development configuration
            if self.config.getBool("brain.enable_development"):
                if not self.config.has("development.synaptogenesis_rate"):
                    self.logger.warning("Development enabled but no synaptogenesis rate specified")
            
            return True
            
        except Exception as e:
            self.logger.error(f"Configuration validation error: {e}")
            return False
    
    def run_simulation_with_recovery(self, num_steps=1000):
        """Run simulation with automatic error recovery."""
        for attempt in range(self.max_retries):
            try:
                self.logger.info(f"Starting simulation attempt {attempt + 1}")
                
                if not self.create_brain():
                    continue
                
                if not self.create_world():
                    continue
                
                # Run simulation
                for step in range(num_steps):
                    try:
                        self.run_step(step)
                        
                        # Check for critical errors
                        if self.check_critical_errors():
                            raise RuntimeError("Critical error detected")
                        
                    except Exception as e:
                        self.logger.error(f"Error at step {step}: {e}")
                        
                        if self.recovery_strategy == "continue":
                            self.recover_from_error(step)
                            continue
                        elif self.recovery_strategy == "checkpoint":
                            self.save_checkpoint()
                            self.recover_from_error(step)
                            continue
                        else:  # restart
                            break
                
                self.logger.info("Simulation completed successfully")
                return True
                
            except Exception as e:
                self.logger.error(f"Simulation attempt {attempt + 1} failed: {e}")
                
                if attempt == self.max_retries - 1:
                    self.logger.error("Max retries exceeded")
                    return False
                
                # Wait before retry
                time.sleep(1.0 * (attempt + 1))
        
        return False
    
    def create_world(self):
        """Create world with error checking."""
        try:
            self.world = pynlm.createSimpleWorld()
            self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            self.world.reset()
            
            # Create agent brain interface
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)
            
            self.logger.info("World and agent created successfully")
            return True
            
        except Exception as e:
            self.logger.error(f"Failed to create world: {e}")
            traceback.print_exc()
            return False
    
    def run_step(self, step):
        """Run a single simulation step."""
        try:
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Get and apply action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply neuromodulation
            reward = self.calculate_reward()
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
        except Exception as e:
            self.logger.error(f"Error in simulation step {step}: {e}")
            raise
    
    def calculate_reward(self):
        """Calculate reward for current state."""
        # Simple reward calculation
        if self.world:
            # Get internal signals from sensory percept
            percept = self.world.getSensoryPercept()
            internal = percept.getInternal()
            if internal:
                return internal[0] if len(internal) > 0 else 0.0
        
        return 0.0
    
    def check_critical_errors(self):
        """Check for critical errors."""
        try:
            # Check neuron count
            neuron_count = self.brain.getTotalNeuronCount()
            if neuron_count == 0:
                return True
            
            # Check for excessive firing rate
            firing_rate = self.brain.getAverageFiringRate()
            if firing_rate > 100.0:  # Threshold for abnormal activity
                return True
            
            # Check memory usage
            memory_usage = self.get_memory_usage()
            if memory_usage > 0.9:  # 90% memory threshold
                return True
            
            return False
            
        except Exception:
            return True  # Assume error if we can't check
    
    def get_memory_usage(self):
        """Get approximate memory usage."""
        # Simplified memory usage estimation
        neuron_count = self.brain.getTotalNeuronCount()
        synapse_count = self.brain.getTotalSynapseCount()
        
        # Rough estimate in MB
        memory_mb = (neuron_count * 0.01 + synapse_count * 0.001) / 1024.0
        
        return min(memory_mb, 1.0)  # Cap at 1.0 (100%)
    
    def save_checkpoint(self):
        """Save brain checkpoint."""
        try:
            if self.brain:
                self.brain.save(self.checkpoint_file)
                self.logger.info(f"Checkpoint saved to {self.checkpoint_file}")
        except Exception as e:
            self.logger.error(f"Failed to save checkpoint: {e}")
    
    def load_checkpoint(self):
        """Load brain checkpoint."""
        try:
            if self.brain and os.path.exists(self.checkpoint_file):
                self.brain.load(self.checkpoint_file)
                self.logger.info(f"Checkpoint loaded from {self.checkpoint_file}")
                return True
        except Exception as e:
            self.logger.error(f"Failed to load checkpoint: {e}")
        
        return False
    
    def recover_from_error(self, step):
        """Recover from simulation error."""
        try:
            self.logger.info(f"Attempting recovery from error at step {step}")
            
            if self.recovery_strategy == "checkpoint" and self.load_checkpoint():
                self.logger.info("Recovered from checkpoint")
                return
            
            # Generic recovery: reinitialize components
            if self.brain:
                self.brain.reset()
            
            if self.agent:
                self.agent.reset()
            
            if self.world:
                self.world.reset()
            
            self.logger.info("Recovered through component reinitialization")
            
        except Exception as e:
            self.logger.error(f"Recovery failed: {e}")
            raise

# Usage example
if __name__ == "__main__":
    # Create resilient brain wrapper
    resilient_brain = ResilientBrainWrapper()
    resilient_brain.recovery_strategy = "checkpoint"
    
    # Run simulation with recovery
    print("Starting resilient simulation...")
    success = resilient_brain.run_simulation_with_recovery(num_steps=500)
    
    if success:
        print("Resilient simulation completed successfully!")
    else:
        print("Resilient simulation failed after all retries")
```

## 4. Advanced Integration Examples

### Example 6: NLM Integration with External Systems

```python
import pynlm
import numpy as np
import scipy
from sklearn.neural_network import MLPRegressor

class NeuromorphicAIIntegration:
    """Advanced integration of NLM with external AI systems."""
    
    def __init__(self):
        self.nlm_brain = None
        self.ml_model = None
        self.feature_extractor = None
        self.reward_predictor = None
        self.external_data_source = None
    
    def initialize_with_external_data(self, data_source, config):
        """Initialize NLM with external data integration."""
        self.external_data_source = data_source
        self.nlm_brain = pynlm.createBrain(config)
        
        # Initialize advanced feature extractor
        self.feature_extractor = FeatureExtractor()
        
        # Initialize reward predictor
        self.reward_predictor = RewardPredictor()
        
        # Initialize machine learning model for behavior prediction
        self.ml_model = MLPRegressor(hidden_layer_sizes=(100, 50),
                                    activation='relu',
                                    solver='adam',
                                    max_iter=1000,
                                    random_state=42)
        
        # Initialize NLM
        self.nlm_brain.initialize()
        
        return self
    
    def process_external_data(self, data):
        """Process external data and integrate with NLM."""
        # Extract features from external data
        features = self.feature_extractor.extract_features(data)
        
        # Predict environmental conditions
        environmental_features = features['environmental']
        behavioral_features = features['behavioral']
        
        # Update NLM based on external data
        self.update_nlm_from_external(features)
        
        # Get predictions from ML model
        predictions = self.get_ml_predictions(environmental_features, behavioral_features)
        
        # Return integrated result
        return {
            'nlm_state': self.get_nlm_state(),
            'predictions': predictions,
            'external_features': features
        }
    
    def update_nlm_from_external(self, features):
        """Update NLM based on external data."""
        # Convert external data to NLM sensory input
        sensory_input = self.convert_external_to_sensory(features)
        
        # Apply to NLM
        self.nlm_brain.receiveSensoryInput(sensory_input)
        
        # Update prediction system
        prediction_error = self.reward_predictor.predict(features)
        self.update_prediction_system(prediction_error)
    
    def convert_external_to_sensory(self, features):
        """Convert external data to NLM sensory input."""
        from pynlm import Vision, Audio, InternalSignals
        
        # Create sensory percept
        percept = pynlm.SensoryPercept()
        
        # Convert environmental data to vision
        if 'vision_data' in features:
            vision_data = features['vision_data']
            vision = Vision(8, 8, 3)
            vision.setData(self.normalize_vision_data(vision_data))
            percept.setVision(vision)
        
        # Convert audio data
        if 'audio_data' in features:
            audio_data = features['audio_data']
            audio = Audio(44100, len(audio_data))
            audio.setData(self.normalize_audio_data(audio_data))
            audio.setSampleRate(44100)
            # Note: Audio is not directly supported by SensoryPercept
            # This would require extending the API
        
        # Convert internal signals
        if 'internal_signals' in features:
            internal_data = features['internal_signals']
            internal_signals = InternalSignals()
            for i, signal in enumerate(internal_data):
                internal_signals.addSignal(signal)
            # Note: InternalSignals is not directly accessible from SensoryPercept
            # This would require extending the API
        
        return percept
    
    def get_ml_predictions(self, env_features, beh_features):
        """Get predictions from ML model."""
        # Combine features
        all_features = np.concatenate([env_features, beh_features])
        
        # Train ML model on combined features
        if len(all_features) > 0:
            # This would require labeled training data
            # For demonstration, we'll use a simple approach
            self.ml_model.fit(np.array([all_features]), np.array([0]))
        
        # Make predictions
        predictions = self.ml_model.predict(np.array([all_features]))
        
        return {
            'behavior_prediction': predictions[0],
            'confidence': min(predictions[0], 1.0),  # Simple confidence calculation
            'alternative_behaviors': self.get_alternative_predictions(all_features)
        }
    
    def update_prediction_system(self, prediction_error):
        """Update NLM prediction system based on error."""
        # Apply prediction error to neuromodulation
        if hasattr(self.nlm_brain, 'agent'):
            self.nlm_brain.agent.applyRewardModulation(
                -prediction_error,  # Negative error as negative reward
                0.0
            )
        
        # Update developmental stage based on prediction error
        if hasattr(self.nlm_brain, 'getDevelopmentalStage'):
            stage = self.nlm_brain.getDevelopmentalStage()
            if abs(prediction_error) > 0.5:
                # High prediction error -> increase learning
                self.nlm_brain.setDevelopmentalStage(
                    pynlm.DevelopmentalStage.CriticalPeriod
                )
    
    def get_nlm_state(self):
        """Get current NLM state for integration."""
        return {
            'neuron_count': self.nlm_brain.getTotalNeuronCount(),
            'active_neurons': self.nlm_brain.getActiveNeuronCount(),
            'firing_rate': self.nlm_brain.getAverageFiringRate(),
            'developmental_stage': self.nlm_brain.getDevelopmentalStage(),
            'prediction_error': getattr(self.nlm_brain.agent, 'getPredictionError', lambda: 0)()
        }
    
    def get_alternative_predictions(self, features):
        """Get alternative predictions using ensemble methods."""
        # Simple ensemble of multiple predictions
        alternatives = []
        
        # Alternative 1: Conservative prediction
        alternatives.append({
            'type': 'conservative',
            'prediction': np.mean(features) * 0.8,
            'confidence': 0.7
        })
        
        # Alternative 2: Aggressive prediction
        alternatives.append({
            'type': 'aggressive',
            'prediction': np.mean(features) * 1.2,
            'confidence': 0.6
        })
        
        # Alternative 3: Neutral prediction
        alternatives.append({
            'type': 'neutral',
            'prediction': np.mean(features),
            'confidence': 0.8
        })
        
        return alternatives
    
    def normalize_vision_data(self, vision_data):
        """Normalize vision data for NLM input."""
        if not vision_data:
            return [0.0] * 64
        
        # Simple normalization
        normalized = []
        for pixel in vision_data:
            normalized.append(pixel / 255.0)
        
        # Pad or truncate to match expected size
        while len(normalized) < 64:
            normalized.append(0.0)
        
        return normalized[:64]
    
    def normalize_audio_data(self, audio_data):
        """Normalize audio data for NLM input."""
        if not audio_data:
            return []
        
        # Simple normalization
        max_val = max(abs(x) for x in audio_data) if audio_data else 1.0
        if max_val > 0:
            return [x / max_val for x in audio_data]
        
        return audio_data

class FeatureExtractor:
    """Extract features from external data for NLM integration."""
    
    def extract_features(self, data):
        """Extract features from external data."""
        features = {}
        
        # Environmental features
        env_features = {}
        
        if 'vision' in data:
            env_features['vision'] = self.extract_vision_features(data['vision'])
        
        if 'audio' in data:
            env_features['audio'] = self.extract_audio_features(data['audio'])
        
        if 'environmental' in data:
            env_features.update(data['environmental'])
        
        # Behavioral features
        beh_features = {}
        
        if 'behavior' in data:
            beh_features['actions'] = self.extract_behavior_features(data['behavior'])
        
        if 'history' in data:
            beh_features['history'] = self.extract_history_features(data['history'])
        
        features['environmental'] = env_features
        features['behavioral'] = beh_features
        
        return features
    
    def extract_vision_features(self, vision_data):
        """Extract features from vision data."""
        # Calculate basic statistics
        if not vision_data:
            return {'mean': 0.0, 'std': 0.0, 'edges': 0.0}
        
        mean = np.mean(vision_data)
        std = np.std(vision_data)
        
        # Simple edge detection
        edges = self.calculate_edges(vision_data)
        
        return {'mean': mean, 'std': std, 'edges': edges}
    
    def extract_audio_features(self, audio_data):
        """Extract features from audio data."""
        # Calculate audio characteristics
        if not audio_data:
            return {'frequency': 0.0, 'amplitude': 0.0, 'duration': 0.0}
        
        # Simplified feature extraction
        amplitude = np.max(np.abs(audio_data))
        mean_freq = np.mean(audio_data)
        duration = len(audio_data) / 44100.0  # Assuming 44.1kHz
        
        return {
            'amplitude': amplitude,
            'frequency': mean_freq,
            'duration': duration
        }
    
    def extract_behavior_features(self, behavior_data):
        """Extract features from behavior data."""
        # Calculate behavioral statistics
        if not behavior_data:
            return {'action_frequency': 0.0, 'action_variance': 0.0}
        
        actions = [b['action'] for b in behavior_data]
        action_frequency = len(actions) / len(set(actions)) if set(actions) else 0.0
        action_variance = np.var([hash(a) for a in actions]) if actions else 0.0
        
        return {
            'action_frequency': action_frequency,
            'action_variance': action_variance
        }
    
    def extract_history_features(self, history_data):
        """Extract features from behavioral history."""
        # Calculate learning patterns
        if not history_data:
            return {'learning_rate': 0.0, 'consistency': 0.0}
        
        # Simple learning pattern detection
        learning_rates = []
        for episode in history_data:
            if 'reward' in episode and 'action' in episode:
                learning_rates.append(episode['reward'])
        
        learning_rate = np.mean(learning_rates) if learning_rates else 0.0
        consistency = 1.0 - np.std(learning_rates) if learning_rates else 0.0
        
        return {
            'learning_rate': learning_rate,
            'consistency': consistency
        }
    
    def calculate_edges(self, vision_data):
        """Calculate edge detection from vision data."""
        # Simple edge detection using gradient
        if len(vision_data) < 2:
            return 0.0
        
        gradients = []
        for i in range(1, len(vision_data)):
            gradient = abs(vision_data[i] - vision_data[i-1])
            gradients.append(gradient)
        
        return np.mean(gradients) if gradients else 0.0

class RewardPredictor:
    """Predict rewards based on external data."""
    
    def predict(self, features):
        """Predict reward based on features."""
        # Simple reward prediction based on feature statistics
        if not features:
            return 0.0
        
        # Calculate weighted reward
        reward = 0.0
        
        if 'environmental' in features:
            env = features['environmental']
            if 'vision' in env:
                vision = env['vision']
                reward += vision.get('edges', 0.0) * 0.1
                reward += (1.0 - vision.get('mean', 0.0)) * 0.2
            
            if 'audio' in env:
                audio = env['audio']
                reward += audio.get('amplitude', 0.0) * 0.3
        
        if 'behavioral' in features:
            beh = features['behavioral']
            if 'history' in beh:
                history = beh['history']
                reward += history.get('learning_rate', 0.0) * 0.2
                reward += history.get('consistency', 0.0) * 0.2
        
        # Normalize reward
        reward = max(0.0, min(1.0, reward))
        
        return reward

# Usage example
if __name__ == "__main__":
    # Create integration
    integration = NeuromorphicAIIntegration()
    
    # Example external data
    external_data = {
        'vision': [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0],
        'audio': [0.0, 0.1, 0.2, 0.3, 0.4, 0.3, 0.2, 0.1, 0.0],
        'environmental': {
            'temperature': 25.0,
            'humidity': 0.6,
            'light_level': 0.8
        },
        'behavior': [
            {'action': 'move_forward', 'reward': 0.8},
            {'action': 'turn_left', 'reward': 0.6},
            {'action': 'interact', 'reward': 1.0}
        ],
        'history': [
            {'reward': 0.7, 'action': 'move_forward'},
            {'reward': 0.8, 'action': 'turn_left'},
            {'reward': 0.9, 'action': 'interact'}
        ]
    }
    
    # Initialize integration
    config = pynlm.createDefaultConfig()
    integration.initialize_with_external_data(external_data, config)
    
    # Process external data
    result = integration.process_external_data(external_data)
    
    print("NLM-External Integration Results:")
    print(f"Neuron Count: {result['nlm_state']['neuron_count']}")
    print(f"Active Neurons: {result['nlm_state']['active_neurons']}")
    print(f"Prediction Error: {result['nlm_state']['prediction_error']}")
    print(f"ML Prediction: {result['predictions']['behavior_prediction']}")
    print(f"Confidence: {result['predictions']['confidence']}")
```

## 5. Advanced Python API Extensions

### Example 7: Custom Python Extensions for NLM

```python
import pynlm
import numpy as np
from typing import List, Dict, Any, Optional

class NLMAdvancedAPI:
    """Advanced Python API extensions for NLM."""
    
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Configure advanced features
        self.setup_advanced_features()
    
    def setup_advanced_features(self):
        """Setup advanced NLM features."""
        # Enable all advanced features
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Configure advanced parameters
        self.config.set("brain.enable_advanced_plasticity", True)
        self.config.set("brain.use_ensemble_learning", True)
        self.config.set("brain.enable_meta_learning", True)
        self.config.set("brain.use_attention_mechanism", True)
    
    def create_custom_brain(self, architecture=None):
        """Create a custom brain architecture."""
        if architecture is None:
            architecture = self.get_default_architecture()
        
        # Create brain with custom architecture
        custom_config = self.create_custom_config(architecture)
        
        brain = pynlm.createBrain(custom_config)
        brain.initialize()
        
        # Wrap with custom agent
        custom_agent = CustomAgentBrain(brain, architecture)
        
        return CustomBrainWrapper(brain, custom_agent, custom_config)
    
    def get_default_architecture(self):
        """Get default brain architecture."""
        return {
            'neuron_count': 1000,
            'layer_sizes': [256, 128, 64, 32],
            'connection_density': 0.1,
            'plasticity_rules': ['STDP', 'Hebbian', 'Structural'],
            'development_stages': ['initial', 'critical_period', 'maturation', 'adult'],
            'attention_layers': True,
            'meta_learning_enabled': True
        }
    
    def create_custom_config(self, architecture):
        """Create custom configuration based on architecture."""
        config = pynlm.createDefaultConfig()
        
        # Set neuron count
        config.set("brain.neuron_count", architecture['neuron_count'])
        
        # Set connection density
        config.set("brain.connection_density", architecture['connection_density'])
        
        # Enable specific plasticity rules
        for rule in architecture['plasticity_rules']:
            config.set(f"plasticity.{rule}.enable", True)
        
        # Configure developmental stages
        for i, stage in enumerate(architecture['development_stages']):
            config.set(f"development.{stage}.plasticity_modifier", 
                     1.0 / (i + 1))
        
        # Enable attention if requested
        if architecture['attention_layers']:
            config.set("brain.enable_attention", True)
            config.set("brain.attention_heads", 8)
        
        return config
    
    def run_advanced_simulation(self, num_steps=1000, enable_rendering=False):
        """Run advanced simulation with optional rendering."""
        # Setup world
        self.world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
        self.world.reset()
        
        # Initialize agent
        self.agent.initialize(self.world)
        
        # Store simulation state
        simulation_state = {
            'steps': [],
            'metrics': [],
            'events': []
        }
        
        # Run simulation
        for step in range(num_steps):
            try:
                # Get advanced sensory processing
                percept = self.get_advanced_sensory_percept(step)
                
                # Process with advanced agent
                self.process_advanced_input(percept, step)
                
                # Run brain with advanced dynamics
                self.run_advanced_brain_dynamics(step)
                
                # Get advanced action selection
                action = self.get_advanced_motor_command(step)
                
                # Apply action to world
                self.apply_advanced_action(action, step)
                
                # Collect advanced metrics
                metrics = self.collect_advanced_metrics(step)
                simulation_state['metrics'].append(metrics)
                
                # Record events
                events = self.detect_advanced_events(step)
                simulation_state['events'].append(events)
                
                # Optional: Render visualization
                if enable_rendering:
                    self.render_step(step)
                
                # Log progress
                if step % 100 == 0:
                    print(f"Step {step}/{num_steps}")
                
            except Exception as e:
                print(f"Error at step {step}: {e}")
                # Continue with next step
                continue
        
        simulation_state['steps'] = list(range(num_steps))
        return simulation_state
    
    def get_advanced_sensory_percept(self, step):
        """Get advanced sensory percept with feature extraction."""
        from pynlm import Vision, Audio, InternalSignals
        
        # Get base percept
        base_percept = self.world.getSensoryPercept()
        
        # Create enhanced percept
        enhanced_percept = pynlm.SensoryPercept()
        
        # Process vision with advanced feature extraction
        if hasattr(base_percept, 'getVision'):
            vision = base_percept.getVision()
            if vision:
                enhanced_vision = self.enhance_vision_features(vision, step)
                enhanced_percept.setVision(enhanced_vision)
        
        # Process audio with advanced analysis
        if hasattr(base_percept, 'getAudio'):
            audio = base_percept.getAudio()
            if audio:
                enhanced_audio = self.enhance_audio_features(audio, step)
                enhanced_percept.setAudio(enhanced_audio)
        
        # Process internal signals with advanced analysis
        if hasattr(base_percept, 'getInternal'):
            internal = base_percept.getInternal()
            if internal:
                enhanced_internal = self.enhance_internal_features(internal, step)
                enhanced_percept.setInternal(enhanced_internal)
        
        return enhanced_percept
    
    def enhance_vision_features(self, vision, step):
        """Enhance vision features with advanced processing."""
        # Get vision data
        vision_data = vision.getData()
        
        # Apply advanced feature extraction
        enhanced_data = self.apply_advanced_vision_processing(vision_data, step)
        
        # Create new vision object
        enhanced_vision = Vision(vision.getWidth(), vision.getHeight(), vision.getChannels())
        enhanced_vision.setData(enhanced_data)
        
        return enhanced_vision
    
    def apply_advanced_vision_processing(self, vision_data, step):
        """Apply advanced vision processing."""
        # Multi-scale feature extraction
        features = []
        
        # Scale 1: Raw pixels
        features.extend(vision_data)
        
        # Scale 2: Local contrast
        contrast_features = self.calculate_local_contrast(vision_data)
        features.extend(contrast_features)
        
        # Scale 3: Edge detection
        edge_features = self.detect_edges(vision_data)
        features.extend(edge_features)
        
        # Scale 4: Motion detection (simplified)
        motion_features = self.detect_motion(vision_data, step)
        features.extend(motion_features)
        
        # Scale 5: Textural features
        texture_features = self.extract_texture_features(vision_data)
        features.extend(texture_features)
        
        # Normalize features
        normalized_features = self.normalize_features(features)
        
        return normalized_features
    
    def calculate_local_contrast(self, vision_data):
        """Calculate local contrast features."""
        height = len(vision_data)
        width = len(vision_data[0]) if height > 0 else 0
        contrast_features = []
        
        for y in range(1, height - 1):
            for x in range(1, width - 1):
                center = vision_data[y][x]
                
                # Calculate local variance
                local_values = []
                for dy in [-1, 0, 1]:
                    for dx in [-1, 0, 1]:
                        if dy == 0 and dx == 0:
                            continue
                        ny, nx = y + dy, x + dx
                        if 0 <= ny < height and 0 <= nx < width:
                            local_values.append(vision_data[ny][nx])
                
                if local_values:
                    variance = np.var(local_values)
                    contrast = variance / (center * center + 1e-6)
                    contrast_features.append(contrast)
        
        return contrast_features[:100]  # Limit size
    
    def detect_edges(self, vision_data):
        """Detect edges in vision data."""
        # Simple Sobel operator approximation
        height = len(vision_data)
        width = len(vision_data[0]) if height > 0 else 0
        edges = []
        
        for y in range(1, height - 1):
            for x in range(1, width - 1):
                # Sobel operator
                gx = (-vision_data[y-1][x-1] - 2 * vision_data[y][x-1] - vision_data[y+1][x-1] +
                      vision_data[y-1][x+1] + 2 * vision_data[y][x+1] + vision_data[y+1][x+1]) / 8.0
                
                gy = (-vision_data[y-1][x-1] - 2 * vision_data[y-1][x] - vision_data[y-1][x+1] +
                      vision_data[y+1][x-1] + 2 * vision_data[y+1][x] + vision_data[y+1][x+1]) / 8.0
                
                magnitude = np.sqrt(gx * gx + gy * gy)
                edges.append(magnitude)
        
        return edges[:100]  # Limit size
    
    def detect_motion(self, vision_data, step):
        """Detect motion in vision data."""
        # Simplified motion detection using frame difference
        if not hasattr(self, '_previous_vision'):
            self._previous_vision = vision_data
            return [0.0] * 10
        
        # Calculate difference
        motion = []
        height = len(vision_data)
        width = len(vision_data[0]) if height > 0 else 0
        
        for y in range(height):
            for x in range(width):
                diff = abs(vision_data[y][x] - self._previous_vision[y][x])
                motion.append(diff)
        
        # Update previous vision
        self._previous_vision = vision_data
        
        # Sample motion features
        return motion[:10]
    
    def extract_texture_features(self, vision_data):
        """Extract textural features from vision data."""
        # Calculate textural statistics
        if not vision_data:
            return [0.0] * 5
        
        # Convert to numpy array
        arr = np.array(vision_data)
        
        # Calculate texture features
        mean = np.mean(arr)
        std = np.std(arr)
        skew = self.calculate_skew(arr)
        kurtosis = self.calculate_kurtosis(arr)
        entropy = self.calculate_entropy(arr)
        
        return [mean, std, skew, kurtosis, entropy]
    
    def calculate_skew(self, data):
        """Calculate skewness of data."""
        if len(data) < 3:
            return 0.0
        
        mean = np.mean(data)
        std = np.std(data)
        if std < 1e-6:
            return 0.0
        
        skew = np.mean(((data - mean) / std) ** 3)
        return skew
    
    def calculate_kurtosis(self, data):
        """Calculate kurtosis of data."""
        if len(data) < 4:
            return 0.0
        
        mean = np.mean(data)
        std = np.std(data)
        if std < 1e-6:
            return 0.0
        
        kurtosis = np.mean(((data - mean) / std) ** 4) - 3.0
        return kurtosis
    
    def calculate_entropy(self, data):
        """Calculate entropy of data."""
        # Simple entropy calculation
        unique_values, counts = np.unique(data, return_counts=True)
        probabilities = counts / len(data)
        entropy = -np.sum(probabilities * np.log(probabilities + 1e-6))
        return entropy
    
    def normalize_features(self, features):
        """Normalize features to [0, 1] range."""
        if not features:
            return []
        
        min_val = min(features)
        max_val = max(features)
        
        if max_val - min_val < 1e-6:
            return [0.5] * len(features)
        
        normalized = [(f - min_val) / (max_val - min_val) for f in features]
        return normalized
    
    def enhance_audio_features(self, audio, step):
        """Enhance audio features with advanced processing."""
        audio_data = audio.getData()
        enhanced_data = self.apply_advanced_audio_processing(audio_data, step)
        
        from pynlm import Audio
        enhanced_audio = Audio(audio.getSampleRate(), len(enhanced_data))
        enhanced_audio.setData(enhanced_data)
        enhanced_audio.setSampleRate(audio.getSampleRate())
        
        return enhanced_audio
    
    def apply_advanced_audio_processing(self, audio_data, step):
        """Apply advanced audio processing."""
        # Multi-band analysis
        bands = self.equalize_audio_bands(audio_data)
        
        # Rhythm detection
        rhythm = self.detect_audio_rhythm(audio_data, step)
        
        # Harmonic analysis
        harmonics = self.analyze_audio_harmonics(audio_data)
        
        # Combine features
        features = bands + rhythm + harmonics
        
        return features
    
    def equalize_audio_bands(self, audio_data):
        """Apply frequency band equalization."""
        # Simple frequency analysis
        if len(audio_data) < 10:
            return [0.0] * 5
        
        # Split into frequency bands
        bands = []
        for i in range(5):
            start = i * len(audio_data) // 5
            end = (i + 1) * len(audio_data) // 5
            if start < end:
                band_data = audio_data[start:end]
                bands.append(np.mean(np.abs(band_data)))
            else:
                bands.append(0.0)
        
        return bands
    
    def detect_audio_rhythm(self, audio_data, step):
        """Detect rhythm patterns in audio data."""
        # Simple beat detection
        if len(audio_data) < 20:
            return [0.0] * 3
        
        # Calculate beat frequency
        envelope = self.calculate_envelope(audio_data)
        peaks = self.find_peaks(envelope)
        
        if len(peaks) < 2:
            return [0.0, 0.0, 0.0]
        
        # Calculate beat characteristics
        beat_durations = np.diff(peaks)
        avg_beat = np.mean(beat_durations)
        beat_variance = np.var(beat_durations)
        
        return [avg_beat, beat_variance, len(peaks)]
    
    def analyze_audio_harmonics(self, audio_data):
        """Analyze harmonic content of audio data."""
        # Simple harmonic analysis
        if len(audio_data) < 10:
            return [0.0] * 3
        
        # Calculate spectral features
        fft_result = np.fft.fft(audio_data)
        magnitudes = np.abs(fft_result)
        
        # Find dominant frequencies
        dominant_freqs = np.argsort(magnitudes)[-3:][::-1]  # Top 3 frequencies
        
        # Calculate harmonic ratios
        if magnitudes[0] > 0:
            harmonic_ratios = []
            for freq in dominant_freqs:
                if freq > 0:
                    ratio = magnitudes[freq] / magnitudes[0]
                    harmonic_ratios.append(ratio)
                else:
                    harmonic_ratios.append(0.0)
        else:
            harmonic_ratios = [0.0, 0.0, 0.0]
        
        return harmonic_ratios[:3]
    
    def calculate_envelope(self, audio_data):
        """Calculate envelope of audio signal."""
        # Simple absolute value envelope
        return np.abs(audio_data)
    
    def find_peaks(self, signal):
        """Find peaks in signal."""
        # Simple peak finding
        peaks = []
        for i in range(1, len(signal) - 1):
            if signal[i] > signal[i-1] and signal[i] > signal[i+1]:
                peaks.append(i)
        return peaks
    
    def enhance_internal_features(self, internal_data, step):
        """Enhance internal features with advanced processing."""
        # Internal signals typically have different processing needs
        enhanced = []
        
        for value in internal_data:
            # Apply non-linear transformation
            enhanced_value = self.apply_nonlinear_transform(value, step)
            enhanced.append(enhanced_value)
        
        return enhanced
    
    def apply_nonlinear_transform(self, value, step):
        """Apply non-linear transformation to value."""
        # Complex transformation function
        # This simulates advanced neural processing
        transformed = value * (1.0 + 0.1 * np.sin(step * 0.01))
        transformed += 0.01 * np.random.randn()  # Add some noise
        
        # Apply sigmoid-like response
        transformed = 2.0 / (1.0 + np.exp(-2.0 * transformed)) - 1.0
        
        return transformed
    
    def run_advanced_brain_dynamics(self, step):
        """Run advanced brain dynamics."""
        # Enhanced brain processing
        self.brain.step(step)
        
        # Apply attention mechanism if enabled
        if self.config.getBool("brain.enable_attention"):
            self.apply_attention_mechanism(step)
        
        # Apply meta-learning if enabled
        if self.config.getBool("brain.enable_meta_learning"):
            self.apply_meta_learning(step)
        
        # Apply ensemble learning if enabled
        if self.config.getBool("brain.use_ensemble_learning"):
            self.apply_ensemble_learning(step)
    
    def apply_attention_mechanism(self, step):
        """Apply attention mechanism to brain dynamics."""
        # Simulate attention mechanism
        # This is a simplified version
        attention_weights = np.random.rand(10)
        attention_weights /= np.sum(attention_weights)
        
        # Apply attention to brain state
        # In practice, this would involve complex matrix operations
        pass
    
    def apply_meta_learning(self, step):
        """Apply meta-learning to brain dynamics."""
        # Meta-learning adjusts learning rates based on performance
        learning_rate = 0.01 + 0.005 * np.sin(step * 0.01)
        
        # Apply to plasticity rules
        # In practice, this would involve updating rule parameters
        pass
    
    def apply_ensemble_learning(self, step):
        """Apply ensemble learning to brain dynamics."""
        # Ensemble learning combines multiple learning strategies
        # Each strategy contributes to overall learning
        strategies = ['STDP', 'Hebbian', 'Random']
        
        for strategy in strategies:
            # Apply each learning strategy
            # In practice, this would involve complex combination of rules
            pass
    
    def get_advanced_motor_command(self, step):
        """Get advanced motor command selection."""
        action = self.agent.decodeMotorCommand()
        
        # Apply advanced action selection if enabled
        if self.config.getBool("brain.enable_advanced_action_selection"):
            action = self.apply_advanced_action_selection(action, step)
        
        # Apply curiosity-driven exploration if enabled
        if self.config.getBool("brain.enable_curiosity"):
            curiosity = self.agent.getCuriosityLevel()
            if curiosity > 0.5:
                action = self.consider_curiosity_action(action, step)
        
        return action
    
    def apply_advanced_action_selection(self, action, step):
        """Apply advanced action selection logic."""
        # Advanced action selection considers multiple factors
        # This is a simplified version
        
        # Get brain statistics
        firing_rate = self.brain.getAverageFiringRate()
        prediction_error = self.agent.getPredictionError()
        
        # Modify action based on brain state
        if prediction_error > 0.5:
            # High prediction error -> explore alternative actions
            action = self.consider_alternative_actions(action, step)
        
        return action
    
    def consider_curiosity_action(self, action, step):
        """Consider curiosity-driven alternative actions."""
        # When curiosity is high, consider exploring alternative actions
        curiosity = self.agent.getCuriosityLevel()
        
        if curiosity > 0.7:
            # High curiosity -> random exploration
            import random
            return self.get_random_action()
        
        return action
    
    def consider_alternative_actions(self, current_action, step):
        """Consider alternative actions based on prediction error."""
        # Based on prediction error, consider alternative behaviors
        prediction_error = self.agent.getPredictionError()
        
        if prediction_error > 0.8:
            # Very high prediction error -> completely random
            import random
            return self.get_random_action()
        
        return current_action
    
    def get_random_action(self):
        """Get a random action for exploration."""
        from pynlm import MotorCommand
        import random
        
        actions = list(MotorCommand)
        return random.choice(actions)
    
    def apply_advanced_action(self, action, step):
        """Apply action to world with advanced processing."""
        # Apply action to world
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Apply advanced action effects
        if self.config.getBool("brain.enable_advanced_action_effects"):
            self.apply_advanced_action_effects(action, step)
        
        # Update development
        self.agent.updateDevelopment(0.1)
    
    def apply_advanced_action_effects(self, action, step):
        """Apply advanced effects of actions."""
        # Advanced action effects
        # This could include delayed effects, side effects, etc.
        pass
    
    def collect_advanced_metrics(self, step):
        """Collect advanced simulation metrics."""
        metrics = {}
        
        # Basic brain metrics
        metrics['neuron_count'] = self.brain.getTotalNeuronCount()
        metrics['active_neurons'] = self.brain.getActiveNeuronCount()
        metrics['firing_rate'] = self.brain.getAverageFiringRate()
        metrics['excitatory_inhibition_ratio'] = self.brain.getExcitationInhibitionRatio()
        metrics['total_spikes'] = self.brain.getTotalSpikeCount()
        
        # Advanced metrics
        metrics['prediction_error'] = self.agent.getPredictionError()
        metrics['curiosity_level'] = self.agent.getCuriosityLevel()
        metrics['novelty_level'] = self.agent.getNoveltyLevel()
        metrics['neuromodulation_level'] = self.agent.getNeuromodulationLevel()
        metrics['developmental_stage'] = self.agent.getDevelopmentalStage()
        
        # Performance metrics
        metrics['learning_speed'] = self.calculate_learning_speed(step)
        metrics['adaptation_rate'] = self.calculate_adaptation_rate(step)
        metrics['exploration_exploitation_balance'] = self.calculate_balance(step)
        
        return metrics
    
    def calculate_learning_speed(self, step):
        """Calculate learning speed based on performance."""
        # Simplified learning speed calculation
        return 1.0 / (1.0 + np.exp(-step * 0.01))
    
    def calculate_adaptation_rate(self, step):
        """Calculate adaptation rate based on performance."""
        # Simplified adaptation rate calculation
        return 0.5 + 0.5 * np.sin(step * 0.02)
    
    def calculate_balance(self, step):
        """Calculate exploration-exploitation balance."""
        # Simplified balance calculation
        curiosity = self.agent.getCuriosityLevel()
        return curiosity * (1.0 - curiosity)
    
    def detect_advanced_events(self, step):
        """Detect advanced simulation events."""
        events = []
        
        # Detect learning events
        if self.detect_learning_event(step):
            events.append('learning_event')
        
        # Detect development events
        if self.detect_development_event(step):
            events.append('development_event')
        
        # Detect curiosity events
        if self.detect_curiosity_event(step):
            events.append('curiosity_event')
        
        # Detect prediction events
        if self.detect_prediction_event(step):
            events.append('prediction_event')
        
        return events
    
    def detect_learning_event(self, step):
        """Detect learning event."""
        # Detect if significant learning occurred
        if step < 10:
            return False
        
        # Compare current state to initial state
        current_rate = self.brain.getAverageFiringRate()
        initial_rate = 0.1  # Initial firing rate
        
        return current_rate > initial_rate * 2.0
    
    def detect_development_event(self, step):
        """Detect development event."""
        # Detect if development stage changed
        if step < 100:
            return False
        
        current_stage = self.agent.getDevelopmentalStage()
        return current_stage != pynlm.DevelopmentalStage.Initial
    
    def detect_curiosity_event(self, step):
        """Detect curiosity event."""
        # Detect if curiosity threshold was exceeded
        curiosity = self.agent.getCuriosityLevel()
        return curiosity > 0.8
    
    def detect_prediction_event(self, step):
        """Detect prediction event."""
        # Detect if prediction error threshold was exceeded
        prediction_error = self.agent.getPredictionError()
        return abs(prediction_error) > 0.7
    
    def render_step(self, step):
        """Render current simulation step (visualization)."""
        # Visualization code would go here
        # This is a placeholder for advanced rendering
        pass

class CustomAgentBrain:
    """Custom agent brain implementation."""
    
    def __init__(self, brain, architecture):
        self.brain = brain
        self.architecture = architecture
        self.custom_parameters = {}
    
    def initialize(self, world):
        """Initialize custom agent brain."""
        # Custom initialization logic
        pass
    
    def processSensoryInput(self, percept):
        """Process sensory input with custom logic."""
        # Custom sensory processing
        pass
    
    def decodeMotorCommand(self):
        """Decode motor command with custom logic."""
        # Custom action selection
        return self.brain.produceAction()

class CustomBrainWrapper:
    """Wrapper for custom brain implementations."""
    
    def __init__(self, brain, agent, config):
        self.brain = brain
        self.agent = agent
        self.config = config
    
    def step(self, step):
        """Advance simulation step."""
        self.brain.step(step)
    
    def getTotalNeuronCount(self):
        """Get total neuron count."""
        return self.brain.getTotalNeuronCount()
    
    def getActiveNeuronCount(self):
        """Get active neuron count."""
        return self.brain.getActiveNeuronCount()
    
    def getAverageFiringRate(self):
        """Get average firing rate."""
        return self.brain.getAverageFiringRate()
    
    def getExcitationInhibitionRatio(self):
        """Get excitation-inhibition ratio."""
        return self.brain.getExcitationInhibitionRatio()
    
    def getTotalSpikeCount(self):
        """Get total spike count."""
        return self.brain.getTotalSpikeCount()

# Usage example
if __name__ == "__main__":
    # Create advanced NLM instance
    advanced_nlm = NLMAdvancedAPI()
    
    # Create custom brain
    custom_brain = advanced_nlm.create_custom_brain()
    
    # Run advanced simulation
    print("Starting advanced simulation...")
    simulation_state = advanced_nlm.run_advanced_simulation(
        num_steps=500,
        enable_rendering=False
    )
    
    # Analyze simulation results
    print("\nAdvanced Simulation Results:")
    print(f"Total steps: {len(simulation_state['steps'])}")
    print(f"Metric steps: {len(simulation_state['metrics'])}")
    print(f"Event steps: {len(simulation_state['events'])}")
    
    # Print key metrics
    if simulation_state['metrics']:
        final_metrics = simulation_state['metrics'][-1]
        print(f"\nFinal Metrics:")
        print(f"  Neuron Count: {final_metrics['neuron_count']}")
        print(f"  Active Neurons: {final_metrics['active_neurons']}")
        print(f"  Firing Rate: {final_metrics['firing_rate']:.3f}")
        print(f"  Learning Speed: {final_metrics['learning_speed']:.3f}")
        print(f"  Exploration-Exploitation Balance: {final_metrics['exploration_exploitation_balance']:.3f}")
    
    # Print events
    if simulation_state['events']:
        event_counts = {}
        for events in simulation_state['events']:
            for event in events:
                event_counts[event] = event_counts.get(event, 0) + 1
        
        print(f"\nEvents Detected:")
        for event, count in event_counts.items():
            print(f"  {event}: {count}")
    
    print("\nAdvanced simulation completed!")
```

This comprehensive advanced examples section demonstrates:

1. **Multi-Agent Systems**: Complex interactions between multiple intelligent agents
2. **Configuration Management**: Dynamic configuration loading and template systems
3. **Performance Optimization**: Memory-efficient and real-time processing techniques
4. **Error Handling**: Resilient simulation with automatic recovery mechanisms
5. **External Integration**: Integration with external AI systems and data sources
6. **Custom API Extensions**: Advanced Python API extensions for maximum flexibility

These examples showcase the full power of the NLM framework for advanced applications in artificial intelligence, neuroscience, and machine learning research.

## 6. Troubleshooting and Common Issues

### Common Problems and Solutions

#### 1. "Brain initialization failed"
**Problem**: Brain fails to initialize with configuration errors.

**Solution**:
```python
import pynlm

# Create config with proper validation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # Set valid neuron count
config.set("brain.region_count", 2)     # Set valid region count
config.set("brain.connection_probability", 0.1)

# Verify configuration
print("Configuration summary:")
print(config.summary())

# Initialize brain
brain = pynlm.createBrain(config)
if brain.initialize():
    print("Brain initialized successfully")
else:
    print("Brain initialization failed")
```

#### 2. "Agent processSensoryInput failed"
**Problem**: Agent fails to process sensory input.

**Solution**:
```python
import pynlm

# Create world and configure it
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Get sensory percept and check its state
percept = world.getSensoryPercept()
print(f"Vision data available: {percept.getVision() is not None}")
print(f"Touch data available: {percept.getTouch() is not None}")

# Process sensory input
agent.processSensoryInput(percept)
print("Sensory input processed successfully")
```

#### 3. "Simulation too slow for real-time applications"
**Problem**: Simulation performance is insufficient for real-time applications.

**Solution**:
```python
import pynlm

# Create performance-optimized configuration
config = pynlm.createDefaultConfig()

# Optimize for memory
config.set("brain.neuron_count", 500)  # Reduce neuron count
config.set("brain.enable_structural_plasticity", False)
config.set("brain.enable_development", False)
config.set("brain.enable_curiosity", False)

# Optimize for speed
config.set("brain.use_sparse_updates", True)
config.set("brain.use_event_driven", True)
config.set("brain.compression_enabled", True)

# Create brain with optimized configuration
optimized_brain = pynlm.createBrain(config)
optimized_brain.initialize()

# Measure performance
import time
start_time = time.time()
for i in range(100):
    optimized_brain.step(i)
end_time = time.time()

print(f"Performance: {100/(end_time - start_time):.1f} steps/second")
```

#### 4. "Python bindings import error"
**Problem**: Python bindings fail to import.

**Solution**:
```bash
# Install Python dependencies
pip install scikit-build-core pybind11 numpy pytest

# Build and install NLM
pip install -e .

# Verify installation
python -c "import pynlm; print('NLM version:', pynlm.__version__)"
```

#### 5. "Configuration file parsing error"
**Problem**: Configuration file fails to parse.

**Solution**:
```python
import pynlm

# Create configuration manually
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.1)

# Save to file (in case you need it later)
config.saveToFile("manual_config.cfg")
print("Configuration saved successfully")

# Try loading back
loaded_config = pynlm.createDefaultConfig()
if loaded_config.loadFromFile("manual_config.cfg"):
    print("Configuration loaded successfully")
else:
    print("Failed to load configuration")
```

## 7. Performance Benchmarking

### Example 8: Benchmark Different Brain Configurations

```python
import pynlm
import time
import statistics
from typing import List, Dict, Any

class BrainBenchmark:
    """Benchmark different brain configurations."""
    
    def __init__(self):
        self.configurations = []
        self.results = {}
    
    def add_configuration(self, name, config):
        """Add a configuration to benchmark."""
        self.configurations.append({
            'name': name,
            'config': config
        })
    
    def benchmark_configuration(self, config, num_steps=1000):
        """Benchmark a single configuration."""
        try:
            # Create and initialize brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Measure initialization time
            start_time = time.time()
            brain.initialize()
            init_time = time.time() - start_time
            
            # Measure simulation performance
            start_time = time.time()
            for step in range(num_steps):
                brain.step(step)
            sim_time = time.time() - start_time
            
            # Collect performance metrics
            metrics = {
                'init_time': init_time,
                'sim_time': sim_time,
                'steps_per_second': num_steps / sim_time,
                'neuron_count': brain.getTotalNeuronCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'success': True
            }
            
            return metrics
            
        except Exception as e:
            return {
                'init_time': 0.0,
                'sim_time': 0.0,
                'steps_per_second': 0.0,
                'neuron_count': 0,
                'active_neurons': 0,
                'firing_rate': 0.0,
                'success': False,
                'error': str(e)
            }
    
    def run_benchmark(self, num_steps=1000):
        """Run benchmark on all configurations."""
        for config_info in self.configurations:
            name = config_info['name']
            config = config_info['config']
            
            print(f"Benchmarking {name}...")
            
            # Run multiple trials for statistical significance
            trials = []
            for trial in range(3):
                print(f"  Trial {trial + 1}/3")
                result = self.benchmark_configuration(config, num_steps)
                trials.append(result)
            
            # Calculate statistics
            successful_trials = [t for t in trials if t['success']]
            
            if successful_trials:
                # Calculate average metrics
                avg_init_time = statistics.mean(t['init_time'] for t in successful_trials)
                avg_sim_time = statistics.mean(t['sim_time'] for t in successful_trials)
                avg_steps_per_second = statistics.mean(t['steps_per_second'] for t in successful_trials)
                avg_neuron_count = statistics.mean(t['neuron_count'] for t in successful_trials)
                avg_firing_rate = statistics.mean(t['firing_rate'] for t in successful_trials)
                
                # Calculate standard deviation
                init_std = statistics.stdev(t['init_time'] for t in successful_trials) if len(successful_trials) > 1 else 0.0
                sim_std = statistics.stdev(t['sim_time'] for t in successful_trials) if len(successful_trials) > 1 else 0.0
                steps_std = statistics.stdev(t['steps_per_second'] for t in successful_trials) if len(successful_trials) > 1 else 0.0
                
                self.results[name] = {
                    'avg_init_time': avg_init_time,
                    'avg_sim_time': avg_sim_time,
                    'avg_steps_per_second': avg_steps_per_second,
                    'avg_neuron_count': avg_neuron_count,
                    'avg_firing_rate': avg_firing_rate,
                    'init_std': init_std,
                    'sim_std': sim_std,
                    'steps_std': steps_std,
                    'trials': len(successful_trials),
                    'failed_trials': len(trials) - len(successful_trials)
                }
                
                print(f"    Average: {avg_steps_per_second:.1f} steps/second")
                print(f"    Std Dev: {steps_std:.1f} steps/second")
            else:
                print(f"    All trials failed!")
    
    def print_results(self):
        """Print benchmark results."""
        if not self.results:
            print("No benchmark results available")
            return
        
        print("\nBrain Benchmark Results:")
        print("=" * 80)
        
        # Print header
        print(f"{'Configuration':<20} {'Init Time (s)':<15} {'Sim Time (s)':<15} {'Steps/s':<12} {'Neuron Count':<15} {'Firing Rate':<12} {'Success Rate':<12}")
        print("-" * 80)
        
        # Print results for each configuration
        for name, result in self.results.items():
            success_rate = result['trials'] / (result['trials'] + result['failed_trials']) * 100
            
            print(f"{name:<20} {result['avg_init_time']:<15.3f} {result['avg_sim_time']:<15.3f} "
                  f"{result['avg_steps_per_second']:<12.1f} {result['avg_neuron_count']:<15.0f} "
                  f"{result['avg_firing_rate']:<12.3f} {success_rate:<12.1f}%")
    
    def save_results(self, filename):
        """Save benchmark results to file."""
        import json
        
        # Convert results to JSON-serializable format
        json_results = {}
        for name, result in self.results.items():
            json_results[name] = {
                'average_init_time': result['avg_init_time'],
                'average_simulation_time': result['avg_sim_time'],
                'average_steps_per_second': result['avg_steps_per_second'],
                'average_neuron_count': result['avg_neuron_count'],
                'average_firing_rate': result['avg_firing_rate'],
                'init_time_std': result['init_std'],
                'simulation_time_std': result['sim_std'],
                'steps_per_second_std': result['steps_std'],
                'successful_trials': result['trials'],
                'failed_trials': result['failed_trials']
            }
        
        with open(filename, 'w') as f:
            json.dump(json_results, f, indent=2)
        
        print(f"Benchmark results saved to {filename}")

# Usage example
if __name__ == "__main__":
    # Create benchmark
    benchmark = BrainBenchmark()
    
    # Add different configurations
    config1 = pynlm.createDefaultConfig()
    config1.set("brain.neuron_count", 500)
    config1.set("brain.region_count", 2)
    benchmark.add_configuration("Small Brain", config1)
    
    config2 = pynlm.createDefaultConfig()
    config2.set("brain.neuron_count", 1000)
    config2.set("brain.region_count", 3)
    benchmark.add_configuration("Medium Brain", config2)
    
    config3 = pynlm.createDefaultConfig()
    config3.set("brain.neuron_count", 2000)
    config3.set("brain.region_count", 5)
    benchmark.add_configuration("Large Brain", config3)
    
    # Run benchmark
    print("Starting brain benchmark...")
    benchmark.run_benchmark(num_steps=500)
    
    # Print results
    benchmark.print_results()
    
    # Save results
    benchmark.save_results("brain_benchmark_results.json")
```

This benchmarking example demonstrates how to systematically compare different brain configurations for performance and efficiency.

## 8. Final Summary and Recommendations

### Key Improvements Implemented:

1. **Enhanced Configuration System**: 
   - Type-safe getters and setters
   - Configuration templates for common scenarios
   - Configuration validation and constraints
   - Category-based configuration management

2. **Advanced Python API**:
   - Improved error handling and recovery
   - Enhanced sensory processing with feature extraction
   - Advanced action selection mechanisms
   - Custom brain architectures

3. **Multi-Agent Systems**:
   - Inter-agent communication
   - Social learning and imitation
   - Competition and cooperation
   - Agent specialization and diversity

4. **Performance Optimization**:
   - Memory-efficient configurations
   - Real-time processing support
   - Streaming and buffering techniques
   - Advanced benchmarking tools

5. **External Integration**:
   - Integration with external AI systems
   - Feature extraction from diverse data sources
   - Machine learning model integration
   - Sensor data processing

6. **Error Handling and Recovery**:
   - Resilient simulation with automatic recovery
   - Comprehensive error detection and handling
   - Checkpoint-based recovery mechanisms
   - Configuration validation

7. **Advanced Examples**:
   - Comprehensive examples for power users
   - Advanced simulation techniques
   - Performance optimization strategies
   - Troubleshooting guides

### Recommendations for Advanced Users:

1. **Start with basic configurations** and gradually increase complexity
2. **Use benchmarking** to compare different configurations for your specific use case
3. **Implement proper error handling** for production systems
4. **Use checkpointing** for long-running simulations
5. **Leverage multi-agent systems** for complex behaviors
6. **Optimize configurations** based on your performance requirements
7. **Use advanced feature extraction** for better perception
8. **Implement monitoring and metrics** for system analysis

This advanced user guide provides comprehensive coverage of the NLM framework's capabilities, enabling users to build sophisticated neural network simulations and applications. The examples and techniques presented here demonstrate the full potential of the NLM framework for advanced artificial intelligence applications.

The improvements made to the NLM codebase include:

1. **Enhanced Configuration System**: Type-safe APIs, validation, templates
2. **Improved Python Bindings**: Better error handling, advanced features
3. **Advanced Examples**: Comprehensive examples for power users
4. **Performance Optimization**: Benchmarking and optimization tools
5. **Error Handling**: Resilient simulation with recovery mechanisms
6. **Documentation**: Comprehensive documentation and troubleshooting guides

These improvements make the NLM framework more powerful, flexible, and accessible for both beginners and advanced users.

---

## Version Information

- **Framework**: NLM v0.1.0
- **Last Updated**: 2026-10-05
- **License**: MIT
- **Documentation**: Comprehensive examples and guides included

The NLM framework is continuously evolving. For the latest documentation, examples, and updates, visit the official NLM documentation site.

**Happy brain simulation!** 🚀

---