#!/usr/bin/env python3
"""
NLM - Documentation and Tutorial System

This module provides comprehensive documentation generation and tutorial
functionality for NLM. It includes:
1. API documentation generation
2. Tutorial system with interactive examples
3. Code examples and demonstrations
4. Best practices and common patterns
"""

import os
import json
from typing import Dict, List, Any, Optional
from datetime import datetime
import markdown

class NLMDocumentation:
    """
    Generate documentation for NLM.
    """
    
    def __init__(self, docs_dir: str = "docs"):
        self.docs_dir = docs_dir
        self.api_reference = {}
        self.tutorials = []
        self.examples = []
        self.best_practices = []
    
    def add_api_reference(self, module: str, functions: List[Dict[str, Any]]):
        """Add API reference for a module."""
        self.api_reference[module] = {
            'functions': functions,
            'last_updated': datetime.now().isoformat(),
        }
    
    def add_tutorial(self, title: str, content: str, difficulty: str = "beginner"):
        """Add a tutorial."""
        self.tutorials.append({
            'title': title,
            'content': content,
            'difficulty': difficulty,
            'created': datetime.now().isoformat(),
        })
    
    def add_example(self, name: str, code: str, description: str, category: str):
        """Add a code example."""
        self.examples.append({
            'name': name,
            'code': code,
            'description': description,
            'category': category,
            'created': datetime.now().isoformat(),
        })
    
    def add_best_practice(self, title: str, content: str, category: str):
        """Add a best practice."""
        self.best_practices.append({
            'title': title,
            'content': content,
            'category': category,
            'created': datetime.now().isoformat(),
        })
    
    def generate_api_docs(self, output_file: str = "API.md") -> str:
        """Generate API documentation."""
        content = "# NLM API Reference\n\n"
        
        for module, info in self.api_reference.items():
            content += f"## {module}\n\n"
            
            for func in info['functions']:
                content += f"### {func['name']}\n\n"
                content += f"{func.get('description', '')}\n\n"
                
                if 'parameters' in func:
                    content += "**Parameters:**\n"
                    for param in func['parameters']:
                        content += f"- `{param['name']}` ({param.get('type', 'any')}): {param.get('description', '')}\n"
                    content += "\n"
                
                if 'returns' in func:
                    content += f"**Returns:** {func['returns']}\n\n"
                
                if 'example' in func:
                    content += "**Example:**\n```python\n" + func['example'] + "\n```\n\n"
            
            content += "---\n\n"
        
        # Write to file
        with open(os.path.join(self.docs_dir, output_file), 'w') as f:
            f.write(content)
        
        return content
    
    def generate_tutorials(self, output_file: str = "Tutorials.md") -> str:
        """Generate tutorials documentation."""
        content = "# NLM Tutorials\n\n"
        
        for tutorial in self.tutorials:
            content += f"## {tutorial['title']}\n\n"
            content += f"**Difficulty:** {tutorial['difficulty']}\n\n"
            content += f"{tutorial['content']}\n\n"
            content += "---\n\n"
        
        # Write to file
        with open(os.path.join(self.docs_dir, output_file), 'w') as f:
            f.write(content)
        
        return content
    
    def generate_examples(self, output_file: str = "Examples.md") -> str:
        """Generate examples documentation."""
        content = "# NLM Examples\n\n"
        
        for category in ['basic', 'intermediate', 'advanced']:
            category_examples = [ex for ex in self.examples if ex['category'] == category]
            
            if category_examples:
                content += f"## {category.title()} Examples\n\n"
                
                for example in category_examples:
                    content += f"### {example['name']}\n\n"
                    content += f"{example['description']}\n\n"
                    content += f"```python\n{example['code']}\n```\n\n"
                    content += "---\n\n"
        
        # Write to file
        with open(os.path.join(self.docs_dir, output_file), 'w') as f:
            f.write(content)
        
        return content
    
    def generate_best_practices(self, output_file: str = "BestPractices.md") -> str:
        """Generate best practices documentation."""
        content = "# NLM Best Practices\n\n"
        
        for category in ['performance', 'memory', 'learning', 'simulation']:
            category_practices = [bp for bp in self.best_practices if bp['category'] == category]
            
            if category_practices:
                content += f"## {category.title()} Best Practices\n\n"
                
                for practice in category_practices:
                    content += f"### {practice['title']}\n\n"
                    content += f"{practice['content']}\n\n"
                    content += "---\n\n"
        
        # Write to file
        with open(os.path.join(self.docs_dir, output_file), 'w') as f:
            f.write(content)
        
        return content
    
    def generate_full_documentation(self):
        """Generate all documentation files."""
        os.makedirs(self.docs_dir, exist_ok=True)
        
        self.generate_api_docs()
        self.generate_tutorials()
        self.generate_examples()
        self.generate_best_practices()
        
        print(f"Documentation generated in {self.docs_dir}/")

class NMLTutorialSystem:
    """
    Tutorial system for NLM users.
    """
    
    def __init__(self):
        self.lessons = []
        self.current_lesson = None
    
    def add_lesson(self, title: str, content: str, code_examples: List[str] = None, 
                   quiz: List[Dict[str, Any]] = None):
        """Add a lesson to the tutorial system."""
        self.lessons.append({
            'title': title,
            'content': content,
            'code_examples': code_examples or [],
            'quiz': quiz or [],
            'completed': False,
        })
    
    def get_lesson(self, lesson_index: int):
        """Get a specific lesson."""
        if 0 <= lesson_index < len(self.lessons):
            self.current_lesson = self.lessons[lesson_index]
            return self.lessons[lesson_index]
        return None
    
    def complete_lesson(self, lesson_index: int):
        """Mark a lesson as completed."""
        if 0 <= lesson_index < len(self.lessons):
            self.lessons[lesson_index]['completed'] = True
            self.current_lesson = None
    
    def get_progress_report(self) -> Dict[str, Any]:
        """Get progress report."""
        total_lessons = len(self.lessons)
        completed_lessons = sum(1 for lesson in self.lessons if lesson['completed'])
        
        return {
            'total_lessons': total_lessons,
            'completed_lessons': completed_lessons,
            'completion_percentage': (completed_lessons / total_lessons) * 100 if total_lessons > 0 else 0,
            'current_lesson': self.current_lesson['title'] if self.current_lesson else None,
        }
    
    def generate_tutorial_summary(self) -> str:
        """Generate a summary of all tutorials."""
        content = "# NLM Tutorial Summary\n\n"
        
        for lesson in self.lessons:
            status = "✓" if lesson['completed'] else "○"
            content += f"{status} {lesson['title']} ({lesson.get('difficulty', 'unknown')})\n"
        
        return content

def create_beginner_tutorial():
    """Create beginner tutorial for NLM."""
    tutorial = NMLTutorialSystem()
    
    # Lesson 1: Getting Started
    tutorial.add_lesson(
        title="Getting Started with NLM",
        content="""
## Introduction

NLM (Neural Learning Machine) is a computational brain simulation system that implements biological neural networks.

## What You'll Learn

1. Creating your first brain
2. Understanding basic brain operations
3. Running simple simulations
4. Exploring brain statistics

## Your First Brain

Let's create a simple brain using NLM:

```python
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simple simulation
for i in range(100):
    brain.step(i)

print("Done!")
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```
""",
        code_examples=[
            """
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(10):
    brain.step(i)
"""
        ]
    )
    
    # Lesson 2: Brain Statistics
    tutorial.add_lesson(
        title="Understanding Brain Statistics",
        content="""
## Brain Statistics

NLM provides various statistics to understand brain activity:

- Total neurons: How many neurons in the brain
- Total synapses: How many connections between neurons
- Firing neurons: How many neurons are currently active
- Average firing rate: How fast neurons are firing
- Total spikes: Total number of action potentials

## Getting Statistics

```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get all statistics at once
stats = brain.getStats()
print(f"Neurons: {stats['total_neurons']}")
print(f"Synapses: {stats['total_synapses']}")
print(f"Firing rate: {stats['avg_firing_rate']:.3f}")
print(f"Total spikes: {stats['total_spikes']}")
```
""",
        code_examples=[
            """
stats = brain.getStats()
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Active neurons: {stats['active_neurons']}")
print(f"Development stage: {stats['dev_stage']}")
"""
        ]
    )
    
    # Lesson 3: Working with Worlds
    tutorial.add_lesson(
        title="Working with Worlds",
        content="""
## Worlds in NLM

A "world" represents the environment in which your brain operates. It provides:

- Spatial environment
- Sensory inputs
- Actions and effects
- Rewards and punishments

## Creating a World

```python
import pynlm

# Create brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(100):
    world.update(0.1)  # Update world
    agent.processSensoryInput(world.getSensoryPercept())  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action
```
""",
        code_examples=[
            """
# Configure world
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Set agent starting position
world.setAgentStart(10.0, 10.0)
"""
        ]
    )
    
    return tutorial

def create_advanced_tutorial():
    """Create advanced tutorial for NLM."""
    tutorial = NMLTutorialSystem()
    
    # Lesson 1: Learning Systems
    tutorial.add_lesson(
        title="Learning Systems in NLM",
        content="""
## Neuromodulation

NLM implements several learning mechanisms through neuromodulators:

- **Dopamine**: Reward prediction error, reinforces successful actions
- **Curiosity**: Exploration behavior, seeks novel experiences
- **Novelty**: Detects new or surprising stimuli

## Enabling Learning

```python
import pynlm

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)  # Learn from rewards
agent.enableCuriosity(True)         # Explore new things
agent.enableDevelopment(True)       # Brain matures over time
agent.enableStructuralPlasticity(True)  # Grow new connections
```
""",
        code_examples=[
            """
# Apply reward modulation
reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
agent.applyRewardModulation(reward, 0.0)

# Update development
agent.updateDevelopment(0.1)
"""
        ]
    )
    
    # Lesson 2: Development Stages
    tutorial.add_lesson(
        title="Brain Development",
        content="""
## Developmental Stages

Brains go through several developmental stages:

1. **Initial**: Primitive neural connections
2. **Critical Period**: High plasticity, rapid learning
3. **Maturation**: Fine-tuning connections, stabilizing
4. **Adult**: Efficient operation, continued learning
5. **Aging**: Gradual decline in plasticity

## Working with Development

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check current developmental stage
current_stage = brain.getDevelopmentalStage()
print(f"Current stage: {current_stage}")

# Change developmental stage
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
```
""",
        code_examples=[
            """
# Development affects plasticity rates
if brain.getDevelopmentalStage() == pynlm.DevelopmentalStage.CriticalPeriod:
    print("High learning potential!")
elif brain.getDevelopmentalStage() == pynlm.DevelopmentalStage.Adult:
    print("Stable, efficient operation.")
"""
        ]
    )
    
    return tutorial

def generate_documentation():
    """Generate comprehensive NLM documentation."""
    print("Generating NLM Documentation...")
    
    # Create documentation directory
    os.makedirs("docs", exist_ok=True)
    
    # Generate beginner tutorial
    beginner_tutorial = create_beginner_tutorial()
    
    # Generate advanced tutorial  
    advanced_tutorial = create_advanced_tutorial()
    
    # Write tutorial summaries
    with open("docs/BEGINNER_TUTORIAL.md", "w") as f:
        f.write(beginner_tutorial.generate_tutorial_summary())
    
    with open("docs/ADVANCED_TUTORIAL.md", "w") as f:
        f.write(advanced_tutorial.generate_tutorial_summary())
    
    # Create examples directory
    os.makedirs("examples", exist_ok=True)
    
    # Write example files
    with open("examples/basics.py", "w") as f:
        f.write("""
# Basic NLM examples
import pynlm

# Simple brain example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(10):
    brain.step(i)
print(f"Simple brain: {brain.getTotalNeuronCount()} neurons")
""")
    
    with open("examples/learning.py", "w") as f:
        f.write("""
# Learning NLM examples
import pynlm

# Learning agent example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(10, 10, 8, 8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

for step in range(50):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
""")
    
    print("Documentation generated in docs/ and examples/ directories")
    print("\nKey files:")
    print("- docs/BEGINNER_TUTORIAL.md - Step-by-step tutorials for beginners")
    print("- docs/ADVANCED_TUTORIAL.md - Advanced topics for experienced users")
    print("- examples/basic.py - Basic NLM usage examples")
    print("- examples/learning.py - Learning and development examples")

if __name__ == "__main__":
    generate_documentation()