# CONTRIBUTING.md - NLM Contribution Guidelines

This document outlines the guidelines for contributing to the NLM (Neural Learning Machine) project. It provides clear instructions for developers who want to contribute code, documentation, or other improvements to the framework.

---

# Table of Contents

1. [Getting Started](#getting-started)
2. [Code of Conduct](#code-of-conduct)
3. [Development Setup](#development-setup)
4. [Contribution Process](#contribution-process)
5. [Pull Request Guidelines](#pull-request-guidelines)
6. [Code Style and Quality](#code-style-and-quality)
7. [Testing](#testing)
8. [Documentation](#documentation)
9. [Performance Considerations](#performance-considerations)
10. [Common Issues and Solutions](#common-issues-and-solutions)
11. [Version Control](#version-control)
12. [Community Guidelines](#community-guidelines)
13. [License](#license)

---

# Getting Started

## Welcome

Thank you for your interest in contributing to NLM! This project is a sophisticated neural simulation framework designed to create brain-like systems that learn from experience.

Before you begin, please read this guide carefully. It covers the development process, code quality standards, and best practices for contributing to this project.

## How to Use This Guide

This guide provides:

- **Development setup instructions** to get your environment ready
- **Contribution process** explaining how to submit changes
- **Code quality standards** to maintain consistency
- **Testing requirements** to ensure reliability
- **Documentation guidelines** for maintaining clear documentation
- **Community guidelines** for collaboration

## Project Overview

NLM is a research-oriented neural simulation framework with the following characteristics:

- **Phase 6 Complete**: Final integration of all major components
- **C++ Core**: Real spiking neural computation with event-driven dynamics
- **Python Bindings**: Interactive access to NLM functionality
- **Modular Architecture**: 13+ distinct components (neurons, synapses, memory, cognition, etc.)
- **Research Focus**: Incremental biological accuracy, not biological realism claims

## Prerequisites

Before contributing, you should:

- Have a basic understanding of neural networks and computational neuroscience
- Be familiar with C++ and Python programming
- Understand object-oriented design principles
- Be comfortable with Git version control

---

# Code of Conduct

## Our Commitment

We are committed to providing a welcoming, inclusive, and harassment-free environment for all contributors. Everyone is expected to respect the code of conduct in all project communications and activities.

## Expected Behavior

- Use welcoming and inclusive language
- Respect different viewpoints and experiences
- Gracefully accept constructive criticism
- Focus on what is best for the community
- Show respect for different learning styles

## Unacceptable Behavior

- Harassment, discrimination, or intimidation
- Public or private harassment
- Publishing private information of others
- Insults, derogatory comments, or personal attacks
- Unwelcome sexual comments or imagery
- Disruptive or insulting behavior

## Reporting Guidelines

If you experience or witness unacceptable behavior, please report it by:

1. Contacting any of the project maintainers
2. Using the project's issue tracker
3. Reporting to the NLM project team

All reports will be handled confidentially and investigated promptly.

---

# Development Setup

## Initial Setup

### Clone the Repository

```bash
# Clone the NLM repository
git clone https://github.com/nlm-project/nlm.git

# Change into the project directory
cd nlm
```

### Build the Project

```bash
# Make sure you have the required dependencies

# On Ubuntu/Debian:
apt-get update
apt-get install -y build-essential cmake python3-dev python3-pip python3-venv

# On macOS:
# Install Xcode Command Line Tools
# Install Homebrew and then:
brew install cmake python3

# On Windows:
# Install Visual Studio Build Tools
# Install Python from python.org

# Build the project
./setup.sh
```

### Verify Installation

```bash
# Run the validation script
python3 validate_setup.py

# Test basic functionality
nlm run --steps 100

# Run examples
python3 examples.py basic

# Test development tools
python3 dev_tools.py validate-all
```

## Development Workflow

### Environment Setup

```bash
# Create and activate virtual environment
python3 -m venv nlm_env
source nlm_env/bin/activate  # On Windows: nlm_env\Scripts\activate

# Install development dependencies
pip install -e .
pip install pytest pytest-cov pytest-xdist numpy matplotlib scipy
```

### Running Tests

```bash
# Run all tests
pytest tests/

# Run tests with coverage
pytest --cov=nlm --cov-report=html tests/

# Run specific test modules
pytest tests/test_brain.py
pytest tests/test_memory.py

# Run tests in parallel
pytest -n auto tests/
```

### Building from Source

```bash
# If the Python bindings need to be rebuilt
pip install --force-reinstall -e .

# For manual C++ build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

---

# Contribution Process

## Step 1: Create an Issue

Before starting work on a feature or bug fix, please create an issue in the repository. This helps us:

- Understand what you're planning to do
- Provide feedback early in the development process
- Coordinate with other contributors
- Track planned work

### Issue Templates

When creating an issue, please use the appropriate template:

#### Bug Report Template
- Describe the problem
- Provide reproduction steps
- Include error messages or stack traces
- Explain the expected behavior

#### Feature Request Template
- Describe the proposed feature
- Explain the motivation and use case
- Provide examples of expected behavior
- Consider alternatives and trade-offs

#### Enhancement Request Template
- Describe the enhancement
- Explain its value to the project
- Provide implementation suggestions
- Consider performance implications

## Step 2: Create a Branch

```bash
# Fetch and checkout the base branch
git fetch origin

git checkout develop

# Create and checkout a new branch
git checkout -b feature/your-feature-name
```

### Branch Naming Convention

- **Bug fixes**: `fix/`, `bug/`
- **Features**: `feature/`, `feat/`
- **Documentation**: `docs/`, `documentation/`
- **Performance**: `perf/`, `performance/`
- **Tests**: `test/`, `testing/`
- **Experiments**: `experiment/`, `experiment/`

## Step 3: Implement Your Changes

### Coding Standards

Follow the existing code style and patterns:

1. **Use consistent indentation (4 spaces)**
2. **Include headers for new files**
3. **Follow existing naming conventions**
4. **Add comments for complex logic**
5. **Maintain comprehensive documentation**

### File Structure

When adding new functionality, consider the existing structure:

```
src/
├── brain/                    # Core brain components
├── memory/                   # Memory systems
├── cognition/                # Cognitive systems
├── plasticity/               # Plasticity rules
├── development/              # Development systems
├── neuromodulation/           # Neuromodulation
├── sensory/                  # Sensory processing
├── motor/                    # Motor system
├── environment/              # Environment interface
├── visualization/            # Visualization components
├── performance/              # Performance optimizations
└── core/                     # Core utilities

python/                      # Python bindings

configs/                     # Configuration files

docs/                       # Documentation

tests/                       # Test suite
```

## Step 4: Test Your Changes

### Unit Testing

```python
# Add new tests to existing test files
# Follow existing test patterns and conventions

# Example test structure
import pytest
from pynlm import createBrain, createDefaultConfig

def test_brain_creation():
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0

def test_brain_step():
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    initial_spikes = brain.getTotalSpikeCount()
    brain.step(0)
    final_spikes = brain.getTotalSpikeCount()
    
    assert final_spikes >= initial_spikes
```

### Integration Testing

```bash
# Run integration tests
pytest tests/integration/
```

## Step 5: Commit Your Changes

```bash
# Add files to staging
git add <file1> <file2> ...

# Commit changes with descriptive message
git commit -m "<type>(<scope>): <description>

# Optional: amend previous commit
git commit --amend --no-edit

# Optional: stage all changes
git add -A
```

### Commit Message Guidelines

Follow conventional commit format:

```
<type>(<scope>): <description>

<optional body with details>

<optional footer with co-authored-by: openhands <openhands@all-hands.dev>]
```

**Types:**
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting, etc.)
- `refactor`: Code refactoring
- `perf`: Performance improvements
- `test`: Adding tests
- `chore`: Maintenance tasks

**Examples:**
```
feat(memory): add working memory system
fix(brain): handle neuron death in simulation
feat(cognition): implement concept formation
fix(config): resolve configuration loading
```

### Co-authored-by

When contributing to this project, please add the following to your commit messages:

```
co-authored-by: openhands <openhands@all-hands.dev>
```

## Step 6: Push and Create a Pull Request

```bash
# Push to your branch
git push origin feature/your-feature-name

# Create a pull request on GitHub
# Follow the template provided by the system
# Include relevant information:
# - Description of changes
# - Screenshots or examples (if applicable)
# - Testing performed
# - Performance impact (if any)
```

### Pull Request Checklist

- [ ] All tests pass
- [ ] Code follows project conventions
- [ ] Documentation is updated
- [ ] Code reviews completed
- [ ] Performance requirements met
- [ ] No breaking changes

---

# Pull Request Guidelines

## Pre-Submission Checklist

Before submitting a pull request, ensure:

1. **Code Quality**
   - Code follows project conventions
   - Appropriate comments and documentation
   - Consistent naming and formatting
   - Handles edge cases appropriately

2. **Testing**
   - All existing tests pass
   - New tests added for new functionality
   - Integration tests included if applicable
   - Performance tests run (if relevant)

3. **Documentation**
   - Updated relevant documentation
   - Added examples if new features
   - Updated API documentation if needed

4. **Code Review**
   - Reviewed by at least one maintainer
   - Add Co-authored-by attribution
   - Address any feedback

## Review Process

1. **Initial Review**: Automated checks and initial assessment
2. **Code Review**: Technical review by maintainers
3. **Testing Review**: Verification of test coverage and quality
4. **Documentation Review**: Assessment of documentation completeness
5. **Final Approval**: Merge decision and integration

## Pull Request Template

```markdown
## Summary
[Brief description of changes]

## Changes Made
- [ ] Added new feature X
- [ ] Fixed bug Y
- [ ] Updated documentation Z
- [ ] Improved performance

## Testing Performed
- [ ] All unit tests pass
- [ ] Integration tests completed
- [ ] Performance benchmarks run
- [ ] Example scripts tested

## Breaking Changes
- [ ] No breaking changes
- [ ] Documented breaking changes

## Performance Impact
- [ ] Minimal impact
- [ ] Significant improvement
- [ ] Performance degradation
- [ ] No performance impact measured

## Documentation
- [ ] Updated API documentation
- [ ] Added usage examples
- [ ] Updated README
- [ ] Added comments in code

## Co-authored-by
- openhands <openhands@all-hands.dev>
```

---

# Code Style and Quality

## Coding Standards

### Python Style

- **Indentation**: 4 spaces per level
- **Line length**: Maximum 100 characters
- **Import order**: Standard library, third-party, local
- **Naming conventions**: snake_case for functions, variables, methods
- **Docstrings**: Google style or NumPy style

### C++ Style

- **Indentation**: 2 spaces per level
- **Line length**: Maximum 80 characters
- **Naming conventions**: CamelCase for classes, snake_case for functions
- **Comments**: Block comments for headers, line comments for inline explanations

### Documentation

- **API Documentation**: Use Doxygen for C++ and docstrings for Python
- **User Documentation**: Maintain clear, comprehensive guides
- **Comments**: Explain complex logic and edge cases

## Code Quality Checks

### Static Analysis

```bash
# Run linters
cppcheck src/
pylint python/
flake8 .

# Check for style issues
clang-format --dry-run src/
black --check python/
```

### Performance Analysis

```bash
# Profile code execution
python3 -m cProfile -o profile.stats examples.py all
python3 perf_analysis/profile_analyzer.py analyze profile.stats

# Check for performance regressions
python3 perf_analysis/benchmark.py --suite --compare
```

### Memory Analysis

```bash
# Check for memory leaks
valgrind --leak-check=full ./nlm run --steps 100
python3 -m memory_profiler python/script.py
```

---

# Testing

## Testing Strategy

NLM uses a comprehensive testing strategy that includes:

1. **Unit Tests**: Test individual components
2. **Integration Tests**: Test component interactions
3. **Performance Tests**: Test performance characteristics
4. **Example Tests**: Test user examples
5. **Regression Tests**: Test for regressions

## Unit Tests

```python
# Unit test structure
test_brain.py
    test_brain_creation()
    test_brain_initialization()
    test_brain_step()
    test_brain_statistics()

test_memory.py
    test_working_memory()
    test_episodic_memory()
    test_associative_memory()

test_plasticity.py
    test_hebbian_learning()
    test_stdp_learning()
    test_structural_plasticity()
```

## Integration Tests

```python
# Integration test structure
test_sensory_motor_loop.py
    test_brain_world_interaction()
    test_agent_behavior()
    test_reward_learning()

test_development_pipeline.py
    test_brain_development()
    test_plasticity_during_development()
    test_memory_consolidation()
```

## Performance Tests

```bash
# Run performance benchmarks
python3 perf_analysis/benchmark.py --suite --save benchmark_results.json
```

## Example Tests

```bash
# Run example tests
python3 examples.py all
python3 -m pytest tests/test_examples.py -v
```

## Test Coverage Requirements

- **Unit Tests**: Minimum 80% coverage
- **Integration Tests**: Minimum 70% coverage
- **Performance Tests**: 100% test execution
- **Example Tests**: All examples run successfully

## Test Reporting

```bash
# Generate test reports
pytest --cov=nlm --cov-report=html --cov-report=term tests/

# Generate JUnit XML reports for CI
pytest --junitxml=test-results.xml tests/

# Run tests with multiple cores
pytest -n auto --tb=short tests/
```

---

# Documentation

## Documentation Structure

```
docs/
├── ARCHITECTURE.md          # Technical architecture
├── SCIENCE.md               # Scientific principles
├── ROADMAP.md               # Development roadmap
├── EXPERIMENTS.md           # Experiment framework
├── PERFORMANCE.md           # Performance analysis
├── PHASE6_FINAL_AUDIT.md    # Phase 6 audit report
└── CONTRIBUTING.md          # Contribution guidelines (this file)

# User documentation
README.md                   # Basic usage
HOW_TO_USE.md               # Complete user guide (this file)
examples.py                 # Examples and tutorials
```

## Documentation Guidelines

### Writing Style

- **Clarity**: Write for the intended audience
- **Brevity**: Be concise but thorough
- **Structure**: Use clear headings and formatting
- **Examples**: Include practical examples
- **References**: Link to related documentation

### Documentation Types

1. **Technical Documentation**: Implementation details, APIs, architecture
2. **User Documentation**: How-to guides, tutorials, examples
3. **Research Documentation**: Scientific background, methodology
4. **Development Documentation**: Contribution guidelines, development setup

### Documentation Generation

```bash
# Generate documentation from docstrings
pydoc -o docs/api python/pynlm.py

# Build documentation with Sphinx
make -C docs html
```

## Example Documentation Structure

```markdown
# [Feature/Topic Name]

## Overview
[Brief introduction]

## Usage
```python
# Code example
```

## Examples
[Link to example scripts]

## API Reference
[Method descriptions]

## Related Topics
[Links to related documentation]

## Common Issues
[Problem and solution]
```

---

# Performance Considerations

## Performance Optimization

### Simulation Performance

```python
# Optimize simulation performance

# 1. Use appropriate brain size
brain = pynlm.createBrain(config)
if config.get("neuron_count") > 1000:
    config.set("plasticity_enabled", False)

# 2. Control simulation complexity
for step in range(steps):
    # Early termination for long simulations
    if step % 100 == 0 and should_checkpoint():
        brain.save(f"checkpoint_step_{step}.brain")

# 3. Memory management
import gc
if step % 50 == 0:
    gc.collect()
```

### Memory Optimization

```python
# Reduce memory usage

# 1. Use checkpointing for long simulations
brain.save("checkpoint.brain")

# 2. Force garbage collection periodically
import gc
for step in range(1000):
    brain.step(step)
    if step % 50 == 0:
        gc.collect()

# 3. Use memory-efficient data structures
# Prefer arrays over lists for numerical data
# Use generators for large datasets
```

### Scalability Considerations

```python
# Design for scalability

def run_scalable_simulation(steps, num_threads=4):
    # Split work across threads
    chunk_size = steps // num_threads
    
    with ThreadPoolExecutor(max_workers=num_threads) as executor:
        futures = [
            executor.submit(
                process_chunk, 
                i * chunk_size, 
                (i + 1) * chunk_size
            )
            for i in range(num_threads)
        ]
        
        # Wait for all threads to complete
        for future in futures:
            future.result()
```

## Performance Testing

```bash
# Run performance benchmarks
python3 perf_analysis/benchmark.py --suite --save benchmark_results.json

# Compare performance across versions
python3 perf_analysis/benchmark.py --compare --baseline baseline.json

# Profile specific operations
python3 -m cProfile -o operation_profile.prof script.py
```

---

# Common Issues and Solutions

## Issue 1: Configuration Loading Failures

**Problem**: Configuration files fail to load or parse.

**Solution**:

```python
import pynlm

# Debug configuration loading
config = pynlm.createDefaultConfig()
filepath = "configs/experimental.json"

if not os.path.exists(filepath):
    print(f"Configuration file not found: {filepath}")
    # Create default config
    config = pynlm.createDefaultConfig()
else:
    # Try loading with error handling
    if config.loadFromFile(filepath):
        print(f"Configuration loaded from {filepath}")
    else:
        print(f"Failed to load {filepath}")
        print("Using default configuration")
        config = pynlm.createDefaultConfig()
        
        # Manually set important values
        config.set("neuron_count", 1000)
        config.set("learning_rate", 0.01)
```

## Issue 2: Brain Initialization Problems

**Problem**: Brain fails to initialize or has zero neurons.

**Solution**:

```python
import pynlm

# Robust brain creation
brain = None
max_attempts = 3
attempt_delay = 0.1  # seconds

for attempt in range(max_attempts):
    try:
        print(f"Brain initialization attempt {attempt + 1}/{max_attempts}")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Verify initialization
        if brain.getTotalNeuronCount() > 0:
            print(f"Success: Created brain with {brain.getTotalNeuronCount()} neurons")
            break
        else:
            print("Warning: Brain created with zero neurons")
            
    except Exception as e:
        print(f"Attempt {attempt + 1} failed: {e}")
        if attempt < max_attempts - 1:
            import time
            time.sleep(attempt_delay)

if brain is None:
    print("All brain initialization attempts failed")
    print("Please check system resources and try again")
```

## Issue 3: Memory Issues in Long Simulations

**Problem**: High memory usage or memory exhaustion during long simulations.

**Solutions**:

### Solution A: Checkpointing

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation with checkpointing
segment_length = 500
total_steps = 10000

for start_step in range(0, total_steps, segment_length):
    end_step = min(start_step + segment_length, total_steps)
    
    print(f"Running simulation steps {start_step}-{end_step}")
    
    for step in range(start_step, end_step):
        brain.step(step)
    
    # Save checkpoint
    checkpoint_file = f"checkpoint_step_{start_step}.brain"
    brain.save(checkpoint_file)
    print(f"Checkpoint saved: {checkpoint_file}")
```

### Solution B: Memory Monitoring

```python
import pynlm
import gc
import psutil
import os

def get_memory_usage():
    """Get current memory usage in MB."""
    process = psutil.Process(os.getpid())
    return process.memory_info().rss / 1024 / 1024

print(f"Initial memory usage: {get_memory_usage():.1f} MB")

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

max_memory = 1000  # MB
checkpoint_interval = 100
memory_checkpoint_interval = 50

for step in range(5000):
    brain.step(step)
    
    # Memory management
    if step % checkpoint_interval == 0:
        checkpoint_file = f"checkpoint_step_{step}.brain"
        brain.save(checkpoint_file)
        print(f"Checkpoint saved: {checkpoint_file}")
    
    if step % memory_checkpoint_interval == 0:
        current_memory = get_memory_usage()
        print(f"Step {step}: Memory usage: {current_memory:.1f} MB")
        
        # Force garbage collection if memory is high
        if current_memory > max_memory:
            print(f"Memory limit exceeded ({current_memory:.1f} MB), forcing GC...")
            collected = gc.collect()
            print(f"After GC: {get_memory_usage():.1f} MB ({collected} objects collected)")

print(f"Final memory usage: {get_memory_usage():.1f} MB")
```

## Issue 4: Performance Problems

**Problem**: Simulation performance is slower than expected.

**Solutions**:

### Solution A: Configuration Optimization

```python
import pynlm

# Optimize configuration for performance
config = pynlm.createDefaultConfig()

# Reduce brain size for better performance
config.set("neuron_count", 500)

# Disable expensive features
config.set("plasticity_enabled", False)
config.set("development_enabled", False)
config.set("novelty_threshold", 0.5)  # Reduce novelty detection

brain = pynlm.createBrain(config)
brain.initialize()
```

### Solution B: Parallel Processing

```python
import pynlm
from concurrent.futures import ThreadPoolExecutor
import time

class ParallelSimulator:
    def __init__(self, steps, num_threads=4):
        self.steps = steps
        self.num_threads = num_threads
        
    def run_thread(self, thread_id, steps_per_thread):
        """Run simulation for a thread."""
        results = []
        
        for i in range(steps_per_thread):
            global_step = thread_id * steps_per_thread + i
            
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            for step in range(steps_per_thread):
                brain.step(step)
            
            results.append({
                'thread_id': thread_id,
                'steps_completed': steps_per_thread,
                'neurons': brain.getTotalNeuronCount(),
                'spikes': brain.getTotalSpikeCount()
            })
        
        return results
    
    def run_parallel(self):
        """Run simulation in parallel."""
        steps_per_thread = self.steps // self.num_threads
        
        with ThreadPoolExecutor(max_workers=self.num_threads) as executor:
            futures = [
                executor.submit(self.run_thread, i, steps_per_thread)
                for i in range(self.num_threads)
            ]
            
            all_results = []
            for future in futures:
                all_results.extend(future.result())
        
        return all_results

# Run parallel simulation
simulator = ParallelSimulator(steps=1000, num_threads=4)
results = simulator.run_parallel()

# Analyze results
print(f"Parallel simulation completed {sum(r['steps_completed'] for r in results)} steps")
total_neurons = sum(r['neurons'] for r in results)
total_spikes = sum(r['spikes'] for r in results)
print(f"Total neurons created: {total_neurons}")
print(f"Total spikes generated: {total_spikes}")
```

---

# Version Control

## Branch Strategy

NLM uses Git for version control with the following branch strategy:

### Main Branches

- **develop**: Development branch containing the latest features
- **master**: Production branch (if used)
- **gh-pages**: GitHub Pages for documentation

### Feature Branches

- **feature/**: New features
- **fix/**: Bug fixes
- **docs/**: Documentation updates
- **perf/**: Performance improvements
- **test/**: Test improvements

## Branching Model

1. **Create feature branch** from `develop`
2. **Implement feature** in feature branch
3. **Test changes** thoroughly
4. **Create pull request** to `develop`
5. **Code review** and merge
6. **Deploy to production** if needed

## Git Workflow

```bash
# Start with develop branch
git checkout develop

# Create and checkout feature branch
git checkout -b feature/new-feature

# Make changes
# ... your work ...

# Test changes
pytest tests/

# Commit changes
git add .
git commit -m "feat(new-feature): add new functionality"

# Push to remote repository
git push origin feature/new-feature

# Create pull request on GitHub
# Wait for review and merge
```

## Merge Conflicts

When merging, use the following strategy:

1. **Rebase** your branch onto the latest `develop`
2. **Resolve conflicts** by reviewing and merging changes
3. **Test thoroughly** after conflict resolution
4. **Commit resolved conflicts**

```bash
# Rebase feature branch onto develop
git checkout feature/your-feature-name
git rebase develop

# Resolve any conflicts
# Test your changes
# Continue rebasing if needed
```

---

# Community Guidelines

## Collaboration

NLM is a collaborative project. Effective collaboration requires:

1. **Clear Communication**: Be clear and concise in your communication
2. **Respectful Interaction**: Treat all contributors with respect
3. **Open Mind**: Be open to different ideas and approaches
4. **Constructive Feedback**: Provide helpful and actionable feedback
5. **Patience**: Understand that development takes time

## Code Review

Effective code review:

1. **Review Thoroughly**: Check code for correctness, style, and best practices
2. **Provide Feedback**: Offer specific, actionable feedback
3. **Be Positive**: Focus on improving the code, not criticizing the author
4. **Be Timely**: Review requests within reasonable timeframes
5. **Document Decisions**: Document why certain changes were approved or rejected

## Mentoring

Experienced contributors should:

1. **Welcome New Contributors**: Be helpful and patient with new contributors
2. **Share Knowledge**: Document your knowledge and processes
3. **Guide Development**: Provide guidance on project standards and practices
4. **Encourage Growth**: Support the development of new skills

## Diversity and Inclusion

NLM welcomes contributions from everyone regardless of:

- Background
- Identity
- Experience level
- Technical expertise
- Perspective

We strive to create an inclusive environment where all contributors can thrive.

---

# License

## Project License

NLM is licensed under the MIT License. This license permits use, modification, distribution, and private use of the software, subject to certain conditions.

## License Terms

```text
MIT License (MIT)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
```

## Copyright

Copyright © 2024-2026 NLM Project Contributors

## Attribution

When using NLM in your research or projects, please cite:

```
@software{nlm_project_2026,
    author = {NLM Project Contributors},
    title = {NLM (Neural Learning Machine)},
    year = {2026},
    publisher = {GitHub},
    journal = {GitHub repository},
    howpublished = {\url{https://github.com/nlm-project/nlm}}
}
```

---

# Index

- [Getting Started](#getting-started)
- [Code of Conduct](#code-of-conduct)
- [Development Setup](#development-setup)
- [Contribution Process](#contribution-process)
- [Pull Request Guidelines](#pull-request-guidelines)
- [Code Style and Quality](#code-style-and-quality)
- [Testing](#testing)
- [Documentation](#documentation)
- [Performance Considerations](#performance-considerations)
- [Common Issues and Solutions](#common-issues-and-solutions)
- [Version Control](#version-control)
- [Community Guidelines](#community-guidelines)
- [License](#license)
