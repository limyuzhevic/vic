# NLM CLI Enhancement Implementation Plan

## Overview
This document provides a comprehensive implementation plan for enhancing the NLM (Neural Learning Machine) project's command-line interface (CLI). Based on the analysis of all executable targets and their current CLI implementations, this plan outlines the steps needed to create a consistent, robust, and user-friendly CLI interface.

## Current State Summary

### Executable Targets and CLI Status:
1. **nlm** (`src/main.cpp`): Basic `--config` support only
2. **nlm_phase3_demo** (`src/experiments/Phase3Demo.cpp`): Positional args for episodes/steps
3. **nlm_phase4_demo** (`src/experiments/Phase4Demo.cpp`): Positional arg for trials
4. **nlm_phase6_demo** (`src/experiments/Phase6Demo.cpp`): No CLI arguments
5. **nlm_test** (`tests/CMakeLists.txt`): No CLI needed (unit tests)

### Critical Issues Identified:
1. **Missing help/usage documentation** - No standard `-h/--help` support
2. **Poor error handling** - Uses `std::atoi()` (security risk)
3. **Inconsistent argument parsing** - Different formats across executables
4. **Limited validation** - No input range checking
5. **No logging control** - Cannot control verbosity via CLI
6. **Security vulnerabilities** - Buffer overflow potential

## Implementation Strategy

### Phase 1: Core CLI Framework (Weeks 1-3)
**Goal**: Establish foundation for consistent CLI across all executables

#### 1.1 Create CLI Framework (`src/core/cli/`)
```cpp
// ArgumentParser.hpp - Centralized argument handling
class ArgumentParser {
public:
    ArgumentParser();
    bool parse(int argc, char* argv[]);
    std::string getHelpText() const;
    std::string getUsageText() const;
    bool hasOption(const std::string& name) const;
    template<typename T> T getOption(const std::string& name) const;
    template<typename T> void addOption(const std::string& name, 
                                     const std::string& description,
                                     T default_value);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

#### 1.2 Implement Core CLI Features
- **Standard help/usage interface**: `-h, --help`, `--version`, `--usage`
- **Option registration system**: Dynamic discovery of available options
- **Error handling framework**: Consistent error reporting
- **Configuration merging**: CLI args override config files

### Phase 2: Enhanced Executable CLIs (Weeks 4-6)
**Goal**: Upgrade all executable CLIs with consistent functionality

#### 2.1 Update Main Executable (`src/main.cpp`)
```cpp
int main(int argc, char* argv[]) {
    ArgumentParser parser;
    
    // Register standard options
    parser.addOption("config", "Configuration file path", std::string("configs/default.cfg"));
    parser.addOption("log-level", "Logging level (DEBUG, INFO, WARNING, ERROR)", std::string("INFO"));
    parser.addOption("log-file", "Log output file path", std::string(""));
    parser.addOption("verbose", "Enable verbose logging", false);
    
    // Register brain configuration options
    parser.addOption("neuron-count", "Number of neurons", 1000);
    parser.addOption("region-count", "Number of brain regions", 2);
    parser.addOption("timestep", "Simulation timestep", 0.001);
    parser.addOption("connection-prob", "Connection probability", 0.1f);
    
    // Register plasticity options
    parser.addOption("stdp-ltp-weight", "STDP LTP weight", 0.01f);
    parser.addOption("stdp-ltd-weight", "STDP LTD weight", 0.012f);
    parser.addOption("stdp-time-constant", "STDP time constant", 20.0f);
    
    if (!parser.parse(argc, argv)) {
        return 1; // Error parsing arguments
    }
    
    // Load configuration
    std::string configFile = parser.getOption("config");
    auto config = std::make_shared<Config>();
    
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    }
    
    // Override with CLI options
    config->set("neuron_count", parser.getOption("neuron-count"), ConfigSource::CommandLine);
    // ... more overrides
    
    // Continue with existing main logic...
}
```

#### 2.2 Update Phase 3 Demo (`src/experiments/Phase3Demo.cpp`)
```cpp
int main(int argc, char* argv[]) {
    ArgumentParser parser;
    
    // Register Phase 3 specific options
    parser.addOption("episodes", "Number of episodes to run", 5);
    parser.addOption("steps-per-episode", "Steps per episode", 200);
    
    // Register standard options
    parser.addOption("random-seed", "Random seed for reproducibility", 42u);
    parser.addOption("log-level", "Logging level", std::string("INFO"));
    
    if (!parser.parse(argc, argv)) {
        return 1;
    }
    
    int numEpisodes = parser.getOption("episodes");
    int stepsPerEpisode = parser.getOption("steps-per-episode");
    
    // Create and run Phase3Demo with validated options
    nlm::Phase3Demo demo;
    demo.run(numEpisodes, stepsPerEpisode);
    
    return 0;
}
```

#### 2.3 Update Phase 4 Demo (`src/experiments/Phase4Demo.cpp`)
```cpp
int main(int argc, char* argv[]) {
    ArgumentParser parser;
    
    // Register Phase 4 specific options
    parser.addOption("trials", "Number of trials to run", 50);
    
    // Register standard options
    parser.addOption("neuron-count", "Number of neurons", 1000u);
    parser.addOption("region-count", "Number of regions", 2u);
    parser.addOption("log-level", "Logging level", std::string("INFO"));
    
    if (!parser.parse(argc, argv)) {
        return 1;
    }
    
    size_t numTrials = parser.getOption("trials");
    
    // Create brain with CLI-specified options
    auto config = std::make_shared<Config>();
    config->set("neuron_count", numTrials, ConfigSource::CommandLine);
    
    // Continue with existing Phase 4 logic...
}
```

#### 2.4 Update Phase 6 Demo (`src/experiments/Phase6Demo.cpp`)
```cpp
int main(int argc, char* argv[]) {
    ArgumentParser parser;
    
    // Register Phase 6 specific options
    parser.addOption("neuron-count", "Number of neurons", 500);
    parser.addOption("max-steps", "Maximum simulation steps", 2000);
    parser.addOption("enable-checkpointing", "Enable checkpointing", true);
    parser.addOption("enable-replay", "Enable replay functionality", true);
    parser.addOption("enable-development", "Enable development system", true);
    
    // Register standard options
    parser.addOption("log-level", "Logging level", std::string("INFO"));
    parser.addOption("random-seed", "Random seed", 42u);
    
    if (!parser.parse(argc, argv)) {
        return 1;
    }
    
    // Create Phase6Config from CLI options
    Phase6Config config;
    config.neuronCount = parser.getOption("neuron-count");
    config.maxSteps = parser.getOption("max-steps");
    config.enableCheckpointing = parser.getOption("enable-checkpointing");
    // ... more config
    
    // Continue with existing Phase 6 logic...
}
```

### Phase 3: Configuration System Enhancement (Weeks 7-8)
**Goal**: Improve Config class with better validation and CLI integration

#### 3.1 Enhance Config Class (`src/core/Config/Config.hpp/.cpp`)
```cpp
// Add validation methods
class Config {
public:
    // Enhanced argument parsing with validation
    bool loadFromArgs(int argc, char** argv);
    
    // New validation methods
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Enhanced type safety
    template<typename T>
    bool setWithValidation(const std::string& key, T value, 
                          ConfigSource source = ConfigSource::Runtime);
    
    // Configuration schema support
    static const std::map<std::string, ConfigSchema>& getSchema();
};
```

#### 3.2 Implement Input Validation
```cpp
// Config validation with type-specific checks
class ConfigValidator {
public:
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
    };
    
    static ValidationResult validate(const std::string& key, 
                                    const ConfigValue& value);
    
    // Type-specific validators
    static ValidationResult validateNeuronCount(const ConfigValue& value);
    static ValidationResult validateTimestep(const ConfigValue& value);
    static ValidationResult validateConnectionProbability(const ConfigValue& value);
};
```

### Phase 4: Logging System Integration (Weeks 9-10)
**Goal**: Add comprehensive logging control via CLI

#### 4.1 Create Logger Controller (`src/core/cli/LoggerController.hpp`)
```cpp
class LoggerController {
public:
    void setLogLevel(const std::string& level);
    void setLogFile(const std::string& filename);
    void setUseColors(bool useColors);
    void setLogFormat(const std::string& format);
    
    LogLevel getEffectiveLogLevel() const;
    std::string getLogLevelString() const;
    
private:
    LogLevel level_;
    std::string file_;
    bool useColors_;
    std::string format_;
};
```

#### 4.2 Update All Executables
- Integrate logger controller in main() functions
- Add `--log-level`, `--log-file`, `--log-colors` options
- Update logging calls to use controlled verbosity

### Phase 5: Testing and Documentation (Weeks 11-12)
**Goal**: Ensure reliability and provide documentation

#### 5.1 Unit Tests for CLI Framework
```cpp
// tests/test_cli_parser.cpp
TEST(ArgumentParser, HelpText) {
    ArgumentParser parser;
    parser.addOption("test-opt", "Test option", "default");
    std::string help = parser.getHelpText();
    EXPECT_TRUE(help.find("--test-opt") != std::string::npos);
}

TEST(ArgumentParser, Validation) {
    ArgumentParser parser;
    parser.addOption("count", "Count", 0, 100); // Range validation
    // Test out-of-range values
}
```

#### 5.2 Integration Tests
- Test all executable CLIs
- Test argument parsing scenarios
- Test configuration merging
- Test error handling

#### 5.3 Documentation Generation
```bash
# Generate CLI documentation
python tools/generate_cli_docs.py --output docs/CLI_REFERENCE.md

# Template for generated docs
# === CLI Reference ===
# 
# Available Options:
#   --neuron-count N      Number of neurons (default: 1000)
#   --timestep T         Simulation timestep (default: 0.001)
#   --log-level L        Logging level (DEBUG, INFO, WARNING, ERROR)
#   --verbose            Enable verbose output
# 
# Examples:
#   nlm --neuron-count 500 --timestep 0.01 --verbose
#   nlm_phase3_demo 10 500
```

## File Structure Changes

### New Directories
```
src/core/cli/          # Core CLI framework
src/core/config/       # Enhanced configuration system
include/nlm/           # Include headers
scripts/              # Build and deployment scripts
tools/                # Development tools
```

### New Files
```
src/core/cli/ArgumentParser.cpp	src/core/cli/ArgumentParser.hpp
src/core/cli/LoggerController.cpp	src/core/cli/LoggerController.hpp
src/core/cli/OptionRegistry.cpp	src/core/cli/OptionRegistry.hpp
src/core/cli/HelpGenerator.cpp	src/core/cli/HelpGenerator.hpp

src/core/config/ConfigSchema.hpp	src/core/config/ConfigValidator.cpp	src/core/config/ConfigValidator.hpp

src/core/config/validation/NeuronCountValidator.hpp
src/core/config/validation/TimestepValidator.hpp
// ... more validators

configs/CLI_options.cfg    # CLI option definitions
configs/development.cfg    # Development configuration

include/nlm/CLI_Help.hpp    # Generated CLI help headers
include/nlm/CLI_Types.hpp   # CLI type definitions

docs/CLI_REFERENCE.md      # Generated documentation

docs/building/CLI_SETUP.md  # CLI setup instructions
```

## Build System Updates

### CMake Integration
```cmake
# Add new CLI library
add_library(nlm_cli STATIC
    src/core/cli/ArgumentParser.cpp
    src/core/cli/LoggerController.cpp
    src/core/cli/OptionRegistry.cpp
    src/core/cli/HelpGenerator.cpp
)

target_include_directories(nlm_cli PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Link with core libraries
target_link_libraries(nlm_cli PUBLIC nlm_core nlm_agent nlm_world)

# Update executables to use CLI library
target_link_libraries(nlm PUBLIC nlm_cli)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_cli)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_cli)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_cli)
```

### Python Integration
```python
# pyproject.toml additions
[project.scripts]
nlm-cli = "nlm.cli:main"
nlm-phase3-demo = "nlm.phase3_demo:main"

# Python bindings for CLI configuration
from nlm.config import CLIConfig
cli_config = CLIConfig()
cli_config.set("neuron_count", 2000)
cli_config.set_log_level("DEBUG")
```

## Migration Guide

### For Existing Users

#### Backwards Compatibility
- **All existing CLI arguments continue to work**
- **New arguments are additive** - no breaking changes
- **Default values ensure same behavior** unless explicitly overridden

#### Migration Examples

##### Old Usage (Still Supported)
```bash
# Old way still works
./nlm --config=myconfig.cfg
./nlm_phase3_demo 10 500
./nlm_phase4_demo 100
```

##### New Usage (Recommended)
```bash
# New way with validation and help
./nlm --help                    # Show all available options
./nlm --neuron-count 2000 --timestep 0.01 --log-level INFO
./nlm_phase3_demo --episodes 10 --steps-per-episode 500 --random-seed 123
./nlm_phase4_demo --trials 100 --neuron-count 1000 --log-level DEBUG
```

### For Developers

#### Adding New CLI Options
1. **Register option in ArgumentParser**:
```cpp
parser.addOption("new-option", 
                 "Description of new option", 
                 default_value,      // Required
                 min_value, max_value, // Optional for numeric types
                 validator);         // Optional
```

2. **Update documentation**: Help text is automatically generated
3. **Add tests**: Unit tests for new option validation

#### Contributing Changes
1. **Fork and clone** the NLM repository
2. **Create feature branch** with descriptive name
3. **Implement changes** following this plan
4. **Add tests** for new functionality
5. **Update documentation** as needed
6. **Submit pull request** with clear description

## Testing Strategy

### Unit Tests
- **Argument parsing**: Valid and invalid inputs
- **Option validation**: Range checking, type validation
- **Help text generation**: Completeness and formatting
- **Configuration merging**: Precedence and overrides

### Integration Tests
- **Executable testing**: All CLI combinations
- **Configuration testing**: File and CLI overrides
- **Error handling**: Invalid input scenarios
- **Performance testing**: Argument parsing speed

### Automated Testing
```bash
# Run all CLI tests
./build/tests/nlm_test --gtest_filter="*CLI*"

# Run specific executable tests
cd build && ./nlm --test-mode

# Continuous integration setup
# .github/workflows/cli-tests.yml
```

## Success Metrics

### User Experience Metrics
- **Help usage**: 90% of users discover help via `-h/--help`
- **Error recovery**: 95% of invalid inputs provide clear error messages
- **Documentation**: All options documented with examples

### Technical Metrics
- **Code coverage**: >80% for CLI parsing code
- **Test coverage**: 100% for new CLI functionality
- **Performance**: Argument parsing <1ms for typical usage
- **Security**: No buffer overflow vulnerabilities

### Adoption Metrics
- **New features used**: 70% of new options within 3 months
- **Backwards compatibility**: 100% existing functionality preserved
- **Documentation usage**: Help text accessed by 50% of users

## Risk Assessment

### High Risk Items
1. **Breaking changes**: Mitigated by backwards compatibility
2. **Performance impact**: Addressed with lightweight parsing
3. **Testing completeness**: Addressed with comprehensive test suite

### Medium Risk Items
1. **Code complexity**: Mitigated with modular design
2. **Documentation maintenance**: Addressed with auto-generation
3. **Integration issues**: Addressed with extensive testing

### Low Risk Items
1. **Minor usability issues**: Addressed during usability testing
2. **Feature gaps**: Addressed in future enhancement phases

## Conclusion

This implementation plan provides a comprehensive roadmap for enhancing the NLM project's CLI interface. By following this phased approach, we can:

1. **Establish consistency** across all executables
2. **Improve robustness** with validation and error handling
3. **Enhance user experience** with comprehensive help and documentation
4. **Ensure security** by fixing identified vulnerabilities
5. **Maintain backwards compatibility** to avoid disrupting existing users

The planned enhancements will transform NLM from a basic research tool into a production-ready application suite with a professional, user-friendly command-line interface.
