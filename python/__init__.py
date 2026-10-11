# NLM Python package
# Main entry point for NLM Python bindings

__version__ = "0.1.0"
__author__ = "NLM Project"
__description__ = "Python bindings for NLM (Neural Learning Machine)"

# Try to import core bindings
try:
    from .core import Config, Brain, SimpleWorld, AgentBrain
    from .types import NeuronId, SynapseId, RegionId, PopulationId
    from .enums import NeuronType, SynapseType, DevelopmentalStage, FiringState
    from .sensory import Vision, Audio, InternalSignals
    from .motor import Action, MotorCommand
    from .world import WorldObject, AgentBody, ActionResult
    from .experiments import Phase6Config, Phase6IntegrationResult
    
    CORE_AVAILABLE = True
except ImportError:
    CORE_AVAILABLE = False

# Try to import advanced bindings
try:
    from .advanced import (
        create_advanced_config,
        compare_configs,
        create_brain_with_architecture,
        inject_complex_input,
        create_simulation_controller,
        create_performance_profiler,
        analyze_neural_activity,
        detect_activity_patterns,
        create_ml_enhanced_brain,
        create_state_manager,
        create_advanced_experiment,
        clone_brain_with_transformation,
        batch_brain_simulation,
        enable_debug_mode,
        get_system_health,
        create_custom_module,
        with_fault_tolerance,
        get_advanced_api_docs,
        parse_advanced_args,
        generate_help
    )
    ADVANCED_AVAILABLE = True
except ImportError:
    ADVANCED_AVAILABLE = False

# Re-export for backward compatibility
if CORE_AVAILABLE:
    from pynlm import (
        Config,
        Brain,
        SimpleWorld,
        AgentBrain,
        NeuronId,
        SynapseId,
        RegionId,
        PopulationId,
        NeuronType,
        SynapseType,
        DevelopmentalStage,
        FiringState,
        Vision,
        Audio,
        InternalSignals,
        Action,
        MotorCommand,
        WorldObject,
        AgentBody,
        ActionResult
    )

# Convenience functions
def create_default_config():
    """Create a default NLM configuration"""
    if not CORE_AVAILABLE:
        raise ImportError("Core NLM bindings not available")
    return Config()

def create_brain(config=None):
    """Create an NLM brain with optional configuration"""
    if not CORE_AVAILABLE:
        raise ImportError("Core NLM bindings not available")
    if config is None:
        config = create_default_config()
    return Brain(config)

def create_simple_world(width=None, height=None, vision_width=None, vision_height=None):
    """Create a simple NLM world"""
    if not CORE_AVAILABLE:
        raise ImportError("Core NLM bindings not available")
    world = SimpleWorld()
    if width is not None and height is not None:
        world.configure(width, height, vision_width or 8, vision_height or 8)
    return world

def create_agent_brain(brain):
    """Create an agent brain interface"""
    if not CORE_AVAILABLE:
        raise ImportError("Core NLM bindings not available")
    return AgentBrain(brain)

# Version and metadata
__all__ = [
    # Core components
    'Config',
    'Brain',
    'SimpleWorld',
    'AgentBrain',
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    'MotorCommand',
    'WorldObject',
    'AgentBody',
    'ActionResult',
    
    # Advanced components
    'Phase6Config',
    'Phase6IntegrationResult',
    
    # Convenience functions
    'create_default_config',
    'create_brain',
    'create_simple_world',
    'create_agent_brain',
    
    # Package metadata
    '__version__',
    '__author__',
    '__description__',
    'CORE_AVAILABLE',
    'ADVANCED_AVAILABLE'
]

# Package setup check
def check_setup():
    """Check if NLM Python package is properly set up"""
    issues = []
    
    if not CORE_AVAILABLE:
        issues.append("Core NLM bindings not available")
    
    if ADVANCED_AVAILABLE:
        print("✓ Core NLM bindings: Available")
        print("✓ Advanced NLM bindings: Available")
        print("✓ All dependencies satisfied")
    else:
        print("✗ Advanced NLM bindings: Not available")
        issues.append("Advanced NLM bindings not available")
    
    if issues:
        print(f"\nSetup issues found ({len(issues)}):")
        for issue in issues:
            print(f"  - {issue}")
        return False
    else:
        print("\n✓ NLM Python package setup complete")
        return True

if __name__ == '__main__':
    check_setup()