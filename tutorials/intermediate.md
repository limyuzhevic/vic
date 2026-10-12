# NLM Intermediate Tutorials

Welcome to the Neural Learning Machine (NLM) intermediate tutorial! This guide builds on the basics and introduces more advanced concepts, custom configurations, and complex learning scenarios.

## Table of Contents

1. [Advanced Configuration Management](#advanced-configuration-management)
2. [Custom Neuron Models](#custom-neuron-models)
3. [Memory System Implementation](#memory-system-implementation)
4. [Neuromodulation Deep Dive](#neuromodulation-deep-dive)
5. [Complex Learning Scenarios](#complex-learning-scenarios)
6. [Performance Optimization](#performance-optimization)
7. [Debugging and Troubleshooting](#debugging-and-troubleshooting)

---

## Advanced Configuration Management

### Configuration Schema Design

NLM uses an INI-style configuration format. For advanced usage, you can create complex configurations:

```python
from nlm.config import ConfigManager

class AdvancedConfigManager(ConfigManager):
    def __init__(self):
        super().__init__()
        self.setup_default_values()
    
    def setup_default_values(self):
        # Core settings
        self.add_section('core', {
            'seed': 42,
            'timestep': 0.001,
            'simulation_steps': 10000,
            'neuron_count': 1000,
            'region_count': 2,
            'connection_probability': 0.2,
            'max_synapses_per_neuron': 50
        })
        
        # Neuron dynamics with variability
        self.add_section('neuron', {
            'v_rest': -65.0,
            'v_thresh': -50.0,
            'v_reset': -65.0,
            'v_decay': 0.95,
            'v_refractory': 0.005,
            'noise_level': 0.1
        })
        
        # Region-specific configurations
        self.add_section('region.excitatory', {
            'type': 'excitatory',
            'neuron_density': 'high',
            'plasticity_enabled': True,
            'learning_rate': 0.05
        })
        
        self.add_section('region.inhibitory', {
            'type': 'inhibitory',
            'neuron_density': 'medium',
            'plasticity_enabled': True,
            'learning_rate': 0.03
        })
        
        # Advanced neuromodulation
        self.add_section('neuromodulation', {
            'dopamine_enabled': True,
            'curiosity_enabled': True,
            'novelty_enabled': True,
            'prediction_error_enabled': True,
            'modulation_strength': 1.0
        })
        
        # Complex memory configurations
        self.add_section('memory', {
            'working_memory_enabled': True,
            'working_memory_capacity': 500,
            'episodic_memory_enabled': True,
            'episodic_memory_capacity': 2000,
            'semantic_memory_enabled': True,
            'semantic_memory_capacity': 10000,
            'procedural_memory_enabled': True,
            'procedural_memory_capacity': 1000,
            'associative_memory_enabled': True,
            'associative_memory_capacity': 5000
        })
        
        # Advanced memory parameters
        self.add_section('memory.working_memory', {
            'std_decay_rate': 0.999,
            'top_activation_threshold': 0.5,
            'competitive_strength': 0.8,
            'encoding_strength': 0.3,
            'replay_probability': 0.1
        })
        
        # Development configuration
        self.add_section('development', {
            'stage_transitions': True,
            'critical_period': 100.0,
            'maturation_rate': 0.01,
            'neuron_maturation_enabled': True,
            'synapse_maturation_enabled': True,
            'region_maturation_enabled': True
        })
        
        # Performance optimization
        self.add_section('performance', {
            'use_memory_pool': True,
            'use_event_queue': True,
            'use_simd': True,
            'parallel_neuron_count': 4,
            'enable_optimization': True,
            'optimization_level': 'balanced'
        })
        
        # Logging configuration
        self.add_section('logging', {
            'level': 'info',
            'file': 'nlm_simulation.log',
            'console': True,
            'timestamp': True,
            'log_file_size': '100MB',
            'backup_count': 5,
            'format': '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        })

# Use the advanced configuration manager
config_manager = AdvancedConfigManager()
config = config_manager.get_config()

# Save configuration
config_manager.save_config("configs/advanced_config.cfg")

# Load and validate
nlm = NeuralLearningMachine.load_config("configs/advanced_config.cfg")
```

### Configuration Validation with Custom Rules

Create custom validation for your specific needs:

```python
from scripts.validate_config import ConfigValidator

class CustomValidator(ConfigValidator):
    def __init__(self, config_path: str):
        super().__init__(config_path)
    
    def validate_custom_rules(self) -> None:
        """Apply custom validation rules."""
        if 'neuron' in self.config:
            # Check neuron parameters are within reasonable ranges
            v_rest = float(self.config['neuron'].get('v_rest', -65.0))
            if v_rest < -100 or v_rest > 0:
                self.warnings.append(f"Unusual resting potential: {v_rest} mV")
            
            # Check threshold is more positive than resting potential
            if 'v_thresh' in self.config['neuron']:
                v_thresh = float(self.config['neuron']['v_thresh'])
                if v_thresh <= v_rest:
                    self.errors.append(f"Threshold potential ({v_thresh}) must be more positive than resting potential ({v_rest})")
        
        # Validate memory consistency
        if 'memory' in self.config:
            memory = self.config['memory']
            total_capacity = sum([
                int(memory.get(f'{mem_type}_memory_capacity', 0))
                for mem_type in ['working', 'episodic', 'semantic', 'procedural', 'associative']
            ])
            
            if total_capacity > 100000:
                self.warnings.append("Total memory capacity is very high - may impact performance")
        
        # Validate neuromodulation consistency
        if 'neuromodulation' in self.config:
            neuromod = self.config['neuromodulation']
            enabled_systems = [k for k, v in neuromod.items() if str(v).lower() == 'true']
            
            if len(enabled_systems) < 2:
                self.warnings.append("At least 2 neuromodulation systems should be enabled for balanced learning")

# Use custom validator
validator = CustomValidator("configs/my_config.cfg")
results = validator.validate_all()
if results['status'] == 'valid':
    print("✓ Custom validation passed")
else:
    print(f"✗ Custom validation failed: {results['errors']}")
```

---

## Custom Neuron Models

### Implementing Custom Neuron Dynamics

NLM allows you to implement custom neuron models by extending the base neuron class:

```python
import numpy as np
from nlm.neuron import BaseNeuron

class CustomNeuron(BaseNeuron):
    """Custom neuron with adaptive dynamics."""
    
    def __init__(self, neuron_id: int, config: Dict):
        super().__init__(neuron_id, config)
        self.adaptation_rate = config.get('adaptation_rate', 0.01)
        self.plasticity_history = []
        self.input_history = []
    
    def adaptive_integration(self, inputs: List[float], current_time: float) -> float:
        """Adaptive integration with history-dependent dynamics."""
        # Calculate weighted input
        weighted_input = np.dot(self.weights, inputs)
        
        # Apply adaptive filtering based on input history
        if self.input_history:
            recent_inputs = np.array(self.input_history[-min(10, len(self.input_history)):]).mean(axis=0)
            adaptive_factor = 1.0 - self.adaptation_rate * np.dot(recent_inputs, inputs)
            weighted_input *= adaptive_factor
        
        # Update state using adaptive integration
        self.state += weighted_input * self.config.get('time_constant', 0.01)
        
        # Apply adaptive thresholds
        adaptive_threshold = self.v_thresh * (1.0 + self.adaptation_rate * len(self.plasticity_history))
        
        # Update input history
        self.input_history.append(inputs)
        if len(self.input_history) > 100:
            self.input_history.pop(0)
        
        return self.state >= adaptive_threshold
    
    def learn_from_error(self, error: float, learning_rate: float, time_step: float):
        """Learn from error with adaptive learning rate."""
        # Calculate adaptive learning rate based on plasticity history
        if self.plasticity_history:
            recent_plasticity = np.mean(self.plasticity_history[-10:])
            adaptive_lr = learning_rate * (1.0 + self.adaptation_rate * recent_plasticity)
        else:
            adaptive_lr = learning_rate
        
        # Update weights based on error
        self.weights += adaptive_lr * error * self.input_history[-1] if self.input_history else error
        
        # Record plasticity event
        self.plasticity_history.append(adaptive_lr)
        if len(self.plasticity_history) > 1000:
            self.plasticity_history.pop(0)
    
    def get_state_representation(self) -> Dict:
        """Get neuron state representation for visualization."""
        return {
            'id': self.id,
            'type': self.neuron_type,
            'state': self.state,
            'threshold': self.v_thresh,
            'weights': self.weights.tolist(),
            'adaptation_rate': self.adaptation_rate,
            'plasticity_history': self.plasticity_history[-100:] if self.plasticity_history else [],
            'input_history': self.input_history[-10:] if self.input_history else []
        }

# Use custom neuron in NLM
class CustomNLM(NeuralLearningMachine):
    def __init__(self, config: Dict = None):
        super().__init__(config)
        self.custom_neurons = []
    
    def create_custom_neuron(self, neuron_id: int, config: Dict) -> CustomNeuron:
        """Create a custom neuron."""
        neuron = CustomNeuron(neuron_id, config)
        self.custom_neurons.append(neuron)
        return neuron

# Example usage
config = modify_config({
    "core": {"neuron_count": 100},
    "neuron": {
        "v_rest": -70.0,
        "v_thresh": -55.0,
        "adaptation_rate": 0.02
    }
})

nlm = CustomNLM(config)
```

### Neuron Network Analysis

Analyze your custom neuron networks:

```python
from scripts.visualize import BrainVisualizer

class NeuronNetworkAnalyzer:
    def __init__(self, nlm: NeuralLearningMachine):
        self.nlm = nlm
        self.visualizer = BrainVisualizer()
    
    def analyze_connectivity(self) -> Dict:
        """Analyze neural connectivity patterns."""
        neurons = self.nlm.get_neurons()
        connections = self.nlm.get_connections()
        
        # Calculate network metrics
        metrics = {
            'total_neurons': len(neurons),
            'total_connections': len(connections),
            'connection_density': len(connections) / (len(neurons) * (len(neurons) - 1)) if len(neurons) > 1 else 0,
            'average_degree': 2 * len(connections) / len(neurons) if len(neurons) > 0 else 0,
            'small_world_coefficient': self.calculate_small_world_coefficient(neurons, connections),
            'clustering_coefficient': self.calculate_clustering_coefficient(neurons, connections),
            'efficiency': self.calculate_efficiency(neurons, connections)
        }
        
        return metrics
    
    def calculate_small_world_coefficient(self, neurons, connections) -> float:
        """Calculate small-world coefficient."""
        # Implementation would involve comparing clustering coefficient and path length
        # to equivalent random and regular networks
        return 0.5  # Placeholder
    
    def calculate_clustering_coefficient(self, neurons, connections) -> float:
        """Calculate clustering coefficient."""
        if len(neurons) < 3:
            return 0.0
        
        # Calculate clustering coefficient for each neuron
        clustering_coeffs = []
        
        for neuron in neurons:
            neighbors = [conn['target'] for conn in connections if conn['source'] == neuron['id']]
            neighbors.extend([conn['source'] for conn in connections if conn['target'] == neuron['id']])
            neighbors = list(set(neighbors))
            
            if len(neighbors) < 2:
                continue
            
            # Count triangles
            triangles = 0
            for i in range(len(neighbors)):
                for j in range(i + 1, len(neighbors)):
                    # Check if neighbors i and j are connected
                    is_connected = any(
                        (conn['source'] == neighbors[i] and conn['target'] == neighbors[j]) or
                        (conn['source'] == neighbors[j] and conn['target'] == neighbors[i])
                        for conn in connections
                    )
                    if is_connected:
                        triangles += 1
            
            # Calculate clustering coefficient for this neuron
            max_triangles = len(neighbors) * (len(neighbors) - 1) / 2
            if max_triangles > 0:
                clustering_coeffs.append(triangles / max_triangles)
        
        return np.mean(clustering_coeffs) if clustering_coeffs else 0.0
    
    def calculate_efficiency(self, neurons, connections) -> float:
        """Calculate network efficiency."""
        if len(neurons) < 2:
            return 0.0
        
        # Calculate inverse path length for all pairs
        total_efficiency = 0
        pairs = 0
        
        for i, neuron1 in enumerate(neurons):
            for j, neuron2 in enumerate(neurons):
                if i != j:
                    # Calculate shortest path
                    path_length = self.calculate_shortest_path(neuron1['id'], neuron2['id'], connections)
                    if path_length > 0:
                        total_efficiency += 1.0 / path_length
                        pairs += 1
        
        return total_efficiency / pairs if pairs > 0 else 0.0
    
    def calculate_shortest_path(self, start_id: int, end_id: int, connections: List[Dict]) -> int:
        """Calculate shortest path between two neurons using BFS."""
        # Build adjacency list
        graph = {}
        for conn in connections:
            if conn['source'] not in graph:
                graph[conn['source']] = []
            if conn['target'] not in graph:
                graph[conn['target']] = []
            
            graph[conn['source']].append(conn['target'])
            graph[conn['target']].append(conn['source'])
        
        # BFS to find shortest path
        queue = [(start_id, 0)]
        visited = {start_id}
        
        while queue:
            current, distance = queue.pop(0)
            
            if current == end_id:
                return distance
            
            for neighbor in graph.get(current, []):
                if neighbor not in visited:
                    visited.add(neighbor)
                    queue.append((neighbor, distance + 1))
        
        return float('inf')  # No path found
    
    def create_connectivity_visualization(self, output_path: str = None):
        """Create connectivity visualization."""
        neurons = self.nlm.get_neurons()
        connections = self.nlm.get_connections()
        
        # Create network graph
        import networkx as nx
        G = nx.Graph()
        
        # Add neurons as nodes
        for neuron in neurons:
            G.add_node(neuron['id'], **neuron)
        
        # Add connections as edges
        for conn in connections:
            G.add_edge(conn['source'], conn['target'], weight=conn.get('weight', 1.0))
        
        # Create visualization
        pos = nx.spring_layout(G, seed=42)
        
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 7))
        
        # Plot network
        nx.draw_networkx_nodes(G, pos, ax=ax1, node_size=100, node_color='lightblue')
        nx.draw_networkx_edges(G, pos, ax=ax1, alpha=0.5)
        nx.draw_networkx_labels(G, pos, ax=ax1, font_size=8)
        ax1.set_title("Neural Network Connectivity")
        ax1.axis('off')
        
        # Plot connectivity histogram
        degrees = [G.degree(node) for node in G.nodes()]
        ax2.hist(degrees, bins=20, alpha=0.7, edgecolor='black')
        ax2.set_xlabel("Degree (Number of Connections)")
        ax2.set_ylabel("Frequency")
        ax2.set_title("Degree Distribution")
        ax2.grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if output_path is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_path = f"network_analysis_{timestamp}.png"
        
        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        plt.close()
        
        return output_path

# Use the analyzer
analyzer = NeuronNetworkAnalyzer(nlm)
connectivity_metrics = analyzer.analyze_connectivity()
print(f"Network metrics: {connectivity_metrics}")

# Create visualization
viz_path = analyzer.create_connectivity_visualization()
print(f"Visualization saved to: {viz_path}")
```

---

## Memory System Implementation

### Custom Memory Classes

Implement custom memory systems with advanced features:

```python
import numpy as np
from nlm.memory import WorkingMemory, EpisodicMemory, SemanticMemory

class EnhancedWorkingMemory(WorkingMemory):
    """Enhanced working memory with compression and attention."""
    
    def __init__(self, capacity: int = 100, decay_rate: float = 0.999, 
                 compression_threshold: float = 0.1, attention_span: int = 20):
        super().__init__(capacity, decay_rate)
        self.compression_threshold = compression_threshold
        self.attention_span = attention_span
        self.attention_weights = {}
        self.compression_index = {}
    
    def store_with_compression(self, key: str, value: Any, importance: float = 1.0):
        """Store value with compression based on importance."""
        # Store original value
        self.store(key, value, importance)
        
        # Compress if value is redundant or low importance
        if importance < self.compression_threshold:
            # Create compressed representation
            compressed_key = f"compressed_{key}"
            compressed_value = self.compress_value(value)
            
            # Store compressed version
            self.store(compressed_key, compressed_value, importance * 0.5)
            
            # Update compression index
            self.compression_index[key] = compressed_key
        
        # Update attention weights
        self.update_attention_weights(key, importance)
    
    def compress_value(self, value: Any) -> Any:
        """Compress value using dimensionality reduction."""
        if isinstance(value, np.ndarray) and value.size > 10:
            # Apply PCA-like dimensionality reduction
            from sklearn.decomposition import PCA
            pca = PCA(n_components=3)  # Reduce to 3 dimensions
            compressed = pca.fit_transform(value.reshape(1, -1))
            return compressed.flatten().tolist()
        elif isinstance(value, str) and len(value) > 100:
            # Compress string by keeping only key words
            words = value.split()
            key_words = [word for word in words if len(word) > 3]
            return ' '.join(key_words[:10])  # Keep first 10 key words
        else:
            return value
    
    def update_attention_weights(self, key: str, importance: float):
        """Update attention weights based on importance."""
        # Boost attention for high-importance items
        if key in self.storage:
            self.attention_weights[key] = importance * 0.9 + self.attention_weights.get(key, 0.1) * 0.1
        
        # Decay old attention weights
        for k in list(self.attention_weights.keys()):
            if k not in self.storage:
                del self.attention_weights[k]
            else:
                self.attention_weights[k] *= 0.95  # Decay factor
    
    def retrieve_with_attention(self, key: str) -> Optional[Any]:
        """Retrieve value with attention-based weighting."""
        if key not in self.storage:
            # Try to find compressed version
            if key in self.compression_index:
                compressed_key = self.compression_index[key]
                return self.storage.get(compressed_key)
            return None
        
        # Apply attention weighting
        attention_weight = self.attention_weights.get(key, 0.5)
        
        # Boost retrieval for high attention items
        if attention_weight > 0.7:
            # Immediate retrieval with higher priority
            return self.storage[key]
        else:
            # Standard retrieval
            return self.storage[key]
    
    def get_attention_summary(self) -> Dict:
        """Get summary of attention distribution."""
        if not self.attention_weights:
            return {'total_attention': 0, 'max_attention': 0, 'items': {}}
        
        total_attention = sum(self.attention_weights.values())
        max_attention = max(self.attention_weights.values()) if self.attention_weights else 0
        
        return {
            'total_attention': total_attention,
            'max_attention': max_attention,
            'items': self.attention_weights.copy()
        }

class HierarchicalEpisodicMemory(EpisodicMemory):
    """Hierarchical episodic memory with temporal abstraction."""
    
    def __init__(self, capacity: int = 1000, encoding_strength: float = 0.1):
        super().__init__(capacity, encoding_strength)
        self.hierarchy_levels = {
            'event': [],      # Specific events
            'day': [],        # Daily summaries
            'week': [],       # Weekly summaries
            'month': [],      # Monthly summaries
            'year': []        # Annual summaries
        }
        self.temporal_abstraction = True
        self.event_segmentation = True
    
    def store_episode_with_hierarchy(self, episode: Dict, timestamp: float):
        """Store episode with hierarchical temporal abstraction."""
        # Determine temporal level
        if timestamp < 86400:  # Less than 1 day
            level = 'event'
        elif timestamp < 604800:  # Less than 1 week
            level = 'day'
        elif timestamp < 2592000:  # Less than 1 month
            level = 'week'
        elif timestamp < 31536000:  # Less than 1 year
            level = 'month'
        else:
            level = 'year'
        
        # Store at appropriate level
        episode['temporal_level'] = level
        episode['timestamp'] = timestamp
        
        if level in self.hierarchy_levels:
            self.hierarchy_levels[level].append(episode)
            
            # Trim if exceeds capacity
            if len(self.hierarchy_levels[level]) > self.capacity // 5:
                self.hierarchy_levels[level] = self.hierarchy_levels[level][-self.capacity // 5:]
        
        # Also store in parent episodic memory
        super().store_episode(episode)
    
    def retrieve_with_temporal_abstraction(self, query: Dict, time_range: Tuple[float, float] = None) -> List[Dict]:
        """Retrieve episodes with temporal abstraction."""
        results = []
        
        # Determine query time range
        if time_range is None:
            # Use all available time range
            if self.episodes:
                timestamps = [ep['timestamp'] for ep in self.episodes if 'timestamp' in ep]
                if timestamps:
                    time_range = (min(timestamps), max(timestamps))
        
        # Retrieve from each hierarchical level
        for level, episodes in self.hierarchy_levels.items():
            for episode in episodes:
                if time_range is None or (episode['timestamp'] >= time_range[0] and 
                                        episode['timestamp'] <= time_range[1]):
                    # Filter based on query
                    if self._episode_matches_query(episode, query):
                        results.append(episode)
        
        return results
    
    def _episode_matches_query(self, episode: Dict, query: Dict) -> bool:
        """Check if episode matches query criteria."""
        # Simple matching based on episode content
        for key, value in query.items():
            if key in episode and episode[key] != value:
                return False
        
        return True
    
    def get_temporal_summary(self, start_time: float, end_time: float) -> Dict:
        """Get temporal summary of stored episodes."""
        summary = {
            'time_range': (start_time, end_time),
            'total_episodes': 0,
            'level_distribution': {},
            'event_categories': {},
            'keyword_frequencies': {}
        }
        
        # Count episodes in time range
        for level, episodes in self.hierarchy_levels.items():
            level_episodes = [ep for ep in episodes 
                           if ep['timestamp'] >= start_time and ep['timestamp'] <= end_time]
            summary['total_episodes'] += len(level_episodes)
            summary['level_distribution'][level] = len(level_episodes)
        
        # Analyze event categories
        for level, episodes in self.hierarchy_levels.items():
            for episode in episodes:
                if episode['timestamp'] >= start_time and episode['timestamp'] <= end_time:
                    # Categorize event
                    event_type = episode.get('type', 'unknown')
                    summary['event_categories'][event_type] = summary['event_categories'].get(event_type, 0) + 1
        
        return summary

# Use enhanced memory systems
class MemorySystemManager:
    def __init__(self, config: Dict):
        self.config = config
        self.working_memory = EnhancedWorkingMemory(
            capacity=config.get('memory', {}).get('working_memory', {}).get('working_memory_capacity', 100),
            compression_threshold=0.2
        )
        self.hierarchical_episodic = HierarchicalEpisodicMemory(
            capacity=config.get('memory', {}).get('episodic_memory', {}).get('episodic_memory_capacity', 1000)
        )
        self.semantic_memory = SemanticMemory(
            capacity=config.get('memory', {}).get('semantic_memory', {}).get('semantic_memory_capacity', 5000)
        )
    
    def process_experience(self, sensory_input: Dict, action: Dict, reward: float, timestamp: float):
        """Process experience through all memory systems."""
        # Store in working memory with compression
        self.working_memory.store_with_compression(
            'sensory_input', sensory_input, 
            importance=self.calculate_importance(sensory_input, reward)
        )
        
        # Store in episodic memory with hierarchy
        episode = {
            'sensory_input': sensory_input,
            'action': action,
            'reward': reward,
            'timestamp': timestamp
        }
        self.hierarchical_episodic.store_episode_with_hierarchy(episode, timestamp)
        
        # Extract and store semantic patterns
        self.extract_semantic_patterns(sensory_input, action, reward, timestamp)
    
    def calculate_importance(self, sensory_input: Dict, reward: float) -> float:
        """Calculate importance of experience."""
        importance = abs(reward)
        
        # Add novelty to importance
        if 'novelty' in sensory_input:
            importance += sensory_input['novelty'] * 0.5
        
        # Add temporal recency
        if sensory_input.get('timestamp'):
            recency = 1.0 - (sensory_input['timestamp'] % 3600) / 3600  # Decay over hour
            importance += recency * 0.3
        
        return min(importance, 1.0)
    
    def extract_semantic_patterns(self, sensory_input: Dict, action: Dict, reward: float, timestamp: float):
        """Extract semantic patterns from experience."""
        # Create semantic representations
        concepts = self.extract_concepts(sensory_input, action, reward)
        
        for concept_name, concept_data in concepts.items():
            # Calculate concept strength
            strength = self.calculate_concept_strength(concept_data, reward, timestamp)
            
            # Store in semantic memory
            self.semantic_memory.store_concept(
                concept_name, 
                {
                    **concept_data,
                    'strength': strength,
                    'last_mentioned': timestamp
                },
                importance=strength
            )
    
    def extract_concepts(self, sensory_input: Dict, action: Dict, reward: float) -> Dict:
        """Extract concepts from experience."""
        concepts = {}
        
        # Extract spatial concepts
        if 'position' in sensory_input:
            concepts['location'] = {
                'coordinates': sensory_input['position'],
                'proximity_to_goal': self.calculate_proximity_to_goal(sensory_input['position'])
            }
        
        # Extract action concepts
        if 'action' in sensory_input:
            concepts['environment'] = {
                'type': sensory_input['action'],
                'outcome': reward,
                'efficiency': self.calculate_action_efficiency(action, reward)
            }
        
        # Extract reward concepts
        if reward > 0:
            concepts['positive_outcome'] = {
                'type': 'success',
                'magnitude': reward,
                'context': self.get_reward_context(reward)
            }
        elif reward < 0:
            concepts['negative_outcome'] = {
                'type': 'failure',
                'magnitude': abs(reward),
                'context': self.get_reward_context(reward)
            }
        
        return concepts
    
    def calculate_proximity_to_goal(self, position: Tuple[float, float]) -> float:
        """Calculate proximity to goal position."""
        goal_position = (5.0, 5.0)  # Default goal
        return 1.0 - np.sqrt((position[0] - goal_position[0])**2 + 
                            (position[1] - goal_position[1])**2) / 10.0
    
    def calculate_action_efficiency(self, action: Dict, reward: float) -> float:
        """Calculate efficiency of action."""
        base_efficiency = 0.5
        
        # Adjust based on reward
        if reward > 0:
            base_efficiency += reward * 0.3
        elif reward < 0:
            base_efficiency -= abs(reward) * 0.2
        
        return max(0.0, min(1.0, base_efficiency))
    
    def get_reward_context(self, reward: float) -> Dict:
        """Get context for reward."""
        return {
            'reward_magnitude': reward,
            'difficulty': 'easy' if abs(reward) < 0.5 else 'hard',
            'type': 'positive' if reward > 0 else 'negative'
        }
    
    def calculate_concept_strength(self, concept_data: Dict, reward: float, timestamp: float) -> float:
        """Calculate strength of concept."""
        strength = 0.5
        
        # Add reward contribution
        strength += abs(reward) * 0.3
        
        # Add recency contribution
        recency = 1.0 - (timestamp % 3600) / 3600
        strength += recency * 0.2
        
        return min(strength, 1.0)
    
    def retrieve_memories(self, query: Dict) -> Dict:
        """Retrieve memories based on query."""
        # Retrieve from working memory
        wm_results = self.working_memory.retrieve(query.get('working_memory_key'))
        
        # Retrieve from episodic memory
        em_results = self.hierarchical_episodic.retrieve_with_temporal_abstraction(query)
        
        # Retrieve from semantic memory
        sm_results = self.semantic_memory.search(query)
        
        return {
            'working_memory': wm_results,
            'episodic_memory': em_results,
            'semantic_memory': sm_results,
            'attention_summary': self.working_memory.get_attention_summary()
        }

# Use the advanced memory systems
config = modify_config({
    "memory": {
        "working_memory_enabled": True,
        "working_memory_capacity": 500,
        "episodic_memory_enabled": True,
        "episodic_memory_capacity": 2000,
        "semantic_memory_enabled": True,
        "semantic_memory_capacity": 10000
    }
})

memory_manager = MemorySystemManager(config)
```

---

## Neuromodulation Deep Dive

### Advanced Neuromodulation Systems

Implement sophisticated neuromodulation with multiple interacting systems:

```python
import numpy as np
from nlm.neuromodulation import DopamineSystem, CuriositySystem, NoveltyDetector

class AdvancedNeuromodulation:
    """Advanced neuromodulation with interacting systems."""
    
    def __init__(self, config: Dict):
        self.config = config
        self.dopamine = DopamineSystem(
            weight=config.get('dopamine', {}).get('weight', 1.0),
            learning_rate=config.get('dopamine', {}).get('learning_rate', 0.01),
            adaptation_rate=config.get('dopamine', {}).get('adaptation_rate', 0.001),
            prediction_error_weight=config.get('dopamine', {}).get('prediction_error_weight', 1.0)
        )
        
        self.curiosity = CuriositySystem(
            weight=config.get('curiosity', {}).get('weight', 0.5),
            novelty_threshold=config.get('curiosity', {}).get('novelty_threshold', 0.3),
            history_size=config.get('curiosity', {}).get('history_size', 100),
            exploration_drive=config.get('curiosity', {}).get('exploration_drive', 0.8)
        )
        
        self.novelty = NoveltyDetector(
            sensitivity=config.get('novelty', {}).get('sensitivity', 0.7),
            history_size=config.get('novelty', {}).get('history_size', 100),
            change_detection=config.get('novelty', {}).get('change_detection', True)
        )
        
        self.prediction_error = PredictionErrorSystem(
            weight=config.get('prediction_error', {}).get('weight', 1.0),
            precision=config.get('prediction_error', {}).get('precision', 1.0),
            prediction_horizon=config.get('prediction_error', {}).get('prediction_horizon', 10)
        )
        
        # Interaction weights
        self.interaction_weights = {
            'dopamine_curiosity': config.get('interactions', {}).get('dopamine_curiosity', 0.3),
            'curiosity_novelty': config.get('interactions', {}).get('curiosity_novelty', 0.4),
            'novelty_dopamine': config.get('interactions', {}).get('novelty_dopamine', 0.2),
            'prediction_error_dopamine': config.get('interactions', {}).get('prediction_error_dopamine', 0.5)
        }
        
        # State variables
        self.dopamine_history = []
        self.curiosity_history = []
        self.novelty_history = []
        self.prediction_error_history = []
        self.basedness_vector = np.zeros(10)  # Working memory representation
    
    def process_experience_with_neuromodulation(self, sensory_input: Dict, action: Dict, reward: float, timestamp: float):
        """Process experience with full neuromodulation."""
        # Update working memory representation
        self.update_basedness_vector(sensory_input, action, reward)
        
        # Calculate prediction error
        prediction_error = self.calculate_prediction_error(sensory_input, action)
        
        # Calculate novelty
        novelty_score = self.novelty.calculate_novelty(sensory_input)
        
        # Calculate curiosity
        curiosity_signal = self.curiosity.calculate_curiosity(prediction_error, novelty_score)
        
        # Calculate dopamine
        dopamine_signal = self.dopamine.calculate_dopamine(reward, curiosity_signal)
        
        # Calculate prediction error signal
        prediction_error_signal = self.prediction_error.calculate_error(
            prediction_error, reward, timestamp
        )
        
        # Apply interactions
        final_dopamine = self.apply_dopamine_interactions(dopamine_signal, curiosity_signal, prediction_error_signal)
        
        # Update neuromodulation systems
        self.dopamine.update_state(final_dopamine, reward, curiosity_signal)
        self.curiosity.update_state(curiosity_signal, novelty_score, prediction_error)
        self.novelty.update_state(sensory_input)
        self.prediction_error.update_state(prediction_error_signal, reward)
        
        # Record history
        self.dopamine_history.append({
            'timestamp': timestamp,
            'dopamine': final_dopamine,
            'reward': reward,
            'curiosity': curiosity_signal,
            'prediction_error': prediction_error_signal
        })
        
        self.curiosity_history.append({
            'timestamp': timestamp,
            'curiosity': curiosity_signal,
            'novelty': novelty_score,
            'prediction_error': prediction_error
        })
        
        self.novelty_history.append({
            'timestamp': timestamp,
            'novelty': novelty_score,
            'sensory_input': sensory_input
        })
        
        self.prediction_error_history.append({
            'timestamp': timestamp,
            'prediction_error': prediction_error_signal,
            'reward': reward,
            'basedness_vector': self.basedness_vector.copy()
        })
        
        return {
            'dopamine': final_dopamine,
            'curiosity': curiosity_signal,
            'novelty': novelty_score,
            'prediction_error': prediction_error_signal
        }
    
    def update_basedness_vector(self, sensory_input: Dict, action: Dict, reward: float):
        """Update working memory representation (basedness vector)."""
        # Extract features from sensory input
        features = self.extract_features(sensory_input)
        
        # Update basedness vector using Hebbian learning
        for i, feature in enumerate(features):
            # Normalize feature
            normalized_feature = feature / (np.linalg.norm(features) + 1e-8)
            
            # Update basedness vector
            self.basedness_vector[i] += normalized_feature * reward * 0.01
            
            # Apply decay
            self.basedness_vector[i] *= 0.99
        
        # Normalize
        self.basedness_vector /= (np.linalg.norm(self.basedness_vector) + 1e-8)
    
    def calculate_prediction_error(self, sensory_input: Dict, action: Dict) -> float:
        """Calculate prediction error for sensory input and action."""
        # Get current prediction
        prediction = self.get_prediction(sensory_input, action)
        
        # Get actual input
        actual_input = self.extract_features(sensory_input)
        
        # Calculate error
        if prediction is not None:
            error = np.linalg.norm(np.array(actual_input) - np.array(prediction))
        else:
            error = np.linalg.norm(np.array(actual_input))
        
        return min(error, 1.0)  # Normalize to 0-1
    
    def get_prediction(self, sensory_input: Dict, action: Dict) -> Optional[List[float]]:
        """Get prediction for sensory input and action."""
        # Simple prediction based on working memory
        # In real implementation, this would use neural network predictions
        
        # Check if we have similar experiences in history
        similar_experiences = []
        
        for history_entry in self.dopamine_history:
            # Simple similarity check
            if abs(history_entry['reward'] - (sensory_input.get('expected_reward', 0))) < 0.1:
                similar_experiences.append(history_entry)
        
        if similar_experiences:
            # Average prediction from similar experiences
            avg_prediction = np.mean([exp['basedness_vector'] for exp in similar_experiences], axis=0)
            return avg_prediction.tolist()
        
        return None
    
    def extract_features(self, sensory_input: Dict) -> List[float]:
        """Extract features from sensory input."""
        features = []
        
        # Extract position features
        if 'position' in sensory_input:
            pos = sensory_input['position']
            features.extend([pos[0] / 10.0, pos[1] / 10.0])  # Normalize
        
        # Extract action features
        if 'action' in sensory_input:
            action_type = sensory_input['action']
            action_features = {'move': [1, 0, 0], 'explore': [0, 1, 0], 'interact': [0, 0, 1]}
            features.extend(action_features.get(action_type, [0, 0, 0]))
        
        # Extract sensory features
        sensory_features = {
            'color': {'red': [1, 0, 0], 'green': [0, 1, 0], 'blue': [0, 0, 1]},
            'sound': {'silence': [1, 0], 'noise': [0, 1]},
            'texture': {'smooth': [1, 0], 'rough': [0, 1]}
        }
        
        for modality, feature_map in sensory_features.items():
            if modality in sensory_input:
                features.extend(feature_map.get(sensory_input[modality], [0] * len(next(iter(feature_map.values())))))
        
        # Add random feature for exploration
        features.append(np.random.random())
        
        return features
    
    def apply_dopamine_interactions(self, dopamine: float, curiosity: float, 
                                   prediction_error: float) -> float:
        """Apply interactions between neuromodulators."""
        # Calculate base dopamine
        result = dopamine
        
        # Apply curiosity modulation
        if self.interaction_weights['dopamine_curiosity'] > 0:
            result += self.interaction_weights['dopamine_curiosity'] * curiosity * 0.5
        
        # Apply novelty modulation
        if self.interaction_weights['curiosity_novelty'] > 0:
            result += self.interaction_weights['curiosity_novelty'] * prediction_error * 0.3
        
        # Apply prediction error modulation
        if self.interaction_weights['prediction_error_dopamine'] > 0:
            result += self.interaction_weights['prediction_error_dopamine'] * prediction_error * 0.4
        
        # Apply novelty modulation to dopamine
        if self.interaction_weights['novelty_dopamine'] > 0:
            result += self.interaction_weights['novelty_dopamine'] * self.novelty.calculate_novelty(
                {'novelty_feature': result}
            ) * 0.2
        
        # Clip to reasonable range
        return max(-1.0, min(1.0, result))
    
    def get_neuromodulation_summary(self) -> Dict:
        """Get summary of neuromodulation activity."""
        if not self.dopamine_history:
            return {'status': 'no_data', 'history_length': 0}
        
        latest = self.dopamine_history[-1]
        
        return {
            'status': 'active',
            'history_length': len(self.dopamine_history),
            'latest_values': {
                'dopamine': latest['dopamine'],
                'curiosity': latest['curiosity'],
                'prediction_error': latest['prediction_error']
            },
            'activity_levels': {
                'dopamine_mean': np.mean([h['dopamine'] for h in self.dopamine_history]),
                'curiosity_mean': np.mean([h['curiosity'] for h in self.curiosity_history]),
                'prediction_error_mean': np.mean([h['prediction_error'] for h in self.prediction_error_history])
            },
            'interaction_patterns': self.analyze_interaction_patterns()
        }
    
    def analyze_interaction_patterns(self) -> Dict:
        """Analyze interaction patterns between neuromodulators."""
        patterns = {}
        
        if len(self.dopamine_history) < 2:
            return patterns
        
        # Analyze dopamine-curiosity relationship
        dopamine_vals = [h['dopamine'] for h in self.dopamine_history]
        curiosity_vals = [h['curiosity'] for h in self.curiosity_history]
        
        # Calculate correlation
        if len(dopamine_vals) == len(curiosity_vals):
            correlation = np.corrcoef(dopamine_vals, curiosity_vals)[0, 1]
            patterns['dopamine_curiosity_correlation'] = correlation
        
        # Analyze dopamine-prediction error relationship
        prediction_error_vals = [h['prediction_error'] for h in self.prediction_error_history]
        
        if len(dopamine_vals) == len(prediction_error_vals):
            correlation = np.corrcoef(dopamine_vals, prediction_error_vals)[0, 1]
            patterns['dopamine_prediction_error_correlation'] = correlation
        
        # Analyze novelty-dopamine relationship
        novelty_vals = [h['novelty'] for h in self.novelty_history]
        
        if len(dopamine_vals) == len(novelty_vals):
            correlation = np.corrcoef(dopamine_vals, novelty_vals)[0, 1]
            patterns['dopamine_novelty_correlation'] = correlation
        
        return patterns

# Use advanced neuromodulation
config = modify_config({
    "neuromodulation": {
        "dopamine_enabled": True,
        "curiosity_enabled": True,
        "novelty_enabled": True,
        "prediction_error_enabled": True,
        "interactions": {
            "dopamine_curiosity": 0.3,
            "curiosity_novelty": 0.4,
            "novelty_dopamine": 0.2,
            "prediction_error_dopamine": 0.5
        }
    }
})

advanced_neuromod = AdvancedNeuromodulation(config)
```

---

## Complex Learning Scenarios

### Multi-Task Learning

Implement agents that can learn multiple tasks simultaneously:

```python
import numpy as np
from nlm import NeuralLearningMachine

class MultiTaskLearningAgent:
    """Agent that learns multiple tasks simultaneously."""
    
    def __init__(self, config: Dict):
        self.config = config
        self.nlm = NeuralLearningMachine(config)
        
        # Define multiple tasks
        self.tasks = {
            'navigation': {
                'goal': (5.0, 5.0),
                'reward_function': self.calculate_navigation_reward,
                'success_threshold': 1.0
            },
            'exploration': {
                'reward_function': self.calculate_exploration_reward,
                'success_threshold': 0.8
            },
            'memory': {
                'reward_function': self.calculate_memory_reward,
                'success_threshold': 0.9
            }
        }
        
        # Task performance tracking
        self.task_performance = {task: {'successes': 0, 'failures': 0, 'total': 0} 
                                for task in self.tasks}
        self.task_weights = {task: 1.0 for task in self.tasks}  # Task importance weights
    
    def learn_from_experience(self, sensory_input: Dict, action: Dict, reward: float, 
                            task_id: str, timestamp: float):
        """Learn from experience for specific task."""
        # Calculate task-specific reward
        task = self.tasks.get(task_id)
        if task:
            task_reward = task['reward_function'](sensory_input, action, reward)
            
            # Update task performance
            self.task_performance[task_id]['total'] += 1
            if task_reward >= task['success_threshold']:
                self.task_performance[task_id]['successes'] += 1
            else:
                self.task_performance[task_id]['failures'] += 1
            
            # Adjust task weights based on performance
            success_rate = self.task_performance[task_id]['successes'] / max(1, 
                                 self.task_performance[task_id]['total'])
            self.task_weights[task_id] = success_rate  # Higher success = higher weight
            
            # Learn with task-weighted reward
            weighted_reward = task_reward * self.task_weights[task_id]
        else:
            weighted_reward = reward
        
        # Learn with weighted reward
        self.nlm.process_experience(sensory_input, action, weighted_reward)
        
        # Store experience in task-specific memory
        self.store_task_experience(task_id, sensory_input, action, weighted_reward, timestamp)
    
    def calculate_navigation_reward(self, sensory_input: Dict, action: Dict, reward: float) -> float:
        """Calculate navigation task reward."""
        # Navigation reward based on proximity to goal
        goal = self.tasks['navigation']['goal']
        distance = np.sqrt((sensory_input['position'][0] - goal[0])**2 + 
                          (sensory_input['position'][1] - goal[1])**2)
        
        # Normalize distance to [0, 1] range
        normalized_distance = max(0, 1 - distance / 10.0)
        
        # Combine with base reward
        return 0.7 * normalized_distance + 0.3 * reward
    
    def calculate_exploration_reward(self, sensory_input: Dict, action: Dict, reward: float) -> float:
        """Calculate exploration task reward."""
        # Exploration reward based on curiosity and novelty
        curiosity_reward = sensory_input.get('curiosity', 0.0)
        novelty_reward = sensory_input.get('novelty', 0.0)
        
        # Reward for discovering new things
        exploration_bonus = 0.3 if curiosity_reward > 0.7 else 0.0
        
        return 0.4 * curiosity_reward + 0.4 * novelty_reward + 0.2 * exploration_bonus
    
    def calculate_memory_reward(self, sensory_input: Dict, action: Dict, reward: float) -> float:
        """Calculate memory task reward."""
        # Memory reward based on retention and recall
        retention_reward = sensory_input.get('retention_strength', 0.0)
        recall_reward = sensory_input.get('recall_success', 0.0)
        
        # Reward for good memory performance
        memory_bonus = 0.5 if retention_reward > 0.8 and recall_reward > 0.8 else 0.0
        
        return 0.5 * retention_reward + 0.5 * memory_bonus
    
    def store_task_experience(self, task_id: str, sensory_input: Dict, action: Dict, 
                             reward: float, timestamp: float):
        """Store experience in task-specific memory."""
        experience = {
            'task_id': task_id,
            'sensory_input': sensory_input,
            'action': action,
            'reward': reward,
            'timestamp': timestamp,
            'task_performance': self.task_performance[task_id]
        }
        
        # Store in task-specific memory
        if not hasattr(self, 'task_memories'):
            self.task_memories = {}
        
        if task_id not in self.task_memories:
            self.task_memories[task_id] = []
        
        self.task_memories[task_id].append(experience)
        
        # Trim memory if too large
        if len(self.task_memories[task_id]) > 1000:
            self.task_memories[task_id] = self.task_memories[task_id][-1000:]
    
    def get_task_performance_summary(self) -> Dict:
        """Get summary of task performance."""
        summary = {}
        
        for task_id, performance in self.task_performance.items():
            total = performance['total']
            if total > 0:
                success_rate = performance['successes'] / total
                summary[task_id] = {
                    'success_rate': success_rate,
                    'total_experiences': total,
                    'current_weight': self.task_weights[task_id]
                }
        
        return summary
    
    def get_balanced_reward(self, sensory_input: Dict, action: Dict, reward: float, 
                          task_id: str) -> float:
        """Get reward that balances multiple task objectives."""
        # Get individual task rewards
        task_rewards = {}
        for task_name, task_config in self.tasks.items():
            task_rewards[task_name] = task_config['reward_function'](sensory_input, action, reward)
        
        # Compute weighted average based on task importance and performance
        total_weight = sum(self.task_weights.values())
        
        if total_weight > 0:
            balanced_reward = sum(
                self.task_weights[task] * reward 
                for task, reward in task_rewards.items()
            ) / total_weight
        else:
            balanced_reward = reward
        
        return balanced_reward

# Use multi-task learning
config = modify_config({
    "memory": {
        "working_memory_enabled": True,
        "working_memory_capacity": 1000,
        "episodic_memory_enabled": True,
        "episodic_memory_capacity": 5000
    }
})

multi_task_agent = MultiTaskLearningAgent(config)
```

### Transfer Learning

Implement transfer learning between tasks:

```python
class TransferLearningAgent:
    """Agent that uses transfer learning between tasks."""
    
    def __init__(self, config: Dict):
        self.config = config
        self.nlm = NeuralLearningMachine(config)
        
        # Source tasks for transfer
        self.source_tasks = ['navigation', 'exploration', 'memory']
        
        # Target task
        self.target_task = 'complex_navigation'
        
        # Transfer knowledge
        self.transfer_knowledge = {}
        self.adaptation_weights = {}
        
        # Initialize transfer knowledge
        self.initialize_transfer_knowledge()
    
    def initialize_transfer_knowledge(self):
        """Initialize transfer knowledge from source tasks."""
        for source_task in self.source_tasks:
            # Extract knowledge from source task
            knowledge = self.extract_knowledge_from_task(source_task)
            
            # Store as transferable knowledge
            self.transfer_knowledge[source_task] = knowledge
            
            # Initialize adaptation weight
            self.adaptation_weights[source_task] = 0.5  # Start with moderate confidence
    
    def extract_knowledge_from_task(self, task_id: str) -> Dict:
        """Extract knowledge from a source task."""
        # This would extract learned parameters, strategies, etc.
        # For demonstration, we'll create a simple knowledge representation
        
        knowledge = {
            'strategies': self.get_task_strategies(task_id),
            'optimal_actions': self.get_optimal_actions(task_id),
            'environmental_understanding': self.get_environmental_understanding(task_id),
            'reward_functions': self.get_task_reward_function(task_id)
        }
        
        return knowledge
    
    def get_task_strategies(self, task_id: str) -> List[Dict]:
        """Get strategies learned for a task."""
        # Simulate retrieval of learned strategies
        return [
            {
                'name': f'strategy_{task_id}_1',
                'success_rate': 0.8,
                'complexity': 0.6,
                'adaptability': 0.7
            },
            {
                'name': f'strategy_{task_id}_2',
                'success_rate': 0.7,
                'complexity': 0.4,
                'adaptability': 0.9
            }
        ]
    
    def get_optimal_actions(self, task_id: str) -> List[Dict]:
        """Get optimal actions for a task."""
        # Simulate retrieval of optimal actions
        return [
            {'action': 'move_toward_goal', 'expected_reward': 0.9, 'risk': 0.2},
            {'action': 'explore_new_area', 'expected_reward': 0.7, 'risk': 0.5},
            {'action': 'avoid_obstacle', 'expected_reward': 0.8, 'risk': 0.3}
        ]
    
    def get_environmental_understanding(self, task_id: str) -> Dict:
        """Get environmental understanding from a task."""
        # Simulate environmental model
        return {
            'layout': 'grid_10x10',
            'obstacle_density': 0.3,
            'goal_positions': [(5.0, 5.0), (7.5, 2.5)],
            'reward_distribution': 'normal',
            'risk_zones': [(1.0, 1.0), (8.0, 8.0)]
        }
    
    def get_task_reward_function(self, task_id: str) -> Dict:
        """Get reward function for a task."""
        # Return reward function parameters
        return {
            'base_reward': 1.0,
            'goal_bonus': 2.0,
            'exploration_bonus': 1.5,
            'complexity_penalty': 0.5
        }
    
    def adapt_for_target_task(self, target_context: Dict) -> Dict:
        """Adapt source knowledge for target task."""
        adapted_knowledge = {}
        
        for source_task, knowledge in self.transfer_knowledge.items():
            # Calculate adaptation weight based on task similarity
            similarity = self.calculate_task_similarity(
                source_task, self.target_task, target_context
            )
            
            # Adjust adaptation weight based on confidence
            confidence = self.get_task_confidence(source_task)
            
            # Apply adaptation
            adaptation_weight = similarity * confidence
            self.adaptation_weights[source_task] = adaptation_weight
            
            # Adapt knowledge based on weight
            adapted_knowledge[source_task] = self.adapt_knowledge(
                knowledge, target_context, adaptation_weight
            )
        
        return adapted_knowledge
    
    def calculate_task_similarity(self, source_task: str, target_task: str, 
                                target_context: Dict) -> float:
        """Calculate similarity between source and target tasks."""
        # Simple similarity based on shared characteristics
        similarity_factors = {
            'environmental': 0.7,
            'action_space': 0.8,
            'reward_structure': 0.6,
            'cognitive_demand': 0.5
        }
        
        # Calculate average similarity
        return sum(similarity_factors.values()) / len(similarity_factors)
    
    def get_task_confidence(self, task_id: str) -> float:
        """Get confidence in source task knowledge."""
        # Base confidence on task performance
        performance = self.task_performance.get(task_id, {})
        success_rate = performance.get('successes', 0) / max(1, performance.get('total', 1))
        
        # Add some noise to make it realistic
        return max(0.1, min(1.0, success_rate + np.random.normal(0, 0.1)))
    
    def adapt_knowledge(self, knowledge: Dict, target_context: Dict, weight: float) -> Dict:
        """Adapt knowledge from source to target."""
        adapted = knowledge.copy()
        
        # Adapt strategies
        adapted['strategies'] = [
            {
                **strategy,
                'success_rate': strategy['success_rate'] * weight + 0.5 * (1 - weight),
                'adaptability': strategy['adaptability'] * weight + 0.7 * (1 - weight)
            }
            for strategy in knowledge['strategies']
        ]
        
        # Adapt optimal actions
        adapted['optimal_actions'] = [
            {
                **action,
                'expected_reward': action['expected_reward'] * weight + 0.6 * (1 - weight),
                'risk': action['risk'] * (1 - weight) + 0.3 * weight
            }
            for action in knowledge['optimal_actions']
        ]
        
        # Adapt environmental understanding
        adapted['environmental_understanding'] = {
            **knowledge['environmental_understanding'],
            'obstacle_density': min(1.0, knowledge['environmental_understanding']['obstacle_density'] * weight + 
                                    target_context.get('obstacle_density', 0.5) * (1 - weight)),
            'goal_positions': target_context.get('goal_positions', 
                                               knowledge['environmental_understanding']['goal_positions'])
        }
        
        return adapted
    
    def learn_with_transfer(self, sensory_input: Dict, action: Dict, reward: float, 
                          task_id: str = None, timestamp: float = None):
        """Learn with transfer learning."""
        if task_id is None:
            task_id = self.target_task
        
        # Get target context
        target_context = self.get_target_context(task_id)
        
        # Adapt knowledge for target task
        adapted_knowledge = self.adapt_for_target_task(target_context)
        
        # Apply adapted knowledge to learning
        self.apply_adapted_knowledge(adapted_knowledge, sensory_input, action, reward)
        
        # Store experience
        if timestamp is None:
            timestamp = time.time()
        
        self.store_experience_with_transfer(
            sensory_input, action, reward, task_id, timestamp, adapted_knowledge
        )
        
        # Update NLM
        self.nlm.process_experience(sensory_input, action, reward)
    
    def get_target_context(self, task_id: str) -> Dict:
        """Get context for target task."""
        # Get target task configuration
        target_task_config = self.tasks.get(task_id, {})
        
        # Combine with general configuration
        return {
            'obstacle_density': target_task_config.get('environment', {}).get('obstacle_density', 0.3),
            'goal_positions': target_task_config.get('environment', {}).get('goal_positions', [(5.0, 5.0)]),
            'reward_function': target_task_config.get('reward_function', {}),
            'action_space': target_task_config.get('action_space', ['move', 'explore', 'interact'])
        }
    
    def apply_adapted_knowledge(self, adapted_knowledge: Dict, sensory_input: Dict, 
                              action: Dict, reward: float):
        """Apply adapted knowledge to learning."""
        # This would apply the adapted strategies and knowledge
        # For demonstration, we'll modify the reward based on adapted knowledge
        
        # Calculate knowledge-based reward adjustment
        knowledge_adjustment = 0.0
        
        for source_task, knowledge in adapted_knowledge.items():
            weight = self.adaptation_weights[source_task]
            
            # Apply strategy adjustments
            for strategy in knowledge['strategies']:
                if strategy['success_rate'] > 0.8:  # High-success strategy
                    knowledge_adjustment += weight * 0.1
        
        # Apply adjustment to reward
        adjusted_reward = reward + knowledge_adjustment
        
        # Store adjusted reward for learning
        self.last_adjusted_reward = adjusted_reward
    
    def store_experience_with_transfer(self, sensory_input: Dict, action: Dict, reward: float,
                                     task_id: str, timestamp: float, adapted_knowledge: Dict):
        """Store experience with transfer learning context."""
        experience = {
            'sensory_input': sensory_input,
            'action': action,
            'reward': reward,
            'adjusted_reward': getattr(self, 'last_adjusted_reward', reward),
            'task_id': task_id,
            'timestamp': timestamp,
            'adapted_knowledge': adapted_knowledge,
            'transfer_weights': self.adaptation_weights.copy()
        }
        
        if not hasattr(self, 'transfer_experiences'):
            self.transfer_experiences = {}
        
        if task_id not in self.transfer_experiences:
            self.transfer_experiences[task_id] = []
        
        self.transfer_experiences[task_id].append(experience)

# Use transfer learning
config = modify_config({
    "memory": {
        "working_memory_enabled": True,
        "working_memory_capacity": 1000,
        "episodic_memory_enabled": True,
        "episodic_memory_capacity": 5000
    }
})

transfer_agent = TransferLearningAgent(config)
```

---

## Performance Optimization

### Parallel Processing

Implement parallel processing for faster simulations:

```python
import concurrent.futures
import threading
from multiprocessing import Pool
import time

class OptimizedNLM(NeuralLearningMachine):
    """Optimized NLM with parallel processing."""
    
    def __init__(self, config: Dict = None, use_parallel: bool = True):
        super().__init__(config)
        self.use_parallel = use_parallel
        self.lock = threading.Lock()
        self.process_pool = None
    
    def __enter__(self):
        """Context manager setup."""
        if self.use_parallel and self.config.get('performance', {}).get('enable_optimization', True):
            num_processes = self.config.get('performance', {}).get('parallel_neuron_count', 4)
            self.process_pool = Pool(processes=num_processes)
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager cleanup."""
        if self.process_pool:
            self.process_pool.close()
            self.process_pool.join()
            self.process_pool = None
    
    def process_experience_parallel(self, experiences: List[Dict]) -> List[Dict]:
        """Process multiple experiences in parallel."""
        if not self.use_parallel or len(experiences) <= 1:
            # Sequential processing
            results = []
            for experience in experiences:
                result = self.process_experience(
                    experience['sensory_input'],
                    experience['action'],
                    experience['reward']
                )
                results.append({
                    **experience,
                    'result': result
                })
            return results
        
        # Parallel processing
        def process_single(experience):
            return self.process_experience(
                experience['sensory_input'],
                experience['action'],
                experience['reward']
            )
        
        # Submit tasks to process pool
        with self.process_pool as pool:
            results = pool.map(process_single, experiences)
        
        # Combine results with original experiences
        return [
            {
                **experiences[i],
                'result': results[i]
            }
            for i in range(len(experiences))
        ]
    
    def simulate_batch_parallel(self, num_steps: int, num_agents: int = 10) -> Dict:
        """Run simulation with multiple parallel agents."""
        print(f"Running {num_steps}-step simulation with {num_agents} parallel agents...")
        
        start_time = time.time()
        
        # Create list of experiences for all agents
        all_experiences = []
        
        for agent_id in range(num_agents):
            for step in range(num_steps):
                # Generate experience for this agent
                experience = self.generate_experience_for_agent(agent_id, step)
                all_experiences.append(experience)
        
        # Process experiences in parallel
        processed = self.process_experience_parallel(all_experiences)
        
        # Aggregate results
        results = self.aggregate_parallel_results(processed)
        
        end_time = time.time()
        
        return {
            'results': results,
            'total_time': end_time - start_time,
            'agents': num_agents,
            'steps_per_agent': num_steps,
            'total_experiences': len(all_experiences),
            'efficiency': len(all_experiences) / (end_time - start_time)
        }
    
    def generate_experience_for_agent(self, agent_id: int, step: int) -> Dict:
        """Generate experience for a specific agent."""
        # Create unique agent seed
        np.random.seed(agent_id * 1000 + step)
        random.seed(agent_id * 1000 + step)
        
        # Generate random experience
        sensory_input = {
            'agent_id': agent_id,
            'step': step,
            'position': (np.random.uniform(0, 10), np.random.uniform(0, 10)),
            'color': random.choice(['red', 'green', 'blue', 'yellow']),
            'sound': random.choice(['silence', 'noise', 'tone']),
            'time': step * 0.1
        }
        
        action = {
            'type': random.choice(['move', 'explore', 'interact']),
            'direction': random.choice(['north', 'south', 'east', 'west']),
            'strength': np.random.uniform(0.5, 1.0)
        }
        
        # Calculate reward based on distance to goal
        goal = (5.0, 5.0)
        distance = np.sqrt((sensory_input['position'][0] - goal[0])**2 + 
                          (sensory_input['position'][1] - goal[1])**2)
        reward = max(0, 10.0 - distance) / 10.0
        
        return {
            'sensory_input': sensory_input,
            'action': action,
            'reward': reward
        }
    
    def aggregate_parallel_results(self, processed_experiences: List[Dict]) -> Dict:
        """Aggregate results from parallel processing."""
        aggregated = {
            'total_rewards': 0,
            'total_curiosity': 0,
            'experience_count': len(processed_experiences),
            'agent_distributions': {},
            'step_distributions': {}
        }
        
        # Aggregate by agent
        for experience in processed_experiences:
            agent_id = experience['sensory_input']['agent_id']
            
            if agent_id not in aggregated['agent_distributions']:
                aggregated['agent_distributions'][agent_id] = {
                    'rewards': 0,
                    'curiosity': 0,
                    'count': 0
                }
            
            aggregated['agent_distributions'][agent_id]['rewards'] += experience['reward']
            aggregated['agent_distributions'][agent_id]['curiosity'] += experience.get('curiosity', 0.0)
            aggregated['agent_distributions'][agent_id]['count'] += 1
            
            aggregated['total_rewards'] += experience['reward']
            aggregated['total_curiosity'] += experience.get('curiosity', 0.0)
        
        # Aggregate by step
        for experience in processed_experiences:
            step = experience['sensory_input']['step']
            
            if step not in aggregated['step_distributions']:
                aggregated['step_distributions'][step] = {
                    'rewards': 0,
                    'count': 0
                }
            
            aggregated['step_distributions'][step]['rewards'] += experience['reward']
            aggregated['step_distributions'][step]['count'] += 1
        
        # Calculate averages
        aggregated['average_reward'] = aggregated['total_rewards'] / max(1, aggregated['experience_count'])
        aggregated['average_curiosity'] = aggregated['total_curiosity'] / max(1, aggregated['experience_count'])
        
        return aggregated
    
    def optimize_memory_usage(self):
        """Optimize memory usage through compression and caching."""
        if not hasattr(self, 'memory_optimizations'):
            self.memory_optimizations = {}
        
        # Optimize working memory
        if hasattr(self, 'working_memory') and self.working_memory:
            self.memory_optimizations['working_memory'] = {
                'original_capacity': self.working_memory.capacity,
                'current_usage': self.working_memory.size(),
                'compression_ratio': self.calculate_memory_compression_ratio('working_memory')
            }
        
        # Optimize episodic memory
        if hasattr(self, 'episodic_memory') and self.episodic_memory:
            self.memory_optimizations['episodic_memory'] = {
                'original_capacity': self.episodic_memory.capacity,
                'current_usage': self.episodic_memory.size(),
                'compression_ratio': self.calculate_memory_compression_ratio('episodic_memory')
            }
        
        # Generate optimization report
        return self.generate_memory_optimization_report()
    
    def calculate_memory_compression_ratio(self, memory_type: str) -> float:
        """Calculate memory compression ratio."""
        optimizations = self.memory_optimizations.get(memory_type, {})
        
        if optimizations:
            return optimizations['current_usage'] / optimizations['original_capacity']
        
        return 1.0
    
    def generate_memory_optimization_report(self) -> Dict:
        """Generate memory optimization report."""
        report = {
            'total_optimized_memory': 0,
            'optimization_improvements': {},
            'recommended_optimizations': []
        }
        
        # Calculate total optimized memory
        for memory_type, optimizations in self.memory_optimizations.items():
            report['total_optimized_memory'] += optimizations.get('current_usage', 0)
            
            # Calculate improvement
            original = optimizations.get('original_capacity', 0)
            current = optimizations.get('current_usage', 0)
            
            if original > 0:
                improvement = (original - current) / original * 100
                report['optimization_improvements'][memory_type] = improvement
        
        # Generate recommendations
        if report['total_optimized_memory'] > 1000:
            report['recommended_optimizations'].append(
                'Enable memory compression for working memory'
            )
        
        if report['optimization_improvements'].get('episodic_memory', 0) < 50:
            report['recommended_optimizations'].append(
                'Implement episodic memory compression'
            )
        
        return report

# Use optimized NLM
config = modify_config({
    "performance": {
        "use_memory_pool": True,
        "use_event_queue": True,
        "use_simd": True,
        "parallel_neuron_count": 4,
        "enable_optimization": True,
        "optimization_level": "balanced"
    }
})

# Run optimized simulation
with OptimizedNLM(config, use_parallel=True) as optimized_nlm:
    results = optimized_nlm.simulate_batch_parallel(
        num_steps=100,
        num_agents=10
    )
    
    print(f"Optimized simulation results: {results}")
    
    # Get memory optimization report
    memory_report = optimized_nlm.optimize_memory_usage()
    print(f"Memory optimization report: {memory_report}")
```

---

## Debugging and Troubleshooting

### Common Issues and Solutions

#### Issue 1: Memory Exhaustion

**Problem**: Memory usage grows too high, causing crashes or performance degradation.

**Solution**: Implement memory management strategies:

```python
class MemoryDebuggedNLM(NeuralLearningMachine):
    def __init__(self, config: Dict = None):
        super().__init__(config)
        self.memory_monitor = MemoryMonitor()
        self.memory_pressure = 0.0
    
    def check_memory_pressure(self) -> bool:
        """Check if memory pressure is high."""
        # Monitor memory usage
        self.memory_monitor.update()
        
        # Calculate memory pressure
        self.memory_pressure = (
            self.get_working_memory_usage() / 
            self.config.get('memory', {}).get('working_memory', {}).get('working_memory_capacity', 100)
        )
        
        # Trigger memory management if pressure is high
        if self.memory_pressure > 0.9:
            self.trigger_memory_management()
            return True
        
        return False
    
    def trigger_memory_management(self):
        """Trigger memory management actions."""
        # Implement memory management strategies
        strategies = [
            self.compress_working_memory,
            self.purge_old_episodes,
            self.prune_semantic_memory,
            self.clear_temporary_data
        ]
        
        # Apply strategies in order of effectiveness
        for strategy in strategies:
            if strategy():
                break  # Stop after first successful strategy
    
    def compress_working_memory(self) -> bool:
        """Compress working memory."""
        if hasattr(self, 'working_memory') and self.working_memory:
            compression_ratio = self.working_memory.compress()
            print(f"✓ Working memory compressed by {compression_ratio:.2f}%")
            return True
        return False
    
    def purge_old_episodes(self) -> bool:
        """Purge old episodic memories."""
        if hasattr(self, 'episodic_memory') and self.episodic_memory:
            purged_count = self.episodic_memory.purge_old(0.5)  # Remove older than 50% of capacity
            print(f"✓ Purged {purged_count} old episodes")
            return True
        return False
    
    def prune_semantic_memory(self) -> bool:
        """Prune semantic memory."""
        if hasattr(self, 'semantic_memory') and self.semantic_memory:
            pruned_count = self.semantic_memory.prune()
            print(f"✓ Pruned {pruned_count} low-importance concepts")
            return True
        return False
    
    def clear_temporary_data(self) -> bool:
        """Clear temporary data."""
        # Clear temporary buffers and caches
        self.clear_prediction_cache()
        self.clear_feature_cache()
        self.clear_temporary_state()
        print("✓ Cleared temporary data")
        return True

# Use memory-debugged NLM
with MemoryDebuggedNLM(config) as debugged_nlm:
    # Run simulation with memory monitoring
    for step in range(1000):
        if debugged_nlm.check_memory_pressure():
            print(f"Memory management triggered at step {step}")
        
        # Continue simulation...
```

#### Issue 2: Slow Performance

**Problem**: Simulation runs too slowly.

**Solution**: Implement performance optimization:

```python
class PerformanceDebuggedNLM(NeuralLearningMachine):
    def __init__(self, config: Dict = None):
        super().__init__(config)
        self.performance_monitor = PerformanceMonitor()
        self.performance_stats = {
            'total_time': 0,
            'neuron_processing_time': 0,
            'memory_access_time': 0,
            'optimization_opportunities': []
        }
    
    def profile_neuron_processing(self, neurons: List[Dict]) -> float:
        """Profile neuron processing time."""
        start_time = time.time()
        
        # Process neurons
        for neuron in neurons:
            self.process_single_neuron(neuron)
        
        end_time = time.time()
        processing_time = end_time - start_time
        
        # Update stats
        self.performance_stats['neuron_processing_time'] += processing_time
        self.performance_stats['total_time'] += processing_time
        
        # Check for optimization opportunities
        if processing_time > 0.1:  # More than 100ms for 1000 neurons
            self.performance_stats['optimization_opportunities'].append(
                'Consider using SIMD for neuron processing'
            )
        
        return processing_time
    
    def profile_memory_access(self, memory_operations: List[Dict]) -> float:
        """Profile memory access time."""
        start_time = time.time()
        
        # Perform memory operations
        for operation in memory_operations:
            self.perform_memory_operation(operation)
        
        end_time = time.time()
        access_time = end_time - start_time
        
        # Update stats
        self.performance_stats['memory_access_time'] += access_time
        self.performance_stats['total_time'] += access_time
        
        # Check for optimization opportunities
        if access_time > 0.05:  # More than 50ms for 100 memory operations
            self.performance_stats['optimization_opportunities'].append(
                'Consider using memory pools or caching'
            )
        
        return access_time
    
    def get_performance_report(self) -> Dict:
        """Get performance report."""
        report = self.performance_stats.copy()
        
        if report['total_time'] > 0:
            report['neuron_processing_percentage'] = (
                report['neuron_processing_time'] / report['total_time'] * 100
            )
            report['memory_access_percentage'] = (
                report['memory_access_time'] / report['total_time'] * 100
            )
        
        # Add recommendations
        report['recommendations'] = self.generate_performance_recommendations()
        
        return report
    
    def generate_performance_recommendations(self) -> List[str]:
        """Generate performance recommendations."""
        recommendations = []
        
        if self.performance_stats['optimization_opportunities']:
            recommendations.extend(self.performance_stats['optimization_opportunities'])
        
        # Check overall performance
        total_time = self.performance_stats['total_time']
        if total_time > 10.0:  # More than 10 seconds
            recommendations.append(
                'Consider increasing parallelization or reducing simulation size'
            )
        
        # Check memory usage
        if self.performance_stats['memory_access_percentage'] > 60:
            recommendations.append(
                'Optimize memory access patterns or increase memory cache size'
            )
        
        # Check neuron processing
        if self.performance_stats['neuron_processing_percentage'] > 70:
            recommendations.append(
                'Consider using vectorized operations or SIMD instructions'
            )
        
        return recommendations

# Use performance-debugged NLM
with PerformanceDebuggedNLM(config) as performance_nlm:
    # Run simulation with performance monitoring
    for step in range(1000):
        # Get neurons for processing
        neurons = nlm.get_neurons()
        
        # Profile neuron processing
        processing_time = performance_nlm.profile_neuron_processing(neurons)
        
        # Get memory operations
        memory_operations = nlm.get_memory_operations()
        
        # Profile memory access
        access_time = performance_nlm.profile_memory_access(memory_operations)
        
        if step % 100 == 0:
            print(f"Step {step}: Neuron processing {processing_time:.3f}s, "
                  f"Memory access {access_time:.3f}s")
    
    # Get performance report
    performance_report = performance_nlm.get_performance_report()
    print(f"\nPerformance Report: {performance_report}")
```

#### Issue 3: Learning Progress Stuck

**Problem**: Agent stops learning or makes no progress.

**Solution**: Implement learning diagnostics:

```python
class LearningDebuggedNLM(NeuralLearningMachine):
    def __init__(self, config: Dict = None):
        super().__init__(config)
        self.learning_analyzer = LearningAnalyzer()
        self.progress_tracker = ProgressTracker()
    
    def analyze_learning_progress(self, experience_history: List[Dict]) -> Dict:
        """Analyze learning progress over time."""
        analysis = {
            'progress_over_time': [],
            'learning_efficiency': [],
            'convergence_metrics': {},
            'bottleneck_analysis': {}
        }
        
        # Calculate progress over time
        for window_size in [10, 50, 100]:
            if len(experience_history) >= window_size:
                recent_experiences = experience_history[-window_size:]
                progress = self.calculate_progress(recent_experiences)
                analysis['progress_over_time'].append({
                    'window_size': window_size,
                    'progress': progress,
                    'average_reward': np.mean([e['reward'] for e in recent_experiences])
                })
        
        # Calculate learning efficiency
        for window_size in [10, 50, 100]:
            if len(experience_history) >= window_size:
                recent_experiences = experience_history[-window_size:]
                efficiency = self.calculate_learning_efficiency(recent_experiences)
                analysis['learning_efficiency'].append({
                    'window_size': window_size,
                    'efficiency': efficiency
                })
        
        # Analyze convergence
        analysis['convergence_metrics'] = self.analyze_convergence(experience_history)
        
        # Analyze bottlenecks
        analysis['bottleneck_analysis'] = self.analyze_learning_bottlenecks(experience_history)
        
        return analysis
    
    def calculate_progress(self, experiences: List[Dict]) -> float:
        """Calculate progress over a set of experiences."""
        if len(experiences) < 2:
            return 0.0
        
        # Calculate progress based on reward improvement
        rewards = [e['reward'] for e in experiences]
        
        # Calculate correlation between experience index and reward
        x = list(range(len(rewards)))
        correlation = np.corrcoef(x, rewards)[0, 1]
        
        return max(0, correlation)  # Ensure non-negative progress
    
    def calculate_learning_efficiency(self, experiences: List[Dict]) -> float:
        """Calculate learning efficiency."""
        if len(experiences) < 2:
            return 0.0
        
        # Calculate efficiency as improvement per experience
        rewards = [e['reward'] for e in experiences]
        
        # Calculate average reward improvement
        if len(rewards) >= 10:
            first_half = rewards[:len(rewards)//2]
            second_half = rewards[len(rewards)//2:]
            
            first_avg = np.mean(first_half)
            second_avg = np.mean(second_half)
            
            improvement = second_avg - first_avg
            efficiency = improvement / (len(rewards) // 2) if (len(rewards) // 2) > 0 else 0
        
        return efficiency
    
    def analyze_convergence(self, experiences: List[Dict]) -> Dict:
        """Analyze convergence of learning."""
        if len(experiences) < 20:
            return {'converged': False, 'confidence': 0.0}
        
        # Split experiences into two halves
        split_point = len(experiences) // 2
        first_half = experiences[:split_point]
        second_half = experiences[split_point:]
        
        # Calculate statistics
        first_rewards = [e['reward'] for e in first_half]
        second_rewards = [e['reward'] for e in second_half]
        
        first_mean = np.mean(first_rewards)
        second_mean = np.mean(second_rewards)
        
        # Check for convergence (small difference in means)
        mean_diff = abs(second_mean - first_mean)
        converged = mean_diff < 0.1  # Less than 0.1 difference
        
        # Calculate confidence based on variance
        first_std = np.std(first_rewards)
        second_std = np.std(second_rewards)
        
        confidence = 1.0 - (first_std + second_std) / 2.0
        confidence = max(0.0, min(1.0, confidence))
        
        return {
            'converged': converged,
            'mean_difference': mean_diff,
            'confidence': confidence,
            'first_half_mean': first_mean,
            'second_half_mean': second_mean
        }
    
    def analyze_learning_bottlenecks(self, experiences: List[Dict]) -> Dict:
        """Analyze potential learning bottlenecks."""
        bottlenecks = {}
        
        if len(experiences) < 10:
            return bottlenecks
        
        # Check for reward sparsity
        rewards = [e['reward'] for e in experiences]
        zero_reward_count = sum(1 for r in rewards if r <= 0)
        
        if zero_reward_count / len(rewards) > 0.7:
            bottlenecks['reward_sparsity'] = {
                'severity': 'high' if zero_reward_count / len(rewards) > 0.9 else 'medium',
                'zero_reward_ratio': zero_reward_count / len(rewards)
            }
        
        # Check for novelty saturation
        novelties = [e.get('novelty', 0.0) for e in experiences]
        avg_novelty = np.mean(novelties)
        
        if avg_novelty < 0.2:
            bottlenecks['novelty_saturation'] = {
                'severity': 'high' if avg_novelty < 0.1 else 'medium',
                'average_novelty': avg_novelty
            }
        
        # Check for action repetition
        actions = [e['action']['type'] for e in experiences]
        action_counts = {}
        for action in actions:
            action_counts[action] = action_counts.get(action, 0) + 1
        
        max_action_count = max(action_counts.values())
        if max_action_count / len(actions) > 0.6:
            bottlenecks['action_repetition'] = {
                'severity': 'high' if max_action_count / len(actions) > 0.8 else 'medium',
                'most_repeated_action': max(action_counts, key=action_counts.get),
                'repetition_ratio': max_action_count / len(actions)
            }
        
        # Check for memory saturation
        memory_usage = [e.get('memory_usage', 0.0) for e in experiences]
        avg_memory_usage = np.mean(memory_usage)
        
        if avg_memory_usage > 0.9:
            bottlenecks['memory_saturation'] = {
                'severity': 'high' if avg_memory_usage > 0.95 else 'medium',
                'average_memory_usage': avg_memory_usage
            }
        
        return bottlenecks
    
    def get_learning_diagnostics(self, experience_history: List[Dict]) -> Dict:
        """Get comprehensive learning diagnostics."""
        diagnostics = {
            'analysis': self.analyze_learning_progress(experience_history),
            'recommendations': self.generate_learning_recommendations(experience_history)
        }
        
        return diagnostics
    
    def generate_learning_recommendations(self, experiences: List[Dict]) -> List[str]:
        """Generate recommendations for improving learning."""
        recommendations = []
        
        # Get analysis results
        analysis = self.analyze_learning_progress(experiences)
        bottlenecks = analysis.get('bottleneck_analysis', {})
        
        # Generate recommendations based on bottlenecks
        if 'reward_sparsity' in bottlenecks:
            recommendations.append(
                'Increase reward diversity by introducing more varied rewards'
            )
        
        if 'novelty_saturation' in bottlenecks:
            recommendations.append(
                'Increase environmental variety or introduce new stimuli'
            )
        
        if 'action_repetition' in bottlenecks:
            recommendations.append(
                'Implement action selection strategies to reduce repetition'
            )
        
        if 'memory_saturation' in bottlenecks:
            recommendations.append(
                'Implement memory management strategies or increase memory capacity'
            )
        
        # Check overall progress
        if len(analysis['progress_over_time']) > 0:
            latest_progress = analysis['progress_over_time'][-1]['progress']
            
            if latest_progress < 0.1:
                recommendations.append(
                    'Learning appears to have stalled. Consider increasing exploration or adjusting reward function'
                )
            elif latest_progress < 0.3:
                recommendations.append(
                    'Learning progress is slow. Consider adjusting learning rate or introducing more challenging tasks'
                )
        
        return recommendations

# Use learning-debugged NLM
with LearningDebuggedNLM(config) as learning_debugged_nlm:
    # Run simulation with learning diagnostics
    experience_history = []
    
    for step in range(1000):
        # Generate experience
        experience = generate_experience()
        experience_history.append(experience)
        
        # Get learning diagnostics periodically
        if step % 100 == 0 and len(experience_history) >= 10:
            diagnostics = learning_debugged_nlm.get_learning_diagnostics(experience_history[-100:])
            print(f"\nLearning Diagnostics at step {step}:")
            print(f"Progress: {diagnostics['analysis']['progress_over_time'][-1]['progress']:.3f}")
            print(f"Learning efficiency: {diagnostics['analysis']['learning_efficiency'][-1]['efficiency']:.3f}")
            
            if diagnostics['analysis']['convergence_metrics']['converged']:
                print("✓ Learning has converged")
            else:
                print(f"⚠ Learning not yet converged (difference: {diagnostics['analysis']['convergence_metrics']['mean_difference']:.3f})")
            
            if diagnostics['analysis']['bottleneck_analysis']:
                print("⚠ Potential bottlenecks detected:")
                for bottleneck, details in diagnostics['analysis']['bottleneck_analysis'].items():
                    print(f"  - {bottleneck}: {details}")
            
            if diagnostics['recommendations']:
                print("Recommendations:")
                for recommendation in diagnostics['recommendations']:
                    print(f"  - {recommendation}")
        
        # Continue simulation...
```

---

## Conclusion

This intermediate tutorial covers advanced NLM concepts including:

1. **Advanced Configuration Management**: Custom configuration schemas and validation
2. **Custom Neuron Models**: Implementing neuron-specific dynamics
3. **Memory System Implementation**: Enhanced memory systems with compression and hierarchy
4. **Neuromodulation Deep Dive**: Complex neuromodulation interactions
5. **Complex Learning Scenarios**: Multi-task learning, transfer learning, and sophisticated optimization
6. **Performance Optimization**: Parallel processing, memory optimization, and vectorization
7. **Debugging and Troubleshooting**: Comprehensive diagnostics for memory, performance, and learning

You now have a comprehensive understanding of NLM's advanced features and can create sophisticated learning systems. The combination of custom configurations, advanced memory systems, and sophisticated neuromodulation allows for the creation of highly capable and adaptable learning agents.

For further exploration, consider:

- [Custom Neuron Models](scripts/examples/) - See examples of custom neuron implementations
- [Performance Optimization](scripts/benchmark.py) - Benchmark and optimize your configurations
- [Visualization Tools](scripts/visualize.py) - Visualize your neural networks and learning progress
- [Configuration Validation](scripts/validate_config.py) - Validate and debug your configurations

The NLM framework offers immense flexibility for creating advanced neural learning systems. Use these tools and concepts to build your unique applications!

---

Happy advanced learning with NLM!