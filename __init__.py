#!/usr/bin/env python3
"""
pynlm - NLM (熙然) Neural Learning Machine Python Bindings
Comprehensive interface to the NLM C++ neural simulation framework.

This module provides access to:
- Core neural components (neurons, synapses, regions)
- Plasticity rules (STDP, Hebbian, reward-modulated learning)
- Memory systems (working, episodic, associative memory)
- Neuromodulation (dopamine, curiosity, novelty, prediction error)
- Prediction and cognition systems
- Development and aging mechanisms
- Performance optimizations (checkpoints, sparse connectivity)
- Advanced simulation controls

Usage:
    import pynlm
    brain = pynlm.create_brain(pynlm.create_default_config())
    # ... use brain for neural simulation
"""

__version__ = "0.1.0"
__author__ = "NLM Authors"

# Core types and classes are imported from the C++ extension via pybind11
# The C++ extension _pynlm provides all the actual bindings
try:
    from pynlm._pynlm import (
        # Core types
        NeuronId, SynapseId, RegionId, PopulationId,
        NeuronType, SynapseType, DevelopmentalStage, FiringState,
        ActionType, MotorCommand, WorldObjectType,
        
        # Core classes
        Config, Brain, AgentBrain, SimpleWorld,
        Neuron, Synapse, NeuralRegion, NeuralPopulation,
        
        # Plasticity rules
        PlasticityRule, HebbianRule, STDP, RewardModulatedSTDP,
        TripletSTDP, StructuralPlasticity,
        
        # Memory systems
        Memory, NeuralWorkingMemory, NeuralEpisodicMemory,
        
        # Neuromodulation
        Neuromodulator, Reward, Curiosity, Novelty, PredictionError,
        
        # Development
        DevelopmentSystem, Maturation, Synaptogenesis, Pruning,
        
        # Performance
        MemoryPool, EventQueue, CheckpointSystem, SparseConnectivity,
        OptimizedBrain,
        
        # Sensory/Motor
        Vision, Audio, InternalSignals, Action,
        SensoryPercept, AgentBody, WorldObject,
        
        # Environment/Experiments
        Environment, Experiment, ExperimentRunner,
        
        # Cognition
        NeuralPlanner, ConceptFormation,
        
        # Utilities
        SimulationClock, RandomGenerator, Logger,
        
        # Factory functions
        create_default_config,
        create_brain, create_agent_brain, create_simple_world,
        create_neural_region,
        create_stdp_rule, create_hebbian_rule,
        create_reward_modulated_stdp,
        create_working_memory, create_episodic_memory,
        create_reward_modulator, create_curiosity_module,
        create_prediction_system,
    )
    
    # Set up __all__ for proper module exports
    __all__ = [
        # Core types
        'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
        'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
        'ActionType', 'MotorCommand', 'WorldObjectType',
        
        # Core classes
        'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
        'Neuron', 'Synapse', 'NeuralRegion', 'NeuralPopulation',
        
        # Plasticity rules
        'PlasticityRule', 'HebbianRule', 'STDP', 'RewardModulatedSTDP',
        'TripletSTDP', 'StructuralPlasticity',
        
        # Memory systems
        'Memory', 'NeuralWorkingMemory', 'NeuralEpisodicMemory',
        
        # Neuromodulation
        'Neuromodulator', 'Reward', 'Curiosity', 'Novelty', 'PredictionError',
        
        # Development
        'DevelopmentSystem', 'Maturation', 'Synaptogenesis', 'Pruning',
        
        # Performance
        'MemoryPool', 'EventQueue', 'CheckpointSystem', 'SparseConnectivity',
        'OptimizedBrain',
        
        # Sensory/Motor
        'Vision', 'Audio', 'InternalSignals', 'Action',
        'SensoryPercept', 'AgentBody', 'WorldObject',
        
        # Environment/Experiments
        'Environment', 'Experiment', 'ExperimentRunner',
        
        # Cognition
        'NeuralPlanner', 'ConceptFormation',
        
        # Utilities
        'SimulationClock', 'RandomGenerator', 'Logger',
        
        # Factory functions
        'create_default_config',
        'create_brain', 'create_agent_brain', 'create_simple_world',
        'create_neural_region',
        'create_stdp_rule', 'create_hebbian_rule',
        'create_reward_modulated_stdp',
        'create_working_memory', 'create_episodic_memory',
        'create_reward_modulator', 'create_curiosity_module',
        'create_prediction_system',
        
        # Factory functions
        'create_region',
        'create_plasticity_rule',
        'create_memory_system',
        'create_neuromodulator',
        'create_development_module',
        'create_performance_optimizer',
        
        # Utility functions
        'get_version',
        'get_info',
    ]
    
    # Backward compatibility aliases
    def __getattr__(name):
        """Handle backward compatibility for renamed or moved classes."""
        # All classes are already imported from _pynlm
        # This handles the case where someone tries to access the class
        # through a different path
        return globals().get(name)
    
    # Import utility functions
    def create_region(name: str) -> 'NeuralRegion':
        """Create a new neural region with the given name."""
        return create_neural_region(name)

    def create_plasticity_rule(rule_type: str, **kwargs) -> 'PlasticityRule':
        """
        Create a plasticity rule of the specified type.
        
        Args:
            rule_type: Type of plasticity rule ("stdp", "hebbian", 
                      "reward_modulated_stdp", "triplet_stdp", 
                      "structural_plasticity")
            **kwargs: Rule-specific parameters
        
        Returns:
            PlasticityRule instance
        """
        rule_type = rule_type.lower()
        if rule_type == "stdp":
            return create_stdp_rule(**kwargs)
        elif rule_type == "hebbian":
            return create_hebbian_rule(**kwargs)
        elif rule_type == "reward_modulated_stdp":
            return create_reward_modulated_stdp(**kwargs)
        elif rule_type == "triplet_stdp":
            return create_stdp_rule(rule_type="triplet", **kwargs)
        elif rule_type == "structural_plasticity":
            return create_stdp_rule(rule_type="structural", **kwargs)
        else:
            raise ValueError(f"Unknown plasticity rule type: {rule_type}")

    def create_memory_system(memory_type: str, **kwargs):
        """
        Create a memory system of the specified type.
        
        Args:
            memory_type: Type of memory system ("working", "episodic")
            **kwargs: Memory-specific parameters
        
        Returns:
            Memory system instance
        """
        memory_type = memory_type.lower()
        if memory_type == "working":
            return create_working_memory(**kwargs)
        elif memory_type == "episodic":
            return create_episodic_memory(**kwargs)
        else:
            raise ValueError(f"Unknown memory system type: {memory_type}")

    def create_neuromodulator(modulator_type: str, **kwargs):
        """
        Create a neuromodulator of the specified type.
        
        Args:
            modulator_type: Type of neuromodulator ("reward", "curiosity",
                           "novelty", "prediction_error")
            **kwargs: Neuromodulator-specific parameters
        
        Returns:
            Neuromodulator instance
        """
        modulator_type = modulator_type.lower()
        if modulator_type == "reward":
            return create_reward_modulator(**kwargs)
        elif modulator_type == "curiosity":
            return create_curiosity_module(**kwargs)
        elif modulator_type == "novelty":
            return Novelty(**kwargs)
        elif modulator_type == "prediction_error":
            return PredictionError(**kwargs)
        else:
            raise ValueError(f"Unknown neuromodulator type: {modulator_type}")

    def create_development_module(dev_type: str, **kwargs):
        """
        Create a development module of the specified type.
        
        Args:
            dev_type: Type of development module ("maturation", "synaptogenesis",
                     "pruning")
            **kwargs: Development module-specific parameters
        
        Returns:
            Development module instance
        """
        dev_type = dev_type.lower()
        if dev_type == "maturation":
            return Maturation(**kwargs)
        elif dev_type == "synaptogenesis":
            return Synaptogenesis(**kwargs)
        elif dev_type == "pruning":
            return Pruning(**kwargs)
        else:
            raise ValueError(f"Unknown development module type: {dev_type}")

    def create_performance_optimizer(opt_type: str, **kwargs):
        """
        Create a performance optimization module.
        
        Args:
            opt_type: Type of optimization ("memory_pool", "event_queue",
                     "checkpoint_system", "sparse_connectivity", "optimized_brain")
            **kwargs: Optimization-specific parameters
        
        Returns:
            Performance optimization instance
        """
        opt_type = opt_type.lower()
        if opt_type == "memory_pool":
            return MemoryPool(**kwargs)
        elif opt_type == "event_queue":
            return EventQueue(**kwargs)
        elif opt_type == "checkpoint_system":
            return CheckpointSystem(**kwargs)
        elif opt_type == "sparse_connectivity":
            return SparseConnectivity(**kwargs)
        elif opt_type == "optimized_brain":
            return OptimizedBrain(**kwargs)
        else:
            raise ValueError(f"Unknown optimization type: {opt_type}")

    def get_version():
        """Return the version of pynlm."""
        return __version__

    def get_info():
        """
        Return information about the NLM Python bindings.
        
        Returns:
            dict: Information about the bindings
        """
        return {
            'version': __version__,
            'author': __author__,
            'description': __doc__,
            'available_classes': __all__,
        }
    
except ImportError as e:
    print(f"Warning: Failed to import pynlm C++ extension: {e}")
    print("Python bindings are not available. Please build the pynlm module.")
    print("See the README.md for build instructions.")
    
    # Define placeholder classes for better error messages
    class _ImportError:
        def __init__(self, name):
            self.name = name
        
        def __getattr__(self, item):
            raise ImportError(f"Cannot import name '{item}' from 'pynlm'. "
                           f"The pynlm module failed to load. "
                           f"Please build the pynlm module first.")
        
        def __call__(self, *args, **kwargs):
            raise ImportError(f"Cannot call '{self.name}'. "
                           f"The pynlm module failed to load. "
                           f"Please build the pynlm module first.")
    
    # Create placeholder classes for all expected exports
    NeuronId = SynapseId = RegionId = PopulationId = _ImportError("NeuronId/SynapseId/RegionId/PopulationId")
    NeuronType = SynapseType = DevelopmentalStage = FiringState = _ImportError("NeuronType/SynapseType/DevelopmentalStage/FiringState")
    ActionType = MotorCommand = WorldObjectType = _ImportError("ActionType/MotorCommand/WorldObjectType")
    Config = Brain = AgentBrain = SimpleWorld = _ImportError("Config/Brain/AgentBrain/SimpleWorld")
    Neuron = Synapse = NeuralRegion = NeuralPopulation = _ImportError("Neuron/Synapse/NeuralRegion/NeuralPopulation")
    PlasticityRule = HebbianRule = STDP = _ImportError("PlasticityRule/HebbianRule/STDP")
    RewardModulatedSTDP = TripletSTDP = StructuralPlasticity = _ImportError("RewardModulatedSTDP/TripletSTDP/StructuralPlasticity")
    Memory = NeuralWorkingMemory = NeuralEpisodicMemory = _ImportError("Memory/NeuralWorkingMemory/NeuralEpisodicMemory")
    Neuromodulator = Reward = Curiosity = _ImportError("Neuromodulator/Reward/Curiosity")
    Novelty = PredictionError = _ImportError("Novelty/PredictionError")
    DevelopmentSystem = Maturation = Synaptogenesis = Pruning = _ImportError("DevelopmentSystem/Maturation/Synaptogenesis/Pruning")
    MemoryPool = EventQueue = CheckpointSystem = _ImportError("MemoryPool/EventQueue/CheckpointSystem")
    SparseConnectivity = OptimizedBrain = _ImportError("SparseConnectivity/OptimizedBrain")
    Vision = Audio = InternalSignals = _ImportError("Vision/Audio/InternalSignals")
    Action = SensoryPercept = AgentBody = WorldObject = _ImportError("Action/SensoryPercept/AgentBody/WorldObject")
    Environment = Experiment = ExperimentRunner = _ImportError("Environment/Experiment/ExperimentRunner")
    NeuralPlanner = ConceptFormation = _ImportError("NeuralPlanner/ConceptFormation")
    SimulationClock = RandomGenerator = Logger = _ImportError("SimulationClock/RandomGenerator/Logger")
    
    # Factory functions
    create_default_config = create_brain = create_agent_brain = create_simple_world = _ImportError("create_default_config/create_brain/create_agent_brain/create_simple_world")
    create_neural_region = create_stdp_rule = create_hebbian_rule = _ImportError("create_neural_region/create_stdp_rule/create_hebbian_rule")
    create_reward_modulated_stdp = create_working_memory = create_episodic_memory = _ImportError("create_reward_modulated_stdp/create_working_memory/create_episodic_memory")
    create_reward_modulator = create_curiosity_module = create_prediction_system = _ImportError("create_reward_modulator/create_curiosity_module/create_prediction_system")
    
    # Add to __all__
    __all__ = []