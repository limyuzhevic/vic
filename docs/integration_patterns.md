# NLM Integration Patterns Guide

This document provides architectural patterns and integration strategies for NLM, covering plugin architecture, batch processing, distributed computing, real-time systems, and integration with external systems.

## Table of Contents

1. [Plugin Architecture](#plugin-architecture)
2. [Batch Processing](#batch-processing)
3. [Distributed Computing](#distributed-computing)
4. [Real-time Systems](#real-time-systems)
5. [HCI Integration](#hci-integration)
6. [Robotics Integration](#robotics-integration)

---

## 1. Plugin Architecture

### Overview
Plugin architecture allows NLM to be extended with custom components while maintaining system stability and modularity.

### Pattern 1: Sensory Plugin Interface

```python
# plugin_interfaces/sensory_plugin.py
from abc import ABC, abstractmethod
import numpy as np

class SensoryPlugin(ABC):
    """Abstract base class for sensory plugins."""
    
    @abstractmethod
    def get_sensor_type(self):
        """Return the sensor type this plugin provides."""
        pass
    
    @abstractmethod
    def get_sensor_config(self):
        """Return sensor configuration."""
        pass
    
    @abstractmethod
    def process_input(self, raw_input):
        """Process raw sensor input into NLM-compatible format."""
        pass
    
    @abstractmethod
    def is_input_valid(self, input_data):
        """Validate input data."""
        pass
    
    def get_plugin_info(self):
        """Return plugin information for registration."""
        return {
            'name': self.__class__.__name__,
            'version': '1.0.0',
            'description': self.get_description(),
            'author': self.get_author(),
            'license': self.get_license()
        }
    
    def get_description(self):
        """Get plugin description (override if needed)."""
        return "Abstract sensory plugin"
    
    def get_author(self):
        """Get plugin author (override if needed)."""
        return "Unknown"
    
    def get_license(self):
        """Get plugin license (override if needed)."""
        return "MIT"

# Example Vision Plugin
class ComputerVisionPlugin(SensoryPlugin):
    """Computer vision plugin for NLM integration."""
    
    def __init__(self, camera_params=None):
        self.camera_params = camera_params or {
            'resolution': (640, 480),
            'fps': 30,
            'color_space': 'RGB',
            'calibration_file': None
        }
        self.calibration_matrix = None
        self.distortion_coefficients = None
        
    def get_sensor_type(self):
        return 'computer_vision'
    
    def get_sensor_config(self):
        return self.camera_params
    
    def process_input(self, raw_input):
        """Process computer vision input for NLM."""
        if raw_input is None:
            return None
        
        # Apply camera calibration if available
        if self.calibration_matrix is not None:
            raw_input = self.apply_calibration(raw_input)
        
        # Preprocess for NLM
        processed = self.preprocess_for_nlm(raw_input)
        
        # Convert to NLM vision format
        vision = pynlm.Vision(
            self.camera_params['resolution'][0],
            self.camera_params['resolution'][1],
            3  # RGB channels
        )
        vision.setData(processed.flatten())
        
        return vision
    
    def is_input_valid(self, input_data):
        """Validate computer vision input."""
        if input_data is None:
            return False
        
        if not isinstance(input_data, np.ndarray):
            return False
        
        expected_shape = (
            self.camera_params['resolution'][1],
            self.camera_params['resolution'][0],
            3
        )
        
        return input_data.shape == expected_shape
    
    def apply_calibration(self, frame):
        """Apply camera calibration to frame."""
        # Undistort frame using calibration parameters
        # Simplified implementation
        return frame
    
    def preprocess_for_nlm(self, frame):
        """Preprocess frame for NLM input."""
        # Resize if needed
        if frame.shape[0] != self.camera_params['resolution'][1] or \
           frame.shape[1] != self.camera_params['resolution'][0]:
            frame = cv2.resize(frame, self.camera_params['resolution'])
        
        # Apply color space conversion if needed
        if self.camera_params['color_space'] != 'RGB':
            if self.camera_params['color_space'] == 'GRAY':
                frame = cv2.cvtColor(frame, cv2.COLOR_RGB2GRAY)
                # Convert back to 3 channels for NLM
                frame = np.stack([frame] * 3, axis=-1)
        
        # Normalize to NLM expected range
        frame = frame / 255.0
        
        return frame
    
    def get_description(self):
        return "Computer vision sensor plugin with camera calibration and preprocessing"
    
    def get_author(self):
        return "NLM Computer Vision Team"
   
# Example Lidar Plugin
class LidarPlugin(SensoryPlugin):
    """Lidar sensor plugin for NLM integration."""
    
    def __init__(self, lidar_params=None):
        self.lidar_params = lidar_params or {
            'range': 10.0,
            'resolution': 0.1,
            'channels': 360,
            'scan_rate': 10.0,
            'noise_level': 0.05
        }
        
    def get_sensor_type(self):
        return 'lidar'
    
    def get_sensor_config(self):
        return self.lidar_params
    
    def process_input(self, raw_input):
        """Process lidar input for NLM."""
        if raw_input is None:
            return None
        
        # Convert to NLM touch format
        touch_data = self.convert_to_touch_format(raw_input)
        
        # Create NLM touch data
        touch = pynlm.SensoryInput()
        touch.set_data(touch_data)
        
        return touch
    
    def is_input_valid(self, input_data):
        """Validate lidar input."""
        if input_data is None:
            return False
        
        if not isinstance(input_data, np.ndarray):
            return False
        
        expected_length = int(self.lidar_params['channels'] * self.lidar_params['range'] / self.lidar_params['resolution'])
        
        return len(input_data) == expected_length
    
    def convert_to_touch_format(self, scan_data):
        """Convert lidar scan to NLM touch format."""
        # Convert polar to Cartesian coordinates
        touch_points = []
        
        for i, distance in enumerate(scan_data):
            if distance > 0:  # Valid measurement
                angle = i * self.lidar_params['resolution']
                x = distance * np.cos(angle)
                y = distance * np.sin(angle)
                
                # Add noise
                x += np.random.normal(0, self.lidar_params['noise_level'])
                y += np.random.normal(0, self.lidar_params['noise_level'])
                
                touch_points.extend([x, y, distance])
        
        return np.array(touch_points)
    
    def get_description(self):
        return "3D laser ranging sensor for obstacle detection and distance measurement"
    
    def get_author(self):
        return "NLM Robotics Team"

# Plugin Manager
class PluginManager:
    """Manage NLM plugins and their lifecycle."""
    
    def __init__(self):
        self.plugins = {}
        self.plugin_interfaces = {}
        
    def register_plugin(self, plugin_instance):
        """Register a plugin instance."""
        plugin_type = plugin_instance.get_sensor_type()
        
        if plugin_type not in self.plugins:
            self.plugins[plugin_type] = []
        
        self.plugins[plugin_type].append(plugin_instance)
        
        # Initialize plugin
        plugin_instance.initialize() if hasattr(plugin_instance, 'initialize') else None
        
        return True
    
    def unregister_plugin(self, plugin_instance):
        """Unregister a plugin instance."""
        plugin_type = plugin_instance.get_sensor_type()
        
        if plugin_type in self.plugins:
            plugin_list = self.plugins[plugin_type]
            if plugin_instance in plugin_list:
                plugin_list.remove(plugin_instance)
                
                # Cleanup plugin
                plugin_instance.cleanup() if hasattr(plugin_instance, 'cleanup') else None
                
                return True
        
        return False
    
    def get_plugins(self, plugin_type=None):
        """Get plugins of specified type or all plugins."""
        if plugin_type:
            return self.plugins.get(plugin_type, [])
        else:
            all_plugins = []
            for plugin_list in self.plugins.values():
                all_plugins.extend(plugin_list)
            return all_plugins
    
    def get_plugin_by_name(self, plugin_type, plugin_name):
        """Get specific plugin by name."""
        for plugin in self.get_plugins(plugin_type):
            if plugin.__class__.__name__ == plugin_name:
                return plugin
        return None
    
    def load_plugins_from_directory(self, directory_path):
        """Load plugins from directory."""
        # This would dynamically import plugins from Python modules
        # Simplified implementation
        return []
    
    def enable_plugin(self, plugin_type, plugin_name):
        """Enable a specific plugin."""
        plugin = self.get_plugin_by_name(plugin_type, plugin_name)
        
        if plugin and hasattr(plugin, 'enable'):
            plugin.enable()
            return True
        
        return False
    
    def disable_plugin(self, plugin_type, plugin_name):
        """Disable a specific plugin."""
        plugin = self.get_plugin_by_name(plugin_type, plugin_name)
        
        if plugin and hasattr(plugin, 'disable'):
            plugin.disable()
            return True
        
        return False

# Plugin Interface Registry
class PluginInterfaceRegistry:
    """Registry for plugin interfaces and their implementations."""
    
    def __init__(self):
        self.interfaces = {}
        self.implementations = {}
        
    def register_interface(self, interface_name, interface_class):
        """Register a plugin interface."""
        self.interfaces[interface_name] = interface_class
        
    def register_implementation(self, interface_name, implementation_class):
        """Register a plugin implementation for an interface."""
        if interface_name not in self.implementations:
            self.implementations[interface_name] = []
        
        self.implementations[interface_name].append(implementation_class)
        
    def get_implementations(self, interface_name):
        """Get all implementations for an interface."""
        return self.implementations.get(interface_name, [])
    
    def create_implementation(self, interface_name, **kwargs):
        """Create an implementation instance for an interface."""
        implementations = self.get_implementations(interface_name)
        
        if implementations:
            # Use first available implementation
            implementation_class = implementations[0]
            return implementation_class(**kwargs)
        
        return None

# Usage Example
if __name__ == "__main__":
    # Create plugin manager
    plugin_manager = PluginManager()
    
    # Create registry
    registry = PluginInterfaceRegistry()
    
    # Register interfaces
    registry.register_interface('SensoryPlugin', SensoryPlugin)
    
    # Register implementations
    registry.register_implementation('SensoryPlugin', ComputerVisionPlugin)
    registry.register_implementation('SensoryPlugin', LidarPlugin)
    
    # Create and register plugins
    vision_plugin = ComputerVisionPlugin({
        'resolution': (320, 240),
        'fps': 15
    })
    
    lidar_plugin = LidarPlugin({
        'range': 5.0,
        'channels': 180
    })
    
    plugin_manager.register_plugin(vision_plugin)
    plugin_manager.register_plugin(lidar_plugin)
    
    # Get plugins
    vision_plugins = plugin_manager.get_plugins('computer_vision')
    lidar_plugins = plugin_manager.get_plugins('lidar')
    
    print(f"Registered {len(vision_plugins)} vision plugins")
    print(f"Registered {len(lidar_plugins)} lidar plugins")
```

### Pattern 2: Neuromodulation Plugin Interface

```python
# plugin_interfaces/neuromodulation_plugin.py
from abc import ABC, abstractmethod
import numpy as np

class NeuromodulationPlugin(ABC):
    """Abstract base class for neuromodulation plugins."""
    
    @abstractmethod
    def get_modulator_type(self):
        """Return the neuromodulator type this plugin provides."""
        pass
    
    @abstractmethod
    def get_modulator_config(self):
        """Return neuromodulator configuration."""
        pass
    
    @abstractmethod
    def apply_modulation(self, neural_state, input_signals):
        """Apply neuromodulation to neural state."""
        pass
    
    @abstractmethod
    def update_modulation(self, time_step, external_inputs):
        """Update neuromodulator state."""
        pass
    
    def get_modulator_info(self):
        """Return modulator information for registration."""
        return {
            'name': self.__class__.__name__,
            'version': '1.0.0',
            'type': self.get_modulator_type(),
            'description': self.get_description(),
            'author': self.get_author()
        }
    
    def get_description(self):
        """Get modulator description (override if needed)."""
        return "Abstract neuromodulation plugin"
    
    def get_author(self):
        """Get modulator author (override if needed)."""
        return "Unknown"
    
    def get_effect_parameters(self):
        """Get parameters affecting modulator effect (override if needed)."""
        return {}
    
    def set_effect_parameters(self, parameters):
        """Set parameters affecting modulator effect."""
        pass

# Example Dopamine Plugin
class DopamineModulatorPlugin(NeuromodulationPlugin):
    """Dopamine neuromodulation plugin for NLM."""
    
    def __init__(self, dopamine_params=None):
        self.dopamine_params = dopamine_params or {
            'baseline_level': 0.1,
            'phasic_gain': 2.0,
            'tonic_decay_rate': 0.01,
            'learning_rate': 0.05,
            'prediction_error_gain': 1.0,
            'reward_prediction_window': 5
        }
        self.current_dopamine_level = 0.1
        self.prediction_history = []
        self.reward_history = []
        
    def get_modulator_type(self):
        return 'dopamine'
    
    def get_modulator_config(self):
        return self.dopamine_params
    
    def apply_modulation(self, neural_state, input_signals):
        """Apply dopamine modulation to neural state."""
        # Calculate modulation strength based on input signals
        modulation_strength = self.calculate_modulation_strength(input_signals)
        
        # Apply modulation to neural state
        modulated_state = self.modulate_neural_state(neural_state, modulation_strength)
        
        return modulated_state
    
    def update_modulation(self, time_step, external_inputs):
        """Update dopamine modulator state."""
        # Update prediction history
        self.update_predictions(external_inputs)
        
        # Calculate prediction error
        prediction_error = self.calculate_prediction_error(external_inputs)
        
        # Update dopamine level based on prediction error
        self.update_dopamine_level(prediction_error, time_step)
        
        return self.current_dopamine_level
    
    def calculate_modulation_strength(self, input_signals):
        """Calculate dopamine modulation strength."""
        # Combine different input sources
        reward_input = input_signals.get('reward', 0.0)
        prediction_input = input_signals.get('prediction_error', 0.0)
        novelty_input = input_signals.get('novelty', 0.0)
        
        # Weighted combination
        strength = (
            reward_input * self.dopamine_params['reward_gain'] +
            prediction_input * self.dopamine_params['prediction_error_gain'] +
            novelty_input * self.dopamine_params['novelty_gain']
        )
        
        return np.clip(strength, 0.0, 1.0)
    
    def update_predictions(self, external_inputs):
        """Update reward prediction based on external inputs."""
        reward = external_inputs.get('reward', 0.0)
        
        # Update prediction history
        self.reward_history.append(reward)
        
        # Keep history bounded
        if len(self.reward_history) > self.dopamine_params['prediction_error_window']:
            self.reward_history.pop(0)
    
    def calculate_prediction_error(self, external_inputs):
        """Calculate reward prediction error."""
        if not self.reward_history:
            return 0.0
        
        predicted_reward = np.mean(self.reward_history)
        actual_reward = external_inputs.get('reward', 0.0)
        
        prediction_error = actual_reward - predicted_reward
        
        return prediction_error
    
    def update_dopamine_level(self, prediction_error, time_step):
        """Update dopamine level based on prediction error."""
        # Phasic response to prediction error
        phasic_change = prediction_error * self.dopamine_params['phasic_gain']
        
        # Tonic decay
        tonic_decay = self.current_dopamine_level * self.dopamine_params['tonic_decay_rate'] * time_step
        
        # Update level
        self.current_dopamine_level += phasic_change - tonic_decay
        
        # Ensure level is within bounds
        self.current_dopamine_level = np.clip(self.current_dopamine_level, 0.0, 2.0)
    
    def modulate_neural_state(self, neural_state, modulation_strength):
        """Apply dopamine modulation to neural state."""
        # This would modify neural properties based on dopamine level
        # For now, return the modified state
        if isinstance(neural_state, dict):
            modified_state = neural_state.copy()
            modified_state['dopamine_modulation'] = modulation_strength
            modified_state['dopamine_level'] = self.current_dopamine_level
            return modified_state
        
        return neural_state
    
    def get_description(self):
        return "Dopamine neuromodulation system for reward prediction and learning"
    
    def get_author(self):
        return "NLM Neuromodulation Team"

# Example Curiosity Plugin
class CuriosityModulatorPlugin(NeuromodulationPlugin):
    """Curiosity neuromodulation plugin for NLM."""
    
    def __init__(self, curiosity_params=None):
        self.curiosity_params = curiosity_params or {
            'baseline_curiosity': 0.15,
            'novelty_sensitivity': 0.8,
            'exploration_bonus': 0.5,
            'information_gain_weight': 0.7,
            'action_cost': 0.01,
            'curiosity_decay_rate': 0.02,
            'habituation_rate': 0.1
        }
        self.current_curiosity_level = 0.15
        self.recent_experiences = []
        self.habituated_patterns = {}
        
    def get_modulator_type(self):
        return 'curiosity'
    
    def get_modulator_config(self):
        return self.curiosity_params
    
    def apply_modulation(self, neural_state, input_signals):
        """Apply curiosity modulation to neural state."""
        # Calculate curiosity level based on input
        curiosity_level = self.calculate_curiosity_level(input_signals)
        
        # Apply modulation to neural state
        modulated_state = self.modulate_with_curiosity(neural_state, curiosity_level)
        
        return modulated_state
    
    def update_modulation(self, time_step, external_inputs):
        """Update curiosity modulator state."""
        # Update recent experiences
        self.update_experiences(external_inputs)
        
        # Apply habituation
        self.apply_habituation()
        
        # Decay curiosity
        self.current_curiosity_level *= (1.0 - self.curiosity_params['curiosity_decay_rate'] * time_step)
        
        return self.current_curiosity_level
    
    def calculate_curiosity_level(self, input_signals):
        """Calculate curiosity level based on input."""
        # Calculate information gain from input
        information_gain = self.calculate_information_gain(input_signals)
        
        # Calculate novelty
        novelty = self.calculate_novelty(input_signals)
        
        # Combine information gain and novelty
        curiosity = (
            information_gain * self.curiosity_params['information_gain_weight'] +
            novelty * (1.0 - self.curiosity_params['information_gain_weight'])
        )
        
        # Add baseline curiosity
        curiosity += self.curiosity_params['baseline_curiosity']
        
        # Apply exploration bonus for novel inputs
        if novelty > self.curiosity_params['habituation_rate']:
            curiosity += self.curiosity_params['exploration_bonus']
        
        return min(1.0, curiosity)
    
    def update_experiences(self, external_inputs):
        """Update stored experiences for habituation."""
        # Create experience signature from input
        experience_signature = self.create_experience_signature(external_inputs)
        
        self.recent_experiences.append({
            'signature': experience_signature,
            'timestamp': time.time(),
            'novelty': self.calculate_novelty(external_inputs)
        })
        
        # Keep experiences bounded
        if len(self.recent_experiences) > 100:
            self.recent_experiences.pop(0)
    
    def calculate_information_gain(self, input_signals):
        """Calculate information gain from input."""
        # Simplified information gain calculation
        # In practice, this would be more sophisticated
        novelty = self.calculate_novelty(input_signals)
        prediction = input_signals.get('prediction', 0.0)
        
        return novelty * (1.0 - abs(prediction))
    
    def calculate_novelty(self, input_signals):
        """Calculate novelty of input."""
        # Check against recent experiences
        if not self.recent_experiences:
            return 1.0
        
        input_signature = self.create_experience_signature(input_signals)
        
        # Calculate similarity to recent experiences
        similarities = []
        for experience in self.recent_experiences:
            similarity = self.calculate_experience_similarity(
                input_signature, experience['signature']
            )
            similarities.append(similarity)
        
        if similarities:
            avg_similarity = np.mean(similarities)
            novelty = 1.0 - avg_similarity
            return max(0.0, min(1.0, novelty))
        
        return 1.0
    
    def create_experience_signature(self, input_signals):
        """Create signature for experience storage."""
        # Create simple signature from input features
        features = []
        
        for key, value in input_signals.items():
            if isinstance(value, (int, float)):
                features.append((key, value))
        
        # Sort for consistent signature
        features.sort(key=lambda x: x[0])
        
        return tuple(features)
    
    def calculate_experience_similarity(self, sig1, sig2):
        """Calculate similarity between two experience signatures."""
        # Simple similarity calculation
        if not sig1 or not sig2:
            return 0.0
        
        # Convert to sets for comparison
        set1 = set(sig1)
        set2 = set(sig2)
        
        # Jaccard similarity
        intersection = len(set1 & set2)
        union = len(set1 | set2)
        
        return intersection / union if union > 0 else 0.0
    
    def apply_habituation(self):
        """Apply habituation to reduce curiosity for familiar inputs."""
        # Remove old experiences
        current_time = time.time()
        self.recent_experiences = [
            exp for exp in self.recent_experiences
            if current_time - exp['timestamp'] < 1000.0  # Keep last 1000 seconds
        ]
    
    def modulate_with_curiosity(self, neural_state, curiosity_level):
        """Modulate neural state with curiosity."""
        if isinstance(neural_state, dict):
            modified_state = neural_state.copy()
            modified_state['curiosity_modulation'] = curiosity_level
            modified_state['curiosity_level'] = self.current_curiosity_level
            
            # Apply curiosity effects to neural properties
            if 'learning_rate' in modified_state:
                modified_state['learning_rate'] *= (1.0 + curiosity_level * 0.5)
            
            return modified_state
        
        return neural_state
    
    def get_description(self):
        return "Curiosity-driven exploration and novelty detection system"
    
    def get_author(self):
        return "NLM Cognitive Science Team"

# Plugin System Integration
class NLMPluginSystem:
    """Main plugin system for NLM."""
    
    def __init__(self):
        self.sensory_plugin_manager = PluginManager()
        self.neuromodulation_plugin_manager = PluginManager()
        self.interface_registry = PluginInterfaceRegistry()
        
        # Initialize default interfaces
        self._initialize_interfaces()
        
    def _initialize_interfaces(self):
        """Initialize plugin interfaces."""
        # Register interfaces
        self.interface_registry.register_interface('SensoryPlugin', SensoryPlugin)
        self.interface_registry.register_interface('NeuromodulationPlugin', NeuromodulationPlugin)
        
        # Register default implementations
        self.interface_registry.register_implementation(
            'SensoryPlugin',
            ComputerVisionPlugin
        )
        self.interface_registry.register_implementation(
            'SensoryPlugin',
            LidarPlugin
        )
        
        self.interface_registry.register_implementation(
            'NeuromodulationPlugin',
            DopamineModulatorPlugin
        )
        self.interface_registry.register_implementation(
            'NeuromodulationPlugin',
            CuriosityModulatorPlugin
        )
    
    def register_sensory_plugin(self, plugin_instance):
        """Register sensory plugin."""
        return self.sensory_plugin_manager.register_plugin(plugin_instance)
    
    def register_neuromodulation_plugin(self, plugin_instance):
        """Register neuromodulation plugin."""
        return self.neuromodulation_plugin_manager.register_plugin(plugin_instance)
    
    def create_sensory_plugin(self, plugin_type, **kwargs):
        """Create sensory plugin instance."""
        return self.interface_registry.create_implementation(
            'SensoryPlugin', **kwargs
        )
    
    def create_neuromodulation_plugin(self, plugin_type, **kwargs):
        """Create neuromodulation plugin instance."""
        return self.interface_registry.create_implementation(
            'NeuromodulationPlugin', **kwargs
        )
    
    def get_active_sensory_plugins(self):
        """Get all active sensory plugins."""
        return self.sensory_plugin_manager.get_plugins()
    
    def get_active_neuromodulation_plugins(self):
        """Get all active neuromodulation plugins."""
        return self.neuromodulation_plugin_manager.get_plugins()
    
    def update_plugins(self, time_step, global_inputs):
        """Update all plugins with global inputs."""
        # Update sensory plugins
        for plugin in self.sensory_plugin_manager.get_plugins():
            if hasattr(plugin, 'update'):
                plugin.update(time_step, global_inputs)
        
        # Update neuromodulation plugins
        for plugin in self.neuromodulation_plugin_manager.get_plugins():
            if hasattr(plugin, 'update'):
                plugin.update(time_step, global_inputs)
    
    def get_system_state(self):
        """Get overall plugin system state."""
        return {
            'sensory_plugins': len(self.sensory_plugin_manager.get_plugins()),
            'neuromodulation_plugins': len(self.neuromodulation_plugin_manager.get_plugins()),
            'interfaces': list(self.interface_registry.interfaces.keys())
        }

# Usage Example
if __name__ == "__main__":
    # Create NLM plugin system
    plugin_system = NLMPluginSystem()
    
    # Create and register custom plugins
    custom_vision_plugin = ComputerVisionPlugin({
        'resolution': (640, 480),
        'fps': 30
    })
    
    custom_curiosity_plugin = CuriosityModulatorPlugin({
        'baseline_curiosity': 0.2,
        'novelty_sensitivity': 0.9
    })
    
    # Register plugins
    plugin_system.register_sensory_plugin(custom_vision_plugin)
    plugin_system.register_neuromodulation_plugin(custom_curiosity_plugin)
    
    # Get system state
    system_state = plugin_system.get_system_state()
    print(f"Plugin system state: {system_state}")
    
    # Get active plugins
    vision_plugins = plugin_system.get_active_sensory_plugins()
    neuromodulation_plugins = plugin_system.get_active_neuromodulation_plugins()
    
    print(f"Active vision plugins: {len(vision_plugins)}")
    print(f"Active neuromodulation plugins: {len(neuromodulation_plugins)}")
    
    # Update plugins
    plugin_system.update_plugins(0.1, {
        'reward': 0.5,
        'prediction_error': 0.1,
        'novelty': 0.3
    })
```

### Plugin Architecture Benefits

1. **Modularity**: Add new functionality without modifying core NLM code
2. **Extensibility**: Community can develop and share plugins
3. **Testing**: Plugins can be tested independently
4. **Performance**: Plugins can be enabled/disabled based on performance needs
5. **Maintainability**: Changes to plugins don't affect core system
6. **Customization**: Users can tailor NLM to specific use cases

### Plugin Development Best Practices

1. **Clear Interface Definition**: Well-defined abstract base classes
2. **Error Handling**: Robust error handling and validation
3. **Documentation**: Comprehensive plugin documentation
4. **Configuration Management**: Support for plugin configuration
5. **Resource Management**: Proper cleanup and resource release
6. **Logging**: Adequate logging for debugging and monitoring

## 2. Batch Processing

### Overview
Batch processing allows multiple NLM simulations to run efficiently, enabling large-scale experiments and data analysis.

### Pattern 1: Batch Simulation Manager

```python
# batch/batch_simulation_manager.py
import pynlm
import numpy as np
import time
import multiprocessing as mp
from concurrent.futures import ProcessPoolExecutor, ThreadPoolExecutor
from typing import List, Dict, Callable, Any
import logging

class BatchSimulationManager:
    """Manages batch processing of NLM simulations."""
    
    def __init__(self, config_template=None, num_workers=None):
        self.config_template = config_template or pynlm.createDefaultConfig()
        self.num_workers = num_workers or mp.cpu_count()
        self.simulation_queue = mp.Queue()
        self.result_queue = mp.Queue()
        self.active_workers = []
        self.completed_simulations = []
        self.failed_simulations = []
        
        # Setup logging
        logging.basicConfig(level=logging.INFO)
        self.logger = logging.getLogger(__name__)
        
        # Performance tracking
        self.performance_metrics = {
            'total_simulations': 0,
            'completed_simulations': 0,
            'failed_simulations': 0,
            'total_time': 0.0,
            'avg_simulation_time': 0.0,
            'parallel_efficiency': 0.0
        }
        
    def create_simulation_config(self, simulation_id: str, parameters: Dict[str, Any] = None):
        """Create configuration for a specific simulation."""
        config = pynlm.createDefaultConfig()
        
        # Copy base configuration
        if self.config_template:
            base_config = self.config_template.summary() if hasattr(self.config_template, 'summary') else str(self.config_template)
            # Parse base config and apply parameters
            config = self.parse_base_config(base_config)
        
        # Apply simulation-specific parameters
        if parameters:
            for key, value in parameters.items():
                config.set(key, value)
        
        # Add simulation-specific settings
        config.set("simulation.id", simulation_id)
        config.set("simulation.timestamp", time.time())
        config.set("simulation.batch_id", id(self))
        
        return config
    
    def parse_base_config(self, base_config_str):
        """Parse base configuration string."""
        # Simplified parsing - in practice, would use proper config format
        config = pynlm.createDefaultConfig()
        
        # Parse common parameters
        import re
        patterns = {
            'neuron_count': r'neuron_count[:\s]*([0-9]+)',
            'synapse_density': r'synapse_density[:\s]*([0-9.]+)',
            'v_thresh': r'v_thresh[:\s]*([-0-9.]+)',
            'tau_mem': r'tau_mem[:\s]*([0-9.]+)'
        }
        
        for param, pattern in patterns.items():
            match = re.search(pattern, base_config_str)
            if match:
                config.set(param, float(match.group(1)))
        
        return config
    
    def run_simulation(self, simulation_id: str, parameters: Dict[str, Any] = None):
        """Run a single simulation."""
        try:
            # Create configuration
            config = self.create_simulation_config(simulation_id, parameters)
            
            # Create and run simulation
            simulation_start_time = time.time()
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Create agent
            agent = pynlm.createAgentBrain(brain)
            
            # Create world
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            
            # Initialize agent
            agent.initialize(world)
            
            # Enable learning
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            
            # Run simulation
            simulation_results = self.execute_simulation(
                brain, agent, world, simulation_id, parameters
            )
            
            simulation_time = time.time() - simulation_start_time
            
            # Prepare results
            result = {
                'simulation_id': simulation_id,
                'parameters': parameters,
                'success': True,
                'simulation_time': simulation_time,
                'start_time': simulation_start_time,
                'results': simulation_results,
                'brain_stats': self.collect_brain_stats(brain, simulation_id),
                'agent_stats': self.collect_agent_stats(agent, simulation_id),
                'error': None
            }
            
            self.logger.info(f"Simulation {simulation_id} completed in {simulation_time:.2f}s")
            
            return result
            
        except Exception as e:
            self.logger.error(f"Simulation {simulation_id} failed: {str(e)}")
            
            result = {
                'simulation_id': simulation_id,
                'parameters': parameters,
                'success': False,
                'simulation_time': time.time() - simulation_start_time,
                'start_time': simulation_start_time,
                'results': None,
                'brain_stats': None,
                'agent_stats': None,
                'error': str(e)
            }
            
            return result
    
    def execute_simulation(self, brain, agent, world, simulation_id: str, parameters: Dict[str, Any]):
        """Execute the core simulation loop."""
        # Simulation configuration
        num_steps = parameters.get('num_steps', 1000) if parameters else 1000
        random_seed = parameters.get('random_seed', None) if parameters else None
        
        # Set random seed if provided
        if random_seed is not None:
            np.random.seed(random_seed)
        
        # Initialize tracking
        simulation_data = {
            'steps': list(range(num_steps)),
            'rewards': [],
            'brain_metrics': {
                'firing_rates': [],
                'neuron_counts': [],
                'synapse_counts': [],
                'spike_counts': []
            },
            'agent_metrics': {
                'curiosity_levels': [],
                'prediction_errors': [],
                'neuromodulation_levels': []
            },
            'world_metrics': {
                'positions': [],
                'velocities': []
            },
            'learning_metrics': {
                'reward_prediction_errors': [],
                'curiosity_values': [],
                'development_stages': []
            }
        }
        
        # Main simulation loop
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain computation
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Calculate reward
            reward = self.calculate_simulation_reward(percept, action, step)
            simulation_data['rewards'].append(reward)
            
            # Apply neuromodulation
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Collect metrics
            self.collect_simulation_metrics(brain, agent, world, step, simulation_data)
            
            # Print progress
            if step % 100 == 0:
                self.logger.info(f"Simulation {simulation_id} at step {step}, reward: {reward:.3f}")
        
        return simulation_data
    
    def calculate_simulation_reward(self, percept, action, step):
        """Calculate reward for simulation step."""
        # Base reward calculation
        reward = 0.1
        
        # Action-based reward
        if action is not None:
            reward += 0.1
        
        # Percept-based reward
        if percept is not None:
            reward += np.random.uniform(-0.05, 0.05)
        
        return np.clip(reward, -1.0, 1.0)
    
    def collect_simulation_metrics(self, brain, agent, world, step, simulation_data):
        """Collect metrics from simulation components."""
        # Brain metrics
        simulation_data['brain_metrics']['firing_rates'].append(brain.getAverageFiringRate())
        simulation_data['brain_metrics']['neuron_counts'].append(brain.getTotalNeuronCount())
        simulation_data['brain_metrics']['synapse_counts'].append(brain.getTotalSynapseCount())
        simulation_data['brain_metrics']['spike_counts'].append(brain.getTotalSpikeCount())
        
        # Agent metrics
        simulation_data['agent_metrics']['curiosity_levels'].append(agent.getCuriosityLevel())
        simulation_data['agent_metrics']['prediction_errors'].append(agent.getPredictionError())
        simulation_data['agent_metrics']['neuromodulation_levels'].append(agent.getNeuromodulationLevel())
        
        # World metrics
        agent_body = world.getAgentBody()
        if agent_body:
            simulation_data['world_metrics']['positions'].append([agent_body.x, agent_body.y])
            simulation_data['world_metrics']['velocities'].append([agent_body.velocityX, agent_body.velocityY])
        
        # Learning metrics
        simulation_data['learning_metrics']['reward_prediction_errors'].append(agent.getPredictionError())
        simulation_data['learning_metrics']['curiosity_values'].append(agent.getCuriosityLevel())
        
        developmental_stage = agent.getDevelopmentalStage()
        simulation_data['learning_metrics']['development_stages'].append(
            developmental_stage.value if hasattr(developmental_stage, 'value') else str(developmental_stage)
        )
    
    def collect_brain_stats(self, brain, simulation_id: str):
        """Collect brain statistics for batch."""
        return {
            'simulation_id': simulation_id,
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'total_spike_count': brain.getTotalSpikeCount(),
            'developmental_stage': brain.getDevelopmentalStage().value if hasattr(brain.getDevelopmentalStage(), 'value') else str(brain.getDevelopmentalStage())
        }
    
    def collect_agent_stats(self, agent, simulation_id: str):
        """Collect agent statistics for batch."""
        return {
            'simulation_id': simulation_id,
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'developmental_stage': agent.getDevelopmentalStage().value if hasattr(agent.getDevelopmentalStage(), 'value') else str(agent.getDevelopmentalStage()),
            'reward_modulation_enabled': agent.isRewardModulationEnabled(),
            'curiosity_enabled': agent.isCuriosityEnabled(),
            'development_enabled': agent.isDevelopmentEnabled()
        }
    
    def run_batch_sims(self, simulations: List[Dict], use_parallel: bool = True):
        """Run batch of simulations."""
        print(f"Starting batch processing of {len(simulations)} simulations")
        print(f"Using {'parallel' if use_parallel else 'sequential'} execution")
        
        batch_start_time = time.time()
        
        if use_parallel and len(simulations) > 1:
            # Run in parallel
            results = self.run_parallel_batch(simulations)
        else:
            # Run sequentially
            results = self.run_sequential_batch(simulations)
        
        batch_time = time.time() - batch_start_time
        
        # Update performance metrics
        self.update_performance_metrics(len(simulations), batch_time, results)
        
        return results
    
    def run_sequential_batch(self, simulations: List[Dict]):
        """Run batch simulations sequentially."""
        results = []
        
        for i, simulation in enumerate(simulations):
            simulation_id = simulation.get('id', f'sim_{i}')
            parameters = simulation.get('parameters', {})
            
            print(f"Running simulation {i + 1}/{len(simulations)}: {simulation_id}")
            
            result = self.run_simulation(simulation_id, parameters)
            results.append(result)
            
            if result['success']:
                self.completed_simulations.append(result)
            else:
                self.failed_simulations.append(result)
        
        return results
    
    def run_parallel_batch(self, simulations: List[Dict]):
        """Run batch simulations in parallel."""
        results = []
        
        # Create process pool
        with ProcessPoolExecutor(max_workers=self.num_workers) as executor:
            # Submit all simulations
            future_to_simulation = {
                executor.submit(self.run_simulation, sim.get('id', f'sim_{i}'), sim.get('parameters', {})): sim
                for i, sim in enumerate(simulations)
            }
            
            # Collect results as they complete
            for i, (future, simulation) in enumerate(zip(future_to_simulation.keys(), simulations)):
                print(f"Running simulation {i + 1}/{len(simulations)} (parallel)")
                
                try:
                    result = future.result()
                    results.append(result)
                    
                    if result['success']:
                        self.completed_simulations.append(result)
                    else:
                        self.failed_simulations.append(result)
                        
                except Exception as e:
                    self.logger.error(f"Parallel simulation failed: {str(e)}")
                    
                    result = {
                        'simulation_id': simulation.get('id', f'sim_{i}'),
                        'parameters': simulation.get('parameters', {}),
                        'success': False,
                        'simulation_time': 0.0,
                        'start_time': time.time(),
                        'results': None,
                        'brain_stats': None,
                        'agent_stats': None,
                        'error': str(e)
                    }
                    
                    results.append(result)
                    self.failed_simulations.append(result)
        
        return results
    
    def update_performance_metrics(self, total_simulations: int, batch_time: float, results: List[Dict]):
        """Update performance metrics."""
        successful_sims = len(self.completed_simulations)
        failed_sims = len(self.failed_simulations)
        
        simulation_times = [r['simulation_time'] for r in results if r['success']]
        
        self.performance_metrics.update({
            'total_simulations': total_simulations,
            'completed_simulations': successful_sims,
            'failed_simulations': failed_sims,
            'total_time': batch_time,
            'avg_simulation_time': np.mean(simulation_times) if simulation_times else 0.0,
            'parallel_efficiency': self.calculate_parallel_efficiency(batch_time, simulation_times, total_simulations)
        })
    
    def calculate_parallel_efficiency(self, total_time: float, individual_times: List[float], num_sims: int):
        """Calculate parallel efficiency."""
        if not individual_times:
            return 0.0
        
        # Calculate ideal time (sequential execution)
        ideal_time = sum(individual_times)
        
        # Calculate efficiency
        efficiency = ideal_time / (total_time * num_sims) if total_time > 0 else 0.0
        
        return min(1.0, efficiency)  # Cap at 1.0
    
    def analyze_batch_results(self, results: List[Dict]):
        """Analyze batch simulation results."""
        if not results:
            return {}
        
        analysis = {
            'success_rate': len(self.completed_simulations) / max(1, len(results)),
            'average_simulation_time': self.performance_metrics['avg_simulation_time'],
            'total_batch_time': self.performance_metrics['total_time'],
            'parallel_efficiency': self.performance_metrics['parallel_efficiency']
        }
        
        # Analyze successful simulations
        successful_results = [r for r in results if r['success']]
        
        if successful_results:
            # Aggregate metrics
            all_rewards = []
            all_firing_rates = []
            all_curiosity_levels = []
            
            for result in successful_results:
                if result['results']:
                    all_rewards.extend(result['results']['rewards'])
                    all_firing_rates.extend(result['results']['brain_metrics']['firing_rates'])
                    all_curiosity_levels.extend(result['results']['agent_metrics']['curiosity_levels'])
            
            analysis['aggregated_metrics'] = {
                'mean_reward': np.mean(all_rewards) if all_rewards else 0.0,
                'std_reward': np.std(all_rewards) if all_rewards else 0.0,
                'mean_firing_rate': np.mean(all_firing_rates) if all_firing_rates else 0.0,
                'mean_curiosity': np.mean(all_curiosity_levels) if all_curiosity_levels else 0.0,
                'total_simulations_analyzed': len(successful_results)
            }
        
        return analysis
    
    def save_batch_results(self, results: List[Dict], filename: str):
        """Save batch results to file."""
        import json
        
        # Convert results to JSON-serializable format
        json_results = self.convert_results_to_json(results)
        
        # Add metadata
        experiment_metadata = {
            'timestamp': time.time(),
            'batch_id': id(self),
            'num_simulations': len(results),
            'num_workers': self.num_workers,
            'parallel_execution': True,
            'performance_metrics': self.performance_metrics
        }
        
        output_data = {
            'metadata': experiment_metadata,
            'results': json_results,
            'analysis': self.analyze_batch_results(results)
        }
        
        with open(filename, 'w') as f:
            json.dump(output_data, f, indent=2, default=str)
        
        self.logger.info(f"Batch results saved to {filename}")
    
    def convert_results_to_json(self, results: List[Dict]):
        """Convert results to JSON-serializable format."""
        json_results = []
        
        for result in results:
            json_result = result.copy()
            
            # Convert simulation data
            if json_result['results']:
                for key, value in json_result['results'].items():
                    if isinstance(value, np.ndarray):
                        json_result['results'][key] = value.tolist()
                    elif isinstance(value, dict):
                        json_result['results'][key] = self._convert_dict_to_json(value)
            
            json_results.append(json_result)
        
        return json_results
    
    def _convert_dict_to_json(self, data):
        """Recursively convert dictionary to JSON-serializable format."""
        if isinstance(data, dict):
            return {key: self._convert_dict_to_json(value) for key, value in data.items()}
        elif isinstance(data, list):
            return [self._convert_dict_to_json(item) for item in data]
        elif isinstance(data, np.ndarray):
            return data.tolist()
        elif isinstance(data, (np.integer, np.floating)):
            return float(data)
        else:
            return data
    
    def get_performance_report(self):
        """Get performance report for batch processing."""
        report = {
            'overview': self.performance_metrics.copy(),
            'simulation_count': {
                'total': self.performance_metrics['total_simulations'],
                'completed': self.performance_metrics['completed_simulations'],
                'failed': self.performance_metrics['failed_simulations']
            },
            'efficiency_analysis': {
                'parallel_efficiency': self.performance_metrics['parallel_efficiency'],
                'average_time_per_simulation': self.performance_metrics['avg_simulation_time'],
                'total_batch_time': self.performance_metrics['total_time']
            }
        }
        
        return report

# Usage Example
if __name__ == "__main__":
    # Create batch simulation manager
    batch_manager = BatchSimulationManager(num_workers=4)
    
    # Define batch of simulations
    simulations = [
        {
            'id': 'sim_small_neural',
            'parameters': {
                'neuron_count': 1000,
                'synapse_density': 0.05,
                'num_steps': 500,
                'random_seed': 42
            }
        },
        {
            'id': 'sim_medium_neural',
            'parameters': {
                'neuron_count': 3000,
                'synapse_density': 0.06,
                'num_steps': 1000,
                'random_seed': 43
            }
        },
        {
            'id': 'sim_large_neural',
            'parameters': {
                'neuron_count': 5000,
                'synapse_density': 0.07,
                'num_steps': 1500,
                'random_seed': 44
            }
        }
    ]
    
    # Run batch simulations
    results = batch_manager.run_batch_sims(simulations, use_parallel=True)
    
    # Analyze results
    analysis = batch_manager.analyze_batch_results(results)
    
    print("\n" + "=" * 60)
    print("Batch Simulation Results")
    print("=" * 60)
    
    print(f"Success Rate: {analysis['success_rate'] * 100:.1f}%")
    print(f"Average Simulation Time: {analysis['average_simulation_time']:.2f}s")
    print(f"Total Batch Time: {analysis['total_batch_time']:.2f}s")
    print(f"Parallel Efficiency: {analysis['parallel_efficiency']:.2f}")
    
    if 'aggregated_metrics' in analysis:
        print(f"\nAggregated Metrics:")
        print(f"  Mean Reward: {analysis['aggregated_metrics']['mean_reward']:.3f}")
        print(f"  Mean Firing Rate: {analysis['aggregated_metrics']['mean_firing_rate']:.3f}")
        print(f"  Mean Curiosity: {analysis['aggregated_metrics']['mean_curiosity']:.3f}")
        print(f"  Simulations Analyzed: {analysis['aggregated_metrics']['total_simulations_analyzed']}")
    
    # Get performance report
    report = batch_manager.get_performance_report()
    print(f"\nPerformance Report:")
    print(f"  Total Simulations: {report['overview']['total_simulations']}")
    print(f"  Completed: {report['overview']['completed_simulations']}")
    print(f"  Failed: {report['overview']['failed_simulations']}")
    print(f"  Parallel Efficiency: {report['efficiency_analysis']['parallel_efficiency']:.2f}")
    
    # Save results
    batch_manager.save_batch_results(results, "batch_simulation_results.json")
```