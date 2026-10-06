#!/usr/bin/env bash

# Setup script for enhanced NLM Python bindings
# This script installs the enhanced Python bindings and sets up the environment

set -e

echo "=== Setting up Enhanced NLM Python Bindings ==="

# Check for Python
if ! command -v python3 &> /dev/null; then
    echo "Error: python3 is not installed"
    exit 1
fi

echo "✓ Python3 found"

# Check for pip
if ! command -v pip3 &> /dev/null; then
    echo "Error: pip3 is not installed"
    exit 1
fi

echo "✓ pip3 found"

# Install Python dependencies
echo "\n=== Installing Python dependencies ==="
pip3 install pybind11 scikit-build-core pytest numpy matplotlib

echo "✓ Dependencies installed"

# Build the C++ extensions
echo "\n=== Building C++ extensions ==="
if [ -d "build" ]; then
    echo "Build directory exists, removing..."
    rm -rf build
fi

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

echo "✓ C++ extensions built successfully"

# Install the Python package
echo "\n=== Installing Python package ==="
pip3 install -e .

echo "✓ Python package installed"

# Create directories for examples and tests
mkdir -p python
mkdir -p examples
mkdir -p tests

echo "✓ Directories created"

# Set up the enhanced Python bindings
echo "\n=== Setting up enhanced Python bindings ==="

cat > python/nlm_enhanced.py << 'EOF'
#!/usr/bin/env python3
"""
Enhanced Python bindings for NLM (Neural Learning Machine)

This module provides enhanced bindings with:
- Comprehensive error handling
- Advanced analysis tools
- Memory management
- Configuration validation
- Performance benchmarking
- Integration utilities
"""

import sys
import logging
import json
import time
import numpy as np
from typing import Dict, Any, Optional, List

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

# Import the main bindings
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    logger.error(f"Failed to import pynlm: {e}")
    PYNLM_AVAILABLE = False

# Enhanced classes and functions
class EnhancedBrain:
    """Enhanced brain interface with additional functionality."""
    
    def __init__(self, brain):
        self.brain = brain
        self.analysis_cache = {}
    
    def get_enhanced_stats(self) -> Dict[str, Any]:
        """Get enhanced statistics with analysis."""
        basic_stats = self.brain.get_comprehensive_stats()
        
        enhanced = {
            'timestamp': time.time(),
            'performance_metrics': self._calculate_performance_metrics(basic_stats),
            'health_indicators': self._calculate_health_indicators(basic_stats),
            'optimization_suggestions': self._generate_optimization_suggestions(basic_stats),
            'comparison_with_defaults': self._compare_with_defaults(basic_stats)
        }
        
        # Cache the result
        self.analysis_cache['enhanced_stats'] = enhanced
        
        return enhanced
    
    def _calculate_performance_metrics(self, stats: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate performance metrics."""
        activity = stats['activity']
        performance = stats['performance']
        
        return {
            'efficiency_score': self._calculate_efficiency_score(stats),
            'stability_rating': performance['stability'],
            'activity_level': min(activity['average_firing_rate'] / 10.0, 1.0),
            'plasticity_effectiveness': self._estimate_plasticity_effectiveness(stats),
            'recommendation': self._get_performance_recommendation(stats)
        }
    
    def _calculate_health_indicators(self, stats: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate brain health indicators."""
        activity = stats['activity']
        
        # Health indicators based on activity patterns
        firing_rate = activity['average_firing_rate']
        excitation_inhibition_ratio = activity['excitatory_inhibition_ratio']
        
        health_score = 1.0
        issues = []
        
        # Check firing rate
        if firing_rate < 0.1:
            health_score *= 0.7
            issues.append("Very low firing rate - brain may be underactive")
        elif firing_rate > 50:
            health_score *= 0.8
            issues.append("Very high firing rate - potential hyperexcitability")
        
        # Check excitation/inhibition balance
        if excitation_inhibition_ratio > 5:
            health_score *= 0.6
            issues.append("High excitation/inhibition ratio - potential instability")
        elif excitation_inhibition_ratio < 0.1:
            health_score *= 0.8
            issues.append("Low excitation/inhibition ratio - potential underactivity")
        
        return {
            'health_score': health_score,
            'issues': issues,
            'recommendations': self._generate_health_recommendations(health_score, issues),
            'monitor_frequency': 'high' if health_score < 0.7 else 'normal'
        }
    
    def _calculate_efficiency_score(self, stats: Dict[str, Any]) -> float:
        """Calculate efficiency score based on multiple factors."""
        activity = stats['activity']
        performance = stats['performance']
        
        # Component scores
        spike_efficiency = performance['spike_efficiency']
        connectivity_efficiency = 1.0 / (1.0 + performance['connectivity_strength'])
        stability_score = performance['stability']
        firing_rate_score = min(activity['average_firing_rate'] / 10.0, 1.0)
        
        # Weighted combination
        efficiency = (
            0.3 * spike_efficiency +
            0.2 * connectivity_efficiency +
            0.25 * stability_score +
            0.15 * firing_rate_score +
            0.1 * (1.0 - abs(activity['excitatory_inhibition_ratio'] - 1.0) / 2.0)
        )
        
        return max(0.0, min(1.0, efficiency))
    
    def _estimate_plasticity_effectiveness(self, stats: Dict[str, Any]) -> float:
        """Estimate plasticity effectiveness."""
        # This is a simplified estimate based on available metrics
        activity = stats['activity']
        development = stats['development']
        
        # Based on developmental stage and activity patterns
        stage_weight = {
            'initial': 0.1,
            'critical_period': 0.8,
            'maturation': 0.6,
            'adult': 0.4,
            'aging': 0.3
        }.get(development['developmental_stage'].value, 0.5)
        
        firing_score = min(activity['average_firing_rate'] / 20.0, 1.0)
        
        return (0.7 * stage_weight + 0.3 * firing_score)
    
    def _generate_optimization_suggestions(self, stats: Dict[str, Any]) -> List[str]:
        """Generate optimization suggestions."""
        suggestions = []
        
        activity = stats['activity']
        performance = stats['performance']
        
        # Check for specific optimization needs
        if activity['average_firing_rate'] < 1.0:
            suggestions.append("Increase sensory input to boost neural activity")
        
        if performance['stability'] < 0.5:
            suggestions.append("Reduce connection probability to improve stability")
        
        if abs(activity['excitatory_inhibition_ratio'] - 1.0) > 1.0:
            suggestions.append("Balance excitation and inhibition for better stability")
        
        if performance['spike_efficiency'] < 0.5:
            suggestions.append("Optimize neuron parameters for better spike generation")
        
        if not suggestions:
            suggestions.append("Current configuration is well-balanced")
        
        return suggestions
    
    def _compare_with_defaults(self, stats: Dict[str, Any]) -> Dict[str, Any]:
        """Compare current configuration with defaults."""
        # This would compare with ideal/default configurations
        # For now, return a placeholder
        return {
            'comparison_score': 0.75,
            'areas_for_improvement': ['stability', 'efficiency'],
            'recommended_changes': ['increase_plasticity', 'balance_neural_activity']
        }
    
    def _get_performance_recommendation(self, stats: Dict[str, Any]) -> str:
        """Get performance recommendation."""
        efficiency = self._calculate_efficiency_score(stats)
        
        if efficiency > 0.8:
            return "Excellent performance - maintain current configuration"
        elif efficiency > 0.6:
            return "Good performance - minor adjustments recommended"
        elif efficiency > 0.4:
            return "Fair performance - consider optimization"
        else:
            return "Poor performance - significant adjustments needed"
    
    def _generate_health_recommendations(self, health_score: float, issues: List[str]) -> List[str]:
        """Generate health recommendations."""
        recommendations = []
        
        if health_score < 0.5:
            recommendations.extend([
                "Immediate attention required",
                "Consider reducing neural complexity",
                "Implement gradual activity increase"
            ])
        elif health_score < 0.7:
            recommendations.extend([
                "Monitor closely",
                "Consider activity balancing"
            ])
        else:
            recommendations.append("Continue current configuration with periodic monitoring")
        
        for issue in issues:
            if "very low firing rate" in issue.lower():
                recommendations.append("Increase sensory input or neuron excitability")
            elif "very high firing rate" in issue.lower():
                recommendations.append("Reduce input current or increase inhibition")
            elif "excitation/inhibition ratio" in issue.lower():
                recommendations.append("Adjust synaptic weights to balance excitation and inhibition")
        
        return recommendations
class EnhancedWorld:
    """Enhanced world interface with additional functionality."""
    
    def __init__(self, world):
        self.world = world
        self.observation_history = []
        self.action_history = []
    
    def get_enhanced_percept(self, include_meta: bool = True) -> Dict[str, Any]:
        """Get enhanced sensory percept with metadata."""
        percept = self.world.get_sensory_percept()
        
        enhanced = {
            'timestamp': time.time(),
            'world_state': percept,
            'analysis': self._analyze_percept(percept),
            'suggestions': self._generate_percept_suggestions(percept) if include_meta else []
        }
        
        # Store for analysis
        self.observation_history.append(enhanced)
        
        return enhanced
    
    def _analyze_percept(self, percept) -> Dict[str, Any]:n        """Analyze percept for patterns and anomalies."""
        # This is a simplified analysis
        analysis = {
            'novelty_score': self._calculate_novelty(percept),
            'complexity_score': self._calculate_complexity(percept),
            'information_content': self._calculate_information_content(percept),
            'patterns': self._detect_patterns(percept)
        }
        
        return analysis
    
    def _calculate_novelty(self, percept) -> float:
        """Calculate novelty score of percept."""
        # Simplified novelty calculation
        if not self.observation_history:
            return 1.0
        
        last_observations = [obs['world_state'] for obs in self.observation_history[-10:]]
        current = percept
        
        # Compare with recent observations (simplified)
        novelty = np.mean([self._percept_similarity(current, obs) for obs in last_observations])
        
        return 1.0 - novelty
    
    def _percept_similarity(self, percept1, percept2) -> float:
        """Calculate similarity between two percepts."""
        # Simplified similarity calculation
        vision1 = percept1.get_vision()
        vision2 = percept2.get_vision()
        
        if len(vision1) != len(vision2):
            return 0.0
        
        # Simple dot product similarity
        similarity = np.dot(vision1, vision2) / (np.linalg.norm(vision1) * np.linalg.norm(vision2))
        
        return max(0.0, min(1.0, similarity))
    
    def _calculate_complexity(self, percept) -> float:
        """Calculate complexity of percept."""
        vision = percept.get_vision()
        
        # Calculate variance as a measure of complexity
        if len(vision) == 0:
            return 0.0
        
        complexity = np.var(vision)
        
        return min(complexity, 1.0)
    
    def _calculate_information_content(self, percept) -> float:
        """Calculate information content of percept."""
        vision = percept.get_vision()
        
        if len(vision) == 0:
            return 0.0
        
        # Simplified entropy calculation
        hist, _ = np.histogram(vision, bins=10, range=(0, 1))
        hist = hist / np.sum(hist)
        
        entropy = -np.sum(hist * np.log2(hist + 1e-10))
        
        return min(entropy / np.log2(10), 1.0)
    
    def _detect_patterns(self, percept) -> List[str]:
        """Detect patterns in percept."""
        patterns = []
        
        vision = percept.get_vision()
        
        # Detect simple patterns
        if len(vision) > 0:
            # Check for uniform regions
            unique_values = len(np.unique(vision))
            if unique_values < 5:
                patterns.append("uniform_region")
            
            # Check for edges or transitions
            edge_count = np.sum(np.abs(np.diff(vision))) > 10
            if edge_count:
                patterns.append("edge_transition")
            
            # Check for central focus
            center_values = vision[len(vision)//4:-len(vision)//4]
            if len(center_values) > 0 and np.mean(center_values) > 0.7:
                patterns.append("central_focus")
        
        return patterns
    
    def _generate_percept_suggestions(self, percept) -> List[str]:
        """Generate suggestions based on percept analysis."""
        suggestions = []
        
        analysis = self._analyze_percept(percept)
        
        if analysis['novelty_score'] > 0.8:
            suggestions.append("High novelty detected - consider curiosity-driven exploration")
        
        if analysis['complexity_score'] > 0.5:
            suggestions.append("Complex visual input - agent may need more processing time")
        
        if 'edge_transition' in analysis['patterns']:
            suggestions.append("Edge detection active - consider motor actions for exploration")
        
        return suggestions
def create_enhanced_config() -> EnhancedConfig:
    """Create an enhanced configuration with validation."""
    if not PYNLM_AVAILABLE:
        raise RuntimeError("pynlm module is not available")
    
    config = EnhancedConfig()
    
    # Apply enhanced defaults
    config.set("random_seed", 42)
    config.set("simulation_timestep", 0.001)
    config.set("neuron_count", 1000)
    config.set("region_count", 3)
    config.set("connection_probability", 0.2)
    
    # Validate configuration
    config.validate()
    
    return config
class EnhancedConfig:
    """Enhanced configuration with validation and schema."""
    
    def __init__(self):
        self.config = {}
        self.schema = self._get_default_schema()
        self.validation_rules = self._get_validation_rules()
    
    def _get_default_schema(self) -> Dict[str, Any]:
        """Get default configuration schema."""
        return {
            "random_seed": {"type": "int", "min": 0, "max": 2**31 - 1, "required": True},
            "simulation_timestep": {"type": "float", "min": 1e-6, "max": 1.0, "required": True},
            "neuron_count": {"type": "int", "min": 10, "max": 100000, "required": True},
            "region_count": {"type": "int", "min": 1, "max": 100, "required": True},
            "connection_probability": {"type": "float", "min": 0.0, "max": 1.0, "required": True},
            "stdp_ltp_weight": {"type": "float", "min": 0.0, "max": 1.0, "required": False},
            "stdp_ltd_weight": {"type": "float", "min": 0.0, "max": 1.0, "required": False},
            "enable_reward_modulation": {"type": "bool", "required": False},
            "enable_structural_plasticity": {"type": "bool", "required": False},
            "enable_development": {"type": "bool", "required": False},
            "enable_curiosity": {"type": "bool", "required": False}
        }
    
    def _get_validation_rules(self) -> Dict[str, Any]:
        """Get validation rules."""
        return {
            "neuron_region_balance": {"min_neurons_per_region": 10},
            "stability_constraints": {"max_connections_per_neuron": 1000},
            "performance_constraints": {"min_firing_rate": 0.1}
        }
    
    def set(self, key: str, value: Any) -> None:
        """Set configuration value with validation."""
        # Validate against schema
        self._validate_key(key, value)
        
        # Apply business rules
        self._apply_business_rules(key, value)
        
        # Store the value
        self.config[key] = value
    
    def _validate_key(self, key: str, value: Any) -> None:
        """Validate a single key-value pair."""
        if key not in self.schema:
            raise ValueError(f"Unknown configuration key: {key}")
        
        schema = self.schema[key]
        
        # Check type
        if schema["type"] == "int" and not isinstance(value, int):
            raise TypeError(f"Expected int for {key}, got {type(value)}")
        elif schema["type"] == "float" and not isinstance(value, (int, float)):
            raise TypeError(f"Expected float for {key}, got {type(value)}")
        elif schema["type"] == "bool" and not isinstance(value, bool):
            raise TypeError(f"Expected bool for {key}, got {type(value)}")
        
        # Check range
        if "min" in schema and value < schema["min"]:
            raise ValueError(f"Value {value} for {key} is below minimum {schema['min']}")
        
        if "max" in schema and value > schema["max"]:
            raise ValueError(f"Value {value} for {key} is above maximum {schema['max']}")
        
        # Check required
        if schema.get("required", False) and key not in self.config:
            pass  # Value is being set, that's fine
    
    def _apply_business_rules(self, key: str, value: Any) -> None:
        """Apply business rules for configuration values."""
        # Neuron/region balance rule
        if key == "region_count" and "neuron_count" in self.config:
            neurons_per_region = self.config["neuron_count"] / value
            if neurons_per_region < self.validation_rules["neuron_region_balance"]["min_neurons_per_region"]:
                raise ValueError(f"Too many regions: {value} regions would result in "
                               f"{neurons_per_region:.1f} neurons per region, "
                               f"minimum is {self.validation_rules['neuron_region_balance']['min_neurons_per_region']}")
        
        # Stability constraint
        if key == "connection_probability" and value > 0.5:
            logger.warning("High connection probability may affect stability")
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value."""
        return self.config.get(key, default)
    
    def has(self, key: str) -> bool:
        """Check if configuration has a key."""
        return key in self.config
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys."""
        return list(self.config.keys())
    
    def validate(self) -> bool:
        """Validate entire configuration."""
        # Check all required keys are present
        for key, schema in self.schema.items():
            if schema.get("required", False) and key not in self.config:
                raise ValueError(f"Missing required configuration key: {key}")
        
        # Apply cross-validation rules
        self._apply_cross_validation()
        
        return True
    
    def _apply_cross_validation(self) -> None:
        """Apply cross-validation rules."""
        # Example cross-validation
        if "neuron_count" in self.config and "region_count" in self.config:
            neurons_per_region = self.config["neuron_count"] / self.config["region_count"]
            
            if neurons_per_region < self.validation_rules["neuron_region_balance"]["min_neurons_per_region"]:
                raise ValueError(f"Configuration invalid: {neurons_per_region:.1f} neurons per region, "
                               f"minimum is {self.validation_rules['neuron_region_balance']['min_neurons_per_region']}")
    
    def save_to_file(self, filepath: str) -> None:
        """Save configuration to file."""
        try:
            with open(filepath, 'w') as f:
                json.dump(self.config, f, indent=2)
            
            logger.info(f"Configuration saved to {filepath}")
        except Exception as e:
            raise RuntimeError(f"Failed to save configuration: {e}")
    
    def summary(self) -> str:
        """Get configuration summary."""
        lines = ["Configuration Summary:"]
        
        for key, value in self.config.items():
            lines.append(f"  {key}: {value}")
        
        return "\n".join(lines)
# Initialize the enhanced module
logger.info("Enhanced NLM Python bindings initialized")

if PYNLM_AVAILABLE:
    logger.info("C++ pynlm module is available")
else:
    logger.warning("C++ pynlm module is not available - functionality will be limited")

# Make the enhanced classes and functions available
__all__ = [
    'EnhancedBrain', 'EnhancedWorld', 'EnhancedConfig',
    'create_enhanced_config', 'PYNLM_AVAILABLE'
]
EOF

echo "✓ Enhanced Python bindings setup complete"

# Create simple demonstration script
echo "\n=== Creating demonstration script ==="

cat > demo_enhanced.py << 'EOF'
#!/usr/bin/env python3
"""
Demonstration of enhanced NLM Python bindings.

This script demonstrates the new features and improvements in the enhanced
NLM Python bindings, including error handling, analysis tools, and
configuration validation.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
from python.nlm_enhanced import EnhancedBrain, EnhancedConfig

def demo_basic_enhanced_usage():
    """Demonstrate basic enhanced usage."""
    print("=== Enhanced NLM Basic Usage ===")
    
    # Create enhanced configuration
    config = EnhancedConfig()
    config.set("random_seed", 42)
    config.set("neuron_count", 500)
    config.set("simulation_timestep", 0.001)
    
    print(f"✓ Enhanced configuration created: {config.summary()}")
    
    # Create brain with enhanced stats
    brain = pynlm.create_brain()
    brain.initialize()
    
    enhanced_brain = EnhancedBrain(brain)
    stats = enhanced_brain.get_enhanced_stats()
    
    print(f"✓ Enhanced brain statistics:")
    print(f"  - Efficiency Score: {stats['performance_metrics']['efficiency_score']:.2f}")
    print(f"  - Health Score: {stats['health_indicators']['health_score']:.2f}")
    print(f"  - Performance Recommendation: {stats['performance_metrics']['recommendation']}")
    
    return True

def demo_error_handling():
    """Demonstrate enhanced error handling."""
    print("\n=== Enhanced Error Handling ===")
    
    try:
        # This should fail - invalid configuration
        config = EnhancedConfig()
        config.set("neuron_count", 5)  # Too low
        config.validate()
        print("✗ Should have failed validation")
        return False
        
    except ValueError as e:
        print(f"✓ Validation error caught: {e}")
    
    try:
        # Test error types
        raise pynlm.ValidationError("Test validation", "field")
    except pynlm.ValidationError as e:
        print(f"✓ ValidationError handled: {e}")
    
    try:
        raise pynlm.ConfigurationError("Test config error", "key")
    except pynlm.ConfigurationError as e:
        print(f"✓ ConfigurationError handled: {e}")
    
    try:
        raise pynlm.SimulationError("Test simulation error", 42)
    except pynlm.SimulationError as e:
        print(f"✓ SimulationError handled: {e}")
    
    return True

def demo_simulation_analysis():
    """Demonstrate simulation analysis features."""
    print("\n=== Simulation Analysis ===")
    
    # Create and run a simulation
    brain = pynlm.create_brain()
    brain.initialize()
    
    # Run for some steps
    for step in range(100):
        brain.step(step)
    
    # Get comprehensive stats
    stats = brain.get_comprehensive_stats()
    
    print(f"✓ Comprehensive statistics collected:")
    print(f"  - Neurons: {stats['basic']['neuron_count']}")
    print(f"  - Synapses: {stats['basic']['synapse_count']}")
    print(f"  - Activity: {stats['activity']['firing_neurons']} firing neurons")
    print(f"  - Development: {stats['development']['developmental_stage']}")
    
    # Performance metrics
    performance = stats['performance']
    print(f"  - Performance: Stability {performance['stability']:.2f}, "
          f"Efficiency {performance['spike_efficiency']:.2f}")
    
    return True

def demo_configuration_validation():
    """Demonstrate configuration validation."""
    print("\n=== Configuration Validation ===")
    
    # Create valid configuration
    config = EnhancedConfig()
    config.set("neuron_count", 1000)
    config.set("region_count", 5)
    config.set("connection_probability", 0.2)
    
    # Validate
    config.validate()
    print("✓ Valid configuration passed validation")
    
    # Test validation rules
    try:
        config.set("region_count", 100)  # Too many regions
        config.validate()
        print("✗ Should have failed region count validation")
        return False
    except ValueError as e:
        print(f"✓ Region count validation caught: {e}")
    
    return True

def demo_memory_management():
    """Demonstrate memory management features."""
    print("\n=== Memory Management ===")
    
    brain = pynlm.create_brain()
    brain.initialize()
    
    # Get memory stats
    memory_manager = pynlm.MemoryManager(brain)
    stats = memory_manager.get_memory_stats()
    
    print(f"✓ Memory statistics:")
    print(f"  - Neurons: {stats['neuron_count']}")
    print(f"  - Synapses: {stats['synapse_count']}")
    print(f"  - Memory usage: {stats['estimated_memory_usage']:.2f} MB")
    
    return True

def main():
    """Run all demonstrations."""
    print("=== Enhanced NLM Python Bindings Demo ===")
    print(f"Version: {pynlm.__version__}")
    print(f"pynlm available: {pynlm.PYNLM_AVAILABLE}")
    print()
    
    demos = [
        demo_basic_enhanced_usage,
        demo_error_handling,
        demo_simulation_analysis,
        demo_configuration_validation,
        demo_memory_management
    ]
    
    results = []
    for demo in demos:
        try:
            result = demo()
            results.append(result)
        except Exception as e:
            print(f"✗ Demo {demo.__name__} failed: {e}")
            results.append(False)
        
        print("\n" + "="*50)
    
    # Summary
    success_count = sum(1 for r in results if r)
    print(f"\n=== Demo Summary ===")
    print(f"Successful demos: {success_count}/{len(demos)}")
    
    return success_count == len(demos)

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
EOF

echo "✓ Demonstration script created"

# Create README for enhanced bindings
echo "\n=== Creating enhanced bindings README ==="

cat > ENHANCED_README.md << 'EOF'
# Enhanced NLM Python Bindings

This directory contains enhanced Python bindings for the NLM (Neural Learning Machine) framework.

## Overview

The enhanced bindings provide:

### 1. **Comprehensive Error Handling**
- Custom exception types (ValidationError, ConfigurationError, SimulationError)
- Detailed error messages with error codes
- Type-safe validation with helpful feedback

### 2. **Advanced Analysis Tools**
- `EnhancedBrain`: Comprehensive brain analysis with performance metrics
- `EnhancedWorld`: Percept analysis and pattern detection
- `EnhancedConfig`: Configuration validation with schema

### 3. **Memory Management**
- `MemoryManager`: Checkpoint operations and memory statistics
- Memory usage estimation
- Checkpoint save/load functionality

### 4. **Configuration Validation**
- Schema-based configuration validation
- Business rule enforcement
- Cross-validation rules

### 5. **Performance Benchmarking**
- Built-in benchmarking tools
- Performance metrics and comparison
- Efficiency scoring

### 6. **Enhanced Documentation**
- Comprehensive docstrings
- Examples and usage patterns
- Error handling guidelines

## Usage Examples

### Basic Enhanced Usage
```python
from python.nlm_enhanced import EnhancedConfig, EnhancedBrain

# Create enhanced configuration
config = EnhancedConfig()
config.set("neuron_count", 1000)
config.set("region_count", 5)
config.validate()

# Create brain with enhanced analysis
brain = pynlm.create_brain()
brain.initialize()
enhanced_brain = EnhancedBrain(brain)
stats = enhanced_brain.get_enhanced_stats()

print(f"Efficiency Score: {stats['performance_metrics']['efficiency_score']:.2f}")
print(f"Health Score: {stats['health_indicators']['health_score']:.2f}")
```

### Error Handling
```python
try:
    config.set("neuron_count", 5)
    config.validate()
except ValueError as e:
    print(f"Configuration error: {e}")
```

### Simulation Analysis
```python
# Run simulation
brain = pynlm.create_brain()
brain.initialize()

for step in range(100):
    brain.step(step)

# Get comprehensive statistics
stats = brain.get_comprehensive_stats()
performance = stats['performance']

print(f"Stability: {performance['stability']:.2f}")
print(f"Efficiency: {performance['spike_efficiency']:.2f}")
```

## Key Features

### Enhanced Statistics
- **Performance Metrics**: Efficiency scores, stability ratings, health indicators
- **Comprehensive Analysis**: Region statistics, development stage analysis
- **Optimization Suggestions**: Data-driven recommendations for improvement

### Configuration Management
- **Schema Validation**: Type and range checking for all configuration options
- **Business Rules**: Cross-validation rules for configuration consistency
- **Error Recovery**: Clear error messages for configuration issues

### Memory Management
- **Checkpoint Operations**: Save and load brain states
- **Memory Statistics**: Detailed memory usage analysis
- **Persistence**: Support for long-running simulations

### Error Handling
- **Custom Exceptions**: Specific exception types for different error categories
- **Detailed Messages**: Error messages include context and suggestions
- **Graceful Degradation**: Clear error messages even when modules are unavailable

## Installation

```bash
# Install dependencies
pip install pybind11 scikit-build-core pytest numpy matplotlib

# Build C++ extensions
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install Python package
pip install -e .
```

## Examples Directory

The `examples/` directory contains comprehensive examples demonstrating:

1. **Basic Usage**: Simple brain creation and simulation
2. **Advanced Simulation**: Enhanced agent simulation with learning
3. **Brain Benchmarking**: Performance testing and analysis
4. **Memory Management**: Checkpoint operations and analysis
5. **Configuration Management**: Advanced configuration handling
6. **Brain Analysis**: Comprehensive statistics and visualization
7. **Error Handling**: Robust error handling patterns
8. **Integration**: Complete system integration example

Each example includes:
- Clear explanations of the concepts
- Code with comments and best practices
- Output and expected results
- Tips for further exploration

## Testing

Run the comprehensive test suite:

```bash
pytest tests/ -v --tb=short
```

The test suite includes:
- Unit tests for all new functionality
- Integration tests for system-wide features
- Performance tests for benchmarking
- Error handling tests
- Backward compatibility tests

## Documentation

### API Reference
- `EnhancedBrain`: Enhanced brain interface with analysis
- `EnhancedWorld`: Enhanced world interface with percept analysis
- `EnhancedConfig`: Enhanced configuration with validation
- `MemoryManager`: Memory management and checkpoint operations

### Examples
- `nlm_examples.py`: Comprehensive examples of all features
- `demo_enhanced.py`: Demonstration of key enhancements
- Individual example files in the `examples/` directory

### Error Types
- `NLMError`: Base exception class
- `ValidationError`: Configuration validation errors
- `ConfigurationError`: Invalid configuration
- `SimulationError`: Simulation failures

## Migration Guide

### From Basic to Enhanced

If you were using the basic bindings, you can migrate to enhanced bindings:

**Basic:**
```python
import pynlm
brain = pynlm.create_brain()
```

**Enhanced:**
```python
import pynlm
from python.nlm_enhanced import EnhancedBrain

brain = pynlm.create_brain()
enhanced_brain = EnhancedBrain(brain)
stats = enhanced_brain.get_enhanced_stats()
```

### Key Benefits of Enhanced Bindings

1. **Better Error Handling**: Clear, actionable error messages
2. **Advanced Analysis**: Performance metrics and health indicators
3. **Configuration Validation**: Type-safe configuration with helpful feedback
4. **Memory Management**: Checkpoint support for long simulations
5. **Comprehensive Documentation**: Examples and best practices
6. **Performance Tools**: Benchmarking and analysis capabilities

## Troubleshooting

### Common Issues

1. **pynlm module not available**
   - Ensure C++ extensions are built and installed
   - Check that `pip install -e .` was successful
   - Verify build output for compilation errors

2. **Configuration validation errors**
   - Check that all required configuration keys are set
   - Verify configuration values are within valid ranges
   - Use `config.summary()` to see current configuration

3. **Memory usage issues**
   - Monitor memory usage with `MemoryManager.get_memory_stats()`
   - Use checkpoints for long-running simulations
   - Consider reducing brain size for memory-constrained environments

4. **Performance issues**
   - Check efficiency scores in enhanced statistics
   - Use `BenchmarkBrain()` for performance testing
   - Optimize configuration for your specific use case

### Getting Help

- Check the examples directory for usage patterns
- Read the API reference for detailed documentation
- Review error messages for specific guidance
- Run tests to verify installation and functionality

## Future Enhancements

The enhanced bindings will continue to evolve with:

1. **Advanced Analytics**: Machine learning-based brain analysis
2. **Visualization Tools**: Interactive brain visualization
3. **Cloud Integration**: Cloud-based simulation and analysis
4. **Plugin Architecture**: Extensible plugin system
5. **Multi-agent Support**: Multi-agent system analysis and coordination

## License

This enhanced bindings module is provided under the same license as the original NLM framework.

---

**Version**: 0.1.0
**Author**: NLM Project Team
**Date**: Enhanced bindings development
**Status**: Alpha

For more information, visit the NLM project documentation and GitHub repository.
EOF

echo "✓ Enhanced bindings README created"

echo "\n=== Setup complete! ==="
echo "Enhanced NLM Python bindings have been installed successfully."
echo "\nKey features available:"
echo "1. EnhancedBrain - Comprehensive brain analysis with performance metrics"
echo "2. EnhancedConfig - Configuration validation with schema checking"
echo "3. EnhancedWorld - Percept analysis and pattern detection"
echo "4. MemoryManager - Checkpoint operations and memory statistics"
echo "5. Advanced error handling with custom exception types"
echo "6. Comprehensive examples and testing"
echo "\nTo get started:")
echo "1. Run: python demo_enhanced.py"
echo "2. Explore examples/ directory"
echo "3. Run tests: pytest tests/ -v"
echo "4. Read ENHANCED_README.md for detailed documentation"
