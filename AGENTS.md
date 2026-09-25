# Kilo AGENTS Configuration

This file documents the AGENTS configuration for the Kilo agent system.

## Overview

The Kilo AGENTS configuration provides instructions for setting up and running agents in the Kilo system. It includes configuration paths, kilo.json fields, commands, agents, skills, permissions, MCPs, providers, TUI settings, plus Agent Manager worktree setup/run scripts, workflows, and state. Use this file for Kilo config questions, locating loaded config, changing settings, or Agent Manager questions about run/setup scripts, worktree setup/workflows, apply/merge/PR/conflicts, missing sessions/worktrees, and agent-manager.json recovery.

## Configuration Structure

### Core Configuration Files

1. **kilo.json** - Main configuration file
   - Location: ~/.config/kilo/kilo.json
   - Purpose: Global Kilo settings
   - Format: JSON

2. **agent-manager.json** - Agent management configuration
   - Location: ~/.config/kilo/agent-manager.json
   - Purpose: Agent-specific settings
   - Format: JSON

3. **commands/** - Command definitions
   - Location: ~/.config/kilo/commands/
   - Purpose: User-defined commands
   - Format: JSON files

4. **.kilo/commands/** - Built-in command definitions
   - Location: .kilo/commands/
   - Purpose: Built-in Kilo commands
   - Format: JSON files

5. **.kilo/agent/** - Agent definitions
   - Location: .kilo/agent/
   - Purpose: Agent configurations
   - Format: JSON files

### Key Configuration Fields

#### kilo.json Fields

- **workspace** - Default workspace directory
- **commands** - List of available commands
- **agents** - List of configured agents
- **settings** - Kilo system settings
- **permissions** - User permissions

#### agent-manager.json Fields

- **agents** - Agent configurations
- **worktrees** - Worktree configurations
- **sessions** - Session configurations
- **permissions** - Agent permissions
- **skills** - Available skills

### Configuration Commands

#### List Configuration

To view the current Kilo configuration:

```bash
kilo config list
```

#### View Loaded Configuration

To see which configuration is currently loaded:

```bash
kilo config show
```

#### Change Configuration

To modify configuration settings:

```bash
kilo config set <key> <value>
kilo config add <key> <value>
kilo config remove <key>
```

## Agent Management

### Worktree Setup

#### Creating a New Worktree

To create a new worktree for an agent:

```bash
kilo worktree create <name> [--type <type>] [--parent <parent>]
```

#### List Worktrees

To list all available worktrees:

```bash
kilo worktree list
```

#### Switch Worktree

To switch to a different worktree:

```bash
kilo worktree switch <name>
```

### Running Agents

#### Start Agent

To start an agent in a worktree:

```bash
kilo agent start <agent-name> [--worktree <worktree-name>]
```

#### Stop Agent

To stop a running agent:

```bash
kilo agent stop <agent-id>
```

#### List Agents

To list all running agents:

```bash
kilo agent list
```

## Workflows

### Session Workflows

#### Create Session

To create a new session:

```bash
kilo session create [--name <name>] [--agent <agent-name>]
```

#### List Sessions

To list all sessions:

```bash
kilo session list
```

#### Attach to Session

To attach to an existing session:

```bash
kilo session attach <session-id>
```

### Command Workflows

#### Run Command in Worktree

To run a command in a specific worktree:

```bash
kilo worktree exec <worktree-name> <command> [args...]
```

#### Get Command Output

To get the output of a previous command:

```bash
kilo command output <command-id>
```

## State Management

### State Recovery

#### Check State

To check the current state of an agent:

```bash
kilo state check <agent-id>
```

#### Restore State

To restore a previous state:

```bash
kilo state restore <agent-id> [--snapshot <snapshot-id>]
```

#### List Snapshots

To list available snapshots:

```bash
kilo state snapshots <agent-id>
```

## Skills

### Skill Management

#### List Skills

To list available skills:

```bash
kilo skill list
```

#### Install Skill

To install a skill:

```bash
kilo skill install <skill-name> [--url <git-url>]
```

#### Use Skill

To use a skill in a session:

```bash
kilo skill use <skill-name> [--args <args>]
```

## Commands and Agents

### Built-in Commands

The Kilo system includes several built-in commands:

1. **config** - Configuration management
2. **agent** - Agent management
3. **worktree** - Worktree management
4. **session** - Session management
5. **skill** - Skill management
6. **state** - State management
7. **command** - Command management
8. **history** - Command history management

### User Commands

Users can define their own commands by adding them to the commands/ directory:

```json
{
  "name": "my-command",
  "description": "My custom command",
  "script": "echo \"Hello, World!\""
}
```

## Permissions

### Permission Levels

- **read** - Read access to files and directories
- **write** - Write access to files and directories
- **execute** - Execute access to files and directories
- **admin** - Full administrative access

### Set Permissions

To set permissions for an agent:

```bash
kilo permission set <agent-name> <permission-level>
```

## MCP (Model Context Protocol)

### MCP Configuration

The Kilo system supports MCP (Model Context Protocol) for enhanced agent capabilities:

```json
{
  "mcp": {
    "enabled": true,
    "servers": [
      {
        "name": "filesystem",
        "command": "mcp-server-fs",
        "args": []
      }
    ]
  }
}
```

## Providers

### Provider Configuration

Kilo supports multiple AI providers:

```json
{
  "providers": {
    "default": "openai",
    "openai": {
      "api_key": "your-api-key",
      "model": "gpt-4"
    },
    "anthropic": {
      "api_key": "your-api-key",
      "model": "claude-2"
    }
  }
}
```

## TUI (Text User Interface) Settings

### TUI Configuration

```json
{
  "tui": {
    "theme": "default",
    "layout": "default",
    "fullscreen": false,
    "show_status_bar": true,
    "show_line_numbers": true
  }
}
```

## Advanced Features

### Plugin Architecture

The Kilo system supports plugins for extending functionality:

1. **Plugin Directory**: ~/.config/kilo/plugins/
2. **Plugin Format**: JSON with metadata and commands
3. **Plugin Loading**: Automatic loading on startup

### Command History

The Kilo system maintains command history for each session:

- **History File**: ~/.config/kilo/history/
- **Format**: JSON with timestamps and commands
- **Search**: Filter by agent, command, or time range

### Session Recovery

The Kilo system provides session recovery capabilities:

- **Automatic Saving**: Sessions are automatically saved
- **Checkpointing**: Periodic state snapshots
- **Rollback**: Restore to previous states

## Troubleshooting

### Common Issues

#### Configuration Not Loading

```bash
# Check if kilo.json exists
ls ~/.config/kilo/

# Validate JSON syntax
kilo config validate
```

#### Agent Not Starting

```bash
# Check agent logs
kilo agent logs <agent-id>

# Check worktree status
kilo worktree status <worktree-name>
```

#### Missing Dependencies

```bash
# Install missing dependencies
kilo dependency install <package-name>
```

#### Worktree Not Found

```bash
# List all worktrees
kilo worktree list

# Create missing worktree
kilo worktree create <name>
```

## Examples

### Example: Complete Agent Setup

```bash
# Create worktree
kilo worktree create my-project --type workspace

# Create session
kilo session create --name my-session --agent my-agent

# Start agent
kilo agent start my-agent --worktree my-project

# Run commands
kilo worktree exec my-project "ls -la"
kilo worktree exec my-project "git status"

# Attach to session
kilo session attach my-session

# Use skill
kilo skill use code-analysis --args "--file main.py"
```

### Example: State Management

```bash
# Check current state
kilo state check my-agent

# Create snapshot
kilo state snapshot my-agent

# List snapshots
kilo state snapshots my-agent

# Restore from snapshot
kilo state restore my-agent --snapshot snapshot-123
```

### Example: Permission Management

```bash
# Set permissions
kilo permission set my-agent read
kilo permission set my-agent write

# Check permissions
kilo permission check my-agent
```

## Conclusion

The Kilo AGENTS configuration provides a comprehensive framework for setting up and managing agents in the Kilo system. It includes configuration management, agent management, worktree management, session management, skill management, state management, and advanced features like MCP, plugins, and command history. Use this configuration as a reference for setting up and managing your Kilo agents.
