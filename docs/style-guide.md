# Documentation Style Guide

## Overview

This document provides guidelines and standards for creating and maintaining documentation throughout the NLM (熙然) project. Consistent documentation ensures clarity, maintainability, and professional quality across all project materials.

## Documentation Structure

### Required Files

All documentation must follow this hierarchical structure:

```
docs/
├── README.md                    # Project overview and quick start
├── ARCHITECTURE.md              # System architecture and components
├── CONTRIBUTING.md              # Contribution guidelines
├── DEVELOPING.md                # Development procedures
├── RUNNING.md                   # How to run experiments
├── user-manual.md               # Complete user manual
├── tutorial/
│   ├── basic.md                # Basic tutorial
│   └── advanced.md             # Advanced tutorial
├── command-reference.md          # CLI command documentation
├── configuration-guide.md        # Configuration parameters
├── examples.md                  # Example experiments and configs
├── developer-guide.md            # Development procedures
├── api-reference/               # Auto-generated API docs
│   └── index.md
├── technical/
│   ├── api-reference.md         # Complete API documentation
│   ├── configuration-reference.md # Full parameter documentation
│   └── architecture-deep-dive.md # Detailed component docs
├── reference/
│   ├── mathematical-models.md    # LIF neurons, plasticity rules, etc.
│   ├── algorithms.md            # Algorithm descriptions
│   ├── benchmarks.md            # Performance benchmarks
│   ├── research-background.md    # Scientific background
│   └── data-structures.md        # Data structure documentation
├── supporting/
│   ├── style-guide.md           # Documentation style guide
│   ├── contributing.md          # Contribution guidelines
│   ├── development-environment.md # Setup instructions
│   └── testing-procedures.md     # Testing guidelines
└── README.md                   # This overview document
```

### File Naming Conventions

#### Markdown Files
- Use kebab-case for filenames: `user-manual.md`, `command-reference.md`
- Use snake_case for internal notes: `_template.md`
- Use UPPER_CASE for constants and configuration

#### Directory Names
- Use kebab-case for subdirectories: `tutorial/`, `api-reference/`
- Use descriptive names that explain content
- Keep directory depth reasonable (max 3 levels)

### File Extensions
- **Primary**: `.md` (Markdown)
- **Code Examples**: `.cpp`, `.h`, `.py` (inline in documentation)
- **Configuration Files**: `.json`, `.yaml` (embedded examples)
- **Diagrams**: `.svg`, `.png` (referenced in documentation)

## Content Standards

### 1. Accessibility

#### Reading Level
- Target reading level: High school senior to college sophomore
- Avoid jargon or explain all technical terms
- Use clear, concise language

#### Formatting Requirements

**Headings**
- Use Level 1 for main titles: `# Document Title`
- Use Level 2 for major sections: `## Section Name`
- Use Level 3 for subsections: `### Subsection Name`
- Use Level 4+ for detailed subsections as needed

**Lists**
- Use bullet points for unordered lists: `- Item`
- Use numbered lists for sequential steps: `1. First item`
- Use nested lists for hierarchical information

**Emphasis**
- Use **bold** for emphasis and UI elements
- Use *italics* for emphasis
- Use `monospace` for code and file paths

**Block Quotes**
- Use `> Quote` for important notes and warnings
- Use `> Warning:` for safety notices
- Use `> Example:` for code examples

#### Link Standards

**Internal Links**
- Use relative paths: `[Documentation](../README.md)`
- Use anchor links for sections: `[#algorithm-overview](#algorithm-overview)`

**External Links**
- Use full URLs: `[GitHub](https://github.com/nlm/nlm)`
- Add descriptive link text
- Use HTTPS when possible

### 2. Technical Documentation

#### Code Examples

**Formatting**
```markdown
// Code block with syntax highlighting
c++
#include <iostream>
int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}

// Inline code
int main() {
    int x = 42; // This is a comment
}
```

**Best Practices**
- Show complete, runnable examples
- Include error handling where relevant
- Use meaningful variable and function names
- Add comments for complex logic
- Follow the code style guide

#### Mathematical Notation

**Formula Formatting**
```markdown
# Leaky Integrate-and-Fire Model

The membrane potential dynamics are given by:

```
dv/dt = (-v + R * I) / τ
```

Where:
- `v` = membrane potential (mV)
- `I` = synaptic current (nA)
- `R` = membrane resistance (MΩ)
- `τ` = time constant (ms)
```

**Tables**

```markdown
| Parameter | Symbol | Default | Range | Description |
|-----------|--------|---------|-------|-------------|
| Membrane Capacitance | C | 1.0 | [0.1, 10] | Membrane capacitance (nF) |
| Time Constant | τ | 20.0 | [1, 100] | Integration time constant (ms) |
| Threshold | V_th | -55.0 | [-70, -40] | Spike threshold (mV) |
```

#### Diagrams and Visualizations

**Image References**
```markdown
![Neuron LIF Dynamics](diagrams/lif_dynamics.svg)

Figure 1: Leaky Integrate-and-Fire neuron dynamics showing membrane potential over time.
```

**Algorithm Flowcharts**
```markdown
```mermaid
topdown pie
    title System Architecture
    A: Sensory Input
    B: Neural Processing
    C: Motor Output
    A --> B
    B --> C
```
```

### 3. User Documentation

#### User Manual Structure

```markdown
# User Manual

## Overview
Brief introduction to NLM and its capabilities.

## Installation
### Prerequisites
List requirements.

### Building Instructions
```bash
# Build commands
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Basic Usage
### Quick Start
Step-by-step basic usage.

### Configuration Examples
Sample configuration files.

## Advanced Topics
### Performance Tuning
Advanced configuration options.

### Troubleshooting
Common issues and solutions.
```

#### Tutorials

**Basic Tutorial Structure**
```markdown
# Basic Tutorial

## Prerequisites
List requirements.

## Step 1: Setup
Installation and initial configuration.

## Step 2: First Experiment
Running your first simulation.

## Step 3: Understanding Results
Analyzing and interpreting output.

## Advanced Topics
### Additional Features
More complex usage patterns.
```

**Advanced Tutorial Structure**
```markdown
# Advanced Tutorial

## Prerequisites
Advanced setup requirements.

## Custom Configuration
Designing custom brain parameters.

## Performance Optimization
Advanced tuning techniques.

## Research Applications
Using NLM for research projects.
```

### Command Reference

**Format**
```markdown
### Command Name

**Syntax:**
```
command-name [options] [arguments]
```

**Description:**
Brief description of the command functionality.

**Options:**
| Option | Description |
|--------|-------------|
| `-h, --help` | Show help information |
| `-v, --verbose` | Enable verbose output |
| `-c, --config` | Configuration file path |

**Arguments:**
- argument1: Description of first argument
- argument2: Description of second argument

**Examples:**
```bash
# Example 1
command-name --option1 value1

# Example 2
command-name -c config.json -v
```
```

### Configuration Guide

**Format**
```markdown
# Configuration Guide

## Configuration Parameters

### [Section Name]

#### Parameter Name
- **Type:** data-type
- **Default:** default-value
- **Range:** min-max (or list of valid values)
- **Description:** Detailed explanation
- **Example:** `["option1", "option2"]`

### Configuration Examples

#### Minimal Configuration
```json
{
    "simulation": {
        "timestep": 0.001,
        "steps": 100
    },
    "brain": {
        "neuron_count": 256
    }
}
```
```

## Code Documentation Standards

### 1. Code Comments

**Doxygen-Style Comments**
```cpp
/**
 * @class Neuron
 * @brief Represents a single neuron with LIF dynamics
 * 
 * The Neuron class implements the Leaky Integrate-and-Fire (LIF) model
 * of neural computation, including membrane potential dynamics, spike
 * generation, and synaptic interactions.
 * 
 * @note This is a biological-inspired model with no claims of
 *       physiological accuracy.
 * 
 * @warning NeuralState::state must be properly initialized before use.
 * 
 * @see https://en.wikipedia.org/wiki/Leaky_integrate-and-fire_model
 * 
 * @since Phase 2: Real LIF neuron dynamics
 * 
 * @author NLM Development Team
 * 
 * @note This class is thread-safe for concurrent access from multiple
 *       simulation steps.
 */
class Neuron {
public:
    /**
     * @brief Constructs a neuron with specified identifier
     * 
     * @param id Unique identifier for this neuron
     * @throws std::invalid_argument if id is zero
     * 
     * @pre Neuron identifiers must be unique within the brain
     * @post Neuron is initialized with resting state
     */
    explicit Neuron(NeuronId id);
    
    // ... other methods ...
};
```

**Comment Best Practices**

1. **Document public interfaces**
2. **Explain complex logic**
3. **Include parameter ranges and constraints**
4. **Reference related functions and classes**
5. **Mention dependencies and requirements**
6. **Provide usage examples where helpful**

### 2. API Documentation

**Complete API Reference**

```markdown
# API Reference

## Core Types

### nlm::Types

The `nlm::Types` namespace provides strongly-typed identifiers:

- `NeuronId`: Unique neuron identifier
- `SynapseId`: Unique synapse identifier
- `RegionId`: Unique brain region identifier
- `PopulationId`: Unique neuron population identifier

## Classes

### nlm::Neuron

**Public Methods:**
- `Neuron(NeuronId id)`: Constructor
- `getId()`: Get neuron identifier
- `getType()`: Get neuron type
- `setType(NeuronType type)`: Set neuron type
- `getMembranePotential()`: Get membrane potential
- `setMembranePotential()`: Set membrane potential
- `addToMembranePotential()`: Add to membrane potential
- And more...

**Parameters:**
- `NeuronId`: Neuron identifier
- `NeuronType`: Neuron type (EXCITATORY, INHIBITORY, etc.)
- `MembranePotential`: Membrane potential in mV
- `FiringState`: Current firing state

## Function Reference

### Function Naming
- Use descriptive names: `calculateWeightChange()` instead of `C1()`
- Use past tense for actions: `getMembranePotential()`, `setMembranePotential()`
- Use camelCase for method names
- Use underscores for constants

### Parameter Naming
- Use descriptive names: `synapticInput` instead of `input`
- Use singular for single items: `neuronId`, `synapseId`
- Use plural for collections: `neuronIds`, `synapseHandles`

## Language and Style

### 1. Code Style

**C++ Specific Rules**

- **Braces**: Place opening brace on same line as control statement
```cpp
if (condition) {
    // code
}
```

- **Indentation**: Use 4 spaces (no tabs)
- **Line Length**: Maximum 100 characters
- **Namespaces**: Use `nlm::` for all NLM code
- **Includes**: Standard library first, then project includes

**Code Example**
```cpp
namespace nlm {

class Neuron {
public:
    explicit Neuron(NeuronId id) : pImpl(new Impl) {
        pImpl->id = id;
        pImpl->type = NeuronType::Internal;
        // ... initialization
    }
    
    // ... methods ...
    
private:
    struct Impl {
        NeuronId id;
        NeuronType type;
        // ... other members
    };
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
```

### 2. Documentation Formatting

**Headers and Structure**
```markdown
# Document Title

## Section 1
Content for section 1.

### Subsection 1.1
More detailed content.

#### Subsection 1.1.1
Even more detailed content.

## Section 2

### Subsection 2.1
Content for subsection 2.1.
```

**Lists and Tables**
```markdown
### Example List

#### Unordered List
- Item one
- Item two
  - Nested item
  - Another nested item

#### Numbered List
1. First item
2. Second item
   1. Sub-item one
   2. Sub-item two

### Example Table

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation.timestep` | float | 0.001 | Simulation time step |
| `brain.neuron_count` | int | 1024 | Number of neurons |
```

**Code Blocks**
```markdown
### Code Example

```cpp
// C++ code example
#include <iostream>

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
```

### Python Code Example
```python
# Python code example
def hello_world():
    print("Hello, World!")
    return True

if __name__ == "__main__":
    hello_world()
```
```

**Mathematical Expressions**
```markdown
### Leaky Integrate-and-Fire Model

The membrane potential dynamics follow the differential equation:

```
dv/dt = (-v + R * I) / τ
```

Where:
- `v` = membrane potential (mV)
- `I` = synaptic current (nA)
- `R` = membrane resistance (MΩ)
- `τ` = time constant (ms)
```

## Writing Guidelines

### 1. Clarity and Precision

**Be Specific**
```markdown
Incorrect: The neuron spikes when voltage is high.
Correct: The neuron fires when membrane potential reaches -55 mV.
```

**Be Precise**
```markdown
Incorrect: The simulation takes a long time.
Correct: The 1024-neuron simulation with 1000 steps takes approximately 5 minutes.
```

### 2. Consistency

**Consistent Terminology**
- Use consistent terms for similar concepts
- Maintain consistent formatting throughout document
- Use same units and measurements throughout

**Consistent Structure**
- Follow same format for similar sections
- Use consistent ordering of information
- Apply consistent indentation and spacing

### 3. Completeness

**Include All Relevant Information**
- Provide complete examples
- Include all necessary parameters and options
- Document edge cases and error conditions

**Provide Context**
- Explain background and motivation
- Reference related concepts and implementations
- Provide usage examples and best practices

### 4. Accuracy

**Verify Information**
- Cross-reference with source code
- Validate parameter ranges and defaults
- Check examples for correctness

**Maintain Currency**
- Update documentation with code changes
- Include version information where relevant
- Note experimental or provisional features

## Documentation Tools

### Automated Documentation Generation

**Doxygen Integration**
```cmake
# CMake configuration for Doxygen
set(DOXYGEN_USE_SOLR FALSE)
set(DOXYGEN_HTML_OUTPUT html)
set(DOXYGEN_XML_OUTPUT xml)
set(DOXYGEN_GENERATE_LATEX FALSE)
```

**Makefile Rules**
```makefile
# Documentation generation rules
docs: html xml

html: Doxyfile
	doxygen -g	doxygen -f Doxyfile

xml: Doxyfile
	doxygen -f Doxyfile
```

### Documentation Build Process

```bash
#!/bin/bash
# build_docs.sh - Build documentation

set -e

# Clean previous documentation
rm -rf docs/html docs/xml

# Configure Doxygen
cmake .. -DENABLE_DOXYGEN=ON

# Generate API documentation
make docs

# Copy generated documentation
cp -r html docs/api-reference/

# Build website (if using Jekyll or similar)
# bundle exec jekyll build --destination docs/website

echo "Documentation built successfully"
```

### Version Control

**Git Integration**
```bash
# Add documentation changes
git add docs/

# Commit with descriptive message
git commit -m "Update API documentation with new methods

- Add detailed parameter descriptions for neuron update method
- Include examples for plasticity rules
- Fix formatting in configuration reference

Co-authored-by: OpenHands <openhands@all-hands.dev>"

# Tag documentation versions
git tag -a "v1.2.3-docs" -m "Documentation for version 1.2.3"
```

## Quality Assurance

### Documentation Review

**Review Checklist**

- [ ] All public classes documented
- [ ] All public methods documented
- [ ] Parameters and return values described
- [ ] Examples provided for complex functions
- [ ] Cross-references are accurate
- [ ] Mathematical formulas are correct
- [ ] Code examples are syntactically correct
- [ ] Style guide conventions followed
- [ ] Consistent formatting throughout
- [ ] Complete index of all documented APIs

### Automated Validation

**Doxygen Validation**
```bash
# Validate Doxygen output
doxygen -u Doxyfile

# Check for warnings
find html -name "*.html" -exec grep -l "WARNING" {} \;
```

**Spell Checking**
```bash
# Install spell checker
apt-get install spell

# Check documentation spelling
find docs/ -name "*.md" -exec aspell -l en check {} \;
```

## Version Control for Documentation

### Documentation Changes

**Change Types**

1. **Minor Updates**: Typos, formatting fixes
2. **Feature Additions**: New sections, examples
3. **Bug Fixes**: Incorrect information, broken links
4. **Refactoring**: Structure reorganization

**Commit Messages**
```bash
# Minor update
git commit -m "Fix typo in user manual"

# Feature addition
git commit -m "Add advanced tutorial section"

# Bug fix
git commit -m "Fix broken link in configuration guide"

# Refactoring
git commit -m "Reorganize documentation structure for better navigation"
```

### Documentation Branching

**Branch Strategy**

1. **Main Branch**: Stable documentation for released versions
2. **Development Branch**: Current documentation under development
3. **Feature Branches**: Specific documentation additions or improvements
4. **Hotfix Branches**: Urgent documentation fixes for releases

**Example Workflow**
```bash
# Start with development branch
git checkout development

# Create feature branch for new tutorial
 git checkout -b feature/advanced-tutorial

# Add documentation
git add docs/tutorial/advanced.md
git commit -m "Add advanced tutorial section"

# Review and merge
# git merge development
# git push origin feature/advanced-tutorial
# Create pull request for review
```

## Documentation Maintenance

### Regular Updates

**Update Schedule**
- **Weekly**: Check for code documentation changes
- **Monthly**: Review and update user documentation
- **Quarterly**: Complete documentation overhaul
- **Annually**: Major documentation update with release

**Maintenance Tasks**
1. Update parameter descriptions based on code changes
2. Add examples for new functions
3. Fix broken links and references
4. Update version-specific information
5. Review and improve formatting

### Documentation Tools Maintenance

**Tool Updates**
```bash
# Update Doxygen configuration
cmake .. -DDOXYGEN_VERSION=1.9.1

# Update documentation templates
git submodule update --recursive

# Clean generated documentation
rm -f Doxyfile
rm -rf html xml latex
```

### Performance Monitoring

**Documentation Performance**
```bash
# Monitor documentation build time
time make docs

# Check documentation size
find docs/ -name "*.html" -exec du -h {} + | sort -hr

# Validate links
linkchecker docs/html
```

## Conclusion

Consistent documentation is essential for:

1. **Developer Productivity**: Clear, well-structured documentation reduces onboarding time
2. **Code Maintenance**: Accurate documentation reduces support costs
3. **Quality Assurance**: Systematic validation ensures correctness
4. **User Experience**: Professional documentation enhances user satisfaction
5. **Project Success**: Comprehensive documentation supports project longevity

**Quality Checklist for Documentation**: