# NLM CLI Enhancement Implementation Plan

## Phase 1 Complete: Core Infrastructure

### 1.1 CLI Directory Structure ✅
**Status**: COMPLETED

Created full directory structure:
```
src/cli/
├── CLI.hpp                    # Main CLI framework header
├── Command.hpp                # Command base class
├── CommandRegistry.hpp        # Command registry system
├── CLI.cpp                    # CLI engine implementation
├── include/nlm/cli/           # Public include headers
│   ├── CLI.hpp
│   ├── Command.hpp
│   └── CommandRegistry.hpp
├── config/                    # Configuration commands
│   ├── ConfigCommands.hpp     # Config command base class
│   ├── ConfigListCommand.hpp/.cpp
│   ├── ConfigGetCommand.hpp/.cpp
│   ├── ConfigSetCommand.hpp/.cpp
│   ├── ConfigValidateCommand.hpp/.cpp
│   └── ConfigExportCommand.hpp/.cpp
├── brain/                     # Brain operation commands
│   ├── BrainCommands.hpp      # Brain command base class
│   ├── BrainInfoCommand.hpp/.cpp
│   ├── BrainResetCommand.hpp/.cpp
│   ├── BrainSaveCommand.hpp/.cpp
│   └── BrainLoadCommand.hpp/.cpp
├── Main.cpp                   # Entry point for nlm command
└── Command.cpp                # Common command implementations
```

### 1.2 Core Framework Implementation ✅

**Command Base Class**:
```cpp
class Command {
public:
    virtual ~Command() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual bool execute(const std::vector<std::string>& args) = 0;
    virtual bool validateArgs(const std::vector<std::string>& args) = 0;
};
```

**Command Registry**:
```cpp
class CommandRegistry {
private:
    std::map<std::string, std::unique_ptr<Command>> commands;
    
public:
    void registerCommand(std::unique_ptr<Command> command);
    Command* getCommand(const std::string& name);
    std::vector<std::string> getCommandNames() const;
    std::string getHelp(const std::string& commandName) const;
};
```

**CLI Engine**:
```cpp
class CLI {
private:
    CommandRegistry registry;
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    
public:
    CLI();
    bool run(int argc, char** argv);
    void printHelp() const;
    bool executeCommand(const std::string& commandName, 
                       const std::vector<std::string>& args);
};
```

### 1.3 Configuration Commands ✅

**Implemented**:
- `nlm config list` - Display configuration options
- `nlm config get <key>` - Get specific configuration value
- `nlm config set <key> <value>` - Set configuration value
- `nlm config validate` - Validate configuration
- `nlm config export <format>` - Export configuration to file

### 1.4 Brain Commands ✅

**Implemented**:
- `nlm brain info` - Display brain statistics and metadata
- `nlm brain reset` - Reset brain to initial state
- `nlm brain save <path>` - Save brain state to checkpoint
- `nlm brain load <path>` - Load brain state from checkpoint

### 1.5 Main CLI Entry Point ✅

**Created** `src/cli/Main.cpp` as the entry point for the unified `nlm` command, replacing the existing `./nlm` demo as specified.

## Integration Points

### 2.1 Configuration Integration ✅
- All configuration commands use the existing `Config` class
- Support for loading from config files and command line
- Backward compatibility maintained with existing config system

### 2.2 Brain Integration ✅
- Brain commands connect to the existing `Brain` class
- Support for both current brain and new brain instances
- Provide read-only access to brain state for commands

### 2.3 Logging Integration ✅
- All commands use the existing logging system
- Different log levels and formats supported
- Progress reporting for long-running operations

## Implementation Details

### File Locations
- **CLI framework**: `/workspace/app/src/cli/`
- **Configuration commands**: `/workspace/app/src/cli/config/`
- **Brain commands**: `/workspace/app/src/cli/brain/`
- **Public headers**: `/workspace/app/src/cli/include/nlm/cli/`

### Dependencies
- **Config class**: `/workspace/app/src/core/Config/Config.hpp`
- **Brain class**: `/workspace/app/src/brain/Brain.hpp`
- **Logger**: `/workspace/app/src/core/Logger/Logger.hpp`

### Build System
- Updated `CMakeLists.txt` to include new CLI library (`nlm_cli`)
- CLI library linked with `nlm_core` for configuration and brain support
- New main executable `nlm` replaces the legacy demo

## Command Examples

### Configuration Management
```bash
nlm config list                    # List all configuration options
nlm config get random_seed         # Get specific value
nlm config set neuron_count 1000   # Set configuration value
nlm config validate               # Validate configuration
nlm config export json > config.json  # Export to JSON
```

### Brain Operations
```bash
nlm brain info                    # Display brain information
nlm brain info --detailed          # Display detailed status
nlm brain reset --preserve-config  # Reset brain preserving config
nlm brain save checkpoint.json     # Save brain state
nlm brain load checkpoint.json     # Load brain state
```

## Next Steps for Phase 2

**Phase 2: Core Commands (Week 3-4)** will include:
- **Experiment Management**: `nlm experiment list`, `nlm experiment run`, etc.
- **System Control**: `nlm system status`, `nlm system monitor`, etc.
- **Debug and Analysis**: `nlm debug neurons`, `nlm debug synapses`, etc.

**Phase 3: Advanced Features (Week 5-6)** will include:
- **Advanced Operations**: `nlm advanced replay`, `nlm advanced pattern`, etc.
- **Batch Processing**: `nlm batch run`, `nlm batch parallel`, etc.

**Phase 4: Integration and Refinement (Week 7-8)** will include:
- Final integration with existing systems
- Command-line completion support
- Comprehensive help system
- Testing and validation

## Files Created

1. **Core Infrastructure**:
   - `src/cli/CLI.hpp`
   - `src/cli/Command.hpp`
   - `src/cli/CommandRegistry.hpp`
   - `src/cli/CLI.cpp`
   - `src/cli/Command.cpp`
   - `src/cli/Main.cpp`
   - `src/cli/include/nlm/cli/*` (headers)

2. **Configuration Commands**:
   - `src/cli/config/ConfigCommands.hpp`
   - `src/cli/config/ConfigListCommand.hpp/.cpp`
   - `src/cli/config/ConfigGetCommand.hpp/.cpp`
   - `src/cli/config/ConfigSetCommand.hpp/.cpp`
   - `src/cli/config/ConfigValidateCommand.hpp/.cpp`
   - `src/cli/config/ConfigExportCommand.hpp/.cpp`

3. **Brain Commands**:
   - `src/cli/brain/BrainCommands.hpp`
   - `src/cli/brain/BrainInfoCommand.hpp/.cpp`
   - `src/cli/brain/BrainResetCommand.hpp/.cpp`
   - `src/cli/brain/BrainSaveCommand.hpp/.cpp`
   - `src/cli/brain/BrainLoadCommand.hpp/.cpp`

## Build Instructions

```bash
cd /workspace/app
mkdir build
cd build
cmake ..
make

# Run the new NLM CLI
./nlm --help

# For example, configuration commands:
./nlm config list
./nlm config get random_seed
./nlm brain info
```

The implementation is now complete for Phase 1 and ready for Phase 2 development. The NLM CLI enhancement provides a unified, extensible command-line interface for controlling and analyzing the NLM neural learning machine system.