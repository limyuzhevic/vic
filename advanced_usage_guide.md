# Advanced NLM Usage Guide

This document provides advanced usage patterns and techniques for power users of the NLM framework. It covers complex architectures, optimization strategies, and advanced debugging techniques.

## 1. Complex Brain Architectures

### Multi-Region Brain Architecture

```python
import pynlm

def create_multi_region_brain(config):
    """Create a brain with specialized regions."""
    
    # Create brain with multiple regions
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Add specialized regions
    region_ids = []
    
    # Sensory cortex region
    sensory_region_id = brain.addRegion("sensory_cortex")
    region_ids.append(sensory_region_id)
    
    # Motor cortex region  
    motor_region_id = brain.addRegion("motor_cortex")
    region_ids.append(motor_region_id)
    
    # Association cortex region
    association_region_id = brain.addRegion("association_cortex")
    region_ids.append(association_region_id)
    
    # Working memory region
    memory_region_id = brain.addRegion("working_memory")
    region_ids.append(memory_region_id)
    
    # Return region IDs for specialized access
    return region_ids

def configure_region_specialization(brain, region_id, neuron_type, density):
    """Configure a specific region for specialized function."""
    
    # In a real implementation, this would configure the region's
    # specific parameters, connectivity patterns, and plasticity rules
    
    region = brain.getRegion(region_id)
    
    if neuron_type == pynlm.NeuronType.Sensory:
        # Configure sensory region
        region.setProperty("sensory_modality", "vision")
        region.setProperty("receptive_field_size", 4)
        region.setProperty("input_gain", 1.5)
        region.setProperty("output_gain", 0.8)
        
    elif neuron_type == pynlm.NeuronType.Motor:
        # Configure motor region
        region.setProperty("output_type", "coordinated_action")
        region.setProperty("movement_resolution", 8)
        region.setProperty("output_gain", 2.0)
        region.setProperty("burst_threshold", -55)
        
    elif neuron_type == pynlm.NeuronType.Internal:
        # Configure association/memory region
        region.setProperty("memory_capacity", "working")
        region.setProperty("recurrent_connections", True)
        region.setProperty("plasticity_rate", 0.8)
        region.setProperty("inhibition_strength", 0.6)
    
    return region

def create_cortically_plausible_network(brain):
    """Create a brain inspired by cortical architecture."""
    
    # Clear existing regions and create new cortical-like structure
    regions = brain.getRegions()
    for region in regions:
        # Remove existing regions
        brain.removeRegion(region.getId())
    
    # Create six-layer cortical architecture
    cortical_layers = {
        'layer_1': pynlm.NeuronType.Sensory,      # Input layer
        'layer_2_3': pynlm.NeuronType.Internal,  # Processing layer
        'layer_4': pynlm.NeuronType.Sensory,      # Input layer
        'layer_5': pynlm.NeuronType.Motor,       # Output layer
        'layer_6': pynlm.NeuronType.Internal,    # Feedback layer
    }
    
    region_ids = []
    for layer_name, neuron_type in cortical_layers.items():
        # Set layer-specific parameters
        if 'sensory' in layer_name:
            neuron_count = 100
            connectivity_density = 0.15
            input_gain = 1.2
        elif 'motor' in layer_name:
            neuron_count = 80
            connectivity_density = 0.2
            output_gain = 1.8
        else:  # internal
            neuron_count = 150
            connectivity_density = 0.25
            recurrent_strength = 0.3
        
        # Create region
        region_id = brain.addRegion(layer_name)
        region_ids.append(region_id)
        
        # Configure region specialization
        configure_region_specialization(brain, region_id, neuron_type, connectivity_density)
    
    # Create inter-layer connections (feedforward and feedback)
    create_cortical_connections(brain, region_ids)
    
    return region_ids

def create_cortical_connections(brain, region_ids):
    """Create feedforward and feedback connections between cortical layers."""
    
    # Layer IDs
    l1_id, l2_3_id, l4_id, l5_id, l6_id = region_ids
    
    # Feedforward connections (simplified)
    brain.addInterRegionConnection(l1_id, l2_3_id, weight=1.0, delay=1)
    brain.addInterRegionConnection(l2_3_id, l5_id, weight=1.2, delay=2)
    brain.addInterRegionConnection(l4_id, l5_id, weight=1.5, delay=1)
    
    # Recurrent connections within layers
    brain.addInterRegionConnection(l2_3_id, l2_3_id, weight=0.8, delay=1)
    brain.addInterRegionConnection(l5_id, l5_id, weight=0.6, delay=1)
    brain.addInterRegionConnection(l6_id, l2_3_id, weight=1.0, delay=1)  # Feedback
    
    # Lateral inhibition between sensory layers
    brain.addInterRegionConnection(l1_id, l4_id, weight=-0.5, delay=1)
    brain.addInterRegionConnection(l4_id, l1_id, weight=-0.5, delay=1)

# Example usage
base_config = pynlm.createDefaultConfig()
base_config.set("brain.neuron_count", 2000)

print("Creating cortical-like brain architecture...")
cortical_regions = create_cortically_plausible_network(brain)
print(f"Created {len(cortical_regions)} cortical regions")

# Use the brain in a simulation
brain = create_multi_region_brain(base_config)
print("Brain ready for simulation")
```

### Hierarchical Memory Systems

```python
import pynlm

class HierarchicalMemorySystem:
    """Hierarchical memory system inspired by biological memory."""
    
    def __init__(self, brain):
        self.brain = brain
        self.memory_levels = {
            'working': [],      # Short-term, active memory
            'short_term': [],   # Minutes-long memory
            'long_term': [],    # Hours/days memory
            'semantic': [],     # Conceptual knowledge
            'episodic': []      # Specific experiences
        }
        self.consolidation_threshold = 0.7
    
    def store_experience(self, experience):
        """Store an experience at the appropriate memory level."""
        
        # Determine memory level based on experience characteristics
        memory_level = self.determine_memory_level(experience)
        
        # Store with metadata
        memory_entry = {
            'timestamp': experience['timestamp'],
            'sensory_input': experience['sensory_input'],
            'action_taken': experience['action'],
            'reward': experience['reward'],
            'novelty': experience['novelty'],
            'relevance': experience['relevance'],
            'strength': 1.0
        }
        
        # Store at appropriate level
        self.memory_levels[memory_level].append(memory_entry)
        
        # If experience is important, also store in higher levels
        if memory_entry['relevance'] > self.consolidation_threshold:
            self.consolidate_to_long_term(memory_entry)
        
        return memory_level
    
    def determine_memory_level(self, experience):
        """Determine where to store based on experience characteristics."""
        
        # Factors affecting memory level:
        
        # 1. Novelty
        if experience['novelty'] > 0.8:
            return 'short_term'  # Very novel experiences go to short-term
        
        # 2. Reward
        if experience['reward'] > 0.5:
            return 'long_term'   # High-reward experiences consolidate
        
        # 3. Duration
        if experience['duration'] > 1000:  # Long experiences
            return 'semantic'    # More conceptual
        
        # 4. Stress/Emotional significance
        if experience['stress_level'] > 0.6:
            return 'long_term'   # Emotional experiences consolidate
        
        # Default: working memory
        return 'working'
    
    def consolidate_to_long_term(self, memory_entry):
        """Move important memories to long-term storage."""
        
        # Create semantic representation if not already present
        semantic_key = self.extract_semantic_key(memory_entry)
        
        if semantic_key not in self.memory_levels['semantic']:
            semantic_entry = {
                'concept': semantic_key,
                'first_occurrence': memory_entry['timestamp'],
                'strength': 0.0,
                'typical_reward': memory_entry['reward'],
                'typical_action': memory_entry['action'],
                'occurrence_count': 0
            }
            self.memory_levels['semantic'].append(semantic_entry)
        
        # Find semantic entry and update
        semantic_entry = next(e for e in self.memory_levels['semantic'] 
                             if e['concept'] == semantic_key)
        semantic_entry['strength'] += memory_entry['strength'] * 0.1
        semantic_entry['occurrence_count'] += 1
        
        # Add to episodic memory
        self.memory_levels['episodic'].append(memory_entry)
    
    def retrieve_similar_experiences(self, query_pattern, max_results=5):
        """Retrieve experiences similar to query pattern."""
        
        similarities = []
        
        # Search all memory levels
        for level_name, experiences in self.memory_levels.items():
            for experience in experiences:
                # Compute similarity (simplified)
                similarity = self.compute_similarity(
                    experience['sensory_input'],
                    query_pattern
                )
                
                # Weight by memory level
                level_weight = {'working': 0.2, 'short_term': 0.4, 
                              'long_term': 0.6, 'semantic': 0.8, 'episodic': 1.0}
                
                weighted_similarity = similarity * level_weight[level_name]
                
                if weighted_similarity > 0.1:  # Threshold
                    similarities.append({
                        'experience': experience,
                        'similarity': weighted_similarity,
                        'level': level_name
                    })
        
        # Sort by similarity and return top results
        similarities.sort(key=lambda x: x['similarity'], reverse=True)
        return similarities[:max_results]
    
    def extract_semantic_key(self, experience):
        """Extract semantic key from experience for consolidation."""
        
        # Simple key extraction based on dominant features
        features = []
        
        # Add action type
        features.append(experience['action'])
        
        # Add reward category
        if experience['reward'] > 0.5:
            features.append('high_reward')
        elif experience['reward'] > 0.1:
            features.append('low_reward')
        else:
            features.append('neutral_reward')
        
        # Add sensory category
        sensory_patterns = experience['sensory_input']
        if max(sensory_patterns) > 0.7:
            features.append('high_intensity')
        
        # Return semantic key
        return '_'.join(features)
    
    def compute_similarity(self, pattern1, pattern2):
        """Compute similarity between two patterns (simplified)."""
        
        if len(pattern1) != len(pattern2):
            return 0.0
        
        # Cosine similarity
        dot_product = sum(a * b for a, b in zip(pattern1, pattern2))
        norm1 = sum(a * a for a in pattern1) ** 0.5
        norm2 = sum(b * b for b in pattern2) ** 0.5
        
        if norm1 * norm2 == 0:
            return 0.0
        
        return dot_product / (norm1 * norm2)

# Example usage
brain_config = pynlm.createDefaultConfig()
brain_config.set("brain.neuron_count", 1000)

brain = pynlm.createBrain(brain_config)
brain.initialize()

# Create hierarchical memory system
hierarchical_memory = HierarchicalMemorySystem(brain)

# Simulate storing experiences
experiences = []
for i in range(50):
    # Create simulated experience
    experience = {
        'timestamp': i * 0.1,
        'sensory_input': [0.1 * (i % 10), 0.2 * ((i + 3) % 7), 0.3 * ((i * 2) % 5)],
        'action': pynlm.ActionType.MoveForward,
        'reward': 0.5 + (i % 5) * 0.1,
        'novelty': (i % 10) / 10.0,
        'relevance': (i % 8) / 10.0,
        'duration': i * 0.5,
        'stress_level': (i % 6) / 10.0
    }
    
    experiences.append(experience)
    memory_level = hierarchical_memory.store_experience(experience)
    
    if i % 10 == 0:
        print(f"Experience {i} stored in {memory_level} memory")

# Retrieve similar experiences
print("\nRetrieving similar experiences for query pattern:")
query_pattern = [0.2, 0.3, 0.1]
retrieved = hierarchical_memory.retrieve_similar_experiences(query_pattern)

for i, result in enumerate(retrieved[:3]):
    print(f"Result {i+1}:")
    print(f"  Level: {result['level']}")
    print(f"  Similarity: {result['similarity']:.3f}")
    print(f"  Timestamp: {result['experience']['timestamp']}")
    print(f"  Action: {result['experience']['action']}")
    print(f"  Reward: {result['experience']['reward']}")
    print()
```

### Neuromodulation Network Dynamics

```python
import pynlm
import numpy as np

class NeuromodulationNetwork:
    """Complex neuromodulation network with cross-system interactions."""
    
    def __init__(self, brain, agent):
        self.brain = brain
        self.agent = agent
        self.modulation_systems = {
            'dopamine': DopamineSystem(),
            'curiosity': CuriositySystem(),
            'novelty': NoveltySystem(),
            'prediction_error': PredictionErrorSystem()
        }
        self.network_dynamics = NetworkDynamics()
        self.inhibition_strength = 0.8
        self.excitation_strength = 1.2
    
    def enable_advanced_modulation(self):
        """Enable advanced neuromodulation interactions."""
        
        # Configure cross-system interactions
        self.modulation_systems['dopamine'].add_input(
            self.modulation_systems['prediction_error'],
            weight=0.6
        )
        
        self.modulation_systems['curiosity'].add_input(
            self.modulation_systems['novelty'],
            weight=0.8
        )
        
        self.modulation_systems['novelty'].add_inhibitory_input(
            self.modulation_systems['dopamine'],
            weight=0.3
        )
        
        self.modulation_systems['prediction_error'].add_inhibitory_input(
            self.modulation_systems['curiosity'],
            weight=0.4
        )
        
        # Enable feedback loops
        self.network_dynamics.add_feedback_loop(
            self.modulation_systems['dopamine'],
            self.modulation_systems['curiosity']
        )
        
        self.network_dynamics.add_feedback_loop(
            self.modulation_systems['curiosity'],
            self.modulation_systems['novelty']
        )
        
        self.network_dynamics.add_feedback_loop(
            self.modulation_systems['novelty'],
            self.modulation_systems['prediction_error']
        )
    
    def run_advanced_modulation_dynamics(self, num_steps):
        """Run advanced neuromodulation dynamics simulation."""
        
        # Initialize modulation systems
        for system_name, system in self.modulation_systems.items():
            system.initialize(self.brain, self.agent)
        
        # Main modulation dynamics loop
        for step in range(num_steps):
            # Update each modulation system
            self.update_modulation_systems()
            
            # Apply cross-system interactions
            self.apply_cross_system_modulation()
            
            # Update network dynamics (inhibition/excitation balance)
            self.update_network_dynamics()
            
            # Apply neuromodulation to brain
            self.apply_modulation_to_brain()
            
            # Log system state periodically
            if step % 100 == 0:
                self.log_modulation_dynamics(step)
    
    def update_modulation_systems(self):
        """Update individual modulation systems."""
        
        # Get current brain state for modulation systems
        current_firing_rate = self.brain.getAverageFiringRate()
        current_activity = self.brain.getFiringNeuronCount()
        
        # Update each system
        for system_name, system in self.modulation_systems.items():
            system.update(current_firing_rate, current_activity)
    
    def apply_cross_system_modulation(self):
        """Apply cross-system interactions and inhibition."""
        
        # Get current modulation levels
        dopamine_level = self.modulation_systems['dopamine'].get_level()
        curiosity_level = self.modulation_systems['curiosity'].get_level()
        novelty_level = self.modulation_systems['novelty'].get_level()
        prediction_error_level = self.modulation_systems['prediction_error'].get_level()
        
        # Apply cross-system weights (inhibition and excitation)
        # Dopamine modulates curiosity: high dopamine -> high curiosity
        dopamine_effect_on_curiosity = dopamine_level * self.excitation_strength
        
        # Novelty inhibits dopamine: high novelty -> low dopamine
        novelty_inhibition_on_dopamine = novelty_level * self.inhibition_strength
        
        # Curiosity and prediction error mutually inhibit
        curiosity_inhibition_on_prediction_error = curiosity_level * self.inhibition_strength
        prediction_error_inhibition_on_curiosity = prediction_error_level * self.inhibition_strength
        
        # Novelty and dopamine interact
        dopamine_excitation_on_novelty = dopamine_level * 0.3
        novelty_inhibition_on_dopamine_feedback = novelty_level * 0.5
        
        # Update systems based on interactions
        self.modulation_systems['curiosity'].add_excitatory_input(
            'dopamine_effect', dopamine_effect_on_curiosity
        )
        
        self.modulation_systems['dopamine'].add_inhibitory_input(
            'novelty_inhibition', novelty_inhibition_on_dopamine
        )
        
        self.modulation_systems['prediction_error'].add_inhibitory_input(
            'curiosity_inhibition', curiosity_inhibition_on_prediction_error
        )
        
        self.modulation_systems['curiosity'].add_inhibitory_input(
            'prediction_error_inhibition', prediction_error_inhibition_on_curiosity
        )
    
    def update_network_dynamics(self):
        """Update network-level dynamics and oscillations."""
        
        # Get all modulation levels
        modulation_levels = {
            name: system.get_level() 
            for name, system in self.modulation_systems.items()
        }
        
        # Apply network dynamics
        self.network_dynamics.update(
            modulation_levels,
            self.inhibition_strength,
            self.excitation_strength
        )
        
        # Update system states based on network dynamics
        for system_name, system in self.modulation_systems.items():
            system.update_from_network(
                self.network_dynamics.get_network_state(system_name)
            )
    
    def apply_modulation_to_brain(self):
        """Apply neuromodulation effects to brain systems."""
        
        # Create neuromodulator signal from combined effects
        neuromodulator = pynlm.Neuromodulator()
        
        # Compute combined neuromodulatory signal
        combined_signal = (
            self.modulation_systems['dopamine'].get_level() * 0.4 +
            self.modulation_systems['curiosity'].get_level() * 0.3 +
            self.modulation_systems['novelty'].get_level() * 0.2 +
            self.modulation_systems['prediction_error'].get_level() * 0.1
        )
        
        # Apply neuromodulation
        self.brain.applyNeuromodulation(neuromodulator)
        
        # Update agent's neuromodulation level
        self.agent.applyRewardModulation(
            combined_signal,
            combined_signal * 0.8  # Predictive component
        )
    
    def log_modulation_dynamics(self, step):
        """Log modulation system state."""
        
        print(f"\n=== Modulation Dynamics at Step {step} ===")
        
        for system_name, system in self.modulation_systems.items():
            level = system.get_level()
            history = system.get_history(step-100, step)
            avg_level = np.mean(history) if history else level
            
            print(f"{system_name.capitalize()}:")
            print(f"  Current level: {level:.3f}")
            print(f"  Average (last 100 steps): {avg_level:.3f}")
            print(f"  Trend: {'↑' if level > avg_level else '↓' if level < avg_level else '→'}")
        
        # Show network state
        network_state = self.network_dynamics.get_overall_state()
        print(f"Network State:")
        print(f"  Inhibition/Excitement Ratio: {network_state['inhibition_excitement_ratio']:.3f}")
        print(f"  Network Oscillation Strength: {network_state['oscillation_strength']:.3f}")
        print(f"  System Coherence: {network_state['system_coherence']:.3f}")

# Supporting system classes for advanced neuromodulation

class Neuromodulator:
    """Neuromodulator signal for brain systems."""
    
    def __init__(self):
        self.dopamine_level = 0.0
        self.curiosity_level = 0.0
        self.novelty_level = 0.0
        self.prediction_error_level = 0.0
        self.signal_strength = 0.0
        self.timestamp = 0.0
    
    def set_levels(self, **kwargs):
        """Set neuromodulator levels."""
        for key, value in kwargs.items():
            setattr(self, f"{key}_level", value)
    
    def compute_signal(self):
        """Compute overall neuromodulatory signal."""
        self.signal_strength = (
            self.dopamine_level * 0.4 +
            self.curiosity_level * 0.3 +
            self.novelty_level * 0.2 +
            self.prediction_error_level * 0.1
        )
        return self.signal_strength

class NetworkDynamics:
    """Network-level dynamics and interactions between neuromodulation systems."""
    
    def __init__(self):
        self.systems = {}
        self.feedback_loops = []
        self.inhibition_strength = 0.8
        self.excitation_strength = 1.2
    
    def add_feedback_loop(self, source_system, target_system):
        """Add a feedback loop between systems."""
        self.feedback_loops.append({
            'source': source_system,
            'target': target_system,
            'weight': 0.5
        })
    
    def update(self, modulation_levels, inhibition, excitation):
        """Update network dynamics based on current modulation levels."""
        
        self.inhibition_strength = inhibition
        self.excitation_strength = excitation
        
        # Compute interaction strengths
        for system_name, level in modulation_levels.items():
            # Store system state
            self.systems[system_name] = {
                'current_level': level,
                'inhibition': self.inhibition_strength,
                'excitation': self.excitation_strength,
                'network_state': self.compute_system_network_state(system_name, modulation_levels)
            }
    
    def compute_system_network_state(self, system_name, all_levels):
        """Compute network state for a specific system."""
        
        # Compute interactions with other systems
        excitation = 0.0
        inhibition = 0.0
        
        for other_name, level in all_levels.items():
            if other_name != system_name:
                # Determine interaction type based on system pair
                interaction_type = self.get_interaction_type(system_name, other_name)
                
                if interaction_type == 'excitation':
                    excitation += level * self.excitation_strength
                elif interaction_type == 'inhibition':
                    inhibition += level * self.inhibition_strength
        
        # Compute network state metrics
        net_excitation = excitation - inhibition
        oscillation_strength = abs(net_excitation) / (excitation + inhibition + 0.001)
        
        return {
            'net_excitation': net_excitation,
            'oscillation_strength': oscillation_strength,
            'inhibition_excitement_ratio': inhibition / (excitation + 0.001)
        }
    
    def get_network_state(self, system_name):
        """Get network state for a specific system."""
        return self.systems.get(system_name, {})
    
    def get_overall_state(self):
        """Get overall network state."""
        
        all_states = list(self.systems.values())
        if not all_states:
            return {}
        
        avg_inhibition_ratio = np.mean([s['inhibition_excitement_ratio'] for s in all_states])
        avg_oscillation = np.mean([s['oscillation_strength'] for s in all_states])
        
        # Compute system coherence (how synchronized the systems are)
        levels = list(self.systems.keys())
        coherence_scores = []
        
        for i in range(len(levels)):
            for j in range(i + 1, len(levels)):
                level1 = levels[i]
                level2 = levels[j]
                
                state1 = self.systems[level1]['network_state']
                state2 = self.systems[level2]['network_state']
                
                # Coherence based on similar network states
                coherence = 1.0 - abs(state1['net_excitation'] - state2['net_excitation'])
                coherence_scores.append(coherence)
        
        system_coherence = np.mean(coherence_scores) if coherence_scores else 0.0
        
        return {
            'inhibition_excitement_ratio': avg_inhibition_ratio,
            'oscillation_strength': avg_oscillation,
            'system_coherence': system_coherence
        }

class DopamineSystem:
    """Dopamine neuromodulation system."""
    
    def __init__(self):
        self.level = 0.0
        self.history = []
        self.trend = 0.0
    
    def initialize(self, brain, agent):
        """Initialize dopamine system with brain and agent references."""
        self.brain = brain
        self.agent = agent
    
    def update(self, firing_rate, activity):
        """Update dopamine level based on brain activity."""
        
        # Dopamine responds to reward prediction errors
        # Simple model: positive reward prediction errors increase dopamine
        
        # Get current reward prediction error from agent
        reward_prediction_error = self.agent.getPredictionError()
        
        # Update dopamine level with temporal dynamics
        dopamine_delta = reward_prediction_error * 0.1 - self.level * 0.05
        self.level += dopamine_delta
        
        # Clip to reasonable range
        self.level = max(0.0, min(1.0, self.level))
        
        # Store in history
        self.history.append(self.level)
    
    def add_input(self, other_system, weight):
        """Add excitatory input from another system."""
        pass  # Simplified for example
    
    def get_level(self):
        return self.level
    
    def get_history(self, start, end):
        if start < 0:
            start = 0
        return self.history[start:end]

class CuriositySystem:
    """Curiosity-driven exploration system."""
    
    def __init__(self):
        self.level = 0.0
        self.history = []
        self.sensitivity = 1.0
    
    def initialize(self, brain, agent):
        self.brain = brain
        self.agent = agent
    
    def update(self, firing_rate, activity):
        """Update curiosity level."""
        
        # Curiosity responds to novelty and uncertainty
        novelty_level = self.agent.getNoveltyLevel()
        
        # Update with temporal dynamics
        curiosity_delta = novelty_level * self.sensitivity * 0.05 - self.level * 0.03
        self.level += curiosity_delta
        
        # Clip to reasonable range
        self.level = max(0.0, min(1.0, self.level))
        
        self.history.append(self.level)
    
    def add_excitatory_input(self, source, weight):
        pass  # Simplified
    
    def add_inhibitory_input(self, source, weight):
        pass  # Simplified
    
    def get_level(self):
        return self.level
    
    def get_history(self, start, end):
        if start < 0:
            start = 0
        return self.history[start:end]
    
    def update_from_network(self, network_state):
        """Update from network dynamics."""
        pass

class NoveltySystem:
    """Novelty detection system."""
    
    def __init__(self):
        self.level = 0.0
        self.history = []
        self.threshold = 0.5
    
    def initialize(self, brain, agent):
        self.brain = brain
        self.agent = agent
    
    def update(self, firing_rate, activity):
        """Update novelty level."""
        
        # Novelty responds to changes in sensory input
        # Simplified: use brain activity as proxy for novelty
        
        # Compute change in activity
        activity_change = abs(activity - self.level) if self.history else 0.0
        
        # Update novelty level
        novelty_delta = activity_change * 0.1 - self.level * 0.02
        self.level += novelty_delta
        
        # Apply threshold
        if self.level < self.threshold:
            self.level = 0.0
        
        # Clip to reasonable range
        self.level = max(0.0, min(1.0, self.level))
        
        self.history.append(self.level)
    
    def add_inhibitory_input(self, source, weight):
        pass
    
    def get_level(self):
        return self.level
    
    def get_history(self, start, end):
        if start < 0:
            start = 0
        return self.history[start:end]

class PredictionErrorSystem:
    """Prediction error signal system."""
    
    def __init__(self):
        self.level = 0.0
        self.history = []
        self.error_threshold = 0.2
    
    def initialize(self, brain, agent):
        self.brain = brain
        self.agent = agent
    
    def update(self, firing_rate, activity):
        """Update prediction error level."""
        
        # Prediction error responds to unexpected outcomes
        # Simplified: use agent's prediction error
        agent_prediction_error = self.agent.getPredictionError()
        
        # Update with temporal dynamics
        error_delta = agent_prediction_error * 0.1 - self.level * 0.04
        self.level += error_delta
        
        # Apply threshold
        if abs(self.level) < self.error_threshold:
            self.level = 0.0
        
        # Clip to reasonable range
        self.level = max(0.0, min(1.0, self.level))
        
        self.history.append(self.level)
    
    def add_inhibitory_input(self, source, weight):
        pass
    
    def get_level(self):
        return self.level
    
    def get_history(self, start, end):
        if start < 0:
            start = 0
        return self.history[start:end]

# Example usage of advanced neuromodulation
brain_config = pynlm.createDefaultConfig()
brain_config.set("brain.neuron_count", 1500)

brain = pynlm.createBrain(brain_config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(None)  # Will be set by world

# Create advanced neuromodulation network
advanced_modulation = NeuromodulationNetwork(brain, agent)

# Enable advanced modulation with cross-system interactions
advanced_modulation.enable_advanced_modulation()

print("Starting advanced neuromodulation simulation...")
advanced_modulation.run_advanced_modulation_dynamics(500)

print("Advanced neuromodulation simulation completed!")
```