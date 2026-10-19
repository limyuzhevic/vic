#!/usr/bin/env python3
"""
Phase 6 Integration Demo - NLM (Neural Learning Machine)

This is the main entry point for the Phase 6 integration demo.
It provides a comprehensive demonstration of all brain systems
working together.

Key Features:
- Integration verification of all brain systems
- Memory, neuromodulation, and prediction demonstrations
- Checkpoint and replay system testing
- Development and plasticity demonstrations
- Performance monitoring and analysis
- Educational examples and tutorials

Usage:
    python phase6_demo.py [options]
"""

import sys
import os
import argparse
import json
import logging
from pathlib import Path
from datetime import datetime
from typing import Dict, Any, Optional

# Add the current directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import NLM modules
try:
    from pynlm.core import Config, createDefaultConfig
    from pynlm.environment import SimpleWorld
    from pynlm.agent import AgentBrain
    from pynlm.experiments import Phase6IntegratedExperiment, Phase6Config, Phase6IntegrationResult
except ImportError as e:
    print(f"Error importing NLM modules: {e}")
    print("Please ensure the Python bindings are properly installed.")
    sys.exit(1)

class Phase6IntegrationDemo:
    """
    Main demo class for Phase 6 integration.
    
    This class provides a comprehensive demonstration of all brain systems
    working together in Phase 6 of the NLM project.
    """
    
    def __init__(self, config_file: Optional[str] = None):
        """
        Initialize the Phase 6 integration demo.
        
        Args:
            config_file: Path to configuration file (JSON format)
        """
        self.logger = self._setup_logging()
        self.config = self._load_config(config_file)
        self.experiment = Phase6IntegratedExperiment()
        self.results: Optional[Phase6IntegrationResult] = None
        
        self.logger.info("Phase 6 Integration Demo initialized")
    
    def _setup_logging(self) -> logging.Logger:
        """
        Setup logging for the demo.
        
        Returns:
            Configured logger instance
        """
        logger = logging.getLogger("phase6_demo")
        logger.setLevel(logging.INFO)
        
        # Console handler
        console_handler = logging.StreamHandler()
        console_handler.setLevel(logging.INFO)
        
        # File handler
        log_dir = Path("logs")
        log_dir.mkdir(exist_ok=True)
        log_file = log_dir / f"phase6_demo_{datetime.now().strftime('%Y%m%d_%H%M%S')}.log"
        file_handler = logging.FileHandler(log_file)
        file_handler.setLevel(logging.DEBUG)
        
        # Formatters
        console_formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        file_formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(filename)s:%(lineno)d - %(message)s'
        )
        
        console_handler.setFormatter(console_formatter)
        file_handler.setFormatter(file_formatter)
        
        logger.addHandler(console_handler)
        logger.addHandler(file_handler)
        
        return logger
    
    def _load_config(self, config_file: Optional[str]) -> Dict[str, Any]:
        """
        Load configuration from file or use defaults.
        
        Args:
            config_file: Path to configuration file
            
        Returns:
            Configuration dictionary
        """
        config = {
            "neuronCount": 500,
            "maxSteps": 2000,
            "enableCheckpointing": True,
            "enableReplay": True,
            "enableDevelopment": True,
            "checkpointPath": "/tmp/nlm_phase6_checkpoint.bin",
            "outputDir": "phase6_results",
            "verbose": False
        }
        
        if config_file and os.path.exists(config_file):
            try:
                with open(config_file, 'r') as f:
                    file_config = json.load(f)
                config.update(file_config)
                self.logger.info(f"Loaded configuration from {config_file}")
            except Exception as e:
                self.logger.warning(f"Failed to load config file {config_file}: {e}")
        
        return config
    
    def run(self, custom_config: Optional[Dict[str, Any]] = None) -> Phase6IntegrationResult:
        """
        Run the Phase 6 integration demo.
        
        Args:
            custom_config: Optional custom configuration dictionary
            
        Returns:
            Integration results
        """
        self.logger.info("Starting Phase 6 Integration Demo")
        
        # Merge custom config
        if custom_config:
            self.config.update(custom_config)
        
        # Create Phase 6 configuration
        phase6_config = Phase6Config()
        phase6_config.neuronCount = self.config["neuronCount"]
        phase6_config.maxSteps = self.config["maxSteps"]
        phase6_config.enableCheckpointing = self.config["enableCheckpointing"]
        phase6_config.enableReplay = self.config["enableReplay"]
        phase6_config.enableDevelopment = self.config["enableDevelopment"]
        phase6_config.checkpointPath = self.config["checkpointPath"]
        
        # Create output directory
        output_dir = Path(self.config["outputDir"])
        output_dir.mkdir(exist_ok=True)
        
        # Run integration experiment
        self.logger.info("Running Phase 6 integration experiment")
        self.results = self.experiment.run(phase6_config)
        
        # Display results
        self._display_results()
        
        # Save results
        self._save_results(output_dir)
        
        # Generate report
        self._generate_report(output_dir)
        
        self.logger.info("Phase 6 Integration Demo completed successfully")
        
        return self.results
    
    def _display_results(self):
        """Display integration results."""
        if not self.results:
            self.logger.error("No results to display")
            return
        
        print("\n" + "="*60)
        print("PHASE 6 INTEGRATION RESULTS")
        print("="*60)
        print(f"Total Reward: {self.results.totalReward:.4f}")
        print(f"Average Firing Rate: {self.results.avgFiringRate:.4f}")
        print(f"Memory Episodes Stored: {self.results.memoryEpisodesStored:.0f}")
        print(f"Dopamine Level: {self.results.dopamineLevel:.4f}")
        print(f"Wall Clock Time: {self.results.totalWallClockTime:.2f}s")
        print()
        print("INTEGRATION STATUS:")
        print(f"  Working Memory: {'CONNECTED' if self.results.memoryWorkingMemoryIntegrated else 'DISCONNECTED'}")
        print(f"  Episodic Memory: {'CONNECTED' if self.results.memoryEpisodicMemoryIntegrated else 'DISCONNECTED'}")
        print(f"  Neuromodulation: {'CONNECTED' if self.results.neuromodulationIntegrated else 'DISCONNECTED'}")
        print(f"  Prediction: {'CONNECTED' if self.results.predictionIntegrated else 'DISCONNECTED'}")
        print(f"  Development: {'CONNECTED' if self.results.developmentIntegrated else 'DISCONNECTED'}")
        print(f"  Checkpointing: {'WORKING' if self.results.checkpointingWorks else 'NOT WORKING'}")
        print("="*60)
        print()
    
    def _save_results(self, output_dir: Path):
        """
        Save results to files.
        
        Args:
            output_dir: Output directory path
        """
        if not self.results:
            self.logger.error("No results to save")
            return
        
        # Save results as JSON
        results_file = output_dir / "integration_results.json"
        results_dict = {
            "totalReward": self.results.totalReward,
            "avgFiringRate": self.results.avgFiringRate,
            "memoryEpisodesStored": self.results.memoryEpisodesStored,
            "noveltyLevel": self.results.noveltyLevel,
            "curiosityLevel": self.results.curiosityLevel,
            "dopamineLevel": self.results.dopamineLevel,
            "totalWallClockTime": self.results.totalWallClockTime,
            "memoryWorkingMemoryIntegrated": self.results.memoryWorkingMemoryIntegrated,
            "memoryEpisodicMemoryIntegrated": self.results.memoryEpisodicMemoryIntegrated,
            "neuromodulationIntegrated": self.results.neuromodulationIntegrated,
            "predictionIntegrated": self.results.predictionIntegrated,
            "developmentIntegrated": self.results.developmentIntegrated,
            "checkpointingWorks": self.results.checkpointingWorks
        }
        
        with open(results_file, 'w') as f:
            json.dump(results_dict, f, indent=2)
        
        self.logger.info(f"Results saved to {results_file}")
    
    def _generate_report(self, output_dir: Path):
        """
        Generate comprehensive report.
        
        Args:
            output_dir: Output directory path
        """
        report_file = output_dir / "INTEGRATION_REPORT.md"
        
        with open(report_file, 'w') as f:
            f.write("# Phase 6 Integration Report\n\n")
            f.write(f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n")
            
            f.write("## Configuration\n")
            for key, value in self.config.items():
                f.write(f"- {key}: {value}\n")
            f.write("\n")
            
            if self.results:
                f.write("## Results Summary\n")
                f.write(f"- Total Reward: {self.results.totalReward:.4f}\n")
                f.write(f"- Average Firing Rate: {self.results.avgFiringRate:.4f}\n")
                f.write(f"- Memory Episodes Stored: {self.results.memoryEpisodesStored:.0f}\n")
                f.write(f"- Dopamine Level: {self.results.dopamineLevel:.4f}\n")
                f.write(f"- Wall Clock Time: {self.results.totalWallClockTime:.2f}s\n\n")
                
                f.write("## Integration Status\n")
                f.write(f"- Working Memory: {'CONNECTED' if self.results.memoryWorkingMemoryIntegrated else 'DISCONNECTED'}\n")
                f.write(f"- Episodic Memory: {'CONNECTED' if self.results.memoryEpisodicMemoryIntegrated else 'DISCONNECTED'}\n")
                f.write(f"- Neuromodulation: {'CONNECTED' if self.results.neuromodulationIntegrated else 'DISCONNECTED'}\n")
                f.write(f"- Prediction: {'CONNECTED' if self.results.predictionIntegrated else 'DISCONNECTED'}\n")
                f.write(f"- Development: {'CONNECTED' if self.results.developmentIntegrated else 'DISCONNECTED'}\n")
                f.write(f"- Checkpointing: {'WORKING' if self.results.checkpointingWorks else 'NOT WORKING'}\n")
        
        self.logger.info(f"Report generated at {report_file}")


def main():
    """Main function."""
    parser = argparse.ArgumentParser(
        description="Phase 6 Integration Demo - NLM (Neural Learning Machine)"
    )
    
    parser.add_argument(
        "--neurons", type=int, default=500,
        help="Number of neurons (default: 500)"
    )
    
    parser.add_argument(
        "--steps", type=int, default=2000,
        help="Number of simulation steps (default: 2000)"
    )
    
    parser.add_argument(
        "--checkpoint", action="store_true", default=True,
        help="Enable checkpoint saving/loading (default: True)"
    )
    
    parser.add_argument(
        "--no-checkpoint", action="store_false", dest="checkpoint",
        help="Disable checkpoint saving/loading"
    )
    
    parser.add_argument(
        "--replay", action="store_true", default=True,
        help="Enable memory replay (default: True)"
    )
    
    parser.add_argument(
        "--no-replay", action="store_false", dest="replay",
        help="Disable memory replay"
    )
    
    parser.add_argument(
        "--development", action="store_true", default=True,
        help="Enable developmental processes (default: True)"
    )
    
    parser.add_argument(
        "--no-development", action="store_false", dest="development",
        help="Disable developmental processes"
    )
    
    parser.add_argument(
        "--config", type=str, default=None,
        help="Load configuration from JSON file"
    )
    
    parser.add_argument(
        "--output", type=str, default="phase6_results",
        help="Output directory for results (default: phase6_results)"
    )
    
    parser.add_argument(
        "--verbose", action="store_true", default=False,
        help="Enable verbose logging"
    )
    
    parser.add_argument(
        "--quick", action="store_true", default=False,
        help="Run quick verification with minimal settings"
    )
    
    args = parser.parse_args()
    
    # Create demo
    demo = Phase6IntegrationDemo(args.config)
    
    # Configure with command line arguments
    config = {
        "neuronCount": args.neurons,
        "maxSteps": args.steps,
        "enableCheckpointing": args.checkpoint,
        "enableReplay": args.replay,
        "enableDevelopment": args.development,
        "outputDir": args.output,
        "verbose": args.verbose
    }
    
    if args.quick:
        # Quick verification
        config.update({
            "neuronCount": 100,
            "maxSteps": 500,
            "enableCheckpointing": False,
            "enableReplay": False,
            "enableDevelopment": False
        })
    
    # Run demo
    results = demo.run(config)
    
    # Exit with error code if integration failed
    if not all([
        results.memoryWorkingMemoryIntegrated,
        results.memoryEpisodicMemoryIntegrated,
        results.neuromodulationIntegrated,
        results.predictionIntegrated,
        results.developmentIntegrated
    ]):
        print("\nWARNING: Some systems are not properly integrated!")
        sys.exit(1)
    
    print("\nSUCCESS: Phase 6 integration completed successfully!")
    sys.exit(0)


if __name__ == "__main__":
    main()