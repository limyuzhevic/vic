# Tutorial 1: Introduction to NLM

## Overview

This tutorial provides a comprehensive introduction to NLM (Neural Learning Machine). It covers the fundamental concepts, architecture, and basic usage of NLM for neural simulation and computational neuroscience research.

## What is NLM?

NLM is a brain-inspired spiking neural network simulator that enables researchers to create, simulate, and study virtual brains. It combines principles from neuroscience, artificial intelligence, and computational modeling to create realistic simulations of neural systems.

### Key Features:

- **Spiking Neural Networks:** Implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation
- **Multiple Plasticity Mechanisms:** Supports STDP, Hebbian learning, and structural plasticity
- **Memory Systems:** Includes working memory, episodic memory, and associative memory
- **Neuromodulation:** Models dopamine, serotonin, and other neuromodulators
- **Environment Interaction:** Simulates interaction with external environments
- **Learning Algorithms:** Supports various learning algorithms and optimization techniques

### Biological Plausibility:

NLM is designed to be biologically plausible, incorporating:

- **Realistic neuron models:** LIF neurons with resting potential, threshold, and refractory periods
- **Synaptic dynamics:** Realistic synaptic transmission with delays and plasticity
- **Network connectivity:** Structured brain regions with functional connectivity
- **Temporal dynamics:** Spike-timing dependent processes

## NLM Architecture

```
NLM System
├── Brain Network
│   ├── Brain Regions
│   │   ├── Visual Cortex
│   │   ├── Motor Cortex
│   │   ├── Hippocampus
│   │   └── Other Regions
│   ├── Neurons
│   │   ├── LIF Neurons
│   │   ├── Synaptic Connections
│   │   └── Plasticity Mechanisms
│   └── Simulation Engine
├── Memory System
│   ├── Working Memory
│   ├── Episodic Memory
│   └── Associative Memory
├── Neuromodulation System
│   ├── Dopamine System
│   ├── Serotonin System
│   └── Other Neuromodulators
├── Environment System
│   ├── World Simulator
│   ├── Sensor Model
│   └── Actuator Model
└── Agent System
    ├── Agent Brain
    ├── Reward Engine
    └── Decision Making
```

## Getting Started with NLM

### Step 1: Installation and Setup

```bash
# Install NLM
pip install nlm

# Or install from source
pip install -e ./nlm/python/

# Verify installation
python -c "import nlm; print('NLM version:', nlm.__version__)"
```

### Step 2: Create a Simple Brain

```python
import nlm

# Create NLM system
nlm_system = nlm.NLM()

# Create a simple brain configuration
brain_config = nlm.nlm_config()
brain_config.set("brain.regions", 3)
brain_config.set("brain.neurons_per_region", 100)
brain_config.set("brain.neuron_type", "lif")

# Create brain
brain = nlm.BrainNetwork(brain_config)
brain.initialize()

print(f"Brain created with {brain.get_region_count()} regions")
print(f"Total neurons: {brain.get_total_neuron_count()}")
```

### Step 3: Run a Basic Simulation

```python
import nlm
import time

# Create NLM system
nlm_system = nlm.NLM()

# Create brain
brain = nlm_system.create_agent({
    "regions": 2,
    "neurons_per_region": 50
})

# Run simulation
print("Running simulation...")
start_time = time.time()

results = nlm_system.run_simulation(steps=100)

end_time = time.time()
elapsed_time = end_time - start_time

print(f"Simulation completed in {elapsed_time:.2f} seconds")
print(f"Steps per second: {100 / elapsed_time:.1f}")
print(f"Results: {results}")
```

### Step 4: Explore Brain Activity

```python
import nlm
import matplotlib.pyplot as plt

# Create NLM system
nlm_system = nlm.NLM()

# Create brain with visualization
brain = nlm_system.create_agent({
    "regions": 3,
    "neurons_per_region": 100
})

# Get brain statistics
stats = brain.get_statistics()
print("Brain Statistics:")
print(f"  Total regions: {stats['total_regions']}")
print(f"  Total neurons: {stats['total_neurons']}")
print(f"  Average neurons per region: {stats['avg_neurons_per_region']}")
print(f"  Total synapses: {stats['total_synapses']}")
print(f"  Connection density: {stats['connection_density']:.3f}")

# Visualize brain regions
region_names = ["Visual Cortex", "Motor Cortex", "Hippocampus"]
neuron_counts = [300, 200, 150]

plt.figure(figsize=(10, 6))
plt.bar(region_names, neuron_counts, color=['#FF6B6B', '#4ECDC4', '#45B7D1'])
plt.title("Brain Regions and Neuron Counts")
plt.ylabel("Number of Neurons")
plt.tight_layout()
plt.savefig("brain_regions.png", dpi=150, bbox_inches='tight')
print("Brain region visualization saved to 'brain_regions.png'")
```

## Understanding NLM Concepts

### 1. Neurons and Synapses

**Neurons:** Basic computational units that process and transmit information

- **LIF Model:** Leaky Integrate-and-Fire neuron model
- **Threshold:** Membrane potential at which neuron fires
- **Refractory Period:** Time after firing when neuron cannot fire again
- **Resting Potential:** Default membrane potential

**Synapses:** Connections between neurons that transmit signals

- **Excitatory:** Increases firing probability
- **Inhibitory:** Decreases firing probability
- **Plastic:** Can change strength based on activity

### 2. Brain Regions

NLM organizes neurons into functional brain regions:

- **Sensory Regions:** Process external information
- **Motor Regions:** Generate actions
- **Association Regions:** Integrate information from multiple sources
- **Memory Regions:** Store and retrieve information

### 3. Learning and Plasticity

**Learning Rules:**

- **STDP:** Spike-Timing-Dependent Plasticity
- **Hebbian Learning:** "Neurons that fire together, wire together"
- **Homeostatic Plasticity:** Maintain stability in neural activity

**Plasticity Types:**

- **Synaptic Plasticity:** Changes in synaptic strength
- **Structural Plasticity:** Changes in neural connectivity
- **Neuronal Plasticity:** Changes in neuron properties

### 4. Memory Systems

**Working Memory:** Temporary storage of information for immediate use

- **Limited capacity:** Typically 4-7 items
- **Rapid decay:** Information fades quickly without rehearsal
- **Active maintenance:** Continuous updating of information

**Episodic Memory:** Storage of specific experiences and events

- **Long-term storage:** Can last from minutes to years
- **Consolidation:** Process of stabilizing memories
- **Replay:** Reactivation of memories for consolidation

### 5. Neuromodulation

**Key Neuromodulators:**

- **Dopamine:** Reward prediction and learning
- **Serotonin:** Mood regulation and behavioral control
- **Acetylcholine:** Attention and arousal
- **Noradrenaline:** Alertness and stress response

### 6. Environment Interaction

**Sensory Processing:**

- **Perception:** Extracting features from environment
- **Integration:** Combining sensory information
- **Action selection:** Choosing appropriate responses

**Motor Control:**

- **Movement generation:** Producing actions
- **Feedback control:** Adjusting actions based on outcomes
- **Learning from feedback:** Improving performance through experience

## Practical Applications

### 1. Cognitive Modeling

Model cognitive processes like:

- **Perception:** How organisms perceive and interpret sensory information
- **Memory:** How information is stored and retrieved
- **Decision Making:** How organisms choose between alternatives
- **Problem Solving:** How organisms find solutions to problems

### 2. Neuroscience Research

Study neural phenomena such as:

- **Neural coding:** How information is represented in neural activity
- **Network dynamics:** How neural networks produce complex behaviors
- **Plasticity mechanisms:** How neural connections change over time
- **Brain development:** How brains develop and mature

### 3. Artificial Intelligence

Develop intelligent systems with:

- **Learning algorithms:** Methods for adaptive behavior
- **Decision making:** Strategies for choosing actions
- **Problem solving:** Approaches to finding solutions
- **Autonomous behavior:** Systems that can act independently

## Next Steps

### Basic Learning Path:

1. **Complete this tutorial:** Understand NLM fundamentals
2. **Try basic examples:** Practice with simple simulations
3. **Explore brain configurations:** Experiment with different brain setups
4. **Study memory systems:** Understand how NLM handles memory
5. **Learn about learning:** Explore different plasticity mechanisms

### Intermediate Learning Path:

1. **Master advanced brain configurations:** Create complex brain structures
2. **Study neuromodulation:** Understand how neuromodulators affect behavior
3. **Implement learning algorithms:** Apply learning to specific tasks
4. **Create environments:** Design custom environments for testing
5. **Analyze results:** Interpret simulation outcomes

### Advanced Learning Path:

1. **Research applications:** Apply NLM to research problems
2. **Extension development:** Create custom NLM extensions
3. **Performance optimization:** Optimize simulations for efficiency
4. **Machine learning integration:** Combine NLM with other AI techniques
5. **Publication and dissemination:** Share research findings

## Resources

### Documentation:

- **NLM Documentation:** https://nlm.readthedocs.io
- **API Reference:** https://nlm.readthedocs.io/en/latest/api/
- **Tutorials:** https://nlm.readthedocs.io/en/latest/tutorials/
- **Examples:** https://github.com/nlm-project/nlm/tree/main/examples

### Community:

- **GitHub Repository:** https://github.com/nlm-project/nlm
- **Discussion Forum:** https://github.com/nlm-project/nlm/discussions
- **Slack Channel:** Join the NLM community
- **Stack Overflow:** Programming questions

### Support:

- **Bug Reports:** File issues on GitHub
- **Feature Requests:** Submit feature requests
- **Technical Support:** Get help from the community
- **Professional Services:** Commercial support and consulting

## Conclusion

This tutorial provides a comprehensive introduction to NLM. By following this guide, you will:

1. **Understand NLM concepts:** Learn the fundamental principles and architecture
2. **Get started quickly:** Create and run your first simulation
3. **Explore key features:** Discover the capabilities of NLM
4. **Build foundations:** Prepare for more advanced learning

NLM offers a powerful framework for neural simulation and computational neuroscience. Whether you're a student, researcher, or developer, NLM provides the tools and flexibility needed to explore complex neural systems and intelligent behavior.

By mastering the concepts and skills introduced in this tutorial, you'll be well-equipped to:

- Create sophisticated neural simulations
- Investigate learning and memory processes
- Develop intelligent agents
- Advance your research in computational neuroscience
- Contribute to the NLM community

The journey with NLM begins with understanding the basics and continues with exploring advanced topics, developing new applications, and contributing to the growing field of neural machine learning.

Happy modeling!