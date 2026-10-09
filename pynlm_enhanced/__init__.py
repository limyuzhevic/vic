"""
Enhanced NLM Python Bindings

A comprehensive Python interface for the NLM (Neural Learning Machine) C++ framework.
Provides both simple beginner-friendly APIs and advanced features for researchers and developers.
"""

import sys
import os

# Add custom path to Python module
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    # Import the enhanced bindings
    from .bindings_enhanced import (
        # Core classes
        EnhancedConfig,
        EnhancedBrain,
        EnhancedAgentBrain,
        createCompleteAgent,
        runEpisode,
        createSimpleBrain,
        createConfig,
        runScript,
        
        # Original bindings (for compatibility)
        createDefaultConfig,
        createBrain,
        createSimpleWorld,
        createAgentBrain,
        
        # Exceptions
        NLMBrainError,
        NLMConfigurationError,
        NLMMemoryError,
        NLMPredictionError,
    )
    
    # Re-export everything
    __all__ = [
        # Core classes
        'EnhancedConfig',
        'EnhancedBrain', 
        'EnhancedAgentBrain',
        
        # Helper functions
        'createCompleteAgent',
        'runEpisode',
        'createSimpleBrain',
        'createConfig',
        'runScript',
        
        # Original API (for compatibility)
        'createDefaultConfig',
        'createBrain',
        'createSimpleWorld',
        'createAgentBrain',
        
        # Exceptions
        'NLMBrainError',
        'NLMConfigurationError',
        'NLMMemoryError',
        'NLMPredictionError',
        
        # Version
        '__version__',
    ]
    
    __version__ = "2.0.0"
    
    # Simple API for beginners
    def create_explorer():
        """Create a complete explorer agent setup (highest learning capacity)"""
        return createCompleteAgent("explorer")
    
    def create_developer():
        """Create a complete developer agent setup (moderate learning capacity)"""
        return createCompleteAgent("developer")
    
    def create_researcher():
        """Create a complete researcher agent setup (high learning capacity)"""
        return createCompleteAgent("researcher")
    
    def create_beginner():
        """Create a complete beginner agent setup (basic learning capacity)"""
        return createCompleteAgent("beginner")
    
    def run_reproduction_experiment(brain, world, agent, steps=1000):
        """Run a standard reproduction experiment with basic monitoring"""
        print(f"Running reproduction experiment with {brain.getTotalNeuronCount()} neurons...")
        
        for step in range(steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 100 == 0:
                metrics = brain.getPerformanceMetrics()
                print(f"Step {step}: Firing={brain.getFiringNeuronCount()}, "
                      f"Reward={metrics.curiosityLevel:.3f}, "
                      f"Development={brain.getDevelopmentStage()}")
        
        print("✓ Experiment completed successfully")
    
    def quick_test():
        """Quick test to verify NLM installation"""
        try:
            config = createDefaultConfig()
            brain = createBrain(config)
            brain.initialize()
            
            print(f"✓ NLM Test Successful!")
            print(f"  - Neurons: {brain.getTotalNeuronCount()}")
            print(f"  - Synapses: {brain.getTotalSynapseCount()}")
            print(f"  - Development Stage: {brain.getDevelopmentStage()}")
            
            # Quick simulation
            for i in range(10):
                brain.step(i)
            
            print(f"  - Test completed: {brain.getTotalSpikeCount()} spikes")
            return True
        except Exception as e:
            print(f"✗ NLM Test Failed: {e}")
            return False
    
    # Add convenience functions to __all__
    __all__.extend([
        'create_explorer',
        'create_developer', 
        'create_researcher',
        'create_beginner',
        'run_reproduction_experiment',
        'quick_test',
    ])
    
except ImportError as e:
    # Fallback for when compiled bindings are not available
    print(f"Warning: Enhanced NLM bindings not available: {e}")
    print("Trying to import original bindings...")
    
    try:
        # Import original bindings
        import pynlm
        
        # Re-export everything from original pynlm
        globals().update(pynlm.__dict__)
        
        # Add simple API functions
        def create_explorer():
            """Create a complete explorer agent setup (highest learning capacity)"""
            brain, world, agent = pynlm.createCompleteAgent("explorer")
            return brain, world, agent
        
        # Add other simple functions...
        # (continuing from above)
        
    except ImportError as e2:
        print(f"Error: NLM Python bindings not available: {e2}")
        raise ImportError("NLM Python bindings are required but not available")

# Module metadata
__author__ = "NLM Project Team"
__copyright__ = "Copyright 2024 NLM Project"
__license__ = "MIT"
__email__ = "contact@nlm-project.org"
__status__ = "Development"

# Print warning if running in development
if __name__ == "__main__":
    print("NLM Python Bindings Module")
    print(f"Version: {__version__}")
    print(f"Status: {__status__}")
    print("\nTo use NLM:")
    print("  import pynlm")
    print("  # See documentation for usage examples")
    print("\nQuick test:")
    quick_test()