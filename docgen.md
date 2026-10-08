# NLM Documentation Generator
# Automated documentation generation for the NLM (Neural Learning Machine) project

This module provides tools for generating documentation from NLM source code comments.
It supports multiple output formats and integrates with the existing build system.

## Overview

The NLM documentation generator extracts Doxygen-style comments from source files and
generates comprehensive documentation including:

- Class and function documentation
- API reference
- Usage examples
- Architecture documentation
- Configuration documentation

## Usage

### Command Line Interface

```bash
# Generate documentation in default format
./docgen

# Generate documentation in specific format
./docgen --format markdown --output docs/api.md
./docgen --format html --output docs/index.html
./docgen --format latex --output docs/reference.tex

# Generate documentation for specific modules
./docgen --modules brain,config,memory --output docs/

# Generate documentation with examples
./docgen --examples --output docs/

# Generate documentation with API reference
./docgen --api-ref --output docs/
```

### Python API

```python
import docgen

# Generate documentation
result = docgen.generate(
    input_dir="src",
    output_dir="docs",
    format="markdown",
    include_examples=True,
    include_api_ref=True
)

# Generate documentation for specific modules
result = docgen.generate_modules(
    modules=["brain", "config", "memory"],
    output_dir="docs"
)
```

## Features

### 1. Comment Extraction
- Extracts Doxygen-style comments from C++ source files
- Supports @brief, @details, @param, @return, @note, @warning
- Handles multi-line comments and comment blocks

### 2. Multiple Output Formats
- **Markdown**: Human-readable documentation with tables
- **HTML**: Web-ready documentation with CSS styling
- **LaTeX**: Scientific documentation for publications
- **JSON**: Machine-readable API documentation

### 3. Module-Specific Documentation
- Separate documentation for each module (brain, config, memory, etc.)
- Cross-references between modules
- Architecture overview

### 4. Example Generation
- Automatically extracts code examples from comments
- Supports multiple programming languages (C++, Python)
- Creates runnable examples where possible

### 5. API Reference
- Automatically generates function signatures
- Type information and parameter documentation
- Return value documentation
- Example usage snippets

### 6. Configuration Documentation
- Auto-generates configuration schemas
- Document all configuration parameters
- Show validation rules and constraints

## Components

### 1. Comment Parser
- Parses Doxygen-style comments
- Extracts comment metadata
- Handles comment blocks and inline comments

### 2. Documentation Generator
- Renders comments to various formats
- Creates documentation structure
- Generates navigation and tables of contents

### 3. Template Engine
- Supports customizable templates
- Uses Mustache/Handlebars-like syntax
- Allows for customization of output formats

### 4. Integration Tools
- Integrates with existing build system
- Can be run as part of build process
- Supports continuous documentation updates

## Configuration

### Configuration File

```yaml
# docgen configuration
docgen:
  input_dir: "src"
  output_dir: "docs"
  format: "markdown"
  include_examples: true
  include_api_ref: true
  modules:
    - brain
    - config
    - memory
    - prediction
    - cognition
  templates:
    path: "templates/"
    name: "default"
  style:
    theme: "default"
    css: "style.css"
    toc: true
    syntax_highlighting: true
```

### Command Line Options

```
--input-dir DIR, -i DIR           Input directory containing source files
--output-dir DIR, -o DIR          Output directory for generated documentation
--format FORMAT, -f FORMAT       Output format (markdown, html, latex, json)
--include-examples, -e            Include code examples
--include-api-ref, -a             Include API reference
--modules MODULES, -m MODULES     Comma-separated list of modules to document
--exclude-modules MODULES, -x MODULES  Comma-separated list of modules to exclude
--templates PATH, -t PATH        Path to templates directory
--template NAME, -T NAME          Template name (default: default)
--style STYLE, -s STYLE           Output style (default: default)
--help, -h                        Show help message
--version, -v                     Show version information
```

## Example

### Input Source

```cpp
/**
 * @brief Neural Learning Machine Brain class
 * @details The Brain class coordinates all neural systems including memory, prediction,
 *         cognition, and neuromodulation. It implements a complete artificial brain
 *         capable of learning, adaptation, and intelligent behavior.
 *
 * @note The brain operates as a closed-loop system connecting the world with neural processing.
 *
 * @see SimpleWorld
 * @see AgentBrain
 *
 * @warning This class uses the Pimpl idiom to hide implementation details.
 *
 * @tparam Config The configuration type
 */
class Brain {
public:
    /**
     * @brief Create a new brain with configuration
     *
     * @param config Configuration parameters for brain initialization
     * @return New Brain instance
     * @throws std::invalid_argument if config is null
     *
     * @example
     * ```cpp
     * auto config = nlm::createDefaultConfig();
     * auto brain = nlm::createBrain(config);
     * brain->initialize();
     * ```
     */
    static std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config);
    
    /**
     * @brief Initialize the brain with configuration
     *
     * @return true if initialization successful, false otherwise
     * @post Brain is ready for simulation steps
     * @see step
     *
     * @note This must be called before any simulation steps.
     */
    bool initialize();
    
    /**
     * @brief Perform a simulation step
     *
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in seconds
     *
     * @post Neural dynamics updated, spikes processed, plasticity applied
     * @see receiveSensoryInput
     * @see produceAction
     *
     * @warning This method is thread-unsafe. Call from single thread only.
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
};
```

### Generated Documentation (Markdown)

```markdown
# Brain Class

## Brief Description

Neural Learning Machine Brain class. The Brain class coordinates all neural systems
including memory, prediction, cognition, and neuromodulation. It implements a
complete artificial brain capable of learning, adaptation, and intelligent behavior.

> **Note:** The brain operates as a closed-loop system connecting the world with neural processing.

> **Warning:** This class uses the Pimpl idiom to hide implementation details.

## Public Members

### Static Methods

#### `createBrain(config)`

**Parameters:**
- `config` - Configuration parameters for brain initialization

**Returns:**
- New Brain instance

**Throws:**
- `std::invalid_argument` if config is null

**Example:**

```cpp
auto config = nlm::createDefaultConfig();
auto brain = nlm::createBrain(config);
brain->initialize();
```

#### `initialize()`

**Returns:**
- `true` if initialization successful, `false` otherwise

**Postconditions:**
- Brain is ready for simulation steps

**See also:** `step`

> **Note:** This must be called before any simulation steps.

#### `step(currentStep, currentTime)`

**Parameters:**
- `currentStep` - Current simulation step number
- `currentTime` - Current simulation time in seconds

**Postconditions:**
- Neural dynamics updated
- Spikes processed
- Plasticity applied

**See also:** `receiveSensoryInput`, `produceAction`

> **Warning:** This method is thread-unsafe. Call from single thread only.
```

## Class Diagram

```
Brain
├── Config
├── SpikeSystem
├── Memory Systems
│   ├── WorkingMemory
│   ├── EpisodicMemory
│   └── AssociativeMemory
├── PredictionSystem
├── CognitionSystems
│   ├── NeuralPlanner
│   ├── ConceptFormation
│   └── AttentionalSelection
└── NeuromodulationSystems
    ├── Dopamine
    ├── Curiosity
    ├── Novelty
    └── PredictionError
```

## Configuration

See [Configuration Documentation](#configuration) for available configuration parameters.

## Related Classes

- [SimpleWorld](simpleworld.md)
- [AgentBrain](agentbrain.md)
```
```

## Architecture

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## References

- [1] Hawkins, G., & Blakeslee, S. (2004). *On Intelligence*. Henry Holt.
- [2] Maass, W., Bishop, E., & Welle, R. (2002). *Distributed Patterns and Self-Organization in Neural Systems*. Psychology Press.
```
```

This module provides a foundation for generating comprehensive documentation
for the NLM project, making it easier for users and developers to understand
the system's architecture, API, and functionality.
```
