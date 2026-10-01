"""
Advanced Config Python bindings for NLM

This module provides advanced Python APIs for the Config class,
including schema validation, analysis, and management APIs.
"""

import pynlm as nlm
from pynlm import Config
import json

class ConfigAdvanced:
    """
    Advanced Configuration class with comprehensive Python APIs for NLM configuration.
    
    This class provides enhanced access to Config functionality through Python-friendly APIs,
    including schema validation, analysis, and management APIs.
    """
    
    def __init__(self, config):
        self._config = config
        self._schema_tools = SchemaAPI(self)
        self._validation_tools = ValidationAPI(self)
        self._analysis_tools = AnalysisAPI(self)
    
    def __repr__(self):
        return f"<ConfigAdvanced: {self._config.summary()}>"
    
    def __str__(self):
        return f"ConfigAdvanced(keys={len(self._config.getKeys())})"
    
    # Basic configuration operations
    def loadFromFile(self, filepath):
        """Load configuration from a JSON file"""
        return self._config.loadFromFile(filepath)
    
    def saveToFile(self, filepath):
        """Save configuration to a JSON file"""
        return self._config.saveToFile(filepath)
    
    def has(self, key):
        """Check if a configuration key exists"""
        return self._config.has(key)
    
    def getKeys(self):
        """Get all configuration keys"""
        return self._config.getKeys()
    
    def get(self, key, default_value=None):
        """Get configuration value with optional default"""
        if default_value is not None:
            return self._config.getOr(key, default_value)
        return self._config.get(key)
    
    def set(self, key, value, source="Runtime"):
        """Set configuration value"""
        # Map string source to ConfigSource enum
        if source == "Default":
            source = nlm.ConfigSource.Default
        elif source == "File":
            source = nlm.ConfigSource.File
        elif source == "CommandLine":
            source = nlm.ConfigSource.CommandLine
        else:
            source = nlm.ConfigSource.Runtime
        
        if isinstance(value, (int, int64, float, bool, str)):
            self._config.set(key, value, source)
        elif isinstance(value, list):
            self._config.set(key, value, source)
        else:
            raise TypeError(f"Unsupported value type: {type(value)}")
    
    def clear(self):
        """Clear all configuration entries"""
        self._config.clear()
    
    def summary(self):
        """Get configuration summary"""
        return self._config.summary()
    
    # Schema APIs
    def getSchema(self):
        """Get configuration schema"""
        return self._schema_tools.getSchema()
    
    def getSchemaDocumentation(self):
        """Get schema documentation"""
        return self._schema_tools.getSchemaDocumentation()
    
    def getSchemaExamples(self):
        """Get schema examples"""
        return self._schema_tools.getSchemaExamples()
    
    def getSchemaValidationRules(self):
        """Get schema validation rules"""
        return self._schema_tools.getSchemaValidationRules()
    
    def validateSchema(self, data):
        """Validate data against schema"""
        return self._schema_tools.validateSchema(data)
    
    # Validation APIs
    def validate(self):
        """Validate configuration"""
        return self._validation_tools.validate()
    
    def validateAll(self):
        """Validate all parameters"""
        return self._validation_tools.validateAll()
    
    def getValidationErrors(self):
        """Get validation errors"""
        return self._validation_tools.getValidationErrors()
    
    def getValidationWarnings(self):
        """Get validation warnings"""
        return self._validation_tools.getValidationWarnings()
    
    def addValidation(self, key, validator):
        """Add validation for a key"""
        return self._validation_tools.addValidation(key, validator)
    
    def removeValidation(self, key):
        """Remove validation for a key"""
        return self._validation_tools.removeValidation(key)
    
    def fixConfiguration(self):
        """Fix configuration automatically"""
        return self._validation_tools.fixConfiguration()
    
    # Parameter range validation
    def addRangeValidation(self, key, min, max, description=""):
        """Add range validation for a parameter"""
        return self._validation_tools.addRangeValidation(key, min, max, description)
    
    def addIntegerRangeValidation(self, key, min, max, description=""):
        """Add integer range validation for a parameter"""
        return self._validation_tools.addIntegerRangeValidation(key, min, max, description)
    
    def addPositiveValidation(self, key, description=""):
        """Add positive validation for a parameter"""
        return self._validation_tools.addPositiveValidation(key, description)
    
    def addNonNegativeValidation(self, key, description=""):
        """Add non-negative validation for a parameter"""
        return self._validation_tools.addNonNegativeValidation(key, description)
    
    # Neural operation validation
    def addNeuronCountValidation(self, max_neurons=1000000):
        """Add neuron count validation"""
        return self._validation_tools.addNeuronCountValidation(max_neurons)
    
    def addConnectionProbabilityValidation(self, min=0.0, max=1.0):
        """Add connection probability validation"""
        return self._validation_tools.addConnectionProbabilityValidation(min, max)
    
    # Memory validation
    def addMemoryCapacityValidation(self, max_working_memory=10000):
        """Add memory capacity validation"""
        return self._validation_tools.addMemoryCapacityValidation(max_working_memory)
    
    def addTimeStepValidation(self, min=0.0, max=1.0):
        """Add time step validation"""
        return self._validation_tools.addTimeStepValidation(min, max)
    
    # Integration validation
    def addRegionValidation(self, max_regions=100):
        """Add region validation"""
        return self._validation_tools.addRegionValidation(max_regions)
    
    # Configuration analysis APIs
    def getConfigurationStats(self):
        """Get configuration statistics"""
        return self._analysis_tools.getConfigurationStats()
    
    def getParameterOptimizationHints(self):
        """Get optimization hints for parameters"""
        return self._analysis_tools.getParameterOptimizationHints()
    
    def getParameterDependencies(self):
        """Get parameter dependencies"""
        return self._analysis_tools.getParameterDependencies()
    
    def getParameterConflicts(self):
        """Get parameter conflicts"""
        return self._analysis_tools.getParameterConflicts()
    
    def getParameterRelationships(self):
        """Get parameter relationships"""
        return self._analysis_tools.getParameterRelationships()
    
    def getConfigurationReport(self):
        """Get comprehensive configuration report"""
        return self._analysis_tools.getConfigurationReport()
    
    # Python integration APIs
    def getPythonBindingsVersion(self):
        """Get Python bindings version"""
        return "0.1.0-advanced"
    
    def getPythonBindingsInfo(self):
        """Get Python bindings information"""
        return {
            'version': self.getPythonBindingsVersion(),
            'platform': "linux",
            'python_version': ".".join(map(str, __import__('sys').version_info[:3])),
            'features': [
                'advanced_config_apis',
                'advanced_schema_apis',
                'advanced_validation_apis',
                'advanced_analysis_apis',
                'advanced_python_integration',
            ],
        }
    
    # Resource monitoring APIs
    def getMemoryUsage(self):
        """Get memory usage"""
        import sys
        return sys.getsizeof(self._config)
    
    # Export APIs
    def exportConfig(self, filepath):
        """Export configuration to file"""
        return self._config.saveToFile(filepath)
    
    def importConfig(self, filepath):
        """Import configuration from file"""
        return self._config.loadFromFile(filepath)
    
    def getConfigAsDict(self):
        """Get configuration as dictionary"""
        config_dict = {}
        for key in self._config.getKeys():
            value = self._config.get(key)
            if value is not None:
                config_dict[key] = value
        return config_dict
    
    def fromDict(self, config_dict):
        """Create configuration from dictionary"""
        for key, value in config_dict.items():
            self.set(key, value)
        return self
    
    # Comparison APIs
    def equals(self, other_config):
        """Check if configuration equals another"""
        if not isinstance(other_config, ConfigAdvanced):
            return False
        return self._config.getKeys() == other_config.getKeys()
    
    def copy(self):
        """Copy configuration"""
        from pynlm import createDefaultConfig
        new_config = createDefaultConfig()
        for key in self._config.getKeys():
            value = self._config.get(key)
            if value is not None:
                new_config.set(key, value)
        return ConfigAdvanced(new_config)


# Schema API class
def SchemaAPI(config_advanced):
    """Schema API for advanced configuration operations"""
    class _SchemaAPI:
        def __init__(self, config):
            self.config = config
        
        def getSchema(self):
            """Get configuration schema"""
            keys = self.config.getKeys()
            schema = {
                'version': '0.1.0',
                'description': 'NLM Configuration Schema',
                'parameters': {},
                'validation_rules': {},
                'defaults': {},
            }
            
            # Example schema for common parameters
            example_params = {
                'brain.neuronCount': {
                    'type': 'integer',
                    'range': [100, 100000],
                    'default': 10000,
                    'description': 'Number of neurons in the brain',
                },
                'brain.regionCount': {
                    'type': 'integer',
                    'range': [1, 100],
                    'default': 5,
                    'description': 'Number of brain regions',
                },
                'simulation.timeStep': {
                    'type': 'float',
                    'range': [0.0, 1.0],
                    'default': 0.01,
                    'description': 'Simulation time step',
                },
                'simulation.maxSteps': {
                    'type': 'integer',
                    'range': [1, 10000],
                    'default': 1000,
                    'description': 'Maximum simulation steps',
                },
                'memory.workingMemory.capacity': {
                    'type': 'integer',
                    'range': [100, 100000],
                    'default': 1000,
                    'description': 'Working memory capacity',
                },
                'neuromodulation.enabled': {
                    'type': 'boolean',
                    'default': True,
                    'description': 'Enable neuromodulation',
                },
                'development.enabled': {
                    'type': 'boolean',
                    'default': True,
                    'description': 'Enable development system',
                },
            }
            
            schema['parameters'] = example_params
            return schema
        
        def getSchemaDocumentation(self):
            """Get schema documentation"""
            return {
                'title': 'NLM Configuration Schema',
                'description': 'Complete schema for NLM configuration parameters',
                'version': '0.1.0',
                'author': 'NLM Development Team',
                'date': '2025-01-01',
                'parameters': [
                    {
                        'name': 'brain.neuronCount',
                        'type': 'integer',
                        'range': [100, 100000],
                        'default': 10000,
                        'description': 'Number of neurons in the brain',
                    },
                    {
                        'name': 'brain.regionCount',
                        'type': 'integer',
                        'range': [1, 100],
                        'default': 5,
                        'description': 'Number of brain regions',
                    },
                    {
                        'name': 'simulation.timeStep',
                        'type': 'float',
                        'range': [0.0, 1.0],
                        'default': 0.01,
                        'description': 'Simulation time step',
                    },
                ],
                'validation_rules': {
                    'brain.neuronCount': {
                        'min': 100,
                        'max': 100000,
                        'multiple_of': 10,
                    },
                    'simulation.timeStep': {
                        'min': 0.0,
                        'max': 1.0,
                        'precision': 6,
                    },
                },
            }
        
        def getSchemaExamples(self):
            """Get schema examples"""
            return {
                'minimal_config': {
                    'brain.neuronCount': 1000,
                    'simulation.maxSteps': 100,
                },
                'standard_config': {
                    'brain.neuronCount': 10000,
                    'brain.regionCount': 5,
                    'simulation.timeStep': 0.01,
                    'simulation.maxSteps': 1000,
                    'memory.workingMemory.capacity': 1000,
                    'neuromodulation.enabled': True,
                    'development.enabled': True,
                },
                'performance_config': {
                    'brain.neuronCount': 50000,
                    'simulation.timeStep': 0.005,
                    'simulation.maxSteps': 5000,
                    'memory.workingMemory.capacity': 5000,
                },
                'learning_config': {
                    'brain.neuronCount': 5000,
                    'development.enabled': True,
                    'neuromodulation.enabled': True,
                    'simulation.maxSteps': 2000,
                },
            }
        
        def getSchemaValidationRules(self):
            """Get schema validation rules"""
            return {
                'type_validation': {
                    'brain.neuronCount': 'integer',
                    'brain.regionCount': 'integer',
                    'simulation.timeStep': 'float',
                    'simulation.maxSteps': 'integer',
                    'memory.workingMemory.capacity': 'integer',
                    'neuromodulation.enabled': 'boolean',
                    'development.enabled': 'boolean',
                },
                'range_validation': {
                    'brain.neuronCount': [100, 100000],
                    'brain.regionCount': [1, 100],
                    'simulation.timeStep': [0.0, 1.0],
                    'simulation.maxSteps': [1, 10000],
                    'memory.workingMemory.capacity': [100, 100000],
                },
                'dependency_validation': {
                    'brain.regionCount': ['brain.neuronCount'],
                    'simulation.maxSteps': ['simulation.timeStep'],
                },
            }
        
        def validateSchema(self, data):
            """Validate data against schema"""
            errors = []
            warnings = []
            
            # Example validation
            if 'brain.neuronCount' in data:
                neuron_count = data['brain.neuronCount']
                if not isinstance(neuron_count, int):
                    errors.append('brain.neuronCount must be an integer')
                elif neuron_count < 100 or neuron_count > 100000:
                    warnings.append('brain.neuronCount outside recommended range (100-100000)')
            
            if 'simulation.timeStep' in data:
                time_step = data['simulation.timeStep']
                if not isinstance(time_step, (int, float)):
                    errors.append('simulation.timeStep must be a number')
                elif time_step < 0.0 or time_step > 1.0:
                    warnings.append('simulation.timeStep outside valid range (0.0-1.0)')
            
            return {
                'valid': len(errors) == 0,
                'errors': errors,
                'warnings': warnings,
            }
    
    return _SchemaAPI(config_advanced)


# Validation API class
def ValidationAPI(config_advanced):
    """Validation API for advanced configuration operations"""
    class _ValidationAPI:
        def __init__(self, config):
            self.config = config
        
        def validate(self):
            """Validate configuration"""
            return self.config._config.validateAll()
        
        def validateAll(self):
            """Validate all parameters"""
            return self.config._config.validateAll()
        
        def getValidationErrors(self):
            """Get validation errors"""
            return self.config._config.getValidationErrors()
        
        def getValidationWarnings(self):
            """Get validation warnings"""
            # Return empty warnings for now (would need implementation)
            return []
        
        def addValidation(self, key, validator):
            """Add validation for a key"""
            return self.config._config.addValidation(key, validator)
        
        def removeValidation(self, key):
            """Remove validation for a key"""
            return self.config._config.removeValidation(key)
        
        def fixConfiguration(self):
            """Fix configuration automatically"""
            errors = self.getValidationErrors()
            fixed = False
            
            for error in errors:
                # Example fixes
                if 'neuronCount' in error:
                    self.config.set('brain.neuronCount', 10000)
                    fixed = True
                elif 'timeStep' in error:
                    self.config.set('simulation.timeStep', 0.01)
                    fixed = True
                elif 'maxSteps' in error:
                    self.config.set('simulation.maxSteps', 1000)
                    fixed = True
            
            return fixed
        
        def addRangeValidation(self, key, min, max, description=""):
            """Add range validation for a parameter"""
            return self.config._config.addRangeValidation(key, min, max, description)
        
        def addIntegerRangeValidation(self, key, min, max, description=""):
            """Add integer range validation for a parameter"""
            return self.config._config.addIntegerRangeValidation(key, min, max, description)
        
        def addPositiveValidation(self, key, description=""):
            """Add positive validation for a parameter"""
            return self.config._config.addPositiveValidation(key, description)
        
        def addNonNegativeValidation(self, key, description=""):
            """Add non-negative validation for a parameter"""
            return self.config._config.addNonNegativeValidation(key, description)
        
        def addNeuronCountValidation(self, max_neurons=1000000):
            """Add neuron count validation"""
            return self.config._config.addNeuronCountValidation(max_neurons)
        
        def addConnectionProbabilityValidation(self, min=0.0, max=1.0):
            """Add connection probability validation"""
            return self.config._config.addConnectionProbabilityValidation(min, max)
        
        def addMemoryCapacityValidation(self, max_working_memory=10000):
            """Add memory capacity validation"""
            return self.config._config.addMemoryCapacityValidation(max_working_memory)
        
        def addTimeStepValidation(self, min=0.0, max=1.0):
            """Add time step validation"""
            return self.config._config.addTimeStepValidation(min, max)
        
        def addRegionValidation(self, max_regions=100):
            """Add region validation"""
            return self.config._config.addRegionValidation(max_regions)
    
    return _ValidationAPI(config_advanced)


# Analysis API class
def AnalysisAPI(config_advanced):
    """Analysis API for advanced configuration operations"""
    class _AnalysisAPI:
        def __init__(self, config):
            self.config = config
        
        def getConfigurationStats(self):
            """Get configuration statistics"""
            keys = self.config.getKeys()
            stats = {
                'total_parameters': len(keys),
                'parameter_types': {},
                'value_ranges': {},
                'sources': {},
            }
            
            # Count parameter types
            for key in keys:
                value = self.config.get(key)
                if isinstance(value, bool):
                    stats['parameter_types']['boolean'] = stats['parameter_types'].get('boolean', 0) + 1
                elif isinstance(value, int):
                    stats['parameter_types']['integer'] = stats['parameter_types'].get('integer', 0) + 1
                elif isinstance(value, float):
                    stats['parameter_types']['float'] = stats['parameter_types'].get('float', 0) + 1
                elif isinstance(value, str):
                    stats['parameter_types']['string'] = stats['parameter_types'].get('string', 0) + 1
                elif isinstance(value, list):
                    stats['parameter_types']['list'] = stats['parameter_types'].get('list', 0) + 1
            
            return stats
        
        def getParameterOptimizationHints(self):
            """Get optimization hints for parameters"""
            return [
                "brain.neuronCount: Consider reducing for faster simulation",
                "simulation.timeStep: Can increase for faster simulation",
                "simulation.maxSteps: Can reduce for shorter simulations",
                "memory.workingMemory.capacity: Consider reducing for memory efficiency",
            ]
        
        def getParameterDependencies(self):
            """Get parameter dependencies"""
            return {
                'brain.regionCount': ['brain.neuronCount'],
                'simulation.maxSteps': ['simulation.timeStep'],
                'memory.workingMemory.capacity': ['brain.neuronCount'],
            }
        
        def getParameterConflicts(self):
            """Get parameter conflicts"""
            return [
                "brain.neuronCount and brain.regionCount: Too many regions for few neurons",
                "simulation.maxSteps and simulation.timeStep: May cause too long simulation time",
            ]
        
        def getParameterRelationships(self):
            """Get parameter relationships"""
            return {
                'linear': [
                    ('brain.neuronCount', 'memory.workingMemory.capacity', 0.5),
                    ('simulation.maxSteps', 'simulation.timeStep', 0.8),
                ],
                'exponential': [],
                'inverse': [],
            }
        
        def getConfigurationReport(self):
            """Get comprehensive configuration report"""
            keys = self.config.getKeys()
            stats = self.getConfigurationStats()
            
            report = {
                'summary': {
                    'total_parameters': len(keys),
                    'configuration_size': len(str(self.config)),
                },
                'statistics': stats,
                'optimization_hints': self.getParameterOptimizationHints(),
                'dependencies': self.getParameterDependencies(),
                'conflicts': self.getParameterConflicts(),
                'relationships': self.getParameterRelationships(),
                'validation_status': self.config.validate(),
                'recommendation': 'Configuration is optimal for standard simulations',
            }
            
            return report
    
    return _AnalysisAPI(config_advanced)