#!/usr/bin/env python3
"""
NLM CLI - Enhanced Command Line Interface for NLM (Neural Learning Machine)

A comprehensive CLI interface with advanced commands and features for NLM.
"""

import sys
import os
import json
import argparse
import subprocess
import tempfile
import shutil
from typing import Dict, List, Any, Optional, Union
from pathlib import Path
from datetime import datetime

# Add the current directory to path to import NLM modules
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import NLM modules
from nlm import (
    createBrain, createDefaultConfig, createSimpleWorld, createAgentBrain,
    NeuronType, ActionType, MotorCommand, DevelopmentalStage
)

# Version information
VERSION = "0.1.0"

class NLMCLI:
    """Main NLM CLI interface class."""
    
    def __init__(self):
        self.config = createDefaultConfig()
        self.brain = None
        self.world = None
        self.agent = None
        self.history = []
        self.current_session = []
        self.output_format = "text"
        self.verbose = False
        self.debug = False
        self.quiet = False
        self.progress_indicators = True
        
    def print_banner(self):
        """Print the NLM banner."""
        banner = """
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║     NLM — 熙然                                                ║
║     Neural Learning Machine                                   ║
║                                                               ║
║     Enhanced Command Line Interface                           ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝
        """
        self.echo(banner)
        
    def echo(self, message: str, level: str = "info"):
        """Print a message with optional formatting."""
        prefix = {
            "info": "",
            "success": "✓ ",
            "warning": "⚠ ",
            "error": "✗ ",
            "debug": "[DEBUG] ",
            "progress": "⏳ "
        }.get(level, "")
        
        if self.output_format == "json":
            if level == "info":
                self.echo_json(message)
        else:
            if self.quiet and level not in ["error", "warning"]:
                return
            print(f"{prefix}{message}", flush=True)
            
    def echo_json(self, data: Dict):
        """Output JSON-formatted data."""
        json.dump(data, sys.stdout, indent=2)
        print()
        
    def run_command(self, command: str, args: List[str] = None) -> bool:
        """Run a CLI command."""
        try:
            # Split command into parts
            cmd_parts = command.split()
            cmd_name = cmd_parts[0]
            
            # Map command names to methods
            command_map = {
                "config": self.cmd_config,
                "brain": self.cmd_brain,
                "experiment": self.cmd_experiment,
                "world": self.cmd_world,
                "monitor": self.cmd_monitor,
                "help": self.cmd_help,
                "version": self.cmd_version,
                "exit": self.cmd_exit,
                "quit": self.cmd_quit,
                "run": self.cmd_run,
                "script": self.cmd_script,
            }
            
            if cmd_name not in command_map:
                self.echo(f"Unknown command: {cmd_name}", "error")
                return False
                
            # Execute the command
            return command_map[cmd_name](args or [])
            
        except Exception as e:
            self.echo(f"Error executing command '{command}': {str(e)}", "error")
            if self.debug:
                import traceback
                self.echo(traceback.format_exc(), "debug")
            return False
            
    def cmd_help(self, args: List[str]):
        """Show help information."""
        if args:
            # Help for specific command
            command = args[0]
            help_map = {
                "config": self.help_config,
                "brain": self.help_brain,
                "experiment": self.help_experiment,
                "world": self.help_world,
                "monitor": self.help_monitor,
                "script": self.help_script,
            }
            
            if command in help_map:
                help_map[command]()
            else:
                self.echo(f"No help available for command: {command}", "warning")
        else:
            # General help
            self.print_general_help()
        return True
        
    def print_general_help(self):
        """Print general help information."""
        help_text = """
NLM CLI - Enhanced Command Line Interface

Available commands:

  config     Configuration management commands
  brain      Brain control commands
  experiment Experiment management
  world      World/environment commands
  monitor    Real-time monitoring
  help       Show help information
  version    Show version information
  exit|quit  Exit the CLI
  run        Run simulation
  script     Execute a command script

Use 'help <command>' for more information about a specific command.

For quick reference, use the following shortcuts:
  nlm --help     Show general help
  nlm --version  Show version
  nlm --config file.cfg  Load configuration from file
        """
        self.echo(help_text)
        
    def help_config(self):
        """Show help for config commands."""
        help_text = """
Configuration Commands:

  nlm config list              List all configuration keys
  nlm config get <key>         Get specific config value
  nlm config set <key> <value> Set config value
  nlm config validate          Validate current configuration
  nlm config save <file>       Save configuration to file
  nlm config load <file>       Load configuration from file
  nlm config schema            Show configuration schema
  nlm config info              Show configuration info
        """
        self.echo(help_text)
        
    def help_brain(self):
        """Show help for brain commands."""
        help_text = """
Brain Commands:

  nlm brain status             Show brain status
  nlm brain reset              Reset brain to initial state
  nlm brain load <file>        Load brain state from checkpoint
  nlm brain save <file>        Save brain state to checkpoint
  nlm brain info               Show brain statistics and metrics
  nlm brain step <steps>       Run brain for specified steps
  nlm brain initialize         Initialize the brain
  nlm brain terminate          Terminate the brain
        """
        self.echo(help_text)
        
    def help_experiment(self):
        """Show help for experiment commands."""
        help_text = """
Experiment Commands:

  nlm experiment run           Run experiments
  nlm experiment list          List available experiments
  nlm experiment info <name>   Show experiment info
  nlm experiment metrics <name> Show experiment metrics
  nlm experiment status        Show experiment status
  nlm experiment create <name> Create new experiment
  nlm experiment delete <name> Delete experiment
        """
        self.echo(help_text)
        
    def help_world(self):
        """Show help for world commands."""
        help_text = """
World Commands:

  nlm world info               Show world state
  nlm world reset              Reset world
  nlm world step               Take a single step
  nml world configure          Configure world parameters
  nml world objects            Manage world objects
  nml world status             Show world status
        """
        self.echo(help_text)
        
    def help_monitor(self):
        """Show help for monitor commands."""
        help_text = """
Monitor Commands:

  nml monitor start            Start monitoring
  nml monitor stop             Stop monitoring
  nml monitor status           Show monitoring status
  nml monitor logs             Show monitoring logs
  nml monitor metrics          Show monitoring metrics
  nml monitor configure        Configure monitoring options
        """
        self.echo(help_text)
        
    def help_script(self):
        """Show help for script commands."""
        help_text = """
Script Commands:

  nml script execute <file>    Execute a command script file
  nml script list              List available scripts
  nml script save <file>       Save current session as script
  nml script info <file>       Show script information
  nml script validate <file>   Validate script
        """
        self.echo(help_text)
        
    def cmd_config(self, args: List[str]):
        """Handle config commands."""
        if not args:
            self.echo("No config subcommand specified", "warning")
            return self.cmd_help(["config"])
            
        subcommand = args[0]
        subargs = args[1:]
        
        config_map = {
            "list": self.config_list,
            "get": self.config_get,
            "set": self.config_set,
            "validate": self.config_validate,
            "save": self.config_save,
            "load": self.config_load,
            "schema": self.config_schema,
            "info": self.config_info,
        }
        
        if subcommand not in config_map:
            self.echo(f"Unknown config subcommand: {subcommand}", "warning")
            return self.cmd_help(["config"])
            
        return config_map[subcommand](subargs)
        
    def config_list(self, args: List[str]):
        """List all configuration keys."""
        keys = self.config.getKeys()
        if not keys:
            self.echo("No configuration keys found.", "info")
            return True
            
        self.echo("Configuration Keys:")
        for key in sorted(keys):
            value = self.config.get(key)
            self.echo(f"  {key} = {value}")
        return True
        
    def config_get(self, args: List[str]):
        """Get specific config value."""
        if len(args) == 0:
            self.echo("No key specified", "warning")
            return False
            
        key = args[0]
        value = self.config.get(key)
        
        if value:
            self.echo(f"{key} = {value}")
        else:
            self.echo(f"Key not found: {key}", "warning")
        return True
        
    def config_set(self, args: List[str]):
        """Set config value."""
        if len(args) < 2:
            self.echo("Usage: config set <key> <value>", "warning")
            return False
            
        key = args[0]
        value = " ".join(args[1:])
        self.config.set(key, value)
        self.echo(f"Set {key} = {value}")
        return True
        
    def config_validate(self, args: List[str]):
        """Validate current configuration."""
        # Simple validation for now
        keys = self.config.getKeys()
        if not keys:
            self.echo("Configuration is empty.", "warning")
            return False
            
        self.echo("Configuration validation passed.")
        return True
        
    def config_save(self, args: List[str]):
        """Save configuration to file."""
        if len(args) == 0:
            self.echo("No filename specified", "warning")
            return False
            
        filename = args[0]
        if self.config.saveToFile(filename):
            self.echo(f"Configuration saved to {filename}")
        else:
            self.echo(f"Failed to save configuration to {filename}", "error")
            return False
        return True
        
    def config_load(self, args: List[str]):
        """Load configuration from file."""
        if len(args) == 0:
            self.echo("No filename specified", "warning")
            return False
            
        filename = args[0]
        if self.config.loadFromFile(filename):
            self.echo(f"Configuration loaded from {filename}")
        else:
            self.echo(f"Failed to load configuration from {filename}", "error")
            return False
        return True
        
    def config_schema(self, args: List[str]):
        """Show configuration schema."""
        self.echo("Configuration Schema:")
        self.echo("(Schema information would be displayed here)")
        return True
        
    def config_info(self, args: List[str]):
        """Show configuration info."""
        self.echo("Configuration Information:")
        self.echo(self.config.summary())
        return True
        
    def cmd_brain(self, args: List[str]):
        """Handle brain commands."""
        if not args:
            self.echo("No brain subcommand specified", "warning")
            return self.cmd_help(["brain"])
            
        subcommand = args[0]
        subargs = args[1:]
        
        brain_map = {
            "status": self.brain_status,
            "reset": self.brain_reset,
            "load": self.brain_load,
            "save": self.brain_save,
            "info": self.brain_info,
            "step": self.brain_step,
            "initialize": self.brain_initialize,
            "terminate": self.brain_terminate,
        }
        
        if subcommand not in brain_map:
            self.echo(f"Unknown brain subcommand: {subcommand}", "warning")
            return self.cmd_help(["brain"])
            
        return brain_map[subcommand](subargs)
        
    def brain_status(self, args: List[str]):
        """Show brain status."""
        if self.brain is None:
            self.echo("Brain is not initialized.", "warning")
            return False
            
        self.echo("Brain Status:")
        self.brain.logStatus()
        return True
        
    def brain_reset(self, args: List[str]):
        """Reset brain to initial state."""
        if self.brain is None:
            self.echo("Brain is not initialized.", "warning")
            return False
            
        self.brain.reset()
        self.brain.initialize()
        self.echo("Brain reset and reinitialized.")
        return True
        
    def brain_load(self, args: List[str]):
        """Load brain state from checkpoint."""
        if len(args) == 0:
            self.echo("No filename specified", "warning")
            return False
            
        filename = args[0]
        if self.brain is None:
            self.echo("Brain is not initialized.", "warning")
            return False
            
        if self.brain.load(filename):
            self.echo(f"Brain state loaded from {filename}")
        else:
            self.echo(f"Failed to load brain state from {filename}", "error")
            return False
        return True
        
    def brain_save(self, args: List[str]):
        """Save brain state to checkpoint."""
        if len(args) == 0:
            self.echo("No filename specified", "warning")
            return False
            
        filename = args[0]
        if self.brain is None:
            self.echo("Brain is not initialized.", "warning")
            return False
            
        if self.brain.save(filename):
            self.echo(f"Brain state saved to {filename}")
        else:
            self.echo(f"Failed to save brain state to {filename}", "error")
            return False
        return True
        
    def brain_info(self, args: List[str]):
        """Show brain statistics and metrics."""
        if self.brain is None:
            self.echo("Brain is not initialized.", "warning")
            return False
            
        self.echo("Brain Statistics:")
        self.echo(f"  Total neurons: {self.brain.getTotalNeuronCount()}")
        self.echo(f"  Total synapses: {self.brain.getTotalSynapseCount()}")
        self.echo(f"  Active neurons: {self.brain.getActiveNeuronCount()}")
        self.echo(f"  Firing neurons: {self.brain.getFiringNeuronCount()}")
        self.echo(f"  Total spikes: {self.brain.getTotalSpikeCount()}")
        self.echo(f"  Average firing rate: {self.brain.getAverageFiringRate()}")
        self.echo(f"  Developmental stage: {self.brain.getDevelopmentalStage()}")
        return True
        
    def brain_step(self, args: List[str]):
        """Run brain for specified steps."""
        if len(args) == 0:
            steps = 1
        else:
            try:
                steps = int(args[0])
            except ValueError:
                self.echo(f"Invalid number of steps: {args[0]}", "warning")
                return False
                
        if self.brain is None:
            self.echo("Brain is not initialized.", "warning")
            return False
            
        for i in range(steps):
            self.brain.step(i)
            if self.progress_indicators:
                self.echo(f"Step {i+1}/{steps}")
                
        self.echo(f"Completed {steps} steps.")
        return True
        
    def brain_initialize(self, args: List[str]):
        """Initialize the brain."""
        if self.brain is None:
            self.brain = createBrain(self.config)
            
        if self.brain.initialize():
            self.echo("Brain initialized successfully.")
        else:
            self.echo("Failed to initialize brain.", "error")
            return False
        return True
        
    def brain_terminate(self, args: List[str]):prefix[1]: /workspace/app
prefix[2]: /workspace/app/python
prefix[3]: /workspace/app/src
prefix[4]: /workspace/app/tests
prefix[5]: /workspace/app/configs
prefix[6]: /workspace/app/docs
prefix[7]: /workspace/app/examples
prefix[8]: /workspace/app/examples
prefix[9]: /workspace/app/examples
prefix[10]: /workspace/app/examples
prefix[11]: /workspace/app/examples
prefix[12]: /workspace/app/examples
prefix[13]: /workspace/app/examples
prefix[14]: /workspace/app/examples
prefix[15]: /workspace/app/examples
prefix[16]: /workspace/app/examples
prefix[16]: /workspace/app/examples
prefix[17]: /workspace/app/examples
prefix[18]: /workspace/app/examples
prefix[19]: /workspace/app/examples
prefix[20]: /workspace/app/examples
prefix[21]: /workspace/app/examples
prefix[22]: /workspace/app/examples
prefix[23]: /workspace/app/examples
prefix[24]: /workspace/app/examples
prefix[25]: /workspace/app/examples
prefix[26]: /workspace/app/examples
prefix[27]: /workspace/app/examples
prefix[28]: /workspace/app/examples
prefix[29]: /workspace/app/examples
prefix[30]: /workspace/app/examples
prefix[31]: /workspace/app/examples
prefix[32]: /workspace/app/examples
prefix[33]: /workspace/app/examples
prefix[34]: /workspace/app/examples
prefix[35]: /workspace/app/examples
prefix[36]: /workspace/app/examples
prefix[37]: /workspace/app/examples
prefix[38]: /workspace/app/examples
prefix[39]: /workspace/app/examples
prefix[40]: /workspace/app/examples
prefix[41]: /workspace/app/examples
prefix[42]: /workspace/app/examples
prefix[43]: /workspace/app/examples
prefix[44]: /workspace/app/examples
prefix[45]: /workspace/app/examples
prefix[46]: /workspace/app/examples
prefix[47]: /workspace/app/examples
prefix[48]: /workspace/app/examples
prefix[49]: /workspace/app/examples
prefix[50]: /workspace/app/examples
prefix[51]: /workspace/app/examples
prefix[52]: /workspace/app/examples
prefix[53]: /workspace/app/examples
prefix[54]: /workspace/app/examples
prefix[55]: /workspace/app/examples
prefix[56]: /workspace/app/examples
prefix[57]: /workspace/app/examples
prefix[58]: /workspace/app/examples
prefix[59]: /workspace/app/examples
prefix[60]: /workspace/app/examples
prefix[61]: /workspace/app/examples
prefix[62]: /workspace/app/examples
prefix[63]: /workspace/app/examples
prefix[64]: /workspace/app/examples
prefix[65]: /workspace/app/examples
prefix[66]: /workspace/app/examples
prefix[67]: /workspace/app/examples
prefix[68]: /workspace/app/examples
prefix[69]: /workspace/app/examples
prefix[70]: /workspace/app/examples
prefix[71]: /workspace/app/examples
prefix[72]: /workspace/app/examples
prefix[73]: /workspace/app/examples
prefix[74]: /workspace/app/examples
prefix[75]: /workspace/app/examples
prefix[76]: /workspace/app/examples
prefix[77]: /workspace/app/examples
prefix[78]: /workspace/app/examples
prefix[79]: /workspace/app/examples
prefix[80]: /workspace/app/examples
prefix[81]: /workspace/app/examples
prefix[82]: /workspace/app/examples
prefix[83]: /workspace/app examples
prefix[84]: /workspace/app examples
prefix[85]: /workspace/app examples
prefix[86]: /workspace/app examples
prefix[87]: /workspace/app examples
prefix[88]: /workspace/app examples
prefix[89]: /workspace/app examples
prefix[90]: /workspace/app examples
prefix[91]: /workspace/app examples
prefix[92]: /workspace/app examples
prefix[93]: /workspace/app examples
prefix[94]: /workspace/app examples
prefix[95]: /workspace/app examples
prefix[96]: /workspace/app examples
prefix[97]: /workspace/app examples
prefix[98]: /workspace/app examples
prefix[99]: /workspace/app examples
prefix[100]: /workspace/app examples
prefix[101]: /workspace/app examples
prefix[102]: /workspace/app examples
prefix[103]: /workspace/app examples
prefix[104]: /workspace/app examples
prefix[105]: /workspace/app examples
prefix[106]: /workspace/app examples
prefix[107]: /workspace/app examples
prefix[108]: /workspace/app examples
prefix[109]: /workspace/app examples
prefix[110]: /workspace/app examples
prefix[111]: /workspace/app examples
prefix[112]: /workspace/app examples
prefix[113]: /workspace/app examples
prefix[114]: /workspace/app examples
prefix[115]: /workspace/app examples
prefix[116]: /workspace/app examples
prefix[117]: /workspace/app examples
prefix[118]: /workspace/app examples
prefix[119]: /workspace/app examples
prefix[120]: /workspace/app examples
prefix[121]: /workspace/app examples
prefix[122]: /workspace/app examples
prefix[123]: /workspace/app examples
prefix[124]: /workspace/app examples
prefix[125]: /workspace/app examples
prefix[126]: /workspace/app examples
prefix[127]: /workspace/app examples
prefix[128]: /workspace/app examples
prefix[129]: /workspace/app examples
prefix[130]: /workspace/app examples
prefix[131]: /workspace/app examples
prefix[132]: /workspace/app examples
prefix[133]: /workspace/app examples
prefix[134]: /workspace/app examples
prefix[135]: /workspace/app examples
prefix[136]: /workspace/app examples
prefix[137]: /workspace/app examples
prefix[138]: /workspace/app examples
prefix[139]: /workspace/app examples
prefix[140]: /workspace/app examples
prefix[141]: /workspace/app examples
prefix[142]: /workspace/app examples
prefix[143]: /workspace/app examples
prefix[144]: /workspace/app examples
prefix[145]: /workspace/app examples
prefix[146]: /workspace/app examples
prefix[147]: /workspace/app examples
prefix[148]: /workspace/app examples
prefix[149]: /workspace/app examples
prefix[150]: /workspace/app examples
prefix[151]: /workspace/app examples
prefix[152]: /workspace/app examples
prefix[153]: /workspace/app examples
prefix[154]: /workspace/app examples
prefix[155]: /workspace/app examples
prefix[156]: /workspace/app examples
prefix[157]: /workspace/app examples
prefix[158]: /workspace/app examples
prefix[159]: /workspace/app examples
prefix[160]: /workspace/app examples
prefix[161]: /workspace/app examples
prefix[162]: /workspace/app examples
prefix[163]: /workspace/app examples
prefix[164]: /workspace/app examples
prefix[165]: /workspace/app examples
prefix[166]: /workspace/app examples
prefix[167]: /workspace/app examples
prefix[168]: /workspace/app examples
prefix[169]: /workspace/app examples
prefix[170]: /workspace/app examples
prefix[171]: /workspace/app examples
prefix[172]: /workspace/app examples
prefix[173]: /workspace/app examples
prefix[174]: /workspace/app examples
prefix[175]: /workspace/app examples
prefix[176]: /workspace/app examples
prefix[177]: /workspace/app examples
prefix[178]: /workspace/app examples
prefix[179]: /workspace/app examples
prefix[180]: /workspace/app examples
prefix[181]: /workspace/app examples
prefix[182]: /workspace/app examples
prefix[183]: /workspace/app examples
prefix[184]: /workspace/app examples
prefix[185]: /workspace/app examples
prefix[186]: /workspace/app examples
prefix[187]: /workspace/app examples
prefix[188]: /workspace/app examples
prefix[189]: /workspace/app examples
prefix[190]: /workspace/app examples
prefix[191]: /workspace/app examples
prefix[192]: /workspace/app examples
prefix[193]: /workspace/app examples
prefix[194]: /workspace/app examples
prefix[195]: /workspace/app examples
prefix[196]: /workspace/app examples
prefix[197]: /workspace/app examples
prefix[198]: /workspace/app examples
prefix[199]: /workspace/app examples
prefix[200]: /workspace/app examples
prefix[201]: /workspace/app examples
prefix[202]: /workspace/app examples
prefix[203]: /workspace/app examples
prefix[204]: /workspace/app examples
prefix[205]: /workspace/app examples
prefix[206]: /workspace/app examples
prefix[207]: /workspace/app examples
prefix[208]: /workspace/app examples
prefix[209]: /workspace/app examples
prefix[210]: /workspace/app examples
prefix[211]: /workspace/app examples
prefix[212]: /workspace/app examples
prefix[213]: /workspace/app examples
prefix[214]: /workspace/app examples
prefix[215]: /workspace/app examples
prefix[216]: /workspace/app examples
prefix[217]: /workspace/app examples
prefix[218]: /workspace/app examples
prefix[219]: /workspace/app examples
prefix[220]: /workspace/app examples
prefix[221]: /workspace/app examples
prefix[222]: /workspace/app examples
prefix[223]: /workspace/app examples
prefix[224]: /workspace/app examples
prefix[225]: /workspace/app examples
prefix[226]: /workspace/app examples
prefix[227]: /workspace/app examples
prefix[228]: /workspace/app examples
prefix[229]: /workspace/app examples
prefix[230]: /workspace/app examples
prefix[231]: /workspace/app examples
prefix[232]: /workspace/app examples
prefix[233]: /workspace/app examples
prefix[234]: /workspace/app examples
prefix[235]: /workspace/app examples
prefix[236]: /workspace/app examples
prefix[237]: /workspace/app examples
prefix[238]: /workspace/app examples
prefix[239]: /workspace/app examples
prefix[240]: /workspace/app examples
prefix[241]: /workspace/app examples
prefix[242]: /workspace/app examples
prefix[243]: /workspace/app examples
prefix[244]: /workspace/app examples
prefix[245]: /workspace/app examples
prefix[246]: /workspace/app examples
prefix[247]: /workspace/app examples
prefix[248]: /workspace/app examples
prefix[249]: /workspace/app examples
prefix[250]: /workspace/app examples
prefix[251]: /workspace/app examples
prefix[252]: /workspace/app examples
prefix[253]: /workspace/app examples
prefix[254]: /workspace/app examples
prefix[255]: /workspace/app examples
prefix[256]: /workspace/app examples
prefix[257]: /workspace/app examples
prefix[258]: /workspace/app examples
prefix[259]: /workspace/app examples
prefix[260]: /workspace/app examples
prefix[261]: /workspace/app examples
prefix[262]: /workspace/app examples
prefix[263]: /workspace/app examples
prefix[264]: /workspace/app examples
prefix[265]: /workspace/app examples
prefix[266]: /workspace/app examples
prefix[267]: /workspace/app examples
prefix[268]: /workspace/app examples
prefix[269]: /workspace/app examples
prefix[270]: /workspace/app examples
prefix[271]: /workspace/app examples
prefix[272]: /workspace/app examples
prefix[273]: /workspace/app examples
prefix[274]: /workspace/app examples
prefix[275]: /workspace/app examples
prefix[276]: /workspace/app examples
prefix[277]: /workspace/app examples
prefix[278]: /workspace/app examples
prefix[279]: /workspace/app examples
prefix[280]: /workspace/app examples
prefix[281]: /workspace/app examples
prefix[282]: /workspace/app examples
prefix[283]: /workspace/app examples
prefix[284]: /workspace/app examples
prefix[285]: /workspace/app examples
prefix[286]: /workspace/app examples
prefix[287]: /workspace/app examples
prefix[288]: /workspace/app examples
prefix[289]: /workspace/app examples
prefix[290]: /workspace/app examples
prefix[291]: /workspace/app examples
prefix[292]: /workspace/app examples
prefix[293]: /workspace/app examples
prefix[294]: /workspace/app examples
prefix[295]: /workspace/app examples
prefix[296]: /workspace/app examples
prefix[297]: /workspace/app examples
prefix[298]: /workspace/app examples
prefix[299]: /workspace/app examples
prefix[300]: /workspace/app examples
prefix[301]: /workspace/app examples
prefix[302]: /workspace/app examples
prefix[303]: /workspace/app examples
prefix[304]: /workspace/app examples
prefix[305]: /workspace/app examples
prefix[306]: /workspace/app examples
prefix[307]: /workspace/app examples
prefix[308]: /workspace/app examples
prefix[309]: /workspace/app examples
prefix[310]: /workspace/app examples
prefix[311]: /workspace/app examples
prefix[312]: /workspace/app examples
prefix[313]: /workspace/app examples
prefix[314]: /workspace/app examples
prefix[315]: /workspace/app examples
prefix[316]: /workspace/app examples
prefix[317]: /workspace/app examples
prefix[318]: /workspace/app examples
prefix[319]: /workspace/app examples
prefix[320]: /workspace/app examples
prefix[321]: /workspace/app examples
prefix[322]: /workspace/app examples
prefix[323]: /workspace/app examples
prefix[324]: /workspace/app examples
prefix[325]: /workspace/app examples
prefix[326]: /workspace/app examples
prefix[327]: /workspace/app examples
prefix[328]: /workspace/app examples
prefix[329]: /workspace/app examples
prefix[330]: /workspace/app examples
prefix[331]: /workspace/app examples
prefix[332]: /workspace/app examples
prefix[333]: /workspace/app examples
prefix[334]: /workspace/app examples
prefix[335]: /workspace/app examples
prefix[336]: /workspace/app examples
prefix[337]: /workspace/app examples
prefix[338]: /workspace/app examples
prefix[339]: /workspace/app examples
prefix[340]: /workspace/app examples
prefix[341]: /workspace/app examples
prefix[342]: /workspace/app examples
prefix[343]: /workspace/app examples
prefix[344]: /workspace/app examples
prefix[345]: /workspace/app examples
prefix[346]: /workspace/app examples
prefix[347]: /workspace/app examples
print(f"  {self.brain.logStatus}")
    `:--
    def brain_terminate(self, args: List[str]):
        """Terminate the brain."""
        if self.brain is not initialized:
            self.echo("Brain is not initialized.", "warning")
            return False
        
        self.brain = None
        self.echo("Brain terminated.")
        return True

    def cmd_version(self, args: List[str]):
        """Show version information."""
        version_info = {
            "version": VERSION,
            "description": "Enhanced CLI for NLM",
            "python_bindings": "Available",
            "config_system": "Enhanced",
            "last_updated": "2026-11-09"
        }
        
        if self.output_format == "json":
            self.echo_json(version_info)
        else:
            self.echo(f"NLM CLI Version {VERSION}")
            self.echo(f"Description: Enhanced Command Line Interface for NLM")
            self.echo(f"Python bindings: Available")
            self.echo(f"Configuration system: Enhanced")
            self.echo(f"Last updated: 2026-11-09")
        return True
        
    def cmd_exit(self, args: List[str]):
        """Exit the CLI."""
        self.echo("Exiting NLM CLI. Goodbye!")
        return False  # Signal to exit
        
    def cmd_quit(self, args: List[str]):
        """Quit the CLI."""
        return self.cmd_exit(args)
        
    def cmd_run(self, args: List[str]):
        """Run simulation."""
        if self.brain is None:
            if not self.brain_initialize([]):
                return False
                
        if self.world is None:
            self.world = createSimpleWorld()
            
        if self.agent is None:
            self.agent = createAgentBrain(self.brain)
            
        self.echo("Running simulation...")
        
        for i in range(100):  # Default to 100 steps
            if self.verbose:
                self.echo(f"Step {i}")
                
            self.world.update(0.1)
            
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            self.brain.step(i)
            
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            if self.progress_indicators:
                self.echo(f"Step {i+1}/100", "progress")
                
        self.echo("Simulation completed!")
        return True
        
    def cmd_script(self, args: List[str]):
        """Handle script commands."""
        if not args:
            self.echo("No script subcommand specified", "warning")
            return self.cmd_help(["script"])
            
        subcommand = args[0]
        subargs = args[1:]
        
        script_map = {
            "execute": self.script_execute,
            "list": self.script_list,
            "save": self.script_save,
            "info": self.script_info,
            "validate": self.script_validate,
        }
        
        if subcommand not in script_map:
            self.echo(f"Unknown script subcommand: {subcommand}", "warning")
            return self.cmd_help(["script"])
            
        return script_map[subcommand](subargs)
        
    def script_execute(self, args: List[str]):
        """Execute a command script file."""
        if len(args) == 0:
            self.echo("No script file specified", "warning")
            return False
            
        filename = args[0]
        if not os.path.exists(filename):
            self.echo(f"Script file not found: {filename}", "error")
            return False
            
        self.echo(f"Executing script: {filename}")
        
        try:
            with open(filename, 'r') as f:
                script_content = f.read()
                
            # Parse script commands
            commands = self.parse_script_commands(script_content)
            
            for cmd in commands:
                self.run_command(cmd)
                
            self.echo(f"Script {filename} completed successfully.")
        except Exception as e:
            self.echo(f"Error executing script {filename}: {str(e)}", "error")
            return False
            
        return True
        
    def script_list(self, args: List[str]):
        """List available scripts."""
        script_dir = ".nlm_scripts"
        if os.path.exists(script_dir):
            scripts = [f for f in os.listdir(script_dir) if f.endswith('.nlm')]
            
            if not scripts:
                self.echo("No scripts found.", "info")
                return True
                
            self.echo("Available Scripts:")
            for script in sorted(scripts):
                script_path = os.path.join(script_dir, script)
                self.echo(f"  {script}")
        else:
            self.echo("No scripts directory found.", "info")
        return True
        
    def script_save(self, args: List[str]):
        """Save current session as script."""
        if len(args) == 0:
            self.echo("No filename specified", "warning")
            return False
            
        filename = args[0]
        script_dir = ".nlm_scripts"
        os.makedirs(script_dir, exist_ok=True)
        
        script_path = os.path.join(script_dir, filename)
        
        # Create script content
        script_content = "# NLM Script\n"
        script_content += f"# Generated by NLM CLI on {datetime.now().isoformat()}\n"
        script_content += f"# Configuration:\n"
        
        for key in self.config.getKeys():
            value = self.config.get(key)
            script_content += f"nlm config set {key} {value}\n"
            
        script_content += "\n# Save brain state\n"
        if self.brain is not None:
            script_content += "nlm brain save brain_checkpoint.nml\n"
            
        script_content += "\n# Run simulation\n"
        script_content += "nlm run\n"
        
        try:
            with open(script_path, 'w') as f:
                f.write(script_content)
                
            self.echo(f"Script saved to {script_path}")
        except Exception as e:
            self.echo(f"Error saving script: {str(e)}", "error")
            return False
        return True
        
    def script_info(self, args: List[str]):
        """Show script information."""
        if len(args) == 0:
            self.echo("No script file specified", "warning")
            return False
            
        filename = args[0]
        if not os.path.exists(filename):
            self.echo(f"Script file not found: {filename}", "error")
            return False
            
        try:
            with open(filename, 'r') as f:
                script_content = f.read()
                
            self.echo(f"Script Information: {filename}")
            self.echo(f"Size: {len(script_content)} characters")
            self.echo(f"Lines: {len(script_content.splitlines())}")
            
            # Count commands
            commands = self.parse_script_commands(script_content)
            self.echo(f"Estimated commands: {len(commands)}")
            
        except Exception as e:
            self.echo(f"Error reading script: {str(e)}", "error")
            return False
        return True
        
    def script_validate(self, args: List[str]):
        """Validate script."""
        if len(args) == 0:
            self.echo("No script file specified", "warning")
            return False
            
        filename = args[0]
        if not os.path.exists(filename):
            self.echo(f"Script file not found: {filename}", "error")
            return False
            
        try:
            with open(filename, 'r') as f:
                script_content = f.read()
                
            commands = self.parse_script_commands(script_content)
            
            if not commands:
                self.echo("Warning: Script contains no commands", "warning")
                return True
                
            self.echo(f"Script validation passed. Contains {len(commands)} commands.")
            return True
            
        except Exception as e:
            self.echo(f"Error validating script: {str(e)}", "error")
            return False
            
    def parse_script_commands(self, script_content: str) -> List[str]:
        """Parse script content into commands."""
        commands = []
        for line in script_content.splitlines():
            line = line.strip()
            if line and not line.startswith('#'):
                commands.append(line)
        return commands
        
    def run(self):
        """Run the NLM CLI."""
        self.print_banner()
        
        # Check if a script file was provided
        if len(sys.argv) > 1 and sys.argv[1].endswith('.nlm'):
            script_file = sys.argv[1]
            self.echo(f"Executing script file: {script_file}")
            self.cmd_script(["execute", script_file])
            return
            
        # Show welcome message
        self.echo("Welcome to NLM CLI!")
        self.echo("Type 'help' for more information.")
        
        # Main CLI loop
        while True:
            try:
                # Get command from user
                command = input(f"nlm> ").strip()
                
                if not command:
                    continue
                    
                # Add to history
                self.history.append(command)
                self.current_session.append(command)
                
                # Check for exit commands
                if command.lower() in ["exit", "quit", "bye"]:
                    return self.cmd_exit([])
                    
                # Run the command
                success = self.run_command(command)
                
                if not success:
                    self.echo("Command failed.", "warning")
                    
            except KeyboardInterrupt:
                self.echo("\nInterrupted. Type 'exit' to quit.")
                continue
            except EOFError:
                self.echo("\nGoodbye!")
                break
            except Exception as e:
                self.echo(f"Unexpected error: {str(e)}", "error")
                if self.debug:
                    import traceback
                    self.echo(traceback.format_exc(), "debug")


def main():
    """Main entry point for the NLM CLI."""
    cli = NLMCLI()
    
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="NLM - Enhanced Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm                    Start interactive CLI
  nlm --help             Show help information
  nlm --version          Show version information
  nlm --config config.json Load configuration from file
  nlm --script myscript.nlm Execute script file
  nlm --output json        Set output format to JSON
  nlm --verbose          Enable verbose output
  nlm --debug            Enable debug output
  nlm --quiet            Suppress normal output
        """
    )
    
    parser.add_argument("--help", action="store_true", help="Show help information")
    parser.add_argument("--version", action="store_true", help="Show version information")
    parser.add_argument("--config", help="Load configuration from file")
    parser.add_argument("--script", help="Execute a script file and exit")
    parser.add_argument("--output", choices=["text", "json"], default="text", help="Output format")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    parser.add_argument("--debug", action="store_true", help="Enable debug output")
    parser.add_argument("--quiet", action="store_true", help="Suppress normal output")
    parser.add_argument("--no-progress", action="store_true", help="Disable progress indicators")
    
    args, remaining_args = parser.parse_known_args()
    
    # Set CLI options
    cli.output_format = args.output
    cli.verbose = args.verbose
    cli.debug = args.debug
    cli.quiet = args.quiet
    cli.progress_indicators = not args.no_progress
    
    # Load configuration if specified
    if args.config:
        if cli.config.loadFromFile(args.config):
            cli.echo(f"Loaded configuration from {args.config}")
        else:
            cli.echo(f"Failed to load configuration from {args.config}", "error")
            sys.exit(1)
    
    # Show help or version if requested
    if args.help:
        cli.cmd_help([])
        sys.exit(0)
        
    if args.version:
        cli.cmd_version([])
        sys.exit(0)
    
    # Execute script if specified
    if args.script:
        cli.cmd_script(["execute", args.script])
        sys.exit(0)
    
    # Run the CLI
    try:
        cli.run()
    except KeyboardInterrupt:
        print("\nInterrupted. Exiting...")
        sys.exit(1)
    except Exception as e:
        cli.echo(f"Error: {str(e)}", "error")
        sys.exit(1)


if __name__ == "__main__":
    main()