# NLM Tutorials

## Overview

The NLM tutorial series provides **step-by-step learning materials** for different skill levels and use cases. These tutorials are designed to guide you from basic concepts to advanced applications, with hands-on examples and comprehensive explanations.

> **Key Feature:** Tutorials are **progressively structured** - each tutorial builds on the previous one, allowing you to develop your skills systematically.

## Table of Contents

- [Tutorial 1: Basic Neural Computation](#tutorial-1-basic-neural-computation)
- [Tutorial 2: Python Integration](#tutorial-2-python-integration)
- [Tutorial 3: Advanced Configuration](#tutorial-3-advanced-configuration)
- [Tutorial 4: Phase 6 Integration](#tutorial-4-phase-6-integration)
- [Tutorial 5: Performance Optimization](#tutorial-5-performance-optimization)
- [Tutorial 6: Research Experiment](#tutorial-6-research-experiment)
- [Tutorial Index and Navigation](#tutorial-index-and-navigation)

---

## Tutorial 1: Basic Neural Computation

### Tutorial Overview

This tutorial introduces you to **basic neural computation** using NLM. You'll learn how to:

- Create and configure a simple brain
- Run basic simulations
- Understand neural dynamics and spike generation
- Access memory systems
- Interpret simulation results

> **Tutorial Level:** Beginner
> **Time Required:** 30-45 minutes
> **Prerequisites:** Basic programming knowledge, no NLM experience required

### Learning Objectives

After completing this tutorial, you will be able to:

1. **Understand** the basic components of NLM
2. **Create** a simple brain configuration
3. **Run** a basic neural simulation
4. **Analyze** simulation results
5. **Use** memory systems in NLM

### Getting Started

#### Step 1: Setup Environment

```bash
# Create a directory for your tutorials
mkdir nlm_tutorials
cd nlm_tutorials

# Install NLM Python dependencies
pip install pynlm numpy matplotlib
```

#### Step 2: Create Tutorial Files

Create a Python file for this tutorial:

```python
# File: tutorial1_basic_neural_computation.py
"""
Tutorial 1: Basic Neural Computation

This tutorial introduces basic neural computation using NLM.
"""
import pynlm
import time

def run_basic_tutorial():
    """Run the basic neural computation tutorial"""
    print("=== Tutorial 1: Basic Neural Computation ===\n")
    
    # Part 1: Understanding NLM Basics
    print("Part 1: Understanding NLM Basics")
    print("-" * 40)
    
    print("\nWhat is NLM?")
    print("NLM (熙然) is an experimental computational brain project.")
    print("It simulates biological brains using spiking neural networks.")
    print("Key features:")
    print("  - Leaky Integrate-and-Fire (LIF) neurons")
    print("  - Event-driven spike processing")
    print("  - Multiple memory systems")
    print("  - Neuromodulation")
    print("  - Development over time")
    
    print("\nWhat you'll create:")
    print("  - A simple brain with default configuration")
    print("  - A basic simulation with spike generation")
    print("  - Memory system access and usage")
    print("  - Results analysis and interpretation")
    
    # Part 2: Create Basic Brain
    print("\n\nPart 2: Creating Basic Brain")
    print("-" * 40)
    
    print("\nStep 1: Create brain configuration")
    config = pynlm.createDefaultConfig()
    print(f"   Configuration created with {config.get('brain.neuron_count')} neurons")
    print(f"   World size: {config.get('world.width')}x{config.get('world.height')}")
    print(f"   Simulation steps: {config.get('simulation.steps')}")
    
    print("\nStep 2: Create and initialize brain")
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("   ✓ Brain created and initialized successfully")
    
    # Part 3: Run Basic Simulation
    print("\n\nPart 3: Running Basic Simulation")
    print("-" * 40)
    
    print("\nStep 1: Prepare for simulation")
    start_time = time.time()
    print("   Configuration:")
    print(f"     - Step time: {config.get('simulation.time_step')} seconds")
    print(f"     - Total steps: {config.get('simulation.steps')}")
    
    print("\nStep 2: Run simulation")
    print("   Simulating neural dynamics...")
    
    for step in range(config.get('simulation.steps')):
        brain.step(step)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"     Step {step}/{config.get('simulation.steps')}")
    
    simulation_time = time.time() - start_time
    print(f"   ✓ Simulation completed in {simulation_time:.2f} seconds")
    
    # Part 4: Analyze Results
    print("\n\nPart 4: Analyzing Results")
    print("-" * 40)
    
    print("\nStep 1: Basic brain statistics")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getTotalSpikeCount() / config.get('simulation.steps'):.2f} Hz")
    
    print("\nStep 2: Memory system analysis")
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    print(f"   Working memory: {'enabled' if working_memory else 'disabled'}")
    print(f"   Episodic memory: {'enabled' if episodic_memory else 'disabled'}")
    print(f"   Associative memory: {'enabled' if associative_memory else 'disabled'}")
    
    if working_memory:
        print(f"   Working memory entries: {working_memory.getEntryCount()}")
    
    # Part 5: Basic Memory Operations
    print("\n\nPart 5: Basic Memory Operations")
    print("-" * 40)
    
    if working_memory:
        print("\nStep 1: Store information in working memory")
        working_memory.store("task_objective", "solve_puzzle")
        working_memory.store("current_location", "room_1")
        working_memory.store("target_objective", "reach_door")
        
        print("   ✓ Information stored successfully")
        
        print("\nStep 2: Retrieve information from working memory")
        retrieved_task = working_memory.retrieve("task_objective", threshold=0.7)
        retrieved_location = working_memory.retrieve("current_location", threshold=0.5)
        
        print(f"   Retrieved task: {retrieved_task}")
        print(f"   Retrieved location: {retrieved_location}")
        print(f"   Total memory entries: {working_memory.getEntryCount()}")
    
    # Part 6: Experiment Design
    print("\n\nPart 6: Designing Basic Experiments")
    print("-" * 40)
    
    print("\nStep 1: Design experiment parameters")
    print("   Experiment parameters:")
    print(f"     - Neuron count: {config.get('brain.neuron_count')}")
    print(f"     - Simulation steps: {config.get('simulation.steps')}")
    print(f"     - Time step: {config.get('simulation.time_step')} seconds")
    print(f"     - Memory capacity: {working_memory.getMemoryCapacity() if working_memory else 'N/A'}")
    
    print("\nStep 2: Define experiment goals")
    print("   Experiment goals:")
    print("     - Test basic neural computation")
    print("     - Validate memory system functionality")
    print("     - Measure spike generation and dynamics")
    print("     - Analyze system performance")
    
    # Part 7: Conclusion
    print("\n\nPart 7: Conclusion")
    print("-" * 40)
    
    print(f"\nTutorial completed successfully!")
    print(f"\nKey achievements:")
    print(f"  ✓ Created basic brain configuration")
    print(f"  ✓ Ran neural simulation with {brain.getTotalSpikeCount()} spikes")
    print(f"  ✓ Accessed memory systems")
    print(f"  ✓ Stored and retrieved information")
    print(f"  ✓ Analyzed simulation results")
    
    print(f"\nNext steps:")
    print(f"  - Explore advanced features in Tutorial 2")
    print(f"  - Try Phase 6 integration in Tutorial 4")
    print(f"  - Learn performance optimization in Tutorial 5")
    
    # Cleanup
    brain.shutdown()
    
    print(f"\n=== Tutorial Complete ===")

if __name__ == "__main__":
    run_basic_tutorial()
```

#### Running Tutorial 1

**Command Line:**
```bash
# Run the tutorial
cd /path/to/tutorials
python tutorial1_basic_neural_computation.py
```

**Expected Output:**
```
=== Tutorial 1: Basic Neural Computation ===

Part 1: Understanding NLM Basics
----------------------------------------

What is NLM?
NLM (熙然) is an experimental computational brain project.
It simulates biological brains using spiking neural networks.
Key features:
  - Leaky Integrate-and-Fire (LIF) neurons
  - Event-driven spike processing
  - Multiple memory systems
  - Neuromodulation
  - Development over time

What you'll create:
  - A simple brain with default configuration
  - A basic simulation with spike generation
  - Memory system access and usage
  - Results analysis and interpretation

Part 2: Creating Basic Brain
----------------------------------------

Step 1: Create brain configuration
   Configuration created with 1000 neurons
   World size: 20x20
   Simulation steps: 1000

Step 2: Create and initialize brain
   ✓ Brain created and initialized successfully

Part 3: Running Basic Simulation
----------------------------------------

Step 1: Prepare for simulation
   Configuration:
     - Step time: 0.1 seconds
     - Total steps: 1000

Step 2: Run simulation
   Simulating neural dynamics...
     Step 0/1000
     Step 100/1000
     Step 200/1000
     Step 300/1000
     Step 400/1000
     Step 500/1000
     Step 600/1000
     Step 700/1000
     Step 800/1000
     Step 900/1000
     Step 1000/1000
   ✓ Simulation completed in X.XX seconds

Part 4: Analyzing Results
----------------------------------------

Step 1: Basic brain statistics
   Total spikes: XXXX
   Average firing rate: X.XX Hz

Step 2: Memory system analysis
   Working memory: enabled
   Episodic memory: enabled
   Associative memory: enabled

   Working memory entries: X

Part 5: Basic Memory Operations
----------------------------------------

Step 1: Store information in working memory
   ✓ Information stored successfully

Step 2: Retrieve information from working memory
   Retrieved task: XXXX
   Retrieved location: XXXX
   Total memory entries: X

Part 6: Designing Basic Experiments
----------------------------------------

Step 1: Design experiment parameters
   Experiment parameters:
     - Neuron count: 1000
     - Simulation steps: 1000
     - Time step: 0.1 seconds
     - Memory capacity: 100

Step 2: Define experiment goals
   Experiment goals:
     - Test basic neural computation
     - Validate memory system functionality
     - Measure spike generation and dynamics
     - Analyze system performance

Part 7: Conclusion
----------------------------------------

Tutorial completed successfully!

Key achievements:
  ✓ Created basic brain configuration
  ✓ Ran neural simulation with XXXX spikes
  ✓ Accessed memory systems
  ✓ Stored and retrieved information
  ✓ Analyzed simulation results

Next steps:
  - Explore advanced features in Tutorial 2
  - Try Phase 6 integration in Tutorial 4
  - Learn performance optimization in Tutorial 5

=== Tutorial Complete ===
```

### Tutorial Structure

#### Learning Modules

1. **Module 1: NLM Fundamentals** (30 minutes)
   - Understanding NLM architecture
   - Key components and terminology
   - Basic simulation setup

2. **Module 2: Brain Creation** (20 minutes)
   - Configuration basics
   - Brain initialization
   - System verification

3. **Module 3: Simulation Execution** (15 minutes)
   - Running neural simulations
   - Monitoring progress
   - Managing simulation parameters

4. **Module 4: Results Analysis** (20 minutes)
   - Interpreting spike data
   - Memory system analysis
   - Performance evaluation

5. **Module 5: Experiment Design** (25 minutes)
   - Designing experiments
   - Setting up parameters
   - Defining objectives

#### Assessment and Practice

**Practice Exercises:**
1. Modify simulation parameters and observe effects
2. Experiment with different memory configurations
3. Create custom brain configurations
4. Design and run your own experiments

**Assessment Tasks:**
1. Complete the tutorial and answer reflection questions
2. Create a simple experiment using NLM
3. Compare results with expected outcomes
4. Document your learning process

### Tutorial Files

#### Required Files

1. **tutorial1_basic_neural_computation.py**
   - Main tutorial implementation
   - Step-by-step instructions
   - Code examples and explanations

2. **tutorial1_readme.md**
   - Tutorial overview and objectives
   - Installation instructions
   - Running and testing guide
   - Additional resources

#### Optional Files

1. **tutorial1_examples/**
   - Additional code examples
   - Advanced demonstrations
   - Edge case handling

2. **tutorial1_tests/**
   - Unit tests for tutorial functionality
   - Automated testing framework
   - Performance benchmarks

### Tutorial Environment Setup

#### Local Development Environment

**Requirements:**
- Python 3.8+ (recommended)
- NumPy 1.20+ (required)
- Matplotlib 3.3+ (optional)
- PyNLM package (main dependency)

**Installation Steps:**
```bash
# Create virtual environment
cd /path/to/tutorials
python -m venv nlm_env
source nlm_env/bin/activate  # On Windows: nlm_env\Scripts\activate

# Install dependencies
pip install pynlm numpy matplotlib scikit-learn pandas

# Install NLM in development mode
pip install -e .
```

#### Cloud/Colab Environment

**Requirements:**
- Google Colab or similar cloud platform
- Internet connection
- Python environment

**Setup Steps:**
```python
# In Google Colab cell
!pip install pynlm numpy matplotlib scikit-learn pandas
```

#### Docker Environment

**Dockerfile Example:**
```dockerfile
FROM python:3.9-slim

WORKDIR /app

COPY requirements.txt .
RUN pip install -r requirements.txt

COPY . .

CMD ["python", "tutorial1_basic_neural_computation.py"]
```

#### Jupyter Notebook Environment

**Notebook Setup:**
```python
# Create Jupyter notebook
jupyter notebook

# In notebook cell
import pynlm
import matplotlib.pyplot as plt
import numpy as np

# Run tutorial code
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Your code here...

brain.shutdown()
```

### Tutorial Customization

#### Customizing for Different Audiences

**For Students:**
- Add more detailed explanations
- Include more visual aids
- Provide additional practice exercises

**For Researchers:**
- Add advanced configuration options
- Include experimental setup examples
- Provide performance benchmarking

**For Developers:**
- Add API documentation examples
- Include error handling and debugging
- Provide code optimization tips

#### Customizing for Different Platforms

**For Web-based Learning:**
- Convert to web-compatible format
- Add interactive elements
- Include browser-based simulations

**For Mobile Learning:**
- Optimize for mobile devices
- Reduce memory usage
- Simplify user interface

**For Offline Learning:**
- Package all dependencies
- Include comprehensive documentation
- Provide standalone execution

### Tutorial Evaluation

#### Assessment Methods

**Knowledge Assessment:**
- Multiple-choice questions
- Short answer questions
- Code completion exercises

**Practical Assessment:**
- Code review
- Peer feedback
- Instructor evaluation
- Automated testing

#### Feedback and Improvement

**Immediate Feedback:**
- Code execution results
- Interactive examples
- Progress tracking

**Delayed Feedback:**
- Instructor comments
- Peer review
- Self-assessment

**Continuous Improvement:**
- User feedback collection
- Tutorial updates
- Feature enhancements

### Tutorial Resources

#### Reference Materials

**Documentation:**
- NLM Documentation Hub
- Python API Reference
- C++ API Reference
- Architecture Documentation

**Code Examples:**
- GitHub repository
- Example projects
- Sample code

**Tutorials:**
- Comprehensive tutorial series
- Step-by-step guides
- Advanced topics

#### Support Resources

**Technical Support:**
- GitHub Issues
- Stack Overflow
- Community forums
- Email support

**Learning Support:**
- Help documentation
- FAQ
- User guides
- Tutorial index

#### Additional Reading

**Related Topics:**
- Neural Networks
- Spiking Neural Networks
- Computational Neuroscience
- Brain-inspired AI

**External Resources:**
- Research papers
- Conference proceedings
- Online courses
- Books and publications

### Tutorial Completion

#### Requirements for Completion

**Knowledge Requirements:**
- [ ] Understand NLM basic concepts
- [ ] Create and configure a brain
- [ ] Run and analyze neural simulations
- [ ] Use memory systems effectively
- [ ] Design and run experiments

**Practical Requirements:**
- [ ] Complete all tutorial exercises
- [ ] Run and document experiments
- [ ] Analyze and interpret results
- [ ] Create custom configurations
- [ ] Use documentation resources

#### Certificate of Completion

Upon completing Tutorial 1, you will receive:
- Certificate of completion
- Tutorial completion record
- Access to advanced tutorials
- Community membership

### Next Tutorial

After completing Tutorial 1, proceed to **Tutorial 2: Python Integration**, where you will:

1. Learn advanced Python API usage
2. Explore Phase 6 integration
3. Use configuration management
4. Implement error handling
5. Optimize performance

**Tutorial 2: Python Integration** →

---

## Tutorial 2: Python Integration

[Content continues...]

To see the complete tutorial content, please visit the NLM documentation hub or the tutorial directory.

---

## Tutorial Index and Navigation

### Complete Tutorial Series

1. **Tutorial 1:** Basic Neural Computation ✅ **Completed**
2. **Tutorial 2:** Python Integration ⏳ **In Progress**
3. **Tutorial 3:** Advanced Configuration ⏳ **Pending**
4. **Tutorial 4:** Phase 6 Integration ⏳ **Pending**
5. **Tutorial 5:** Performance Optimization ⏳ **Pending**
6. **Tutorial 6:** Research Experiment ⏳ **Pending**

### Tutorial Navigation

Use the following links to navigate through the tutorial series:

- [Tutorial 1: Basic Neural Computation](docs/tutorials/tutorial1_basic_neural_computation.md) ✅
- [Tutorial 2: Python Integration](docs/tutorials/tutorial2_python_integration.md)
- [Tutorial 3: Advanced Configuration](docs/tutorials/tutorial3_advanced_configuration.md)
- [Tutorial 4: Phase 6 Integration](docs/tutorials/tutorial4_phase6_integration.md)
- [Tutorial 5: Performance Optimization](docs/tutorials/tutorial5_performance_optimization.md)
- [Tutorial 6: Research Experiment](docs/tutorials/tutorial6_research_experiment.md)

### Tutorial Resources

#### Learning Resources

**Tutorial Materials:**
- Complete tutorial source code
- Example implementations
- Reference documentation
- Test frameworks

**Support Materials:**
- Troubleshooting guide
- FAQ
- Community resources
- Contact information

#### Tutorial Updates

**Version Control:**
- GitHub repository
- Issue tracking
- Pull requests
- Code reviews

**Documentation Updates:**
- Regular tutorial updates
- Bug fixes
- Feature additions
- User feedback

#### Tutorial Community

**Join the Community:**
- GitHub discussions
- Forum participation
- Social media
- Meetups

**Contribute to Tutorials:**
- Write tutorials
- Fix issues
- Add examples
- Improve documentation

---

## Tutorial Usage

### Running Tutorials

**Command Line Usage:**
```bash
# Run all tutorials
cd /path/to/tutorials
for tutorial in tutorial1_*.py; do
    python "$tutorial"
    echo "Completed: $tutorial"
done

# Run specific tutorial
python tutorial1_basic_neural_computation.py
```

**Jupyter Notebook Usage:**
```python
# In Jupyter notebook
from tutorial1_basic_neural_computation import run_basic_tutorial
run_basic_tutorial()
```

### Tutorial Documentation

**API Documentation:**
- Python API Reference
- C++ API Reference
- Configuration documentation
- Function references

**Usage Documentation:**
- Installation guide
- Setup instructions
- Configuration options
- Troubleshooting

### Tutorial Examples

**Example 1: Basic Usage**
```python
import pynlm

# Create and run brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

brain.shutdown()
```

**Example 2: Memory Operations**
```python
brain = pynlm.createBrain(pynlm.createPhase6Config())
brain.initialize()

working_memory = brain.getWorkingMemory()
working_memory.store("task", "solve_puzzle")
retrieved = working_memory.retrieve("task", threshold=0.7)
```

**Example 3: Advanced Configuration**
```python
config = pynlm.createPhase6Config()
config.set("memory.working.capacity", 200)
config.set("cognition.planning.enabled", True)
```

### Tutorial Conclusion

The NLM tutorial series provides **comprehensive learning materials** for mastering NLM. Each tutorial builds on the previous one, allowing you to develop your skills systematically.

> **Key Features:**
> - **Progressive learning**: Each tutorial builds on previous knowledge
> - **Hands-on practice**: Code examples and exercises
> - **Comprehensive documentation**: Complete references and examples
> - **Community support**: Resources for troubleshooting and learning
> - **Continuous updates**: Regular improvements and new features

> **Next Steps:**
> 1. Complete Tutorial 1: Basic Neural Computation
> 2. Proceed to Tutorial 2: Python Integration
> 3. Explore advanced tutorials for research applications
> 4. Join the community for additional support
> 5. Contribute to tutorial development

**Happy learning with NLM! 🚀**