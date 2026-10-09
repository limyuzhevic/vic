#!/usr/bin/env python3
"""
NLM Python Package Initialization
==================================

This is the main initialization file for the NLM Python package.
It provides package metadata and entry points for the NLM Python modules.
"""

__version__ = '0.1.0'
__author__ = 'NLM Authors'
__description__ = 'Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework'

# Core module imports
from .nlm_python_module import (
    NLPSimulation,
    SimulationState,
    ConnectionType,
    PlasticityType,
    NeuromodulatorType,
    AnalysisMetric,
    ScriptingMode,
)

# Advanced learning algorithms module imports
from .advanced_learning_algorithms import (
    AdvancedLearningAlgorithms,
    advanced_learning,
    AdvancedSTDP,
    AdvancedLearningFramework,
    PlasticityState,
    Spike,
)

# Configuration module imports
from .nlm_configuration_module import (
    NetworkArchitecture,
    SynapticConnection,
    PlasticityController,
    NeuromodulationTuner,
    ParameterOptimizer,
    RealTimeParameterTuner,
    NLMConfigurator,
    ConfigurationManager,
)

# Analysis module imports
from .nlm_analysis_module import (
    NeuralActivityAnalyzer,
    NetworkTopologyAnalyzer,
    LearningProgressAnalyzer,
    VisualizationEngine,
    NLPMonitoring,
    NLPAnalysisSuite,
    AnalysisType,
    VisualizationType,
    AnalysisWindow,
)

# Machine learning module imports
from .nlm_ml_module import (
    DeepLearningFramework,
    CustomNeuralNetwork,
    FeedForwardNN,
    RecurrentNN,
    TransformerModel,
    ModelValidator,
    LearningAlgorithmType,
    ModelArchitecture,
    LearningObjective,
    OptimizationMethod,
    TrainingStrategy,
)

# Debug module imports
from .nlm_debug_module import (
    NLPToolkit,
    ErrorSeverity,
    ErrorCategory,
    ErrorContext,
    NLMError,
    ErrorHandler,
    Debugger,
    PerformanceMonitor,
    SystemMonitor,
)

# Package metadata
__all__ = [
    # Core classes
    'NLPSimulation',
    'SimulationState',
    'ConnectionType',
    'PlasticityType',
    'NeuromodulatorType',
    'AnalysisMetric',
    'ScriptingMode',
    
    # Configuration classes
    'NetworkArchitecture',
    'SynapticConnection',
    'PlasticityController',
    'NeuromodulationTuner',
    'ParameterOptimizer',
    'RealTimeParameterTuner',
    'NLMConfigurator',
    'ConfigurationManager',
    
    # Analysis classes
    'NeuralActivityAnalyzer',
    'NetworkTopologyAnalyzer',
    'LearningProgressAnalyzer',
    'VisualizationEngine',
    'NLPMonitoring',
    'NLPAnalysisSuite',
    'AnalysisType',
    'VisualizationType',
    'AnalysisWindow',
    
    # Machine learning classes
    'DeepLearningFramework',
    'CustomNeuralNetwork',
    'FeedForwardNN',
    'RecurrentNN',
    'TransformerModel',
    'ModelValidator',
    'LearningAlgorithmType',
    'ModelArchitecture',
    'LearningObjective',
    'OptimizationMethod',
    'TrainingStrategy',
    
    # Advanced learning classes
    'AdvancedLearningAlgorithms',
    'advanced_learning',
    'AdvancedSTDP',
    'AdvancedLearningFramework',
    'PlasticityState',
    'Spike',
]

# Convenience functions
def create_default_simulation(config_file=None, headless=False):
    """
    Create a default NLM simulation with recommended configuration.
    
    Args:
        config_file: Path to configuration file
        headless: Whether to run without UI
        
    Returns:
        NLPSimulation instance
    """
    return NLPSimulation(config_file=config_file, headless=headless)

def create_configuration(config_file=None):
    """
    Create a default NLM configuration.
    
    Args:
        config_file: Path to configuration file
        
    Returns:
        NLMConfigurator instance
    """
    from .nlm_configuration_module import NLMConfigurator
    configurator = NLMConfigurator()
    
    if config_file:
        # Load configuration if file specified
        # This would require a method to load from file
        pass
    
    return configurator

def create_analysis(simulation=None):
    """
    Create analysis suite for NLM simulation.
    
    Args:
        simulation: NLM simulation instance
        
    Returns:
        NLPAnalysisSuite instance
    """
    from .nlm_analysis_module import NLPAnalysisSuite
    return NLPAnalysisSuite(simulation)

def create_toolkit(simulation=None):
    """
    Create debugging and performance toolkit.
    
    Args:
        simulation: NLM simulation instance
        
    Returns:
        NLPToolkit instance
    """
    from .nlm_debug_module import NLPToolkit
    return NLPToolkit(simulation)

# Module-level functions
def get_version():
    """Get package version."""
    return __version__

def get_description():
    """Get package description."""
    return __description__

# Main entry point for command-line interface
def main():
    """
    Main entry point for the NLM command-line interface.
    
    This provides a unified interface to all NLM Python tools.
    """
    import argparse
    import sys
    
    parser = argparse.ArgumentParser(
        description='NLM Python Module - Brain Emulation and Neural Network Tools',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm configure --create my_brain
  nlm simulate --config my_config.json
  nlm analyze --input results.json --output plot.png
  nlm ml train --model-type feed_forward
  nlm debug --monitor --log-level INFO
  nlm explore --help

For more information, visit: https://github.com/nlm-project/nlm
        """
    )
    
    parser.add_argument(
        '--version',
        action='version',
        version=f'NLM Python Module {__version__}'
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Configuration command
    config_parser = subparsers.add_parser('configure', help='Configuration management')
    config_parser.add_argument('--create', help='Create new configuration')
    config_parser.add_argument('--load', help='Load existing configuration')
    config_parser.add_argument('--save', help='Save configuration')
    config_parser.add_argument('--list', action='store_true', help='List available configurations')
    
    # Simulation command
    sim_parser = subparsers.add_parser('simulate', help='Simulation control')
    sim_parser.add_argument('--config', help='Configuration file')
    sim_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
    sim_parser.add_argument('--timestep', type=float, default=0.01, help='Simulation timestep')
    sim_parser.add_argument('--headless', action='store_true', help='Run without UI')
    sim_parser.add_argument('--monitor', action='store_true', help='Enable monitoring')
    
    # Analysis command
    analysis_parser = subparsers.add_parser('analyze', help='Data analysis')
    analysis_parser.add_argument('--input', help='Input file')
    analysis_parser.add_argument('--output', help='Output file')
    analysis_parser.add_argument('--type', choices=['neural', 'topology', 'learning', 'all'], 
                                default='neural', help='Analysis type')
    analysis_parser.add_argument('--visualization', help='Visualization type')
    
    # Machine learning command
    ml_parser = subparsers.add_parser('ml', help='Machine learning tools')
    ml_parser.add_argument('--model-type', choices=['feed_forward', 'recurrent', 'transformer'],
                           default='feed_forward', help='Model type')
    ml_parser.add_argument('--train', action='store_true', help='Train model')
    ml_parser.add_argument('--predict', action='store_true', help='Make predictions')
    ml_parser.add_argument('--evaluate', action='store_true', help='Evaluate model')
    
    # Debug command
    debug_parser = subparsers.add_parser('debug', help='Debugging tools')
    debug_parser.add_argument('--monitor', action='store_true', help='Start monitoring')
    debug_parser.add_argument('--log-level', choices=['DEBUG', 'INFO', 'WARNING', 'ERROR', 'CRITICAL'],
                              default='INFO', help='Log level')
    debug_parser.add_argument('--breakpoint', help='Add breakpoint')
    debug_parser.add_argument('--watch', help='Watch variable')
    
    # Exploration command
    explore_parser = subparsers.add_parser('explore', help='Explore neural networks')
    explore_parser.add_argument('--analysis', choices=['spike', 'connectivity', 'topology', 'plasticity'],
                               help='Analysis type')
    explore_parser.add_argument('--visualization', help='Visualization type')
    explore_parser.add_argument('--output', help='Output file')
    
    # Parse arguments
    args = parser.parse_args()
    
    # Handle commands
    if args.command == 'configure':
        if args.create:
            print(f"Creating configuration: {args.create}")
            # Implementation would go here
        elif args.load:
            print(f"Loading configuration: {args.load}")
            # Implementation would go here
        elif args.save:
            print(f"Saving configuration: {args.save}")
            # Implementation would go here
        elif args.list:
            print("Available configurations:")
            # Implementation would go here
        else:
            parser.print_help()
            
    elif args.command == 'simulate':
        print(f"Starting simulation with {args.steps} steps")
        print(f"Timestep: {args.timestep}")
        print(f"Headless: {args.headless}")
        print(f"Monitor: {args.monitor}")
        # Implementation would go here
        
    elif args.command == 'analyze':
        if args.input:
            print(f"Analyzing input: {args.input}")
            print(f"Output: {args.output}")
            print(f"Type: {args.type}")
            print(f"Visualization: {args.visualization}")
            # Implementation would go here
        else:
            parser.print_help()
            
    elif args.command == 'ml':
        print(f"Machine learning operations:")
        print(f"  Model type: {args.model_type}")
        print(f"  Train: {args.train}")
        print(f"  Predict: {args.predict}")
        print(f"  Evaluate: {args.evaluate}")
        # Implementation would go here
        
    elif args.command == 'debug':
        print(f"Debugging operations:")
        print(f"  Monitor: {args.monitor}")
        print(f"  Log level: {args.log_level}")
        print(f"  Breakpoint: {args.breakpoint}")
        print(f"  Watch: {args.watch}")
        # Implementation would go here
        
    elif args.command == 'explore':
        print(f"Exploration operations:")
        print(f"  Analysis: {args.analysis}")
        print(f"  Visualization: {args.visualization}")
        print(f"  Output: {args.output}")
        # Implementation would go here
        
    else:
        parser.print_help()

if __name__ == '__main__':
    main()
