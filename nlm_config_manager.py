#!/usr/bin/env python3
"""
NLM Configuration Manager

Advanced configuration management tools for NLM experiments.
"""

import json
import argparse
import sys
import time
from pathlib import Path
import pynlm

class NLMConfigManager:
    """Manager for NLM configurations and experiments."""
    
    def __init__(self, config_dir="configs"):
        self.config_dir = Path(config_dir)
        self.config_dir.mkdir(exist_ok=True)
        
    def create_preset(self, name, description="", config_dict=None):
        """Create a preset configuration."""
        config = pynlm.createDefaultConfig()
        
        if config_dict:
            for key, value in config_dict.items():
                config.set(key, value)
        
        preset = {
            "name": name,
            "description": description,
            "config": config_dict or {},
            "created_at": time.time()
        }
        
        # Save to file
        preset_file = self.config_dir / f"{name}.json"
        with open(preset_file, 'w') as f:
            json.dump(preset, f, indent=2)
        
        return preset
    
    def load_preset(self, name):
        """Load a preset configuration."""
        preset_file = self.config_dir / f"{name}.json"
        if not preset_file.exists():
            raise FileNotFoundError(f"Preset '{name}' not found")
        
        with open(preset_file, 'r') as f:
            preset = json.load(f)
        
        config = pynlm.createDefaultConfig()
        if preset.get("config"):
            for key, value in preset["config"].items():
                config.set(key, value)
        
        return config, preset
    
    def list_presets(self):
        """List all available presets."""
        presets = []
        for preset_file in self.config_dir.glob("*.json"):
            with open(preset_file, 'r') as f:
                preset = json.load(f)
                presets.append(preset)
        
        if not presets:
            print("No presets found.")
            return
        
        print("Available NLM Presets:")
        print("-" * 50)
        for preset in presets:
            print(f"Name: {preset['name']}")
            print(f"Description: {preset['description']}")
            print(f"Created: {preset.get('created_at', 'Unknown')}")
            print(f"Config keys: {list(preset.get('config', {}).keys())}")
            print("-" * 50)

def create_experiment_template(args):
    """Create an experiment template."""
    template = {
        "experiment_name": args.name,
        "description": args.description,
        "created_at": time.time(),
        "parameters": {
            "steps": args.steps,
            "timestep": args.timestep,
            "width": args.width,
            "height": args.height,
            "vision_width": args.vision_width,
            "vision_height": args.vision_height,
            "start_x": args.start_x,
            "start_y": args.start_y
        },
        "configurations": [
            {
                "name": "baseline",
                "profile": "balanced",
                "neuron_count": args.neurons
            }
        ]
    }
    
    # Save template
    output_file = Path(args.output)
    with open(output_file, 'w') as f:
        json.dump(template, f, indent=2)
    
    print(f"Experiment template saved to {output_file}")
    return template

def run_experiment_from_template(template_file):
    """Run an experiment from a template."""
    with open(template_file, 'r') as f:
        template = json.load(f)
    
    print(f"Running experiment: {template['experiment_name']}")
    print(f"Description: {template['description']}")
    
    for config_template in template["configurations"]:
        print(f"\nConfiguration: {config_template['name']}")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Apply profile
        if config_template.get('profile') == 'exploration':
            config.set("plasticity.curiosity.enable", True)
            config.set("neuromod.curiosity.enable", True)
            config.set("neuromod.novelty.enable", True)
            config.set("brain.neuron_count", 2000)
        elif config_template.get('profile') == 'exploitation':
            config.set("plasticity.curiosity.enable", False)
            config.set("neuromod.curiosity.enable", False)
            config.set("neuromod.novelty.enable", False)
            config.set("plasticity.stdp.enable", True)
            config.set("brain.synapse_density", 0.08)
        else:  # balanced
            config.set("plasticity.curiosity.enable", True)
            config.set("neuromod.curiosity.enable", True)
            config.set("neuromod.novelty.enable", True)
            config.set("plasticity.stdp.enable", True)
            config.set("plasticity.hebbian.enable", True)
            config.set("brain.synapse_density", 0.12)
        
        # Apply custom settings
        if config_template.get('neuron_count'):
            config.set("brain.neuron_count", config_template['neuron_count'])
        
        # Run simulation
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.enableAllFeatures()
        
        world = pynlm.createSimpleWorld()
        params = template["parameters"]
        world.configure(
            width=params["width"],
            height=params["height"],
            visionWidth=params["vision_width"],
            visionHeight=params["vision_height"]
        )
        world.reset()
        world.setAgentStart(params["start_x"], params["start_y"])
        agent.initialize(world)
        
        for step in range(template["parameters"]["steps"]):
            world.update(template["parameters"]["timestep"])
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
            agent.applyRewardModulation(0.0, 0.0)
            agent.updateDevelopment(template["parameters"]["timestep"])
        
        # Print results
        print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Final novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"  Final prediction error: {agent.getPredictionError():.3f}")
        print(f"  Development stage: {agent.getDevelopmentalStage()}")
    
    print("\nExperiment completed!")

def main():
    parser = argparse.ArgumentParser(
        description="NLM Configuration Manager and Experiment Tools"
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Create preset command
    preset_parser = subparsers.add_parser("create-preset", help="Create a preset configuration")
    preset_parser.add_argument("name", help="Name of the preset")
    preset_parser.add_argument("--description", help="Description of the preset")
    preset_parser.add_argument("--config-file", help="JSON file with configuration settings")
    
    # Load preset command
    load_parser = subparsers.add_parser("load-preset", help="Load a preset configuration")
    load_parser.add_argument("name", help="Name of the preset to load")
    
    # List presets command
    list_parser = subparsers.add_parser("list-presets", help="List all available presets")
    
    # Create experiment template command
    template_parser = subparsers.add_parser("create-template", help="Create an experiment template")
    template_parser.add_argument("name", help="Name of the experiment")
    template_parser.add_argument("--description", help="Description of the experiment")
    template_parser.add_argument("--steps", type=int, default=1000, help="Number of steps")
    template_parser.add_argument("--timestep", type=float, default=0.1, help="Timestep")
    template_parser.add_argument("--width", type=int, default=20, help="World width")
    template_parser.add_argument("--height", type=int, default=20, help="World height")
    template_parser.add_argument("--vision-width", type=int, default=8, help="Vision width")
    template_parser.add_argument("--vision-height", type=int, default=8, help="Vision height")
    template_parser.add_argument("--start-x", type=float, default=10.0, help="Starting x")
    template_parser.add_argument("--start-y", type=float, default=10.0, help="Starting y")
    template_parser.add_argument("--neurons", type=int, default=1000, help="Number of neurons")
    template_parser.add_argument("--output", default="experiment_template.json", help="Output template file")
    
    # Run experiment command
    run_parser = subparsers.add_parser("run-template", help="Run experiment from template")
    run_parser.add_argument("template_file", help="Template file to run")
    
    args = parser.parse_args()
    
    if args.command == "create-preset":
        config_dict = {}
        if args.config_file:
            with open(args.config_file, 'r') as f:
                config_dict = json.load(f)
        
        manager = NLMConfigManager()
        preset = manager.create_preset(
            args.name,
            args.description or f"Preset configuration for {args.name}",
            config_dict
        )
        print(f"Preset '{args.name}' created successfully!")
        
    elif args.command == "load-preset":
        manager = NLMConfigManager()
        try:
            config, preset = manager.load_preset(args.name)
            print(f"Preset '{args.name}' loaded successfully!")
            print(f"Description: {preset.get('description', 'No description')}")
        except FileNotFoundError as e:
            print(f"Error: {e}")
            sys.exit(1)
            
    elif args.command == "list-presets":
        manager = NLMConfigManager()
        manager.list_presets()
        
    elif args.command == "create-template":
        template = create_experiment_template(args)
        print("Experiment template created successfully!")
        
    elif args.command == "run-template":
        run_experiment_from_template(args.template_file)
    
    print("\nConfiguration manager completed!")

if __name__ == "__main__":
    main()
