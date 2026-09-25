# NLM Tutorials

This directory contains interactive learning materials for the NLM neural simulation platform.

## Overview

NLM tutorials provide step-by-step guidance for understanding and using the neural simulation system. Each tutorial builds on previous concepts and includes interactive exercises.

## Tutorial Structure

Each tutorial includes:
- Learning objectives
- Step-by-step instructions
- Interactive exercises
- Code examples
- Practice challenges

## Available Tutorials

### 1. NLM Fundamentals

**File:** `tutorials/01_fundamentals.md`

This tutorial introduces the NLM platform and covers:

#### Learning Objectives
- Understand NLM architecture
- Identify key components
- Run basic simulations
- Interpret results

#### Getting Started

**Step 1: Installation**
```bash
# Clone the repository
nlm@example:/# git clone https://github.com/yourrepo/nlm.git
cd nlm

# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

**Step 2: First Run**
```bash
# Run the basic demonstration
./nlm_phase6_demo

# Or use the CLI
nlm demo phase2
```

**Step 3: Explore the Interface**
```bash
# Interactive mode
nlm explore

# Check configuration
nlm config show

# View documentation
nlm help
```

#### Key Concepts

##### Neurons and Synapses

NLM simulates neurons using the Leaky Integrate-and-Fire (LIF) model:

```cpp
class Neuron {
private:
    double membranePotential;  // Current voltage
    double threshold;          // Spike threshold
    double refractoryPeriod;   // Refractory period after spike
    
public:
    void update(double dt);   // Update membrane potential
    void injectCurrent(double I);  // Apply current input
    bool isSpiking();         // Check if neuron is spiking
};
```

**Key Equations:**
- Membrane potential dynamics: τₘ·dV/dt = -V + R·I
- Spike when V ≥ threshold
- Reset after spike: V = V_reset

##### Plasticity Rules

NLM implements two main plasticity rules:

1. **STDP (Spike-Timing-Dependent Plasticy)**
   - Pre-before-post: LTP (strengthening)
   - Post-before-pre: LTD (weakening)
   - Time window: ~20-50ms

2. **Hebbian Learning**
   - Co-activation strengthens synapses
   - Supports pattern recognition

#### Interactive Exercise 1: Basic Connectivity

**Objective:** Understand how neurons connect and communicate.

**Instructions:**
1. Run a simulation with 100 neurons
2. Inject current into neuron 0
3. Observe spike propagation
4. Record the number of spikes

**Task:** Modify the injection current and observe the effect on spike count.

#### Practice Challenge

Design an experiment to test Hebbian learning:
- Create two groups of neurons
- Co-activate them repeatedly
- Measure synaptic weight changes
- Compare with theoretical predictions

### 2. Memory Systems

**File:** `tutorials/02_memory.md`

This tutorial explores NLM's memory systems and covers:

#### Learning Objectives
- Understand working memory
- Learn about episodic memory
- Explore concept formation
- Implement memory-based decision making

#### Working Memory

**Concept:** Neural working memory maintains information through persistent activity.

```cpp
class WorkingMemory {
private:
    std::vector<NeuronId> activeNeurons;
    std::vector<float> storedValues;
    double decayRate;
    
public:
    void encode(float value);
    float retrieve();
    void update();  // Decay over time
    bool isActive();
};
```

**Key Properties:**
- Capacity limited (~7±2 items)
- Decay over time
- Competitive selection
- Neural grounding (not vector storage)

#### Episodic Memory

**Concept:** Experiences stored as neural patterns for later replay.

```cpp
class EpisodicMemory {
private:
    std::vector<MemoryPattern> patterns;
    std::vector<double> timestamps;
    double replayFrequency;
    
public:
    void recordExperience(const std::vector<float>& pattern);
    void replay(double timePoint);
    std::vector<float> retrieveSimilar(double timeWindow);
};
```

#### Interactive Exercise 2: Memory Capacity

**Objective:** Understand working memory limitations.

**Instructions:**
1. Create a working memory system
2. Attempt to store items sequentially
3. Measure memory span
4. Vary decay rate and observe effects

**Task:** Determine the optimal decay rate for different memory spans.

#### Concept Formation

**Concept:** Discover patterns without pre-labeled categories.

```cpp
class ConceptFormation {
private:
    std::vector<Concept> concepts;
    double stabilityThreshold;
    
public:
    void processExperience(const std::vector<float>& features);
    std::string classifyExperience(const std::vector<float>& features);
    void createNewConcept(const std::vector<float>& features);
};
```

**Process:**
1. Receive feature vectors from experience
2. Calculate similarity to existing concepts
3. Assign to best-matching concept
4. Create new concept if similarity below threshold

#### Practice Challenge

Implement a simple concept formation system:
- Process 100 random experience patterns
- Limit to 5 concepts
- Measure classification accuracy
- Visualize concept boundaries

### 3. Neuromodulation

**File:** `tutorials/03_neuromodulation.md`

This tutorial covers neuromodulation systems and covers:

#### Learning Objectives
- Understand neuromodulation concepts
- Implement dopamine signals
- Create curiosity-driven exploration
- Build prediction error systems

#### Dopamine System

**Role:** Reinforcement learning and reward prediction.

```cpp
class DopamineSystem {
private:
    double expectedValue;
    double actualValue;
    double predictionError;
    double learningRate;
    double baselineLevel;
    
public:
    void update(double reward, double uncertainty);
    double getSignal();
    void setExpectedValue(double value);
};
```

**Prediction Error:** δ = r + γV(s') - V(s)

#### Curiosity System

**Role:** Drive exploration and discovery.

```cpp
class CuriositySystem {
private:
    double uncertaintyEstimate;
    double intrinsicMotivation;
    double explorationBonus;
    
public:
    void updateUncertainty(const std::vector<float>& state);
    double getExplorationDrive();
    bool shouldExplore();
};
```

**Formula:** Motivation = Novelty × Uncertainty × Interest

#### Interactive Exercise 3: Neuromodulation

**Objective:** Design a neuromodulation system for exploration.

**Instructions:**
1. Create a dopamine system
2. Set up prediction error learning
3. Add curiosity signals
4. Run in an environment
5. Compare with pure exploration

**Task:** Modify the exploration-exploitation balance and observe effects.

#### Practice Challenge

Create a dual-system neuromodulation:
- Primary: Reward prediction error
- Secondary: Novelty detection
- Integrate: Average both signals
- Test: Performance in changing environments

### 4. Advanced Integration

**File:** `tutorials/04_integration.md`

This tutorial covers system integration and covers:

#### Learning Objectives
- Integrate all NLM components
- Debug complex interactions
- Optimize performance
- Analyze system behavior

#### System Integration

**Challenge:** Make all components work together cohesively.

**Key Components:**
1. Neural computation (Phase 2)
2. Memory systems (Phase 4)
3. Neuromodulation
4. Prediction and planning
5. Development and adaptation

#### Interactive Exercise 4: Full Integration

**Objective:** Run and analyze the complete integrated system.

**Instructions:**
1. Load Phase 6 integration configuration
2. Run with monitoring enabled
3. Track all system metrics
4. Identify bottlenecks
5. Optimize parameters

**Task:** Find parameter settings that maximize learning while minimizing computation.

#### Practice Challenge

Design an adaptive learning system:
- Continuously monitor performance
- Adjust neuromodulation parameters
- Balance exploration and exploitation
- Adapt to environment changes
- Maintain memory of successful strategies

## Tutorial Tools

### Interactive Runner

**File:** `tutorials/run_tutorial.py`

```python
#!/usr/bin/env python3

"""
Interactive tutorial runner for NLM
Provides a command-line interface for accessing tutorials
"""

import argparse
import subprocess
import os
import sys
from pathlib import Path

class TutorialRunner:
    def __init__(self):
        self.tutorial_dir = Path(__file__).parent
        self.available_tutorials = [
            "01_fundamentals",
            "02_memory", 
            "03_neuromodulation",
            "04_integration"
        ]
    
    def list_tutorials(self):
        print("Available NLM Tutorials:\n")
        
        for tutorial_id in self.available_tutorials:
            readme_path = self.tutorial_dir / f"{tutorial_id}.md"
            if readme_path.exists():
                with open(readme_path, 'r') as f:
                    content = f.read()
                
                # Extract first few lines for description
                lines = content.split('\n')
                description = ""
                for line in lines:
                    if line.strip().startswith('This tutorial'):
                        description = line.strip().replace('This tutorial', '')
                        break
                
                print(f"  {tutorial_id}: {description}")
    
    def run_tutorial(self, tutorial_id, interactive=False):
        if tutorial_id not in self.available_tutorials:
            print(f"Error: Tutorial '{tutorial_id}' not found")
            return False
        
        tutorial_path = self.tutorial_dir / f"{tutorial_id}.md"
        
        if not tutorial_path.exists():
            print(f"Error: Tutorial file not found: {tutorial_path}")
            return False
        
        print(f"\n=== NLM Tutorial {tutorial_id} ===\n")
        
        # Display tutorial
        with open(tutorial_path, 'r') as f:
            tutorial_content = f.read()
        
        print(tutorial_content)
        
        # Ask if user wants to run associated NLM commands
        if interactive:
            print("\nDo you want to run interactive exercises? (y/n): ", end="")
            choice = input().lower().strip()
            
            if choice == 'y' or choice == 'yes':
                self.run_interactive_exercises(tutorial_id)
        
        return True
    
    def run_interactive_exercises(self, tutorial_id):
        print(f"\nRunning interactive exercises for Tutorial {tutorial_id}...")
        
        # Common exercise commands
        exercise_commands = {
            "01_fundamentals": [
                ["nlm demo phase2", "Run Phase 2 demonstration"],
                ["nlm config init", "Create sample configuration"],
                ["nlm explore", "Start exploration mode"]
            ],
            "02_memory": [
                ["nlm experiment phase4 memory_test", "Run memory experiment"],
                ["nlm explore --memory", "Explore memory systems"],
                ["nlm benchmark", "Benchmark memory performance"]
            ],
            "03_neuromodulation": [
                ["nlm demo phase6 --neuromodulation", "Run neuromodulation demo"],
                ["nlm explore --curiosity", "Explore curiosity-driven learning"],
                ["nlm experiment phase4 neuromodulation", "Run neuromodulation experiment"]
            ],
            "04_integration": [
                ["nlm run --config examples/phase6_integration.cfg", "Run full integration"],
                ["nlm explore --integration", "Explore integrated system"],
                ["nlm benchmark", "Benchmark integrated system"]
            ]
        }
        
        if tutorial_id in exercise_commands:
            for cmd, description in exercise_commands[tutorial_id]:
                print(f"\nExercise: {description}")
                print(f"Command: {cmd}")
                
                try:
                    subprocess.run(cmd.split())
                    print("Exercise completed.")
                except Exception as e:
                    print(f"Error running exercise: {e}")
        else:
            print("No specific exercises defined for this tutorial.")

def main():
    parser = argparse.ArgumentParser(description="Run NLM tutorials")
    parser.add_argument("action", choices=["list", "run"], help="Action to perform")
    parser.add_argument("tutorial_id", nargs="?", help="Tutorial ID to run")
    parser.add_argument("--interactive", action="store_true", help="Run interactive exercises")
    
    args = parser.parse_args()
    
    runner = TutorialRunner()
    
    if args.action == "list":
        runner.list_tutorials()
    elif args.action == "run":
        if not args.tutorial_id:
            print("Error: tutorial_id required for 'run' action")
            runner.list_tutorials()
            sys.exit(1)
        
        runner.run_tutorial(args.tutorial_id, args.interactive)

if __name__ == "__main__":
    main()
```

### Web-based Tutorial Interface

**File:** `tutorials/web_interface.html`

A simple web interface for browsing and running tutorials:

```html
<!DOCTYPE html>
<html>
<head>
    <title>NLM Tutorials</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; }
        .tutorial-list { max-width: 800px; margin: 0 auto; }
        .tutorial-card {
            border: 1px solid #ddd;
            margin: 20px 0;
            padding: 20px;
            border-radius: 5px;
        }
        .tutorial-card h3 { margin-top: 0; }
        .exercise-list {
            background-color: #f5f5f5;
            padding: 15px;
            margin: 15px 0;
            border-radius: 3px;
        }
    </style>
</head>
<body>
    <h1>NLM Tutorial System</h1>
    <p>An interactive learning platform for NLM neural simulation.</p>
    
    <div class="tutorial-list">
        <h2>Available Tutorials</h2>
        
        <div class="tutorial-card">
            <h3>01: NLM Fundamentals</h3>
            <p>Introduction to NLM architecture, neurons, synapses, and basic plasticity.</p>
            <div class="exercise-list">
                <strong>Exercises:</strong>
                <ul>
                    <li>Basic connectivity test</li>
                    <li>Plasticity investigation</li>
                    <li>Experiment design</li>
                </ul>
            </div>
            <button onclick="runTutorial('01')">Start Tutorial</button>
        </div>
        
        <div class="tutorial-card">
            <h3>02: Memory Systems</h3>
            <p>Working memory, episodic memory, and concept formation in NLM.</p>
            <div class="exercise-list">
                <strong>Exercises:</strong>
                <ul>
                    <li>Memory capacity analysis</li>
                    <li>Episodic memory encoding</li>
                    <li>Concept formation</li>
                </ul>
            </div>
            <button onclick="runTutorial('02')">Start Tutorial</button>
        </div>
        
        <div class="tutorial-card">
            <h3>03: Neuromodulation</h3>
            <p>Dopamine, curiosity, and prediction error systems.</p>
            <div class="exercise-list">
                <strong>Exercises:</strong>
                <ul>
                    <li>Neuromodulation design</li>
                    <li>Curiosity-driven exploration</li>
                    <li>Dual-system integration</li>
                </ul>
            </div>
            <button onclick="runTutorial('03')">Start Tutorial</button>
        </div>
        
        <div class="tutorial-card">
            <h3>04: Advanced Integration</h3>
            <p>Integrating all NLM components for complex behavior.</p>
            <div class="exercise-list">
                <strong>Exercises:</strong>
                <ul>
                    <li>Full system integration</li>
                    <li>Performance optimization</li>
                    <li>Adaptive learning design</li>
                </ul>
            </div>
            <button onclick="runTutorial('04')">Start Tutorial</button>
        </div>
    </div>
    
    <script>
        function runTutorial(tutorialId) {
            // In a real implementation, this would fetch the tutorial content
            // and run the associated NLM commands
            console.log(`Starting tutorial ${tutorialId}`);
            alert(`Tutorial ${tutorialId} would start here`);
        }
        
        // Load tutorial content on page load
        document.addEventListener('DOMContentLoaded', function() {
            // Initialize tutorial system
            console.log('NLM Tutorial System loaded');
        });
    </script>
</body>
</html>
```

## Tutorial Metadata

Each tutorial can be tracked with metadata for progress tracking:

**File:** `tutorials/tutorial_progress.json`

```json
{
  "tutorials": {
    "01_fundamentals": {
      "name": "NLM Fundamentals",
      "completed": true,
      "progress": 1.0,
      "last_accessed": "2026-01-15T10:30:00Z",
      "exercises_completed": ["basic_connectivity", "plasticity_test", "experiment_design"],
      "time_spent": 120.5,
      "rating": 5
    },
    "02_memory": {
      "name": "Memory Systems",
      "completed": false,
      "progress": 0.7,
      "last_accessed": "2026-01-15T14:20:00Z",
      "exercises_completed": ["working_memory", "episodic_memory"],
      "time_spent": 85.0,
      "rating": 4
    }
  },
  "current_tutorial": "02_memory"
}
```

This tutorial system provides a comprehensive learning experience for NLM users, from complete beginners to advanced users.