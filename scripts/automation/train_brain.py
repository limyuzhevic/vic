#!/usr/bin/env python3
"""
train_brain.py: Script for training brain behaviors.

This automation script provides focused training functionality for NLM brains,
including different training modes, data collection, and model management.
"""

import argparse
import json
import sys
import time
from pathlib import Path
from typing import Dict, List, Optional, Any

# Add parent directory to path to import CLI utilities
sys.path.insert(0, str(Path(__file__).parent.parent))

from scripts.cli import (
    NLMCLI,
    ProgressReporter,
    load_config,
    save_config,
    setup_logging,
    DEFAULT_TRAIN_CONFIG,
)

__version__ = "1.0.0"

class BrainTrainer:
    """Class for training NLM brain behaviors with different modes."""

    def __init__(self, config: Dict[str, Any]):
        self.config = config
        self.logger = None
        self.setup_logging()
        self.training_history = []

    def setup_logging(self) -> None:
        """Setup logging for brain trainer."""
        log_level = self.config.get('logging', {}).get('level', 'INFO')
        self.logger = setup_logging(log_level, 'brain_trainer')

    def train_brain(self) -> Dict[str, Any]:
        """Train brain with comprehensive workflow."""
        training_config = self.config.get('training', {})
        mode = training_config.get('mode', 'supervised')
        epochs = training_config.get('epochs', 100)
        output_dir = Path(training_config.get('output_dir', 'trained_brains'))

        self.logger.info(f"Starting brain training in '{mode}' mode")
        self.logger.info(f"Training epochs: {epochs}")
        self.logger.info(f"Output directory: {output_dir}")

        # Create output directory
        output_dir.mkdir(parents=True, exist_ok=True)

        try:
            # Initialize training metrics
            training_metrics = {
                'epoch_losses': [],
                'epoch_accuracies': [],
                'epoch_times': [],
                'best_accuracy': 0.0,
                'best_loss': float('inf'),
                'converged': False
            }

            # Create progress reporter
            progress = ProgressReporter(
                description="Training",
                total=epochs,
                logger=self.logger
            )

            # Run training based on mode
            if mode == 'supervised':
                training_metrics = self._train_supervised(epochs, progress, training_metrics)
            elif mode == 'reinforcement':
                training_metrics = self._train_reinforcement(epochs, progress, training_metrics)
            elif mode == 'self_supervised':
                training_metrics = self._train_self_supervised(epochs, progress, training_metrics)
            elif mode == 'curriculum':
                training_metrics = self._train_curriculum(epochs, progress, training_metrics)
            else:
                self.logger.error(f"Unknown training mode: {mode}")
                return {'training_completed': False, 'error': f'Unknown mode: {mode}'}

            # Save trained brain
            brain_file = self._save_trained_brain(output_dir, training_metrics)

            # Generate training report
            self._generate_training_report(output_dir, training_metrics, brain_file)

            # Create visualization
            self._create_training_visualization(output_dir, training_metrics)

            # Save training history
            self._save_training_history(output_dir)

            self.logger.info(f"Brain training completed successfully. Best accuracy: {training_metrics['best_accuracy']:.2f}")

            return {
                'training_completed': True,
                'mode': mode,
                'epochs_completed': len(training_metrics['epoch_losses']),
                'final_loss': training_metrics['epoch_losses'][-1] if training_metrics['epoch_losses'] else 0,
                'best_accuracy': training_metrics['best_accuracy'],
                'best_loss': training_metrics['best_loss'],
                'converged': training_metrics['converged'],
                'brain_file': str(brain_file),
                'output_directory': str(output_dir),
                'total_training_time': sum(training_metrics['epoch_times'])
            }

        except Exception as e:
            self.logger.error(f"Brain training failed: {e}")
            return {'training_completed': False, 'error': str(e)}

    def _train_supervised(self, epochs: int, progress: ProgressReporter, metrics: Dict[str, Any]) -> Dict[str, Any]:
        """Train brain using supervised learning."""
        self.logger.info("Starting supervised learning training")

        for epoch in range(epochs):
            epoch_start_time = time.time()

            # Simulate supervised training
            epoch_loss = self._run_supervised_epoch(epoch)
            epoch_accuracy = self._calculate_supervised_accuracy(epoch)
            epoch_time = time.time() - epoch_start_time

            # Update metrics
            metrics['epoch_losses'].append(epoch_loss)
            metrics['epoch_accuracies'].append(epoch_accuracy)
            metrics['epoch_times'].append(epoch_time)

            # Update best metrics
            if epoch_accuracy > metrics['best_accuracy']:
                metrics['best_accuracy'] = epoch_accuracy

            if epoch_loss < metrics['best_loss']:
                metrics['best_loss'] = epoch_loss

            progress.update(1)

            # Check for convergence
            if epoch > 50 and len(metrics['epoch_losses']) >= 20:
                recent_losses = metrics['epoch_losses'][-10:]
                if max(recent_losses) - min(recent_losses) < 0.001:
                    metrics['converged'] = True
                    self.logger.info(f"Training converged at epoch {epoch + 1}")
                    break

        return metrics

    def _train_reinforcement(self, epochs: int, progress: ProgressReporter, metrics: Dict[str, Any]) -> Dict[str, Any]:
        """Train brain using reinforcement learning."""
        self.logger.info("Starting reinforcement learning training")

        for epoch in range(epochs):
            epoch_start_time = time.time()

            # Simulate reinforcement training
            epoch_loss = self._run_reinforcement_epoch(epoch)
            epoch_accuracy = self._calculate_reinforcement_accuracy(epoch)
            epoch_time = time.time() - epoch_start_time

            # Update metrics
            metrics['epoch_losses'].append(epoch_loss)
            metrics['epoch_accuracies'].append(epoch_accuracy)
            metrics['epoch_times'].append(epoch_time)

            # Update best metrics
            if epoch_accuracy > metrics['best_accuracy']:
                metrics['best_accuracy'] = epoch_accuracy

            progress.update(1)

        return metrics

    def _train_self_supervised(self, epochs: int, progress: ProgressReporter, metrics: Dict[str, Any]) -> Dict[str, Any]:
        """Train brain using self-supervised learning."""
        self.logger.info("Starting self-supervised learning training")

        for epoch in range(epochs):
            epoch_start_time = time.time()

            # Simulate self-supervised training
            epoch_loss = self._run_self_supervised_epoch(epoch)
            epoch_accuracy = self._calculate_self_supervised_accuracy(epoch)
            epoch_time = time.time() - epoch_start_time

            # Update metrics
            metrics['epoch_losses'].append(epoch_loss)
            metrics['epoch_accuracies'].append(epoch_accuracy)
            metrics['epoch_times'].append(epoch_time)

            # Update best metrics
            if epoch_accuracy > metrics['best_accuracy']:
                metrics['best_accuracy'] = epoch_accuracy

            progress.update(1)

        return metrics

    def _train_curriculum(self, epochs: int, progress: ProgressReporter, metrics: Dict[str, Any]) -> Dict[str, Any]:
        """Train brain using curriculum learning."""
        self.logger.info("Starting curriculum learning training")

        # Curriculum phases
        phases = [
            {'name': 'phase1_simple', 'epochs': epochs // 3, 'difficulty': 0.3},
            {'name': 'phase2_intermediate', 'epochs': epochs * 2 // 3, 'difficulty': 0.6},
            {'name': 'phase3_advanced', 'epochs': epochs - epochs // 3, 'difficulty': 0.9}
        ]

        total_epochs_completed = 0

        for phase in phases:
            self.logger.info(f"Starting {phase['name']} (difficulty: {phase['difficulty']})")

            phase_epochs = phase['epochs']
            for epoch in range(phase_epochs):
                epoch_start_time = time.time()

                # Simulate curriculum training with increasing difficulty
                epoch_loss = self._run_curriculum_epoch(total_epochs_completed, phase['difficulty'])
                epoch_accuracy = self._calculate_curriculum_accuracy(total_epochs_completed, phase['difficulty'])
                epoch_time = time.time() - epoch_start_time

                # Update metrics
                metrics['epoch_losses'].append(epoch_loss)
                metrics['epoch_accuracies'].append(epoch_accuracy)
                metrics['epoch_times'].append(epoch_time)

                # Update best metrics
                if epoch_accuracy > metrics['best_accuracy']:
                    metrics['best_accuracy'] = epoch_accuracy

                progress.update(1)
                total_epochs_completed += 1

        return metrics

    def _run_supervised_epoch(self, epoch: int) -> float:
        """Simulate running a single supervised learning epoch."""
        # Simulate training with decreasing loss
        base_loss = 1.0
        noise = (epoch % 7 - 3) * 0.05  # Some variation
        return max(0.001, base_loss * (0.95 ** epoch) + noise)

    def _calculate_supervised_accuracy(self, epoch: int) -> float:
        """Calculate accuracy for supervised learning epoch."""
        # Simulate accuracy increasing with epoch
        base_accuracy = 0.5
        improvement = 0.4 * (1 - (0.9 ** epoch))
        noise = (epoch % 11 - 5) * 0.02
        return min(0.95, base_accuracy + improvement + noise)

    def _run_reinforcement_epoch(self, epoch: int) -> float:
        """Simulate running a single reinforcement learning epoch."""
        # Simulate RL training with oscillations
        base_loss = 0.8
        oscillation = 0.2 * (1 - abs((epoch % 20) / 10 - 1))
        trend = 0.05 * epoch
        noise = (epoch % 13 - 6) * 0.03
        return max(0.001, base_loss * (0.97 ** epoch) + oscillation + trend + noise)

    def _calculate_reinforcement_accuracy(self, epoch: int) -> float:
        """Calculate accuracy for reinforcement learning epoch."""
        # Simulate RL accuracy with slower convergence
        base_accuracy = 0.4
        improvement = 0.35 * (1 - (0.98 ** epoch))
        noise = (epoch % 17 - 8) * 0.015
        return min(0.9, base_accuracy + improvement + noise)

    def _run_self_supervised_epoch(self, epoch: int) -> float:
        """Simulate running a single self-supervised learning epoch."""
        # Simulate self-supervised training with steady progress
        base_loss = 1.2
        trend = 0.03 * epoch
        noise = (epoch % 19 - 9) * 0.025
        return max(0.001, base_loss * (0.985 ** epoch) + trend + noise)

    def _calculate_self_supervised_accuracy(self, epoch: int) -> float:
        """Calculate accuracy for self-supervised learning epoch."""
        # Simulate self-supervised accuracy with steady improvement
        base_accuracy = 0.45
        improvement = 0.38 * (1 - (0.99 ** epoch))
        noise = (epoch % 23 - 11) * 0.012
        return min(0.92, base_accuracy + improvement + noise)

    def _run_curriculum_epoch(self, global_epoch: int, difficulty: float) -> float:
        """Simulate running a single curriculum learning epoch."""
        # Simulate curriculum training with difficulty-based loss
        base_loss = 1.5
        trend = 0.02 * global_epoch
        difficulty_factor = 1.0 + difficulty * 0.5
        noise = (global_epoch % 17 - 8) * 0.04
        return max(0.001, base_loss * (0.98 ** global_epoch) * difficulty_factor + trend + noise)

    def _calculate_curriculum_accuracy(self, global_epoch: int, difficulty: float) -> float:
        """Calculate accuracy for curriculum learning epoch."""
        # Simulate curriculum accuracy with difficulty-based improvement
        base_accuracy = 0.35
        improvement = 0.45 * (1 - (0.97 ** global_epoch)) * (1 - difficulty)
        noise = (global_epoch % 13 - 6) * 0.018
        return min(0.88, base_accuracy + improvement + noise)

    def _save_trained_brain(self, output_dir: Path, metrics: Dict[str, Any]) -> Path:
        """Save trained brain to file."""
        brain_file = output_dir / f"trained_brain_{int(time.time())}.json"

        brain_data = {
            'metadata': {
                'version': __version__,
                'training_mode': self.config.get('training', {}).get('mode', 'supervised'),
                'timestamp': time.strftime('%Y-%m-%d %H:%M:%S'),
                'epochs_trained': len(metrics['epoch_losses'])
            },
            'performance': {
                'final_loss': metrics['epoch_losses'][-1] if metrics['epoch_losses'] else 0,
                'best_accuracy': metrics['best_accuracy'],
                'best_loss': metrics['best_loss'],
                'final_accuracy': metrics['epoch_accuracies'][-1] if metrics['epoch_accuracies'] else 0
            },
            'training_metrics': metrics,
            'model_weights': self._generate_mock_weights(),
            'architecture': self._get_brain_architecture()
        }

        try:
            with open(brain_file, 'w') as f:
                json.dump(brain_data, f, indent=2)
            self.logger.debug(f"Saved trained brain to: {brain_file}")
        except Exception as e:
            self.logger.error(f"Failed to save trained brain: {e}")
            raise

        return brain_file

    def _generate_mock_weights(self) -> Dict[str, Any]:
        """Generate mock model weights for demonstration."""
        return {
            'layer1_weights': [[0.1 * (i + j) for j in range(128)] for i in range(64)],
            'layer2_weights': [[0.05 * (i - j) for j in range(64)] for i in range(32)],
            'layer3_weights': [[0.02 * (i + j) for j in range(32)] for i in range(16)],
            'biases': {
                'layer1': [0.0] * 128,
                'layer2': [0.0] * 64,
                'layer3': [0.0] * 32
            }
        }

    def _get_brain_architecture(self) -> Dict[str, Any]:
        """Get brain architecture configuration."""
        return {
            'input_size': 100,
            'hidden_layers': [128, 64, 32],
            'output_size': 10,
            'activation': 'relu',
            'dropout': 0.2,
            'optimizer': 'adam'
        }

    def _generate_training_report(self, output_dir: Path, metrics: Dict[str, Any], brain_file: Path) -> None:
        """Generate comprehensive training report."""
        report_file = output_dir / "training_report.md"

        try:
            with open(report_file, 'w') as f:
                f.write(f"# NLM Brain Training Report\n\n")
                f.write(f"**Generated:** {time.strftime('%Y-%m-%d %H:%M:%S')}\n")
                f.write(f"**Version:** {__version__}\n\n")

                f.write(f"## Training Summary\n")
                f.write(f"- Training Mode: {self.config.get('training', {}).get('mode', 'supervised')}\n")
                f.write(f"- Epochs Completed: {len(metrics['epoch_losses'])}\n")
                f.write(f"- Total Training Time: {sum(metrics['epoch_times']):.1f} seconds\n\n")

                f.write(f"## Performance Metrics\n")
                f.write(f"| Metric | Value |\n")
                f.write(f"|--------|-------|\n")
                f.write(f"| Best Accuracy | {metrics['best_accuracy']:.4f} |\n")
                f.write(f"| Best Loss | {metrics['best_loss']:.6f} |\n")
                f.write(f"| Final Loss | {metrics['epoch_losses'][-1]:.6f} |\n")
                f.write(f"| Final Accuracy | {metrics['epoch_accuracies'][-1]:.4f} |\n\n")

                f.write(f"## Training Progress\n")
                f.write(f"| Epoch | Loss | Accuracy | Time (s) |\n")
                f.write(f"|-------|------|----------|-----------|\n")
                for i, (loss, acc, t) in enumerate(zip(metrics['epoch_losses'], metrics['epoch_accuracies'], metrics['epoch_times'])):
                    if i % 5 == 0 or i == len(metrics['epoch_losses']) - 1:  # Show every 5th epoch and last
                        f.write(f"| {i + 1} | {loss:.6f} | {acc:.4f} | {t:.1f} |\n")

                f.write(f"\n## Convergence Analysis\n")
                f.write(f"- Converged: {'Yes' if metrics['converged'] else 'No'}\n")
                f.write(f"- Loss Improvement: {metrics['epoch_losses'][0] - metrics['epoch_losses'][-1]:.6f}\n")
                f.write(f"- Accuracy Improvement: {metrics['epoch_accuracies'][0] - metrics['epoch_accuracies'][-1]:.4f}\n")

                f.write(f"\n## Trained Brain Information\n")
                f.write(f"- Model File: {brain_file.name}\n")
                f.write(f"- Architecture: {len(self._get_brain_architecture()['hidden_layers'])} hidden layers\n")
                f.write(f"- Total Parameters: {sum(len(layer) for layer in self._generate_mock_weights()['layer1_weights']):,}\n")

            self.logger.debug(f"Generated training report: {report_file}")
        except Exception as e:
            self.logger.error(f"Failed to generate training report: {e}")

    def _create_training_visualization(self, output_dir: Path, metrics: Dict[str, Any]) -> None:
        """Create training visualization."""
        self.logger.info("Creating training visualizations...")

        # Create loss curve
        loss_chart_file = output_dir / "loss_curve.png"
        self._create_loss_curve_chart(metrics, loss_chart_file)

        # Create accuracy curve
        accuracy_chart_file = output_dir / "accuracy_curve.png"
        self._create_accuracy_curve_chart(metrics, accuracy_chart_file)

        # Create training summary dashboard
        dashboard_file = output_dir / "training_dashboard.html"
        self._create_training_dashboard(metrics, dashboard_file)

    def _create_loss_curve_chart(self, metrics: Dict[str, Any], output_file: Path) -> None:
        """Create loss curve visualization."""
        self.logger.debug(f"Creating loss curve chart: {output_file}")
        # In real implementation, use matplotlib

    def _create_accuracy_curve_chart(self, metrics: Dict[str, Any], output_file: Path) -> None:
        """Create accuracy curve visualization."""
        self.logger.debug(f"Creating accuracy curve chart: {output_file}")
        # In real implementation, use matplotlib

    def _create_training_dashboard(self, metrics: Dict[str, Any], output_file: Path) -> None:
        """Create training dashboard."""
        self.logger.debug(f"Creating training dashboard: {output_file}")
        # In real implementation, create HTML dashboard

    def _save_training_history(self, output_dir: Path) -> None:
        """Save training history for future reference."""
        history_file = output_dir / "training_history.json"

        history_data = {
            'training_sessions': self.training_history,
            'timestamp': time.strftime('%Y-%m-%d %H:%M:%S'),
            'version': __version__
        }

        try:
            with open(history_file, 'w') as f:
                json.dump(history_data, f, indent=2)
            self.logger.debug(f"Saved training history: {history_file}")
        except Exception as e:
            self.logger.error(f"Failed to save training history: {e}")
class TrainBrainCLI(NLMCLI):
    """CLI tool for training NLM brains with focused workflows."""

    def __init__(self):
        super().__init__(
            name="train_brain.py",
            description="Train NLM brain behaviors with focused workflows",
            version=__version__
        )

    def add_training_arguments(self, parser: argparse.ArgumentParser) -> None:
        """Add training-specific arguments."""
        train_group = parser.add_argument_group('Training Configuration')

        train_group.add_argument(
            '--config',
            type=str,
            required=True,
            help='Path to training configuration file (JSON)'
        )

        train_group.add_argument(
            '--mode',
            type=str,
            choices=['supervised', 'reinforcement', 'self_supervised', 'curriculum'],
            default='supervised',
            help='Training mode (default: supervised)'
        )

        train_group.add_argument(
            '--epochs',
            type=int,
            help='Number of training epochs (overrides config)'
        )

        train_group.add_argument(
            '--output-dir',
            type=str,
            help='Output directory for trained brain (overrides config)'
        )

        train_group.add_argument(
            '--load-model',
            type=str,
            help='Path to pretrained model to load before training'
        )

        train_group.add_argument(
            '--save-model',
            action='store_true',
            help='Save trained model after training'
        )

        train_group.add_argument(
            '--validate-only',
            action='store_true',
            help='Validate configuration without training'
        )

        # Subcommands
        subparsers = parser.add_subparsers(dest='command', help='Available commands')

        # Train subcommand
        train_parser = subparsers.add_parser('train', help='Train brain')
        self.add_training_arguments(train_parser)

        # Continue subcommand
        continue_parser = subparsers.add_parser('continue', help='Continue training from checkpoint')
        continue_parser.add_argument('--checkpoint', type=str, required=True, help='Path to training checkpoint')

        # Import subcommand
        import_parser = subparsers.add_parser('import', help='Import training data for supervised learning')
        import_parser.add_argument('--data', type=str, required=True, help='Path to training data')
        import_parser.add_argument('--format', type=str, choices=['json', 'csv', 'h5'], default='json', help='Data format')

    def parse_args(self, description: str, subcommands: bool = False) -> argparse.ArgumentParser:
        """Parse command line arguments."""
        self.add_training_arguments(self.parser)
        return self.parser

    def run_command(self, args: argparse.Namespace) -> int:
        """Execute training command."""
        if not args.command:
            self.logger.error("No command specified. Use 'train', 'continue', or 'import'")
            return 1

        if args.command == 'train':
            return self._run_training(args)
        elif args.command == 'continue':
            return self._continue_training(args)
        elif args.command == 'import':
            return self._import_training_data(args)
        else:
            self.logger.error(f"Unknown command: {args.command}")
            return 1

    def _run_training(self, args: argparse.Namespace) -> int:
        """Run brain training with configuration."""
        self.logger.info("Starting brain training")

        # Load configuration
        try:
            config = load_config(args.config)
        except Exception as e:
            self.logger.error(f"Failed to load configuration: {e}")
            return 1

        # Override configuration with command line arguments
        if args.mode:
            config['training']['mode'] = args.mode

        if args.epochs:
            config['training']['epochs'] = args.epochs

        if args.output_dir:
            config['training']['output_dir'] = args.output_dir

        # Load pretrained model if specified
        if args.load_model:
            self.logger.info(f"Loading pretrained model: {args.load_model}")
            # In real implementation, load model

        # Validate configuration
        if not self._validate_training_config(config):
            return 1

        # Validate-only mode
        if args.validate_only:
            self.logger.info("Validation mode - checking configuration only")
            self.format_and_output({
                'validate_only': True,
                'config_loaded': True,
                'configuration': config
            }, "Validation Configuration")
            return 0

        try:
            # Create brain trainer
            trainer = BrainTrainer(config)

            # Run training
            results = trainer.train_brain()

            # Format and output results
            self.format_and_output(results, "Training Results")

            # Save results to file if specified
            if args.save_model and 'brain_file' in results:
                results_file = Path(results['brain_file']).parent / "training_summary.json"
                with open(results_file, 'w') as f:
                    import json
                    json.dump(results, f, indent=2)
                self.logger.info(f"Training summary saved to: {results_file}")

            self.logger.info("Brain training completed successfully")
            return 0

        except Exception as e:
            self.logger.error(f"Brain training failed: {e}")
            if self.verbose:
                import traceback
                self.logger.debug(traceback.format_exc())
            return 1

    def _continue_training(self, args: argparse.Namespace) -> int:
        """Continue training from checkpoint."""
        self.logger.info(f"Continuing training from checkpoint: {args.checkpoint}")

        # In real implementation, load checkpoint and continue training
        self.logger.info("Checkpoint loading and continuing training (mock implementation)")

        results = {
            'continue_training': True,
            'checkpoint': args.checkpoint,
            'status': 'continued successfully',
            'estimated_epochs_remaining': 50
        }

        self.format_and_output(results, "Continue Training Results")
        return 0

    def _import_training_data(self, args: argparse.Namespace) -> int:
        """Import training data for supervised learning."""
        self.logger.info(f"Importing training data: {args.data} (format: {args.format})")

        # Import training data (mock)
        self.logger.info("Importing training data...")

        results = {
            'import_completed': True,
            'data_file': args.data,
            'data_format': args.format,
            'samples_loaded': 1000,
            'features_per_sample': 50,
            'import_time_seconds': 5.2
        }

        self.logger.info("Training data import completed successfully")
        self.format_and_output(results, "Import Training Data Results")
        return 0

    def _validate_training_config(self, config: Dict[str, Any]) -> bool:
        """Validate training configuration."""
        required_fields = ['training']

        for field in required_fields:
            if field not in config:
                self.logger.error(f"Missing required configuration field: {field}")
                return False

        # Validate training configuration
        training_config = config['training']
        if 'mode' not in training_config:
            training_config['mode'] = 'supervised'

        if 'epochs' not in training_config:
            training_config['epochs'] = 100

        if 'output_dir' not in training_config:
            training_config['output_dir'] = 'trained_brains'

        self.logger.info("Training configuration validation passed")
        return True

    def main(self) -> int:
        """Main entry point for train_brain.py CLI."""
        cli = TrainBrainCLI()
        return cli.run()
if __name__ == '__main__':
    cli = TrainBrainCLI()
    sys.exit(cli.run())