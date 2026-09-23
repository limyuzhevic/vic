# NLM Configuration Guide

This document provides comprehensive guidance on configuration options,
setup, and customization for the enhanced NLM Python bindings.

## Overview

Configuration is a fundamental aspect of NLM simulations. The enhanced bindings
provide a flexible, validation-based configuration system that supports
multiple configuration methods and environment-specific setups.

## Configuration System

### Core Configuration Classes

#### Config Class

The `Config` class is the main configuration interface providing:

- **Parameter Validation**: Validates all configuration parameters
- **Error Handling**: Comprehensive error messages for invalid configurations
- **Flexibility**: Support for dynamic configuration changes
- **Documentation**: Built-in parameter descriptions and validation rules

**Basic Usage:**
```python
from pynlm import Config

# Create and configure
config = Config()
config.configure(
    max_steps=1000,
    simulation_speed=1.0,
    enable_visualization=True,
    enable_monitoring=True
)

print(config.summary())
```

**Key Features:**
- Type-safe parameter validation
- Custom validation rules
- Parameter descriptions and documentation
- Method chaining for fluent API

#### Configuration Validation

The enhanced bindings include comprehensive validation rules:

```python
from pynlm import Config, ValidationError

# Custom validation rules
config = Config()

def validate_positive_int(value):
    errors = []
    if not isinstance(value, int) or value <= 0:
        errors.append("Must be a positive integer")
    return errors

config.add_validation_rule('max_steps', validate_positive_int)

# Configuration will be validated automatically
try:
    config.configure(max_steps=100)
    print("✓ Valid configuration accepted")
except ValidationError as e:
    print(f"✗ Validation error: {e}")
```

### Configuration Methods

#### Direct Configuration

```python
from pynlm import AdvancedBrain, Config

# Method 1: Direct configuration
brain = AdvancedBrain()
brain.configure_simulation(
    max_steps=1000,
    simulation_speed=1.0,
    enable_visualization=False,
    enable_monitoring=True
)
```

#### Configuration File I/O

```python
from pynlm import Config
import json
from pathlib import Path

# Save configuration to file
config = Config().configure(
    max_steps=1000,
    simulation_speed=1.0,
    enable_visualization=True
)

config_file = Path("simulation_config.json")
with open(config_file, 'w') as f:
    json.dump(dict(config._parameters), f, indent=2)

# Load configuration from file
loaded_config = Config()
with open(config_file, 'r') as f:
    params = json.load(f)
loaded_config.configure(**params)

print(f"Loaded configuration: {loaded_config.summary()}")
```

#### Environment-Specific Configurations

```python
from pynlm import Config

class EnvironmentConfigs:
    """Environment-specific configuration templates."""
    
    @staticmethod
    def development_config():
        return Config().configure(
            max_steps=500,
            simulation_speed=0.5,
            enable_monitoring=True,
            monitoring_interval=0.05,
            enable_visualization=True,
            enable_development=True,
            enable_curiosity=True
        )
    
    @staticmethod
    def production_config():
        return Config().configure(
            max_steps=5000,
            simulation_speed=1.5,
            enable_monitoring=True,
            monitoring_interval=0.1,
            enable_visualization=True,
            enable_reward_modulation=True,
            memory_efficient_mode=True
        )
    
    @staticmethod
    def testing_config():
        return Config().configure(
            max_steps=50,
            simulation_speed=2.0,
            enable_monitoring=False,
            enable_visualization=False,
            enable_development=False
        )
    
    @staticmethod
    def real_time_config():
        return Config().configure(
            max_steps=100,
            simulation_speed=0.3,
            enable_monitoring=True,
            monitoring_interval=0.01,
            enable_visualization=True,
            visualization_backend='plotly',
            enable_reward_modulation=True,
            prioritize_response_time=True
        )

# Use environment-specific configurations
configs = EnvironmentConfigs()
dev_config = configs.development_config()
prod_config = configs.production_config()
test_config = configs.testing_config()
realtime_config = configs.real_time_config()
```

### Configuration Parameters

#### Core Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `max_steps` | int | 1000 | Maximum number of simulation steps |
| `simulation_speed` | float | 1.0 | Simulation time scaling factor |
| `reward_threshold` | float | 0.0 | Minimum reward for success |
| `enable_visualization` | bool | False | Enable visualization |
| `enable_monitoring` | bool | True | Enable performance monitoring |
| `monitoring_interval` | float | 0.1 | Monitoring interval in seconds |

#### Advanced Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `visualization_backend` | str | 'matplotlib' | Visualization backend |
| `enable_reward_modulation` | bool | True | Enable reward-based neuromodulation |
| `enable_structural_plasticity` | bool | False | Enable structural plasticity |
| `enable_development` | bool | True | Enable developmental processes |
| `enable_curiosity` | bool | False | Enable curiosity-driven learning |
| `enable_optimization` | bool | False | Enable performance optimizations |
| `memory_efficient_mode` | bool | False | Enable memory-efficient processing |

#### Visualization Backend Options

- `matplotlib`: Standard Python plotting library
- `plotly`: Interactive web-based visualizations
- `bokeh`: Advanced web plotting library

### Configuration Validation Rules

#### Built-in Validation

The enhanced bindings include built-in validation for common parameters:

```python
# Validation examples
config = Config()

# Step count validation
config.configure(max_steps=100)  # Valid
# config.configure(max_steps=-10)  # Invalid (negative steps)

# Simulation speed validation
config.configure(simulation_speed=1.0)  # Valid
# config.configure(simulation_speed=0)  # Invalid (non-positive)

# Reward threshold validation
config.configure(reward_threshold=0.5)  # Valid
# config.configure(reward_threshold=2.0)  # Invalid (too high)

# Monitoring interval validation
config.configure(monitoring_interval=0.1)  # Valid
# config.configure(monitoring_interval=0)  # Invalid (zero)
```

#### Custom Validation Rules

Add custom validation rules for specialized requirements:

```python
from pynlm import Config, ValidationError

class SpecializedConfig:
    """Specialized configuration for specific use cases."""
    
    @staticmethod
    def validate_for_neural_network(config_dict):
        """Validate configuration for neural network simulations."""
        errors = []
        
        # Neural network specific rules
        if config_dict.get('max_steps', 0) < 10:
            errors.append("Neural network simulations require at least 10 steps")
        
        if config_dict.get('simulation_speed', 1.0) <= 0:
            errors.append("Simulation speed must be positive")
        
        if config_dict.get('reward_threshold', 0) < -1000:
            errors.append("Reward threshold too negative")
        
        return len(errors) == 0
    
    @staticmethod
    def validate_for_reinforcement_learning(config_dict):
        """Validate configuration for reinforcement learning."""
        errors = []
        
        # RL specific requirements
        if config_dict.get('max_steps', 0) < 100:
            errors.append("Reinforcement learning requires at least 100 steps")
        
        if not config_dict.get('enable_reward_modulation', True):
            errors.append("Reinforcement learning requires reward modulation")
        
        if not config_dict.get('enable_structural_plasticity', False):
            errors.append("Advanced RL may benefit from structural plasticity")
        
        return len(errors) == 0

# Use specialized validation
config = Config().configure(
    max_steps=500,
    simulation_speed=1.0,
    enable_reward_modulation=True,
    enable_structural_plasticity=True
)

# Validate configuration
if SpecializedConfig.validate_for_reinforcement_learning(dict(config._parameters)):
    print("✓ Configuration valid for reinforcement learning")
else:
    print("✗ Configuration invalid for reinforcement learning")
```

### Configuration Inheritance and Override

Implement configuration inheritance for reusable configurations:

```python
from pynlm import Config, AdvancedBrain

class ConfigurationManager:
    """Configuration manager with inheritance support."""
    
    def __init__(self):
        self.config_registry: Dict[str, Config] = {}
    
    def register_config(self, name: str, config: Config, 
                       inherit_from: List[str] = None):
        """Register configuration with optional inheritance."""
        if inherit_from:
            # Merge configurations
            base_config = Config()
            
            # Inherit from parent configurations
            for parent_name in inherit_from:
                if parent_name in self.config_registry:
                    parent_config = self.config_registry[parent_name]
                    # Merge parameters (child overrides parent)
                    base_config.configure(**parent_config._parameters)
            
            # Apply child's specific parameters
            base_config.configure(**config._parameters)
            self.config_registry[name] = base_config
        else:
            self.config_registry[name] = config
    
    def get_config(self, name: str) -> Config:
        """Get configuration by name."""
        if name not in self.config_registry:
            raise ValueError(f"Configuration not found: {name}")
        
        return self.config_registry[name]

# Create configuration hierarchy
base_config = Config().configure(
    max_steps=1000,
    simulation_speed=1.0,
    enable_monitoring=True
)

experiment_config = Config().configure(
    max_steps=500,
    simulation_speed=1.5,
    enable_curiosity=True,
    enable_structural_plasticity=True
)

production_config = Config().configure(
    max_steps=2000,
    simulation_speed=1.2,
    enable_visualization=True,
    memory_efficient_mode=True
)

# Setup configuration manager
config_manager = ConfigurationManager()
config_manager.register_config('base', base_config)
config_manager.register_config('experiment', experiment_config, inherit_from=['base'])
config_manager.register_config('production', production_config, inherit_from=['base'])

# Use inherited configurations
base = config_manager.get_config('base')
experiment = config_manager.get_config('experiment')
production = config_manager.get_config('production')

print(f"Base: {base.get('max_steps')} steps")
print(f"Experiment: {experiment.get('max_steps')} steps (inherits base + specific)")
print(f"Production: {production.get('max_steps')} steps (inherits base + specific)")
```

### Configuration Templates

Predefined configuration templates for common use cases:

```python
from pynlm import AdvancedBrain, Config

class ConfigurationTemplates:
    """Configuration templates for common scenarios."""
    
    @staticmethod
    def get_research_template():
        """Research template for detailed analysis."""
        return Config().configure(
            max_steps=2000,
            simulation_speed=1.0,
            enable_monitoring=True,
            monitoring_interval=0.05,
            enable_development=True,
            enable_curiosity=True,
            enable_structural_plasticity=True,
            enable_reward_modulation=True,
            enable_visualization=True,
            visualization_backend='plotly',
            collect_detailed_metrics=True,
            export_data_format='csv'
        )
    
    @staticmethod
    def get_production_template():
        """Production template for stable operation."""
        return Config().configure(
            max_steps=5000,
            simulation_speed=1.5,
            enable_monitoring=True,
            monitoring_interval=0.1,
            enable_visualization=True,
            enable_reward_modulation=True,
            memory_efficient_mode=True,
            enable_optimization=True,
            parallel_processing=True,
            backup_interval=3600  # seconds
        )
    
    @staticmethod
    def get_testing_template():
        """Testing template for rapid validation."""
        return Config().configure(
            max_steps=50,
            simulation_speed=2.0,
            enable_monitoring=False,
            enable_visualization=False,
            enable_development=False,
            skip_complex_features=True,
            minimal_dependencies=True,
            fast_validation=True
        )
    
    @staticmethod
    def get_real_time_template():
        """Real-time template for interactive applications."""
        return Config().configure(
            max_steps=100,
            simulation_speed=0.5,
            enable_monitoring=True,
            monitoring_interval=0.01,
            enable_visualization=True,
            visualization_backend='bokeh',
            enable_reward_modulation=True,
            prioritize_response_time=True,
            background_processing=False,
            streaming_mode=True,
            adaptive_quality=True
        )

# Use configuration templates
brain = AdvancedBrain(verbose=True)

# Research configuration
research_brain = AdvancedBrain(config=ConfigurationTemplates.get_research_template())
print(f"Research template: {research_brain._config.get('max_steps')} steps")

# Production configuration
production_brain = AdvancedBrain(config=ConfigurationTemplates.get_production_template())
print(f"Production template: {production_brain._config.get('max_steps')} steps")

# Testing configuration
testing_brain = AdvancedBrain(config=ConfigurationTemplates.get_testing_template())
print(f"Testing template: {testing_brain._config.get('max_steps')} steps")

# Real-time configuration
realtime_brain = AdvancedBrain(config=ConfigurationTemplates.get_real_time_template())
print(f"Real-time template: {realtime_brain._config.get('max_steps')} steps")
```

### Configuration Error Handling

Robust error handling for configuration issues:

```python
from pynlm import AdvancedBrain, Config, ValidationError, NLMError
import time

class RobustConfigurationManager:
    """Robust configuration manager with error handling."""
    
    def __init__(self):
        self.config_history: List[Dict[str, Any]] = []
        self.error_count = 0
    
    def create_brain_with_validation(self, config_params: Dict[str, Any], 
                                   brain_kwargs: Dict[str, Any] = None) -> AdvancedBrain:
        """Create brain with comprehensive validation."""
        try:
            # Validate configuration parameters
            config = Config().configure(**config_params)
            
            # Validate brain parameters
            if brain_kwargs:
                for key, value in brain_kwargs.items():
                    if not isinstance(value, (str, int, float, bool, type(None))):
                        raise ValidationError(f"Invalid type for parameter {key}: {type(value)}")
            
            # Create brain
            brain = AdvancedBrain(config=config, **brain_kwargs)
            
            # Record successful configuration
            self.config_history.append({
                'timestamp': time.time(),
                'config': config_params,
                'brain_kwargs': brain_kwargs,
                'success': True
            })
            
            return brain
            
        except ValidationError as e:
            self.error_count += 1
            self.config_history.append({
                'timestamp': time.time(),
                'config': config_params,
                'brain_kwargs': brain_kwargs,
                'success': False,
                'error': str(e),
                'error_type': 'ValidationError'
            })
            raise
            
        except Exception as e:
            self.error_count += 1
            self.config_history.append({
                'timestamp': time.time(),
                'config': config_params,
                'brain_kwargs': brain_kwargs,
                'success': False,
                'error': str(e),
                'error_type': type(e).__name__
            })
            raise NLMError(f"Configuration failed: {e}") from e
    
    def get_configuration_report(self) -> str:
        """Generate configuration report."""
        if not self.config_history:
            return "No configuration attempts recorded."
        
        report = f"Configuration Report\n"
        report += f"{'=' * 40}\n"
        report += f"Total Attempts: {len(self.config_history)}\n"
        report += f"Successful: {len([c for c in self.config_history if c['success']])}\n"
        report += f"Failed: {self.error_count}\n"
        report += f"Success Rate: {len([c for c in self.config_history if c['success']]) / len(self.config_history) * 100:.1f}%\n\n"
        
        # Recent configurations
        recent_configs = self.config_history[-5:]
        report += f"Recent Configurations:\n"
        for i, config in enumerate(recent_configs):
            status = "✓" if config['success'] else "✗"
            report += f"  {status} Attempt {len(self.config_history) - i}: {config.get('config', {}).get('max_steps', 'N/A')} steps\n"
        
        return report

# Use robust configuration manager
config_manager = RobustConfigurationManager()

print("Creating brains with robust validation...")

# Valid configuration
try:
    valid_brain = config_manager.create_brain_with_validation({
        'max_steps': 100,
        'simulation_speed': 1.0,
        'enable_monitoring': True
    })
    print("✓ Valid brain created")
except Exception as e:
    print(f"✗ Valid brain creation failed: {e}")

# Invalid configuration
try:
    invalid_brain = config_manager.create_brain_with_validation({
        'max_steps': -100,  # Invalid
        'simulation_speed': 0,  # Invalid
    })
    print("✗ Invalid brain should have been rejected")
except ValidationError as e:
    print(f"✓ Invalid brain correctly rejected: {e}")
except Exception as e:
    print(f"✗ Unexpected error: {e}")

# Get configuration report
report = config_manager.get_configuration_report()
print(f"\n=== Configuration Report ===\n{report}")
```

### Configuration Performance Optimization

Optimize configuration for performance:

```python
from pynlm import AdvancedBrain, Config
import time
import psutil

class PerformanceOptimizedConfig:
    """Performance-optimized configuration."""
    
    def __init__(self):
        self.performance_data: List[Dict[str, Any]] = []
    
    def optimize_for_performance(self, target_steps: int, memory_limit: float):
        """Optimize configuration for performance targets."""
        optimization_result = {
            'original_config': {},
            'optimized_config': {},
            'improvements': [],
            'performance_gains': []
        }
        
        # Analyze current system state
        current_memory = self._get_memory_usage()
        current_cpu = psutil.cpu_percent()
        
        optimization_result['original_config'] = {
            'memory_usage': current_memory,
            'cpu_usage': current_cpu,
            'target_steps': target_steps,
            'memory_limit': memory_limit
        }
        
        # Optimize based on targets
        if memory_limit < current_memory * 0.8:
            # Memory-constrained optimization
            optimization_result['optimized_config'].update({
                'enable_monitoring': False,
                'enable_visualization': False,
                'enable_development': False,
                'enable_curiosity': False,
                'simulation_speed': min(2.0, 1.0 * (memory_limit / current_memory))
            })
            optimization_result['improvements'].append(
                "Reduced memory usage by disabling non-essential features"
            )
        
        elif target_steps > 1000:
            # High-performance optimization
            optimization_result['optimized_config'].update({
                'enable_monitoring': True,
                'monitoring_interval': max(0.01, 0.1 * (1000 / target_steps)),
                'simulation_speed': min(2.0, 1.5 * (target_steps / 1000)),
                'enable_optimization': True,
                'parallel_processing': True
            })
            optimization_result['improvements'].append(
                "Optimized for high-performance simulation"
            )
        
        else:
            # Standard optimization
            optimization_result['optimized_config'].update({
                'simulation_speed': min(1.5, self._calculate_optimal_speed())
            })
            optimization_result['improvements'].append(
                "Optimized simulation speed for current system"
            )
        
        # Estimate performance gains
        optimization_result['performance_gains'] = self._estimate_performance_gains(
            optimization_result['original_config'],
            optimization_result['optimized_config']
        )
        
        return optimization_result
    
    def _get_memory_usage(self) -> float:
        """Get current memory usage in MB."""
        try:
            process = psutil.Process()
            return process.memory_info().rss / 1024 / 1024
        except ImportError:
            return 0.0
    
    def _calculate_optimal_speed(self) -> float:
        """Calculate optimal simulation speed based on system resources."""
        cpu_usage = psutil.cpu_percent()
        memory_usage = self._get_memory_usage()
        
        # Simple heuristic for optimal speed
        speed = 1.0
        
        if cpu_usage > 80:
            speed *= 0.8  # Reduce speed if CPU usage high
        
        if memory_usage > 500:
            speed *= 0.7  # Reduce speed if memory usage high
        
        return min(2.0, max(0.5, speed))
    
    def _estimate_performance_gains(self, original: Dict[str, Any], 
                                   optimized: Dict[str, Any]) -> List[str]:
        """Estimate performance gains from optimizations."""
        gains = []
        
        if optimized.get('enable_monitoring') == False and original.get('enable_monitoring', True):
            gains.append("Reduced memory overhead by 15-20%")
        
        if optimized.get('enable_visualization') == False and original.get('enable_visualization', False):
            gains.append("Reduced CPU usage by 10-15%")
        
        speed_original = original.get('simulation_speed', 1.0)
        speed_optimized = optimized.get('simulation_speed', 1.0)
        
        if speed_optimized > speed_original:
            gains.append(f"Increased simulation speed by {(speed_optimized/speed_original - 1)*100:.0f}%")
        
        if not gains:
            gains.append("Maintained current performance while adding features")
        
        return gains
    
    def create_optimized_brain(self, target_steps: int, memory_limit: float = None) -> AdvancedBrain:
        """Create performance-optimized brain."""
        print(f"Creating performance-optimized brain for {target_steps} steps")
        
        # Determine memory limit if not provided
        if memory_limit is None:
            memory_limit = self._get_memory_usage() * 2  # Use 2x current memory
        
        # Optimize configuration
        optimization = self.optimize_for_performance(target_steps, memory_limit)
        
        if optimization['improvements']:
            print("Optimizations applied:")
            for improvement in optimization['improvements']:
                print(f"  ✓ {improvement}")
        
        if optimization['performance_gains']:
            print("Expected performance gains:")
            for gain in optimization['performance_gains']:
                print(f"  → {gain}")
        
        # Create optimized brain
        optimized_config = Config().configure(**optimization['optimized_config'])
        brain = AdvancedBrain(config=optimized_config, verbose=True)
        
        # Record optimization
        self.performance_data.append({
            'timestamp': time.time(),
            'original_config': optimization['original_config'],
            'optimized_config': optimization['optimized_config'],
            'target_steps': target_steps,
            'performance_gains': optimization['performance_gains']
        })
        
        return brain

# Use performance-optimized configuration
print("Testing performance-optimized configurations...")

optimizer = PerformanceOptimizedConfig()

# Memory-constrained optimization
print("\n1. Memory-constrained optimization (512 MB limit)...")
memory_brain = optimizer.create_optimized_brain(target_steps=500, memory_limit=512)
print(f"   Created brain with {memory_brain._config.get('max_steps')} steps")

# High-performance optimization
print("\n2. High-performance optimization (2000+ steps)...")
performance_brain = optimizer.create_optimized_brain(target_steps=2000)
print(f"   Created brain with {performance_brain._config.get('max_steps')} steps")

# Standard optimization
print("\n3. Standard optimization (100 steps)...")
standard_brain = optimizer.create_optimized_brain(target_steps=100)
print(f"   Created brain with {standard_brain._config.get('max_steps')} steps")

# Generate optimization report
optimization_report = optimizer.generate_optimization_report()
print(f"\n=== Optimization Report ===\n{optimization_report}")
```

### Configuration Documentation

Generate configuration documentation:

```python
from pynlm import Config, AdvancedBrain

class ConfigurationDocumentation:
    """Generate configuration documentation."""
    
    @staticmethod
    def generate_api_reference(config: Config) -> str:
        """Generate API reference for configuration."""
        doc = f"Configuration API Reference\n"
        doc += f"{'=' * 40}\n\n"
        
        doc += f"Available Parameters:\n"
        for key in config.get_keys():
            doc += f"- `{key}`: {config._description.get(key, 'No description available')}\n"
        
        doc += f"\nValidation Rules:\n"
        # Add validation rules documentation
        for key, rule in config._validation_rules.items():
            doc += f"- `{key}`: {rule.__doc__ or 'Custom validation rule'}\n"
        
        return doc
    
    @staticmethod
    def generate_user_guide(config: Config) -> str:
        """Generate user guide for configuration."""
        guide = f"NLM Configuration User Guide\n"
        guide += f"{'=' * 40}\n\n"
        
        guide += f"Getting Started:\n"
        guide += f"1. Create a configuration object: `Config()`\n"
        guide += f"2. Configure parameters: `config.configure(key1=value1, key2=value2)`\n"
        guide += f"3. Create brain: `brain = AdvancedBrain(config=config)`\n\n"
        
        guide += f"Common Configurations:\n"
        # Add common configuration examples
        templates = [
            ('Quick Test', {'max_steps': 50, 'simulation_speed': 2.0}),
            ('Research', {'max_steps': 1000, 'enable_visualization': True}),
            ('Production', {'max_steps': 5000, 'memory_efficient_mode': True})
        ]
        
        for name, params in templates:
            guide += f"- **{name}**: {params}\n"
        
        guide += f"\nParameter Descriptions:\n"
        for key in config.get_keys():
            doc = config._description.get(key, 'No description')
            guide += f"- **{key}**: {doc}\n"
        
        return guide
    
    @staticmethod
    def generate_example_configs() -> Dict[str, Config]:
        """Generate example configurations."""
        examples = {}
        
        # Basic configuration
        examples['basic'] = Config().configure(
            max_steps=100,
            simulation_speed=1.0
        )
        
        # Advanced configuration
        examples['advanced'] = Config().configure(
            max_steps=500,
            simulation_speed=1.5,
            enable_visualization=True,
            enable_monitoring=True,
            enable_development=True,
            enable_curiosity=True
        )
        
        # Minimal configuration
        examples['minimal'] = Config().configure(
            max_steps=50,
            simulation_speed=2.0,
            enable_monitoring=False,
            enable_visualization=False
        )
        
        return examples

# Generate documentation
config = Config().configure(
    max_steps=100,
    simulation_speed=1.0,
    enable_visualization=True,
    enable_monitoring=True
)

doc_gen = ConfigurationDocumentation()

print("=== API Reference ===")
api_ref = doc_gen.generate_api_reference(config)
print(api_ref[:1000] + "..." if len(api_ref) > 1000 else api_ref)

print("\n=== User Guide ===")
user_guide = doc_gen.generate_user_guide(config)
print(user_guide[:800] + "..." if len(user_guide) > 800 else user_guide)

print("\n=== Example Configurations ===")
examples = doc_gen.generate_example_configs()
for name, example_config in examples.items():
    print(f"{name}: {example_config.summary()}")
```

## Configuration Best Practices

### 1. Start Simple

Begin with minimal configuration and add features gradually:

```python
# Start with basic configuration
brain = AdvancedBrain()
brain.configure_simulation(max_steps=100, simulation_speed=1.0)

# Add features as needed
brain.configure_simulation(enable_monitoring=True)
brain.configure_simulation(enable_visualization=True)
```

### 2. Validate Configuration

Always validate configuration parameters:

```python
from pynlm import ValidationError

try:
    config = Config().configure(max_steps=100, invalid_param='value')
    # This will fail if invalid_param is not a valid parameter
    print("✓ Configuration validated")
except ValidationError as e:
    print(f"✗ Configuration validation failed: {e}")
```

### 3. Use Environment-Specific Configurations

Use different configurations for different environments:

```python
def get_config_for_environment(env: str):
    if env == 'development':
        return Config().configure(max_steps=500, enable_monitoring=True)
    elif env == 'production':
        return Config().configure(max_steps=5000, memory_efficient_mode=True)
    elif env == 'testing':
        return Config().configure(max_steps=50, enable_monitoring=False)
    else:
        raise ValueError(f"Unknown environment: {env}")
```

### 4. Document Configuration

Document your configuration for future reference:

```python
# Save configuration documentation
config = Config().configure(
    max_steps=1000,
    simulation_speed=1.0,
    enable_visualization=True
)

with open('config_documentation.txt', 'w') as f:
    f.write(f"Configuration: {config.summary()}\n")
    f.write(f"Parameters: {config.get_keys()}\n")
```

### 5. Test Configuration

Test configuration thoroughly before deployment:

```python
def test_configuration(config):
    try:
        brain = AdvancedBrain(config=config)
        results = brain.run_episode(max_steps=10)
        return results['success']
    except Exception as e:
        print(f"Configuration test failed: {e}")
        return False

# Test different configurations
for config_name, config_template in [
    ('basic', Config().configure(max_steps=50)),
    ('advanced', Config().configure(max_steps=200, enable_monitoring=True)),
    ('production', Config().configure(max_steps=1000, memory_efficient_mode=True))
]:
    if test_configuration(config_template):
        print(f"✓ {config_name} configuration tested successfully")
    else:
        print(f"✗ {config_name} configuration test failed")
```

## Configuration Summary

The enhanced NLM Python bindings provide a comprehensive configuration system with:

- **Flexible Configuration**: Support for dynamic configuration changes
- **Robust Validation**: Comprehensive parameter validation
- **Environment-Specific**: Templates for different use cases
- **Performance Optimization**: Automatic performance tuning
- **Documentation**: Built-in documentation and examples
- **Error Handling**: Comprehensive error handling and recovery

This configuration system enables users to easily set up and manage NLM simulations across different environments and use cases while maintaining stability, performance, and ease of use.

---

*This documentation provides comprehensive guidance on configuration options and best practices for the enhanced NLM Python bindings. For the most up-to-date information, refer to the API reference and examples in the repository.*

Enjoy configuring your NLM simulations! 🎯⚙️