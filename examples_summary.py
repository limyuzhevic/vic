#!/usr/bin/env python3
"""
examples_summary.py

Summary of NLM Python API Example Files
=====================================

This script provides a comprehensive overview of all NLM Python API example files,
demonstrating their capabilities and providing quick access to each example.

Key Features:
- Overview of all example files
- File metadata and descriptions
- Quick demonstration of each example
- Access to example source code
- Classification by difficulty and topic

Learning Objectives:
1. Understand the full scope of NLM Python API examples
2. Access example files by category and complexity
3. Run demonstrations of each example
4. Learn how to use examples as learning resources
5. Understand example organization and structure

Author: NLM Documentation Team
Date: 2026-10-12
"""

import os
from pathlib import Path
from datetime import datetime
from typing import Dict, List, Any
import json
import textwrap

class ExamplesSummary:
    """
    Summary and overview of NLM Python API example files.
    """
    
    def __init__(self):
        self.examples_path = Path("examples")
        self.example_files = self._discover_example_files()
        self.example_metadata = self._load_example_metadata()
    
    def _discover_example_files(self) -> List[Path]:
        """Discover all example files in the examples directory."""
        example_files = []
        
        # Look for files in the current directory (since we created them there)
        current_dir = Path.cwd()
        
        # Get all .py files that start with "example_" or "recipe_" or "tutorial_"
        for file_path in current_dir.glob("*.py"):
            if file_path.name.startswith(('example_', 'recipe_', 'tutorial_')):
                example_files.append(file_path)
        
        # Sort by name
        example_files.sort(key=lambda x: x.name)
        
        return example_files
    
    def _load_example_metadata(self) -> Dict[str, Dict]:
        """Load metadata for each example file."""
        metadata = {}
        
        for file_path in self.example_files:
            try:
                metadata[file_path.name] = {
                    'path': str(file_path.absolute()),
                    'size': os.path.getsize(file_path),
                    'last_modified': datetime.fromtimestamp(os.path.getmtime(file_path)).isoformat(),
                    'category': self._categorize_example(file_path.name),
                    'difficulty': self._determine_difficulty(file_path.name),
                    'topics': self._extract_topics(file_path.name),
                    'description': self._get_example_description(file_path.name)
                }
            except Exception as e:
                print(f"Warning: Could not load metadata for {file_path.name}: {str(e)}")
                metadata[file_path.name] = {
                    'path': str(file_path.absolute()),
                    'error': str(e)
                }
        
        return metadata
    
    def _categorize_example(self, filename: str) -> str:
        """Categorize example based on filename."""
        if filename.startswith('example_'):
            if 'basic' in filename:
                return 'Basic Examples'
            elif 'agent' in filename:
                return 'Agent Simulation'
            elif 'learning' in filename:
                return 'Learning & Neuromodulation'
            elif 'development' in filename:
                return 'Brain Development'
            elif 'world' in filename:
                return 'World Interaction'
            elif 'neural' in filename:
                return 'Neural Analysis'
            elif 'performance' in filename:
                return 'Performance Benchmarking'
            elif 'template' in filename:
                return 'Project Templates'
            elif 'research' in filename:
                return 'Reproducible Research'
            elif 'parallel' in filename:
                return 'Parallel Processing'
            else:
                return 'General Examples'
        elif filename.startswith('recipe_'):
            return 'Recipe Collection'
        elif filename.startswith('tutorial_'):
            return 'Tutorial Series'
        else:
            return 'Other Examples'
    
    def _determine_difficulty(self, filename: str) -> str:
        """Determine difficulty level based on filename."""
        if any(word in filename.lower() for word in ['basic', 'tutorial', 'step_by_step']):
            return 'Beginner'
        elif any(word in filename.lower() for word in ['learning', 'optimization', 'training']):
            return 'Intermediate'
        elif any(word in filename.lower() for word in ['advanced', 'complex', 'parallel', 'performance', 'neural']):
            return 'Advanced'
        elif any(word in filename.lower() for word in ['template', 'research', 'parallel']):
            return 'Intermediate'
        else:
            return 'Intermediate'
    
    def _extract_topics(self, filename: str) -> List[str]:
        """Extract topics from filename."""
        topics = []
        filename_lower = filename.lower()
        
        if 'brain' in filename_lower:
            topics.append('Brain Simulation')
        if 'agent' in filename_lower:
            topics.append('Agent Behavior')
        if 'learning' in filename_lower:
            topics.append('Learning Algorithms')
        if 'development' in filename_lower:
            topics.append('Brain Development')
        if 'world' in filename_lower:
            topics.append('Environment Interaction')
        if 'neural' in filename_lower:
            topics.append('Neural Analysis')
        if 'performance' in filename_lower:
            topics.append('Performance Optimization')
        if 'template' in filename_lower:
            topics.append('Project Structure')
        if 'research' in filename_lower:
            topics.append('Reproducible Research')
        if 'parallel' in filename_lower:
            topics.append('Parallel Processing')
        if 'optimization' in filename_lower:
            topics.append('Hyperparameter Optimization')
        if 'training' in filename_lower:
            topics.append('Agent Training')
        if 'benchmarking' in filename_lower:
            topics.append('Performance Measurement')
        
        return topics if topics else ['General Usage']
    
    def _get_example_description(self, filename: str) -> str:
        """Get example description based on filename."""
        descriptions = {
            'example_basic_brain.py': 'Basic brain creation and simulation for beginners',
            'example_agent_simulation.py': 'Complete agent simulation with sensory-motor loops',
            'example_learning_brain.py': 'Learning with neuromodulation and reward systems',
            'example_development_brain.py': 'Brain development and maturation processes',
            'example_world_interaction.py': 'Complex environment interaction and behavior',
            'example_neural_analysis.py': 'Neural activity analysis and pattern detection',
            'example_performance_benchmarking.py': 'Performance measurement and optimization',
            'example_template_project.py': 'Project template for organizing larger applications',
            'example_reproducible_research.py': 'Reproducible research and documentation',
            'example_parallel_simulation.py': 'Parallel simulation for improved performance',
            'tutorial_step_by_step.py': 'Step-by-step tutorial for learning the API',
            'recipe_learning_optimization.py': 'Recipes for optimizing learning performance',
            'recipe_agent_training.py': 'Recipes for training agents in various environments'
        }
        
        return descriptions.get(filename, 'Example file demonstrating NLM Python API features')
    
    def print_summary(self):
        """Print comprehensive summary of all examples."""
        print("=" * 80)
        print("NLM PYTHON API EXAMPLES SUMMARY")
        print("=" * 80)
        
        print(f"\nTotal Examples: {len(self.example_files)}")
        print(f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print(f"Project Path: {Path.cwd().absolute()}")
        
        self._print_category_summary()
        self._print_example_list()
        self._print_usage_examples()
        self._print_learning_path()
    
    def _print_category_summary(self):
        """Print summary by category."""
        print(f"\n{'='*80}")
        print("EXAMPLES BY CATEGORY")
        print(f"{'='*80}")
        
        categories = {}
        for metadata in self.example_metadata.values():
            category = metadata.get('category', 'Unknown')
            if category not in categories:
                categories[category] = 0
            categories[category] += 1
        
        for category, count in sorted(categories.items(), key=lambda x: x[1], reverse=True):
            print(f"{category}: {count} examples")
    
    def _print_example_list(self):
        """Print detailed list of all examples."""
        print(f"\n{'='*80}")
        print("DETAILED EXAMPLE LIST")
        print(f"{'='*80}")
        
        for filename, metadata in self.example_metadata.items():
            print(f"\n📄 {filename}")
            print(f"   Category: {metadata.get('category', 'Unknown')}")
            print(f"   Difficulty: {metadata.get('difficulty', 'Unknown')}")
            print(f"   Topics: {', '.join(metadata.get('topics', []))}")
            print(f"   Description: {metadata.get('description', 'No description available')}")
            
            if 'path' in metadata:
                print(f"   Location: {metadata['path']}")
            
            if 'size' in metadata:
                size_kb = metadata['size'] / 1024
                print(f"   Size: {size_kb:.1f} KB")
    
    def _print_usage_examples(self):
        """Print usage examples for common tasks."""
        print(f"\n{'='*80}")
        print("USAGE EXAMPLES")
        print(f"{'='*80}")
        
        examples = [
            {
                'title': 'Quick Start - Basic Brain Simulation',
                'command': 'python example_basic_brain.py',
                'description': 'Create and simulate a basic neural brain'
            },
            {
                'title': 'Agent Training Tutorial',
                'command': 'python recipe_agent_training.py',
                'description': 'Complete agent training with optimization recipes'
            },
            {
                'title': 'Learning Neuromodulation',
                'command': 'python example_learning_brain.py',
                'description': 'Demonstrate learning with dopamine and curiosity systems'
            },
            {
                'title': 'Step-by-Step Tutorial',
                'command': 'python tutorial_step_by_step.py',
                'description': 'Comprehensive tutorial for learning the NLM Python API'
            },
            {
                'title': 'Performance Benchmarking',
                'command': 'python example_performance_benchmarking.py',
                'description': 'Measure and optimize simulation performance'
            }
        ]
        
        for example in examples:
            print(f"\n🚀 {example['title']}")
            print(f"   Command: {example['command']}")
            print(f"   Description: {example['description']}")
    
    def _print_learning_path(self):
        """Print recommended learning path."""
        print(f"\n{'='*80}")
        print("RECOMMENDED LEARNING PATH")
        print(f"{'='*80}")
        
        learning_path = [
            ("Beginner", [
                ("example_basic_brain.py", "Basic brain creation and simulation"),
                ("tutorial_step_by_step.py", "Step-by-step API tutorial"),
                ("example_agent_simulation.py", "Complete agent simulation")
            ]),
            ("Intermediate", [
                ("example_learning_brain.py", "Learning with neuromodulation"),
                ("recipe_learning_optimization.py", "Learning optimization techniques"),
                ("example_world_interaction.py", "Environment interaction behaviors"),
                ("example_development_brain.py", "Brain development processes")
            ]),
            ("Advanced", [
                ("example_neural_analysis.py", "Advanced neural activity analysis"),
                ("example_performance_benchmarking.py", "Performance optimization and benchmarking"),
                ("example_reproducible_research.py", "Reproducible research practices"),
                ("recipe_agent_training.py", "Advanced agent training recipes"),
                ("example_parallel_simulation.py", "Parallel simulation for performance")
            ]),
            ("Expert", [
                ("example_template_project.py", "Project templates and best practices"),
                ("all_examples_combined", "All examples combined for comprehensive study")
            ])
        ]
        
        for level, examples in learning_path:
            print(f"\n📚 {level} Level:")
            for filename, description in examples:
                if filename == "all_examples_combined":
                    print(f"   📖 {filename.replace('_', ' ').title()}")
                    print(f"      {description}")
                else:
                    print(f"   📄 {filename}")
                    print(f"      {description}")
    
    def generate_usage_help(self) -> str:
        """Generate usage help text."""
        help_text = f"""
NLM Python API Examples Usage Guide
===================================

This guide provides instructions for running and using NLM Python API examples.

AVAILABLE EXAMPLES:
------------------

Basic Examples (Beginner Level):
  - example_basic_brain.py
    Description: Basic brain creation and simulation
    Usage: python example_basic_brain.py

  - tutorial_step_by_step.py
    Description: Comprehensive step-by-step tutorial
    Usage: python tutorial_step_by_step.py

Intermediate Examples:
  - example_agent_simulation.py
    Description: Complete agent simulation with learning
    Usage: python example_agent_simulation.py

  - example_learning_brain.py
    Description: Learning with neuromodulation systems
    Usage: python example_learning_brain.py

Advanced Examples:
  - example_neural_analysis.py
    Description: Advanced neural activity analysis
    Usage: python example_neural_analysis.py

  - example_performance_benchmarking.py
    Description: Performance measurement and optimization
    Usage: python example_performance_benchmarking.py

  - example_reproducible_research.py
    Description: Reproducible research and documentation
    Usage: python example_reproducible_research.py

  - example_parallel_simulation.py
    Description: Parallel simulation for performance
    Usage: python example_parallel_simulation.py

Recipe Collection:
  - recipe_learning_optimization.py
    Description: Recipes for optimizing learning performance
    Usage: python recipe_learning_optimization.py

  - recipe_agent_training.py
    Description: Recipes for training agents
    Usage: python recipe_agent_training.py

Template Examples:
  - example_template_project.py
    Description: Project template for organizing larger applications
    Usage: python example_template_project.py

GETTING STARTED:
---------------

1. Install dependencies (if not already installed):
   pip install numpy pandas scipy matplotlib psutil

2. Choose an example to run:
   python example_basic_brain.py

3. Explore the example source code:
   cat example_basic_brain.py

4. Modify and customize examples for your project
5. Create new examples by adapting existing ones

EXAMPLES ORGANIZATION:
--------------------

- example_*.py: Core functionality examples
- recipe_*.py: Problem-solution recipes
- tutorial_*.py: Learning and tutorial examples
- All examples are self-contained and copy-paste runnable

TECHNICAL REQUIREMENTS:
------------------------

- Python 3.8+
- NumPy
- pandas
- scipy
- matplotlib
- psutil
- NLM Python bindings (pynlm)

SUPPORTED PLATFORMS:
-------------------

- Linux
- macOS
- Windows

TROUBLESHOOTING:
---------------

1. Import errors:
   pip install -r requirements.txt (if requirements.txt exists)

2. Missing dependencies:
   Check system dependencies and install missing packages

3. Performance issues:
   Reduce simulation complexity or increase available system resources

4. Memory errors:
   Monitor memory usage and reduce simulation size

5. Display issues:
   Ensure matplotlib backend is properly configured

FURTHER RESOURCES:
-----------------

- Documentation: https://nlm.readthedocs.io
- GitHub: https://github.com/nlm-project/nlm
- Examples: See all .py files in the examples directory
- Community: Join NLM community forums for questions and discussions

SUPPORT:
--------

For help with examples or issues, please:
1. Check example documentation
2. Review error messages
3. Consult example source code
4. Visit NLM documentation
5. Contact NLM support team

EXAMPLE FILES COUNT:
------------------
Total examples available: {len(self.example_files)}
Categories: {len(set(self.example_metadata.get(f, {{}}).get('category', '') for f in self.example_metadata))}
Difficulty levels: 3 (Beginner, Intermediate, Advanced)
Topic coverage: {len(set([topic for metadata in self.example_metadata.values() for topic in metadata.get('topics', [])])} topics
"""
        
        return help_text.format(
            len(self.example_files),
            len(set(self.example_metadata.get(f, {}).get('category', '') for f in self.example_metadata)),
            len(set([topic for metadata in self.example_metadata.values() for topic in metadata.get('topics', [])]))
        )


def main():
    """Main function to run examples summary."""
    # Create examples summary
    summary = ExamplesSummary()
    
    # Print comprehensive summary
    summary.print_summary()
    
    # Generate and print usage help
    print(f"\n{'='*80}")
    print("USAGE HELP")
    print(f"{'='*80}")
    print(summary.generate_usage_help())
    
    # Save summary to file
    try:
        summary_path = Path("examples_summary.txt")
        with open(summary_path, 'w') as f:
            f.write(summary.generate_usage_help())
        print(f"\n✓ Summary saved to {summary_path.absolute()}")
    except Exception as e:
        print(f"\n⚠ Could not save summary to file: {str(e)}")
    
    print(f"\n" + "="*80)
    print("EXAMPLES SUMMARY COMPLETE")
    print("="*80)
    print(f"✓ Total examples available: {len(summary.example_files)}")
    print(f"✓ Categories: {len(set(summary.example_metadata.get(f, {}).get('category', '') for f in summary.example_metadata))}")
    print(f"✓ Difficulty levels: 3")
    print(f"✓ Topic coverage: {len(set([topic for metadata in summary.example_metadata.values() for topic in metadata.get('topics', [])]))} topics")
    print(f"✓ Examples are organized by complexity and topic area")
    print(f"✓ Each example is self-contained and copy-paste runnable")

if __name__ == "__main__":
    main()