#!/usr/bin/env python3
"""
Advanced NLM Command Line Tools

This module provides advanced command-line utilities for the NLM (Neural Learning Machine)
simulation framework, complementing the basic demo executables.
"""

import argparse
import json
import os
import sys
from pathlib import Path
from typing import Dict, Any, List, Optional

# Add parent directory to path for imports
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import nlm

class NLMToolkit:
    """Advanced toolkit for NLM simulation and analysis."""
    
    def __init__(self, world_size: tuple = (100, 100), vision_size: tuple = (16, 16)):
        """Initialize NLM toolkit with specified world dimensions."""
        self.world = nlm.createSimpleWorld()
        self.world.configure(world_size[0], world_size[1], vision_size[0], vision_size[1])
        self.brain = None
        self.agent = None
        self.episode_history = []
    
    def create_scenario(self, config_path: Optional[str] = None) -> Dict[str, Any]:
        """Create a complete simulation scenario."""
        self.brain = nlm.createBrain(nlm.create_default_config())
        self.agent = nlm.createAgentBrain(self.brain)
        self.brain.initialize()
        
        # Set random agent start position
        import random
        start_x = random.randint(10, self.world.getWidth() - 10)
        start_y = random.randint(10, self.world.getHeight() - 10)
        self.world.setAgentStart(start_x, start_y)
        self.agent.initialize(self.world)
        
        return {
            "brain": self.brain,
            "agent": self.agent,
            "world": self.world,
            "start_position": (start_x, start_y)
        }
    
    def run_episode(self, max_steps: int = 100, save_data: bool = False) -> Dict[str, Any]:
        """Run a single simulation episode."""
        if not self.agent or not self.brain:
            raise RuntimeError("Scenario not created. Call create_scenario() first.")
        
        self.agent.reset()
        self.world.reset()
        
        episode_data = {
            "steps": [],
            "total_reward": 0.0,
            "max_curiosity": 0.0,
            "max_novelty": 0.0,
            "actions_taken": [],
            "sensory_data": []
        }
        
        for step in range(max_steps):
            # Update development
            self.agent.updateDevelopment(step * 0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Record sensory data
            sensory_dict = {
                "vision": percept.getVision(),
                "touch": percept.getTouch(),
                "internal": percept.getInternal(),
                "proprioception": percept.getProprioception(),
                "audio": percept.getAudio()
            }
            episode_data["sensory_data"].append(sensory_dict)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            episode_data["actions_taken"].append(action)
            
            # Apply action
            self.world.applyMotorCommand(action, step * 0.1)
            
            # Generate random reward for simulation
            import random
            reward = random.random() * 2.0 - 1.0  # Range [-1, 1]
            self.agent.applyRewardModulation(reward, self.agent.getPredictionError())
            
            episode_data["total_reward"] += reward
            
            # Record step data
            step_data = {
                "step": step,
                "action": action,
                "reward": reward,
                "total_reward": episode_data["total_reward"],
                "neuromodulation": self.agent.getNeuromodulationLevel(),
                "curiosity": self.agent.getCuriosityLevel(),
                "novelty": self.agent.getNoveltyLevel(),
                "prediction_error": self.agent.getPredictionError(),
                "developmental_stage": self.agent.getDevelopmentalStage()
            }
            
            # Update max values
            episode_data["max_curiosity"] = max(episode_data["max_curiosity"], 
                                             self.agent.getCuriosityLevel())
            episode_data["max_novelty"] = max(episode_data["max_novelty"], 
                                           self.agent.getNoveltyLevel())
            
            episode_data["steps"].append(step_data)
            
            # Check for early termination
            if abs(reward) > 0.9:  # Large reward or punishment
                break
        
        if save_data:
            self.episode_history.append(episode_data)
        
        return episode_data
    
    def analyze_episode(self, episode_data: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze episode data and return statistics."""
        steps = episode_data["steps"]
        
        if not steps:
            return {"error": "No steps recorded"}
        
        # Basic statistics
        total_steps = len(steps)
        rewards = [step["reward"] for step in steps]
        neuromodulation = [step["neuromodulation"] for step in steps]
        curiosity = [step["curiosity"] for step in steps]
        novelty = [step["novelty"] for step in steps]
        
        analysis = {
            "total_steps": total_steps,
            "total_reward": episode_data["total_reward"],
            "average_reward": sum(rewards) / total_steps,
            "reward_std": (sum((r - sum(rewards)/total_steps)**2 for r in rewards) / total_steps)**0.5,
            "max_reward": max(rewards),
            "min_reward": min(rewards),
            "reward_variance": sum((r - sum(rewards)/total_steps)**2 for r in rewards) / total_steps,
            "average_neuromodulation": sum(neuromodulation) / total_steps,
            "max_curiosity": episode_data["max_curiosity"],
            "max_novelty": episode_data["max_novelty"],
            "average_curiosity": sum(curiosity) / total_steps,
            "average_novelty": sum(novelty) / total_steps,
            "action_distribution": self._analyze_action_distribution(episode_data["actions_taken"])
        }
        
        return analysis
    
    def _analyze_action_distribution(self, actions: List) -> Dict[str, float]:
        """Analyze action distribution."""
        action_counts = {}
        for action in actions:
            action_name = self._get_action_name(action)
            action_counts[action_name] = action_counts.get(action_name, 0) + 1
        
        total = len(actions)
        distribution = {action: count / total for action, count in action_counts.items()}
        return distribution
    
    def _get_action_name(self, action: int) -> str:
        """Convert action enum to name."""
        action_names = {
            nlm.MotorCommand.MoveForward: "MoveForward",
            nlm.MotorCommand.MoveBackward: "MoveBackward",
            nlm.MotorCommand.TurnLeft: "TurnLeft",
            nlm.MotorCommand.TurnRight: "TurnRight",
            nlm.MotorCommand.LookLeft: "LookLeft",
            nlm.MotorCommand.LookRight: "LookRight",
            nlm.MotorCommand.Interact: "Interact",
            nlm.MotorCommand.Wait: "Wait"
        }
        return action_names.get(action, f"Unknown({action})")
    
    def save_scenario(self, filepath: str, scenario: Dict[str, Any] = None) -> bool:
        """Save scenario to file."""
        if scenario is None:
            if not self.agent or not self.brain:
                return False
            scenario = {
                "brain": self.brain,
                "agent": self.agent,
                "world": self.world,
                "config": self.brain.getConfig()
            }
        
        try:
            data = {
                "world_size": (self.world.getWidth(), self.world.getHeight()),
                "brain_config": scenario["config"].summary() if "config" in scenario else None,
                "timestamp": str(__import__('datetime').datetime.now()),
                "version": "1.0.0"
            }
            
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2)
            
            return True
        except Exception as e:
            print(f"Error saving scenario: {e}")
            return False
    
    def load_scenario(self, filepath: str) -> bool:
        """Load scenario from file (placeholder implementation)."""
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
            
            print(f"Loaded scenario from {filepath}")
            print(f"World size: {data.get('world_size', 'Unknown')}")
            print(f"Timestamp: {data.get('timestamp', 'Unknown')}")
            
            return True
        except Exception as e:
            print(f"Error loading scenario: {e}")
            return False
    
    def export_data(self, episode_data: Dict[str, Any], filepath: str) -> bool:
        """Export episode data to file."""
        try:
            # Convert data to JSON-serializable format
            export_data = {
                "episode_data": episode_data,
                "exported_at": str(__import__('datetime').datetime.now()),
                "version": "1.0.0"
            }
            
            with open(filepath, 'w') as f:
                json.dump(export_data, f, indent=2)
            
            return True
        except Exception as e:
            print(f"Error exporting data: {e}")
            return False
    
    def compare_episodes(self, episode1: Dict[str, Any], episode2: Dict[str, Any]) -> Dict[str, Any]:
        """Compare two episodes and return differences."""
        analysis1 = self.analyze_episode(episode1)
        analysis2 = self.analyze_episode(episode2)
        
        comparison = {
            "reward_differences": {
                "episode1": episode1["total_reward"],
                "episode2": episode2["total_reward"],
                "difference": episode2["total_reward"] - episode1["total_reward"]
            },
            "analysis1": analysis1,
            "analysis2": analysis2,
            "similarity_score": self._calculate_similarity(analysis1, analysis2)
        }
        
        return comparison
    
    def _calculate_similarity(self, analysis1: Dict[str, Any], analysis2: Dict[str, Any]) -> float:
        """Calculate similarity score between two analyses."""
        # Simple similarity calculation based on key metrics
        metrics = ['average_reward', 'average_curiosity', 'average_novelty', 'reward_variance']
        
        similarities = []
        for metric in metrics:
            val1 = analysis1.get(metric, 0)
            val2 = analysis2.get(metric, 0)
            # Normalize difference (0 = identical, 1 = completely different)
            similarity = max(0, 1 - abs(val1 - val2) / max(abs(val1), abs(val2), 1))
            similarities.append(similarity)
        
        return sum(similarities) / len(similarities) if similarities else 0.0


def main():
    """Main entry point for command line interface."""
    parser = argparse.ArgumentParser(
        description="Advanced NLM Command Line Tools",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Create and run a single episode
  nlm-tools --action run-episode --max-steps 200
  
  # Create a comparison between two episodes
  nlm-tools --action compare --episode1 data1.json --episode2 data2.json
  
  # Export simulation data
  nlm-tools --action export --data episode.json --output analysis.json
  
  # Analyze existing episode data
  nlm-tools --action analyze --input episode.json
        """
    )
    
    # Global options
    parser.add_argument(
        "--world-size", 
        nargs=2, 
        type=int, 
        default=[100, 100],
        help="World dimensions (width height)"
    )
    parser.add_argument(
        "--vision-size",
        nargs=2,
        type=int,
        default=[16, 16],
        help="Vision dimensions (width height)"
    )
    parser.add_argument(
        "--verbose", "-v",
        action="store_true",
        help="Enable verbose output"
    )
    
    # Subcommands
    subparsers = parser.add_subparsers(dest="action", help="Available actions")
    
    # Run episode
    parser_run = subparsers.add_parser("run-episode", help="Run a simulation episode")
    parser_run.add_argument(
        "--max-steps", "-m",
        type=int,
        default=100,
        help="Maximum number of simulation steps"
    )
    parser_run.add_argument(
        "--save-data",
        action="store_true",
        help="Save episode data to history"
    )
    parser_run.add_argument(
        "--output",
        type=str,
        help="Output file for episode data"
    )
    
    # Create scenario
    parser_create = subparsers.add_parser("create-scenario", help="Create a new simulation scenario")
    parser_create.add_argument(
        "--config",
        type=str,
        help="Configuration file path"
    )
    parser_create.add_argument(
        "--output",
        type=str,
        help="Output file for scenario"
    )
    
    # Analyze episode
    parser_analyze = subparsers.add_parser("analyze", help="Analyze episode data")
    parser_analyze.add_argument(
        "--input",
        type=str,
        help="Input episode data file"
    )
    parser_analyze.add_argument(
        "--from-stdout",
        action="store_true",
        help="Read episode data from stdout"
    )
    parser_analyze.add_argument(
        "--detailed",
        action="store_true",
        help="Enable detailed analysis"
    )
    
    # Compare episodes
    parser_compare = subparsers.add_parser("compare", help="Compare two episodes")
    parser_compare.add_argument(
        "--episode1",
        type=str,
        help="First episode data file"
    )
    parser_compare.add_argument(
        "--episode2",
        type=str,
        help="Second episode data file"
    )
    parser_compare.add_argument(
        "--detailed",
        action="store_true",
        help="Enable detailed comparison"
    )
    
    # Export data
    parser_export = subparsers.add_parser("export", help="Export data in different formats")
    parser_export.add_argument(
        "--data",
        type=str,
        help="Input data file"
    )
    parser_export.add_argument(
        "--output",
        type=str,
        help="Output file path"
    )
    parser_export.add_argument(
        "--format",
        choices=["json", "csv", "yaml"],
        default="json",
        help="Output format"
    )
    
    # Load scenario
    parser_load = subparsers.add_parser("load", help="Load saved scenario")
    parser_load.add_argument(
        "--input",
        type=str,
        help="Input scenario file"
    )
    
    # Help and version
    parser.add_argument(
        "--version",
        action="version",
        version="NLM Tools v1.0.0"
    )
    
    # Parse arguments
    args = parser.parse_args()
    
    # Initialize toolkit
    toolkit = NLMToolkit(
        world_size=tuple(args.world_size),
        vision_size=tuple(args.vision_size)
    )
    
    try:
        if args.action == "run-episode":
            episode_data = toolkit.run_episode(
                max_steps=args.max_steps,
                save_data=args.save_data
            )
            
            analysis = toolkit.analyze_episode(episode_data)
            
            if args.verbose:
                print("Episode Analysis:")
                for key, value in analysis.items():
                    print(f"  {key}: {value}")
            
            if args.output:
                toolkit.export_data(episode_data, args.output)
                print(f"Episode data exported to {args.output}")
            
            print(f"Episode completed. Total reward: {episode_data['total_reward']:.2f}")
            
        elif args.action == "create-scenario":
            scenario = toolkit.create_scenario(args.config)
            
            if args.output:
                toolkit.save_scenario(args.output, scenario)
                print(f"Scenario saved to {args.output}")
            
            print("Scenario created successfully")
            
        elif args.action == "analyze":
            if args.from_stdout:
                import sys
                data = json.load(sys.stdin)
            else:
                with open(args.input, 'r') as f:
                    data = json.load(f)
            
            analysis = toolkit.analyze_episode(data)
            
            if args.detailed:
                print("Detailed Analysis:")
                import pprint
                pprint.pprint(analysis)
            else:
                print("Analysis Summary:")
                for key in ['total_steps', 'total_reward', 'average_reward', 'max_reward']:
                    if key in analysis:
                        print(f"  {key}: {analysis[key]:.4f}")
            
        elif args.action == "compare":
            with open(args.episode1, 'r') as f:
                episode1_data = json.load(f)
            
            with open(args.episode2, 'r') as f:
                episode2_data = json.load(f)
            
            comparison = toolkit.compare_episodes(episode1_data, episode2_data)
            
            print("Comparison Results:")
            print(f"  Reward difference: {comparison['reward_differences']['difference']:.4f}")
            print(f"  Similarity score: {comparison['similarity_score']:.4f}")
            
            if args.detailed:
                print("\nDetailed Comparison:")
                for key, value in comparison.items():
                    if key not in ['analysis1', 'analysis2']:
                        print(f"  {key}: {value}")
            
        elif args.action == "export":
            if not args.data or not args.output:
                parser.error("Both --data and --output are required for export")
            
            with open(args.data, 'r') as f:
                data = json.load(f)
            
            # For now, just export as JSON
            toolkit.export_data(data, args.output)
            print(f"Data exported to {args.output}")
            
        elif args.action == "load":
            if not args.input:
                parser.error("--input is required for load")
            
            toolkit.load_scenario(args.input)
            
        else:
            parser.print_help()
            
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        if args.verbose:
            import traceback
            traceback.print_exc()
        sys.exit(1)


if __name__ == "__main__":
    main()