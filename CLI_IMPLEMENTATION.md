# NLM Brain Control CLI

## Overview

The NLM Brain Control CLI is a command-line interface for controlling brain states and systems in the NLM (Neural Learning Machine) project. It provides comprehensive control over neural computation, memory systems, neuromodulation, and development.

## Commands

### Brain State Control

#### save [filepath]
Save the current brain state to a checkpoint file.
- **filepath**: Path to the checkpoint file to save

Example:
```
save brain_state.bin
```

#### load [filepath]
Load a brain state from a checkpoint file.
- **filepath**: Path to the checkpoint file to load

Example:
```
load brain_state.bin
```

#### reset
Reset the brain to its initial state.

Example:
```
reset
```

#### checkpoint [interval]
Set the checkpoint interval (in simulation steps).
- **interval**: Number of simulation steps between automatic checkpoints

Example:
```
checkpoint 1000
```

#### develop [stage]
Set the developmental stage.
- **stage**: One of: Initial, CriticalPeriod, Maturation, Adult

Example:
```
develop CriticalPeriod
```

### Memory System Control

#### memory working - clear
Clear all items from working memory.

Example:
```
memory working - clear
```

#### memory working - strengthen
Strengthen items in working memory.

Example:
```
memory working - strengthen
```

#### memory working - compete
Initiate competition among working memory items.

Example:
```
memory working - compete
```

#### memory episodic - store
Store the current experience in episodic memory.

Example:
```
memory episodic - store
```

#### memory episodic - replay
Replay stored episodic memories.

Example:
```
memory episodic - replay
```

#### memory episodic - consolidate
Consolidate episodic memories into long-term storage.

Example:
```
memory episodic - consolidate
```

#### memory replay [count]
Set the number of episodes to replay.
- **count**: Number of episodes to replay

Example:
```
memory replay 10
```

### Neuromodulation Control

#### neuromod [type] - [level]
Set neuromodulator level.
- **type**: One of: dopamine, curiosity, novelty, predictionError, acetylcholine, norepinephrine, serotonin
- **level**: Level value (0.0 to 1.0)

Example:
```
neuromod dopamine - 1.0
```

#### plasticity [factor]
Set global plasticity factor.
- **factor**: Plasticity factor (0.0 to 10.0)

Example:
```
plasticity 1.5
```

### Debug and Status Commands

#### status
Display current brain state and statistics.

Example:
```
status
```

#### neurons
Show neuron distribution across regions.

Example:
```
neurons
```

#### synapses
Show synapse statistics and connections.

Example:
```
synapses
```

#### memory
Show memory system status.

Example:
```
memory
```

#### cognition
Show cognitive system status.

Example:
```
cognition
```

#### performance
Show performance metrics.

Example:
```
performance
```

### Performance Commands

#### profile [metric]
Start profiling.
- **metric**: One of: memory, time, spikes

Example:
```
profile memory
```

#### stats
Show accumulated statistics.

Example:
```
stats
```

#### monitor
Start continuous monitoring mode.

Example:
```
monitor
```

### System Commands

#### help
Show this help text.

Example:
```
help
```

#### quit|exit
Exit the CLI.

Example:
```
quit
```

#### about
Show version information.

Example:
```
about
```

## Interactive Mode

When run without arguments, the CLI enters interactive mode:

```
nlm>
status
nlm>
neurons
nlm>
quit
```

## Command Line Mode

The CLI can also be used with command line arguments:

```
./nlm_cli save brain_state.bin
./nlm_cli load brain_state.bin
develop CriticalPeriod
neuromod dopamine - 1.0
memory episodic - store
neurons
status
```

## Integration

The CLI integrates with:
- **Checkpoint System**: For saving and loading brain states
- **Brain Logging System**: For displaying status and metrics
- **Configuration System**: For managing simulation parameters

## Development Notes

### File Structure

- `src/cli/main.cpp`: Main CLI implementation
- `src/cli/`: Directory for CLI-specific code

### Dependencies

The CLI depends on:
- NLM core library (nlm_core)
- NLM agent system (nlm_agent)
- NLM world system (nlm_world)
- NLM experiment libraries (nlm_phase3, nlm_phase4, nlm_phase5, nlm_phase6)

### Build

The CLI is built as a separate executable (`nlm_cli`) and linked with all necessary NLM libraries.

### Command Parsing

Commands are parsed using a simple command-line argument parser. The CLI supports:
- Interactive mode (for manual control)
- Non-interactive mode (for scripting)
- Help system (for documentation)

## Future Enhancements

Potential future enhancements include:
- Command history
- Tab completion
- Configuration file support
- Script execution
- Real-time monitoring dashboard
