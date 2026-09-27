#!/usr/bin/env python3
"""
Setup script for NLM CLI
"""

import os
import sys
from pathlib import Path
import json
import yaml

def create_project_structure():
    """Create the complete NLM CLI project structure"""
    print("Creating NLM CLI project structure...")
    
    # Main project directories
    directories = [
        "python",
        "python/nlm",
        "python/nlm/cli",
        "configs",
        "docs",
        "examples",
        "examples/cli",
        "examples/presets",
        "scripts",
        "tests",
        "tests/unit",
        "tests/integration",
        "tests/benchmarks"
    ]
    
    for directory in directories:
        Path(directory).mkdir(parents=True, exist_ok=True)
        print(f"✓ Created directory: {directory}")
    
    return True

def create_main_cli():
    """Create the main NLM CLI implementation"""
    print("\nCreating main NLM CLI implementation...")
    
    main_cli_content = open("python/nlm/cli/nlm_cli.py").read()
    return True

def create_config_manager():
    """Create configuration manager module"""
    print("\nCreating configuration manager module...")
    
    config_manager_content = open("python/nlm/cli/config_manager.py").read()
    return True

def create_examples():
    """Create example files and scripts"""
    print("\nCreating example files...")
    
    # Create example configuration files
    example_configs = [
        ("configs/default.cfg", {
            "# NLM Default Configuration": "",
            "# This is the default configuration for Phase 1 skeleton": "",
            "random_seed = 42": "",
            "simulation_timestep = 0.001": "",
            "neuron_count = 1000": "",
            "region_count = 2": "",
            "connection_probability = 0.1": "",
            "plasticity_learning_rate = 0.01": "",
            "stdp_ltp_weight = 0.01": "",
            "stdp_ltd_weight = 0.012": "",
            "stdp_time_constant = 20.0": "",
            "development_synaptogenesis_rate = 0.001": "",
            "development_pruning_rate = 0.0001": "",
            "dopamine_baseline = 0.1": "",
            "reward_discount_factor = 0.99": "",
            "environment_name = GridWorld": "",
            "environment_width = 10": "",
            "environment_height = 10": "",
            "max_simulation_steps = 10000": "",
            "visualization_enabled = false": "",
            "log_level = INFO": "",
            "log_to_file = false": "",
            "log_filename = nlm.log": ""
        })
    ]
    
    for filepath, lines in example_configs:
        full_path = Path(filepath)
        full_path.parent.mkdir(parents=True, exist_ok=True)
        with open(full_path, 'w') as f:
            for line in lines:
                f.write(f"{line}\n")
        print(f"✓ Created example config: {filepath}")
    
    # Create CLI examples
    cli_examples = [
        ("examples/cli/basic_usage.py", '''#!/usr/bin/env python3
"""
Basic NLM CLI usage examples
"""

import subprocess
import sys
import os

def run_command(cmd):
    """Run a command and return output"""
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        return result.returncode, result.stdout, result.stderr
    except Exception as e:
        return 1, "", str(e)

def main():
    print("NLM CLI Basic Usage Examples")
    print("=" * 40)
    
    examples = [
        ("Help", ["nlm", "--help"]),
        ("Version", ["nlm", "--version"]),
        ("Config List", ["nlm", "config", "list"]),
    ]
    
    for name, cmd in examples:
        print(f"\\nExample: {name}")
        print(f"Command: {' '.join(cmd)}")
        returncode, stdout, stderr = run_command(' '.join(cmd))
        
        if returncode == 0:
            print("✓ Command executed successfully")
            if stdout:
                print(f"Output: {stdout[:200]}...")
        else:
            print(f"✗ Command failed (exit code: {returncode})")
            if stderr:
                print(f"Error: {stderr}")

if __name__ == "__main__":
    main()
''')
    ]
    
    for filepath, content in cli_examples:
        full_path = Path(filepath)
        full_path.parent.mkdir(parents=True, exist_ok=True)
        with open(full_path, 'w') as f:
            f.write(content)
        print(f"✓ Created CLI example: {filepath}")

def create_scripts():
    """Create helper scripts"""
    print("\nCreating helper scripts...")
    
    scripts_dir = Path("scripts")
    
    # Create a simple test script
    test_script = scripts_dir / "test_nlm_cli.sh"
    with open(test_script, 'w') as f:
        f.write('''#!/bin/bash
# Test NLM CLI installation and basic functionality

set -e

echo "Testing NLM CLI Installation..."

# Check Python version
python3 --version
if [[ $? -ne 0 ]]; then
    echo "ERROR: Python 3 is required"
    exit 1
fi

# Test CLI help
echo "1. Checking help..."
nlm --help

# Test version
echo "2. Checking version..."
nlm --version

# Test config list
echo "3. Checking config list..."
nlm config list

echo "4. Creating test configuration..."
nlm create --neurons 100 --output test_config.cfg

echo "5. Testing simulation..."
nlm simulate --steps 10 --config test_config.cfg

echo "6. Cleaning up..."
rm -f test_config.cfg

echo "All tests completed successfully!"
''')
    
    test_script.chmod(0o755)
    print(f"✓ Created test script: {test_script}")

def create_documentation():
    """Create comprehensive documentation"""
    print("\nCreating documentation...")
    
    docs_dir = Path("docs")
    
    # Quick start guide
    quickstart = docs_dir / "QUICKSTART.md"
    with open(quickstart, 'w') as f:
        f.write('''# NLM CLI Quick Start

## Overview

The NLM CLI provides a comprehensive command-line interface for NLM (Neural Learning Machine) brain simulations and experiments.

## Installation

### From pip

```bash
pip install nlm
```

### From source

```bash
cd nlm-cli
pip install -e .
```

## Quick Examples

### 1. Run a simple simulation

```bash
nlm simulate --steps 500 --neuron-count 1000 --output results.json
```

### 2. Create a brain configuration

```bash
nlm create --neurons 2000 --output mybrain.cfg
```

### 3. Explore brain statistics

```bash
nlm explore --stats --neurons --synapses
```

### 4. Run Phase 6 integration test

```bash
nlm experiment --phase6 --output phase6_results.json
```

### 5. Manage configuration

```bash
nlm config set neuron_count 2000
nlm config get environment_name
nlm config list
```

## Common Commands

| Command | Description |
|---------|-------------|
| `nlm --help` | Show help information |
| `nlm --version` | Show version information |
| `nlm config list` | List all configuration keys |
| `nlm simulate` | Run brain simulation |
| `nlm create` | Create brain configuration |
| `nlm explore` | Explore brain state |
| `nlm experiment` | Run experiments |
| `nlm batch` | Process multiple configurations |
| `nlm benchmark` | Run performance benchmarks |

## Configuration Files

The NLM CLI supports JSON, YAML, and simple .cfg format configuration files.

## Output Formats

- **text**: Human-readable output (default)
- **json**: Structured output for programming
- **yaml**: Structured output for data processing

## Getting Help

- `nlm --help`: Show help for all commands
- `nlm [command] --help`: Show help for specific command
- Visit the documentation website for detailed guides

## Next Steps

1. Install the NLM CLI package
2. Create your first configuration file
3. Run a simple simulation
4. Explore the configuration management commands
5. Try batch processing for multiple simulations
''')
    
    print(f"✓ Created quick start guide: {quickstart}")

def main():
    """Main setup function"""
    print("NLM CLI Project Setup")
    print("=" * 50)
    
    try:
        create_project_structure()
        print("✓ Project structure created")
        
        if os.path.exists("python/nlm/cli/nlm_cli.py"):
            print("✓ Main CLI already exists")
        else:
            print("✗ Main CLI not found")
            return False
        
        create_examples()
        create_scripts()
        create_documentation()
        
        print("\n" + "=" * 50)
        print("NLM CLI project setup completed successfully! ✓")
        print("\nNext steps:")
        print("1. Run 'pip install -e .' to install the CLI package")
        print("2. Run './scripts/test_nlm_cli.sh' to test the installation")
        print("3. Visit docs/QUICKSTART.md for quick start guide")
        print("4. Check examples/ for usage examples")
        
        return True
        
    except Exception as e:
        print(f"\nSetup failed: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == '__main__':
    success = main()
    sys.exit(0 if success else 1)
