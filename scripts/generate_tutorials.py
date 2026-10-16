#!/usr/bin/env python3
"""
NLM Tutorials Documentation Generator
Generates comprehensive tutorial documentation for learning and teaching NLM
"""

import argparse
import re
import os
import sys
from pathlib import Path
from datetime import datetime
from typing import Dict, List, Any, Optional

class TutorialsDocumentationGenerator:
    def __init__(self, docs_dir: str, output_dir: str):
        self.docs_dir = Path(docs_dir)
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(parents=True, exist_ok=True)
        
        # Tutorial categories
        self.tutorial_categories = {
            "basics": "Basic NLM Concepts",
            "programming": "Programming NLM",
            "memory": "Memory Systems",
            "neuromodulation": "Neuromodulation",
            "development": "Brain Development",
            "prediction": "Prediction Systems",
            "cognition": "Cognitive Mechanisms",
            "integration": "System Integration",
            "performance": "Performance Optimization",
            "troubleshooting": "Troubleshooting"
        }
    
    def generate_tutorials_documentation(self):
        """Generate comprehensive tutorials documentation"""
        print("Generating NLM tutorials documentation...")
        
        # Parse existing tutorials
        parsed_tutorials = self.parse_existing_tutorials()
        
        # Generate main tutorials documentation
        self.generate_main_tutorials_doc(parsed_tutorials)
        
        # Generate tutorial chapters
        self.generate_tutorial_chapters(parsed_tutorials)
        
        # Generate tutorial index
        self.generate_tutorial_index(parsed_tutorials)
        
        # Generate interactive tutorial guides
        self.generate_interactive_tutorials()
        
        print(f"Tutorials documentation generated in: {self.output_dir}")
    
    def parse_existing_tutorials(self) -> Dict[str, Any]:
        """Parse existing tutorial documentation"""
        tutorials_data = {
            "chapters": {},
            "interactive": [],
            "quick_starts": [],
            "step_by_step": []
        }
        
        # Look for TUTORIALS.md and other tutorial files
        tutorial_files = list(self.docs_dir.glob("*.md"))
        
        for tutorial_file in tutorial_files:
            if tutorial_file.name.lower() == "tutorials.md":
                self.parse_tutorials_file(tutorial_file, tutorials_data)
            elif "tutorial" in tutorial_file.name.lower():
                self.parse_tutorial_file(tutorial_file, tutorials_data)
        
        return tutorials_data
    
    def parse_tutorials_file(self, tutorial_file: Path, tutorials_data: Dict[str, Any]):
        """Parse the main TUTORIALS.md file"""
        try:
            content = tutorial_file.read_text()
            
            # Extract tutorial sections
            sections = self.extract_tutorial_sections(content)
            
            tutorials_data["chapters"].update(sections)
            
        except Exception as e:
            print(f"Warning: Failed to parse {tutorial_file}: {e}")
    
    def parse_tutorial_file(self, tutorial_file: Path, tutorials_data: Dict[str, Any]):
        """Parse other tutorial files"""
        try:
            content = tutorial_file.read_text()
            filename = tutorial_file.stemname().lower()
            
            # Categorize based on filename
            if "quick" in filename:
                tutorials_data["quick_starts"].append({
                    "name": tutorial_file.name,
                    "content": content,
                    "file": str(tutorial_file.relative_to(self.docs_dir))
                })
            elif "step" in filename or "guide" in filename:
                tutorials_data["step_by_step"].append({
                    "name": tutorial_file.name,
                    "content": content,
                    "file": str(tutorial_file.relative_to(self.docs_dir))
                })
            elif "interactive" in filename or "demo" in filename:
                tutorials_data["interactive"].append({
                    "name": tutorial_file.name,
                    "content": content,
                    "file": str(tutorial_file.relative_to(self.docs_dir))
                })
            
        except Exception as e:
            print(f"Warning: Failed to parse {tutorial_file}: {e}")
    
    def extract_tutorial_sections(self, content: str) -> Dict[str, Any]:
        """Extract sections from tutorial content"""
        sections = {}
        
        # Split by section headers
        lines = content.split('\n')
        current_section = None
        current_content = []
        
        for line in lines:
            if line.startswith('## '):
                if current_section:
                    sections[current_section] = '\n'.join(current_content)
                
                current_section = line[3:].strip()
                current_content = []
            elif line.startswith('### '):
                if current_section:
                    current_content.append(f'\n**{line[4:].strip()}**\n')
                else:
                    current_content.append(line)
            else:
                current_content.append(line)
        
        if current_section and current_content:
            sections[current_section] = '\n'.join(current_content)
        
        return sections
    
    def generate_main_tutorials_doc(self, tutorials_data: Dict[str, Any]):
        """Generate main tutorials documentation"""
        content = f"""# NLM Tutorials Documentation

## Overview

This document provides comprehensive tutorials for learning and teaching the NLM (熙然) neural learning machine system. The tutorials are organized into logical categories to guide learners from basic concepts to advanced integration and optimization techniques.

The NLM tutorials cover the following areas:

1. [Basic NLM Concepts](#basics-nlm-concepts)
2. [Programming NLM](#programming-nlm)
3. [Memory Systems](#memory-systems)
4. [Neuromodulation](#neuromodulation)
5. [Brain Development](#brain-development)
6. [Prediction Systems](#prediction-systems)
7. [Cognitive Mechanisms](#cognitive-mechanisms)
8. [System Integration](#system-integration)
9. [Performance Optimization](#performance-optimization)
10. [Troubleshooting](#troubleshooting)

## Learning Path

This tutorial series is designed to provide a comprehensive learning experience:

### Beginner Level
- Start with the basic examples and tutorials
- Learn fundamental NLM concepts and terminology
- Understand core components and their interactions

### Intermediate Level
- Explore memory systems and neuromodulation
- Study brain development and prediction mechanisms
- Practice system integration and optimization

### Advanced Level
- Master complex interactions and advanced techniques
- Analyze performance and troubleshoot issues
- Create custom environments and applications

## Quick Start Tutorials

For those new to NLM, we recommend starting with the quick start tutorials:

"""
        
        for tutorial in tutorials_data["quick_starts"]:
            content += f"- [{tutorial['name']}]({tutorial['file']})\n"
        
        content += "\n## Interactive Tutorials\n\n"
        content += "The interactive tutorials provide hands-on learning experiences:\n\n"
        
        for tutorial in tutorials_data["interactive"]:
            content += f"- [{tutorial['name']}]({tutorial['file']})\n"
        
        content += "\n## Step-by-Step Guides\n\n"
        content += "These step-by-step guides provide detailed instructions:\n\n"
        
        for tutorial in tutorials_data["step_by_step"]:
            content += f"- [{tutorial['name']}]({tutorial['file']})\n"
        
        content += self.generate_sample_tutorial_content()
        
        # Write to file
        tutorials_file = self.output_dir / "TUTORIALS.md"
        tutorials_file.write_text(content)
    
    def generate_tutorial_chapters(self, tutorials_data: Dict[str, Any]):
        """Generate individual tutorial chapters"""
        # Generate chapter for each main tutorial
        chapters = [
            ("basics", "Basic NLM Concepts", self.generate_basics_chapter()),
            ("programming", "Programming NLM", self.generate_programming_chapter()),
            ("memory", "Memory Systems", self.generate_memory_chapter()),
            ("neuromodulation", "Neuromodulation", self.generate_neuromodulation_chapter()),
            ("development", "Brain Development", self.generate_development_chapter()),
            ("prediction", "Prediction Systems", self.generate_prediction_chapter()),
            ("cognition", "Cognitive Mechanisms", self.generate_cognition_chapter()),
            ("integration", "System Integration", self.generate_integration_chapter()),
            ("performance", "Performance Optimization", self.generate_performance_chapter()),
            ("troubleshooting", "Troubleshooting", self.generate_troubleshooting_chapter())
        ]
        
        for slug, title, content in chapters:
            chapter_file = self.output_dir / f"{slug}.md"
            chapter_file.write_text(content)
    
    def generate_basics_chapter(self) -> str:
        """Generate basics tutorial chapter"""
        return """
# Basic NLM Concepts

## Introduction

This chapter provides an introduction to the fundamental concepts of the NLM neural learning machine system.

## What is NLM?

NLM (熙然) is an experimental artificial developmental brain system that simulates brain-like computation through:

- **Spiking Neural Networks**: Event-driven neural computation
- **Developmental Processes**: Age-dependent learning and maturation
- **Multiple Memory Systems**: Working, episodic, and associative memory
- **Neuromodulation**: Reward-based and curiosity-driven learning
- **Predictive Processing**: Expectation-based inference and learning

## Core Components

The NLM brain consists of several interconnected systems:

1. **Neural Core**: Processing engine with neurons and synapses
2. **Memory Systems**: Working, episodic, and associative memory
3. **Neuromodulation**: Reward, curiosity, and novelty systems
4. **Prediction**: Expectation-based learning
5. **Cognition**: Planning and decision-making
6. **Development**: Age-dependent plasticity

## Key Features

### Spiking Neurons
NLM uses Leaky Integrate-and-Fire (LIF) neuron models:

- **Membrane Potential**: Continuous variable with threshold
- **Refractory Period**: Fixed duration after spike
- **Event-Driven Computation**: Spike-based communication

### Memory Systems
NLM integrates three memory systems:

- **Working Memory**: Active, transient information storage
- **Episodic Memory**: Experience encoding and replay
- **Associative Memory**: Pattern association and recall

### Neuromodulation
Neuromodulators regulate motivation and learning:

- **Dopamine**: Reward prediction and reinforcement
- **Curiosity**: Exploration motivation and novelty detection
- **Novelty**: Sensory novelty detection for exploration

### Development and Plasticity
Developmental processes affect learning throughout the system:

- **Age-dependent plasticity**: Plasticity decreases with development
- **Structural plasticity**: Synaptic rewiring based on experience
- **Maturation**: Neuronal maturation and specialization

## Getting Started

### Installing NLM
```bash
# Clone the repository
git clone https://github.com/neural-learning-machine/nlm.git
cd nlm

# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Running Basic Examples
```bash
# Run Phase 6 integration demo
cd build
./nlm_phase6_demo

# Run basic brain simulation
./examples/ex01_basic_brain

# Run memory systems example
./examples/ex02_memory_systems
```

## Next Steps

1. **Explore Examples**: Study the example programs to understand NLM functionality
2. **Configure Brain**: Create custom brain configurations for your use case
3. **Run Simulations**: Start with simple simulations and gradually increase complexity
4. **Understand Memory**: Learn how memory systems work and interact
5. **Study Neuromodulation**: Explore how neuromodulators affect learning

## Further Reading

- [API Reference](API_REFERENCE.md) - Complete API documentation
- [Tutorials](TUTORIALS.md) - Step-by-step learning guides
- [Best Practices](BEST_PRACTICES.md) - Development best practices
- [Performance Tuning](PERFORMANCE_TUNING.md) - Performance optimization guide
"""
    
    def generate_programming_chapter(self) -> str:
        """Generate programming tutorial chapter"""
        return """
# Programming NLM

## Introduction to NLM Programming

This chapter covers the programming aspects of NLM, including configuration, initialization, and interaction with the brain system.

## Brain Configuration

NLM brains are configured using the `nlm::Config` class. Configuration is done through key-value pairs in JSON-like format.

### Basic Configuration

```cpp
#include <nlm/Brain.hpp>
#include <nlm/Config.hpp>
#include <nlm/Logger.hpp>
#include <iostream>

int main() {
    // Initialize logging
    nlm::Logger::getInstance().setLevel(nlm::LogLevel::INFO);
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.maxNeurons", 100);
    config->set("brain.learningRate", 0.01);
    config->set("brain.simulation.maxSteps", 1000);
    
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // ... rest of program
}
```

### Configuration Options

The NLM configuration system supports the following key categories:

#### Core Brain Settings
- `brain.maxNeurons`: Number of neurons in the brain
- `brain.learningRate`: Learning rate for plasticity
- `brain.simulation.maxSteps`: Maximum simulation steps
- `brain.simulation.timestep`: Simulation time step

#### Memory System Settings
- `brain.memory.workingMemory.enabled`: Enable working memory
- `brain.memory.workingMemory.capacity`: Working memory capacity
- `brain.memory.episodicMemory.enabled`: Enable episodic memory
- `brain.memory.associativeMemory.enabled`: Enable associative memory

#### Neuromodulation Settings
- `brain.neuromodulation.dopamine.enabled`: Enable dopamine system
- `brain.neuromodulation.curiosity.enabled`: Enable curiosity system
- `brain.neuromodulation.novelty.enabled`: Enable novelty detection

#### Development Settings
- `brain.development.enabled`: Enable developmental processes
- `brain.development.plasticityDecayRate`: Plasticity decay rate

## Brain Operations

### Simulation Loop

```cpp
// Main simulation loop
for (nlm::SimulationStep step = 0; step < maxSteps; ++step) {
    // Process sensory input
    brain->step(step);
    
    // Optional: Apply neuromodulation
    if (step % 100 == 0) {
        float reward = calculateReward();
        brain->getDopamine()->processReward(reward, 0.0f);
    }
}
```

### Memory System Access

```cpp
// Access memory systems
auto workingMemory = brain->getWorkingMemory();
auto episodicMemory = brain->getEpisodicMemory();
auto associativeMemory = brain->getAssociativeMemory();

// Store in working memory
std::vector<float> experience = {0.1f, 0.5f, 0.8f};
workingMemory->store(experience);

// Retrieve from working memory
auto retrieved = workingMemory->retrieve();

// Store in episodic memory
auto currentTime = std::chrono::duration<double>::zero();
episodicMemory->encode(experience, currentTime);

// Create associations
std::vector<float> pattern1 = {0.1f, 0.5f, 0.8f};
std::vector<float> pattern2 = {0.9f, 0.2f, 0.4f};
associativeMemory->associate(pattern1, pattern2);
```

### Neuromodulation Access

```cpp
// Access neuromodulation systems
auto dopamine = brain->getDopamine();
auto curiosity = brain->getCuriosity();
auto novelty = brain->getNovelty();

// Process reward
float reward = 1.0f;
dopamine->processReward(reward, 0.0f);

// Process novelty
float noveltyLevel = 0.5f;
curiosity->processNovelty(noveltyLevel, 0.1f);
```

## Advanced Programming

### Event-Driven Programming

NLM uses event-driven programming for efficient neural simulation:

```cpp
void processSensoryInput(const nlm::SensoryInput& input) {
    brain->receiveSensoryInput(input);
}

void applyRewardModulation(float reward, float predictionError) {
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->applyRewardModulation(reward, predictionError);
}
```

### Configuration from File

```cpp
// Load configuration from JSON file
if (!config->loadFromFile("config.json")) {
    std::cerr << "Failed to load configuration!" << std::endl;
    return 1;
}
```

## Error Handling

### Error Checking

```cpp
// Check initialization success
if (!brain->initialize()) {
    std::cerr << "Brain initialization failed!" << std::endl;
    return 1;
}

// Check configuration validity
if (!config->has("brain.maxNeurons")) {
    std::cerr << "Configuration missing required setting: brain.maxNeurons" << std::endl;
    return 1;
}
```

### Exception Handling

```cpp
// Exception handling for brain operations
try {
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    // ... simulation code
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
```

## Best Practices

### Coding Standards

1. **Use smart pointers**: Always use `std::shared_ptr` or `std::unique_ptr` for memory management
2. **Check return values**: Always check the success of operations
3. **Use constants**: Define constants for magic numbers
4. **Log appropriately**: Use the logging system for debugging and monitoring
5. **Handle errors**: Implement proper error handling

### Performance Considerations

1. **Use SIMD**: Enable SIMD optimizations for large networks
2. **Configure memory pools**: Use memory pools for allocation-heavy operations
3. **Optimize configuration**: Set appropriate time steps and neuron counts
4. **Monitor performance**: Track and optimize critical paths

## Next Steps

1. **Study examples**: Examine the example programs to see NLM programming patterns
2. **Practice configuration**: Create custom brain configurations
3. **Experiment with neuromodulation**: Explore how neuromodulators affect learning
4. **Implement memory systems**: Work with different memory system types
5. **Performance tuning**: Optimize your configurations for specific use cases

## Further Reading

- [Configuration Reference](API_REFERENCE.md#configuration) - Complete configuration documentation
- [Examples](EXAMPLES.md) - Working code examples
- [Best Practices](BEST_PRACTICES.md) - Programming best practices
- [Troubleshooting](TROUBLESHOOTING.md) - Common programming issues
"""
    
    def generate_memory_chapter(self) -> str:
        """Generate memory systems tutorial chapter"""
        return "# Memory Systems\n\nContent for memory systems chapter..."
    
    def generate_neuromodulation_chapter(self) -> str:
        """Generate neuromodulation tutorial chapter"""
        return "# Neuromodulation\n\nContent for neuromodulation chapter..."
    
    def generate_development_chapter(self) -> str:
        """Generate development tutorial chapter"""
        return "# Brain Development\n\nContent for development chapter..."
    
    def generate_prediction_chapter(self) -> str:
        """Generate prediction tutorial chapter"""
        return "# Prediction Systems\n\nContent for prediction chapter..."
    
    def generate_cognition_chapter(self) -> str:
        """Generate cognition tutorial chapter"""
        return "# Cognitive Mechanisms\n\nContent for cognition chapter..."
    
    def generate_integration_chapter(self) -> str:
        """Generate integration tutorial chapter"""
        return "# System Integration\n\nContent for integration chapter..."
    
    def generate_performance_chapter(self) -> str:
        """Generate performance tutorial chapter"""
        return "# Performance Optimization\n\nContent for performance chapter..."
    
    def generate_troubleshooting_chapter(self) -> str:
        """Generate troubleshooting tutorial chapter"""
        return "# Troubleshooting\n\nContent for troubleshooting chapter..."
    
    def generate_sample_tutorial_content(self) -> str:
        """Generate sample tutorial content"""
        return """
## Sample Tutorial Content

### Tutorial: Creating Your First NLM Brain

This tutorial will guide you through creating your first NLM neural brain simulation.

#### Step 1: Create a New Project

```bash
mkdir my_nlm_project
cd my_nlm_project

# Create a simple C++ file
nano main.cpp
```

#### Step 2: Write the Basic Code

```cpp
#include <nlm/Brain.hpp>
#include <nlm/Config.hpp>
#include <nlm/Logger.hpp>
#include <iostream>

int main() {
    // Set up logging
    nlm::Logger::getInstance().setLevel(nlm::LogLevel::INFO);
    
    std::cout << "=== NLM Basic Simulation ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.maxNeurons", 50);
    config->set("brain.learningRate", 0.01);
    config->set("brain.simulation.maxSteps", 100);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize
    if (!brain->initialize()) {
        std::cerr << "Brain initialization failed!" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (int step = 0; step < 100; ++step) {
        brain->step(step);
        
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": "
                      << brain->getActiveNeuronCount() << " active neurons" << std::endl;
        }
    }
    
    std::cout << "=== Simulation Complete ===" << std::endl;
    return 0;
}
```

#### Step 3: Build and Run

```bash
# Build using CMake
mkdir build
cd build
cmake ..
make

# Run the simulation
./my_nlm_project
```

#### Step 4: Experiment and Learn

Now try modifying the code:

1. **Change neuron count**: Try `config->set("brain.maxNeurons", 100);`
2. **Adjust learning rate**: Try `config->set("brain.learningRate", 0.1);`
3. **Add memory systems**: Enable `config->set("brain.memory.workingMemory.enabled", true);`
4. **Explore neuromodulation**: Add `config->set("brain.neuromodulation.dopamine.enabled", true);`

Save the file and rebuild to see the effects!

## Resources

- [NLM Documentation](README.md) - Complete documentation
- [API Reference](API_REFERENCE.md) - Detailed API information
- [Examples](EXAMPLES.md) - Working code examples
- [Community Forum](https://forum.neural-learning-machine.org) - Get help from other users
"""

    def generate_interactive_tutorials(self):
        """Generate interactive tutorial content"""
        # This would generate interactive web-based tutorials
        pass


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate NLM tutorials documentation")
    parser.add_argument("--docs-dir", default="docs", help="Documentation directory")
    parser.add_argument("--output-dir", default="docs", help="Output directory for generated documentation")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    
    args = parser.parse_args()
    
    generator = TutorialsDocumentationGenerator(args.docs_dir, args.output_dir)
    generator.generate_tutorials_documentation()
    
    print("Tutorials documentation generation completed successfully!")
