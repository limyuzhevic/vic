#pragma once

#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace nlm {

// Documentation namespace for Python-specific notes and examples
class Documentation {
public:
    // Config class documentation
    static const char* getConfigDocumentation() {
        return R"pbdoc(
        Configuration class for NLM system.
        
        The Config class manages key-value pairs with typed values, supporting
        JSON file I/O, command-line argument parsing, and validation.
        
        Usage examples:
        
        .. code-block:: python
        
            # Create default configuration
            config = Config()
            
            # Load from file
            config.loadFromFile("config.json")
            
            # Set configuration values
            config.set("learning_rate", 0.01)
            config.set("max_steps", 1000)
            config.set("render_mode", True)
            
            # Get configuration values
            lr = config.get("learning_rate")
            max_steps = config.getOr("max_steps", 1000)
            
            # Save configuration
            config.saveToFile("saved_config.json")
            
        .. code-block:: python
        
            # Using ConfigContextManager for automatic resource management
            with ConfigContextManager(config) as cfg:
                cfg.set("simulation", "enabled")
                cfg.set("target_fps", 60)
                
        Note:
            - All values are type-checked for consistency
            - ConfigSource determines value precedence: Default < Runtime < File < CommandLine
            - Configuration is immutable after initialization by default
        )pbdoc";
    }
    
    // Brain class documentation
    static const char* getBrainDocumentation() {
        return R"pbdoc(
        Central neural simulation brain class.
        
        The Brain class implements a complete spiking neural network simulation
        with integrated memory systems, prediction mechanisms, and neuromodulation.
        
        Usage examples:
        
        .. code-block:: python
        
            # Create default config
            config = createDefaultConfig()
            config.set("brain.num_regions", 3)
            config.set("brain.neuron_type", "Excitatory")
            config.set("brain.num_neurons_per_region", 100)
            
            # Create brain with configuration
            with BrainContextManager(createBrain(config)) as brain:
                brain.initialize()
                
                # Run simulation steps
                for step in range(1000):
                    brain.step(step)
                    
                    # Inject sensory input periodically
                    if step % 10 == 0:
                        sensory_input = Vision(100, 100)
                        brain.receiveSensoryInput(sensory_input)
                    
                    # Produce action
                    action = brain.produceAction()
                
                # Save brain state
                brain.save("checkpoint.brain")
        
        Properties:
            - Fully type-safe parameter access
            - Automatic memory management via context managers
            - Thread-safe for concurrent access
            - Checkpoint/restart capabilities
        
        Note:
            - Brain operations are deterministic with fixed random seed
            - All neural computations happen in continuous time simulation
            - External interfaces only (no internal state access)
        )pbdoc";
    }
    
    // AgentBrain class documentation
    static const char* getAgentBrainDocumentation() {
        return R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        The AgentBrain class bridges the gap between neural computation and
        environmental interaction by processing sensory inputs and generating
        motor outputs. It includes curiosity-driven exploration and reward
        modulation systems.
        
        Usage examples:
        
        .. code-block:: python
        
            # Create brain and agent brain
            config = createDefaultConfig()
            brain = createBrain(config)
            world = createSimpleWorld()
            
            # Initialize agent brain with world
            with AgentBrainContextManager(createAgentBrain(brain)) as agent_brain:
                agent_brain.initialize(world)
                
                # Episode loop
                for episode in range(num_episodes):
                    # Reset agent for new episode
                    agent_brain.reset()
                    
                    # Run episode
                    for t in range(max_steps):
                        # Get current sensory percept
                        percept = world.getSensoryPercept()
                        
                        # Process sensory input
                        agent_brain.processSensoryInput(percept)
                        
                        # Get motor command
                        action = agent_brain.decodeMotorCommand()
                        
                        # Apply action to world
                        reward = world.applyMotorCommand(action, t)
                        
                        # Apply reward modulation
                        predicted_reward = agent_brain.getPredictionError()
                        agent_brain.applyRewardModulation(reward, predicted_reward)
                    
                    # Log episode statistics
                    print(f"Episode {episode}: Neuromodulation={agent_brain.getNeuromodulationLevel():.2f}")
        
        Features:
            - Automatic neuromodulation based on reward prediction error
            - Curiosity-driven exploration behavior
            - Developmental stage updates
            - Sensorimotor learning integration
        )pbdoc";
    }
    
    // SimpleWorld class documentation
    static const char* getSimpleWorldDocumentation() {
        return R"pbdoc(
        Simple 2D world for NLM simulation.
        
        The SimpleWorld class provides a physical environment for agents to
        interact with, featuring dynamic objects, collision detection, and
        comprehensive sensory feedback.
        
        Usage examples:
        
        .. code-block:: python
        
            # Create and configure world
            with WorldContextManager(createSimpleWorld()) as world:
                world.configure(width=100.0, height=100.0,
                              visionWidth=50, visionHeight=50)
                world.setAgentStart(10.0, 10.0)
                
                # Add some objects to the world
                world.addObject(WorldObject(x=20.0, y=20.0, type=WorldObjectType.Resource, value=10.0))
                world.addObject(WorldObject(x=80.0, y=80.0, type=WorldObjectType.Hazard, value=-5.0))
                
                # Run simulation
                for t in range(1000):
                    # Update world state
                    world.update(t * 0.1)  # 10ms timestep
                    
                    # Get sensory percept
                    percept = world.getSensoryPercept()
                    
                    # Apply actions and check results
                    if t % 50 == 0:
                        action = Action(ActionType.Eat)
                        result = world.applyMotorCommand(action, t)
                        print(f"Eat action: success={result.success}, reward={result.reward}")
        
        Properties:
            - Deterministic physics simulation
            - Pixel-accurate collision detection
            - Energy conservation and decay
            - Random object spawning capabilities
        
        Note:
            - Vision is processed as grayscale intensity maps
            - All positions are in world coordinates (0,0) to (width,height)
            - Objects are represented as circles for collision detection
        )pbdoc";
    }
    
    // Context manager documentation
    static const char* getContextManagerDocumentation() {
        return R"pbdoc(
        Context managers for automatic resource management.
        
        The context managers ensure proper cleanup of NLM objects and prevent
        resource leaks in Python programs. They provide RAII (Resource Acquisition
        Is Initialization) semantics.
        
        Supported context managers:
        
        .. code-block:: python
        
            # ConfigContextManager
            with ConfigContextManager(config) as cfg:
                # Config is guaranteed to be properly released
                cfg.set("key", "value")
            
            # BrainContextManager
            with BrainContextManager(brain) as brain:
                # Brain simulation is properly managed
                brain.initialize()
                
            # AgentBrainContextManager
            with AgentBrainContextManager(agent_brain) as agent:
                # Agent brain resources are cleaned up
                agent.processSensoryInput(percept)
                
            # WorldContextManager
            with WorldContextManager(world) as w:
                # World simulation is properly managed
                w.update(timestep)
        
        All context managers can be used as decorators or in with statements,
        and provide the same resource (object) reference for use within the block.
        )pbdoc";
    }
};