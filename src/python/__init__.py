"""
NLM Python Bindings for Advanced Functionality
# This module provides advanced Python APIs for all NLM systems

import sys
from pathlib import Path
import json
import datetime

# Try to import the enhanced bindings module
try:
    import pynlm as nlm
    ADVANCED_BINDINGS_AVAILABLE = True
except ImportError:
    ADVANCED_BINDINGS_AVAILABLE = False

# Re-export all advanced classes and functions
__all__ = [
    'BrainAdvanced',
    'AgentBrainAdvanced', 
    'ConfigAdvanced',
    'SimpleWorldAdvanced',
    'SensoryInputAdvanced',
    'MemorySystemAdvanced',
    'DevelopmentSystemAdvanced',
    'NeuromodulationSystemAdvanced',
    'PredictionSystemAdvanced',
    'PerformanceSystemAdvanced',
    'ExportSystemAdvanced',
    'ToolSystemAdvanced',
    'PluginSystemAdvanced',
    'ErrorSystemAdvanced',
]

# Version information
VERSION = "0.1.0-advanced"
NLM_VERSION = "0.1.0"

# Platform information
PLATFORM = "linux"
PYTHON_VERSION = ".".join(map(str, sys.version_info[:3]))

print(f"NLM Advanced Python Bindings v{VERSION}")
print(f"Platform: {PLATFORM}, Python: {PYTHON_VERSION}")

if ADVANCED_BINDINGS_AVAILABLE:
    print("✓ Advanced bindings loaded successfully")
else:
    print("✗ Advanced bindings not available - falling back to basic bindings")

# Import advanced modules if available
if ADVANCED_BINDINGS_AVAILABLE:
    # Import from the advanced modules directory
    import importlib.util
    import os
    
    # Load advanced modules
    advanced_modules = {
        'brain_advanced': 'src/python/advanced/brain_advanced.py',
        'agent_advanced': 'src/python/advanced/agent_advanced.py',
        'config_advanced': 'src/python/advanced/config_advanced.py',
        'world_advanced': 'src/python/advanced/world_advanced.py',
        'sensory_advanced': 'src/python/advanced/sensory_advanced.py',
        'memory_advanced': 'src/python/advanced/memory_advanced.py',
        'development_advanced': 'src/python/advanced/development_advanced.py',
        'neuromodulation_advanced': 'src/python/advanced/neuromodulation_advanced.py',
        'prediction_advanced': 'src/python/advanced/prediction_advanced.py',
        'performance_advanced': 'src/python/advanced/performance_advanced.py',
        'export_advanced': 'src/python/advanced/export_advanced.py',
        'tools_advanced': 'src/python/advanced/tools_advanced.py',
        'plugins_advanced': 'src/python/advanced/plugins_advanced.py',
        'errors_advanced': 'src/python/advanced/errors_advanced.py',
    }
    
    for module_name, file_path in advanced_modules.items():
        if os.path.exists(file_path):
            spec = importlib.util.spec_from_file_location(module_name, file_path)
            module = importlib.util.module_from_spec(spec)
            sys.modules[module_name] = module
            try:
                spec.loader.exec_module(module)
                # Make module attributes available at package level
                globals()[module_name] = module
            except Exception as e:
                print(f"✗ Failed to load {module_name}: {e}")
        else:
            print(f"⚠ File not found: {file_path}")

    # Create class aliases for backward compatibility
    try:
        BrainAdvanced = brain_advanced.BrainAdvanced
        AgentBrainAdvanced = agent_advanced.AgentBrainAdvanced
        ConfigAdvanced = config_advanced.ConfigAdvanced
        SimpleWorldAdvanced = world_advanced.SimpleWorldAdvanced
        SensoryInputAdvanced = sensory_advanced.SensoryInputAdvanced
        MemorySystemAdvanced = memory_advanced.MemorySystemAdvanced
        DevelopmentSystemAdvanced = development_advanced.DevelopmentSystemAdvanced
        NeuromodulationSystemAdvanced = neuromodulation_advanced.NeuromodulationSystemAdvanced
        PredictionSystemAdvanced = prediction_advanced.PredictionSystemAdvanced
        PerformanceSystemAdvanced = performance_advanced.PerformanceSystemAdvanced
        ExportSystemAdvanced = export_advanced.ExportSystemAdvanced
        ToolSystemAdvanced = tools_advanced.ToolSystemAdvanced
        PluginSystemAdvanced = plugins_advanced.PluginSystemAdvanced
        ErrorSystemAdvanced = errors_advanced.ErrorSystemAdvanced
    except NameError:
        print("⚠ Warning: Some advanced modules not available")

    # Helper factory functions
    def create_brain_advanced(config=None):
        """Create an advanced Brain instance"""
        if config is None:
            config = createDefaultConfig()
        return BrainAdvanced(createBrain(config))
    
    def create_agent_brain_advanced(brain=None):
        """Create an advanced AgentBrain instance"""
        if brain is None:
            brain = create_brain_advanced()
        return AgentBrainAdvanced(createAgentBrain(brain))
    
    def create_simple_world_advanced():
        """Create an advanced SimpleWorld instance"""
        return SimpleWorldAdvanced(createSimpleWorld())
    
    def validate_configuration(config):
        """Validate configuration and return validation results"""
        errors = config.getValidationErrors()
        warnings = config.getValidationWarnings()
        return {
            'valid': len(errors) == 0,
            'errors': errors,
            'warnings': warnings,
            'is_valid': len(errors) == 0,
        }
    
    def optimize_configuration(config):
        """Get optimization hints for configuration"""
        return config.getParameterOptimizationHints()
    
    def get_configuration_stats(config):
        """Get statistics about configuration"""
        return config.getConfigurationStats()
    
    def create_standard_config():
        """Create a standard configuration with reasonable defaults"""
        config = ConfigAdvanced()
        
        # Set standard parameters
        config.set("brain.neuronCount", 10000)
        config.set("brain.regionCount", 5)
        config.set("simulation.timeStep", 0.01)
        config.set("simulation.maxSteps", 1000)
        config.set("memory.workingMemory.capacity", 1000)
        config.set("memory.episodicMemory.capacity", 5000)
        config.set("neuromodulation.enabled", True)
        config.set("development.enabled", True)
        config.set("prediction.enabled", True)
        
        return config
    
    def create_standard_brain():
        """Create a brain with standard configuration"""
        return create_brain_advanced(create_standard_config())
    
    def create_agent_with_standard_brain(world=None):
        """Create an agent brain with standard configuration"""
        if world is None:
            world = create_simple_world_advanced()
        brain = create_standard_brain()
        return create_agent_brain_advanced(brain)
else:
    # Fallback implementations when advanced bindings are not available
    print("\n⚠ Warning: Advanced bindings not available")
    print("Available basic functionality from pynlm module")
    print("To enable advanced bindings, ensure pynlm is properly installed")

# Diagnostic and debugging utilities

def diagnose_system(brain):
    """Comprehensive system diagnosis"""
    if ADVANCED_BINDINGS_AVAILABLE and 'BrainAdvanced' in globals():
        try:
            brain_advanced_obj = BrainAdvanced(brain)
            diagnostics = {}
            
            # Check brain status
            diagnostics['brain'] = {
                'initialized': brain_advanced_obj.getBrain() is not None,
                'region_count': brain_advanced_obj.getRegionCount(),
                'neuron_count': brain_advanced_obj.getTotalNeuronCount(),
                'developmental_stage': brain_advanced_obj.getDevelopmentalStage(),
                'neuromodulation_level': brain_advanced_obj.getNeuromodulationLevel(),
            }
            
            # Check memory systems
            diagnostics['memory'] = {
                'working_memory': {
                    'capacity': brain_advanced_obj.getWorkingMemoryStats().get('capacity', 0),
                    'utilization': brain_advanced_obj.getMemoryUtilization(),
                },
            }
            
            # Check prediction system
            diagnostics['prediction'] = {
                'accuracy': brain_advanced_obj.getPredictionAccuracy(),
                'error': brain_advanced_obj.getPredictionError(),
                'confidence': brain_advanced_obj.getPredictionConfidence(),
            }
            
            return diagnostics
        except Exception as e:
            return {
                'status': 'diagnosis_error',
                'message': f'Failed to diagnose brain: {e}',
            }
    else:
        return {
            'status': 'basic_bindings_only',
            'message': 'Advanced diagnostics require pynlm advanced bindings',
            'brain_info': 'Use basic pynlm functionality',
        }

def export_diagnostics(brain, filepath):
    """Export diagnostic data to file"""
    diagnostics = diagnose_system(brain)
    diagnostics['export_timestamp'] = datetime.datetime.now().isoformat()
    diagnostics['version'] = VERSION
    
    with open(filepath, 'w') as f:
        json.dump(diagnostics, f, indent=2, default=str)
    
    return f"Diagnostics exported to {filepath}"

# Print startup information
print("\n" + "="*60)
print("NLM Advanced Python Bindings")
print("="*60)
if ADVANCED_BINDINGS_AVAILABLE:
    print("✓ All advanced modules loaded successfully")
    print("✓ Comprehensive Python APIs for all NLM systems")
    print("✓ Development, neuromodulation, memory, prediction, and performance APIs")
    print("✓ Error handling, tool, plugin, and export APIs")
else:
    print("⚠ Limited functionality - basic pynlm bindings available")
    print("⚠ For advanced features, install NLM with advanced Python support")
print("="*60)

# List available advanced classes
print("\nAvailable Advanced Classes:")
for class_name in __all__:
    print(f"  • {class_name}")

print("\nAvailable Core Classes (from basic pynlm):")
core_classes = ['Brain', 'AgentBrain', 'Config', 'SimpleWorld', 'Vision', 
               'Audio', 'InternalSignals', 'SensoryInput', 'Action',
               'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept']
for class_name in core_classes:
    print(f"  • {class_name}")

print("\nAdvanced APIs Include:")
advanced_apis = [
    "Brain: Development, Neuromodulation, Memory, Prediction, Performance APIs",
    "AgentBrain: Cognitive, Integration, Development APIs",
    "Config: Schema, Validation, Analysis APIs",
    "SimpleWorld: Metrics, Environment, Performance APIs",
    "SensoryInput: Vision, Audio, Internal Signals APIs",
    "Error System: NLM-specific and Python error APIs",
    "Development System: Stage, Progression, Tools APIs",
    "Neuromodulation System: Modulators, State, Dynamics APIs",
    "Memory System: Working, Episodic, Semantic, Procedural APIs",
    "Prediction System: System, State, Performance APIs",
    "Performance System: Metrics, Monitoring, Optimization APIs",
    "Export System: State, Reports, Data Export APIs",
    "Tool System: Execution, Pipelines, Output APIs",
    "Plugin System: Information, Management, Configuration APIs",
]
for api in advanced_apis:
    print(f"  • {api}")

print(f"\nFor more information, visit: https://nlm.readthedocs.io")
print(f"Current version: {VERSION}")
print(f"Platform: {PLATFORM}")
print(f"Python: {PYTHON_VERSION}")

# Note about dependencies
print("\n" + "="*60)
print("Note: Advanced features require compiled C++ extensions")
print("To build with advanced support, run:")
print("  pip install -e .[dev]")
print("  cmake -DADVANCED_PYTHON_BINDINGS=ON ..")
print("="*60)

print("\nNLM Advanced Python Bindings loaded successfully!")
print("Available classes and functions are now available from this module.")