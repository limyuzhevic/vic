# Brain Manipulation Commands Reference

## Overview

This document provides a comprehensive reference for all brain manipulation commands available in the Brain Command Line Interface tools. These commands enable users to perform various operations on brain data, from basic configuration management to advanced analysis and visualization.

## Command Categories

### 1. Configuration Management Commands

#### brain config load <path>
Load a brain configuration from a file.

**Description**: Loads brain configuration data from the specified file path. Supports various formats including JSON, YAML, INI, and custom text formats.

**Syntax**: `brain config load <path>`

**Examples**:
```bash
brain config load ./brain_config.json
brain config load ./brain_config.yaml
brain config load ./brain_config.ini
```

**Options**:
- No additional options available

**Exit Codes**:
- 0: Success
- 1: File not found or invalid format

#### brain config save <path>
Save the current brain configuration to a file.

**Description**: Saves the current brain configuration to the specified file path. Automatically determines the output format based on file extension.

**Syntax**: `brain config save <path>`

**Examples**:
```bash
brain config save ./brain_config.json
brain config save ./brain_config.yaml
brain config save ./brain_config.ini
```

**Options**:
- No additional options available

**Exit Codes**:
- 0: Success
- 1: Cannot write to file

#### brain config validate
Validate the current brain configuration.

**Description**: Validates the current brain configuration against predefined rules and constraints. Checks for required fields, data types, and consistency.

**Syntax**: `brain config validate`

**Examples**:
```bash
brain config validate
```

**Options**:
- `--strict`: Use strict validation rules
- `--verbose`: Show detailed validation results

**Exit Codes**:
- 0: Configuration is valid
- 1: Configuration contains errors
- 2: Validation warnings only

#### brain config backup <path>
Create a backup of the current configuration.

**Description**: Creates a backup copy of the current brain configuration to the specified backup path.

**Syntax**: `brain config backup <path>`

**Examples**:
```bash
brain config backup ./backups/config_backup.json
brain config backup ./backups/config_backup_$(date +%Y%m%d)
```

**Options**:
- No additional options available

**Exit Codes**:
- 0: Backup created successfully
- 1: Cannot create backup

#### brain config restore <path>
Restore configuration from a backup.

**Description**: Restores brain configuration from a backup file, replacing the current configuration.

**Syntax**: `brain config restore <path>`

**Examples**:
```bash
brain config restore ./backups/config_backup.json
brain config restore ./backups/config_backup_20240101
```

**Options**:
- No additional options available

**Exit Codes**:
- 0: Configuration restored successfully
- 1: Cannot restore from backup

#### brain config compare <path1> <path2>
Compare two configuration files.

**Description**: Compares two configuration files and shows differences in values, keys, and structure.

**Syntax**: `brain config compare <path1> <path2>`

**Examples**:
```bash
brain config compare ./config1.json ./config2.json
brain config compare ./original.ini ./modified.ini
```

**Options**:
- `--ignore-case`: Ignore case differences in keys
- `--ignore-order`: Ignore order of keys
- `--diff-only`: Show only differences

**Exit Codes**:
- 0: Comparison completed
- 1: Error reading files

#### brain config optimize
Optimize the current configuration.

**Description**: Applies optimization rules to improve configuration efficiency, remove redundant settings, and suggest improvements.

**Syntax**: `brain config optimize`

**Examples**:
```bash
brain config optimize
```

**Options**:
- `--aggressive`: Apply aggressive optimizations
- `--dry-run`: Show optimizations without applying them

**Exit Codes**:
- 0: Optimization completed
- 1: Optimization failed

#### brain config export <format> <path>
Export configuration in specified format.

**Description**: Exports the current configuration to a file in the specified format (JSON, YAML, CSV, etc.).

**Syntax**: `brain config export <format> <path>`

**Examples**:
```bash
brain config export json ./config_export.json
brain config export yaml ./config_export.yaml
brain config export csv ./config_export.csv
```

**Options**:
- `--pretty`: Format output with indentation (for JSON, YAML)
- `--compress`: Compress output (for supported formats)

**Exit Codes**:
- 0: Export completed successfully
- 1: Export failed

#### brain config import <format> <path>
Import configuration from file.

**Description**: Imports configuration from a file in the specified format and merges it with the current configuration.

**Syntax**: `brain config import <format> <path>`

**Examples**:
```bash
brain config import json ./config_import.json
brain config import yaml ./config_import.yaml
brain config import csv ./config_import.csv
```

**Options**:
- `--merge`: Merge with existing configuration
- `--overwrite`: Overwrite existing configuration

**Exit Codes**:
- 0: Import completed successfully
- 1: Import failed

### 2. Brain Creation and Initialization Commands

#### brain create <path>
Create a new brain configuration.

**Description**: Creates a new brain configuration file with default settings at the specified path.

**Syntax**: `brain create <path>`

**Examples**:
```bash
brain create ./brains/neural_network.json
brain create ./brains/spiking_network.json
brain create ./brains/recurrent_network.json
```

**Options**:
- `--type <type>`: Specify brain type (neural_network, spiking_network, recurrent_network)
- `--template <path>`: Use a configuration template
- `--data <data>`: Initialize with sample data

**Exit Codes**:
- 0: Brain created successfully
- 1: Cannot create brain

#### brain init <path> <data>
Initialize an existing brain with data.

**Description**: Initializes an existing brain configuration file with the specified data.

**Syntax**: `brain init <path> <data>`

**Examples**:
```bash
brain init ./brains/neural_network.json "sample_data"
brain init ./brains/spiking_network.json ./init_data.txt
```

**Options**:
- `--format <format>`: Specify data format (json, text, binary)
- `--force`: Overwrite existing brain

**Exit Codes**:
- 0: Brain initialized successfully
- 1: Cannot initialize brain

### 3. Backup and Restore Commands

#### brain backup <path>
Create a backup of a brain file.

**Description**: Creates a backup of the specified brain file, useful for preserving important brain configurations.

**Syntax**: `brain backup <path>`

**Examples**:
```bash
brain backup ./brains/neural_network.json
brain backup ./brains/spiking_network.json --compress
```

**Options**:
- `--compress`: Compress backup file
- `--encrypt`: Encrypt backup file
- `--metadata`: Include metadata in backup

**Exit Codes**:
- 0: Backup created successfully
- 1: Cannot create backup

#### brain restore <path> <backup>
Restore a brain from backup.

**Description**: Restores a brain configuration from a backup file.

**Syntax**: `brain restore <path> <backup>`

**Examples**:
```bash
brain restore ./brains/neural_network.json ./backups/neural_network_20240101.json
brain restore ./brains/spiking_network.json ./backups/spiking_network_backup.gz
```

**Options**:
- `--force`: Overwrite existing brain
- `--verify`: Verify backup integrity

**Exit Codes**:
- 0: Brain restored successfully
- 1: Cannot restore brain

### 4. Comparison and Diff Commands

#### brain compare <path1> <path2>
Compare two brain files.

**Description**: Compares two brain files and shows differences in structure, data, and metadata.

**Syntax**: `brain compare <path1> <path2>`

**Examples**:
```bash
brain compare ./brain1.json ./brain2.json
brain compare ./original_brain.txt ./modified_brain.txt
```

**Options**:
- `--detailed`: Show detailed comparison
- `--ignore-metadata`: Ignore metadata differences
- `--only-data`: Compare only data, not structure

**Exit Codes**:
- 0: Comparison completed
- 1: Error reading files

#### brain diff <path1> <path2>
Show diff between two brain files.

**Description**: Shows a unified diff format comparison between two brain files, useful for version control.

**Syntax**: `brain diff <path1> <path2>`

**Examples**:
```bash
brain diff ./brain1.json ./brain2.json
brain diff ./original.txt ./modified.txt
```

**Options**:
- `--unified`: Show unified diff format
- `--context <n>`: Number of context lines (default: 3)
- `--ignore-whitespace`: Ignore whitespace changes

**Exit Codes**:
- 0: Diff completed
- 1: Error reading files

### 5. Modification and Optimization Commands

#### brain modify <path> <key> <value>
Modify a brain configuration value.

**Description**: Modifies a specific key in the brain configuration with a new value.

**Syntax**: `brain modify <path> <key> <value>`

**Examples**:
```bash
brain modify ./brains/neural_network.json "brain.layers" "5"
brain modify ./brains/neural_network.json "brain.learning_rate" "0.001"
```

**Options**:
- `--type <type>`: Specify value type (string, number, boolean, array)
- `--json <json>`: Modify with JSON value

**Exit Codes**:
- 0: Value modified successfully
- 1: Cannot modify value

#### brain optimize <path>
Optimize a brain configuration.

**Description**: Applies optimization algorithms to improve brain performance and efficiency.

**Syntax**: `brain optimize <path>`

**Examples**:
```bash
brain optimize ./brains/neural_network.json
brain optimize ./brains/spiking_network.json --aggressive
```

**Options**:
- `--aggressive`: Apply aggressive optimizations
- `--memory`: Optimize for memory usage
- `--speed`: Optimize for speed
- `--accuracy`: Optimize for accuracy

**Exit Codes**:
- 0: Optimization completed
- 1: Optimization failed

### 6. Validation and Verification Commands

#### brain validate <path>
Validate a brain file.

**Description**: Validates a brain file against schema, format, and content rules.

**Syntax**: `brain validate <path>`

**Examples**:
```bash
brain validate ./brains/neural_network.json
brain validate ./brains/spiking_network.json --strict
```

**Options**:
- `--strict`: Use strict validation
- `--fix`: Attempt to fix validation errors
- `--verbose`: Show detailed validation results

**Exit Codes**:
- 0: Validation passed
- 1: Validation failed
- 2: Validation warnings only

#### brain verify <path>
Verify brain integrity and consistency.

**Description**: Performs comprehensive verification of brain file integrity, including checksum validation, structure verification, and consistency checks.

**Syntax**: `brain verify <path>`

**Examples**:
```bash
brain verify ./brains/neural_network.json
brain verify ./brains/spiking_network.json --thorough
```

**Options**:
- `--thorough`: Perform thorough verification
- `--skip-checksum`: Skip checksum validation
- `--skip-structure`: Skip structure validation

**Exit Codes**:
- 0: Verification passed
- 1: Verification failed
- 2: Verification warnings only

## Command-Line Interface Commands

### 1. brain-cli Command

The main entry point for all brain CLI tools.

#### brain-cli --help
Display help information for all available commands.

#### brain-cli --version
Display version information.

#### brain-cli --config <path>
Specify configuration file path.

#### brain-cli --output <path>
Specify output directory.

#### brain-cli --log-level <level>
Set logging level (DEBUG, INFO, WARNING, ERROR).

### 2. brain-shell Command

Interactive brain manipulation shell.

#### brain-shell
Start interactive brain shell.

#### help
Display help information.

#### exit/quit
Exit the brain shell.

#### history
Show command history.

#### brain <command> <args>
Execute brain-specific commands.

### 3. brain Command (Alias)

Shorthand for brain-cli.

## Usage Examples

### Basic Configuration Management
```bash
# Load a configuration
brain config load ./config.json

# Save the current configuration
brain config save ./config.json

# Validate the configuration
brain config validate

# Create a backup
brain config backup ./config_backup.json

# Restore from backup
brain config restore ./config.json ./config_backup.json
```

### Brain Creation and Initialization
```bash
# Create a new neural network brain
brain create ./brains/neural_network.json --type neural_network

# Initialize with data
brain init ./brains/neural_network.json ./sample_data.txt
```

### Backup and Restore
```bash
# Backup a brain
brain backup ./brains/neural_network.json

# Restore a brain
brain restore ./brains/neural_network.json ./backups/neural_network.json
```

### Comparison and Diff
```bash
# Compare two brains
brain compare ./brain1.json ./brain2.json

# Show diff
brain diff ./brain1.json ./brain2.json
```

### Modification and Optimization
```bash
# Modify a configuration value
brain modify ./brains/neural_network.json "brain.layers" "5"

# Optimize the brain
brain optimize ./brains/neural_network.json --aggressive
```

### Validation and Verification
```bash
# Validate a brain
brain validate ./brains/neural_network.json

# Verify brain integrity
brain verify ./brains/neural_network.json
```

## Command Summary

| Command | Description | Category |
|---------|-------------|----------|
| `brain config load <path>` | Load configuration | Configuration |
| `brain config save <path>` | Save configuration | Configuration |
| `brain config validate` | Validate configuration | Configuration |
| `brain config backup <path>` | Create backup | Configuration |
| `brain config restore <path>` | Restore from backup | Configuration |
| `brain config compare <path1> <path2>` | Compare configurations | Configuration |
| `brain config optimize` | Optimize configuration | Configuration |
| `brain create <path>` | Create new brain | Creation |
| `brain init <path> <data>` | Initialize brain | Creation |
| `brain backup <path>` | Backup brain | Backup |
| `brain restore <path> <backup>` | Restore brain | Backup |
| `brain compare <path1> <path2>` | Compare brains | Comparison |
| `brain diff <path1> <path2>` | Show diff | Comparison |
| `brain modify <path> <key> <value>` | Modify configuration | Modification |
| `brain optimize <path>` | Optimize brain | Modification |
| `brain validate <path>` | Validate brain | Validation |
| `brain verify <path>` | Verify brain | Validation |

## Troubleshooting

### Common Issues

1. **Command Not Found**
   - Ensure you're using the correct command syntax
   - Check if the command is installed
   - Use `brain-cli --help` for available commands

2. **File Not Found**
   - Verify the file path exists
   - Use absolute paths for remote files
   - Check file permissions

3. **Permission Denied**
   - Check file permissions
   - Use sudo for system-wide operations
   - Ensure proper ownership

4. **Invalid Configuration**
   - Check configuration format
   - Use `brain config validate` to check errors
   - Refer to documentation for format requirements

### Debugging
```bash
# Enable verbose logging
brain-cli --log-level DEBUG command

# Run with strace for system call tracing
strace -o trace.log brain-cli command

# Use valgrind for memory debugging
valgrind --leak-check=full brain-cli command
```

## Tips and Best Practices

1. **Use Absolute Paths**: Always use absolute paths to avoid ambiguity.

2. **Backup Before Changes**: Create backups before making significant changes.

3. **Validate Before Save**: Always validate configurations before saving.

4. **Check Documentation**: Refer to documentation for command-specific options.

5. **Use Interactive Shell**: For complex operations, use the interactive shell (`brain-shell`).

6. **Batch Processing**: Use batch processing for multiple operations.

7. **Scripting**: Use scripts for repetitive operations.

## License

This software is licensed under the Apache 2.0 License.
