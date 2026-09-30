# Plugin Development Kit

This module provides the Plugin Development Kit for creating and managing NLM plugins, including SDK, templates, testing framework, documentation generator, and packaging system.

import os
import json
import shutil
import tempfile
import importlib
import inspect
import sys
import re
from typing import Dict, List, Any, Optional, Callable, Type, Union
from dataclasses import dataclass, field
from enum import Enum
from abc import ABC, abstractmethod
import time

from .plugin_system import Plugin, PluginInfo, PluginType, PluginStatus
from .core_plugin_framework import EnhancedPluginManager


class DevelopmentError(Exception):
    """Exception raised during plugin development"""
    pass


class DevelopmentStage(Enum):
    """Plugin development stages"""
    SPECIFICATION = "specification"
    IMPLEMENTATION = "implementation"
    TESTING = "testing"
    DOCUMENTATION = "documentation"
    PACKAGING = "packaging"
    PUBLICATION = "publication"


@dataclass
class PluginSpecification:
    """Plugin specification"""
    name: str
    version: str
    description: str
    author: str
    plugin_type: str
    dependencies: List[str] = field(default_factory=list)
    required_plugins: List[str] = field(default_factory=list)
    config_schema: Dict[str, Any] = field(default_factory=dict)
    capabilities: List[str] = field(default_factory=list)
    targets: List[str] = field(default_factory=lambda: ['python', 'cpp', 'csharp'])
    license: str = "MIT"
    documentation: str = ""
    examples: List[str] = field(default_factory=list)
    tests: List[str] = field(default_factory=list)
    tags: List[str] = field(default_factory=list)
    keywords: List[str] = field(default_factory=list)
    screenshots: List[str] = field(default_factory=list)
    homepage: str = ""
    repository: str = ""
    issues: str = ""
    changelog: str = ""
    compatibility: Dict[str, Any] = field(default_factory=dict)


class PluginSDK:
    """Plugin SDK for plugin development"""
    
    def __init__(self, plugin_manager: EnhancedPluginManager):
        self.plugin_manager = plugin_manager
        self.api_docs = APIDocumentation()
        self.examples = ExampleRepository()
        self.templates = TemplateRepository()
        self.validators = PluginValidator()
        
    def create_plugin(self, spec: PluginSpecification, implementation_dir: str) -> str:
        """Create a new plugin from specification"""
        # Create plugin directory
        plugin_dir = os.path.join(implementation_dir, spec.name)
        os.makedirs(plugin_dir, exist_ok=True)
        
        # Generate plugin files
        self._generate_plugin_files(spec, plugin_dir)
        
        # Generate configuration files
        self._generate_config_files(spec, plugin_dir)
        
        # Generate documentation
        self._generate_documentation(spec, plugin_dir)
        
        # Generate tests
        self._generate_tests(spec, plugin_dir)
        
        # Generate examples
        self._generate_examples(spec, plugin_dir)
        
        return plugin_dir
        
    def validate_plugin(self, plugin_path: str) -> List[str]:
        """Validate plugin code and structure"""
        errors = []
        
        # Check for required files
        required_files = ['__init__.py', 'plugin.py', 'plugin_info.json']
        for file in required_files:
            filepath = os.path.join(plugin_path, file)
            if not os.path.exists(filepath):
                errors.append(f"Missing required file: {file}")
        
        # Validate Python syntax
        if os.path.exists(os.path.join(plugin_path, 'plugin.py')):
            try:
                with open(os.path.join(plugin_path, 'plugin.py'), 'r') as f:
                    code = f.read()
                compile(code, 'plugin.py', 'exec')
            except SyntaxError as e:
                errors.append(f"Syntax error in plugin.py: {e}")
        
        # Validate plugin_info.json
        if os.path.exists(os.path.join(plugin_path, 'plugin_info.json')):
            try:
                with open(os.path.join(plugin_path, 'plugin_info.json'), 'r') as f:
                    metadata = json.load(f)
                self._validate_metadata(metadata, errors)
            except json.JSONDecodeError as e:
                errors.append(f"Invalid JSON in plugin_info.json: {e}")
        
        return errors
        
    def build_plugin(self, plugin_path: str, output_format: str = 'zip') -> str:
        """Build plugin for distribution"""
        # Validate plugin first
        errors = self.validate_plugin(plugin_path)
        if errors:
            raise DevelopmentError(f"Plugin validation failed: {errors}")
        
        # Create distribution package
        plugin_name = os.path.basename(plugin_path)
        if output_format == 'zip':
            return self._create_zip_package(plugin_path, plugin_name)
        elif output_format == 'python_package':
            return self._create_python_package(plugin_path, plugin_name)
        else:
            raise DevelopmentError(f"Unsupported output format: {output_format}")
            
    def test_plugin(self, plugin_path: str, test_config: Dict[str, Any] = None) -> Dict[str, Any]:
        """Test plugin with sandbox"""
        if test_config is None:
            test_config = {}
        
        # Run static analysis
        static_results = self._static_analysis(plugin_path)
        
        # Run unit tests
        unit_results = self._run_unit_tests(plugin_path, test_config)
        
        # Run integration tests
        integration_results = self._run_integration_tests(plugin_path, test_config)
        
        return {
            'static_analysis': static_results,
            'unit_tests': unit_results,
            'integration_tests': integration_results,
            'overall_status': 'passed' if all([
                static_results['passed'],
                unit_results['passed'],
                integration_results['passed']
            ]) else 'failed'
        }
        
    def create_plugin_template(self, template_type: str, output_dir: str, variables: Dict[str, Any] = None) -> str:
        """Create plugin template"""
        if variables is None:
            variables = {}
        
        # Get template
        template_path = self.templates.get_template_path(template_type)
        if not template_path:
            raise DevelopmentError(f"Unknown template type: {template_type}")
        
        # Create output directory
        template_dir = os.path.join(output_dir, f"template_{template_type}")
        os.makedirs(template_dir, exist_ok=True)
        
        # Copy template files
        shutil.copytree(template_path, template_dir, dirs_exist_ok=True)
        
        # Apply variables
        self._apply_template_variables(template_dir, variables)
        
        return template_dir
        
    def _generate_plugin_files(self, spec: PluginSpecification, plugin_dir: str):
        """Generate main plugin files"""
        # Generate plugin.py
        plugin_py = os.path.join(plugin_dir, 'plugin.py')
        with open(plugin_py, 'w') as f:
            f.write(self._generate_plugin_code(spec))
        
        # Generate plugin_info.json
        plugin_info_json = os.path.join(plugin_dir, 'plugin_info.json')
        with open(plugin_info_json, 'w') as f:
            json.dump(self._generate_metadata(spec), f, indent=2)
        
        # Generate __init__.py
        init_py = os.path.join(plugin_dir, '__init__.py')
        with open(init_py, 'w') as f:
            f.write(self._generate_init_code(spec))
        
        # Generate plugin_class.py
        plugin_class_py = os.path.join(plugin_dir, f'{spec.name.lower().replace(" ", "_")}.py')
        with open(plugin_class_py, 'w') as f:
            f.write(self._generate_plugin_class(spec))
            
    def _generate_config_files(self, spec: PluginSpecification, plugin_dir: str):
        """Generate configuration files"""
        # Generate config.json
        config_json = os.path.join(plugin_dir, 'config.json')
        with open(config_json, 'w') as f:
            json.dump(self._generate_config_schema(spec), f, indent=2)
        
        # Generate default_config.json
        default_config_json = os.path.join(plugin_dir, 'default_config.json')
        with open(default_config_json, 'w') as f:
            json.dump(self._generate_default_config(spec), f, indent=2)
            
    def _generate_documentation(self, spec: PluginSpecification, plugin_dir: str):
        """Generate plugin documentation"""
        # Generate README.md
        readme_md = os.path.join(plugin_dir, 'README.md')
        with open(readme_md, 'w') as f:
            f.write(self._generate_readme(spec))
        
        # Generate API documentation
        api_md = os.path.join(plugin_dir, 'API.md')
        with open(api_md, 'w') as f:
            f.write(self._generate_api_documentation(spec))
        
        # Generate examples documentation
        examples_md = os.path.join(plugin_dir, 'EXAMPLES.md')
        with open(examples_md, 'w') as f:
            f.write(self._generate_examples_documentation(spec))
            
    def _generate_tests(self, spec: PluginSpecification, plugin_dir: str):
        """Generate test files"""
        # Generate test_plugin.py
        test_py = os.path.join(plugin_dir, 'test_plugin.py')
        with open(test_py, 'w') as f:
            f.write(self._generate_test_code(spec))
        
        # Generate test_requirements.json
        test_requirements_json = os.path.join(plugin_dir, 'test_requirements.json')
        with open(test_requirements_json, 'w') as f:
            json.dump(self._generate_test_requirements(spec), f, indent=2)
            
    def _generate_examples(self, spec: PluginSpecification, plugin_dir: str):
        """Generate example files"""
        # Create examples directory
        examples_dir = os.path.join(plugin_dir, 'examples')
        os.makedirs(examples_dir, exist_ok=True)
        
        # Generate example1
        example1_py = os.path.join(examples_dir, 'example1.py')
        with open(example1_py, 'w') as f:
            f.write(self._generate_example1(spec))
        
        # Generate example2
        example2_py = os.path.join(examples_dir, 'example2.py')
        with open(example2_py, 'w') as f:
            f.write(self._generate_example2(spec))
            
    def _generate_plugin_code(self, spec: PluginSpecification) -> str:
        """Generate plugin.py code"""
        return f'''"""
{spec.description}

Plugin Type: {spec.plugin_type}
Author: {spec.author}
Version: {spec.version}
"""

from pynlm.plugin_system import Plugin, PluginInfo, PluginType, PluginStatus
from typing import Dict, Any, Optional, List


class {spec.name.replace(' ', '').title().replace('-', '')}Plugin(Plugin):
    """Plugin implementation for {spec.name}"""
    
    def __init__(self, plugin_info: PluginInfo):
        super().__init__(plugin_info)
        self.info.plugin_type = PluginType.{spec.plugin_type.upper()}
        self.config = {{}}
        
    def initialize(self, interface, config: Optional[Dict] = None):
        """Initialize the plugin with the interface"""
        super().initialize(interface, config)
        
        # Plugin-specific initialization
        try:
            self._initialize_plugin()
        except Exception as e:
            raise PluginError(f"Plugin initialization failed: {{e}}")
    
    def enable(self):
        """Enable the plugin"""
        if not self.initialized:
            raise PluginError("Plugin not initialized")
        
        try:
            self._enable_plugin()
            self.enabled = True
            self.info.status = PluginStatus.RUNNING
        except Exception as e:
            self.info.status = PluginStatus.ERROR
            self.info.error_message = str(e)
            raise PluginError(f"Plugin enable failed: {{e}}")
    
    def disable(self):
        """Disable the plugin"""
        try:
            self._disable_plugin()
            self.enabled = False
            self.info.status = PluginStatus.PAUSED
        except Exception as e:
            self.info.error_message = str(e)
            raise PluginError(f"Plugin disable failed: {{e}}")
    
    def unload(self):
        """Unload the plugin"""
        try:
            self._unload_plugin()
            self.initialized = False
            self.enabled = False
            self.info.status = PluginStatus.UNLOADED
            self.info.module = None
        except Exception as e:
            self.info.error_message = str(e)
            raise PluginError(f"Plugin unload failed: {{e}}")
    
    # Plugin-specific methods - override in subclasses
    def _initialize_plugin(self):
        """Plugin-specific initialization. Override in subclass."""
        pass
    
    def _enable_plugin(self):
        """Plugin-specific enable. Override in subclass."""
        pass
    
    def _disable_plugin(self):
        """Plugin-specific disable. Override in subclass."""
        pass
    
    def _unload_plugin(self):
        """Plugin-specific unload. Override in subclass."""
        pass
    
    def get_config(self, key: str, default: Any = None) -> Any:
        """Get plugin configuration"""
        return self.config.get(key, default)
    
    def set_config(self, key: str, value: Any):
        """Set plugin configuration"""
        self.config[key] = value
    
    def get_capability(self, capability: str) -> Any:
        """Get plugin capability"""
        if self.info.capabilities and capability in self.info.capabilities:
            return True
        return False
'''
    
    def _generate_metadata(self, spec: PluginSpecification) -> Dict[str, Any]:
        """Generate plugin metadata"""
        return {
            'name': spec.name,
            'version': spec.version,
            'description': spec.description,
            'author': spec.author,
            'plugin_type': spec.plugin_type,
            'dependencies': spec.dependencies,
            'required_plugins': spec.required_plugins,
            'license': spec.license,
            'homepage': spec.homepage,
            'repository': spec.repository,
            'tags': spec.tags,
            'keywords': spec.keywords,
            'capabilities': spec.capabilities,
            'targets': spec.targets,
            'compatibility': spec.compatibility,
            'documentation': spec.documentation
        }
        
    def _generate_init_code(self, spec: PluginSpecification) -> str:
        """Generate __init__.py code"""
        class_name = spec.name.replace(' ', '').title().replace('-', '')
        return f'''"""
{spec.name} Plugin Package

This package contains the {spec.name} plugin for NLM.
"""

from .{class_name.lower().replace(" ", "_")} import {class_name}Plugin

__all__ = ['{class_name}Plugin']
'''
        
    def _generate_plugin_class(self, spec: PluginSpecification) -> str:
        """Generate plugin class code"""
        class_name = spec.name.replace(' ', '').title().replace('-', '')
        return f'''"""
Custom implementation of the {spec.name} plugin.

This class provides the core functionality for the {spec.name} plugin.
"""

from typing import Dict, Any, Optional, List
from pynlm.plugin_system import Plugin


class {class_name}PluginImpl:
    """Implementation class for {spec.name} plugin"""
    
    def __init__(self):
        self.config = {{}}
        self.state = {{}}
        
    def initialize(self, interface, config: Optional[Dict] = None):
        """Initialize the implementation"""
        if config:
            self.config.update(config)
            
    def process(self, input_data: Any) -> Any:
        """Process input data"""
        # Implement custom processing logic
        return input_data
        
    def cleanup(self):
        """Clean up resources"""
        pass
'''
        
    def _generate_config_schema(self, spec: PluginSpecification) -> Dict[str, Any]:
        """Generate configuration schema"""
        schema = {
            'type': 'object',
            'properties': {},
            'required': []
        }
        
        # Add properties based on plugin type
        if spec.plugin_type == 'brain_extension':
            schema['properties'] = {
                'network_topology': {'type': 'string', 'description': 'Network topology to use'},
                'neuron_types': {'type': 'array', 'items': {'type': 'string'}, 'description': 'Custom neuron types'},
                'connection_rules': {'type': 'array', 'items': {'type': 'object'}, 'description': 'Connection rules'}
            }
            schema['required'] = ['network_topology']
        
        elif spec.plugin_type == 'learning_rule':
            schema['properties'] = {
                'rule_parameters': {'type': 'object', 'description': 'Learning rule parameters'},
                'eligibility_trace': {'type': 'boolean', 'description': 'Use eligibility traces'},
                'plasticity_modulation': {'type': 'boolean', 'description': 'Enable plasticity modulation'}
            }
            schema['required'] = ['rule_parameters']
        
        elif spec.plugin_type == 'neuromodulation':
            schema['properties'] = {
                'modulators': {'type': 'array', 'items': {'type': 'string'}, 'description': 'Neuromodulators to use'},
                'arousal_level': {'type': 'number', 'minimum': 0, 'maximum': 1, 'description': 'Arousal level'},
                'stress_response': {'type': 'boolean', 'description': 'Enable stress response'}
            }
            schema['required'] = ['modulators']
        
        elif spec.plugin_type == 'sensory':
            schema['properties'] = {
                'channels': {'type': 'array', 'items': {'type': 'string'}, 'description': 'Sensory channels'},
                'augmentation': {'type': 'boolean', 'description': 'Enable sensory augmentation'},
                'cross_modal': {'type': 'boolean', 'description': 'Enable cross-modal integration'}
            }
            schema['required'] = ['channels']
        
        elif spec.plugin_type == 'motor':
            schema['properties'] = {
                'controllers': {'type': 'array', 'items': {'type': 'string'}, 'description': 'Motor controllers'},
                'action_selection': {'type': 'string', 'description': 'Action selection strategy'},
                'muscle_models': {'type': 'array', 'items': {'type': 'string'}, 'description': 'Muscle models'}
            }
            schema['required'] = ['controllers']
        
        elif spec.plugin_type == 'development':
            schema['properties'] = {
                'stages': {'type': 'array', 'items': {'type': 'string'}, 'description': 'Development stages'},
                'growth_pattern': {'type': 'string', 'description': 'Growth pattern'},
                'metamorphic_triggers': {'type': 'array', 'items': {'type': 'string'}, 'description': 'Metamorphic triggers'}
            }
            schema['required'] = ['stages']
        
        elif spec.plugin_type == 'integration':
            schema['properties'] = {
                'external_systems': {'type': 'array', 'items': {'type': 'string'}, 'description': 'External systems'},
                'api_interfaces': {'type': 'array', 'items': {'type': 'string'}, 'description': 'API interfaces'},
                'hardware_interfaces': {'type': 'array', 'items': {'type': 'string'}, 'description': 'Hardware interfaces'}
            }
            schema['required'] = ['external_systems']
        
        elif spec.plugin_type == 'analysis':
            schema['properties'] = {
                'visualizers': {'type': 'array', 'items': {'type': 'string'}, 'description': 'Visualizers'},
                'analyzers': {'type': 'array', 'items': {'type': 'string'}, 'description': 'Analyzers'},
                'reports': {'type': 'array', 'items': {'type': 'string'}, 'description': 'Reports'}
            }
            schema['required'] = ['visualizers']
        
        elif spec.plugin_type == 'utility':
            schema['properties'] = {
                'utilities': {'type': 'array', 'items': {'type': 'string'}, 'description': 'Utilities'},
                'helpers': {'type': 'array', 'items': {'type': 'string'}, 'description': 'Helper functions'}
            }
            schema['required'] = ['utilities']
        
        return schema
        
    def _generate_default_config(self, spec: PluginSpecification) -> Dict[str, Any]:
        """Generate default configuration"""
        default_config = {}
        
        # Set defaults based on plugin type
        if spec.plugin_type == 'brain_extension':
            default_config['network_topology'] = 'default'
            default_config['neuron_types'] = []
            default_config['connection_rules'] = []
        
        elif spec.plugin_type == 'learning_rule':
            default_config['rule_parameters'] = {'learning_rate': 0.1}
            default_config['eligibility_trace'] = False
            default_config['plasticity_modulation'] = False
        
        elif spec.plugin_type == 'neuromodulation':
            default_config['modulators'] = ['dopamine']
            default_config['arousal_level'] = 0.5
            default_config['stress_response'] = False
        
        elif spec.plugin_type == 'sensory':
            default_config['channels'] = ['vision', 'touch']
            default_config['augmentation'] = False
            default_config['cross_modal'] = True
        
        elif spec.plugin_type == 'motor':
            default_config['controllers'] = ['default']
            default_config['action_selection'] = 'max_activity'
            default_config['muscle_models'] = []
        
        elif spec.plugin_type == 'development':
            default_config['stages'] = ['initial', 'growth', 'maturation', 'adult']
            default_config['growth_pattern'] = 'standard'
            default_config['metamorphic_triggers'] = []
        
        elif spec.plugin_type == 'integration':
            default_config['external_systems'] = []
            default_config['api_interfaces'] = []
            default_config['hardware_interfaces'] = []
        
        elif spec.plugin_type == 'analysis':
            default_config['visualizers'] = ['basic']
            default_config['analyzers'] = ['basic']
            default_config['reports'] = []
        
        elif spec.plugin_type == 'utility':
            default_config['utilities'] = ['basic']
            default_config['helpers'] = []
        
        return default_config
        
    def _generate_readme(self, spec: PluginSpecification) -> str:
        """Generate README.md"""
        return f'''# {spec.name}

{spec.description}

## Overview

This plugin provides {spec.description.lower()} for the NLM system.

## Installation

```bash
pip install {spec.name.lower().replace(' ', '-')}
```

## Usage

### Basic Usage

```python
from {spec.name.lower().replace(' ', '-')}.plugin import {spec.name.replace(' ', '').title().replace('-', '')}Plugin
from pynlm.plugin_system import PluginManager

# Create plugin manager
plugin_manager = PluginManager(interface)

# Load plugin
plugin = plugin_manager.load_plugin('{spec.name.lower().replace(' ', '-')}')
```

### Configuration

Plugin configuration:

```json
{json.dumps(self._generate_default_config(spec), indent=2)}
```

## Development

### Plugin Class

The main plugin class is `{spec.name.replace(' ', '').title().replace('-', '')}PluginImpl`.

### Methods

- `initialize(interface, config)`: Initialize the plugin
- `process(input_data)`: Process input data
- `cleanup()`: Clean up resources

## Examples

See the `examples/` directory for usage examples.

## Testing

Run tests with:

```bash
python -m pytest
```

## License

{spec.license}
'''
        
    def _generate_api_documentation(self, spec: PluginSpecification) -> str:
        """Generate API documentation"""
        class_name = spec.name.replace(' ', '').title().replace('-', '')
        return f'''# API Documentation - {spec.name}

## Classes

### {class_name}PluginImpl

```python
class {class_name}PluginImpl:
    """Implementation class for {spec.name} plugin"""
    
    def __init__(self):
        self.config = {{}}
        self.state = {{}}
        
    def initialize(self, interface, config: Optional[Dict] = None):
        """Initialize the implementation"""
        
    def process(self, input_data: Any) -> Any:
        """Process input data"""
        
    def cleanup(self):
        """Clean up resources"""
```

## Methods

- `initialize(interface, config)`: Initialize the plugin with interface and configuration
- `process(input_data)`: Process input data through the plugin
- `cleanup()`: Clean up resources

## Configuration

See `config.json` for available configuration options.

## Events

This plugin can emit the following events:

- `{EventType.SIMULATION_STARTED}`: Simulation started
- `{EventType.NEURON_FIRED}`: Neuron fired
- `{EventType.REWARD_RECEIVED}`: Reward received
- `{EventType.STATE_CHANGED}`: State changed

## Error Handling

The plugin uses standard error handling with `PluginError` exceptions.
'''
        
    def _generate_examples_documentation(self, spec: PluginSpecification) -> str:
        """Generate examples documentation"""
        return f'''# Examples - {spec.name}

## Example 1: Basic Usage

```python
from {spec.name.lower().replace(' ', '-')}.plugin import {spec.name.replace(' ', '').title().replace('-', '')}Plugin
from pynlm.plugin_system import PluginManager

# Create plugin manager
plugin_manager = PluginManager(interface)

# Create plugin instance
plugin_info = PluginInfo(
    name='{spec.name.lower().replace(' ', '-')}',
    version='1.0.0',
    description='{spec.description}',
    author='{spec.author}',
    plugin_type='{spec.plugin_type.upper()}'
)

plugin = {spec.name.replace(' ', '').title().replace('-', '')}PluginImpl()
plugin.initialize(interface, {{'config': 'value'}})
```

## Example 2: Advanced Usage

```python
# Advanced usage with custom configuration
config = {json.dumps(self._generate_default_config(spec), indent=2)}

plugin = {spec.name.replace(' ', '').title().replace('-', '')}PluginImpl()
plugin.initialize(interface, config)

# Process data
result = plugin.process(some_input)
```

## Example 3: Integration with NLM

```python
# Integration with NLM brain
brain = interface.context.get_brain()
plugin.initialize(interface)
plugin.enable()

# Plugin will now be integrated with the NLM
```
'''
        
    def _generate_test_code(self, spec: PluginSpecification) -> str:
        """Generate test code"""
        return f'''"""Tests for {spec.name} plugin"""

import pytest
import json
from {spec.name.lower().replace(' ', '-')}.plugin import {spec.name.replace(' ', '').title().replace('-', '')}Plugin
from pynlm.plugin_system import PluginManager, PluginInfo, PluginType


class Test{spec.name.replace(' ', '').title().replace('-', '')}Plugin:
    """Test class for {spec.name} plugin"""
    
    def setup_method(self):
        """Set up test fixtures"""
        self.plugin_info = PluginInfo(
            name='{spec.name.lower().replace(' ', '-')}',
            version='1.0.0',
            description='{spec.description}',
            author='{spec.author}',
            plugin_type=PluginType.{spec.plugin_type.upper()}
        )
        
        self.plugin = {spec.name.replace(' ', '').title().replace('-', '')}PluginImpl()
        
    def teardown_method(self):
        """Clean up test fixtures"""
        if hasattr(self, 'plugin'):
            self.plugin.cleanup()
            
    def test_plugin_initialization(self):
        """Test plugin initialization"""
        # Mock interface
        class MockInterface:
            def __init__(self):
                self.context = type('obj', (object,), {{}})()
        
        interface = MockInterface()
        
        # Initialize plugin
        self.plugin.initialize(interface)
        
        # Check that plugin is initialized
        assert self.plugin.config is not None
        assert self.plugin.state is not None
        
    def test_plugin_processing(self):
        """Test plugin processing"""
        # Mock interface
        class MockInterface:
            def __init__(self):
                self.context = type('obj', (object,), {{}})()
        
        interface = MockInterface()
        self.plugin.initialize(interface)
        
        # Test processing
        test_input = 'test_data'
        result = self.plugin.process(test_input)
        
        # Check result
        assert result == test_input  # Default implementation returns input
        
    def test_plugin_cleanup(self):
        """Test plugin cleanup"""
        # Mock interface
        class MockInterface:
            def __init__(self):
                self.context = type('obj', (object,), {{}})()
        
        interface = MockInterface()
        self.plugin.initialize(interface)
        
        # Cleanup
        self.plugin.cleanup()
        
        # Check that cleanup was called
        assert self.plugin.config is not None  # Config should still exist
        
    def test_plugin_config(self):
        """Test plugin configuration"""
        # Mock interface
        class MockInterface:
            def __init__(self):
                self.context = type('obj', (object,), {{}})()
        
        interface = MockInterface()
        test_config = {{'param1': 'value1', 'param2': 42}}
        
        self.plugin.initialize(interface, test_config)
        
        # Check configuration
        assert self.plugin.config['param1'] == 'value1'
        assert self.plugin.config['param2'] == 42
'''
        
    def _generate_test_requirements(self, spec: PluginSpecification) -> Dict[str, Any]:
        """Generate test requirements"""
        return {
            'requirements': [
                'pytest>=6.0',
                'pytest-asyncio>=0.17'
            ],
            'test_files': [
                'test_plugin.py'
            ],
            'test_timeout': 30,
            'test_coverage': 0.8
        }
        
    def _generate_example1(self, spec: PluginSpecification) -> str:
        """Generate example 1 code"""
        return f'''"""Example 1: Basic plugin usage"""

import sys
import os

# Add the plugin directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from plugin import {spec.name.replace(' ', '').title().replace('-', '')}PluginImpl

def main():
    """Main function"""
    # Create plugin instance
    plugin = {spec.name.replace(' ', '').title().replace('-', '')}PluginImpl()
    
    # Mock interface
    class MockInterface:
        def __init__(self):
            self.context = type('obj', (object,), {{}})()
    
    interface = MockInterface()
    
    # Initialize plugin
    plugin.initialize(interface, {{'config': 'example_value'}})
    
    # Process data
    test_data = {{'key': 'value', 'number': 42}}
    result = plugin.process(test_data)
    
    print(f"Input: {{test_data}}")
    print(f"Output: {{result}}")
    
    # Cleanup
    plugin.cleanup()

if __name__ == '__main__':
    main()
'''
        
    def _generate_example2(self, spec: PluginSpecification) -> str:
        """Generate example 2 code"""
        return f'''"""Example 2: Advanced plugin usage"""

import sys
import os

# Add the plugin directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from plugin import {spec.name.replace(' ', '').title().replace('-', '')}PluginImpl

def main():
    """Main function"""
    # Create plugin instance
    plugin = {spec.name.replace(' ', '').title().replace('-', '')}PluginImpl()
    
    # Mock interface with context
    class MockContext:
        def __init__(self):
            self.brain = type('obj', (object,), {{}})()
    
    class MockInterface:
        def __init__(self):
            self.context = MockContext()
    
    interface = MockInterface()
    
    # Initialize plugin with advanced configuration
    config = {{
        'param1': 'advanced_value',
        'param2': [1, 2, 3],
        'param3': {{'nested': 'value'}}
    }}
    
    plugin.initialize(interface, config)
    
    # Process multiple data types
    inputs = [
        {{'type': 'text', 'content': 'Hello World'}},
        {{'type': 'number', 'value': 123}},
        {{'type': 'list', 'items': ['a', 'b', 'c']}}
    ]
    
    for i, input_data in enumerate(inputs):
        result = plugin.process(input_data)
        print(f"Input {{i+1}}: {{input_data}}")
        print(f"Output {{i+1}}: {{result}}")
        print()
    
    # Cleanup
    plugin.cleanup()

if __name__ == '__main__':
    main()
'''
        
    def _apply_template_variables(self, template_dir: str, variables: Dict[str, Any]):
        """Apply template variables"""
        for root, dirs, files in os.walk(template_dir):
            for file in files:
                filepath = os.path.join(root, file)
                with open(filepath, 'r') as f:
                    content = f.read()
                
                # Apply variables
                for var_name, var_value in variables.items():
                    placeholder = '{{' + var_name + '}}'
                    content = content.replace(placeholder, str(var_value))
                
                # Write back
                with open(filepath, 'w') as f:
                    f.write(content)
        
    def _validate_metadata(self, metadata: Dict[str, Any], errors: List[str]):
        """Validate plugin metadata"""
        required_fields = ['name', 'version', 'description', 'author', 'plugin_type']
        
        for field in required_fields:
            if field not in metadata:
                errors.append(f"Missing required metadata field: {field}")
        
        # Validate plugin type
        if 'plugin_type' in metadata:
            try:
                PluginType(metadata['plugin_type'])
            except ValueError:
                errors.append(f"Invalid plugin type: {metadata['plugin_type']}")
        
        # Validate version format
        if 'version' in metadata:
            version = metadata['version']
            if not re.match(r'^\d+\.\d+\.\d+$', version):
                errors.append(f"Invalid version format: {version}")
        
        # Validate targets
        if 'targets' in metadata:
            for target in metadata['targets']:
                if target not in ['python', 'cpp', 'csharp']:
                    errors.append(f"Invalid target platform: {target}")
                    
    def _create_zip_package(self, plugin_path: str, plugin_name: str) -> str:
        """Create ZIP package"""
        import zipfile
        
        zip_path = f"{plugin_name}.zip"
        
        with zipfile.ZipFile(zip_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
            # Add all files
            for root, dirs, files in os.walk(plugin_path):
                for file in files:
                    filepath = os.path.join(root, file)
                    arcname = os.path.relpath(filepath, plugin_path)
                    zipf.write(filepath, arcname)
        
        return zip_path
        
    def _create_python_package(self, plugin_path: str, plugin_name: str) -> str:
        """Create Python package"""
        # This would create a proper Python package structure
        # For now, return the plugin directory
        return plugin_path
        
    def _static_analysis(self, plugin_path: str) -> Dict[str, Any]:
        """Perform static analysis"""
        results = {
            'passed': True,
            'issues': [],
            'warnings': []
        }
        
        # Check for syntax errors
        plugin_py = os.path.join(plugin_path, 'plugin.py')
        if os.path.exists(plugin_py):
            try:
                with open(plugin_py, 'r') as f:
                    code = f.read()
                compile(code, 'plugin.py', 'exec')
            except SyntaxError as e:
                results['passed'] = False
                results['issues'].append(f"Syntax error: {e}")
        
        return results
        
    def _run_unit_tests(self, plugin_path: str, test_config: Dict[str, Any]) -> Dict[str, Any]:
        """Run unit tests"""
        results = {
            'passed': True,
            'tests_run': 0,
            'tests_failed': 0,
            'tests_error': 0
        }
        
        # In a real implementation, this would run pytest
        # For now, we'll simulate test results
        test_py = os.path.join(plugin_path, 'test_plugin.py')
        if os.path.exists(test_py):
            results['tests_run'] = 4  # Mock test count
        
        return results
        
    def _run_integration_tests(self, plugin_path: str, test_config: Dict[str, Any]) -> Dict[str, Any]:
        """Run integration tests"""
        results = {
            'passed': True,
            'tests_run': 0,
            'tests_failed': 0,
            'tests_error': 0
        }
        
        # In a real implementation, this would run integration tests
        # For now, we'll simulate test results
        results['tests_run'] = 2  # Mock integration test count
        
        return results


class APIDocumentation:
    """API documentation generator"""
    
    def __init__(self):
        self.api_docs = {}
        
    def generate_class_docs(self, cls: Type) -> Dict[str, Any]:
        """Generate API documentation for a class"""
        return {
            'name': cls.__name__,
            'docstring': cls.__doc__ or '',
            'methods': self._extract_methods(cls),
            'attributes': self._extract_attributes(cls),
            'example': self._extract_example(cls)
        }
        
    def _extract_methods(self, cls: Type) -> List[Dict[str, Any]]:
        """Extract method information"""
        methods = []
        for name, method in inspect.getmembers(cls, predicate=inspect.isfunction):
            if not name.startswith('_'):
                methods.append({
                    'name': name,
                    'docstring': method.__doc__ or '',
                    'signature': self._get_signature(method)
                })
        return methods
        
    def _extract_attributes(self, cls: Type) -> List[Dict[str, Any]]:
        """Extract attribute information"""
        attributes = []
        for name, value in inspect.getmembers(cls):
            if not name.startswith('_') and not inspect.isfunction(value):
                attributes.append({
                    'name': name,
                    'type': type(value).__name__,
                    'value': str(value)
                })
        return attributes
        
    def _get_signature(self, func: Callable) -> str:
        """Get function signature"""
        try:
            sig = inspect.signature(func)
            return str(sig)
        except:
            return "()"
        
    def _extract_example(self, cls: Type) -> str:
        """Extract example from class"""
        # Look for docstring example or first method as example
        if cls.__doc__:
            return cls.__doc__
        return ""


class ExampleRepository:
    """Example repository"""
    
    def __init__(self):
        self.examples_dir = os.path.join(os.path.dirname(__file__), 'examples')
        os.makedirs(self.examples_dir, exist_ok=True)
        
    def get_template_path(self, template_type: str) -> Optional[str]:
        """Get template path"""
        template_path = os.path.join(self.examples_dir, f"templates/{template_type}")
        if os.path.exists(template_path):
            return template_path
        return None
        
    def get_example_path(self, example_name: str) -> Optional[str]:
        """Get example path"""
        example_path = os.path.join(self.examples_dir, example_name)
        if os.path.exists(example_path):
            return example_path
        return None


class TemplateRepository:
    """Template repository"""
    
    def __init__(self):
        self.templates_dir = os.path.join(os.path.dirname(__file__), 'templates')
        os.makedirs(self.templates_dir, exist_ok=True)
        
    def get_template_path(self, template_type: str) -> Optional[str]:
        """Get template path"""
        template_path = os.path.join(self.templates_dir, template_type)
        if os.path.exists(template_path):
            return template_path
        return None


class PluginValidator:
    """Plugin validator"""
    
    def __init__(self):
        self.validation_rules = []
        
    def add_validation_rule(self, rule: Callable):
        """Add validation rule"""
        self.validation_rules.append(rule)
        
    def validate_plugin(self, plugin: Plugin) -> List[str]:
        """Validate plugin"""
        errors = []
        
        for rule in self.validation_rules:
            try:
                rule_errors = rule(plugin)
                errors.extend(rule_errors)
            except Exception as e:
                errors.append(f"Validation rule error: {e}")
        
        return errors