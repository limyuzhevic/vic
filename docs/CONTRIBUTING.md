# NLM Framework Contributing Guidelines

## Overview

Welcome to the NLM (熙然) framework contributing guidelines. NLM is an experimental artificial developmental brain project focused on creating neural systems that learn from experience through brain-inspired architectures. This document provides clear guidelines for external contributors to help advance our scientific research goals.

## 1. Code of Conduct and Community Guidelines

### Our Core Values

- **Scientific Integrity**: All contributions must adhere to rigorous scientific standards
- **Open Collaboration**: We welcome diverse perspectives and encourage constructive dialogue
- **Reproducibility**: Code and results must be reproducible and well-documented
- **Respectful Discourse**: We maintain a professional and supportive environment for all contributors

### Expected Behavior

- Treat all contributors with respect and courtesy
- Focus on technical merit and scientific validity
- Provide constructive feedback with specific, actionable suggestions
- Share knowledge and mentor newcomers when possible
- Cite all external resources and build upon existing work

### Prohibited Behavior

- Harassment, discrimination, or personal attacks
- Trolling or disruptive behavior
- Submission of plagiarized code
- Ignoring established peer review processes
- Providing misinformation about research findings

## 2. Contribution Guidelines and Workflow

### Getting Started

1. **Read the Project Overview**: Understand the NLM project's goals and current Phase 6 integration
2. **Review Documentation**: Study the architecture and scientific foundations in `docs/`
3. **Join Discussions**: Participate in project forums or Slack channels (if available)
4. **Identify Opportunities**: Look for open issues marked "good first issue" or "help wanted"

### Contribution Process

1. **Fork the Repository**: Create your own copy of the NLM framework
2. **Create Feature Branch**: Use `feature/` or `research/` prefix for new features
3. **Implement Changes**: Follow NLM's coding style and add comprehensive tests
4. **Document Work**: Update relevant documentation with clear explanations
5. **Request Review**: Submit a pull request with detailed description of changes

### Branching Strategy

- `main`: Production-ready code (current Phase 6)
- `feature/<name>`: New features or experiments
- `research/<name>`: Research prototypes and exploratory work
- `bugfix/<name>`: Bug fixes and patches
- `docs/<name>`: Documentation updates

## 3. Issue Reporting Process

### When to Report Issues

Report issues for:
- Bug reports in existing functionality
- Scientific discrepancies or errors
- Missing or incorrect documentation
- Performance or scalability problems
- Integration failures between components

### Issue Template

Use the following template when reporting issues:

```
## Issue Type
[Bug] [Question] [Enhancement] [Documentation]

## Description
[Clear, concise description of the problem]

## Reproduction Steps
1. [Step 1]
2. [Step 2]
3. [Step 3]

## Expected vs Actual Behavior
- Expected: [what should happen]
- Actual: [what actually happens]

## Environment
- NLM Version: [version number]
- Platform: [OS, architecture]
- Build Configuration: [compile flags, settings]

## Additional Context
[Any additional information, screenshots, or code snippets]
```

### Scientific Rigor

For scientific issues:
- Include detailed reproduction steps
- Provide expected vs. actual results
- Share any relevant configuration files
- Document the experimental setup
- Note any assumptions made

## 4. Pull Request Process

### Pre-PR Checklist

- [ ] All tests pass
- [ ] Code follows NLM style guidelines
- [ ] New functionality has comprehensive tests
- [ ] Documentation is updated (if applicable)
- [ ] Scientific claims are properly documented
- [ ] No breaking changes without deprecation notices

### PR Standards

**Title Format**: `[Component] Brief description of changes`

Examples:
- `[Memory] Fix synaptic weight decay bug`
- `[Documentation] Add experimental setup guide`
- `[Integration] Improve neuromodulation connection`

**Description Content**:
- Clear motivation for changes
- Technical details of implementation
- Scientific significance and validation
- Any known limitations or trade-offs
- References to related issues or papers

### Review Process

1. **Automated Checks**: CI runs linting, type checking, and tests
2. **Peer Review**: At least two maintainers review the changes
3. **Scientific Review**: Research-oriented changes require additional scrutiny
4. **Approval**: PR must be approved by core team members

## 5. Code Review Standards

### Code Quality

- **Performance**: Ensure no performance regressions
- **Scientific Validity**: Verify all claims are properly supported
- **Reproducibility**: Code should include all necessary setup details
- **Maintainability**: Write clean, well-documented code

### Code Structure

```cpp
// Example NLM coding conventions

// Neural network parameters should be documented
/**
 * @class NeuronParameters
 * @brief Configuration parameters for neural units
 * 
 * @param v_thresh Threshold potential for spike initiation (mV)
 * @param tau_mem Membrane time constant (ms)
 * @param v_rest Resting potential (mV)
 */
struct NeuronParameters {
    double v_thresh = -50.0;
    double tau_mem = 20.0;
    double v_rest = -70.0;
    // ... other parameters
};

// Use meaningful variable names for neural components
// avoid single-letter variables except in mathematical contexts
NeuronNetwork brain_network("IntegrationBrain", neuron_params);

// Document experimental conditions and assumptions
/**
 * @brief Simulates neural development over specified time period
 * 
 * @param duration Total simulation time in developmental stages
 * @param plasticity_rate Rate of synaptic modification during development
 * 
 * @note This implementation assumes gradual maturation of neural circuits
 */
void simulateDevelopment(int duration, double plasticity_rate);
```

### Testing Requirements

- **Unit Tests**: Every function and class must have corresponding tests
- **Integration Tests**: Verify connections between different systems
- **Performance Tests**: Validate computational efficiency claims
- **Regression Tests**: Ensure existing functionality remains intact

## 6. Development Setup for Contributors

### Prerequisites

```bash
# Required system packages
apt-get install build-essential cmake git python3 python3-pip ninja-build

# Additional scientific computing packages (optional)
# For experimental work requiring simulation tools:
# apt-get install python3-numpy python3-scipy python3-matplotlib
```

### Building from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake (adjust paths as needed)
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNLM_PHASE=6 \
         -DNLM_PYTHON_BINDINGS=ON \
         -DNLM_TESTS=ON

# Build the framework
make -j4

# Run tests
ctest --output-on-failure
```

### Python API Development

```python
# Install Python development dependencies
pip install -r requirements-dev.txt

# Set up development environment
export PYTHONPATH=/path/to/nlm/src/python:$PYTHONPATH

# Run Python-specific tests
python -m pytest tests/python/ -v

# Test the Python bindings
python -c "import pynlm; print(pynlm.__version__)"
```

### Experimental Setup

For research-oriented contributions:
1. Create a separate experimental branch
2. Document all experimental conditions
3. Include reproducibility scripts
4. Share any custom datasets or parameters

## 7. Documentation Standards

### Documentation Structure

- **Technical Documentation**: In `docs/` directory following Markdown
- **API Documentation**: Generated with Doxygen for C++, docstrings for Python
- **Research Papers**: Separate documentation for scientific publications
- **Tutorials**: Step-by-step guides for new contributors

### Documentation Requirements

**For Code Changes**:
- Document public APIs with examples
- Include parameter descriptions and types
- Provide use cases and limitations
- Add performance characteristics if relevant

**For Scientific Claims**:
- Include experimental setup details
- Document methodology and assumptions
- Provide statistical validation when applicable
- Reference related work and literature

**Example Documentation Style**:

```markdown
## Neural Integration Module

### Overview

The neural integration module combines prediction systems with cognitive planning...

### Key Features

- **Prediction Integration**: Error signals drive plasticity
- **Cognitive Planning**: Action selection based on neural evaluations

### API

#### createIntegrationBrain(config)
```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("integration.prediction_weight", 0.7)
brain = pynlm.createIntegrationBrain(config)
```

#### Limitations

- Current implementation assumes linear prediction weights
- Non-stationary environments may reduce accuracy
```

## 8. Success Metrics and Recognition

### Contribution Impact

#### Technical Contributions
- Number and severity of bugs fixed
- Performance improvements quantified
- New features with comprehensive testing
- Code quality improvements (linting, type checking)

#### Research Contributions
- Novel scientific insights discovered
- Validated against existing literature
- New experimental setups established
- Reproducible results shared

#### Documentation Contributions
- New tutorials or guides created
- Existing documentation improved
- API documentation completed
- Examples added for complex features

### Recognition System

#### GitHub Credits
- **Top Contributors**: Recognized in project newsletters
- **Bug Fixers**: Issues closed with attribution
- **Documentarians**: Documentation improvements highlighted

#### Publication Credits
- **Co-authorship**: Significant research contributions may lead to
  co-authorship on papers
- **Conference Presentations**: Notable findings may be presented at
  neuroscience conferences

#### Awards
- **NLM Pioneer Award**: For foundational contributions
- **Integration Award**: For successful system integration work
- **Research Excellence Award**: For groundbreaking scientific discoveries

### Contributing to Research

For contributions involving scientific research:
- Publish findings in appropriate venues
- Share datasets and experimental code
- Reproduce existing results to validate claims
- Engage with the broader neuroscience community

## Appendix: Troubleshooting Common Issues

### Build Errors

**Problem**: CMake cannot find required libraries

**Solution**: Install missing dependencies:
```bash
sudo apt-get install libjsoncpp-dev libboost-system-dev libboost-filesystem-dev
```

### Python Bindings Issues

**Problem**: Cannot import pynlm module

**Solution**:
```bash
export PYTHONPATH=/workspace/nlm/src/python:$PYTHONPATH
pip install -e .[dev]
```

### Performance Issues

**Problem**: Simulation runs slowly

**Solution**: Profile with `perf` or `gprof` and optimize critical paths.
Document performance improvements with before/after benchmarks.
