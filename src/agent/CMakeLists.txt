# CMake Configuration for NLM Command-Line Interface

This file contains CMake configuration for building the NLM Command-Line Interface (nlm_command) executable.

## Overview

The NLM Command-Line Interface is a separate executable that provides real-time control and monitoring capabilities for NLM brain simulations. It integrates with the existing NLM libraries and AgentBrain system.

## Dependencies

The nlm_command executable depends on:

1. **nlm_core** - Core neural network library
2. **nlm_agent** - Agent system (AgentBrain, SensoryPercept, etc.)
3. **nlm_world** - World simulation system
4. **C++ standard library** (C++20)
5. **Standard libraries** (iostream, fstream, etc.)

## Build Configuration

### Source Files

The nlm_command executable is built from:

1. **src/agent/CommandLineInterface.cpp** - Main command-line interface implementation
2. **src/agent/CommandLineInterface.hpp** - Command-line interface header

### CMake Targets

The target `nlm_command` is added as an executable.

### Include Directories

The target includes the `src/agent` directory for header files.

### Linking

The target links against:
- `nlm_agent` (for AgentBrain and agent functionality)
- `nlm_core` (for core neural network functionality)

## Usage

### Basic Usage

```bash
./nlm_command [options]
```

### Available Commands

The command-line interface provides the following commands:

- `help` - Show available commands
- `status` - Show brain state
- `step [count]` - Run simulation steps
- `reset` - Reset brain state
- `save [file]` - Save brain state
- `load [file]` - Load brain state
- `config` - Configuration management
- `memory` - Memory inspection
- `stats` - Neural network statistics
- `experiment` - Experiment control
- `batch` - Batch processing
- `script` - Script file support
- `event` - Event-driven commands

### Interactive Mode

Run in interactive mode to enter commands manually:

```bash
./nlm_command --interactive
```

### Batch Mode

Run a single command in batch mode:

```bash
./nlm_command --batch "step 10; save test.bin"
```

### Script Mode

Execute commands from a script file:

```bash
./nlm_command --script commands.txt
```

## Configuration

### Command-Line Arguments

The executable accepts the following options:

- `--help` - Display help message and exit
- `--version` - Display version information and exit
- `--config <file>` - Load configuration from file
- `--interactive` - Run in interactive mode (default)
- `--batch <command>` - Run a single command in batch mode
- `--script <file>` - Execute commands from script file
- `--daemon` - Run as a daemon process
- `--pid <file>` - Write process ID to specified file

### Configuration File

The command-line interface can load configuration from a file specified with `--config`. Configuration files are in key=value format.

## Integration with NLM

### AgentBrain Integration

The command-line interface integrates with AgentBrain to provide:

1. **Brain Control**: Commands can start, stop, reset, and step the brain
2. **State Management**: Save and load brain state
3. **Monitoring**: Commands provide real-time status and statistics
4. **Configuration**: Commands can view and modify brain configuration

### Memory Inspection

Commands provide access to:

- Working memory - Transient active information
- Episodic memory - Stored experiences
- Semantic memory - Knowledge
- Procedural memory - Skills
- Associative memory - Pattern associations

### Experiment Control

The command-line interface enables:

1. **Experiment Execution**: Run predefined experiments
2. **Experiment Status**: Check experiment progress
3. **Result Monitoring**: Track experiment outcomes

## Building

### With CMake

To build the NLM Command-Line Interface:

```bash
mkdir build
cd build
cmake ..
make nlm_command
```

### With Ninja

```bash
mkdir build
cd build
cmake -G "Ninja" ..
ninja nlm_command
```

## Installation

### System Installation

Install the command-line interface system-wide:

```bash
cd build
sudo make install
```

The executable will be installed to:
- `/usr/local/bin/nlm_command` (Unix-like systems)
- `C:\Program Files\NLM\nlm_command.exe` (Windows)

### User Installation

Install locally in the user's bin directory:

```bash
cd build
make install DESTDIR=/path/to/local/prefix
```

## Development

### Adding New Commands

To add a new command to the NLM Command-Line Interface:

1. Create a new command class that inherits from `Command` in `CommandLineInterface.hpp`
2. Implement the `execute()`, `getHelp()`, and `getName()` methods
3. Register the command in `CommandLineInterface.cpp`
4. Update the help documentation

### Command Structure

The command system uses the following structure:

```cpp
class Command {
public:
    virtual ~Command() = default;
    virtual std::string execute(const std::vector<std::string>& args) = 0;
    virtual std::string getHelp() const = 0;
    virtual std::string getName() const = 0;
};
```

### Command Implementation Example

```cpp
class MyCommand : public Command {
public:
    std::string execute(const std::vector<std::string>& args) override {
        // Implement command logic
        return "Command executed successfully";
    }
    
    std::string getHelp() const override {
        return "mycommand - Description of mycommand";
    }
    
    std::string getName() const override {
        return "mycommand";
    }
};
```

### Command Registration

Commands are registered in `CommandLineInterface::initialize()`:

```cpp
void CommandLineInterface::initialize(...) {
    registerCommand(std::make_unique<MyCommand>(this));
    // ... other commands
}
```

## Testing

The NLM Command-Line Interface can be tested using:

1. **Unit Tests**: Test individual command implementations
2. **Integration Tests**: Test command integration with AgentBrain
3. **End-to-End Tests**: Test command-line interface as a whole

### Running Tests

```bash
cd build
ctest
```

## Troubleshooting

### Build Errors

If you encounter build errors, check:

1. Required dependencies are installed
2. Compiler supports C++20
3. NLM libraries are built successfully
4. Include directories are correctly set

### Runtime Errors

If you encounter runtime errors:

1. Check configuration file format
2. Verify command arguments
3. Check file permissions
4. Review error messages for details

## License

This CMake configuration is part of the NLM (Neural Learning Machine) project. See the main LICENSE file for details.

## See Also

- NLM Documentation
- AgentBrain Documentation
- NLM Command-Line Interface User Guide
- NLM Core Library Documentation