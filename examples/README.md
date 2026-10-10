Basic Example
=============
This example demonstrates the core workflow of using the NLM Python bindings:

1. Create a configuration with parameters
2. Initialize a neural brain
3. Set up the simulation world
4. Run a simulation episode
5. Analyze results

The code shows how the improved Python API makes it easy to:
- Create configurations with sensible defaults
- Initialize brains and environments
- Process sensory input and produce actions
- Track simulation statistics

Key improvements over the original bindings:
- Type-safe interfaces with docstrings
- Convenience functions (create_default_config, create_brain)
- Better error handling with meaningful messages
- High-level abstractions for common operations

Example code snippet:
    >>> from pynlm import Config, Brain, SimpleWorld
    >>> config = Config(neuron_count=100, region_count=2)
    >>> brain = Brain(config)
    >>> brain.initialize()
    >>> world = SimpleWorld()
    >>> world.configure(64, 64, 32, 32)
    >>> # ... run simulation

Advanced Example
===============
This example demonstrates the advanced features of the NLM Python bindings:

- Module system with type hints and comprehensive documentation
- Convenience functions for setting up complex scenarios
- High-level action creation (movement, rotation)
- Sensory system integration
- Agent brain interface with neuromodulation

Key improvements:
- Modular design with separate modules for brain, agent, world, sensory
- Pythonic API that feels natural to Python developers
- Extensive error handling and validation
- Support for complex simulation scenarios

Example code snippet:
    >>> from pynlm import create_default_config, create_brain
    >>> >>> config = create_default_config()
    >>> brain = create_brain(config)
    >>> action = create_movement_action("forward", 2.0)

Error Handling Example
=====================
This example demonstrates the improved error handling in the NLM Python bindings:

- Validation of configuration parameters
- Clear error messages for invalid operations
- Graceful handling of missing files and invalid states
- Type checking and safety

Example code snippet:
    >>> config = Config()
    >>> try:
    ...     config.load_from_file("nonexistent.json")
    ... except ConfigError as e:
    ...     print(f"Error: {e}")

API Usage Example
================
This example demonstrates a complete simulation using the full API:

- Creating and configuring all system components
- Running complex simulation scenarios
- Tracking agent behavior and statistics
- Using different action types and sensory modalities

Example code snippet:
    >>> from pynlm import Brain, AgentBrain, SimpleWorld
    >>> >>> brain = Brain(Config(neuron_count=500))
    >>> agent_brain = AgentBrain(brain)
    >>> agent_brain.initialize(world)
    >>> action = brain.produce_action()
    >>> result = world.applyMotorCommand(action, timestep)

Integration with Original System
===============================
The NLM Python bindings maintain full compatibility with the original C++ system:

- All original C++ types and functions are accessible
- Complete backward compatibility with existing code
- No performance compromises - uses the same underlying C++ implementation
- Enables Python development while maintaining C++ performance

Example of integration:
    >>> import pynlm
    >>> # Access low-level C++ extension if needed
    >>> _c_extension = pynlm._c_extension

Benefits for Developers
=======================

1. **Developer Experience**: Type hints, docstrings, and examples make development easy
2. **Productivity**: Convenience functions and high-level abstractions reduce boilerplate
3. **Safety**: Comprehensive error handling prevents runtime issues
4. **Maintainability**: Modular design makes code easier to understand and modify
5. **Testing**: Built-in test suite ensures reliability
6. **Documentation**: Comprehensive examples and docstrings

The NLM Python bindings transform a complex C++ neural simulation framework
into an intuitive, Pythonic API that developers can easily use and extend.