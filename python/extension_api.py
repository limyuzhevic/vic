"""
Extension API for NLM - Plugin Architecture and Custom Components

This module provides extension APIs for creating custom plugins and components
for the NLM system. It enables power users to extend the system with custom
neural architectures, learning algorithms, and neuromodulation patterns.
"""

import abc
import importlib
from typing import Dict, List, Optional, Any, Type, Union
from dataclasses import dataclass, field
from pathlib import Path
import yaml
import json
from abc import ABC, abstractmethod
import pynlm

# Base plugin interfaces
class NLMPipelineComponent(ABC):
    """Abstract base class for all NLM pipeline components."""
    
    @property
    @abstractmethod
    def name(self) -> str:
        """Return the component name."""
        pass
    
    @property
    @abstractmethod
    def version(self) -> str:
        """Return the component version."""
        pass
    
    @abstractmethod
    def initialize(self, config: Dict[str, Any]) -> bool:
        """
        Initialize the component with configuration.
        
        Args:
            config: Configuration dictionary
            
        Returns:
            True if initialization succeeded, False otherwise
        """
        pass
    
    @abstractmethod
    def process(self, input_data: Any) -> Any:
        """
        Process input data.
        
        Args:
            input_data: Input data to process
            
        Returns:
            Processed output
        """
        pass
    
    def cleanup(self):
        """Clean up component resources."""
        pass
    
    def get_info(self) -> Dict[str, Any]:
        """Get component information."""
        return {
            "name": self.name,
            "version": self.version,
            "type": self.__class__.__name__
        }
class NeuralArchitecturePlugin(NLMPipelineComponent):
    """Base class for neural architecture plugins."""
    
    @abstractmethod
    def build_neural_network(self, config: Dict[str, Any]) -> Any:
        """
        Build a neural network using the plugin's architecture.
        
        Args:
            config: Architecture configuration
            
        Returns:
            Neural network instance
        """
        pass
    
    @abstractmethod
    def get_architecture_description(self) -> str:
        """Get description of the neural architecture."""
        pass
class LearningAlgorithmPlugin(NLMPipelineComponent):
    """Base class for learning algorithm plugins."""
    
    @abstractmethod
    def compute_gradient(self, input_data: Any, target: Any) -> Dict[str, Any]:
        """
        Compute gradients for learning.
        
        Args:
            input_data: Input data
            target: Target values
            
        Returns:
            Dictionary containing gradients and auxiliary information
        """
        pass
    
    @abstractmethod
    def update_parameters(self, gradients: Dict[str, Any], learning_rate: float):
        """
        Update parameters based on gradients.
        
        Args:
            gradients: Computed gradients
            learning_rate: Learning rate
        """
        pass
class NeuromodulationPlugin(NLMPipelineComponent):
    """Base class for neuromodulation plugins."""
    
    @abstractmethod
    def compute_modulation_signals(self, brain_state: Dict[str, Any],
                                  context: Dict[str, Any]) -> Dict[str, float]:
        """
        Compute neuromodulation signals.
        
        Args:
            brain_state: Current brain state
            context: Context information
            
        Returns:
            Dictionary of neuromodulation signals
        """
        pass
    
    @abstractmethod
    def update_dynamics(self, signals: Dict[str, float], timestep: float):
        """
        Update neuromodulation dynamics.
        
        Args:
            signals: Current neuromodulation signals
            timestep: Time step
        """
        pass
class ExtensionAPI:
    """
    Main extension API for NLM plugins and custom components.
    
    This API provides functionality for:
    - Plugin registration and management
    - Custom component creation
    - Configuration management for plugins
    - Plugin discovery and loading
    - Extension point integration with NLM core
    """
    
    def __init__(self, plugin_directory: Optional[str] = None):
        self.plugin_directory = plugin_directory or "./nlm_plugins"
        self.plugins: Dict[str, NLMPipelineComponent] = {}
        self.plugin_configs: Dict[str, Dict[str, Any]] = {}
        self.custom_components: Dict[str, Any] = {}
        self.architecture_plugins: Dict[str, NeuralArchitecturePlugin] = {}
        self.learning_plugins: Dict[str, LearningAlgorithmPlugin] = {}
        self.neuromodulation_plugins: Dict[str, NeuromodulationPlugin] = {}
        
        Path(self.plugin_directory).mkdir(exist_ok=True)
    
    def register_plugin(self, plugin: NLMPipelineComponent,
                       config: Optional[Dict[str, Any]] = None,
                       plugin_type: Optional[str] = None):
        """
        Register a plugin with the extension API.
        
        Args:
            plugin: Plugin instance to register
            config: Configuration for the plugin
            plugin_type: Type of plugin (auto-detected if not provided)
        """
        plugin_name = plugin.name
        
        # Auto-detect plugin type if not specified
        if plugin_type is None:
            if isinstance(plugin, NeuralArchitecturePlugin):
                plugin_type = "architecture"
            elif isinstance(plugin, LearningAlgorithmPlugin):
                plugin_type = "learning"
            elif isinstance(plugin, NeuromodulationPlugin):
                plugin_type = "neuromodulation"
            else:
                plugin_type = "generic"
        
        self.plugins[plugin_name] = plugin
        self.plugin_configs[plugin_name] = config or {}
        
        # Add to appropriate category
        if plugin_type == "architecture" and isinstance(plugin, NeuralArchitecturePlugin):
            self.architecture_plugins[plugin_name] = plugin
        elif plugin_type == "learning" and isinstance(plugin, LearningAlgorithmPlugin):
            self.learning_plugins[plugin_name] = plugin
        elif plugin_type == "neuromodulation" and isinstance(plugin, NeuromodulationPlugin):
            self.neuromodulation_plugins[plugin_name] = plugin
        
        # Initialize plugin if config provided
        if config:
            try:
                plugin.initialize(config)
            except Exception as e:
                print(f"Warning: Failed to initialize plugin {plugin_name}: {e}")
    
    def load_plugin_from_file(self, filepath: str, config: Optional[Dict[str, Any]] = None,
                            plugin_type: Optional[str] = None) -> bool:
        """
        Load a plugin from a Python file.
        
        Args:
            filepath: Path to plugin file
            config: Configuration for the plugin
            plugin_type: Type of plugin
            
        Returns:
            True if plugin loaded successfully, False otherwise
        """
        try:
            # Import the plugin module
            module_name = Path(filepath).stem
            spec = importlib.util.spec_from_file_location(module_name, filepath)
            module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(module)
            
            # Look for plugin class in the module
            for attr_name in dir(module):
                attr = getattr(module, attr_name)
                if (isinstance(attr, type) and 
                    issubclass(attr, NLMPipelineComponent) and 
                    attr != NLMPipelineComponent):
                    
                    plugin_instance = attr()
                    self.register_plugin(plugin_instance, config, plugin_type)
                    return True
            
            return False
            
        except Exception as e:
            print(f"Error loading plugin from {filepath}: {e}")
            return False
    
    def create_custom_architecture(self, name: str, **kwargs) -> Any:
        """
        Create a custom neural architecture.
        
        Args:
            name: Name of the architecture
            **kwargs: Architecture parameters
            
        Returns:
            Custom architecture instance
        """
        # Look for architecture plugin
        if self.architecture_plugins:
            for plugin_name, plugin in self.architecture_plugins.items():
                try:
                    return plugin.build_neural_network(kwargs)
                except Exception as e:
                    print(f"Error using plugin {plugin_name}: {e}")
                    continue
        
        # Fallback to NLM's default with customizations
        config = pynlm.createExperimentConfig()
        
        # Apply custom parameters
        if "neuron_count" in kwargs:
            config.set("brain.neuron_count", kwargs["neuron_count"])
        if "region_count" in kwargs:
            config.set("brain.region_count", kwargs["region_count"])
        if "connection_probability" in kwargs:
            config.set("brain.connection_probability", kwargs["connection_probability"])
        
        return pynlm.createBrain(config)
    
    def create_custom_learning_algorithm(self, name: str, **kwargs) -> LearningAlgorithmPlugin:
        """
        Create a custom learning algorithm.
        
        Args:
            name: Name of the learning algorithm
            **kwargs: Algorithm parameters
            
        Returns:
            Learning algorithm instance
        """
        # Look for learning plugin
        if self.learning_plugins:
            for plugin_name, plugin in self.learning_plugins.items():
                try:
                    if plugin.name.lower() == name.lower():
                        plugin.initialize(kwargs)
                        return plugin
                except Exception as e:
                    print(f"Error initializing plugin {plugin_name}: {e}")
                    continue
        
        # Create a custom learning algorithm
        class CustomLearningAlgorithm(LearningAlgorithmPlugin):
            def __init__(self, params: Dict[str, Any]):
                self.name = name
                self.version = "1.0"
                self.params = params
                self.initialized = False
            
            @property
            def name(self) -> str:
                return self._name
            
            @name.setter
            def name(self, value: str):
                self._name = value
            
            @property
            def version(self) -> str:
                return self._version
            
            @version.setter
            def version(self, value: str):
                self._version = value
            
            def initialize(self, config: Dict[str, Any]) -> bool:
                self.params.update(config)
                self.initialized = True
                return True
            
            def process(self, input_data: Any) -> Any:
                # Custom processing logic
                return input_data * self.params.get("multiplier", 1.0)
            
            def compute_gradient(self, input_data: Any, target: Any) -> Dict[str, Any]:
                gradient = (target - input_data) * self.params.get("learning_rate", 0.01)
                return {"gradient": gradient, "error": abs(gradient)}
            
            def update_parameters(self, gradients: Dict[str, Any], learning_rate: float):
                self.params["last_gradient"] = gradients.get("gradient", 0)
        
        return CustomLearningAlgorithm(kwargs)
    
    def create_custom_neuromodulation(self, name: str, **kwargs) -> NeuromodulationPlugin:
        """
        Create a custom neuromodulation pattern.
        
        Args:
            name: Name of the neuromodulation pattern
            **kwargs: Pattern parameters
            
        Returns:
            Neuromodulation plugin instance
        """
        # Look for neuromodulation plugin
        if self.neuromodulation_plugins:
            for plugin_name, plugin in self.neuromodulation_plugins.items():
                try:
                    if plugin.name.lower() == name.lower():
                        plugin.initialize(kwargs)
                        return plugin
                except Exception as e:
                    print(f"Error initializing plugin {plugin_name}: {e}")
                    continue
        
        # Create a custom neuromodulation pattern
        class CustomNeuromodulation(NeuromodulationPlugin):
            def __init__(self, params: Dict[str, Any]):
                self.name = name
                self.version = "1.0"
                self.params = params
                self.signals = {}
                self.initialized = False
            
            @property
            def name(self) -> str:
                return self._name
            
            @name.setter
            def name(self, value: str):
                self._name = value
            
            @property
            def version(self) -> str:
                return self._version
            
            @version.setter
            def version(self, value: str):
                self._version = value
            
            def initialize(self, config: Dict[str, Any]) -> bool:
                self.params.update(config)
                self.initialized = True
                return True
            
            def process(self, input_data: Any) -> Any:
                return input_data
            
            def compute_modulation_signals(self, brain_state: Dict[str, Any],
                                          context: Dict[str, Any]) -> Dict[str, float]:
                # Custom neuromodulation logic
                signals = {}
                for key, value in self.params.items():
                    if key.startswith("signal_"):
                        signal_name = key[7:]
                        signals[signal_name] = value * context.get("activity", 1.0)
                
                return signals
            
            def update_dynamics(self, signals: Dict[str, float], timestep: float):
                # Update neuromodulation dynamics
                self.signals.update(signals)
                # Apply decay or other dynamics
                for key in self.signals:
                    self.signals[key] *= (1.0 - self.params.get("decay_rate", 0.01) * timestep)
        
        return CustomNeuromodulation(kwargs)
    
    def get_registered_plugins(self) -> Dict[str, Dict[str, Any]]:
        """
        Get information about all registered plugins.
        
        Returns:
            Dictionary containing plugin information
        """
        plugins_info = {}
        
        for name, plugin in self.plugins.items():
            plugins_info[name] = {
                "info": plugin.get_info(),
                "config": self.plugin_configs.get(name, {}),
                "type": self._get_plugin_type(plugin)
            }
        
        return plugins_info
    
    def _get_plugin_type(self, plugin: NLMPipelineComponent) -> str:
        """Determine plugin type."""
        if isinstance(plugin, NeuralArchitecturePlugin):
            return "neural_architecture"
        elif isinstance(plugin, LearningAlgorithmPlugin):
            return "learning_algorithm"
        elif isinstance(plugin, NeuromodulationPlugin):
            return "neuromodulation"
        else:
            return "generic"
    
    def export_plugin_config(self, plugin_name: str, filepath: str):
        """
        Export plugin configuration to file.
        
        Args:
            plugin_name: Name of the plugin
            filepath: Path to export file (JSON or YAML)
        """
        if plugin_name not in self.plugins:
            raise ValueError(f"Plugin {plugin_name} not found")
        
        config = {
            "plugin_info": self.plugins[plugin_name].get_info(),
            "configuration": self.plugin_configs.get(plugin_name, {})
        }
        
        if filepath.endswith('.json'):
            with open(filepath, 'w') as f:
                json.dump(config, f, indent=2, default=str)
        elif filepath.endswith('.yaml') or filepath.endswith('.yml'):
            with open(filepath, 'w') as f:
                yaml.dump(config, f, default_flow_style=False)
        else:
            raise ValueError("Unsupported file format. Use .json or .yaml/.yml")
    
    def import_plugin_config(self, filepath: str) -> Dict[str, Any]:
        """
        Import plugin configuration from file.
        
        Args:
            filepath: Path to import file (JSON or YAML)
            
        Returns:
            Imported configuration dictionary
        """
        if filepath.endswith('.json'):
            with open(filepath, 'r') as f:
                return json.load(f)
        elif filepath.endswith('.yaml') or filepath.endswith('.yml'):
            with open(filepath, 'r') as f:
                return yaml.safe_load(f)
        else:
            raise ValueError("Unsupported file format. Use .json or .yaml/.yml")
    
    def create_plugin_bundle(self, name: str, plugin_configs: Dict[str, Dict[str, Any]],
                           description: str = "") -> Dict[str, Any]:
        """
        Create a plugin bundle with multiple plugins.
        
        Args:
            name: Bundle name
            plugin_configs: Dictionary mapping plugin names to configurations
            description: Bundle description
            
        Returns:
            Bundle configuration dictionary
        """
        bundle = {
            "name": name,
            "description": description,
            "plugins": plugin_configs,
            "created_at": datetime.now().isoformat()
        }
        
        return bundle
    
    def save_plugin_bundle(self, bundle: Dict[str, Any], filepath: str):
        """
        Save plugin bundle to file.
        
        Args:
            bundle: Bundle dictionary
            filepath: Path to save bundle
        """
        with open(filepath, 'w') as f:
            json.dump(bundle, f, indent=2, default=str)
    
    def load_plugin_bundle(self, filepath: str) -> Dict[str, Any]:
        """
        Load plugin bundle from file.
        
        Args:
            filepath: Path to bundle file
            
        Returns:
            Loaded bundle dictionary
        """
        with open(filepath, 'r') as f:
            return json.load(f)
class ScriptedNLM:
    """
    Script-based NLM interface for automation and custom workflows.
    
    This class provides a scripting interface for NLM operations,
    allowing users to create custom workflows and automation scripts.
    """
    
    def __init__(self, extension_api: ExtensionAPI):
        self.extension_api = extension_api
        self.script_history = []
        
    def execute_script(self, script: str, variables: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """
        Execute a Python script with NLM context.
        
        Args:
            script: Python script to execute
            variables: Variables to make available in script
            
        Returns:
            Dictionary containing execution results
        """
        namespace = {
            "pynlm": pynlm,
            "nlm_features": AdvancedNLMFeatures(),
            "config_presets": AdvancedConfigurationPresets(),
            "profiler": PerformanceProfiler(),
            "extension_api": self.extension_api,
            **(variables or {})
        }
        
        # Add script to history
        script_record = {
            "timestamp": time.time(),
            "script": script,
            "variables": variables or {}
        }
        
        try:
            exec(script, namespace)
            result = namespace.get("result", {})
            script_record["success"] = True
            script_record["result"] = result
        except Exception as e:
            script_record["success"] = False
            script_record["error"] = str(e)
            result = {"error": str(e)}
        
        self.script_history.append(script_record)
        return result
    
    def create_macro(self, name: str, steps: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Create a macro (sequence of operations).
        
        Args:
            name: Macro name
            steps: List of macro steps
            
        Returns:
            Macro definition dictionary
        """
        macro = {
            "name": name,
            "steps": steps,
            "created_at": time.time(),
            "description": f"Macro {name} for NLM operations"
        }
        
        return macro
    
    def execute_macro(self, macro: Dict[str, Any]) -> Dict[str, Any]:
        """
        Execute a macro.
        
        Args:
            macro: Macro definition
            
        Returns:
            Execution results
        """
        results = {
            "macro_name": macro["name"],
            "steps_executed": [],
            "successful": True,
            "errors": []
        }
        
        for step in macro["steps"]:
            try:
                step_result = self._execute_macro_step(step)
                results["steps_executed"].append({
                    "step": step,
                    "result": step_result,
                    "success": True
                })
            except Exception as e:
                results["successful"] = False
                results["errors"].append({
                    "step": step,
                    "error": str(e)
                })
        
        return results
    
    def _execute_macro_step(self, step: Dict[str, Any]) -> Any:
        """Execute a single macro step."""
        action = step.get("action")
        parameters = step.get("parameters", {})
        
        if action == "create_brain":
            config = parameters.get("config", {})
            return pynlm.createBrain(config)
        elif action == "run_simulation":
            brain = parameters.get("brain")
            steps = parameters.get("steps", 100)
            return pynlm.runSimulationSteps(brain, steps)
        elif action == "save_brain":
            brain = parameters.get("brain")
            filepath = parameters.get("filepath")
            return pynlm.saveBrainState(brain, filepath)
        elif action == "load_brain":
            filepath = parameters.get("filepath")
            config = parameters.get("config", {})
            brain = pynlm.createBrain(config)
            return pynlm.loadBrainState(brain, filepath)
        else:
            raise ValueError(f"Unknown macro action: {action}")
if __name__ == "__main__":
    # Example usage
    print("Initializing NLM Extension API...")
    
    # Create extension API
    extension_api = ExtensionAPI()
    
    # Create scripted NLM
    scripted_nlm = ScriptedNLM(extension_api)
    
    # Execute a simple script
    script = """
import pynlm

# Create a simple brain
brain = pynlm.createSimpleBrain()
config = pynlm.createExperimentConfig()

# Run simulation
pynlm.runSimulationSteps(brain, 100)

# Create result
result = {
    "brain_created": brain is not None,
    "simulation_steps": 100,
    "success": True
}

# Save result to namespace
locals()["result"] = result
"""
    
    print("Executing script...")
    result = scripted_nlm.execute_script(script)
    print(f"Script execution result: {result}")
    
    # Create a macro
    macro = scripted_nlm.create_macro(
        name="basic_brain_workflow",
        steps=[
            {"action": "create_brain", "parameters": {"config": {}}},
            {"action": "run_simulation", "parameters": {"steps": 50}},
            {"action": "save_brain", "parameters": {"filepath": "test_brain.nbm"}}
        ]
    )
    
    print(f"Created macro: {macro['name']}")
    
    # Execute macro
    macro_result = scripted_nlm.execute_macro(macro)
    print(f"Macro execution result: {macro_result}")
    
    print("\nExtension API and scripting interface initialized successfully!")
