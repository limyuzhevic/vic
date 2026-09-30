# NLM Advanced Examples Guide

This document provides comprehensive examples for sophisticated NLM use cases, including multi-brain architectures, complex world dynamics, and advanced learning systems.

## Table of Contents

1. [Multi-Brain Architecture](#multi-brain-architecture)
2. [Complex World Dynamics](#complex-world-dynamics)
3. [Advanced Learning Systems](#advanced-learning-systems)
4. [Memory Management](#memory-management)
5. [Neuromodulation Networks](#neuromodulation-networks)
6. [Developmental Psychology](#developmental-psychology)
7. [Cognitive Architecture](#cognitive-architecture)
8. [Performance Optimization](#performance-optimization)
9. [Real-time Control](#real-time-control)
10. [Research Experiments](#research-experiments)

---

## 1. Multi-Brain Architecture

### Overview
This example demonstrates creating and coordinating multiple interconnected brains, enabling collaborative intelligence and distributed cognition.

```python
import pynlm
import numpy as np
import time

class MultiBrainSystem:
    """Multi-brain architecture with specialized functions."""
    
    def __init__(self, num_brains=3):
        self.num_brains = num_brains
        self.brains = []
        self.agents = []
        self.shared_world = None
        self.synchronization_frequency = 10  # Steps between sync
        self.communication_channels = []
        
        # Create specialized brain configurations
        self.create_brain_configurations()
        self.setup_communication_network()
        
    def create_brain_configurations(self):
        """Create different brain types for distributed tasks."""
        brain_configs = [
            {
                "neuron_count": 2000,
                "synapse_density": 0.08,
                "plasticity_type": "STDP",
                "role": "sensorimotor",
                "specialization": "visual_processing"
            },
            {
                "neuron_count": 1500,
                "synapse_density": 0.06,
                "plasticity_type": "Hebbian",
                "role": "working_memory",
                "specialization": "short_term_memory"
            },
            {
                "neuron_count": 3000,
                "synapse_density": 0.10,
                "plasticity_type": "Reward_Modulated",
                "role": "decision_making",
                "specialization": "goal_directed"
            }
        ]
        
        for i, config_dict in enumerate(brain_configs[:self.num_brains]):
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", config_dict["neuron_count"])
            config.set("brain.synapse_density", config_dict["synapse_density"])
            config.set(f"brain.plasticity_type", config_dict["plasticity_type"])
            config.set(f"brain.role", config_dict["role"])
            config.set(f"brain.specialization", config_dict["specialization"])
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            self.brains.append(brain)
            
            agent = pynlm.createAgentBrain(brain)
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            agent.enableStructuralPlasticity(True)
            
            self.agents.append(agent)
    
    def setup_communication_network(self):
        """Setup inter-brain communication channels."""
        for i in range(self.num_brains):
            # Create attention-based communication
            channel = {
                'source_brain': i,
                'attention_weights': np.ones(self.num_brains) / self.num_brains,
                'connection_strength': 0.5,
                'modulation_type': 'dopamine',
                'phase': 0.0
            }
            self.communication_channels.append(channel)
    
    def create_shared_world(self):
        """Create a complex world for multi-brain interaction."""
        self.shared_world = pynlm.createSimpleWorld()
        
        # Configure rich environment
        self.shared_world.configure(
            width=50,
            height=50,
            visionWidth=16,
            visionHeight=16
        )
        
        # Add multiple interacting objects
        self.add_environment_objects()
        self.setup_resource_dynamics()
        self.create_obstacle_map()
        
        self.shared_world.reset()
    
    def add_environment_objects(self):
        """Add dynamic objects to the world."""
        # Resources that appear periodically
        for i in range(10):
            x = np.random.uniform(5, 45)
            y = np.random.uniform(5, 45)
            resource = pynlm.WorldObject(
                x, y, pynlm.WorldObjectType.Resource,
                value=np.random.uniform(10, 100)
            )
            self.shared_world.addObject(resource)
    
    def setup_resource_dynamics(self):
        """Setup resource regeneration and consumption."""
        # This would be implemented in the world update loop
        pass
    
    def create_obstacle_map(self):
        """Create complex obstacle layout."""
        # Create walls and obstacles for navigation
        pass
    
    def synchronize_brains(self, step):
        """Synchronize brains through attention and communication."""
        if step % self.synchronization_frequency == 0:
            # Calculate attention patterns
            attention_matrix = self.calculate_attention_patterns()
            
            # Exchange information between brains
            self.exchange_information(attention_matrix)
            
            # Synchronize developmental stages
            self.synchronize_development()
            
            # Update neuromodulation states
            self.synchronize_neuromodulation()
    
    def calculate_attention_patterns(self):
        """Calculate attention patterns between brains."""
        n = len(self.brains)
        attention = np.zeros((n, n))
        
        for i in range(n):
            for j in range(n):
                if i != j:
                    # Attention decays with distance in network
                    attention[i, j] = np.exp(-abs(i - j) / 2.0)
        
        return attention
    
    def exchange_information(self, attention_matrix):
        """Exchange information between brains based on attention."""
        for i, agent in enumerate(self.agents):
            # Create composite percept from other brains
            composite_percept = self.create_composite_percept(i, attention_matrix)
            
            # Inject into current brain
            agent.processSensoryInput(composite_percept)
            
            # Update brain's prediction system
            brain = agent.getBrain()
            # This would use brain-specific prediction mechanisms
    
    def create_composite_percept(self, source_brain_idx, attention_matrix):
        """Create a composite sensory percept from multiple sources."""
        # In a real implementation, this would combine:
        # - Visual information from other agents
        # - Motor patterns from other brains
        # - Memory states from shared memory system
        
        # Placeholder implementation
        vision_data = np.random.random((16, 16, 3))
        touch_data = np.random.random(6)
        internal_data = np.random.random(4)
        
        percept = pynlm.SensoryPercept()
        percept.setVision(pynlm.Vision(16, 16, 3))
        percept.setTouch(touch_data)
        percept.setInternal(internal_data)
        
        return percept
    
    def synchronize_development(self):
        """Synchronize developmental stages across brains."""
        # Calculate average developmental stage
        avg_stage = 0.0
        for agent in self.agents:
            avg_stage += agent.getDevelopmentalStage().value if hasattr(agent.getDevelopmentalStage(), 'value') else 0
        
        avg_stage /= len(self.agents)
        
        # Align brains to average stage for coordination
        for agent in self.agents:
            current_stage = agent.getDevelopmentalStage()
            if hasattr(current_stage, 'value') and current_stage.value < avg_stage:
                # Advance development
                agent.updateDevelopment(0.5)
    
    def synchronize_neuromodulation(self):
        """Synchronize neuromodulation states."""
        # Average neuromodulation levels
        avg_curiosity = 0.0
        avg_prediction_error = 0.0
        avg_dopamine = 0.0
        
        for agent in self.agents:
            avg_curiosity += agent.getCuriosityLevel()
            avg_prediction_error += agent.getPredictionError()
            avg_dopamine += agent.getNeuromodulationLevel()
        
        # Apply synchronized neuromodulation
        for agent in self.agents:
            agent.applyRewardModulation(avg_dopamine / len(self.agents), 
                                      avg_prediction_error / len(self.agents))
    
    def run_simulation(self, num_steps):
        """Run multi-brain coordination simulation."""
        print(f"Starting Multi-Brain System with {self.num_brains} brains")
        
        # Create shared world
        self.create_shared_world()
        
        # Initialize all agents with the world
        for agent in self.agents:
            agent.initialize(self.shared_world)
        
        # Statistics collection
        statistics = {
            'steps': [],
            'collective_firing_rates': [],
            'coordination_scores': [],
            'learning_rates': []
        }
        
        # Main simulation loop
        for step in range(num_steps):
            # Update environment
            self.shared_world.update(0.1)
            
            # Each brain processes its sensory input
            for i, agent in enumerate(self.agents):
                # Get sensory input from world
                percept = self.shared_world.getSensoryPercept()
                
                # Apply agent's specific processing
                processed_percept = self.process_agent_specific_input(
                    agent, percept, i
                )
                
                # Agent processes input
                agent.processSensoryInput(processed_percept)
                
                # Brain computes
                brain = agent.getBrain()
                brain.step(step)
                
                # Decode action
                action = agent.decodeMotorCommand()
                
                # Apply action to world
                self.shared_world.applyMotorCommand(action, self.shared_world.getSimulationTime())
                
                # Apply reward
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(0.1)
            
            # Multi-brain synchronization
            self.synchronize_brains(step)
            
            # Collect statistics
            stats = self.collect_statistics(step)
            for key, value in stats.items():
                statistics[key].append(value)
            
            # Print progress
            if step % 100 == 0:
                self.print_progress(step, stats)
        
        return statistics
    
    def process_agent_specific_input(self, agent, percept, brain_idx):
        """Process input specific to each brain's specialization."""
        # Modify percept based on brain specialization
        if brain_idx == 0:  # Visual processing brain
            # Enhance visual features
            vision = percept.getVision()
            # Apply feature enhancement (placeholder)
            
        elif brain_idx == 1:  # Working memory brain
            # Enhance temporal features
            pass
            
        elif brain_idx == 2:  # Decision making brain
            # Enhance reward prediction
            pass
        
        return percept
    
    def collect_statistics(self, step):
        """Collect simulation statistics."""
        stats = {
            'step': step,
            'collective_firing_rate': 0.0,
            'coordination_score': 0.0,
            'learning_rate': 0.0
        }
        
        # Calculate collective firing rate
        total_firing = 0
        for brain in self.brains:
            total_firing += brain.getFiringNeuronCount()
        
        stats['collective_firing_rate'] = total_firing / len(self.brains)
        
        # Calculate coordination score (simplified)
        coordination = 0.0
        for i in range(self.num_brains):
            for j in range(i + 1, self.num_brains):
                # Calculate correlation in activity patterns
                pass  # Would need activity pattern comparison
        
        stats['coordination_score'] = coordination / (self.num_brains * (self.num_brains - 1) / 2)
        
        # Calculate learning rate (changes in firing rates)
        learning_rate = 0.0
        # Would track changes over time
        
        stats['learning_rate'] = learning_rate
        
        return stats
    
    def print_progress(self, step, stats):
        """Print simulation progress."""
        print(f"Step {step:4d}: "
              f"Firing Rate: {stats['collective_firing_rate']:.1f}, "
              f"Coordination: {stats['coordination_score']:.3f}, "
              f"Learning Rate: {stats['learning_rate']:.4f}")

# Complete usage example
def multi_brain_example():
    """Run the multi-brain architecture example."""
    print("=" * 60)
    print("NLM Multi-Brain Architecture Example")
    print("=" * 60)
    
    # Create multi-brain system
    multi_brain = MultiBrainSystem(num_brains=3)
    
    # Run simulation
    results = multi_brain.run_simulation(num_steps=1000)
    
    print("\n" + "=" * 60)
    print("Multi-Brain Simulation Complete!")
    print("=" * 60)
    print(f"Total steps: {len(results['steps'])}")
    print(f"Final average firing rate: {results['collective_firing_rates'][-1]:.2f}")
    print(f"Final coordination score: {results['coordination_scores'][-1]:.4f}")
    print(f"Final learning rate: {results['learning_rates'][-1]:.4f}")
    
    # Example of saving results
    import json
    # multi_brain.save_results(results, "multi_brain_results.json")
    
    return results

if __name__ == "__main__":
    multi_brain_example()
```

## 2. Complex World Dynamics

### Overview
This example demonstrates designing rich environments with multiple interacting objects, physics simulation, and emergent behaviors.

```python
import pynlm
import numpy as np
import math

class ComplexWorldEnvironment:
    """Complex world with physics, ecology, and emergent behaviors."""
    
    def __init__(self, width=100, height=100):
        self.width = width
        self.height = height
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=width,
            height=height,
            visionWidth=16,
            visionHeight=16
        )
        
        # Environmental systems
        self.resources = []
        self.predators = []
        self.prey = []
        self.obstacles = []
        self.weather_system = WeatherSystem()
        self.climate_zones = ClimateZoneSystem()
        self.seasonal_changes = SeasonalChangeSystem()
        
        # Physics simulation
        self.particle_system = ParticleSystem()
        self.fluid_dynamics = FluidDynamics()
        self.terrain_generator = TerrainGenerator()
        
        # Initialize world
        self.initialize_environment()
        
    def initialize_environment(self):
        """Initialize complex environmental systems."""
        # Generate terrain
        self.terrain = self.terrain_generator.generate_terrain(
            self.width, self.height
        )
        
        # Create ecological zones
        self.create_ecological_zones()
        
        # Setup atmospheric effects
        self.setup_atmospheric_effects()
        
        # Initialize resource dynamics
        self.initialize_resource_systems()
        
        # Create predator-prey dynamics
        self.setup_predator_prey_system()
        
        self.world.reset()
    
    def create_ecological_zones(self):
        """Create different ecological zones."""
        self.ecological_zones = {
            'forest': {'density': 0.7, 'richness': 0.8, 'complexity': 0.9},
            'desert': {'density': 0.3, 'richness': 0.2, 'complexity': 0.4},
            'ocean': {'density': 0.0, 'richness': 0.6, 'complexity': 0.5},
            'mountain': {'density': 0.5, 'richness': 0.4, 'complexity': 0.7}
        }
        
        # Distribute zones based on terrain
        self.zone_map = self.create_zone_map()
    
    def create_zone_map(self):
        """Create map of ecological zones."""
        zone_map = np.zeros((self.height, self.width), dtype=int)
        
        # Assign zones based on terrain height and type
        for y in range(self.height):
            for x in range(self.width):
                terrain_type = self.terrain[y, x]
                zone_type = self.determine_zone_type(terrain_type, x, y)
                zone_map[y, x] = zone_type
        
        return zone_map
    
    def determine_zone_type(self, terrain_type, x, y):
        """Determine ecological zone based on terrain."""
        # Deterministic assignment based on terrain features
        if terrain_type < 0.3:
            return 0  # Desert
        elif terrain_type < 0.6:
            return 1  # Forest
        elif terrain_type < 0.8:
            return 2  # Mountain
        else:
            return 3  # Ocean
    
    def setup_atmospheric_effects(self):
        """Setup weather and atmospheric effects."""
        self.atmosphere = {
            'temperature': 25.0,
            'humidity': 0.5,
            'pressure': 1013.25,
            'wind_speed': 5.0,
            'wind_direction': 0.0,
            'precipitation': 0.0,
            'visibility': 100.0,
            'air_quality': 1.0
        }
        
        # Weather patterns
        self.weather_pattern = WeatherPattern()
    
    def initialize_resource_systems(self):
        """Initialize dynamic resource systems."""
        # Renewable resources
        self.renewable_resources = []
        for i in range(20):
            x = np.random.uniform(5, self.width - 5)
            y = np.random.uniform(5, self.height - 5)
            resource_type = np.random.choice(['water', 'nutrients', 'minerals', 'energy'])
            
            resource = {
                'position': (x, y),
                'type': resource_type,
                'amount': np.random.uniform(50, 200),
                'regeneration_rate': np.random.uniform(0.1, 1.0),
                'capacity': np.random.uniform(200, 500),
                'accessibility': self.calculate_accessibility(x, y)
            }
            
            self.renewable_resources.append(resource)
    
    def calculate_accessibility(self, x, y):
        """Calculate resource accessibility based on terrain."""
        # Lower accessibility for steep terrain
        terrain_height = self.terrain[int(y), int(x)]
        distance_penalty = np.sqrt((x - self.width/2)**2 + (y - self.height/2)**2)
        
        accessibility = (1.0 - terrain_height) * (1.0 - distance_penalty / (self.width/2))
        return max(0.1, min(1.0, accessibility))
    
    def setup_predator_prey_system(self):
        """Setup predator-prey population dynamics."""
        # Initial populations
        self.predator_population = 10
        self.prey_population = 100
        
        # Predator characteristics
        self.predator_params = {
            'base_energy': 100.0,
            'energy_cost': 2.0,
            'hunger_rate': 0.5,
            'reproduction_threshold': 50.0,
            'vision_range': 8,
            'speed': 2.0,
            'aggression': 0.7
        }
        
        # Prey characteristics
        self.prey_params = {
            'base_energy': 60.0,
            'energy_cost': 1.5,
            'fear_rate': 0.3,
            'reproduction_threshold': 30.0,
            'vision_range': 6,
            'speed': 1.5,
            'stealth': 0.6
        }
    
    def setup_agent_interaction(self, agent):
        """Setup agent to interact with complex world."""
        agent.initialize(self.world)
        
        # Enable advanced perception
        agent.enableCuriosity(True)
        agent.enableRewardModulation(True)
        agent.enableDevelopment(True)
        
        # Add custom sensory processing
        agent.addSensorProcessing(self.process_environmental_percepts)
    
    def process_environmental_percepts(self, percept):
        """Process environmental percepts with ecological awareness."""
        # Enhance percept with environmental context
        zone_type = self.get_zone_at_position(percept)
        
        # Add environmental features to percept
        environmental_features = {
            'zone_type': zone_type,
            'temperature': self.atmosphere['temperature'],
            'humidity': self.atmosphere['humidity'],
            'resource_abundance': self.get_resource_abundance(percept),
            'predator_presence': self.detect_predators(percept),
            'prey_presence': self.detect_prey(percept)
        }
        
        # Integrate environmental features with existing percept
        # This would modify the brain's processing
        
        return environmental_features
    
    def get_zone_at_position(self, percept):
        """Get ecological zone at agent's position."""
        # Would need to track agent position
        return self.zone_map[0, 0]  # Placeholder
    
    def get_resource_abundance(self, percept):
        """Get resource abundance in agent's area."""
        # Calculate resource availability in local area
        return 0.5  # Placeholder
    
    def detect_predators(self, percept):
        """Detect predator presence."""
        return 0.0  # Placeholder
    
    def detect_prey(self, percept):
        """Detect prey presence."""
        return 0.0  # Placeholder
    
    def run_simulation(self, num_steps):
        """Run complex world simulation."""
        print(f"Starting Complex World Simulation ({self.width}x{self.height})")
        
        statistics = {
            'steps': list(range(num_steps)),
            'predator_population': [],
            'prey_population': [],
            'temperature': [],
            'resource_availability': [],
            'environmental_stability': []
        }
        
        for step in range(num_steps):
            # Update environmental systems
            self.update_environment(step)
            
            # Update ecological populations
            self.update_ecological_dynamics(step)
            
            # Update world state
            self.world.update(0.1)
            
            # Collect statistics
            self.collect_environmental_statistics(step, statistics)
            
            # Print progress
            if step % 100 == 0:
                self.print_environmental_status(step, statistics)
        
        return statistics
    
    def update_environment(self, step):
        """Update environmental systems."""
        # Update weather system
        self.weather_system.update(step)
        self.atmosphere = self.weather_system.get_state()
        
        # Update seasonal changes
        self.seasonal_changes.update(step)
        
        # Update fluid dynamics
        self.particle_system.update(step)
        
        # Update resource regeneration
        self.update_resource_regeneration(step)
        
        # Update world objects based on new conditions
        self.update_world_objects(step)
    
    def update_resource_regeneration(self, step):
        """Update resource regeneration."""
        for resource in self.renewable_resources:
            # Regeneration based on environmental conditions
            regeneration_rate = resource['regeneration_rate'] * (
                0.5 + 0.5 * self.atmosphere['humidity']
            )
            
            resource['amount'] += regeneration_rate
            resource['amount'] = min(resource['amount'], resource['capacity'])
    
    def update_world_objects(self, step):
        """Update dynamic world objects."""
        # This would update world objects based on physics and environmental changes
        pass
    
    def update_ecological_dynamics(self, step):
        """Update predator-prey population dynamics."""
        # Lotka-Volterra style dynamics
        prey_eaten = min(self.prey_population, self.predator_population * 0.1)
        self.prey_population -= prey_eaten
        
        self.predator_population += (prey_eaten * 0.5) - (self.predator_population * 0.05)
        
        # Reproduction
        if self.prey_population > 50:
            self.prey_population += self.prey_population * 0.02
        
        if self.predator_population > 20:
            self.predator_population += self.predator_population * 0.01
        
        # Keep populations in reasonable bounds
        self.prey_population = max(10, min(500, self.prey_population))
        self.predator_population = max(5, min(100, self.predator_population))
    
    def collect_environmental_statistics(self, step, statistics):
        """Collect environmental statistics."""
        statistics['predator_population'].append(self.predator_population)
        statistics['prey_population'].append(self.prey_population)
        statistics['temperature'].append(self.atmosphere['temperature'])
        statistics['resource_availability'].append(self.calculate_resource_availability())
        statistics['environmental_stability'].append(self.calculate_environmental_stability())
    
    def calculate_resource_availability(self):
        """Calculate overall resource availability."""
        total_resources = sum(r['amount'] for r in self.renewable_resources)
        max_resources = sum(r['capacity'] for r in self.renewable_resources)
        return total_resources / max_resources
    
    def calculate_environmental_stability(self):
        """Calculate environmental stability score."""
        # Stability based on population balance and resource availability
        population_balance = abs(
            self.predator_population - self.prey_population
        ) / max(self.prey_population, 1)
        
        resource_stability = abs(
            1 - self.calculate_resource_availability()
        )
        
        return 1.0 - (population_balance * 0.6 + resource_stability * 0.4)
    
    def print_environmental_status(self, step, statistics):
        """Print environmental status."""
        print(f"Step {step:4d}: "
              f"Predators: {self.predator_population:4.0f}, "
              f"Prey: {self.prey_population:4.0f}, "
              f"Resources: {statistics['resource_availability'][-1]:.2f}, "
              f"Stability: {statistics['environmental_stability'][-1]:.2f}")

def complex_world_example():
    """Run the complex world dynamics example."""
    print("=" * 60)
    print("NLM Complex World Dynamics Example")
    print("=" * 60)
    
    # Create complex world
    world = ComplexWorldEnvironment(width=100, height=100)
    
    # Run simulation
    results = world.run_simulation(num_steps=1000)
    
    print("\n" + "=" * 60)
    print("Complex World Simulation Complete!")
    print("=" * 60)
    print(f"Total steps: {len(results['steps'])}")
    print(f"Final predator population: {results['predator_population'][-1]:.0f}")
    print(f"Final prey population: {results['prey_population'][-1]:.0f}")
    print(f"Final resource availability: {results['resource_availability'][-1]:.2f}")
    print(f"Average environmental stability: {np.mean(results['environmental_stability']):.2f}")
    
    return results

if __name__ == "__main__":
    # Note: This is a comprehensive example - actual implementation would need
    # integration with NLM's specific C++ classes and Python bindings
    print("Complex world example requires full NLM integration")
    print("This is a template showing the structure and key concepts.")
    
    # complex_world_example()
```

## 3. Advanced Learning Systems

### Overview
This example demonstrates advanced learning techniques including reward prediction, curiosity-driven exploration, and meta-learning in NLM.

```python
import pynlm
import numpy as np
import math

class AdvancedLearningSystem:
    """Advanced learning system with multiple learning mechanisms."""
    
    def __init__(self):
        # Create brain with advanced configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 5000)
        config.set("brain.synapse_density", 0.06)
        
        # Initialize core systems
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create environment
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=50, height=50, visionWidth=12, visionHeight=12)
        
        # Initialize learning agents
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Enable advanced learning features
        self.setup_advanced_learning()
        
        # Learning system components
        self.reward_predictor = RewardPredictor()
        self.curiosity_system = CuriositySystem()
        self.meta_learner = MetaLearner()
        self.adaptive_planner = AdaptivePlanner()
        
        self.initialize_learning_systems()
        
    def setup_advanced_learning(self):
        """Setup advanced learning capabilities."""
        # Enable all advanced learning systems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        # Advanced brain configuration
        self.brain_config = AdvancedBrainConfig()
        
        # Learning rate adapters
        self.reward_learning_rate = 0.1
        self.curiosity_learning_rate = 0.05
        self.meta_learning_rate = 0.02
    
    def initialize_learning_systems(self):
        """Initialize advanced learning system components."""
        # Reward prediction system
        self.reward_predictor = RewardPredictor(
            prediction_window=10,
            confidence_threshold=0.8
        )
        
        # Curiosity system
        self.curiosity_system = CuriositySystem(
            exploration_bonus=0.5,
            novelty_threshold=0.3,
            intrinsic_motivation=True
        )
        
        # Meta-learning system
        self.meta_learner = MetaLearner(
            meta_learning_rate=0.01,
            adaptation_threshold=0.7,
            transfer_learning=True
        )
        
        # Adaptive planning
        self.adaptive_planner = AdaptivePlanner(
            planning_horizon=20,
            action_selection='ucb1',  # Upper confidence bound
            uncertainty_aware=True
        )
    
    def run_adaptive_learning_cycle(self, num_steps):
        """Run adaptive learning cycle with multiple learning mechanisms."""
        print("Starting Advanced Learning Cycle")
        print("=" * 50)
        
        learning_metrics = {
            'steps': list(range(num_steps)),
            'predicted_rewards': [],
            'actual_rewards': [],
            'curiosity_scores': [],
            'prediction_errors': [],
            'meta_learning_progress': [],
            'adaptation_rates': [],
            'performance_scores': []
        }
        
        # Initialize tracking variables
        prediction_error_history = []
        learning_progress_history = []
        performance_history = []
        
        # Main learning loop
        for step in range(num_steps):
            # 1. Environmental interaction and perception
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            
            # 2. Multi-stage processing
            processed_percept = self.process_with_attention(percept, step)
            
            # 3. Reward prediction
            predicted_reward = self.reward_predictor.predict(processed_percept)
            learning_metrics['predicted_rewards'].append(predicted_reward)
            
            # 4. Intrinsic curiosity assessment
            curiosity_score = self.curiosity_system.calculate_curiosity(
                processed_percept, self.brain
            )
            learning_metrics['curiosity_scores'].append(curiosity_score)
            
            # 5. Agent processes input with enhanced mechanisms
            self.agent.processSensoryInput(processed_percept)
            
            # 6. Brain computation with multi-system integration
            brain_step_result = self.compute_with_advanced_planning(step, curiosity_score)
            
            # 7. Action selection with uncertainty handling
            action = self.adaptive_planner.select_action(
                brain_step_result, predicted_reward, curiosity_score
            )
            
            # 8. Environmental interaction
            world_result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            actual_reward = world_result.get_reward() if hasattr(world_result, 'get_reward') else 0.0
            learning_metrics['actual_rewards'].append(actual_reward)
            
            # 9. Meta-learning and adaptation
            meta_learning_progress = self.meta_learner.update(
                predicted_reward, actual_reward, processed_percept
            )
            learning_metrics['meta_learning_progress'].append(meta_learning_progress)
            
            # 10. Learning rate adaptation
            learning_rates = self.adapt_learning_rates(
                predicted_reward, actual_reward, curiosity_score, step
            )
            learning_metrics['adaptation_rates'].append(learning_rates)
            
            # 11. Performance evaluation
            performance_score = self.evaluate_performance(step, brain_step_result)
            learning_metrics['performance_scores'].append(performance_score)
            
            # 12. Error tracking for prediction
            prediction_error = abs(predicted_reward - actual_reward)
            learning_metrics['prediction_errors'].append(prediction_error)
            prediction_error_history.append(prediction_error)
            
            # 13. Development and maturation
            self.agent.updateDevelopment(0.1)
            
            # 14. Structural plasticity
            self.update_structural_plasticity(step, curiosity_score)
            
            # Record metrics periodically
            if step % 100 == 0:
                self.print_learning_progress(step, learning_metrics)
        
        return learning_metrics
    
    def process_with_attention(self, percept, step):
        """Process percept with advanced attention mechanisms."""
        # Apply selective attention based on curiosity and prediction error
        attention_weights = self.calculate_attention_weights(percept, step)
        
        # Process with focused attention
        enhanced_percept = self.apply_attention_focus(percept, attention_weights)
        
        return enhanced_percept
    
    def calculate_attention_weights(self, percept, step):
        """Calculate attention weights based on multiple factors."""
        n_neurons = self.brain.getTotalNeuronCount()
        attention = np.ones(n_neurons) / n_neurons
        
        # Incorporate curiosity-driven attention
        curiosity_boost = self.curiosity_system.get_curiosity_boost()
        attention *= (1.0 + curiosity_boost)
        
        # Incorporate prediction error-driven attention
        prediction_error = self.agent.getPredictionError()
        if prediction_error > 0.5:
            attention *= (1.0 + prediction_error)
        
        # Temporal attention modulation
        temporal_factor = self.calculate_temporal_attention(step)
        attention *= temporal_factor
        
        # Normalize attention weights
        attention = attention / np.sum(attention)
        
        return attention
    
    def calculate_temporal_attention(self, step):
        """Calculate temporal attention patterns."""
        # Create rhythmic attention patterns
        rhythm_frequency = 0.1  # Hz
        attention_pattern = 0.5 + 0.5 * math.sin(2 * math.pi * rhythm_frequency * step / 10)
        
        # Create spatially varying temporal attention
        temporal_attention = np.full(self.brain.getTotalNeuronCount(), attention_pattern)
        
        return temporal_attention
    
    def apply_attention_focus(self, percept, attention_weights):
        """Apply attention focus to percept processing."""
        # This would apply attention weights to sensory input
        # Implementation depends on NLM's specific processing pipeline
        return percept
    
    def compute_with_advanced_planning(self, step, curiosity_score):
        """Compute brain step with advanced planning integration."""
        # Integrate prediction, curiosity, and planning
        planning_context = self.adaptive_planner.get_planning_context(
            curiosity_score, step
        )
        
        # Update brain with planning information
        self.brain.step(step)
        
        # Get planning-enhanced computation result
        computation_result = self.get_computation_result()
        
        return computation_result
    
    def adapt_learning_rates(self, predicted_reward, actual_reward, curiosity, step):
        """Adapt learning rates based on performance."""
        # Calculate prediction error
        prediction_error = abs(predicted_reward - actual_reward)
        
        # Adapt reward learning rate
        reward_lr = self.reward_learning_rate * (1.0 + prediction_error)
        
        # Adapt curiosity learning rate
        curiosity_lr = self.curiosity_learning_rate * (1.0 + curiosity)
        
        # Adapt meta-learning rate
        meta_lr = self.meta_learning_rate * self.meta_learner.get_adaptation_factor()
        
        # Apply learning rate limits
        reward_lr = min(reward_lr, 0.5)
        curiosity_lr = min(curiosity_lr, 0.3)
        meta_lr = min(meta_lr, 0.1)
        
        return {
            'reward_learning_rate': reward_lr,
            'curiosity_learning_rate': curiosity_lr,
            'meta_learning_rate': meta_lr,
            'prediction_error': prediction_error
        }
    
    def update_structural_plasticity(self, step, curiosity_score):
        """Update structural plasticity based on learning signals."""
        # Create new connections based on curiosity
        if curiosity_score > 0.8 and step % 100 == 0:
            # Synaptogenesis driven by curiosity
            self.brain.addRegion(f"curiosity_region_{step}")
        
        # Prune weak connections based on prediction error
        prediction_error = self.agent.getPredictionError()
        if prediction_error > 0.6:
            # Prune underperforming connections
            self.prune_weak_connections()
    
    def prune_weak_connections(self):
        """Prune weak synaptic connections."""
        # This would implement synaptic pruning based on activity patterns
        # Implementation depends on NLM's plasticity system
        pass
    
    def get_computation_result(self):
        """Get enhanced computation result."""
        # This would capture the brain's computational state
        return {
            'firing_rate': self.brain.getAverageFiringRate(),
            'prediction_error': self.agent.getPredictionError(),
            'curiosity_level': self.agent.getCuriosityLevel()
        }
    
    def evaluate_performance(self, step, computation_result):
        """Evaluate learning performance."""
        # Calculate composite performance score
        firing_rate = computation_result['firing_rate']
        prediction_error = computation_result['prediction_error']
        curiosity_level = computation_result['curiosity_level']
        
        # Optimal firing rate range
        optimal_firing = 5.0  # Hz
        firing_efficiency = 1.0 - abs(firing_rate - optimal_firing) / optimal_firing
        
        # Prediction accuracy
        prediction_accuracy = 1.0 - prediction_error
        
        # Exploration-exploitation balance
        curiosity_efficiency = min(curiosity_level, 1.0) * 0.5
        
        # Composite score
        performance = (
            0.4 * firing_efficiency +
            0.4 * prediction_accuracy +
            0.2 * curiosity_efficiency
        )
        
        return max(0.0, min(1.0, performance))
    
    def print_learning_progress(self, step, metrics):
        """Print learning progress summary."""
        current_step = metrics['steps'][step]
        
        # Get latest metrics
        latest_pred_reward = metrics['predicted_rewards'][step]
        latest_actual_reward = metrics['actual_rewards'][step]
        latest_curiosity = metrics['curiosity_scores'][step]
        latest_error = metrics['prediction_errors'][step]
        latest_meta_progress = metrics['meta_learning_progress'][step]
        latest_performance = metrics['performance_scores'][step]
        
        print(f"Step {current_step:4d}:")
        print(f"  Reward Prediction: {latest_pred_reward:6.3f} -> {latest_actual_reward:6.3f} (error: {latest_error:.3f})")
        print(f"  Curiosity Level:   {latest_curiosity:6.3f}")
        print(f"  Meta-Learning:     {latest_meta_progress:6.3f}")
        print(f"  Performance:       {latest_performance:6.3f}")
        
        # Learning rate adaptation (simplified)
        if step > 0:
            avg_reward_error = np.mean(metrics['prediction_errors'][max(0, step-10):step])
            adaptation_needed = min(avg_reward_error, 1.0)
            print(f"  Adaptation Factor:  {adaptation_needed:.3f}")
    
    def analyze_learning_patterns(self, metrics):
        """Analyze learning patterns and trends."""
        analysis = {}
        
        # Reward prediction accuracy
        pred_errors = metrics['prediction_errors']
        analysis['mean_prediction_error'] = np.mean(pred_errors)
        analysis['prediction_error_variance'] = np.var(pred_errors)
        analysis['prediction_improvement_rate'] = self.calculate_improvement_rate(pred_errors)
        
        # Curiosity patterns
        curiosity = metrics['curiosity_scores']
        analysis['mean_curiosity'] = np.mean(curiosity)
        analysis['curiosity_volatility'] = np.std(curiosity)
        
        # Meta-learning progress
        meta_progress = metrics['meta_learning_progress']
        analysis['meta_learning_convergence'] = self.calculate_convergence_rate(meta_progress)
        
        # Performance trends
        performance = metrics['performance_scores']
        analysis['final_performance'] = performance[-1] if performance else 0.0
        analysis['performance_stability'] = np.std(performance) if len(performance) > 1 else 0.0
        
        return analysis
    
    def calculate_improvement_rate(self, errors):
        """Calculate prediction error improvement rate."""
        if len(errors) < 2:
            return 0.0
        
        # Calculate slope of error reduction
        x = np.arange(len(errors))
        y = np.array(errors)
        
        # Simple linear regression for trend
        slope = np.polyfit(x, y, 1)[0]
        return max(0.0, -slope)  # Positive slope means improvement
    
    def calculate_convergence_rate(self, progress):
        """Calculate meta-learning convergence rate."""
        if len(progress) < 2:
            return 0.0
        
        # Calculate rate of approach to asymptote
        x = np.arange(len(progress))
        y = np.array(progress)
        
        # Fit exponential decay to approach asymptote
        try:
            popt = np.polyfit(x, np.log(np.abs(y - np.min(y)) + 1e-6), 1)
            return -popt[0]  # Decay rate
        except:
            return 0.0

def advanced_learning_example():
    """Run the advanced learning systems example."""
    print("=" * 60)
    print("NLM Advanced Learning Systems Example")
    print("=" * 60)
    
    # Create advanced learning system
    learning_system = AdvancedLearningSystem()
    
    # Run learning cycle
    results = learning_system.run_adaptive_learning_cycle(num_steps=1000)
    
    # Analyze learning patterns
    analysis = learning_system.analyze_learning_patterns(results)
    
    print("\n" + "=" * 60)
    print("Advanced Learning Analysis Complete!")
    print("=" * 60)
    
    # Print key metrics
    print(f"Total learning steps: {len(results['steps'])}")
    print(f"Mean prediction error: {analysis['mean_prediction_error']:.3f}")
    print(f"Mean curiosity level: {analysis['mean_curiosity']:.3f}")
    print(f"Meta-learning convergence: {analysis['meta_learning_convergence']:.3f}")
    print(f"Final performance: {analysis['final_performance']:.3f}")
    print(f"Performance stability: {analysis['performance_stability']:.3f}")
    
    # Save results
    import json
    results_dict = {
        'metrics': {k: (v if not np.isscalar(v) else float(v)) for k, v in results.items()},
        'analysis': analysis
    }
    
    return results, analysis

if __name__ == "__main__":
    # Note: This example demonstrates advanced learning patterns
    # Actual implementation would require complete NLM integration
    print("Advanced learning example requires full NLM integration")
    print("This template shows the structure and key concepts.")
    
    # advanced_learning_example()
```

## 4. Memory Management

### Overview
This example demonstrates comprehensive memory management systems including working memory, episodic memory, and associative memory.

```python
import pynlm
import numpy as np
import math

class MemoryManagementSystem:
    """Advanced memory management system for NLM."""
    
    def __init__(self, brain_size='medium'):
        # Create brain with memory-optimized configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", self.get_brain_size(brain_size))
        config.set("brain.synapse_density", 0.05)
        
        # Initialize core components
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create environment
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        
        # Initialize memory systems
        self.working_memory = WorkingMemorySystem(self.brain)
        self.episodic_memory = EpisodicMemorySystem(self.brain)
        self.associative_memory = AssociativeMemorySystem(self.brain)
        
        # Initialize agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        # Initialize agent with world
        self.agent.initialize(self.world)
        
        # Memory tracking
        self.memory_usage_log = {
            'steps': [],
            'working_memory_usage': [],
            'episodic_memory_usage': [],
            'associative_memory_usage': [],
            'memory_efficiency': []
        }
        
        # Memory management policies
        self.setup_memory_management_policies()
        
    def get_brain_size(self, size):
        """Get brain size based on complexity level."""
        sizes = {
            'small': 1000,
            'medium': 3000,
            'large': 8000,
            'massive': 15000
        }
        return sizes.get(size, 3000)
    
    def setup_memory_management_policies(self):
        """Setup memory management and optimization policies."""
        # Memory consolidation policies
        self.consolidation_policy = {
            'enabled': True,
            'consolidation_interval': 50,
            'consolidation_threshold': 0.7,
            'replay_frequency': 0.3
        }
        
        # Memory compression policies
        self.compression_policy = {
            'enabled': True,
            'compression_threshold': 0.8,
            'compression_ratio': 0.5,
            'loss_tolerance': 0.1
        }
        
        # Memory allocation policies
        self.allocation_policy = {
            'working_memory_fraction': 0.2,
            'episodic_memory_fraction': 0.5,
            'associative_memory_fraction': 0.3,
            'dynamic_allocation': True,
            'emergency_allocation': True
        }
        
        # Memory maintenance policies
        self.maintenance_policy = {
            'pruning_threshold': 0.1,
            'decay_rate': 0.001,
            'immune_system': True,
            'anomaly_detection': True
        }
    
    def run_memory_efficient_simulation(self, num_steps):
        """Run simulation with advanced memory management."""
        print("Starting Memory-Efficient Simulation")
        print("=" * 50)
        
        for step in range(num_steps):
            # 1. Environmental interaction
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            
            # 2. Working memory processing
            working_memory_result = self.working_memory.process(
                percept, step, self.agent.getCuriosityLevel()
            )
            
            # 3. Episodic memory encoding
            episode_id = self.episodic_memory.encode(
                percept, working_memory_result, step
            )
            
            # 4. Associative memory update
            associations = self.associative_memory.update(
                percept, episode_id, self.agent.getBrain()
            )
            
            # 5. Integrate with neural processing
            self.process_with_memory_integration(working_memory_result, episode_id, associations)
            
            # 6. Memory management and optimization
            self.optimize_memory_usage(step)
            
            # 7. Consolidation (periodic)
            if step % self.consolidation_policy['consolidation_interval'] == 0:
                self.consolidate_memories(step)
            
            # 8. Compression (if needed)
            self.compress_memories_if_needed(step)
            
            # 9. Agent processing
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # 10. Action and learning
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            self.agent.updateDevelopment(0.1)
            
            # 11. Record memory usage statistics
            self.record_memory_usage(step)
            
            # 12. Print progress
            if step % 100 == 0:
                self.print_memory_status(step)
        
        return self.memory_usage_log
    
    def process_with_memory_integration(self, working_memory, episode_id, associations):
        """Process with memory system integration."""
        # Integrate working memory contents into neural processing
        self.brain.receiveSensoryInput(working_memory.get_neural_representation())
        
        # Apply episodic memory influences
        episodic_influence = self.episodic_memory.get_influence(episode_id)
        self.apply_episodic_influence(episodic_influence)
        
        # Apply associative memory patterns
        associative_patterns = self.associative_memory.get_patterns(associations)
        self.apply_associative_patterns(associative_patterns)
    
    def apply_episodic_influence(self, influence):
        """Apply episodic memory influence to brain processing."""
        # This would apply episodic memory influences to neural dynamics
        pass
    
    def apply_associative_patterns(self, patterns):
        """Apply associative memory patterns."""
        # This would apply learned associations to neural processing
        pass
    
    def optimize_memory_usage(self, step):
        """Optimize memory usage based on current demands."""
        # Dynamic memory allocation based on task demands
        current_usage = self.get_memory_usage()
        
        # Apply allocation policies
        self.apply_allocation_policies(current_usage, step)
        
        # Emergency memory management if needed
        if self.is_memory_pressure_high(current_usage):
            self.emergency_memory_management(step)
    
    def get_memory_usage(self):
        """Get current memory usage statistics."""
        usage = {
            'working_memory': self.working_memory.get_usage(),
            'episodic_memory': self.episodic_memory.get_usage(),
            'associative_memory': self.associative_memory.get_usage()
        }
        
        return usage
    
    def apply_allocation_policies(self, usage, step):
        """Apply memory allocation policies."""
        total_usage = sum(usage.values())
        
        if self.allocation_policy['dynamic_allocation']:
            # Dynamic reallocation based on learning progress
            learning_efficiency = self.calculate_learning_efficiency(step)
            
            if learning_efficiency < 0.5:
                # Increase working memory for learning
                self.allocation_policy['working_memory_fraction'] = 0.3
                self.allocation_policy['episodic_memory_fraction'] = 0.4
                self.allocation_policy['associative_memory_fraction'] = 0.3
            else:
                # Optimize for efficiency
                self.allocation_policy['working_memory_fraction'] = 0.15
                self.allocation_policy['episodic_memory_fraction'] = 0.6
                self.allocation_policy['associative_memory_fraction'] = 0.25
    
    def is_memory_pressure_high(self, usage):
        """Check if memory pressure is high."""
        total_usage = sum(usage.values())
        max_capacity = 1.0  # Normalized capacity
        
        return total_usage > (max_capacity * 0.9)
    
    def emergency_memory_management(self, step):
        """Emergency memory management procedures."""
        # Compress working memory
        self.working_memory.compress()
        
        # Prioritize recent episodic memories
        self.episodic_memory.prioritize_recent()
        
        # Reduce associative memory usage
        self.associative_memory.reduce_usage()
        
        # Clean up unused associations
        self.associative_memory.clean_unused()
    
    def consolidate_memories(self, step):
        """Consolidate memories into long-term storage."""
        if not self.consolidation_policy['enabled']:
            return
        
        # Select memories for consolidation
        memories_to_consolidate = self.select_memories_for_consolidation(step)
        
        # Replay important memories
        for memory_id in memories_to_consolidate:
            replay_strength = self.calculate_replay_strength(memory_id, step)
            
            if replay_strength > self.consolidation_policy['consolidation_threshold']:
                self.replay_memory(memory_id, replay_strength)
    
    def select_memories_for_consolidation(self, step):
        """Select memories for consolidation based on importance."""
        # Use various criteria to select memories
        importance_scores = {}
        
        # Get all episodic memories
        all_episodes = self.episodic_memory.get_all_episodes()
        
        for episode_id in all_episodes:
            score = self.calculate_episode_importance(episode_id, step)
            importance_scores[episode_id] = score
        
        # Select top memories for consolidation
        sorted_memories = sorted(importance_scores.items(), key=lambda x: x[1], reverse=True)
        num_to_consolidate = max(1, len(sorted_memories) // 10)  # Top 10%
        
        return [episode_id for episode_id, _ in sorted_memories[:num_to_consolidate]]
    
    def calculate_episode_importance(self, episode_id, step):
        """Calculate importance score for an episode."""
        episode = self.episodic_memory.get_episode(episode_id)
        
        # Multiple factors contribute to importance
        importance = 0.0
        
        # Recency (recent episodes are more important)
        recency = 1.0 - min(1.0, (step - episode['timestamp']) / 1000.0)
        importance += 0.3 * recency
        
        # Novelty (novel experiences are more important)
        novelty = self.calculate_episode_novelty(episode)
        importance += 0.2 * novelty
        
        # Emotional significance (reward prediction errors)
        emotion_significance = abs(episode.get('prediction_error', 0))
        importance += 0.3 * emotion_significance
        
        # Learning potential
        learning_potential = self.calculate_learning_potential(episode)
        importance += 0.2 * learning_potential
        
        return min(1.0, importance)
    
    def calculate_episode_novelty(self, episode):
        """Calculate novelty of an episode."""
        # Compare with similar episodes in memory
        similar_episodes = self.associative_memory.find_similar(episode)
        
        if not similar_episodes:
            return 1.0
        
        # Calculate average similarity to similar episodes
        avg_similarity = np.mean([e.get('similarity', 0) for e in similar_episodes])
        
        return 1.0 - avg_similarity
    
    def calculate_learning_potential(self, episode):
        """Calculate learning potential of an episode."""
        # Based on prediction errors and information content
        prediction_error = abs(episode.get('prediction_error', 0))
        information_content = self.calculate_information_content(episode)
        
        return min(1.0, prediction_error * information_content)
    
    def calculate_information_content(self, episode):
        """Calculate information content of an episode."""
        # Shannon entropy of episode features
        features = episode.get('features', [])
        
        if not features:
            return 0.0
        
        # Simple entropy calculation
        unique_features = set(features)
        entropy = -np.sum([len(f) / len(features) * np.log(len(f) / len(features) + 1e-6) 
                         for f in unique_features])
        
        return entropy / np.log(len(features) + 1e-6)
    
    def replay_memory(self, memory_id, replay_strength):
        """Replay memory with specified strength."""
        # Reactivate memory patterns in neural network
        if memory_id.startswith('episode_'):
            # Replay episodic memory
            episode = self.episodic_memory.get_episode(memory_id)
            self.replay_episodic_memory(episode, replay_strength)
        elif memory_id.startswith('association_'):
            # Replay associative memory
            association = self.associative_memory.get_association(memory_id)
            self.replay_associative_memory(association, replay_strength)
    
    def replay_episodic_memory(self, episode, strength):
        """Replay episodic memory with specified strength."""
        # Reactivate neural patterns associated with the episode
        neural_pattern = episode.get('neural_pattern', None)
        if neural_pattern is not None:
            self.brain.receiveSensoryInput(neural_pattern)
    
    def replay_associative_memory(self, association, strength):
        """Replay associative memory."""
        # Reactivate learned associations
        pattern = association.get('pattern', None)
        if pattern is not None:
            self.brain.receiveSensoryInput(pattern)
    
    def compress_memories_if_needed(self, step):
        """Compress memories if memory usage is too high."""
        if not self.compression_policy['enabled']:
            return
        
        usage = self.get_memory_usage()
        total_usage = sum(usage.values())
        
        if total_usage > self.compression_policy['compression_threshold']:
            self.compress_memories()
    
    def compress_memories(self):
        """Compress memory representations."""
        # Compress episodic memories
        self.episodic_memory.compress()
        
        # Compress associative memories
        self.associative_memory.compress()
        
        # Maintain working memory efficiency
        self.working_memory.maintain_efficiency()
    
    def record_memory_usage(self, step):
        """Record memory usage statistics."""
        usage = self.get_memory_usage()
        
        self.memory_usage_log['steps'].append(step)
        self.memory_usage_log['working_memory_usage'].append(usage['working_memory'])
        self.memory_usage_log['episodic_memory_usage'].append(usage['episodic_memory'])
        self.memory_usage_log['associative_memory_usage'].append(usage['associative_memory'])
        
        # Calculate memory efficiency
        total_usage = sum(usage.values())
        max_capacity = 1.0
        efficiency = 1.0 - (total_usage / max_capacity)
        self.memory_usage_log['memory_efficiency'].append(efficiency)
    
    def print_memory_status(self, step):
        """Print memory usage status."""
        if step >= len(self.memory_usage_log['steps']):
            return
        
        usage = {
            'working': self.memory_usage_log['working_memory_usage'][step],
            'episodic': self.memory_usage_log['episodic_memory_usage'][step],
            'associative': self.memory_usage_log['associative_memory_usage'][step],
            'efficiency': self.memory_usage_log['memory_efficiency'][step]
        }
        
        print(f"Step {step:4d}:")
        print(f"  Memory Usage - Working: {usage['working']:.3f}, "
              f"Episodic: {usage['episodic']:.3f}, "
              f"Associative: {usage['associative']:.3f}")
        print(f"  Memory Efficiency: {usage['efficiency']:.3f}")
        
        # Check memory pressure
        total_pressure = usage['working'] + usage['episodic'] + usage['associative']
        if total_pressure > 0.8:
            print(f"  WARNING: High memory pressure ({total_pressure:.2f})")

class WorkingMemorySystem:
    """Working memory system for NLM."""
    
    def __init__(self, brain):
        self.brain = brain
        self.capacity = 1000
        self.current_usage = 0
        self.content = []
        self.decay_rate = 0.01
        
    def process(self, percept, step, curiosity_level):
        """Process input through working memory system."""
        # Update working memory with new percept
        self.update(percept, step)
        
        # Maintain capacity constraints
        self.enforce_capacity()
        
        # Apply decay
        self.apply_decay()
        
        # Return processed representation
        return self.get_neural_representation()
    
    def update(self, percept, step):
        """Update working memory with new input."""
        # Convert percept to working memory item
        item = {
            'timestamp': step,
            'percept': percept,
            'activation': 1.0,
            'recency': step,
            'curiosity_context': 1.0 - min(1.0, step / 1000.0)
        }
        
        self.content.append(item)
        self.current_usage += len(str(item))
    
    def enforce_capacity(self):
        """Enforce working memory capacity constraints."""
        while self.current_usage > self.capacity and self.content:
            # Remove oldest item
            removed = self.content.pop(0)
            self.current_usage -= len(str(removed))
    
    def apply_decay(self):
        """Apply decay to working memory items."""
        decay_factor = math.exp(-self.decay_rate)
        
        for item in self.content:
            item['activation'] *= decay_factor
            item['recency'] *= decay_factor
    
    def get_neural_representation(self):
        """Get neural representation of working memory content."""
        # Summarize working memory for neural integration
        if not self.content:
            return None
        
        # Average activation of recent items
        recent_items = [item for item in self.content if item['recency'] > 0.5]
        
        if not recent_items:
            return None
        
        avg_activation = np.mean([item['activation'] for item in recent_items])
        
        # Create simple neural representation
        return avg_activation
    
    def get_usage(self):
        """Get current working memory usage."""
        return min(1.0, self.current_usage / self.capacity)
    
    def compress(self):
        """Compress working memory representations."""
        # Reduce number of items while preserving key information
        if len(self.content) > self.capacity // 2:
            # Keep only high-activation items
            self.content.sort(key=lambda x: x['activation'], reverse=True)
            self.content = self.content[:self.capacity // 2]
            self.update_usage_from_content()
    
    def maintain_efficiency(self):
        """Maintain working memory efficiency."""
        # Prune low-activation items
        self.content = [item for item in self.content if item['activation'] > 0.1]
        self.update_usage_from_content()
    
    def update_usage_from_content(self):
        """Update usage based on current content."""
        self.current_usage = sum(len(str(item)) for item in self.content)

class EpisodicMemorySystem:
    """Episodic memory system for NLM."""
    
    def __init__(self, brain):
        self.brain = brain
        self.episodes = {}
        self.max_episodes = 1000
        self.current_usage = 0
        
    def encode(self, percept, working_memory_result, step):
        """Encode experience as episodic memory."""
        # Create episodic memory entry
        episode_id = f"episode_{step}_{len(self.episodes)}"
        
        episode = {
            'id': episode_id,
            'timestamp': step,
            'percept': percept,
            'working_memory_state': working_memory_result,
            'neural_pattern': self.create_neural_pattern(percept, working_memory_result),
            'prediction_error': self.calculate_prediction_error(percept),
            'reward_prediction': self.predict_reward(percept),
            'importance': 0.0,
            'recency': step
        }
        
        self.episodes[episode_id] = episode
        self.current_usage += len(str(episode))
        
        # Enforce capacity
        self.enforce_capacity()
        
        return episode_id
    
    def create_neural_pattern(self, percept, working_memory_result):
        """Create neural pattern representation of episode."""
        # Create simple neural representation
        if percept is None:
            return None
        
        # Extract features from percept
        features = self.extract_features_from_percept(percept)
        
        # Create pattern based on features
        pattern = {
            'features': features,
            'working_memory': working_memory_result,
            'hash': self.calculate_pattern_hash(features)
        }
        
        return pattern
    
    def extract_features_from_percept(self, percept):
        """Extract features from percept."""
        # Placeholder for feature extraction
        if percept is None:
            return []
        
        # Simple feature extraction
        features = []
        if hasattr(percept, 'getVision'):
            vision = percept.getVision()
            features.append(('vision_shape', len(vision)))
        
        if hasattr(percept, 'getInternal'):
            internal = percept.getInternal()
            features.append(('internal_signals', len(internal)))
        
        return features
    
    def calculate_prediction_error(self, percept):
        """Calculate prediction error for episode."""
        # Placeholder for prediction error calculation
        return np.random.random() * 0.5
    
    def predict_reward(self, percept):
        """Predict reward for percept."""
        # Placeholder for reward prediction
        return np.random.random() * 2.0 - 1.0
    
    def calculate_pattern_hash(self, features):
        """Calculate hash of pattern features."""
        # Create hash based on features
        feature_str = '_'.join(str(f) for f in features)
        return hash(feature_str)
    
    def get_influence(self, episode_id):
        """Get influence of episodic memory."""
        if episode_id not in self.episodes:
            return None
        
        episode = self.episodes[episode_id]
        
        # Calculate influence based on importance and recency
        influence = {
            'strength': episode['importance'],
            'recency': 1.0 - min(1.0, (self.brain.getCurrentStep() - episode['timestamp']) / 1000.0),
            'prediction_error': episode['prediction_error'],
            'reward_prediction': episode['reward_prediction']
        }
        
        return influence
    
    def get_all_episodes(self):
        """Get all episode IDs."""
        return list(self.episodes.keys())
    
    def get_episode(self, episode_id):
        """Get episode by ID."""
        return self.episodes.get(episode_id)
    
    def get_usage(self):
        """Get current episodic memory usage."""
        return min(1.0, self.current_usage / (self.max_episodes * 1000))  # Rough estimate
    
    def enforce_capacity(self):
        """Enforce episodic memory capacity."""
        while len(self.episodes) > self.max_episodes and self.episodes:
            # Remove oldest episode
            oldest_id = min(self.episodes.keys(), key=lambda x: self.episodes[x]['timestamp'])
            removed = self.episodes.pop(oldest_id)
            self.current_usage -= len(str(removed))
    
    def compress(self):
        """Compress episodic memories."""
        # Reduce detail of older episodes
        current_step = self.brain.getCurrentStep() if hasattr(self.brain, 'getCurrentStep') else 0
        
        for episode_id, episode in self.episodes.items():
            age = current_step - episode['timestamp']
            if age > 500:  # Old episodes get compressed
                # Reduce feature detail
                episode['features'] = episode['features'][:max(1, len(episode['features']) // 2)]
    
    def prioritize_recent(self):
        """Prioritize recent episodic memories."""
        # Increase recency weights of recent episodes
        current_step = self.brain.getCurrentStep() if hasattr(self.brain, 'getCurrentStep') else 0
        
        for episode in self.episodes.values():
            age = current_step - episode['timestamp']
            episode['recency'] = min(1.0, 1.0 - age / 500.0)
    
    def clean_unused(self):
        """Clean up unused episodic memories."""
        # Remove episodes with very low importance
        to_remove = []
        for episode_id, episode in self.episodes.items():
            if episode['importance'] < 0.05 and episode['recency'] < 0.1:
                to_remove.append(episode_id)
        
        for episode_id in to_remove:
            removed = self.episodes.pop(episode_id)
            self.current_usage -= len(str(removed))

class AssociativeMemorySystem:
    """Associative memory system for NLM."""
    
    def __init__(self, brain):
        self.brain = brain
        self.associations = {}
        self.current_usage = 0
        
    def update(self, percept, episode_id, brain):
        """Update associative memory with new associations."""
        # Extract features from percept
        features = self.extract_percept_features(percept)
        
        # Create associations based on episode
        associations = self.create_associations(features, episode_id)
        
        # Update association strengths
        self.update_association_strengths(associations, percept)
        
        return associations
    
    def extract_percept_features(self, percept):
        """Extract features from percept for association."""
        features = {}
        
        if percept is None:
            return features
        
        # Extract various features from percept
        if hasattr(percept, 'getVision'):
            vision = percept.getVision()
            features['visual_shape'] = vision.shape if hasattr(vision, 'shape') else (len(vision),)
            features['visual_features'] = self.extract_visual_features(vision)
        
        if hasattr(percept, 'getInternal'):
            internal = percept.getInternal()
            features['internal_signals'] = len(internal) if internal else 0
            features['reward_signals'] = np.mean(internal) if internal else 0.0
        
        return features
    
    def extract_visual_features(self, vision):
        """Extract visual features from vision data."""
        # Placeholder for visual feature extraction
        if isinstance(vision, np.ndarray):
            # Simple texture features
            return {
                'mean_intensity': np.mean(vision),
                'contrast': np.std(vision),
                'edges': np.sum(np.abs(np.diff(vision, axis=0))) + np.sum(np.abs(np.diff(vision, axis=1)))
            }
        return {}
    
    def create_associations(self, features, episode_id):
        """Create associations based on features and episode."""
        associations = []
        
        # Create feature-based associations
        for feature_key, feature_value in features.items():
            if isinstance(feature_value, (int, float)):
                association_id = f"association_{feature_key}_{episode_id}"
                
                association = {
                    'id': association_id,
                    'source': feature_key,
                    'value': feature_value,
                    'episode_id': episode_id,
                    'strength': 1.0,
                    'last_accessed': self.brain.getCurrentStep() if hasattr(self.brain, 'getCurrentStep') else 0
                }
                
                self.associations[association_id] = association
                associations.append(association_id)
        
        return associations
    
    def update_association_strengths(self, associations, percept):
        """Update association strengths based on prediction error."""
        for association_id in associations:
            if association_id in self.associations:
                association = self.associations[association_id]
                
                # Calculate prediction error (placeholder)
                prediction_error = np.random.random() * 0.5
                
                # Update strength based on prediction error
                if prediction_error > 0.3:
                    association['strength'] *= (1.0 + prediction_error)
                else:
                    association['strength'] *= (1.0 - 0.1 * prediction_error)
                
                # Cap strength
                association['strength'] = min(10.0, max(0.1, association['strength']))
    
    def get_patterns(self, association_ids):
        """Get associative patterns based on association IDs."""
        patterns = []
        
        for association_id in association_ids:
            if association_id in self.associations:
                pattern = self.associations[association_id]
                patterns.append(pattern)
        
        return patterns
    
    def find_similar(self, percept):
        """Find similar episodes based on percept."""
        similar_episodes = []
        
        # Extract features from current percept
        features = self.extract_percept_features(percept)
        
        # Compare with stored episodes
        for episode_id, episode in self.episodes.items():
            similarity = self.calculate_similarity(features, episode['features'])
            if similarity > 0.7:
                similar_episodes.append({
                    'episode_id': episode_id,
                    'similarity': similarity,
                    'episode': episode
                })
        
        return similar_episodes
    
    def calculate_similarity(self, features1, features2):
        """Calculate similarity between two feature sets."""
        if not features1 or not features2:
            return 0.0
        
        # Compare common features
        common_keys = set(features1.keys()) & set(features2.keys())
        
        if not common_keys:
            return 0.0
        
        # Calculate similarity based on common features
        similarities = []
        for key in common_keys:
            val1 = features1[key]
            val2 = features2[key]
            
            if isinstance(val1, dict) and isinstance(val2, dict):
                # Nested dictionary similarity
                sim = self.calculate_nested_similarity(val1, val2)
            elif isinstance(val1, (int, float)) and isinstance(val2, (int, float)):
                # Numerical similarity
                sim = 1.0 / (1.0 + abs(val1 - val2))
            else:
                # Binary similarity
                sim = 1.0 if val1 == val2 else 0.0
            
            similarities.append(sim)
        
        return np.mean(similarities)
    
    def calculate_nested_similarity(self, dict1, dict2):
        """Calculate similarity between nested dictionaries."""
        common_keys = set(dict1.keys()) & set(dict2.keys())
        
        if not common_keys:
            return 0.0
        
        similarities = []
        for key in common_keys:
            val1 = dict1[key]
            val2 = dict2[key]
            
            if isinstance(val1, (int, float)) and isinstance(val2, (int, float)):
                sim = 1.0 / (1.0 + abs(val1 - val2))
            else:
                sim = 1.0 if val1 == val2 else 0.0
            
            similarities.append(sim)
        
        return np.mean(similarities)
    
    def get_usage(self):
        """Get current associative memory usage."""
        return min(1.0, self.current_usage / (len(self.associations) * 100))
    
    def compress(self):
        """Compress associative memories."""
        # Reduce strength of weak associations
        weak_associations = []
        for association_id, association in self.associations.items():
            if association['strength'] < 1.0:
                weak_associations.append(association_id)
        
        for association_id in weak_associations:
            del self.associations[association_id]
    
    def reduce_usage(self):
        """Reduce associative memory usage."""
        # Reduce number of associations
        excess = max(0, len(self.associations) - 500)
        
        if excess > 0:
            # Remove weakest associations
            sorted_associations = sorted(
                self.associations.items(),
                key=lambda x: x[1]['strength']
            )
            
            for association_id, _ in sorted_associations[:excess]:
                del self.associations[association_id]
    
    def clean_unused(self):
        """Clean up unused associative memories."""
        # Remove associations that haven't been accessed recently
        current_step = self.brain.getCurrentStep() if hasattr(self.brain, 'getCurrentStep') else 0
        max_age = 1000  # Steps since last access
        
        to_remove = []
        for association_id, association in self.associations.items():
            if current_step - association['last_accessed'] > max_age:
                to_remove.append(association_id)
        
        for association_id in to_remove:
            del self.associations[association_id]

def memory_management_example():
    """Run the memory management example."""
    print("=" * 60)
    print("NLM Memory Management Example")
    print("=" * 60)
    
    # Create memory management system
    memory_system = MemoryManagementSystem(brain_size='medium')
    
    # Run memory-efficient simulation
    results = memory_system.run_memory_efficient_simulation(num_steps=500)
    
    print("\n" + "=" * 60)
    print("Memory Management Simulation Complete!")
    print("=" * 60)
    
    # Print summary statistics
    final_step = results['steps'][-1] if results['steps'] else 0
    print(f"Total simulation steps: {final_step}")
    print(f"Final working memory usage: {results['working_memory_usage'][-1]:.3f}")
    print(f"Final episodic memory usage: {results['episodic_memory_usage'][-1]:.3f}")
    print(f"Final associative memory usage: {results['associative_memory_usage'][-1]:.3f}")
    print(f"Final memory efficiency: {results['memory_efficiency'][-1]:.3f}")
    
    # Calculate average memory usage
    avg_working = np.mean(results['working_memory_usage']) if results['working_memory_usage'] else 0
    avg_episodic = np.mean(results['episodic_memory_usage']) if results['episodic_memory_usage'] else 0
    avg_associative = np.mean(results['associative_memory_usage']) if results['associative_memory_usage'] else 0
    avg_efficiency = np.mean(results['memory_efficiency']) if results['memory_efficiency'] else 0
    
    print(f"\nAverage Memory Usage:")
    print(f"  Working Memory: {avg_working:.3f}")
    print(f"  Episodic Memory: {avg_episodic:.3f}")
    print(f"  Associative Memory: {avg_associative:.3f}")
    print(f"  Overall Efficiency: {avg_efficiency:.3f}")
    
    return results

if __name__ == "__main__":
    # Note: This example demonstrates memory management concepts
    # Actual implementation would require complete NLM integration
    print("Memory management example requires full NLM integration")
    print("This template shows the structure and key concepts.")
    
    # memory_management_example()
```

## 5. Neuromodulation Networks

### Overview
This example demonstrates implementing dopamine, curiosity, novelty detection, and prediction error systems.

```python
import pynlm
import numpy as np
import math

class NeuromodulationNetwork:
    """Advanced neuromodulation network with multiple neuromodulators."""
    
    def __init__(self, brain_size='medium'):
        # Create brain with neuromodulation-optimized configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", self.get_brain_size(brain_size))
        config.set("brain.synapse_density", 0.06)
        config.set("neuromod.dopamine.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.prediction_error.enable", True)
        
        # Initialize core components
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create environment
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=40, height=40, visionWidth=10, visionHeight=10)
        
        # Initialize neuromodulation agents
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        # Initialize neuromodulation subsystems
        self.dopamine_system = DopamineSystem()
        self.curiosity_system = CuriositySystem()
        self.novelty_system = NoveltyDetectionSystem()
        self.prediction_error_system = PredictionErrorSystem()
        
        # Initialize neuromodulation tracking
        self.neuromodulation_history = {
            'steps': [],
            'dopamine_levels': [],
            'curiosity_levels': [],
            'novelty_levels': [],
            'prediction_error_levels': [],
            'neuromodulation_balance': []
        }
        
        # Neuromodulation parameters
        self.setup_neuromodulation_parameters()
        
    def get_brain_size(self, size):
        """Get brain size based on complexity level."""
        sizes = {
            'small': 1500,
            'medium': 3000,
            'large': 6000,
            'massive': 10000
        }
        return sizes.get(size, 3000)
    
    def setup_neuromodulation_parameters(self):
        """Setup neuromodulation system parameters."""
        # Dopamine system parameters
        self.dopamine_params = {
            'baseline_level': 0.1,
            'learning_rate': 0.05,
            'prediction_window': 5,
            'temporal_dilation': 1.0,
            'phasic_response_strength': 0.8,
            'tonic_response_strength': 0.2,
            'reward_prediction_error_scaling': 1.0,
            'information_gating_strength': 0.7
        }
        
        # Curiosity system parameters
        self.curiosity_params = {
            'baseline_level': 0.15,
            'exploration_bonus': 0.5,
            'novelty_sensitivity': 0.8,
            'intrinsic_motivation_strength': 0.6,
            'action_cost': 0.01,
            'exploration_exploitation_balance': 0.5,
            'attention_modulation_strength': 0.9
        }
        
        # Novelty detection parameters
        self.novelty_params = {
            'detection_threshold': 0.3,
            'adaptation_rate': 0.05,
            'memory_decay_rate': 0.02,
            'pattern_complexity_threshold': 0.6,
            'sensory_gating_strength': 0.8,
            'pattern_matching_threshold': 0.7
        }
        
        # Prediction error parameters
        self.prediction_error_params = {
            'baseline_error': 0.2,
            'learning_rate': 0.03,
            'confidence_threshold': 0.8,
            'error_amplification': 2.0,
            'error_decay_rate': 0.01,
            'precision_weighting': 0.9,
            'prediction_horizon': 3
        }
    
    def run_advanced_neuromodulation_cycle(self, num_steps):
        """Run advanced neuromodulation cycle with integrated systems."""
        print("Starting Advanced Neuromodulation Cycle")
        print("=" * 60)
        
        for step in range(num_steps):
            # 1. Environmental interaction
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            
            # 2. Process with neuromodulation integration
            neuromodulated_percept = self.process_with_neuromodulation(
                percept, step, self.neuromodulation_history
            )
            
            # 3. Agent processes neuromodulated input
            self.agent.processSensoryInput(neuromulated_percept)
            
            # 4. Brain computation with neuromodulation
            brain_result = self.compute_with_neuromodulation(step, neuromodulated_percept)
            
            # 5. Action selection with neuromodulation
            action = self.select_action_with_neuromodulation(
                brain_result, neuromodulated_percept, step
            )
            
            # 6. Environmental interaction
            world_result = self.world.applyMotorCommand(
                action, self.world.getSimulationTime()
            )
            reward = world_result.get_reward() if hasattr(world_result, 'get_reward') else 0.0
            
            # 7. Neuromodulation update cycle
            neuromodulation_update = self.update_neuromodulation_systems(
                neuromodulated_percept, reward, brain_result, step
            )
            
            # 8. Learning and plasticity
            self.apply_neuromodulation_learning(
                neuromodulation_update, step
            )
            
            # 9. Development and maturation
            self.agent.updateDevelopment(0.1)
            
            # 10. Record neuromodulation history
            self.record_neuromodulation_history(step, neuromodulation_update)
            
            # 11. Print progress
            if step % 100 == 0:
                self.print_neuromodulation_status(step, neuromodulation_update)
        
        return self.neuromodulation_history
    
    def process_with_neuromodulation(self, percept, step, neuromodulation_history):
        """Process percept with integrated neuromodulation systems."""
        # Get current neuromodulation levels
        current_levels = self.get_current_neuromodulation_levels()
        
        # Apply neuromodulation to percept processing
        neuromodulated_percept = self.apply_neuromodulation_to_percept(
            percept, current_levels, step
        )
        
        return neuromodulated_percept
    
    def get_current_neuromodulation_levels(self):
        """Get current neuromodulation system levels."""
        levels = {
            'dopamine': self.agent.getNeuromodulationLevel(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.novelty_system.get_current_novelty_level(),
            'prediction_error': self.agent.getPredictionError()
        }
        
        return levels
    
    def apply_neuromodulation_to_percept(self, percept, levels, step):
        """Apply neuromodulation effects to percept."""
        if percept is None:
            return None
        
        # Create neuromodulated percept
        neuromodulated = percept
        
        # Apply dopamine modulation (reward-based attention)
        dopamine_influence = levels['dopamine'] * self.dopamine_params['information_gating_strength']
        
        # Apply curiosity modulation (exploration bonus)
        curiosity_influence = levels['curiosity'] * self.curiosity_params['attention_modulation_strength']
        
        # Apply novelty modulation
        novelty_influence = self.novelty_system.get_current_novelty_level() * self.novelty_params['sensory_gating_strength']
        
        # Apply prediction error modulation
        pe_influence = levels['prediction_error'] * self.prediction_error_params['error_amplification']
        
        # Combine modulations
        total_influence = dopamine_influence + curiosity_influence + novelty_influence + pe_influence
        
        # Apply to percept features (placeholder implementation)
        return neuromodulated
    
    def compute_with_neuromodulation(self, step, neuromodulated_percept):
        """Compute brain step with neuromodulation integration."""
        # Integrate neuromodulation signals with neural computation
        neuromodulation_signals = self.get_current_neuromodulation_levels()
        
        # Apply neuromodulation to brain dynamics
        self.apply_neuromodulation_to_brain(neuromodulation_signals)
        
        # Brain computation
        self.brain.step(step)
        
        # Return computation results
        return self.get_computation_results_with_neuromodulation()
    
    def apply_neuromodulation_to_brain(self, levels):
        """Apply neuromodulation signals to brain dynamics."""
        # This would apply neuromodulation effects to neural computations
        # Implementation depends on NLM's neuromodulation system
        pass
    
    def get_computation_results_with_neuromodulation(self):
        """Get computation results with neuromodulation effects."""
        return {
            'firing_rate': self.brain.getAverageFiringRate(),
            'dopamine_level': self.agent.getNeuromodulationLevel(),
            'curiosity_level': self.agent.getCuriosityLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'novelty_level': self.novelty_system.get_current_novelty_level()
        }
    
    def select_action_with_neuromodulation(self, brain_result, neuromodulated_percept, step):
        """Select action with neuromodulation guidance."""
        # Get neuromodulation influences
        neuromod_levels = self.get_current_neuromodulation_levels()
        
        # Apply neuromodulation to action selection
        action_preferences = self.calculate_action_preferences(brain_result, neuromod_levels, step)
        
        # Select action based on preferences
        action = self.select_action_based_on_preferences(action_preferences)
        
        return action
    
    def calculate_action_preferences(self, brain_result, neuromod_levels, step):
        """Calculate action preferences based on neuromodulation."""
        # Get available actions
        available_actions = self.get_available_actions()
        
        # Calculate preferences for each action
        preferences = {}
        for action in available_actions:
            base_preference = self.calculate_base_action_preference(action, brain_result)
            
            # Apply neuromodulation influences
            neuromod_influence = self.calculate_neuromodulation_influence(
                action, neuromod_levels, step
            )
            
            total_preference = base_preference + neuromod_influence
            preferences[action] = total_preference
        
        return preferences
    
    def calculate_base_action_preference(self, action, brain_result):
        """Calculate base action preference from brain activity."""
        # Get motor neuron activity for action
        motor_activity = self.get_motor_neuron_activity(action)
        
        # Normalize activity to preference
        preference = motor_activity / np.max([m for m in self.get_all_motor_activities().values()] + [1.0])
        
        return preference
    
    def calculate_neuromodulation_influence(self, action, neuromod_levels, step):
        """Calculate neuromodulation influence on action selection."""
        influence = 0.0
        
        # Dopamine influence (reward prediction)
        dopamine_influence = neuromod_levels['dopamine'] * self.dopamine_params['phasic_response_strength']
        
        # Curiosity influence (exploration bonus)
        curiosity_influence = neuromod_levels['curiosity'] * self.curiosity_params['exploration_bonus']
        
        # Novelty influence
        novelty_influence = neuromod_levels['novelty'] * self.novelty_params['detection_threshold']
        
        # Prediction error influence
        pe_influence = neuromod_levels['prediction_error'] * self.prediction_error_params['error_amplification']
        
        # Combine influences
        influence = dopamine_influence + curiosity_influence + novelty_influence + pe_influence
        
        # Apply action-specific modulations
        if self.is_exploratory_action(action):
            influence *= self.curiosity_params['exploration_exploitation_balance']
        
        return influence
    
    def is_exploratory_action(self, action):
        """Check if action is exploratory (novel)."""
        # Placeholder for exploratory action detection
        return action in ['Look', 'TurnLeft', 'TurnRight', 'MoveForward']
    
    def get_motor_neuron_activity(self, action):
        """Get motor neuron activity for action."""
        # This would get actual motor neuron activity from the brain
        # Placeholder implementation
        return np.random.random() * 0.5 + 0.5
    
    def get_all_motor_activities(self):
        """Get all motor neuron activities."""
        # Placeholder implementation
        return {
            'move_forward': np.random.random() * 0.5 + 0.5,
            'move_backward': np.random.random() * 0.5 + 0.5,
            'turn_left': np.random.random() * 0.5 + 0.5,
            'turn_right': np.random.random() * 0.5 + 0.5,
            'interact': np.random.random() * 0.5 + 0.5,
            'wait': np.random.random() * 0.5 + 0.5
        }
    
    def select_action_based_on_preferences(self, preferences):
        """Select action based on calculated preferences."""
        if not preferences:
            # Default action if no preferences calculated
            return self.get_default_action()
        
        # Convert preferences to action selection probabilities
        total_pref = sum(preferences.values())
        if total_pref <= 0:
            return self.get_default_action()
        
        probabilities = {action: pref / total_pref for action, pref in preferences.items()}
        
        # Sample action based on probabilities
        actions = list(probabilities.keys())
        probs = list(probabilities.values())
        
        selected_action = np.random.choice(actions, p=probs)
        
        return selected_action
    
    def get_default_action(self):
        """Get default action when no preferences calculated."""
        return 'MoveForward'
    
    def get_available_actions(self):
        """Get available actions for selection."""
        return ['MoveForward', 'MoveBackward', 'TurnLeft', 'TurnRight', 'Interact', 'Wait']
    
    def update_neuromodulation_systems(self, percept, reward, brain_result, step):
        """Update all neuromodulation systems."""
        # Update dopamine system
        dopamine_update = self.dopamine_system.update(
            reward, brain_result.get('prediction_error', 0.0), step
        )
        
        # Update curiosity system
        curiosity_update = self.curiosity_system.update(
            percept, brain_result.get('firing_rate', 0.0), step
        )
        
        # Update novelty detection
        novelty_update = self.novelty_system.update(
            percept, self.get_memory_similarity(), step
        )
        
        # Update prediction error system
        pe_update = self.prediction_error_system.update(
            percept, reward, step
        )
        
        # Combine updates
        neuromodulation_update = {
            'dopamine': dopamine_update,
            'curiosity': curiosity_update,
            'novelty': novelty_update,
            'prediction_error': pe_update,
            'step': step
        }
        
        return neuromodulation_update
    
    def apply_neuromodulation_learning(self, neuromodulation_update, step):
        """Apply neuromodulation learning to brain plasticity."""
        # Apply dopamine-based learning
        dopamine_effect = neuromodulation_update['dopamine'] * self.dopamine_params['learning_rate']
        self.apply_dopamine_learning(dopamine_effect, step)
        
        # Apply curiosity-based learning
        curiosity_effect = neuromodulation_update['curiosity'] * self.curiosity_params['learning_rate']
        self.apply_curiosity_learning(curiosity_effect, step)
        
        # Apply prediction error-based learning
        pe_effect = neuromodulation_update['prediction_error'] * self.prediction_error_params['learning_rate']
        self.apply_prediction_error_learning(pe_effect, step)
    
    def apply_dopamine_learning(self, dopamine_effect, step):
        """Apply dopamine-based learning to brain."""
        # This would apply dopamine signals to synaptic plasticity
        # Implementation depends on NLM's dopamine system
        pass
    
    def apply_curiosity_learning(self, curiosity_effect, step):
        """Apply curiosity-based learning to brain."""
        # This would apply curiosity signals to exploratory learning
        # Implementation depends on NLM's curiosity system
        pass
    
    def apply_prediction_error_learning(self, pe_effect, step):
        """Apply prediction error-based learning to brain."""
        # This would apply prediction error signals to learning
        # Implementation depends on NLM's prediction error system
        pass
    
    def record_neuromodulation_history(self, step, neuromodulation_update):
        """Record neuromodulation system history."""
        self.neuromodulation_history['steps'].append(step)
        
        # Record system levels
        current_levels = self.get_current_neuromodulation_levels()
        self.neuromodulation_history['dopamine_levels'].append(current_levels['dopamine'])
        self.neuromodulation_history['curiosity_levels'].append(current_levels['curiosity'])
        self.neuromodulation_history['novelty_levels'].append(current_levels['novelty'])
        self.neuromodulation_history['prediction_error_levels'].append(current_levels['prediction_error'])
        
        # Calculate balance
        balance = self.calculate_neuromodulation_balance(current_levels)
        self.neuromodulation_history['neuromodulation_balance'].append(balance)
    
    def calculate_neuromodulation_balance(self, levels):
        """Calculate neuromodulation system balance."""
        # Balance based on relative levels of different neuromodulators
        total = sum(levels.values())
        
        if total == 0:
            return 0.0
        
        # Normalized balance (closer to 1.0 means more balanced)
        balance = 1.0 - np.std(list(levels.values())) / np.mean(list(levels.values()))
        
        return max(0.0, min(1.0, balance))
    
    def print_neuromodulation_status(self, step, neuromodulation_update):
        """Print neuromodulation system status."""
        current_levels = self.get_current_neuromodulation_levels()
        balance = self.calculate_neuromodulation_balance(current_levels)
        
        print(f"Step {step:4d}:")
        print(f"  Neuromodulation Levels:")
        print(f"    Dopamine:     {current_levels['dopamine']:.3f}")
        print(f"    Curiosity:    {current_levels['curiosity']:.3f}")
        print(f"    Novelty:      {current_levels['novelty']:.3f}")
        print(f"    PredictionError: {current_levels['prediction_error']:.3f}")
        print(f"  System Balance: {balance:.3f}")
        
        # Check for system dysregulation
        dysregulation = self.detect_neuromodulation_dysregulation(current_levels)
        if dysregulation:
            print(f"  ⚠️  Neuromodulation Dysregulation Detected!")
    
    def detect_neuromodulation_dysregulation(self, levels):
        """Detect neuromodulation system dysregulation."""
        # Check for abnormal levels
        dysregulation_indicators = []
        
        # Dopamine dysregulation
        if levels['dopamine'] > 2.0 or levels['dopamine'] < 0.01:
            dysregulation_indicators.append('dopamine_extreme')
        
        # Curiosity dysregulation  
        if levels['curiosity'] > 1.5 or levels['curiosity'] < 0.01:
            dysregulation_indicators.append('curiosity_extreme')
        
        # Novelty dysregulation
        if levels['novelty'] > 1.5 or levels['novelty'] < 0.01:
            dysregulation_indicators.append('novelty_extreme')
        
        # Prediction error dysregulation
        if levels['prediction_error'] > 2.0 or levels['prediction_error'] < 0.01:
            dysregulation_indicators.append('prediction_error_extreme')
        
        return len(dysregulation_indicators) > 0

class DopamineSystem:
    """Dopamine neuromodulation system."""
    
    def __init__(self, params=None):
        self.params = params or {}
        self.current_level = 0.1
        self.prediction_history = []
        
    def update(self, reward, prediction_error, step):
        """Update dopamine system."""
        # Calculate prediction error
        pe = reward - prediction_error
        
        # Update prediction history
        self.prediction_history.append(pe)
        
        # Keep history bounded
        if len(self.prediction_history) > 10:
            self.prediction_history.pop(0)
        
        # Calculate new dopamine level
        if len(self.prediction_history) > 1:
            avg_prediction_error = np.mean(self.prediction_history)
            self.current_level = self.calculate_dopamine_level(avg_prediction_error, pe)
        
        return self.current_level
    
    def calculate_dopamine_level(self, avg_prediction_error, current_pe):
        """Calculate dopamine level based on prediction errors."""
        # Phasic response to prediction error
        phasic_response = abs(current_pe) * self.params.get('phasic_response_strength', 0.8)
        
        # Tonic response modulated by average prediction error
        tonic_response = self.params.get('tonic_response_strength', 0.2) * (
            1.0 - abs(avg_prediction_error) / max(abs(avg_prediction_error) if avg_prediction_error != 0 else 1, 1.0)
        )
        
        return phasic_response + tonic_response

class CuriositySystem:
    """Curiosity neuromodulation system."""
    
    def __init__(self, params=None):
        self.params = params or {}
        self.current_level = 0.15
        self.exploration_history = []
        
    def update(self, percept, firing_rate, step):
        """Update curiosity system."""
        # Calculate novelty from percept
        novelty = self.calculate_percept_novelty(percept)
        
        # Calculate intrinsic motivation
        intrinsic_motivation = self.calculate_intrinsic_motivation(firing_rate)
        
        # Update curiosity level
        self.current_level = self.calculate_curiosity_level(
            novelty, intrinsic_motivation, step
        )
        
        self.exploration_history.append(self.current_level)
        
        # Keep history bounded
        if len(self.exploration_history) > 20:
            self.exploration_history.pop(0)
        
        return self.current_level
    
    def calculate_percept_novelty(self, percept):
        """Calculate novelty of percept."""
        if percept is None:
            return 0.0
        
        # Placeholder for novelty calculation
        return np.random.random() * self.params.get('novelty_sensitivity', 0.8)
    
    def calculate_intrinsic_motivation(self, firing_rate):
        """Calculate intrinsic motivation from firing rate."""
        # Optimal firing rate is around 5 Hz
        optimal_firing = 5.0
        deviation = abs(firing_rate - optimal_firing)
        
        # High deviation from optimal indicates either under- or over-excitation
        intrinsic_motivation = 1.0 - min(1.0, deviation / optimal_firing)
        
        return intrinsic_motivation * self.params.get('intrinsic_motivation_strength', 0.6)
    
    def calculate_curiosity_level(self, novelty, intrinsic_motivation, step):
        """Calculate curiosity level based on inputs."""
        # Combine novelty and intrinsic motivation
        curiosity = (novelty * self.params.get('novelty_sensitivity', 0.8) +
                    intrinsic_motivation * self.params.get('intrinsic_motivation_strength', 0.6))
        
        # Apply adaptation based on recent history
        if len(self.exploration_history) > 5:
            avg_curiosity = np.mean(self.exploration_history[-5:])
            adaptation = self.params.get('adaptation_rate', 0.05)
            curiosity = curiosity * (1.0 - adaptation) + avg_curiosity * adaptation
        
        # Ensure level is within bounds
        curiosity = max(0.0, min(1.0, curiosity))
        
        return curiosity

class NoveltyDetectionSystem:
    """Novelty detection neuromodulation system."""
    
    def __init__(self, params=None):
        self.params = params or {}
        self.current_level = 0.0
        self.pattern_memory = {}
        
    def update(self, percept, similarity_score, step):
        """Update novelty detection system."""
        # Calculate novelty based on pattern similarity
        novelty = self.calculate_pattern_novelty(percept, similarity_score)
        
        # Apply adaptation
        self.current_level = self.adapt_novelty_level(novely, step)
        
        return self.current_level
    
    def calculate_pattern_novelty(self, percept, similarity_score):
        """Calculate novelty based on pattern similarity."""
        # Low similarity indicates high novelty
        if similarity_score < self.params.get('detection_threshold', 0.3):
            novelty = 1.0 - similarity_score / self.params.get('detection_threshold', 0.3)
        else:
            novelty = 0.0
        
        # Apply pattern complexity threshold
        pattern_complexity = self.calculate_pattern_complexity(percept)
        if pattern_complexity > self.params.get('pattern_complexity_threshold', 0.6):
            novelty *= (1.0 + pattern_complexity - 0.6)
        
        return min(1.0, novelty)
    
    def calculate_pattern_complexity(self, percept):
        """Calculate complexity of percept pattern."""
        if percept is None:
            return 0.0
        
        # Placeholder for complexity calculation
        return np.random.random() * 0.5 + 0.3
    
    def adapt_novelty_level(self, novelty, step):
        """Adapt novelty level based on recent exposure."""
        adaptation_rate = self.params.get('adaptation_rate', 0.05)
        
        # Decay novelty over time
        decay_factor = math.exp(-adaptation_rate * step)
        
        adapted_novelty = novelty * decay_factor
        
        # Ensure level is within bounds
        return max(0.0, min(1.0, adapted_novelty))
    
    def get_current_novelty_level(self):
        """Get current novelty level."""
        return self.current_level

class PredictionErrorSystem:
    """Prediction error neuromodulation system."""
    
    def __init__(self, params=None):
        self.params = params or {}
        self.current_level = 0.2
        self.prediction_history = []
        
    def update(self, percept, reward, step):
        """Update prediction error system."""
        # Calculate prediction error
        prediction_error = self.calculate_prediction_error(percept, reward)
        
        # Update prediction history
        self.prediction_history.append(prediction_error)
        
        # Keep history bounded
        if len(self.prediction_history) > 10:
            self.prediction_history.pop(0)
        
        # Calculate new prediction error level
        if len(self.prediction_history) > 1:
            avg_error = np.mean(self.prediction_history)
            self.current_level = self.calculate_error_level(avg_error, prediction_error, step)
        
        return self.current_level
    
    def calculate_prediction_error(self, percept, reward):
        """Calculate prediction error from percept and reward."""
        # Simplified prediction error calculation
        predicted_reward = np.random.random() * 2.0 - 1.0  # [-1, 1] range
        prediction_error = reward - predicted_reward
        
        return prediction_error
    
    def calculate_error_level(self, avg_error, current_error, step):
        """Calculate prediction error level."""
        # Scale current error
        scaled_error = abs(current_error) * self.params.get('error_amplification', 2.0)
        
        # Modulate by average error
        modulation = 1.0 - abs(avg_error) / max(abs(avg_error) if avg_error != 0 else 1, 1.0)
        
        # Combine
        error_level = scaled_error * modulation
        
        # Apply decay
        decay_rate = self.params.get('error_decay_rate', 0.01)
        error_level *= math.exp(-decay_rate * step)
        
        # Ensure level is within bounds
        return max(0.0, min(1.0, error_level))

class DevelopmentalPsychologySystem:
    """Developmental psychology simulation system."""
    
    def __init__(self):
        # Initialize developmental stages
        self.developmental_stages = {
            'sensory': {'start': 0, 'end': 50, 'active': True},
            'motor': {'start': 50, 'end': 150, 'active': False},
            'social': {'start': 150, 'end': 400, 'active': False},
            'cognitive': {'start': 400, 'end': 1000, 'active': False}
        }
        
        self.current_stage = 'sensory'
        self.development_progress = 0.0
        
    def update(self, brain, agent, world, step):
        """Update developmental psychology system."""
        # Update development progress
        self.development_progress += 0.1
        
        # Check for developmental stage transitions
        self.check_stage_transitions(step)
        
        # Apply stage-specific learning
        self.apply_stage_specific_learning()
        
        # Update agent's development
        agent.updateDevelopment(0.1)
        
        return self.get_development_metrics()
    
    def check_stage_transitions(self, step):
        """Check and handle developmental stage transitions."""
        for stage_name, stage_info in self.developmental_stages.items():
            if stage_info['active']:
                progress = (step - stage_info['start']) / (stage_info['end'] - stage_info['start'])
                stage_info['progress'] = min(1.0, progress)
                
                # Check if stage should transition
                if progress >= 1.0:
                    stage_info['active'] = False
                    
                    # Activate next stage
                    stages = list(self.developmental_stages.keys())
                    current_idx = stages.index(stage_name)
                    if current_idx < len(stages) - 1:
                        next_stage = stages[current_idx + 1]
                        self.developmental_stages[next_stage]['active'] = True
                        self.current_stage = next_stage
                        print(f"Developmental transition: {stage_name} → {next_stage}")
    
    def apply_stage_specific_learning(self):
        """Apply stage-specific learning mechanisms."""
        if self.current_stage == 'sensory':
            self.apply_sensory_learning()
        elif self.current_stage == 'motor':
            self.apply_motor_learning()
        elif self.current_stage == 'social':
            self.apply_social_learning()
        elif self.current_stage == 'cognitive':
            self.apply_cognitive_learning()
    
    def apply_sensory_learning(self):
        """Apply sensory learning (early development)."""
        # Focus on feature extraction and basic perception
        pass
    
    def apply_motor_learning(self):
        """Apply motor learning (middle development)."""
        # Focus on action selection and refinement
        pass
    
    def apply_social_learning(self):
        """Apply social learning (late development)."""
        # Focus on imitation and social coordination
        pass
    
    def apply_cognitive_learning(self):
        """Apply cognitive learning (advanced development)."""
        # Focus on abstract thinking and planning
        pass
    
    def get_development_metrics(self):
        """Get development metrics."""
        return {
            'current_stage': self.current_stage,
            'development_progress': self.development_progress,
            'stage_progress': {name: info.get('progress', 0.0) 
                             for name, info in self.developmental_stages.items()}
        }

def neuromodulation_example():
    """Run the neuromodulation networks example."""
    print("=" * 60)
    print("NLM Neuromodulation Networks Example")
    print("=" * 60)
    
    # Create neuromodulation network
    neuromodulation_network = NeuromodulationNetwork(brain_size='medium')
    
    # Run advanced neuromodulation cycle
    results = neuromodulation_network.run_advanced_neuromodulation_cycle(num_steps=500)
    
    # Analyze neuromodulation patterns
    analysis = neuromodulation_network.analyze_neuromodulation_patterns(results)
    
    print("\n" + "=" * 60)
    print("Neuromodulation Networks Analysis Complete!")
    print("=" * 60)
    
    # Print summary statistics
    final_step = results['steps'][-1] if results['steps'] else 0
    print(f"Total neuromodulation steps: {final_step}")
    print(f"Final dopamine level: {results['dopamine_levels'][-1]:.3f}")
    print(f"Final curiosity level: {results['curiosity_levels'][-1]:.3f}")
    print(f"Final novelty level: {results['novelty_levels'][-1]:.3f}")
    print(f"Final prediction error level: {results['prediction_error_levels'][-1]:.3f}")
    print(f"Final neuromodulation balance: {results['neuromodulation_balance'][-1]:.3f}")
    
    # Calculate average neuromodulation levels
    avg_dopamine = np.mean(results['dopamine_levels']) if results['dopamine_levels'] else 0
    avg_curiosity = np.mean(results['curiosity_levels']) if results['curiosity_levels'] else 0
    avg_novelty = np.mean(results['novelty_levels']) if results['novelty_levels'] else 0
    avg_pe = np.mean(results['prediction_error_levels']) if results['prediction_error_levels'] else 0
    avg_balance = np.mean(results['neuromodulation_balance']) if results['neuromodulation_balance'] else 0
    
    print(f"\nAverage Neuromodulation Levels:")
    print(f"  Dopamine:     {avg_dopamine:.3f}")
    print(f"  Curiosity:    {avg_curiosity:.3f}")
    print(f"  Novelty:      {avg_novelty:.3f}")
    print(f"  PredictionError: {avg_pe:.3f}")
    print(f"  System Balance: {avg_balance:.3f}")
    
    return results, analysis

if __name__ == "__main__":
    # Note: These examples demonstrate advanced neuromodulation concepts
    # Actual implementation would require complete NLM integration
    print("Neuromodulation networks examples require full NLM integration")
    print("This template shows the structure and key concepts.")
    
    # neuromodulation_example()
```