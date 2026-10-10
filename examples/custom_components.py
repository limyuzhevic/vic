#!/usr/bin/env python3
"""
NLM Custom Component Examples

This file demonstrates custom component creation and extension for NLM
including custom brain regions, plasticity rules, neuromodulators, memory systems,
and plugin architecture.

Requirements:

### 3. **Custom Component Examples**
- **Custom Brain Region**: Creating new neural regions with specific functions
- **Custom Plasticity Rule**: Implementing novel learning rules
- **Custom Neuromodulator**: Creating new neuromodulator types
- **Custom Memory System**: Adding new memory capabilities
- **Plugin Architecture**: Loading and integrating third-party components
"""

import pynlm
import logging
import json
import importlib
from abc import ABC, abstractmethod
from typing import Dict, Any, List, Optional

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

def print_header(title: str):
    """Print a formatted header."""
    print(f"\n{'='*60}")
    print(f"{title}")
    print(f"{'='*60}")

# ============================================================================
# 1. Custom Brain Region Examples
# ============================================================================

class CustomBrainRegion:
    """Example of creating a custom brain region."""
    
    def __init__(self, name: str, function: str, neuron_count: int = 100):
        self.name = name
        self.function = function
        self.neuron_count = neuron_count
        self.properties = {}
        self.connections = []
        
    def add_property(self, key: str, value: Any):
        """Add a custom property to the region."""
        self.properties[key] = value
        
    def add_connection(self, target_region: str, weight: float = 1.0):
        """Add a connection to another region."""
        self.connections.append({
            'target': target_region,
            'weight': weight,
            'delay': 1
        })
        
    def configure_special_functions(self, neuromodulators: List[str]):
        """Configure which neuromodulators affect this region."""
        self.neuromodulators = neuromodulators
        
    def __repr__(self):
        return f"CustomBrainRegion(name='{self.name}', function='{self.function}', neurons={self.neuron_count})"


def create_custom_foraging_region():
    """Create a custom region specialized for foraging behavior."""
    print_header("1.1 Custom Foraging Region")
    
    # Create custom region configuration
    foraging_region = CustomBrainRegion(
        name="ForagingRegion",
        function="resource_location_and_acquisition",
        neuron_count=150
    )
    
    # Add region-specific properties
    foraging_region.add_property("receptive_field_size", 8.0)
    foraging_region.add_property("movement_range", 15.0)
    foraging_region.add_property("exploration_bias", 0.3)
    foraging_region.add_property("exploitation_bias", 0.7)
    foraging_region.add_property("resource_sensitivity", 1.2)
    
    # Configure neuromodulator sensitivity
    foraging_region.configure_special_functions(["dopamine", "acetylcholine", "norepinephrine"])
    
    # Add connections to other regions
    foraging_region.add_connection("SensoryRegion", 0.8)
    foraging_region.add_connection("MotorRegion", 1.0)
    foraging_region.add_connection("WorkingMemory", 0.6)
    
    # Demonstrate using the custom region
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # In a real implementation, this would use plugin architecture
    # For now, we demonstrate the concept
    print(f"Created custom foraging region: {foraging_region}")
    print(f"Region properties: {foraging_region.properties}")
    print(f"Target connections: {[c['target'] for c in foraging_region.connections]}")
    print(f"Neuromodulator sensitivity: {foraging_region.neuromodulators}")
    
    return foraging_region, brain


def create_custom_social_region():
    """Create a custom region specialized for social behavior."""
    print_header("1.2 Custom Social Region")
    
    # Create social behavior region
    social_region = CustomBrainRegion(
        name="SocialRegion",
        function="social_cognition_and_interaction",
        neuron_count=200
    )
    
    # Social region specific properties
    social_region.add_property("group_size", 5)
    social_region.add_property("status_sensitivity", 1.5)
    social_region.add_property("imitation_strength", 0.8)
    social_region.add_property("cooperation_bias", 0.6)
    social_region.add_property("competition_level", 0.4)
    
    # Social neuromodulators
    social_region.configure_special_functions(["serotonin", "dopamine"])
    
    # Connections for social processing
    social_region.add_connection("WorkingMemory", 0.7)
    social_region.add_connection("EpisodicMemory", 0.8)
    social_region.add_connection("Curiosity", 0.5)
    
    # Demonstrate social learning patterns
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"Created custom social region: {social_region}")
    print(f"Key social properties:")
    print(f"  - Status sensitivity: {social_region.properties['status_sensitivity']}")
    print(f"  - Imitation strength: {social_region.properties['imitation_strength']}")
    print(f"  - Cooperation bias: {social_region.properties['cooperation_bias']}")
    
    # Simulate social learning
    print("\nSocial learning simulation:")
    social_patterns = [
        [0.1, 0.2, 0.3, 0.4],
        [0.2, 0.3, 0.4, 0.5],  # Imitated pattern
        [0.3, 0.4, 0.5, 0.6],  # Modified pattern
    ]
    
    for i, pattern in enumerate(social_patterns):
        imitation_strength = social_region.properties['imitation_strength']
        modified_pattern = [p * imitation_strength for p in pattern]
        if i > 0:
            modified_pattern = [p + (1 - imitation_strength) * 0.1 for p in modified_pattern]
        print(f"  Pattern {i+1}: {pattern} -> {modified_pattern}")
    
    return social_region, brain


def custom_region_factory():
    """Factory pattern for creating custom regions."""
    print_header("1.3 Custom Region Factory")
    
    region_registry = {}
    
    def register_region(region_type: str, creator_func):
        """Register a region creator function."""
        region_registry[region_type] = creator_func
        
    def create_region(region_type: str, **kwargs) -> CustomBrainRegion:
        """Create a custom region by type."""
        if region_type in region_registry:
            return region_registry[region_type](**kwargs)
        else:
            raise ValueError(f"Unknown region type: {region_type}")
    
    # Register different region types
    register_region("foraging", lambda **kwargs: CustomBrainRegion(
        name="DynamicForaging", function="adaptive_resource_search", **kwargs))
    
    register_region("social", lambda **kwargs: CustomBrainRegion(
        name="DynamicSocial", function="group_coordination", **kwargs))
    
    register_region("cognitive", lambda **kwargs: CustomBrainRegion(
        name="DynamicCognitive", function="problem_solving", **kwargs))
    
    register_region("motor", lambda **kwargs: CustomBrainRegion(
        name="DynamicMotor", function="action_execution", **kwargs))
    
    # Use the factory
    print("Available region types:", list(region_registry.keys()))
    
    foraging_region = create_region("foraging", neuron_count=120)
    social_region = create_region("social", neuron_count=180)
    
    print(f"\nCreated via factory:")
    print(f"  - {foraging_region}")
    print(f"  - {social_region}")
    
    return foraging_region, social_region


def custom_region_examples_summary():
    """Print summary of custom brain region examples."""
    print_header("Custom Brain Region Examples Summary")
    
    print("\nThis section demonstrated:")
    print("✓ Creating specialized brain regions for specific functions")
    print("✓ Region-specific property configuration")
    print("✓ Neuromodulator sensitivity tuning")
    print("✓ Connection management between regions")
    print("✓ Social and foraging behavior modeling")
    print("✓ Factory pattern for region creation")

# ============================================================================
# 2. Custom Plasticity Rule Examples
# ============================================================================

class PlasticityRule(ABC):
    """Abstract base class for plasticity rules."""
    
    @abstractmethod
    def update_synaptic_strength(self, pre_neuron: Any, post_neuron: Any, 
                               current_strength: float, dt: float) -> float:
        """Update synaptic strength based on pre/post activity."""
        pass
    
    @abstractmethod
    def get_name(self) -> str:
        """Get rule name."""
        pass
    
    @abstractmethod
    def get_parameters(self) -> Dict[str, Any]:
        """Get rule parameters."""
        pass


class HebbianPlasticity(PlasticityRule):
    """Classic Hebbian learning rule (neurons that fire together wire together)."""
    
    def __init__(self, learning_rate: float = 0.01, max_weight: float = 1.0):
        self.learning_rate = learning_rate
        self.max_weight = max_weight
        
    def update_synaptic_strength(self, pre_neuron: Any, post_neuron: Any,
                               current_strength: float, dt: float) -> float:
        # Simple Hebbian update: strengthen if both active
        new_strength = current_strength + self.learning_rate * dt
        return min(new_strength, self.max_weight)
    
    def get_name(self) -> str:
        return "HebbianPlasticity"
    
    def get_parameters(self) -> Dict[str, Any]:
        return {
            "learning_rate": self.learning_rate,
            "max_weight": self.max_weight
        }


class OjaPlasticity(PlasticityRule):
    """Oja's rule - normalized Hebbian learning for stable weight updates."""
    
    def __init__(self, learning_rate: float = 0.01, regularization: float = 0.001):
        self.learning_rate = learning_rate
        self.regularization = regularization
        
    def update_synaptic_strength(self, pre_neuron: Any, post_neuron: Any,
                               current_strength: float, dt: float) -> float:
        # Oja's rule with normalization
        # w_new = w_old + η * y * (x - y * w_old)
        pre_activity = 1.0  # Simplified
        post_activity = 1.0  # Simplified
        
        weight_change = self.learning_rate * post_activity * (pre_activity - 
                                                             post_activity * current_strength)
        new_strength = current_strength + weight_change
        
        # Normalize to prevent runaway
        normalized = new_strength / (1 + self.regularization * abs(new_strength))
        
        return max(0.0, min(normalized, 1.0))
    
    def get_name(self) -> str:
        return "OjaPlasticity"
    
    def get_parameters(self) -> Dict[str, Any]:
        return {
            "learning_rate": self.learning_rate,
            "regularization": self.regularization
        }


class BayesianPlasticity(PlasticityRule):
    """Bayesian curve matching for dynamic threshold adaptation."""
    
    def __init__(self, adaptation_rate: float = 0.05, initial_threshold: float = 0.5):
        self.adaptation_rate = adaptation_rate
        self.current_threshold = initial_threshold
        self.covariance_estimate = 1.0
        
    def update_synaptic_strength(self, pre_neuron: Any, post_neuron: Any,
                               current_strength: float, dt: float) -> float:
        # Bayesian update based on prediction error
        prediction_error = abs(pre_neuron - post_neuron)
        
        # Update threshold based on error
        learning_signal = min(prediction_error, 1.0)
        self.current_threshold += self.adaptation_rate * (learning_signal - self.current_threshold)
        
        # Compute weight based on threshold
        if prediction_error > self.current_threshold:
            # Strong update when error is high
            new_strength = current_strength + 0.02 * dt
        else:
            # Weak update when error is low
            new_strength = current_strength + 0.001 * dt
        
        return max(0.0, min(new_strength, 1.0))
    
    def get_name(self) -> str:
        return "BayesianPlasticity"
    
    def get_parameters(self) -> Dict[str, Any]:
        return {
            "adaptation_rate": self.adaptation_rate,
            "current_threshold": self.current_threshold,
            "covariance_estimate": self.covariance_estimate
        }


def create_custom_plasticity_rules():
    """Create and demonstrate custom plasticity rules."""
    print_header("2.1 Custom Plasticity Rules")
    
    # Create different plasticity rule instances
    rules = {
        "Hebbian": HebbianPlasticity(learning_rate=0.01),
        "Oja": OjaPlasticity(learning_rate=0.02, regularization=0.001),
        "Bayesian": BayesianPlasticity(adaptation_rate=0.05)
    }
    
    print("\nAvailable plasticity rules:")
    for name, rule in rules.items():
        print(f"  - {name}: {rule.get_name()}")
        print(f"    Parameters: {rule.get_parameters()}")
    
    # Demonstrate plasticity rule application
    print("\n2.2 Plasticity Rule Demonstration:")
    
    test_neuron = {"activity": 0.8}
    pre_neuron = {"activity": 0.6}
    post_neuron = {"activity": 0.9}
    
    current_strength = 0.5
    dt = 0.1
    
    for name, rule in rules.items():
        new_strength = rule.update_synaptic_strength(
            pre_neuron, post_neuron, current_strength, dt)
        
        print(f"  {name} rule:")
        print(f"    Input strength: {current_strength:.3f}")
        print(f"    New strength: {new_strength:.3f}")
        print(f"    Change: {new_strength - current_strength:.3f}")
    
    return rules


def plasticity_rule_factory():
    """Factory for creating plasticity rule instances."""
    print_header("2.3 Plasticity Rule Factory")
    
    rule_registry = {}
    
    def register_rule(rule_type: str, creator_func):
        """Register a rule creator function."""
        rule_registry[rule_type] = creator_func
        
    def create_rule(rule_type: str, **kwargs) -> PlasticityRule:
        """Create a plasticity rule by type."""
        if rule_type in rule_registry:
            return rule_registry[rule_type](**kwargs)
        else:
            raise ValueError(f"Unknown rule type: {rule_type}")
    
    # Register rule types
    register_rule("hebbian", lambda **kwargs: HebbianPlasticity(**kwargs))
    register_rule("oja", lambda **kwargs: OjaPlasticity(**kwargs))
    register_rule("bayesian", lambda **kwargs: BayesianPlasticity(**kwargs))
    
    print("Available rule types:", list(rule_registry.keys()))
    
    # Create rules using factory
    hebbian_rule = create_rule("hebbian", learning_rate=0.02)
    oja_rule = create_rule("oja", learning_rate=0.03, regularization=0.005)
    
    print(f"\nCreated via factory:")
    print(f"  - {hebbian_rule.get_name()}: {hebbian_rule.get_parameters()}")
    print(f"  - {oja_rule.get_name()}: {oja_rule.get_parameters()}")
    
    return hebbian_rule, oja_rule


def adaptive_plasticity_example():
    """Demonstrate adaptive plasticity with neuromodulation."""
    print_header("2.4 Adaptive Plasticity with Neuromodulation")
    
    print("\n2.4.1 Neuromodulator-Adaptive Plasticity:")
    
    # Different neuromodulators affect plasticity differently
    neuromodulator_effects = {
        "dopamine": {"plasticity_boost": 1.5, "learning_rate": 0.1},
        "acetylcholine": {"plasticity_boost": 1.2, "learning_rate": 0.05},
        "norepinephrine": {"plasticity_boost": 1.3, "learning_rate": 0.08},
        "serotonin": {"plasticity_boost": 0.8, "learning_rate": 0.03}
    }
    
    print("Neuromodulator effects on plasticity:")
    for mod_name, effects in neuromodulator_effects.items():
        print(f"  {mod_name.upper()}:")
        print(f"    Plasticity boost: {effects['plasticity_boost']:.1f}x")
        print(f"    Learning rate: {effects['learning_rate']}")
    
    # Simulate learning with different neuromodulator states
    print("\n2.4.2 Learning with Neuromodulation:")
    
    for mod_name, effects in neuromodulator_effects.items():
        base_rule = HebbianPlasticity(learning_rate=0.01)
        adjusted_rule = HebbianPlasticity(
            learning_rate=effects['learning_rate'] * effects['plasticity_boost'])
        
        print(f"\n{mod_name.upper()} learning:")
        print(f"  Base learning rate: 0.01")
        print(f"  {mod_name} learning rate: {adjusted_rule.learning_rate:.4f}")
        
        # Test with same inputs
        new_strength = adjusted_rule.update_synaptic_strength(
            {"activity": 0.7}, {"activity": 0.8}, 0.5, 0.1)
        
        print(f"  Weight update: {0.5:.3f} -> {new_strength:.3f}")
    
    return neuromodulator_effects


def plasticity_examples_summary():
    """Print summary of custom plasticity rule examples."""
    print_header("Custom Plasticity Rule Examples Summary")
    
    print("\nThis section demonstrated:")
    print("✓ Abstract base class for plasticity rules")
    print("✓ Implementation of Hebbian, Oja, and Bayesian rules")
    print("✓ Factory pattern for rule creation")
    print("✓ Neuromodulator-dependent plasticity modulation")
    print("✓ Rule parameter configuration and adaptation")

# ============================================================================
# 3. Custom Neuromodulator Examples
# ============================================================================

class CustomNeuromodulator:
    """Base class for custom neuromodulators."""
    
    def __init__(self, name: str, base_level: float = 0.5):
        self.name = name
        self.base_level = base_level
        self.level = base_level
        self.time_constant = 1.0
        self.target_neuron_types = []
        self.effects = {}
        
    def update_level(self, delta: float, dt: float):
        """Update neuromodulator level with exponential decay."""
        self.level += delta
        self.level = max(0.0, min(self.level, 1.0))
        
    def apply_to_neuron(self, neuron_id: Any, neuron_activity: float) -> float:
        """Apply neuromodulatory effect to a neuron."""
        effect_strength = self.effects.get(neuron_id.type, 1.0)
        return neuron_activity * (1 + effect_strength * (self.level - 0.5))
        
    def set_sensitivity(self, neuron_type: str, sensitivity: float):
        """Set sensitivity for specific neuron types."""
        self.effects[neuron_type] = sensitivity


class AttentionGatingNeuromodulator(CustomNeuromodulator):
    """Custom neuromodulator that controls attention allocation."""
    
    def __init__(self):
        super().__init__("AttentionGating", 0.5)
        self.focus_regions = []
        self.inhibition_strength = 0.8
        self.excitation_strength = 1.2
        
    def focus_on_region(self, region_id: str):
        """Apply strong attention to a region."""
        self.focus_regions.append(region_id)
        
    def release_attention(self, region_id: str):
        """Release attention from a region."""
        if region_id in self.focus_regions:
            self.focus_regions.remove(region_id)
            
    def update(self, dt: float):
        """Update attention dynamics."""
        # Decay attention when not focused
        if not self.focus_regions:
            self.level *= (1 - dt / self.time_constant)


class StressNeuromodulator(CustomNeuromodulator):
    """Custom neuromodulator representing stress response."""
    
    def __init__(self):
        super().__init__("Stress", 0.0)
        self.cortisol_level = 0.0
        self.hyperarousal = False
        self.response_time = 0.5
        
    def signal_stress(self, intensity: float, duration: float):
        """Signal a stress event."""
        self.level += intensity
        self.cortisol_level += intensity * duration
        self.hyperarousal = True
        
    def reduce_stress(self, recovery_rate: float = 0.1):
        """Reduce stress level over time."""
        self.level *= (1 - recovery_rate)
        self.cortisol_level *= (1 - recovery_rate)
        if self.level < 0.1:
            self.hyperarousal = False


class CuriosityNeuromodulator(CustomNeuromodulator):
    """Enhanced curiosity neuromodulator for exploration."""
    
    def __init__(self):
        super().__init__("Curiosity", 0.3)
        self.exploration_history = []
        self.novelty_threshold = 0.7
        self.intrinsic_motivation = 0.5
        
    def experience_novelty(self, novelty_score: float):
        """Experience something novel."""
        self.exploration_history.append(novelty_score)
        if len(self.exploration_history) > 10:
            self.exploration_history.pop(0)
            
        # Update curiosity based on novelty
        if novelty_score > self.novelty_threshold:
            self.level += 0.1
            self.intrinsic_motivation = min(1.0, self.intrinsic_motivation + 0.05)
            
    def get_exploration_drive(self) -> float:
        """Calculate current exploration drive."""
        recent_novelty = sum(self.exploration_history[-3:]) / len(self.exploration_history) if self.exploration_history else 0
        return self.level * 0.5 + self.intrinsic_motivation * 0.5 + recent_novelty * 0.3


def create_custom_neuromodulators():
    """Create and demonstrate custom neuromodulators."""
    print_header("3.1 Custom Neuromodulators")
    
    # Create custom neuromodulator instances
    neuromodulators = {
        "AttentionGating": AttentionGatingNeuromodulator(),
        "Stress": StressNeuromodulator(),
        "Curiosity": CuriosityNeuromodulator()
    }
    
    print("\nAvailable custom neuromodulators:")
    for name, mod in neuromodulators.items():
        print(f"  - {name}: {mod.name}")
        print(f"    Base level: {mod.base_level}")
        print(f"    Time constant: {mod.time_constant}")
    
    # Demonstrate neuromodulator functionality
    print("\n3.2 Neuromodulator Demonstration:")
    
    attention_mod = neuromodulators["AttentionGating"]
    stress_mod = neuromodulators["Stress"]
    curiosity_mod = neuromodulators["Curiosity"]
    
    # Simulate attention modulation
    attention_mod.focus_on_region("VisualCortex")
    attention_mod.level = 0.9
    print(f"\nAttentionGating:")
    print(f"  Focused regions: {attention_mod.focus_regions}")
    print(f"  Current level: {attention_mod.level}")
    
    # Simulate stress response
    stress_mod.signal_stress(intensity=0.8, duration=2.0)
    print(f"\nStress:")
    print(f"  Stress level: {stress_mod.level}")
    print(f"  Cortisol level: {stress_mod.cortisol_level}")
    print(f"  Hyperarousal: {stress_mod.hyperarousal}")
    
    # Simulate curiosity
    curiosity_mod.experience_novelty(0.9)  # High novelty
    curiosity_mod.experience_novelty(0.3)  # Low novelty
    print(f"\nCuriosity:")
    print(f"  Current level: {curiosity_mod.level}")
    print(f"  Exploration drive: {curiosity_mod.get_exploration_drive():.3f}")
    print(f"  Intrinsic motivation: {curiosity_mod.intrinsic_motivation:.3f}")
    
    return neuromodulators


def neuromodulator_plugin_example():
    """Plugin architecture for neuromodulators."""
    print_header("3.3 Neuromodulator Plugin Architecture")
    
    print("\nPlugin Architecture:")
    print("1. Plugin Registry - Centralized plugin management")
    print("2. Plugin Interface - Standardized API")
    print("3. Dynamic Loading - Runtime plugin integration")
    print("4. Parameter Configuration - Plugin-specific settings")
    
    # Plugin registry simulation
    neuromodulator_plugins = {}
    
    def register_neuromodulator_plugin(plugin_name: str, plugin_class):
        """Register a neuromodulator plugin."""
        neuromodulator_plugins[plugin_name] = plugin_class
        
    def load_and_configure_plugin(plugin_name: str, **config) -> CustomNeuromodulator:
        """Load and configure a neuromodulator plugin."""
        if plugin_name in neuromodulator_plugins:
            plugin_class = neuromodulator_plugins[plugin_name]
            instance = plugin_class()
            
            # Apply configuration
            for key, value in config.items():
                if hasattr(instance, key):
                    setattr(instance, key, value)
                    
            return instance
        else:
            raise ValueError(f"Unknown plugin: {plugin_name}")
    
    # Register example plugins
    def create_advanced_attention_plugin(**config):
        """Plugin for advanced attention modulation."""
        mod = AttentionGatingNeuromodulator()
        mod.inhibition_strength = config.get('inhibition_strength', 0.8)
        mod.excitation_strength = config.get('excitation_strength', 1.2)
        return mod
        
    def create_advanced_stress_plugin(**config):
        """Plugin for advanced stress response."""
        mod = StressNeuromodulator()
        mod.response_time = config.get('response_time', 0.5)
        mod.cortisol_half_life = config.get('cortisol_half_life', 1.0)
        return mod
        
    register_neuromodulator_plugin("AdvancedAttention", create_advanced_attention_plugin)
    register_neuromodulator_plugin("AdvancedStress", create_advanced_stress_plugin)
    
    # Load and configure plugins
    print(f"\nRegistered plugins: {list(neuromodulator_plugins.keys())}")
    
    advanced_attention = load_and_configure_plugin(
        "AdvancedAttention",
        inhibition_strength=0.9,
        excitation_strength=1.5
    )
    
    advanced_stress = load_and_configure_plugin(
        "AdvancedStress",
        response_time=0.8,
        cortisol_half_life=2.0
    )
    
    print(f"\nLoaded plugins:")
    print(f"  - AdvancedAttention level: {advanced_attention.level}")
    print(f"  - AdvancedStress level: {advanced_stress.level}")
    
    return neuromodulator_plugins


def neuromodulator_examples_summary():
    """Print summary of custom neuromodulator examples."""
    print_header("Custom Neuromodulator Examples Summary")
    
    print("\nThis section demonstrated:")
    print("✓ Custom neuromodulator base class")
    print("✓ AttentionGating, Stress, and Curiosity neuromodulators")
    print("✓ Neuromodulator sensitivity and targeting")
    print("✓ Plugin architecture for neuromodulator extension")
    print("✓ Dynamic neuromodulator configuration")

# ============================================================================
# 4. Custom Memory System Examples
# ============================================================================

class CustomMemorySystem:
    """Base class for custom memory systems."""
    
    def __init__(self, name: str, capacity: int = 1000):
        self.name = name
        self.capacity = capacity
        self.storage = {}
        self.metadata = {}
        
    def store(self, key: str, value: Any, metadata: Dict = None):
        """Store data in the memory system."""
        if len(self.storage) >= self.capacity:
            self._evict_oldest()
            
        self.storage[key] = value
        self.metadata[key] = metadata or {}
        
    def retrieve(self, key: str) -> Any:
        """Retrieve data from the memory system."""
        return self.storage.get(key)
        
    def contains(self, key: str) -> bool:
        """Check if key exists."""
        return key in self.storage
        
    def clear(self):
        """Clear all data."""
        self.storage.clear()
        self.metadata.clear()
        
    def size(self) -> int:
        """Get current size."""
        return len(self.storage)
        
    def _evict_oldest(self):
        """Evict oldest entry when at capacity."""
        if self.storage:
            oldest_key = next(iter(self.storage))
            del self.storage[oldest_key]
            if oldest_key in self.metadata:
                del self.metadata[oldest_key]
class TemporalMemory(CustomMemorySystem):
    """Memory system optimized for temporal sequences."""
    
    def __init__(self, capacity: int = 500):
        super().__init__("TemporalMemory", capacity)
        self.sequence_patterns = {}
        self.temporal_weights = {}
        
    def store_sequence(self, sequence_id: str, sequence: List[Any], time_window: float = 1.0):
        """Store a temporal sequence with time information."""
        key = f"seq_{sequence_id}"
        self.sequence_patterns[key] = sequence
        self.temporal_weights[key] = time_window
        
        # Store in base memory system
        super().store(key, sequence, {"type": "temporal", "time_window": time_window})
        
    def retrieve_sequence(self, sequence_id: str, time_threshold: float = 0.0) -> List[Any]:
        """Retrieve a temporal sequence if within time threshold."""
        key = f"seq_{sequence_id}"
        if key in self.storage:
            time_window = self.temporal_weights.get(key, 0.0)
            if time_threshold <= time_window:
                return self.storage[key]
        return None
        
    def complete_pattern(self, partial_sequence: List[Any]) -> List[Any]:
        """Complete a partial sequence based on stored patterns."""
        completed = partial_sequence.copy()
        
        # Find most similar partial sequence
        for key, stored_seq in self.sequence_patterns.items():
            if len(stored_seq) > len(partial_sequence) and len(partial_sequence) > 0:
                # Check if partial matches start of stored sequence
                if stored_seq[:len(partial_sequence)] == partial_sequence:
                    # Complete the pattern
                    remaining = stored_seq[len(partial_sequence):]
                    completed.extend(remaining)
                    break
        
        return completed
class EmotionalMemory(CustomMemorySystem):
    """Memory system that stores emotional valences and associations."""
    
    def __init__(self, capacity: int = 300):
        super().__init__("EmotionalMemory", capacity)
        self.valence_scores = {}
        self.arousal_levels = {}
        self.emotional_tags = {}
        
    def store_with_emotion(self, key: str, value: Any, valence: float, arousal: float, tags: List[str]):
        """Store data with emotional information."""
        super().store(key, value, {
            "valence": valence,
            "arousal": arousal,
            "tags": tags,
            "timestamp": pynlm.getSimulationTime() if hasattr(pynlm, 'getSimulationTime') else 0
        })
        
        self.valence_scores[key] = valence
        self.arousal_levels[key] = arousal
        self.emotional_tags[key] = tags
        
    def retrieve_by_emotion(self, min_valence: float = -1.0, max_valence: float = 1.0,
                           min_arousal: float = 0.0, max_arousal: float = 1.0) -> Dict:
        """Retrieve memories based on emotional criteria."""
        results = {}
        
        for key, value in self.storage.items():
            meta = self.metadata.get(key, {})
            
            if (min_valence <= meta.get("valence", 0) <= max_valence and
                min_arousal <= meta.get("arousal", 0) <= max_arousal):
                results[key] = {
                    "value": value,
                    "valence": meta.get("valence", 0),
                    "arousal": meta.get("arousal", 0),
                    "tags": meta.get("tags", [])
                }
        
        return results
        
    def influence_learning(self, target_neuron: Any, base_strength: float) -> float:
        """Use emotional memory to modulate learning."""
        # Find most relevant emotional memory
        relevant_memories = []
        
        for key, strength in self.valence_scores.items():
            relevance = abs(strength) * (1.0 - abs(strength))  # Peak at strength = ±0.5
            relevant_memories.append((relevance, strength))
        
        if relevant_memories:
            total_relevance = sum(r for r, _ in relevant_memories)
            weighted_effect = sum(r * s for r, s in relevant_memories) / total_relevance
            
            # Apply emotional influence
            return base_strength * (1 + weighted_effect * 0.5)
        
        return base_strength
class ImplicitMemory(CustomMemorySystem):
    """Implicit memory system for skills and habits (subsynaptic)."""
    
    def __init__(self, capacity: int = 200):
        super().__init__("ImplicitMemory", capacity)
        self.skill_patterns = {}
        self.habit_strengths = {}
        self.procedural_memory = {}
        
    def learn_skill(self, skill_name: str, pattern: List[float], proficiency: float = 0.0):
        """Learn a procedural skill."""
        self.skill_patterns[skill_name] = pattern
        self.habit_strengths[skill_name] = proficiency
        self.procedural_memory[skill_name] = {
            "pattern": pattern,
            "proficiency": proficiency,
            "last_practice": 0,
            "decay_rate": 0.95
        }
        
    def execute_skill(self, skill_name: str, input_pattern: List[float]) -> List[float]:
        """Execute a learned skill."""
        if skill_name in self.procedural_memory:
            skill = self.procedural_memory[skill_name]
            
            # Apply skill pattern to input
            output = []
            for i, (input_val, pattern_val) in enumerate(zip(input_pattern, skill["pattern"])):
                if i < len(input_pattern):
                    output_val = input_val * pattern_val * skill["proficiency"]
                    output.append(output_val)
            
            # Decay proficiency if not used recently
            skill["last_practice"] += 1
            if skill["last_practice"] > 10:  # Not used recently
                skill["proficiency"] *= skill["decay_rate"]
                skill["last_practice"] = 0
            
            return output
            
        return input_pattern
        
    def get_skill_proficiency(self, skill_name: str) -> float:
        """Get current proficiency of a skill."""
        if skill_name in self.procedural_memory:
            return self.procedural_memory[skill_name]["proficiency"]
        return 0.0


def create_custom_memory_systems():
    """Create and demonstrate custom memory systems."""
    print_header("4.1 Custom Memory Systems")
    
    # Create custom memory system instances
    memory_systems = {
        "TemporalMemory": TemporalMemory(capacity=500),
        "EmotionalMemory": EmotionalMemory(capacity=300),
        "ImplicitMemory": ImplicitMemory(capacity=200)
    }
    
    print("\nAvailable custom memory systems:")
    for name, system in memory_systems.items():
        print(f"  - {name}: {system.name}")
        print(f"    Capacity: {system.capacity}")
        print(f"    Current size: {system.size()}")
    
    # Demonstrate memory system functionality
    print("\n4.2 Memory System Demonstration:")
    
    temporal_mem = memory_systems["TemporalMemory"]
    emotional_mem = memory_systems["EmotionalMemory"]
    implicit_mem = memory_systems["ImplicitMemory"]
    
    # Temporal memory demonstration
    print("\nTemporalMemory:")
    temporal_mem.store_sequence("morning_routine", [0.1, 0.3, 0.7, 0.9], time_window=1.0)
    temporal_mem.store_sequence("evening_routine", [0.9, 0.7, 0.3, 0.1], time_window=2.0)
    
    completed = temporal_mem.complete_pattern([0.1, 0.3])
    print(f"  Completed pattern [0.1, 0.3]: {completed}")
    
    # Emotional memory demonstration
    print("\nEmotionalMemory:")
    emotional_mem.store_with_emotion("safe_place", [0.5, 0.5], valence=0.8, arousal=0.3, tags=["comfort", "safety"])
    emotional_mem.store_with_emotion("challenge", [0.8, 0.2], valence=-0.5, arousal=0.9, tags=["stress", "growth"])
    
    happy_memories = emotional_mem.retrieve_by_emotion(min_valence=0.0, max_valence=1.0)
    print(f"  Found {len(happy_memories)} positive memories")
    
    # Implicit memory demonstration
    print("\nImplicitMemory:")
    implicit_mem.learn_skill("typing", [0.1, 0.2, 0.3, 0.4, 0.5], proficiency=0.7)
    implicit_mem.learn_skill("navigation", [0.9, 0.8, 0.7, 0.6, 0.5], proficiency=0.5)
    
    input_pattern = [0.2, 0.3, 0.4, 0.5, 0.6]
    typed_output = implicit_mem.execute_skill("typing", input_pattern)
    navigated_output = implicit_mem.execute_skill("navigation", input_pattern)
    
    print(f"  Typing skill output: {typed_output}")
    print(f"  Navigation skill output: {navigated_output}")
    print(f"  Current typing proficiency: {implicit_mem.get_skill_proficiency('typing'):.2f}")
    
    return memory_systems


def memory_system_plugin_example():
    """Plugin architecture for memory systems."""
    print_header("4.3 Memory System Plugin Architecture")
    
    print("\nPlugin Architecture for Memory Systems:")
    print("1. MemoryPlugin Interface - Standardized API")
    print("2. Plugin Registry - Centralized management")
    print("3. Configuration Loading - Dynamic settings")
    print("4. Integration with Brain Systems - Seamless compatibility")
    
    # Memory system plugin registry
    memory_plugins = {}
    
    def register_memory_plugin(plugin_name: str, plugin_class):
        """Register a memory system plugin."""
        memory_plugins[plugin_name] = plugin_class
        
    def load_memory_plugin(plugin_name: str, **config) -> CustomMemorySystem:
        """Load and configure a memory system plugin."""
        if plugin_name in memory_plugins:
            plugin_class = memory_plugins[plugin_name]
            instance = plugin_class(**config)
            return instance
        else:
            raise ValueError(f"Unknown memory plugin: {plugin_name}")
    
    # Register example plugins
    def create_advanced_temporal_plugin(**config):
        """Plugin for advanced temporal memory."""
        return TemporalMemory(
            capacity=config.get('capacity', 1000),
            sequence_retention=config.get('sequence_retention', 0.9)
        )
        
    def create_advanced_emotional_plugin(**config):
        """Plugin for advanced emotional memory."""
        return EmotionalMemory(
            capacity=config.get('capacity', 500),
            valence_sensitivity=config.get('valence_sensitivity', 1.0)
        )
        
    register_memory_plugin("AdvancedTemporal", create_advanced_temporal_plugin)
    register_memory_plugin("AdvancedEmotional", create_advanced_emotional_plugin)
    
    # Load and configure plugins
    print(f"\nRegistered memory plugins: {list(memory_plugins.keys())}")
    
    advanced_temporal = load_memory_plugin("AdvancedTemporal", capacity=800, sequence_retention=0.95)
    advanced_emotional = load_memory_plugin("AdvancedEmotional", capacity=400, valence_sensitivity=1.2)
    
    print(f"\nLoaded plugins:")
    print(f"  - AdvancedTemporal capacity: {advanced_temporal.capacity}")
    print(f"  - AdvancedEmotional capacity: {advanced_emotional.capacity}")
    
    return memory_plugins


def memory_examples_summary():
    """Print summary of custom memory system examples."""
    print_header("Custom Memory System Examples Summary")
    
    print("\nThis section demonstrated:")
    print("✓ Custom memory system base class")
    print("✓ TemporalMemory - sequence storage and pattern completion")
    print("✓ EmotionalMemory - valenced associations and affect-guided retrieval")
    print("✓ ImplicitMemory - procedural skills and habits")
    print("✓ Plugin architecture for memory system extension")
    print("✓ Integration of custom systems with brain functionality")

# ============================================================================
# 5. Plugin Architecture Example
# ============================================================================

class PluginManager:
    """Manages loading and configuration of plugins."""
    
    def __init__(self):
        self.plugins = {}
        self.plugin_configs = {}
        self.plugin_instances = {}
        
    def register_plugin(self, plugin_name: str, plugin_class, config_path: str = None):
        """Register a plugin for loading."""
        self.plugins[plugin_name] = plugin_class
        
        if config_path:
            self.plugin_configs[plugin_name] = self._load_config(config_path)
            
    def load_plugin(self, plugin_name: str, **kwargs) -> Any:
        """Load and instantiate a plugin with configuration."""
        if plugin_name not in self.plugins:
            raise ValueError(f"Unknown plugin: {plugin_name}")
            
        plugin_class = self.plugins[plugin_name]
        
        # Get configuration
        config = self.plugin_configs.get(plugin_name, {})
        config.update(kwargs)
        
        # Create instance
        instance = plugin_class(**config)
        self.plugin_instances[plugin_name] = instance
        
        return instance
        
    def get_plugin(self, plugin_name: str) -> Any:
        """Get an already loaded plugin instance."""
        return self.plugin_instances.get(plugin_name)
        
    def _load_config(self, config_path: str) -> Dict:
        """Load plugin configuration from file."""
        try:
            with open(config_path, 'r') as f:
                return json.load(f)
        except Exception as e:
            logger.warning(f"Could not load config for {config_path}: {e}")
            return {}
class BrainRegionPlugin:
    """Plugin for extending brain regions."""
    
    def __init__(self, name: str, capabilities: List[str], neuron_count: int):
        self.name = name
        self.capabilities = capabilities
        self.neuron_count = neuron_count
        self.enabled = True
        self.custom_properties = {}
        
    def configure(self, **kwargs):
        """Configure plugin with custom properties."""
        self.custom_properties.update(kwargs)
        
    def get_region_config(self) -> Dict:
        """Get region configuration for integration."""
        return {
            "name": self.name,
            "neuron_count": self.neuron_count,
            "capabilities": self.capabilities,
            "custom_properties": self.custom_properties,
            "enabled": self.enabled
        }
class PlasticityRulePlugin:
    """Plugin for extending plasticity rules."""
    
    def __init__(self, rule_type: str, parameters: Dict):
        self.rule_type = rule_type
        self.parameters = parameters
        self.name = f"{rule_type}_plugin"
        
    def get_rule_config(self) -> Dict:
        """Get rule configuration for integration."""
        return {
            "rule_type": self.rule_type,
            "parameters": self.parameters,
            "description": self._get_description()
        }
        
    def _get_description(self) -> str:
        """Get human-readable description of the rule."""
        descriptions = {
            "hebbian": "Classical Hebbian learning - neurons that fire together wire together",
            "oja": "Oja's rule - normalized Hebbian learning for stable weight updates",
            "stp": "Spike-timing dependent plasticity - precise temporal learning",
            "bcm": "Bienenstock-Cooper-Munro - sliding threshold learning"
        }
        return descriptions.get(self.rule_type, "Custom plasticity rule")
class NeuromodulatorPlugin:
    """Plugin for extending neuromodulators."""
    
    def __init__(self, mod_name: str, target_neuron_types: List[str]):
        self.name = mod_name
        self.target_neuron_types = target_neuron_types
        self.level = 0.5
        self.effects = {}
        self.signaling_pathways = []
        
    def add_signaling_pathway(self, pathway_name: str, effect_config: Dict):
        """Add a neuromodulatory signaling pathway."""
        self.signaling_pathways.append({
            "name": pathway_name,
            "effect_config": effect_config
        })
        
    def get_modulator_config(self) -> Dict:
        """Get modulator configuration for integration."""
        return {
            "name": self.name,
            "target_neuron_types": self.target_neuron_types,
            "level": self.level,
            "effects": self.effects,
            "signaling_pathways": self.signaling_pathways
        }
class MemorySystemPlugin:
    """Plugin for extending memory systems."""
    
    def __init__(self, mem_name: str, storage_type: str, capacity: int):
        self.name = mem_name
        self.storage_type = storage_type
        self.capacity = capacity
        self.encoding_strategy = "standard"
        self.retrieval_strategy = "exact"
        self.consolidation_rule = "standard"
        
    def configure_encoding(self, strategy: str, **kwargs):
        """Configure memory encoding strategy."""
        self.encoding_strategy = strategy
        self.encoding_config = kwargs
        
    def configure_retrieval(self, strategy: str, **kwargs):
        """Configure memory retrieval strategy."""
        self.retrieval_strategy = strategy
        self.retrieval_config = kwargs
        
    def get_memory_config(self) -> Dict:
        """Get memory system configuration for integration."""
        return {
            "name": self.name,
            "storage_type": self.storage_type,
            "capacity": self.capacity,
            "encoding_strategy": self.encoding_strategy,
            "encoding_config": getattr(self, 'encoding_config', {}),
            "retrieval_strategy": self.retrieval_strategy,
            "retrieval_config": getattr(self, 'retrieval_config', {}),
            "consolidation_rule": self.consolidation_rule
        }

def plugin_architecture_example():
    """Demonstrate plugin architecture for NLM extensions."""
    print_header("5.1 Plugin Architecture Demonstration")
    
    # Initialize plugin manager
    plugin_manager = PluginManager()
    
    print("\nPlugin Manager initialized:")
    print("  - Plugin registry: Centralized management")
    print("  - Configuration loading: JSON-based settings")
    print("  - Dynamic instantiation: Runtime plugin creation")
    print("  - Instance caching: Reuse configured instances")
    
    # Register brain region plugins
    print("\n5.2 Brain Region Plugins:")
    
    def create_advanced_visual_plugin(**config):
        """Plugin for advanced visual processing region."""
        return BrainRegionPlugin(
            name="AdvancedVisualRegion",
            capabilities=["edge_detection", "color_processing", "object_recognition"],
            neuron_count=config.get('neuron_count', 500)
        )
        
    def create_advanced_auditory_plugin(**config):
        """Plugin for advanced auditory processing region."""
        return BrainRegionPlugin(
            name="AdvancedAuditoryRegion",
            capabilities=["frequency_analysis", "pattern_recognition", "sound_localization"],
            neuron_count=config.get('neuron_count', 300)
        )
    
    plugin_manager.register_plugin("AdvancedVisual", create_advanced_visual_plugin)
    plugin_manager.register_plugin("AdvancedAuditory", create_advanced_auditory_plugin)
    
    # Load brain region plugins
    visual_region = plugin_manager.load_plugin(
        "AdvancedVisual",
        neuron_count=750,
        custom_focus=0.8,
        receptive_field_size=12.0
    )
    
    auditory_region = plugin_manager.load_plugin(
        "AdvancedAuditory",
        neuron_count=450,
        custom_sensitivity=1.2,
        frequency_range=[20, 20000]
    )
    
    print(f"  Loaded visual region: {visual_region.get_region_config()}")
    print(f"  Loaded auditory region: {auditory_region.get_region_config()}")
    
    # Register plasticity rule plugins
    print("\n5.3 Plasticity Rule Plugins:")
    
    def create_advanced_hebbian_plugin(**config):
        """Plugin for enhanced Hebbian learning."""
        return PlasticityRulePlugin(
            rule_type="hebbian",
            parameters={
                "learning_rate": config.get('learning_rate', 0.01),
                "normalization": config.get('normalization', True),
                "momentum": config.get('momentum', 0.1)
            }
        )
        
    def create_stp_plugin(**config):
        """Plugin for spike-timing dependent plasticity."""
        return PlasticityRulePlugin(
            rule_type="stp",
            parameters={
                "learning_window": config.get('learning_window', 20.0),
                "max_weight_change": config.get('max_weight_change', 0.1),
                "stp_type": config.get('stp_type', "symmetric")
            }
        )
    
    plugin_manager.register_plugin("AdvancedHebbian", create_advanced_hebbian_plugin)
    plugin_manager.register_plugin("STPLearning", create_stp_plugin)
    
    # Load plasticity rule plugins
    advanced_hebbian = plugin_manager.load_plugin(
        "AdvancedHebbian",
        learning_rate=0.02,
        normalization=True,
        momentum=0.15
    )
    
    stp_rule = plugin_manager.load_plugin(
        "STPLearning",
        learning_window=25.0,
        max_weight_change=0.15,
        stp_type="asymmetric"
    )
    
    print(f"  Loaded advanced Hebbian: {advanced_hebbian.get_rule_config()}")
    print(f"  Loaded STP rule: {stp_rule.get_rule_config()}")
    
    # Register neuromodulator plugins
    print("\n5.4 Neuromodulator Plugins:")
    
    def create_advanced_dopamine_plugin(**config):
        """Plugin for enhanced dopamine signaling."""
        mod = NeuromodulatorPlugin(
            mod_name="AdvancedDopamine",
            target_neuron_types=["dopaminergic_receptors", "learning_neurons"]
        )
        mod.add_signaling_pathway("reward_prediction_error", {"gain": 2.0, "time_constant": 0.5})
        mod.add_signaling_pathway("reinforcement_learning", {"gain": 1.5, "adaptation": 0.1})
        return mod
        
    def create_advanced_curiosity_plugin(**config):
        """Plugin for enhanced curiosity signaling."""
        mod = NeuromodulatorPlugin(
            mod_name="AdvancedCuriosity",
            target_neuron_types=["exploration_neurons", "novelty_detectors"]
        )
        mod.add_signaling_pathway("novelty_detection", {"threshold": 0.7, "learning_rate": 0.05})
        mod.add_signaling_pathway("intrinsic_motivation", {"base_level": 0.4, "decay_rate": 0.9})
        return mod
    
    plugin_manager.register_plugin("AdvancedDopamine", create_advanced_dopamine_plugin)
    plugin_manager.register_plugin("AdvancedCuriosity", create_advanced_curiosity_plugin)
    
    # Load neuromodulator plugins
    advanced_dopamine = plugin_manager.load_plugin(
        "AdvancedDopamine",
        signaling_pathways=[{"name": "enhanced_reward", "gain": 3.0}]
    )
    
    advanced_curiosity = plugin_manager.load_plugin(
        "AdvancedCuriosity",
        signaling_pathways=[{"name": "boosted_novelty", "threshold": 0.8}]
    )
    
    print(f"  Loaded advanced dopamine: {advanced_dopamine.get_modulator_config()['name']}")
    print(f"    Pathways: {[p['name'] for p in advanced_dopamine.signaling_pathways]}")
    print(f"  Loaded advanced curiosity: {advanced_curiosity.get_modulator_config()['name']}")
    print(f"    Pathways: {[p['name'] for p in advanced_curiosity.signaling_pathways]}")
    
    # Register memory system plugins
    print("\n5.5 Memory System Plugins:")
    
    def create_advanced_temporal_plugin(**config):
        """Plugin for advanced temporal memory."""
        return MemorySystemPlugin(
            mem_name="AdvancedTemporalMemory",
            storage_type="hierarchical_temporal",
            capacity=config.get('capacity', 2000)
        )
        
    def create_advanced_social_plugin(**config):
        """Plugin for advanced social memory."""
        return MemorySystemPlugin(
            mem_name="AdvancedSocialMemory",
            storage_type="network_based",
            capacity=config.get('capacity', 1500)
        )
    
    plugin_manager.register_plugin("AdvancedTemporalMemory", create_advanced_temporal_plugin)
    plugin_manager.register_plugin("AdvancedSocialMemory", create_advanced_social_plugin)
    
    # Load memory system plugins
    advanced_temporal = plugin_manager.load_plugin(
        "AdvancedTemporalMemory",
        capacity=3000,
        encoding_strategy="hierarchical",
        retrieval_strategy="pattern_completion"
    )
    
    advanced_social = plugin_manager.load_plugin(
        "AdvancedSocialMemory",
        capacity=2000,
        encoding_strategy="network_embedding",
        retrieval_strategy="social_similarity"
    )
    
    print(f"  Loaded advanced temporal memory: {advanced_temporal.get_memory_config()}")
    print(f"  Loaded advanced social memory: {advanced_social.get_memory_config()}")
    
    print("\n5.6 Plugin Management Summary:")
    print(f"  Total registered plugins: {len(plugin_manager.plugins)}")
    print(f"  Loaded plugin instances: {len(plugin_manager.plugin_instances)}")
    print("  Plugin types:")
    print("    - BrainRegionPlugin: Custom neural regions")
    print("    - PlasticityRulePlugin: Novel learning rules")
    print("    - NeuromodulatorPlugin: Specialized neuromodulators")
    print("    - MemorySystemPlugin: Custom memory architectures")
    
    return plugin_manager


def plugin_examples_summary():
    """Print summary of plugin architecture examples."""
    print_header("Plugin Architecture Examples Summary")
    
    print("\nThis section demonstrated:")
    print("✓ Plugin Manager - centralized plugin lifecycle management")
    print("✓ BrainRegionPlugin - custom neural region extensions")
    print("✓ PlasticityRulePlugin - custom learning rule implementations")
    print("✓ NeuromodulatorPlugin - custom neuromodulator types")
    print("✓ MemorySystemPlugin - custom memory system architectures")
    print("✓ Plugin registration and loading system")
    print("✓ Configuration management for plugins")

# ============================================================================
# Main execution
# ============================================================================

def main():
    """Run all custom component example demonstrations."""
    print("=" * 80)
    print("NLM Custom Component Examples")
    print("Demonstrating brain region, plasticity rule, neuromodulator, and memory system extensions")
    print("=" * 80)
    
    # Run custom brain region examples
    custom_foraging_region, brain1 = create_custom_foraging_region()
    custom_social_region, brain2 = create_custom_social_region()
    factory_regions = custom_region_factory()
    custom_region_examples_summary()
    
    # Run custom plasticity rule examples
    plasticity_rules = create_custom_plasticity_rules()
    factory_rules = plasticity_rule_factory()
    neuromodulator_effects = adaptive_plasticity_example()
    plasticity_examples_summary()
    
    # Run custom neuromodulator examples
    neuromodulators = create_custom_neuromodulators()
    neuromodulator_plugins = neuromodulator_plugin_example()
    neuromodulator_examples_summary()
    
    # Run custom memory system examples
    memory_systems = create_custom_memory_systems()
    memory_plugins = memory_system_plugin_example()
    memory_examples_summary()
    
    # Run plugin architecture example
    plugin_manager = plugin_architecture_example()
    plugin_examples_summary()
    
    print("\n" + "=" * 80)
    print("All custom component example demonstrations completed successfully!")
    print("=" * 80)
    
    print("\nPlugin Architecture Features Demonstrated:")
    print("1. Dynamic Plugin Registration - Runtime plugin discovery")
    print("2. Configuration Management - JSON-based plugin settings")
    print("3. Instance Caching - Efficient plugin reuse")
    print("4. Type-Specific Plugins - Specialized implementations")
    print("5. Integration Points - Brain, learning, and memory systems")
    
    print("\nCustom Component Capabilities:")
    print("• Specialized brain regions for specific functions")
    print("• Novel learning rules with neuromodulator adaptation")
    print("• Custom neuromodulators with unique signaling pathways")
    print("• Advanced memory systems with custom architectures")
    print("• Plugin architecture for runtime extension")

if __name__ == "__main__":
    main()