#!/usr/bin/env python3
"""
NLM Python API Documentation Generator

This script generates comprehensive documentation for the NLM (Neural Learning Machine)
Python bindings, including:
- Class and method documentation with parameters and return types
- Usage examples for common use cases
- Configuration options
- Best practices and gotchas
- API reference guide

The NLM Python bindings provide a Python interface to the C++ neural simulation framework,
allowing users to create and simulate neural networks, brains, and agents using Python.
"""

import os
import json
import datetime
from pathlib import Path

def generate_api_documentation():
    """
    Generate comprehensive NLM Python API documentation.
    
    This function creates a complete documentation file covering all Python classes,
    their methods, parameters, examples, and usage patterns.
    """
    
    # Document template structure
    doc_template = {
        "title": "NLM Python API Documentation",
        "version": "1.0",
        "generated": datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
        "description": "Complete reference for NLM Python bindings",
        "api_reference": {},
        "examples": [],
        "best_practices": [],
        "configuration": {},
        "gotchas": [],
        "quick_start": []
    }
    
    # Build comprehensive API reference
    doc_template["api_reference"] = build_api_reference()
    
    # Add examples
    doc_template["examples"] = build_examples()
    
    # Add best practices
    doc_template["best_practices"] = build_best_practices()
    
    # Add configuration options
    doc_template["configuration"] = build_configuration_options()
    
    # Add common gotchas
    doc_template["gotchas"] = build_gotchas()
    
    # Add quick start guide
    doc_template["quick_start"] = build_quick_start()
    
    # Write documentation to file
    output_file = Path(__file__).parent / "nlm_python_api.md"
    
    with open(output_file, "w", encoding="utf-8") as f:
        f.write(generate_markdown_doc(doc_template))
    
    print(f"✅ API documentation generated: {output_file}")


def build_api_reference():
    """
    Build the API reference section covering all Python classes and their methods.
    
    Returns:
        dict: Complete API reference documentation
    """
    api_reference = {
        "core_identifiers": build_identifier_classes(),
        "neural_units": build_neural_units(),
        "brain_components": build_brain_components(),
        "memory_systems": build_memory_systems(),
        "world_systems": build_world_systems(),
        "sensory_input": build_sensory_classes(),
        "motor_systems": build_motor_classes(),
        "convenience_functions": build_convenience_functions()
    }
    
    return api_reference


def build_identifier_classes():
    """Build documentation for identifier classes."""
    return {
        "NeuronId": {
            "description": "Unique identifier for a neuron",
            "methods": [
                {"name": "__init__", "params": [{"name": "value", "type": "uint64_t", "optional": True}],
                 "return": "NeuronId", "doc": "Create neuron identifier from value"},
                {"name": "value", "type": "property", "doc": "Get the numeric value"},
                {"name": "index", "type": "method", "return": "uint64_t", "doc": "Get neuron index"},
                {"name": "__eq__", "params": [{"name": "other", "type": "NeuronId"}], "return": "bool", "doc": "Equality comparison"},
                {"name": "__ne__", "params": [{"name": "other", "type": "NeuronId"}], "return": "bool", "doc": "Inequality comparison"},
                {"name": "__hash__", "type": "method", "return": "int", "doc": "Hash for use in dictionaries/sets"},
                {"name": "__repr__", "type": "method", "return": "str", "doc": "String representation"}
            ]
        },
        "SynapseId": {
            "description": "Unique identifier for a synapse",
            "methods": [
                {"name": "__init__", "params": [{"name": "value", "type": "uint64_t", "optional": True}], "return": "SynapseId", "doc": "Create synapse identifier"},
                {"name": "value", "type": "property", "doc": "Get the numeric value"},
                {"name": "index", "type": "method", "return": "uint64_t", "doc": "Get synapse index"},
                {"name": "__eq__", "params": [{"name": "other", "type": "SynapseId"}], "return": "bool", "doc": "Equality comparison"},
                {"name": "__ne__", "params": [{"name": "other", "type": "SynapseId"}], "return": "bool", "doc": "Inequality comparison"}
            ]
        },
        "RegionId": {
            "description": "Unique identifier for a brain region",
            "methods": [
                {"name": "__init__", "params": [{"name": "value", "type": "uint64_t", "optional": True}], "return": "RegionId", "doc": "Create region identifier"},
                {"name": "value", "type": "property", "doc": "Get the numeric value"},
                {"name": "index", "type": "method", "return": "uint64_t", "doc": "Get region index"}
            ]
        },
        "PopulationId": {
            "description": "Unique identifier for a neuron population",
            "methods": [
                {"name": "__init__", "params": [{"name": "value", "type": "uint64_t", "optional": True}], "return": "PopulationId", "doc": "Create population identifier"},
                {"name": "value", "type": "property", "doc": "Get the numeric value"},
                {"name": "index", "type": "method", "return": "uint64_t", "doc": "Get population index"}
            ]
        }
    }


def build_neural_units():
    """Build documentation for neural unit classes."""
    return {
        "Neuron": {
            "description": "Neural unit with membrane potential, firing dynamics, and synaptic connections",
            "parameters": [
                {"name": "id", "type": "NeuronId", "doc": "Neuron identifier", "optional": False}
            ],
            "methods": [
                {"name": "getId", "type": "method", "return": "NeuronId", "doc": "Get neuron identifier"},
                {"name": "getType", "type": "method", "return": "NeuronType", "doc": "Get neuron type"},
                {"name": "setType", "params": [{"name": "type", "type": "NeuronType"}], "return": "None", "doc": "Set neuron type"},
                {"name": "getMembranePotential", "type": "method", "return": "float", "doc": "Get current membrane potential (mV)"},
                {"name": "setMembranePotential", "params": [{"name": "potential", "type": "float"}], "return": "None", "doc": "Set membrane potential"},
                {"name": "addToMembranePotential", "params": [{"name": "delta", "type": "float"}], "return": "None", "doc": "Add delta to membrane potential"},
                {"name": "getThreshold", "type": "method", "return": "float", "doc": "Get firing threshold"},
                {"name": "setThreshold", "params": [{"name": "threshold", "type": "float"}], "return": "None", "doc": "Set firing threshold"},
                {"name": "isFiring", "type": "method", "return": "bool", "doc": "Check if neuron is currently firing"},
                {"name": "isRefractory", "type": "method", "return": "bool", "doc": "Check if neuron is in refractory period"},
                {"name": "setFiringState", "params": [{"name": "state", "type": "FiringState"}], "return": "None", "doc": "Set neuron firing state"},
                {"name": "setRefractoryPeriod", "params": [{"name": "steps", "type": "uint32_t"}], "return": "None", "doc": "Set refractory period in steps"},
                {"name": "decrementRefractory", "type": "method", "return": "None", "doc": "Decrement refractory period counter"},
                {"name": "getFiringRate", "type": "method", "return": "float", "doc": "Get current firing rate (Hz)"},
                {"name": "setFiringRate", "params": [{"name": "rate", "type": "float"}], "return": "None", "doc": "Set firing rate"},
                {"name": "stepLIF", "params": [{"name": "currentTime", "type": "float"}, {"name": "dt", "type": "float"}], "return": "bool", "doc": "Perform LIF integration (returns True if fired)"},
                {"name": "step", "params": [{"name": "currentTime", "type": "float"}], "return": "None", "doc": "Perform one simulation step (uses default 1ms timestep)"}
            ]
        },
        "Synapse": {
            "description": "Connection between neurons with weight, delay, and synaptic properties",
            "parameters": [
                {"name": "id", "type": "SynapseId", "doc": "Synapse identifier", "optional": False},
                {"name": "source", "type": "NeuronId", "doc": "Source neuron ID", "optional": False},
                {"name": "destination", "type": "NeuronId", "doc": "Destination neuron ID", "optional": False}
            ],
            "methods": [
                {"name": "getId", "type": "method", "return": "SynapseId", "doc": "Get synapse identifier"},
                {"name": "getSourceNeuron", "type": "method", "return": "NeuronId", "doc": "Get source neuron ID"},
                {"name": "getDestinationNeuron", "type": "method", "return": "NeuronId", "doc": "Get destination neuron ID"},
                {"name": "getWeight", "type": "method", "return": "float", "doc": "Get current synaptic weight (-1.0 to 1.0)"},
                {"name": "setWeight", "params": [{"name": "weight", "type": "float"}], "return": "None", "doc": "Set synaptic weight"},
                {"name": "addToWeight", "params": [{"name": "delta", "type": "float"}], "return": "None", "doc": "Add delta to weight"},
                {"name": "getDelay", "type": "method", "return": "uint32_t", "doc": "Get synaptic delay in simulation steps"},
                {"name": "setDelay", "params": [{"name": "delay", "type": "uint32_t"}], "return": "None", "doc": "Set synaptic delay"},
                {"name": "getType", "type": "method", "return": "SynapseType", "doc": "Get synapse type"},
                {"name": "setType", "params": [{"name": "type", "type": "SynapseType"}], "return": "None", "doc": "Set synapse type"},
                {"name": "isExcitatory", "type": "method", "return": "bool", "doc": "Check if synapse is excitatory"},
                {"name": "isInhibitory", "type": "method", "return": "bool", "doc": "Check if synapse is inhibitory"}
            ]
        }
    }


def build_brain_components():
    """Build documentation for brain component classes."""
    return {
        "Region": {
            "description": "Brain region containing multiple populations and local connectivity",
            "parameters": [
                {"name": "id", "type": "RegionId", "doc": "Region identifier", "optional": False},
                {"name": "name", "type": "str", "doc": "Region name", "optional": True}
            ],
            "methods": [
                {"name": "getId", "type": "method", "return": "RegionId", "doc": "Get region identifier"},
                {"name": "getName", "type": "method", "return": "str", "doc": "Get region name"},
                {"name": "setName", "params": [{"name": "name", "type": "str"}], "return": "None", "doc": "Set region name"},
                {"name": "addPopulation", "params": [{"name": "size", "type": "int"}, {"name": "type", "type": "NeuronType", "optional": True}], "return": "PopulationId", "doc": "Add population with neurons"},
                {"name": "getPopulation", "params": [{"name": "id", "type": "PopulationId"}], "return": "Population", "doc": "Get population by ID"},
                {"name": "getPopulationCount", "type": "method", "return": "int", "doc": "Get number of populations"},
                {"name": "addSynapse", "params": [{"name": "source", "type": "NeuronId"}, {"name": "destination", "type": "NeuronId"}, {"name": "weight", "type": "float", "optional": True}, {"name": "delay", "type": "int", "optional": True}], "return": "SynapseId", "doc": "Add synapse between neurons"}
            ]
        },
        "Population": {
            "description": "Group of neurons with shared properties",
            "parameters": [
                {"name": "id", "type": "PopulationId", "doc": "Population identifier", "optional": False},
                {"name": "size", "type": "int", "doc": "Number of neurons in population", "optional": False}
            ],
            "methods": [
                {"name": "getId", "type": "method", "return": "PopulationId", "doc": "Get population identifier"},
                {"name": "getSize", "type": "method", "return": "int", "doc": "Get population size"},
                {"name": "isEmpty", "type": "method", "return": "bool", "doc": "Check if population is empty"},
                {"name": "getNeuron", "params": [{"name": "index", "type": "int"}], "return": "Neuron", "doc": "Get neuron by index"},
                {"name": "addNeuron", "params": [{"name": "neuron", "type": "Neuron"}], "return": "None", "doc": "Add neuron to population"},
                {"name": "step", "params": [{"name": "currentTime", "type": "float"}], "return": "None", "doc": "Step all neurons in population"}
            ]
        },
        "Brain": {
            "description": "Central neural simulation brain class",
            "parameters": [
                {"name": "config", "type": "Config", "doc": "Brain configuration", "optional": False}
            ],
            "methods": [
                {"name": "initialize", "type": "method", "return": "bool", "doc": "Initialize the brain with configuration"},
                {"name": "step", "params": [{"name": "currentStep", "type": "float"}], "return": "None", "doc": "Perform a simulation step"},
                {"name": "addRegion", "params": [{"name": "name", "type": "str", "optional": True}], "return": "RegionId", "doc": "Add a new neural region"},
                {"name": "getRegion", "params": [{"name": "id", "type": "RegionId"}], "return": "Region", "doc": "Get a region by ID"},
                {"name": "produceAction", "type": "method", "return": "Action", "doc": "Produce motor action based on neural activity"},
                {"name": "save", "params": [{"name": "filepath", "type": "str"}], "return": "bool", "doc": "Save brain state to file"},
                {"name": "load", "params": [{"name": "filepath", "type": "str"}], "return": "bool", "doc": "Load brain state from file"}
            ]
        }
    }


def build_memory_systems():
    """Build documentation for memory system classes."""
    return {
        "WorkingMemory": {
            "description": "Neural working memory using persistent activity and recurrent connections",
            "methods": [
                {"name": "initialize", "params": [{"name": "brain", "type": "Brain"}], "return": "None", "doc": "Initialize with brain reference"},
                {"name": "store", "params": [{"name": "pattern", "type": "List[float]"}, {"name": "strength", "type": "float", "optional": True}], "return": "None", "doc": "Store information in working memory"},
                {"name": "retrieve", "type": "method", "return": "List[float]", "doc": "Retrieve current working memory content as activity levels"},
                {"name": "clear", "type": "method", "return": "None", "doc": "Clear working memory"},
                {"name": "getActiveTraces", "type": "method", "return": "int", "doc": "Get number of active memory traces"},
                {"name": "runCompetition", "type": "method", "return": "None", "doc": "Run competition between memory traces"}
            ]
        },
        "EpisodicMemory": {
            "description": "Neural episodic memory storing experiences as neural patterns",
            "methods": [
                {"name": "initialize", "params": [{"name": "brain", "type": "Brain"}], "return": "None", "doc": "Initialize with brain reference"},
                {"name": "storeEpisode", "params": [{"name": "episode", "type": "EpisodicMemoryItem"}], "return": "None", "doc": "Store a new experience"},
                {"name": "retrieveSimilar", "params": [{"name": "sensoryPattern", "type": "List[float]"}, {"name": "maxResults", "type": "int", "optional": True}], "return": "List[EpisodicMemoryItem]", "doc": "Retrieve episodes similar to query pattern"},
                {"name": "replayEpisode", "params": [{"name": "episode", "type": "EpisodicMemoryItem"}], "return": "None", "doc": "Replay an episode to reactivate neural patterns"},
                {"name": "clear", "type": "method", "return": "None", "doc": "Clear all episodes"}
            ]
        },
        "AssociativeMemory": {
            "description": "Neural associative memory creating relationships between experiences",
            "methods": [
                {"name": "initialize", "params": [{"name": "brain", "type": "Brain"}], "return": "None", "doc": "Initialize with brain reference"},
                {"name": "associate", "params": [{"name": "patternA", "type": "List[float]"}, {"name": "patternB", "type": "List[float]"}, {"name": "strength", "type": "float", "optional": True}], "return": "None", "doc": "Create association between two patterns"},
                {"name": "retrieve", "params": [{"name": "queryPattern", "type": "List[float]"}, {"name": "maxResults", "type": "int", "optional": True}], "return": "List[List[float]]", "doc": "Retrieve patterns associated with query"},
                {"name": "clear", "type": "method", "return": "None", "doc": "Clear all associations"}
            ]
        }
    }


def build_world_systems():
    """Build documentation for world system classes."""
    return {
        "SimpleWorld": {
            "description": "Simple 2D world for NLM simulation",
            "methods": [
                {"name": "configure", "params": [{"name": "width", "type": "int"}, {"name": "height", "type": "int"}, {"name": "visionWidth", "type": "int"}, {"name": "visionHeight", "type": "int"}], "return": "None", "doc": "Configure world dimensions"},
                {"name": "update", "params": [{"name": "timestep", "type": "float"}], "return": "None", "doc": "Update world simulation"},
                {"name": "getSensoryPercept", "type": "method", "return": "SensoryPercept", "doc": "Get sensory percept"},
                {"name": "getAgentBody", "type": "method", "return": "AgentBody", "doc": "Get agent body"},
                {"name": "applyMotorCommand", "params": [{"name": "cmd", "type": "MotorCommand"}, {"name": "currentTime", "type": "float"}], "return": "None", "doc": "Apply motor command"}
            ]
        }
    }


def build_sensory_classes():
    """Build documentation for sensory input classes."""
    return {
        "Vision": {
            "description": "Vision sensory input with image data",
            "parameters": [
                {"name": "width", "type": "int", "doc": "Image width in pixels", "optional": False},
                {"name": "height", "type": "int", "doc": "Image height in pixels", "optional": False},
                {"name": "channels", "type": "int", "doc": "Color channels (typically 3 for RGB)", "optional": True}
            ],
            "methods": [
                {"name": "setData", "params": [{"name": "data", "type": "List[float]"}], "return": "None", "doc": "Set vision data. Validates data dimensions"},
                {"name": "getWidth", "type": "method", "return": "int", "doc": "Get vision width"},
                {"name": "getHeight", "type": "method", "return": "int", "doc": "Get vision height"},
                {"name": "getChannels", "type": "method", "return": "int", "doc": "Get number of channels"},
                {"name": "validateImage", "type": "method", "return": "bool", "doc": "Validate vision image"}
            ]
        },
        "Audio": {
            "description": "Audio sensory input with sound samples",
            "parameters": [
                {"name": "sampleRate", "type": "int", "doc": "Samples per second", "optional": False},
                {"name": "numSamples", "type": "int", "doc": "Number of audio samples", "optional": False}
            ],
            "methods": [
                {"name": "setData", "params": [{"name": "data", "type": "List[float]"}], "return": "None", "doc": "Set audio data. Validates data size"},
                {"name": "setSampleRate", "params": [{"name": "sampleRate", "type": "int"}], "return": "None", "doc": "Set sample rate"},
                {"name": "getSampleRate", "type": "method", "return": "int", "doc": "Get sample rate"},
                {"name": "getNumSamples", "type": "method", "return": "int", "doc": "Get number of samples"},
                {"name": "validateAudio", "type": "method", "return": "bool", "doc": "Validate audio data"}
            ]
        }
    }


def build_motor_classes():
    """Build documentation for motor output classes."""
    return {
        "Action": {
            "description": "Action representation for motor output",
            "parameters": [
                {"name": "type", "type": "ActionType", "doc": "Action type", "optional": False},
                {"name": "parameters", "type": "List[float]", "doc": "Action parameters", "optional": True}
            ],
            "methods": [
                {"name": "getType", "type": "method", "return": "ActionType", "doc": "Get action type"},
                {"name": "setType", "params": [{"name": "type", "type": "ActionType"}], "return": "None", "doc": "Set action type"},
                {"name": "getParameters", "type": "method", "return": "List[float]", "doc": "Get action parameters"},
                {"name": "setParameters", "params": [{"name": "params", "type": "List[float]"}], "return": "None", "doc": "Set action parameters"},
                {"name": "getName", "type": "method", "return": "str", "doc": "Get action name"},
                {"name": "clone", "type": "method", "return": "Action", "doc": "Create a copy of this action"}
            ]
        }
    }


def build_convenience_functions():
    """Build documentation for convenience functions."""
    return {
        "createDefaultConfig": {
            "description": "Create a default configuration",
            "parameters": [],
            "return": "Config",
            "doc": "Create and return a default configuration object"
        },
        "createBrain": {
            "description": "Create a new brain with configuration",
            "parameters": [
                {"name": "config", "type": "Config", "doc": "Brain configuration"}
            ],
            "return": "Brain",
            "doc": "Create and return a new brain object"
        },
        "createSimpleWorld": {
            "description": "Create a new simple world",
            "parameters": [],
            "return": "SimpleWorld",
            "doc": "Create and return a new simple world object"
        },
        "createAgentBrain": {
            "description": "Create a new agent brain interface",
            "parameters": [
                {"name": "brain", "type": "Brain", "doc": "Underlying brain object"}
            ],
            "return": "AgentBrain",
            "doc": "Create and return a new agent brain interface"
        }
    }


def build_examples():
    """Build comprehensive examples section."""
    return [
        {
            "title": "Basic Brain Simulation",
            "description": "Create a simple brain and run a basic simulation",
            "code": """
import nlm

# Create a default configuration
config = nlm.createDefaultConfig()

# Create a brain with the configuration
brain = nlm.createBrain(config)

# Create a simple world
world = nlm.createSimpleWorld()

# Initialize the brain with the world
agent_brain = nlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Run a simulation for 100 steps
for step in range(100):
    # Get sensory input from world
    percept = world.getSensoryPercept()
    
    # Process sensory input through agent brain
    agent_brain.processSensoryInput(percept)
    
    # Get motor command
    action = agent_brain.decodeMotorCommand()
    
    # Apply action in the world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Update world
    world.update(0.001)  # 1ms timestep

print("Simulation completed successfully!")
""",
            "tags": ["basic", "simulation", "step-by-step"]
        },
        {
            "title": "Working Memory Example",
            "description": "Demonstrate working memory functionality",
            "code": """
import nlm

# Create a brain and initialize working memory
brain = nlm.createBrain(nlm.createDefaultConfig())
working_memory = brain.getWorkingMemory()
working_memory.initialize(brain)

# Store a pattern in working memory
pattern = [0.1, 0.5, 0.3, 0.8, 0.2]
working_memory.store(pattern, strength=1.0)

# Retrieve the pattern
retrieved = working_memory.retrieve()
print(f"Stored pattern: {pattern}")
print(f"Retrieved pattern: {retrieved}")

# Store specific neuron activation
neuron_id = nlm.INVALID_NEURON_ID  # In practice, use a real neuron ID
working_memory.storeToNeuron(neuron_id, activation=0.7)

# Check if neuron is in working memory
if working_memory.contains(neuron_id):
    print(f"Neuron {neuron_id} is in working memory with activation: {working_memory.getNeuronActivation(neuron_id)}")
""",
            "tags": ["memory", "working-memory", "storage"]
        },
        {
            "title": "Neuron Network Simulation",
            "description": "Create and simulate a network of neurons",
            "code": """
import nlm

# Create brain with configuration
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)

# Add a region to the brain
region_id = brain.addRegion("TestRegion")
region = brain.getRegion(region_id)

# Create a population of neurons
population_id = region.addPopulation(size=10, type=nlm.NeuronType.Excitatory)
population = region.getPopulation(population_id)

# Initialize neurons with random parameters
import random
random_rng = random.Random()
for i in range(population.getSize()):
    neuron = population.getNeuron(i)
    if neuron:
        neuron.initializeRandom(random_rng)

# Run simulation
for step in range(50):
    # Step all neurons
    region.step(step)
    
    # Print firing statistics every 10 steps
    if step % 10 == 0:
        firing_count = region.getFiringNeuronCount()
        avg_firing_rate = region.getAverageFiringRate()
        print(f"Step {step}: {firing_count} firing neurons, avg rate {avg_firing_rate:.2f} Hz")
""",
            "tags": ["neurons", "network", "population"]
        },
        {
            "title": "Memory Replay Example",
            "description": "Demonstrate episodic memory replay functionality",
            "code": """
import nlm
import random

# Create brain and episodic memory
brain = nlm.createBrain(nlm.createDefaultConfig())
episodic_memory = brain.getEpisodicMemory()
episodic_memory.initialize(brain)

# Create sample episodic memory items
sample_episodes = []
for i in range(3):
    episode = nlm.EpisodicMemoryItem()
    episode.timestamp = i * 100  # 100 time units apart
    episode.sensoryState = [random.random() for _ in range(5)]
    episode.reward = random.random() * 10
    episode.action = nlm.ActionType.Look
    sample_episodes.append(episode)

# Store episodes in episodic memory
for episode in sample_episodes:
    episodic_memory.storeEpisode(episode)

# Retrieve similar episodes to a query pattern
query_pattern = [0.5, 0.3, 0.7, 0.2, 0.6]
retrieved_episodes = episodic_memory.retrieveSimilar(query_pattern, maxResults=2)

print(f"Retrieved {len(retrieved_episodes)} similar episodes:")
for i, episode in enumerate(retrieved_episodes):
    print(f"  Episode {i}: reward={episode.reward}, action={episode.action}")

# Replay an episode (e.g., the first one)
if retrieved_episodes:
    episodic_memory.replayEpisode(retrieved_episodes[0])
    print("Episode replay completed!")
""",
            "tags": ["memory", "episodic", "replay"]
        }
    ]


def build_best_practices():
    """Build best practices section."""
    return [
        {
            "title": "Initialize Components Properly",
            "content": "Always initialize all brain components before use. Call initialize() on memory systems, agent brain, and other subsystems after creation but before simulation."
        },
        {
            "title": "Validate Sensory Input",
            "content": "Always validate sensory input data before processing. Use validateData() or validateImage() methods to ensure data integrity and prevent simulation errors."
        },
        {
            "title": "Use Proper Timesteps",
            "content": "Choose appropriate timesteps for your simulation. Use consistent timesteps across all components (typically 0.001 for 1ms resolution). Smaller timesteps may be needed for stability."
        },
        {
            "title": "Manage Memory Usage",
            "content": "Monitor memory usage, especially for episodic memory. Use consolidate() method to remove old episodes when memory capacity is exceeded."
        },
        {
            "title": "Error Handling",
            "content": "Always handle exceptions in production code. Catch RuntimeError for invalid operations and provide meaningful error messages to users."
        },
        {
            "title": "Simulation Loop Structure",
            "content": "Follow the standard simulation loop: 1) Get sensory input, 2) Process through agent brain, 3) Get action, 4) Apply action in world, 5) Update world. Always update time after each step."
        }
    ]


def build_configuration_options():
    """Build configuration options section."""
    return {
        "brain_settings": [
            {"key": "neural.regionCount", "type": "int", "default": "10", "description": "Number of brain regions"},
            {"key": "neural.populationSize", "type": "int", "default": "100", "description": "Default population size"},
            {"key": "neural.simulationTimestep", "type": "float", "default": "0.001", "description": "Default simulation timestep in seconds"},
            {"key": "memory.workingMemoryCapacity", "type": "int", "default": "1000", "description": "Working memory capacity"},
            {"key": "memory.episodicMemoryMaxEpisodes", "type": "int", "default": "10000", "description": "Maximum episodic memory episodes"},
            {"key": "neuromodulation.dopamineLevel", "type": "float", "default": "0.5", "description": "Initial dopamine level"},
            {"key": "development.initialStage", "type": "string", "default": "CriticalPeriod", "description": "Initial developmental stage"}
        ],
        "sensory_settings": [
            {"key": "sensory.vision.defaultWidth", "type": "int", "default": "84", "description": "Default vision width in pixels"},
            {"key": "sensory.vision.defaultHeight", "type": "int", "default": "84", "description": "Default vision height in pixels"},
            {"key": "sensory.audio.defaultSampleRate", "type": "int", "default": "44100", "description": "Default audio sample rate"},
            {"key": "sensory.audio.defaultDuration", "type": "float", "default": "1.0", "description": "Default audio duration in seconds"}
        ]
    }


def build_gotchas():
    """Build common gotchas section."""
    return [
        {
            "title": "Memory Growth",
            "content": "Episodic memory grows indefinitely. Implement consolidate() calls regularly to prevent memory exhaustion. Consider setting relevance thresholds based on experience importance."
        },
        {
            "title": "Neuron Initialization",
            "content": "All neurons must be properly initialized before use. Uninitialized neurons may have default values that don't represent biological realism. Always call initializeRandom() after creation."
        },
        {
            "title": "Sensory Data Validation",
            "content": "Invalid sensory data (NaN, Inf, wrong dimensions) can cause simulation crashes. Always validate data before processing, especially when working with external data sources."
        },
        {
            "title": "Thread Safety",
            "content": "NLM is not thread-safe by default. For multi-threaded applications, implement proper locking mechanisms around brain access and modification."
        },
        {
            "title": "Numerical Stability",
            "content": "LIF neuron dynamics can become unstable with extreme parameter values. Monitor membrane potential values and clamp them to reasonable biological ranges."
        },
        {
            "title": "Performance",
            "content": "NLM is computationally intensive. Profile your application regularly and consider using numpy arrays for large-scale sensory data processing."
        }
    ]


def build_quick_start():
    """Build quick start guide."""
    return [
        {
            "title": "Install NLM Python Bindings",
            "steps": [
                "pip install nlm",
                "Check installation: python -c \"import nlm; print('NLM version:', nlm.__version__)\""
            ]
        },
        {
            "title": "Basic Simulation",
            "steps": [
                "Create configuration: config = nlm.createDefaultConfig()",
                "Create brain: brain = nlm.createBrain(config)",
                "Create world: world = nlm.createSimpleWorld()",
                "Initialize agent: agent = nlm.createAgentBrain(brain); agent.initialize(world)",
                "Run simulation loop with sensory input, processing, and action application"
            ]
        },
        {
            "title": "Working with Neurons",
            "steps": [
                "Create region: region_id = brain.addRegion('MyRegion')",
                "Add population: pop_id = region.addPopulation(10, nlm.NeuronType.Excitatory)",
                "Access neurons: population.getNeuron(index)",
                "Simulate: neuron.step(current_time)"
            ]
        },
        {
            "title": "Memory Integration",
            "steps": [
                "Get memory systems: brain.getWorkingMemory(), brain.getEpisodicMemory()",
                "Initialize memory systems with: memory.initialize(brain)",
                "Store data: working_memory.store(pattern)",
                "Retrieve data: retrieved = working_memory.retrieve()"
            ]
        }
    ]


def generate_markdown_doc(doc_template):
    """
    Generate markdown documentation from template data.
    
    Args:
        doc_template: Documentation template data
        
    Returns:
        str: Complete markdown documentation
    """
    markdown = f"""# {doc_template["title"]}

**Version {doc_template["version"]}** | Generated: {doc_template["generated"]}

{doc_template["description"]}

## Quick Start

"""
    
    # Add quick start sections
    for section in doc_template["quick_start"]:
        markdown += f"### {section["title"]}\n\n"
        markdown += "\n".join([f"1. {step}" for step in section["steps"]]) + "\n\n"
    
    # Add API reference sections
    markdown += "## API Reference\n\n"
    
    for category, classes in doc_template["api_reference"].items():
        markdown += f"### {format_category_name(category)}\n\n"
        
        for class_name, class_info in classes.items():
            markdown += f"#### {class_name}\n\n"
            markdown += f"{class_info.get('description', '')}\n\n"
            
            if 'parameters' in class_info:
                markdown += "**Parameters:**\n\n"
                for param in class_info["parameters"]:
                    optional_text = " (optional)" if param.get("optional", False) else ""
                    markdown += f"- `{param["name"]}` ({param["type"]}){optional_text}: {param["doc"]}\n"
                markdown += "\n"
            
            if "methods" in class_info:
                markdown += "**Methods:**\n\n"
                for method in class_info["methods"]:
                    params_str = ", ".join([f"{p['name']}: {p['type']}" for p in method.get("params", [])])
                    markdown += f"- `{method["name"]}({params_str})`"
                    if "return" in method:
                        markdown += f" -> {method["return"]}"
                    markdown += f": {method["doc"]}\n"
                markdown += "\n"
    
    # Add examples section
    markdown += "## Examples\n\n"
    for example in doc_template["examples"]:
        markdown += f"### {example["title"]}\n\n"
        markdown += f"{example["description"]}\n\n"
        markdown += "```python\n"
        markdown += example["code"]
        markdown += "```\n\n"
        
        if "tags" in example:
            tags_str = ", ".join([f"`{tag}`" for tag in example["tags"]])
            markdown += f"**Tags:** {tags_str}\n\n"
    
    # Add best practices section
    markdown += "## Best Practices\n\n"
    for practice in doc_template["best_practices"]:
        markdown += f"### {practice["title"]}\n\n"
        markdown += f"{practice["content"]}\n\n"
    
    # Add configuration section
    markdown += "## Configuration Options\n\n"
    
    for category, settings in doc_template["configuration"].items():
        markdown += f"### {format_category_name(category)}\n\n"
        markdown += "| Key | Type | Default | Description |\n"
        markdown += "|-----|------|---------|-------------|\n"
        for setting in settings:
            markdown += f"| `{setting["key"]}` | `{setting["type"]}` | `{setting["default"]}` | {setting["description"]} |\n"
        markdown += "\n"
    
    # Add gotchas section
    markdown += "## Common Gotchas\n\n"
    for gotcha in doc_template["gotchas"]:
        markdown += f"### {gotcha["title"]}\n\n"
        markdown += f"{gotcha["content"]}\n\n"
    
    # Add footer
    markdown += "---\n\n"
    markdown += f"Documentation generated by NLM Python API Documentation Generator (v{doc_template["version"]})\n"
    markdown += f"Last updated: {doc_template["generated"]}\n"
    
    return markdown


def format_category_name(category):
    """
    Format category name for display.
    
    Args:
        category: Category key
        
    Returns:
        str: Formatted category name
    """
    category_names = {
        "core_identifiers": "Core Identifiers",
        "neural_units": "Neural Units",
        "brain_components": "Brain Components",
        "memory_systems": "Memory Systems",
        "world_systems": "World Systems",
        "sensory_input": "Sensory Input",
        "motor_systems": "Motor Systems",
        "convenience_functions": "Convenience Functions"
    }
    
    return category_names.get(category, category.replace("_", " ").title())


if __name__ == "__main__":
    print("🔄 Generating NLM Python API documentation...")
    generate_api_documentation()
    print("✅ Documentation generation complete!")
"