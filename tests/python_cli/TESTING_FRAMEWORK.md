# NLM CLI Testing Framework
# Comprehensive Testing Suite for Enhanced NLM CLI

## Overview

This document provides a comprehensive overview of the NLM CLI testing framework, which validates all enhanced features of the NLM (Neural Learning Machine) CLI interface while maintaining backward compatibility.

## Testing Strategy

The testing framework is organized into six main categories:

### 1. Unit Testing Framework
**Purpose**: Validate individual CLI components and commands
**Focus**: Code correctness, error handling, and command functionality
**Coverage**: All enhanced CLI commands, configuration management, scripting

### 2. Integration Testing
**Purpose**: Validate CLI-C++ system interactions
**Focus**: End-to-end workflows, data flow between components
**Coverage**: Command execution chains, script processing, configuration handling

### 3. System Testing
**Purpose**: Validate performance and scalability
**Focus**: Performance benchmarks, memory usage, stress testing
**Coverage**: Large-scale simulations, concurrent operations

### 4. Validation Testing
**Purpose**: Validate configuration, data integrity, and error recovery
**Focus**: Config validation, data integrity, error handling
**Coverage**: JSON/YAML/INI parsing, error recovery mechanisms

### 5. Documentation Testing
**Purpose**: Validate documentation accuracy and completeness
**Focus**: Examples, API references, user guides
**Coverage**: Documentation examples, help system, command references

### 6. Continuous Integration
**Purpose**: Automated test execution and reporting
**Focus**: Test automation, reporting, regression detection
**Coverage**: CI pipeline setup, performance monitoring

## Test Structure

```
tests/
├── python_cli/
│   ├── test_nlm_cli_unit.py                    # Unit tests
│   ├── test_nlm_cli_integration.py              # Integration tests  
│   ├── test_nlm_cli_system.py                   # System tests
│   └── test_nlm_cli_validation.py               # Validation tests
├── cpp/
│   ├── test_config_enhanced.cpp                 # Original C++ config tests
│   ├── test_brain.cpp                           # Original brain tests
│   └── .../other_existing_cpp_tests/            # All existing tests
├── docs/
│   ├── test_documentation_accuracy.md           # Documentation tests
│   └── test_examples.py                          # Documentation examples
└── ci/
    ├── pytest.ini                                # Pytest configuration
    ├── requirements.txt                           # Test dependencies
    └── test_runner.sh                             # CI test runner
```

## Test Categories Detailed

### 1. Unit Tests (test_nlm_cli_unit.py)

**Purpose**: Test individual CLI components in isolation
**Coverage**:
- CLI initialization and state management
- Command help systems
- Configuration commands (list, get, set, validate, save, load)
- Brain commands (status, reset, load, save, info, step, initialize, terminate)
- Experiment commands (run, list, info, metrics, create, delete)
- World commands (info, reset, step)
- Monitor commands (start, stop, status, logs, metrics, configure)
- Script commands (execute, list, save, info, validate)
- Script parsing and command extraction
- Error handling and recovery
- Output formatting (text vs JSON)
- Progress indicators and verbose mode

**Key Test Patterns**:
```python
def test_cli_initialization(self):
    # Test all CLI components initialized correctly

def test_config_commands(self):
    # Test config set/get/validate/save/load

def test_brain_commands(self):
    # Test brain operations with mocked dependencies

def test_script_parsing(self):
    # Test script command extraction and validation
```

### 2. Integration Tests (test_nlm_cli_integration.py)

**Purpose**: Test CLI interactions with C++ backend
**Coverage**:
- Real configuration file handling (JSON, YAML, INI)
- Complete command workflows
- Script execution integration
- Help system integration
- Version information system
- Error handling integration
- Configuration serialization/deserialization

**Key Test Patterns**:
```python
def test_cli_with_real_config_system(self):
    # Test CLI with actual config files

def test_cli_brain_workflow(self):
    # Test complete brain workflow through CLI

def test_cli_script_workflow_integration(self):
    # Test script execution with real files

def test_cli_interaction_simulation(self):
    # Test realistic user interactions
```

### 3. System Tests (test_nlm_cli_system.py)

**Purpose**: Test performance and scalability
**Coverage**:
- Memory usage and leak detection
- Performance benchmarks
- Large-scale configuration handling
- Concurrent operations
- Stress testing
- Resource utilization

**Key Test Patterns**:
```python
def test_memory_usage_initialization(self):
    # Test memory usage with multiple CLI instances

def test_config_operations_performance(self):
    # Benchmark config operations

def test_large_scale_simulation(self):
    # Test large-scale brain simulations

def test_stress_test_simulation(self):
    # Stress test the entire system
```

### 4. Validation Tests (test_nlm_cli_validation.py)

**Purpose**: Test validation and security
**Coverage**:
- JSON/YAML/INI configuration validation
- Data integrity across operations
- Configuration schema validation
- Error recovery mechanisms
- Backup and restore functionality

**Key Test Patterns**:
```python
def test_json_config_validation(self):
    # Test JSON configuration validation

def test_config_data_integrity(self):
    # Test data preservation across operations

def test_config_error_recovery(self):
    # Test recovery from error conditions

def test_config_backup_and_restore(self):
    # Test backup and restore functionality
```

### 5. Documentation Tests (test_documentation_accuracy.py)

**Purpose**: Test documentation accuracy and examples
**Coverage**:
- Documentation example validation
- Help system completeness
- API reference accuracy
- User guide validation

**Key Test Patterns**:
```python
def test_documentation_examples(self):
    # Test all documented command examples

def test_help_documentation_completeness(self):
    # Test help system completeness

def test_cli_flags_help(self):
    # Test CLI flags documentation
```

### 6. Continuous Integration Setup

**Purpose**: Automated test execution and reporting
**Coverage**:
- Pytest configuration
- Test dependency management
- Automated test execution
- Performance regression detection
- Coverage analysis

**Key Configuration**:
```ini
[tool.pytest.ini_options]
testpaths = ["tests/python_cli"]
python_files = ["test_*.py"]
python_classes = ["Test*"]
python_functions = ["test_*"]
addopts = [
    "-v",
    "--tb=short",
    "--cov=nlm_cli",
    "--cov-report=term-missing",
    "--cov-report=html"
]

[tool.coverage.run]
source = ["nlm_cli"]
```

## Test Execution

### Running Tests

```bash
# Run all Python CLI tests
pytest tests/python_cli/ -v

# Run specific test category
pytest tests/python_cli/test_nlm_cli_unit.py -v
pytest tests/python_cli/test_nlm_cli_integration.py -v

# Run tests with coverage
pytest tests/python_cli/ --cov=nlm_cli --cov-report=term-missing

# Run tests with HTML coverage report
pytest tests/python_cli/ --cov=nlm_cli --cov-report=html
```

### Continuous Integration

```yaml
# GitHub Actions CI configuration
name: NLM CLI Tests

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        python-version: [3.8, 3.9, 3.10, 3.11, 3.12]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up Python ${{ matrix.python-version }}
      uses: actions/setup-python@v4
      with:
        python-version: ${{ matrix.python-version }}
    
    - name: Install dependencies
      run: |
        python -m pip install --upgrade pip
        pip install -e .[dev]
    
    - name: Run tests
      run: |
        pytest tests/python_cli/ -v
        pytest tests/cpp/ -v  # Original C++ tests
    
    - name: Generate coverage report
      run: |
        pytest tests/python_cli/ --cov=nlm_cli --cov-report=xml
    
    - name: Upload coverage
      uses: codecov/codecov-action@v3
      with:
        file: ./coverage.xml
        flags: unittests
        name: codecov-umbrella
```

## Test Data and Fixtures

### Test Configuration Files

```bash
# Test config files
configs/
├── default.cfg              # Original config format
├── test_config.json          # JSON test config
├── test_config.yaml          # YAML test config  
├── test_config.ini           # INI test config
└── large_config.json         # Large config for performance testing
```

### Test Script Files

```bash
# Test script files
.nlm_scripts/
├── basic_script.nlm         # Basic script with commands
├── script_with_comments.nlm  # Script with comments and formatting
├── invalid_script.nlm        # Script with errors (for error testing)
└── large_script.nlm          # Large script for performance testing
```

## Test Reporting

### Coverage Reports

The testing framework generates comprehensive coverage reports:

**HTML Coverage Report**:
- `htmlcov/index.html` - Interactive HTML coverage report
- Shows which lines are covered/not covered
- Allows drilling down to specific functions

**Text Coverage Report**:
- Console output during pytest runs
- Shows percentage coverage and missing lines

**Codecov Integration**:
- Upload coverage to Codecov for tracking
- Compare coverage across branches
- Track regression over time

### Performance Reports

```bash
# Performance benchmarks
performance/
├── initialization_benchmark.json
├── config_operations_benchmark.json
├── script_parsing_benchmark.json
├── command_execution_benchmark.json
└── system_stress_benchmark.json
```

## Testing Best Practices

### 1. Mocking Strategy

**Use mocking for external dependencies**:
- Mock C++ brain, world, and agent components
- Mock file system operations
- Mock network operations
- Mock hardware access

**Example**:
```python
with patch.object(self.cli, 'brain', MagicMock()):
    self.cli.cmd_brain(["status"])
    # Test without actual C++ dependencies
```

### 2. Test Isolation

**Ensure test isolation**:
- Use temporary files for file operations
- Clean up after each test
- Mock global state changes
- Use unique test identifiers

### 3. Performance Testing

**Measure performance systematically**:
- Use `time.time()` for timing measurements
- Monitor memory usage with `psutil`
- Run multiple iterations for statistical significance
- Compare against baseline performance

### 4. Error Testing

**Test error conditions thoroughly**:
- Invalid inputs
- Missing files
- Corrupted data
- Network failures
- Permission issues

### 5. Integration Testing

**Test end-to-end workflows**:
- Complete command sequences
- Real file operations
- Script execution
- Configuration validation

## Maintenance and Updates

### Adding New Tests

To add new tests:

1. **For new CLI commands**:
   ```python
   def test_new_command(self):
       # Test the new command
   ```

2. **For new configuration formats**:
   ```python
   def test_new_format(self):
       # Test the new format support
   ```

3. **For new integration points**:
   ```python
   def test_new_integration(self):
       # Test the new integration
   ```

### Running Tests Locally

```bash
# Install test dependencies
pip install -e .[dev]

# Run all tests
pytest tests/python_cli/ -v

# Run with coverage
pytest tests/python_cli/ --cov=nlm_cli --cov-report=html

# Run specific test category
pytest tests/python_cli/test_nlm_cli_unit.py -v
```

### CI Integration

The testing framework integrates with CI systems:

**GitHub Actions**: Configure in `.github/workflows/test.yml`
**GitLab CI**: Configure in `.gitlab-ci.yml`
**Azure DevOps**: Configure in `azure-pipelines.yml`

## Conclusion

This comprehensive testing framework ensures:

✅ **Complete test coverage** of all enhanced CLI features
✅ **Robust integration** with C++ backend
✅ **Performance validation** for scalability
✅ **Documentation accuracy** verification
✅ **Security validation** for input handling
✅ **Automated CI/CD integration**
✅ **Backward compatibility** maintenance

The framework provides a solid foundation for continuous development and ensures the NLM CLI remains reliable, performant, and maintainable.

---

**Testing Framework Version**: 1.0.0
**Last Updated**: 2026-11-09
**Status**: Production Ready
**Coverage Target**: 95%+ code coverage
**Performance Target**: < 1s for most operations