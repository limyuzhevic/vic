# NLM Documentation - Complete Project Overview

**Neural Learning Machine - Documentation Index**

## Overview

This document provides a comprehensive overview of the NLM (Neural Learning Machine) project documentation structure and references. NLM is an experimental artificial developmental brain project that simulates spiking neural networks with learning capabilities.

## Documentation Structure

### Primary Documentation Files

| File | Purpose | Location | Last Updated |
|------|---------|----------|---------------|
| **README.md** | Project overview, build instructions, API reference | Root | Current |
| **easy_usage.md** | Python beginner guide | Root | Current |
| **HOW_TO_USE.md** | Comprehensive usage guide | Root | Current |
| **docs/ARCHITECTURE.md** | System architecture overview | docs/ | Research |
| **docs/SCIENCE.md** | Scientific background | docs/ | Research |
| **docs/EXPERIMENTS.md** | Experiment descriptions | docs/ | Research |
| **docs/LIMITATIONS.md** | Current limitations | docs/ | Research |
| **docs/PHASE6_FINAL_AUDIT.md** | Final integration audit | docs/ | Research |

### Module-Specific Documentation

#### Python Bindings Documentation
- **python/README.md** - Complete API reference and examples for Python bindings
- **python/bindings.cpp** - Python binding implementation source

#### C++ Core Documentation
- **docs/ARCHITECTURE.md** - C++ system architecture
- **docs/SCIENCE.md** - Scientific principles implemented
- **docs/LIMITATIONS.md** - Technical limitations and constraints

## Documentation Access Paths

### For C++ Developers

#### Building and Installation
```bash
# Build C++ libraries and executables
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run tests
ctest --output-on-failure

# Install (optional)
make install
```

#### Key Build Outputs
- `nlm` - Main simulation executable
- `nlm_core` - Core brain components library
- `nlm_agent` - Agent system library
- `nlm_world` - World simulation library
- `nlm_phase3_demo`, `nlm_phase4_demo` - Phase demonstrations
- `nlm_test` - Unit test suite

#### Running Examples
```bash
# Phase 2: Real neural computation
./nlm

# Phase 3: Basic world interaction
./nlm_phase3_demo

# Phase 4: Advanced agent demo
./nlm_phase4_demo

# Phase 6: Full integration experiment
./nlm_phase6_demo
```

### For Python Users

#### Installation
```bash
# Install Python bindings
pip install pybind11 scikit-build-core pytest numpy
pip install -e .

# Verify installation
python -c "import pynlm; print('NLM version:', pynlm.__version__)"
```

#### Quick Start Guide
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

print(f"Simulation complete: {brain.getTotalNeuronCount()} neurons")
```

#### Complete Examples
```python
# Environment interaction
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

## Cross-References and Navigation

### From README.md
- References: easy_usage.md, HOW_TO_USE.md
- References: docs/ARCHITECTURE.md, docs/SCIENCE.md
- References: python/README.md (Python bindings)

### From easy_usage.md
- References: HOW_TO_USE.md
- References: python/README.md

### From HOW_TO_USE.md
- References: docs/ARCHITECTURE.md
- References: docs/SCIENCE.md
- References: docs/EXPERIMENTS.md
- References: docs/LIMITATIONS.md
- References: python/README.md

### From docs/ARCHITECTURE.md
- References: pyproject.toml (build configuration)
- References: CMakeLists.txt (build system)
- References: python/README.md (Python bindings)

### From docs/SCIENCE.md
- References: src/ (implementation source)
- References: docs/ARCHITECTURE.md (system overview)

### From docs/EXPERIMENTS.md
- References: src/experiments/ (experiment implementations)
- References: docs/PHASE6_FINAL_AUDIT.md (integration results)

## Project Organization

### Core Components

#### Source Code Structure
```
src/
├── core/           # Core utilities and types
├── brain/          # Neural components (neurons, synapses, populations)
├── memory/         # Memory systems (working, episodic, semantic, procedural, associative)
├── neuromodulation/ # Neuromodulators (dopamine, curiosity, novelty, prediction error)
├── cognition/      # Cognitive mechanisms (neural planner, concept formation, attention, planning)
├── sensory/        # Sensory processing (vision, audio, internal signals)
├── motor/          # Motor system
├── prediction/     # Prediction systems (neural prediction, confidence tracking)
├── development/    # Developmental system (synaptogenesis, pruning, maturation)
├── plasticity/     # Plasticity rules (STDP, Hebbian, structural plasticity)
├── environment/    # Environment interface
├── agent/          # Agent system (sensory percept, agent-brain interface)
├── world/          # World simulation (simple world)
├── experiments/    # Experiment framework (Phase 3-6)
└── visualization/   # Visualization interface
```

#### Python Bindings
```
python/
├── bindings.cpp        # Complete pybind11 bindings
└── CMakeLists.txt      # Python bindings build
```

#### Configuration
```
configs/
└── default.cfg         # Default configuration
```

#### Documentation
```
docs/
├── ARCHITECTURE.md      # System architecture
├── SCIENCE.md           # Scientific background
├── ROADMAP.md           # Development roadmap
├── EXPERIMENTS.md       # Experiment descriptions
├── LIMITATIONS.md       # Current limitations
└── PHASE6_FINAL_AUDIT.md # Final integration audit
```

### Build System

#### C++ Build
- **CMakeLists.txt** - Main build configuration
- **scikit-build-core** - Python package build system
- **pybind11** - Python bindings generation

#### Python Build
- **pyproject.toml** - Modern Python package configuration
- **setup.py** - Legacy setup (for compatibility)
- **requirements.txt** - Development dependencies

### Testing

#### Test Structure
```
tests/
├── test_brain.py          # Brain functionality tests
├── test_agent.py          # Agent system tests
├── test_world.py          # World simulation tests
├── test_memory.py         # Memory system tests
├── test_plasticity.py     # Plasticity rule tests
├── test_neuromodulation.py # Neuromodulation tests
└── test_integration.py    # Integration tests
```

## Usage Guidelines

### For Developers

#### Adding New Features
1. **C++ Extensions**: Add new source files in appropriate `src/` subdirectories
2. **Python Extensions**: Extend `python/bindings.cpp`
3. **Configuration**: Add new keys to `configs/default.cfg`
4. **Documentation**: Update relevant documentation files
5. **Testing**: Add new test files to `tests/`

#### Modifying Existing Code
1. **Follow existing coding style** in the codebase
2. **Maintain backward compatibility** where possible
3. **Add comprehensive comments** for new functionality
4. **Update documentation** when changing public APIs
5. **Add unit tests** for modified functionality

### For Researchers

#### Conducting Experiments
1. **Read `docs/EXPERIMENTS.md`** for experiment descriptions
2. **Use `src/experiments/`** for experiment implementations
3. **Configure experiments** using `configs/default.cfg`
4. **Document results** in research notes
5. **Share findings** with the research community

#### Understanding the System
1. **Study `docs/ARCHITECTURE.md`** for system architecture
2. **Review `docs/SCIENCE.md`** for scientific background
3. **Examine `src/`** for implementation details
4. **Run `docs/PHASE6_FINAL_AUDIT.md`** for integration verification
5. **Use Python examples** for rapid prototyping

### For Educators

#### Teaching NLM
1. **Use `easy_usage.md`** for beginner-friendly introduction
2. **Use `HOW_TO_USE.md`** for comprehensive course material
3. **Use `docs/ARCHITECTURE.md`** for system architecture teaching
4. **Use `docs/SCIENCE.md`** for scientific principles
5. **Use `docs/EXPERIMENTS.md`** for laboratory exercises

#### Creating Tutorials
1. **Start with simple examples** in `easy_usage.md`
2. **Build to complex examples** in `HOW_TO_USE.md`
3. **Use Python bindings** for accessible teaching
4. **Include C++ examples** for performance considerations
5. **Provide exercises** that reinforce concepts

## Version Control and Dependencies

### Git Repository
- **Main branch**: Development and stable releases
- **Tags**: Version releases (v0.1.0, v0.2.0, etc.)
- **Documentation**: Version-controlled documentation
- **Configuration**: Version-controlled configuration files

### Dependencies
- **C++20 Compiler** - Modern C++ features
- **CMake 3.16+** - Build system
- **Python 3.8+** - Python bindings
- **pybind11 2.11+** - Python bindings library
- **scikit-build-core** - Python package build system
- **pytest** - Test framework (Python)
- **numpy** - Numerical computing (optional)

## Troubleshooting and Support

### Common Issues

#### Build Issues
**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

#### Runtime Issues
**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

### Support Resources

#### Documentation
- **README.md** - Quick start and overview
- **easy_usage.md** - Python beginner guide
- **HOW_TO_USE.md** - Comprehensive reference
- **docs/ARCHITECTURE.md** - Technical architecture

#### Code Examples
- **src/experiments/Phase6IntegratedExperiment.cpp** - Full integration example
- **python/bindings.cpp** - Python API implementation
- **easy_usage.md** - Beginner Python examples

#### Research Resources
- **docs/SCIENCE.md** - Scientific background
- **docs/EXPERIMENTS.md** - Experiment protocols
- **docs/LIMITATIONS.md** - Current limitations
- **docs/PHASE6_FINAL_AUDIT.md** - Integration verification

## Future Documentation

### Planned Documentation Improvements

1. **API Documentation**
   - Auto-generated API documentation
   - Detailed parameter descriptions
   - Return value explanations

2. **Tutorial Series**
   - Step-by-step tutorials
   - Progressive complexity
   - Interactive examples

3. **Research Papers**
   - Scientific publications
   - Methodology documentation
   - Results and analysis

4. **User Guide**
   - Best practices
   - Performance optimization
   - Troubleshooting guide
   - FAQ section

5. **Developer Reference**
   - Code contribution guidelines
   - Build system documentation
   - Testing procedures
   - Extension development

## Documentation Index

### Quick Reference Guide
- **[README.md](#)** - Project overview and quick start
- **[easy_usage.md](#)** - Python beginner guide
- **[HOW_TO_USE.md](#)** - Comprehensive usage guide
- **[docs/ARCHITECTURE.md](#)** - System architecture
- **[docs/SCIENCE.md](#)** - Scientific background
- **[docs/EXPERIMENTS.md](#)** - Experiment descriptions
- **[docs/LIMITATIONS.md](#)** - Current limitations
- **[python/README.md](#)** - Python bindings documentation

### Development Guide
- **Build Instructions** - CMake and Python installation
- **Code Style** - Programming conventions
- **Testing Procedures** - Running and adding tests
- **Extension Development** - Adding new features

### Research Guide
- **Experiment Design** - Creating and running experiments
- **Data Collection** - Recording simulation results
- **Analysis Methods** - Interpreting results
- **Reporting** - Documenting findings

## Legal and Attribution

### License
MIT License

### Attribution
This project is part of the NLM research initiative. For detailed attribution and acknowledgments, see `docs/SCIENCE.md` and individual source files.

### Copyright
Copyright (c) 2024 NLM Research Project

## Contact and Community

### For Questions
1. **GitHub Issues** - Bug reports and feature requests
2. **Documentation** - FAQ and troubleshooting
3. **Research Community** - Scientific discussions

### For Collaboration
1. **Code Repository** - Pull requests and contributions
2. **Documentation** - Contributing improvements
3. **Research** - Collaborative research projects

## Conclusion

This documentation provides comprehensive coverage of the NLM project, from beginner tutorials to advanced developer references. The documentation is organized to support multiple user groups:

- **Beginners**: Start with `easy_usage.md`
- **Users**: Refer to `HOW_TO_USE.md`
- **Developers**: Consult `README.md` and `docs/ARCHITECTURE.md`
- **Researchers**: Use `docs/SCIENCE.md` and `docs/EXPERIMENTS.md`
- **Educators**: Utilize teaching guides and tutorial series

The documentation is continuously updated as NLM evolves, reflecting new features, improvements, and research findings.

---

**Documentation Version**: 1.0
**Last Updated**: $(date)
**Status**: Active Documentation
**Maintainers**: NLM Development Team