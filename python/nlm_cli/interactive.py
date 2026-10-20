#!/usr/bin/env python3
"""
Enhanced Interactive NLM Session

Interactive mode for NLM with comprehensive error handling, validation,
and recovery mechanisms.
"""

from prompt_toolkit import PromptSession
from prompt_toolkit.history import FileHistory
from prompt_toolkit.completion import Completer, Completion
from prompt_toolkit.key_binding import KeyBindings
from prompt_toolkit.styles import Style
import asyncio
import threading
import time
from typing import List, Optional, Dict, Any
import os
from pathlib import Path
import sys

# Import error handling system
from nlm_core.error_handling import (
    NLMException, ConfigurationException, BrainException,
    ValidationException, ErrorHandling, ErrorRecovery, Validation
)

class InteractiveErrorHandler:
    """Error handler for interactive NLM session"""
    
    def __init__(self, cli):
        self.cli = cli
        self.error_handler = ErrorHandling()
        self.command_history = []
        self.error_count = 0
        
    def handle_command_error(self, command: str, error: Exception) -> bool:
        """Handle error for a command execution"""
        self.error_count += 1
        
        # Log the error
        self.cli.logger.error(f"Command '{command}' failed: {str(error)}")
        
        # Store in error handler
        if isinstance(error, NLMException):
            self.error_handler.handle_error(error)
        else:
            # Convert non-NLM exceptions
            nlm_error = NLMException(
                ErrorCode.SYSTEM_ERROR,
                f"Error executing command '{command}': {str(error)}",
                cause=error
            )
            self.error_handler.handle_error(nlm_error)
        
        # Check if we should continue
        if self.error_count >= 5:
            self.cli.logger.warning("Too many errors in sequence. Please check configuration.")
            return False
        
        return True
    
    def suggest_recovery(self, error: Exception) -> List[str]:
        """Generate recovery suggestions for an error"""
        suggestions = []
        
        if isinstance(error, ConfigurationException):
            suggestions.extend([
                "Check configuration file syntax",
                "Verify required configuration keys are present",
                "Ensure configuration values are within valid ranges"
            ])
        elif isinstance(error, BrainException):
            suggestions.extend([
                "Check if brain is properly initialized",
                "Verify brain configuration is valid",
                "Try resetting the brain state"
            ])
        elif isinstance(error, ValidationException):
            suggestions.extend([
                "Check input parameters",
                "Verify data types and formats",
                "Ensure values meet validation criteria"
            ])
        elif isinstance(error, MemoryException):
            suggestions.extend([
                "Check available memory",
                "Try closing other applications",
                "Reduce the size of data being processed"
            ])
        else:
            suggestions.extend([
                "Check system resources",
                "Verify network connectivity",
                "Check logs for more details"
            ])
        
        return suggestions

class InteractiveNLMSession:
    def __init__(self, cli):
        self.cli = cli
        self.error_handler = InteractiveErrorHandler(cli)
        self.session = None
        self.running = False
        self.prompt = "nlm> "
        self.setup_session()
        
    def setup_session(self):
        """Setup interactive session with error handling"""
        try:
            from prompt_toolkit import PromptSession
            from prompt_toolkit.history import FileHistory
            from prompt_toolkit.auto_suggest import AutoSuggestFromHistory
            import readline
            
            # Setup readline history
            history_file = Path.home() / ".nlm_history"
            
            self.session = PromptSession(
                history=FileHistory(str(history_file)),
                completer=self._get_completer(),
                auto_suggest=AutoSuggestFromHistory(),
                style=Style.from_dict({
                    'prompt': 'ansibrightgreen bold',
                }),
                enable_history_search=True,
                enable_open_in_editor=True,
                enable_system_prompt=False,
            )
            
            # Setup key bindings for error recovery
            self._setup_error_recovery_key_bindings()
            
        except ImportError:
            self.cli.logger.warning("prompt_toolkit not installed. Interactive mode may be limited.")
            self.session = None
            
    def _get_completer(self):
        """Get command completer with error context"""
        from prompt_toolkit.completion import Completer, Completion
        
        class InteractiveCompleter(Completer):
            def __init__(self, cli):
                self.cli = cli
                self.commands = {
                    "brain": ["create", "load", "save", "simulate", "inspect", "benchmark", "validate"],
                    "world": ["create", "configure", "simulate", "inspect", "export"],
                    "agent": ["create", "train", "evaluate", "explore", "analyze"],
                    "config": ["create", "load", "save", "validate", "compare", "migrate"],
                    "experiment": ["run", "list", "create", "analyze"],
                    "profile": ["start", "stop", "analyze", "report"],
                    "help": [],
                    "exit": [],
                    "quit": [],
                    "status": [],
                    "error": [],
                    "history": [],
                }
                
            def get_completions(self, document, complete_event):
                text = document.text_before_cursor
                words = text.split()
                
                if not words:
                    for cmd in self.commands.keys():
                        yield Completion(cmd, start_position=0)
                    return
                    
                if words[0] in self.commands:
                    cmd = words[0]
                    if len(words) == 1:
                        for subcmd in self.commands[cmd]:
                            yield Completion(subcmd, start_position=-len(words[-1]))
                        return
                        
        return InteractiveCompleter(self.cli)
    
    def _setup_error_recovery_key_bindings(self):
        """Setup key bindings for error recovery"""
        kb = KeyBindings()
        
        @kb.add('c-r')
        def restart_command(event):
            """Restart the session"""
            self.cli.logger.info("Session restart requested")
            # Clear session state
            self.cli.brain = None
            self.cli.world = None
            self.cli.agent = None
            self.cli.config_obj = None
            
        @kb.add('c-e')
        def edit_config(event):
            """Edit configuration"""
            self.cli.logger.info("Configuration edit requested")
            # Implementation would open editor
            
        @kb.add('c-h')
        def show_help(event):
            """Show help"""
            self._show_interactive_help()
            
        self.session.key_bindings = kb
    
    def run(self):
        """Run the interactive session"""
        if not self.session:
            self._run_simple_loop()
            return
            
        self.running = True
        self.cli.logger.info("Starting interactive NLM session")
        
        try:
            while self.running:
                try:
                    command = self._get_command()
                    if command is None:
                        continue
                        
                    self._execute_command(command)
                    
                except KeyboardInterrupt:
                    self.cli.logger.info("Interrupted by user")
                    continue
                    
                except Exception as e:
                    if not self.error_handler.handle_command_error(command if 'command' in locals() else "unknown", e):
                        self.cli.logger.error("Too many errors. Exiting.")
                        break
                        
        except Exception as e:
            self.cli.logger.error(f"Session error: {e}")
        finally:
            self.running = False
            self.cli.logger.info("Interactive session ended")
    
    def _get_command(self) -> Optional[str]:
        """Get command from user"""
        try:
            command = self.session.prompt(self.prompt)
            return command.strip()
        except Exception as e:
            self.cli.logger.error(f"Error getting command: {e}")
            return None
    
    def _execute_command(self, command: str):
        """Execute command with error handling"""
        if not command:
            return
            
        # Add to history
        self.error_handler.command_history.append(command)
        
        # Parse and execute
        try:
            self._parse_and_execute(command)
            
        except NLMException:
            raise
        except Exception as e:
            # Convert to NLM exception
            nlm_error = NLMException(
                ErrorCode.SYSTEM_ERROR,
                f"Error executing command '{command}': {str(e)}",
                context={"command": command},
                cause=e
            )
            self.error_handler.handle_command_error(command, nlm_error)
            raise
    
    def _parse_and_execute(self, command: str):
        """Parse and execute command"""
        parts = command.split()
        cmd = parts[0].lower()
        args = parts[1:]
        
        # Handle special commands
        if cmd == "help":
            self._show_interactive_help()
            return
            
        elif cmd == "exit" or cmd == "quit":
            self.running = False
            self.cli.logger.info("Exiting interactive session")
            return
            
        elif cmd == "status":
            self._show_session_status()
            return
            
        elif cmd == "error":
            self._show_error_history()
            return
            
        elif cmd == "history":
            self._show_command_history()
            return
        
        # Handle NLM CLI commands
        if cmd in ["brain", "world", "agent", "config", "experiment", "profile"]:
            if len(parts) < 2:
                self.cli.logger.error(f"Missing subcommand for {cmd}")
                return
                
            subcmd = parts[1]
            full_args = [cmd, subcmd] + args
            self._execute_cli_command(full_args)
        else:
            self.cli.logger.error(f"Unknown command: {cmd}")
    
    def _execute_cli_command(self, args: List[str]):
        """Execute NLM CLI command"""
        try:
            # Simulate CLI execution (in real implementation, would call actual CLI)
            self.cli.logger.info(f"Executing: {' '.join(args)}")
            
            # For demo purposes, just acknowledge the command
            self.cli.logger.info(f"Command executed: {args[0]} {args[1]}")
            
        except Exception as e:
            raise
    
    def _show_interactive_help(self):
        """Show interactive help"""
        help_text = """
Interactive NLM Session Commands:
=============================== 

General Commands:
  help              - Show this help message
  status            - Show session status
  error             - Show error history
  history           - Show command history
  exit/quit         - Exit interactive session

NLM Commands:
  brain <subcommand>   - Brain operations
  world <subcommand>    - World operations
  agent <subcommand>    - Agent operations
  config <subcommand>   - Configuration operations
  experiment <subcommand> - Experiment operations
  profile <subcommand>  - Profiling operations

Error Recovery:
  Ctrl+R - Restart session
  Ctrl+E - Edit configuration
  Ctrl+H - Show help

Available Subcommands:
  brain: create, load, save, simulate, inspect, benchmark, validate
  world: create, configure, simulate, inspect, export
  agent: create, train, evaluate, explore, analyze
  config: create, load, save, validate, compare, migrate
  experiment: run, list, create, analyze
  profile: start, stop, analyze, report
"""
        print(help_text)
    
    def _show_session_status(self):
        """Show session status with error information"""
        status = {
            "session_running": self.running,
            "error_count": self.error_handler.error_count,
            "command_count": len(self.error_handler.command_history),
            "components_initialized": {
                "brain": self.cli.brain is not None,
                "world": self.cli.world is not None,
                "agent": self.cli.agent is not None,
                "config": self.cli.config_obj is not None,
            }
        }
        
        print("\nSession Status:")
        print("-" * 40)
        for key, value in status.items():
            if isinstance(value, dict):
                print(f"{key}:")
                for sub_key, sub_value in value.items():
                    print(f"  {sub_key}: {sub_value}")
            else:
                print(f"{key}: {value}")
    
    def _show_error_history(self):
        """Show error history with recovery suggestions"""
        print("\nError History:")
        print("-" * 40)
        
        if self.error_handler.error_count == 0:
            print("No errors recorded in this session.")
            return
            
        print(f"Total errors: {self.error_handler.error_count}")
        
        # In a real implementation, would show actual error history
        print("\nError recovery suggestions:")
        suggestions = self.error_handler.suggest_recovery(NLMException(ErrorCode.SYSTEM_ERROR, "sample error"))
        for i, suggestion in enumerate(suggestions, 1):
            print(f"{i}. {suggestion}")
    
    def _show_command_history(self):
        """Show command history"""
        print("\nCommand History:")
        print("-" * 40)
        
        if not self.error_handler.command_history:
            print("No commands executed in this session.")
            return
            
        for i, cmd in enumerate(self.error_handler.command_history[-10:], 1):
            print(f"{i:3d}. {cmd}")
        
        print(f"\nTotal commands executed: {len(self.error_handler.command_history)}")
    
    def _run_simple_loop(self):
        """Run simple command loop when prompt_toolkit is not available"""
        self.cli.logger.warning("Using simple command loop (prompt_toolkit not available)")
        
        while self.running:
            try:
                command = input(self.prompt).strip()
                if not command:
                    continue
                    
                self._execute_command(command)
                
            except KeyboardInterrupt:
                self.cli.logger.info("Interrupted by user")
                continue
                
            except EOFError:
                self.cli.logger.info("End of input")
                break
                
            except Exception as e:
                self.cli.logger.error(f"Command error: {e}")

# Backwards compatibility alias
InteractiveNLMCLI = InteractiveNLMSession

if __name__ == "__main__":
    # Example usage
    from nlm_cli.nlm_cli import NLMCLI
    
    print("Starting Interactive NLM Session...")
    print("Note: This is an example. Install prompt_toolkit for full functionality.")
    
    cli = NLMCLI()
    session = InteractiveNLMSession(cli)
    
    try:
        session.run()
    except KeyboardInterrupt:
        print("\nInterrupted by user")
    except Exception as e:
        print(f"\nError: {e}")
        sys.exit(1)