#!/usr/bin/env python3
"""
NLM Command Line Interface (CLI)
Provides a text-based interface for interacting with NLM simulations.
"""

import sys
import os
import json
import tempfile
from datetime import datetime

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

class NLMCLI:
    def __init__(self):
        self.brain = None
        self.world = None
        self.agent = None
        self.running = False
        
    def print_banner(self):
        print("=" * 70)
        print("NLM (熙然) - Neural Learning Machine CLI")
        print("Phase 2: Enhanced Neural Computation")
        print("=" * 70)
        
    def print_help(self):
        print("\nAvailable commands:")
        print("  help                    - Show this help message")
        print("  exit/quit              - Exit the program")
        print("  create [brain|world|agent] - Create a component")
        print("  config <key> <value>    - Set configuration value")
        print("  get <key>               - Get configuration value")
        print("  status                  - Show current system status")
        print("  simulate <steps>        - Run simulation")
        print("  save <file>             - Save brain state")
        print("  load <file>             - Load brain state")
        print("  reset                   - Reset all components")
        print("  help <command>          - Get help for specific command")
        
    def create_config(self):
        config = pynlm.createDefaultConfig()
        print(f"✓ Created default configuration")
        print(f"  Configuration has {len(config.getKeys())} keys")
        
    def create_brain(self, config=None):
        if config is None:
            config = pynlm.createDefaultConfig()
            
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        print(f"✓ Created brain with {self.brain.getTotalNeuronCount()} neurons")
        print(f"  Synapse count: {self.brain.getTotalSynapseCount()}")
        
    def create_world(self):
        self.world = pynlm.createSimpleWorld()
        print("✓ Created simple world")
        
    def create_agent(self):
        if self.brain is None:
            print("✗ Please create a brain first")
            return
            
        self.agent = pynlm.createAgentBrain(self.brain)
        if self.world is not None:
            self.agent.initialize(self.world)
        print("✓ Created agent brain")
        
    def set_config(self, key, value):
        if self.brain is None:
            print("✗ Please create a brain first")
            return
            
        # Try to parse value as different types
        try:
            if value.lower() == "true":
                val = True
            elif value.lower() == "false":
                val = False
            elif value.isdigit():
                val = int(value)
            elif '.' in value and value.replace('.', '', 1).isdigit():
                val = float(value)
            else:
                val = value
                
            self.brain.getConfig().set(key, val)
            print(f"✓ Set '{key}' = {val}")
        except Exception as e:
            print(f"✗ Failed to set '{key}': {e}")
            
    def get_config(self, key):
        if self.brain is None:
            print("✗ Please create a brain first")
            return
            
        config = self.brain.getConfig()
        if config.has(key):
            value = config.get(key)
            print(f"'{key}' = {value}")
        else:
            print(f"✗ Key '{key}' not found in configuration")
            
    def show_status(self):
        print("\n=== System Status ===")
        if self.brain:
            print(f"Brain: {self.brain.getTotalNeuronCount()} neurons")
            print(f"         {self.brain.getTotalSynapseCount()} synapses")
            print(f"         {self.brain.getFiringNeuronCount()} firing neurons")
            print(f"         {self.brain.getTotalSpikeCount()} total spikes")
            print(f"         Stage: {self.brain.getDevelopmentalStage()}")
            
        if self.world:
            print(f"World: {self.world.getWidth()}x{self.world.getHeight()}")
            print(f"         Agent position: ({self.world.getAgentBody().x}, {self.world.getAgentBody().y})")
            
        print("====================\n")
        
    def run_simulation(self, steps):
        if not self.world or not self.agent or not self.brain:
            print("✗ Please create world, agent, and brain first")
            return
            
        print(f"\nRunning simulation for {steps} steps...")
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get and process sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Run brain
            self.brain.step(step)
            
            # Get action and apply
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Print progress every 20 steps
            if step % 20 == 0:
                print(f"  Step {step}: "
                      f"Firing: {self.brain.getFiringNeuronCount()}, "
                      f"Curiosity: {self.agent.getCuriosityLevel():.2f}")
                    
        print(f"Simulation completed. Final stats:")
        print(f"  Total spikes: {self.brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {self.brain.getAverageFiringRate():.2f}")
        print(f"  Final curiosity level: {self.agent.getCuriosityLevel():.2f}")
        print(f"  Final novelty level: {self.agent.getNoveltyLevel():.2f}")
        
    def save_brain(self, filename):
        if not self.brain:
            print("✗ Please create a brain first")
            return
            
        try:
            self.brain.save(filename)
            print(f"✓ Saved brain to '{filename}'")
        except Exception as e:
            print(f"✗ Failed to save brain: {e}")
            
    def load_brain(self, filename):
        try:
            if not self.brain:
                config = pynlm.createDefaultConfig()
                self.brain = pynlm.createBrain(config)
                self.brain.initialize()
                
            self.brain.load(filename)
            print(f"✓ Loaded brain from '{filename}'")
        except Exception as e:
            print(f"✗ Failed to load brain: {e}")
            
    def reset_all(self):
        self.brain = None
        self.world = None
        self.agent = None
        print("✓ Reset all components")
        
    def handle_command(self, command):
        parts = command.strip().split()
        if not parts:
            return
            
        cmd = parts[0].lower()
        
        if cmd == "help":
            if len(parts) > 1:
                self.show_specific_help(parts[1])
            else:
                self.print_help()
                
        elif cmd == "exit" or cmd == "quit":
            self.running = False
            print("Goodbye!")
            
        elif cmd == "create":
            if len(parts) > 1:
                if parts[1] == "config":
                    self.create_config()
                elif parts[1] == "brain":
                    self.create_brain()
                elif parts[1] == "world":
                    self.create_world()
                elif parts[1] == "agent":
                    self.create_agent()
                else:
                    print(f"✗ Unknown component: {parts[1]}")
            else:
                print("✗ Please specify component to create (config, brain, world, agent)")
                
        elif cmd == "config":
            if len(parts) >= 3:
                self.set_config(parts[1], ' '.join(parts[2:]))
            else:
                print("✗ Usage: config <key> <value>")
                
        elif cmd == "get":
            if len(parts) >= 2:
                self.get_config(parts[1])
            else:
                print("✗ Usage: get <key>")
                
        elif cmd == "status":
            self.show_status()
            
        elif cmd == "simulate":
            if len(parts) >= 2:
                try:
                    steps = int(parts[1])
                    self.run_simulation(steps)
                except ValueError:
                    print("✗ Invalid number of steps")
            else:
                print("✗ Please specify number of steps to simulate")
                
        elif cmd == "save":
            if len(parts) >= 2:
                self.save_brain(parts[1])
            else:
                print("✗ Usage: save <filename>")
                
        elif cmd == "load":
            if len(parts) >= 2:
                self.load_brain(parts[1])
            else:
                print("✗ Usage: load <filename>")
                
        elif cmd == "reset":
            self.reset_all()
            
        else:
            print(f"✗ Unknown command: {cmd}")
            print("Type 'help' for available commands")
            
    def show_specific_help(self, command):
        help_texts = {
            "config": "Set a configuration value: config <key> <value>",
            "get": "Get a configuration value: get <key>",
            "status": "Show current system status: status",
            "simulate": "Run a simulation: simulate <steps>",
            "save": "Save brain state: save <filename>",
            "load": "Load brain state: load <filename>",
            "reset": "Reset all components: reset",
        }
        
        if command in help_texts:
            print(f"\n{help_texts[command]}")
        else:
            print(f"\nNo specific help available for '{command}'")
            

def main():
    print("Loading NLM CLI...")
    
    try:
        import pynlm
        print("✓ NLM Python bindings loaded successfully")
    except ImportError as e:
        print(f"✗ Failed to load NLM Python bindings: {e}")
        print("\nPlease build the NLM Python bindings first.")
        print("Run: pip install -e .")
        return 1
        
    cli = NLMCLI()
    cli.print_banner()
    cli.print_help()
    cli.running = True
    
    print(f"\nType 'help' for available commands or 'exit' to quit.")
    
    try:
        while cli.running:
            try:
                command = input("> ").strip()
                if command:
                    cli.handle_command(command)
            except KeyboardInterrupt:
                print("\nInterrupted. Type 'exit' to quit.")
            except Exception as e:
                print(f"✗ Error: {e}")
                
    except Exception as e:
        print(f"\nError: {e}")
        
    print("Goodbye!")
    return 0

if __name__ == "__main__":
    sys.exit(main())
            
        print("====================\n")
        
    def run_simulation(self, steps):
        if not self.world or not self.agent or not self.brain:
            print("✗ Please create world, agent, and brain first")
            return
            
        print(f"\nRunning simulation for {steps} steps...")
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get and process sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Run brain
            self.brain.step(step)
            
            # Get action and apply
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Print progress every 20 steps
            if step % 20 == 0:
                print(f"  Step {step}: "
                      f"Firing: {self.brain.getFiringNeuronCount()}, "
                      f"Curiosity: {self.agent.getCuriosityLevel():.2f}")
                    
        print(f"Simulation completed. Final stats:")
        print(f"  Total spikes: {self.brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {self.brain.getAverageFiringRate():.2f}")
        print(f"  Final curiosity level: {self.agent.getCuriosityLevel():.2f}")
        print(f"  Final novelty level: {self.agent.getNoveltyLevel():.2f}")
        
    def save_brain(self, filename):
        if not self.brain:
            print("✗ Please create a brain first")
            return
            
        try:
            self.brain.save(filename)
            print(f"✓ Saved brain to '{filename}'")
        except Exception as e:
            print(f"✗ Failed to save brain: {e}")
            
    def load_brain(self, filename):
        try:
            if not self.brain:
                config = pynlm.createDefaultConfig()
                self.brain = pynlm.createBrain(config)
                self.brain.initialize()
                
            self.brain.load(filename)
            print(f"✓ Loaded brain from '{filename}'")
        except Exception as e:
            print(f"✗ Failed to load brain: {e}")
            
    def reset_all(self):
        self.brain = None
        self.world = None
        self.agent = None
        print("✓ Reset all components")
        
    def handle_command(self, command):
        parts = command.strip().split()
        if not parts:
            return
            
        cmd = parts[0].lower()
        
        if cmd == "help":
            if len(parts) > 1:
                self.show_specific_help(parts[1])
            else:
                self.print_help()
                
        elif cmd == "exit" or cmd == "quit":
            self.running = False
            print("Goodbye!")
            
        elif cmd == "create":
            if len(parts) > 1:
                if parts[1] == "config":
                    self.create_config()
                elif parts[1] == "brain":
                    self.create_brain()
                elif parts[1] == "world":
                    self.create_world()
                elif parts[1] == "agent":
                    self.create_agent()
                else:
                    print(f"✗ Unknown component: {parts[1]}")
            else:
                print("✗ Please specify component to create (config, brain, world, agent)")
                
        elif cmd == "config":
            if len(parts) >= 3:
                self.set_config(parts[1], ' '.join(parts[2:]))
            else:
                print("✗ Usage: config <key> <value>")
                
        elif cmd == "get":
            if len(parts) >= 2:
                self.get_config(parts[1])
            else:
                print("✗ Usage: get <key>")
                
        elif cmd == "status":
            self.show_status()
            
        elif cmd == "simulate":
            if len(parts) >= 2:
                try:
                    steps = int(parts[1])
                    self.run_simulation(steps)
                except ValueError:
                    print("✗ Invalid number of steps")
            else:
                print("✗ Please specify number of steps to simulate")
                
        elif cmd == "save":
            if len(parts) >= 2:
                self.save_brain(parts[1])
            else:
                print("✗ Usage: save <filename>")
                
        elif cmd == "load":
            if len(parts) >= 2:
                self.load_brain(parts[1])
            else:
                print("✗ Usage: load <filename>")
                
        elif cmd == "reset":
            self.reset_all()
            
        else:
            print(f"✗ Unknown command: {cmd}")
            print("Type 'help' for available commands")
            
    def show_specific_help(self, command):
        help_texts = {
            "config": "Set a configuration value: config <key> <value>",
            "get": "Get a configuration value: get <key>",
            "status": "Show current system status: status",
            "simulate": "Run a simulation: simulate <steps>",
            "save": "Save brain state: save <filename>",
            "load": "Load brain state: load <filename>",
            "reset": "Reset all components: reset",
        }
        
        if command in help_texts:
            print(f"\n{help_texts[command]}")
        else:
            print(f"\nNo specific help available for '{command}'")
            
def main():
    print("Loading NLM CLI...")
    
    try:
        import pynlm
        print("✓ NLM Python bindings loaded successfully")
    except ImportError as e:
        print(f"✗ Failed to load NLM Python bindings: {e}")
        print("\nPlease build the NLM Python bindings first.")
        print("Run: pip install -e .")
        return 1
        
    cli = NLMCLI()
    cli.print_banner()
    cli.print_help()
    cli.running = True
    
    print(f"\nType 'help' for available commands or 'exit' to quit.")
    
    try:
        while cli.running:
            try:
                command = input("> ").strip()
                if command:
                    cli.handle_command(command)
            except KeyboardInterrupt:
                print("\nInterrupted. Type 'exit' to quit.")
            except Exception as e:
                print(f"✗ Error: {e}")
                
    except Exception as e:
        print(f"\nError: {e}")
        
    print("Goodbye!")
    return 0

if __name__ == "__main__":
    sys.exit(main())