#!/usr/bin/env python3
"""
run_experiment.py: Script for running experiments with full configuration.

This automation script provides a complete workflow for running NLM experiments,
including configuration management, progress tracking, and result collection.
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
)

__version__ = "1.0.0"

class ExperimentRunner:
    """Class for running NLM experiments with comprehensive workflow."""

    def __init__(self, config: Dict[str, Any]):
        self.config = config
        self.logger = None
        self.setup_logging()

    def setup_logging(self) -> None:
        """Setup logging for experiment runner."""
        log_level = self.config.get('logging', {}).get('level', 'INFO')
        self.logger = setup_logging(log_level, 'experiment_runner')

    def run_experiment(self) -> Dict[str, Any]:
        """Run the complete experiment workflow."""
        experiment_config = self.config.get('experiment', {})
        experiment_name = experiment_config.get('name', 'Unnamed Experiment')
        num_runs = experiment_config.get('num_runs', 1)
        output_dir = Path(experiment_config.get('output_dir', 'experiments'))

        self.logger.info(f"Starting experiment: {experiment_name}")
        self.logger.info(f"Number of runs: {num_runs}")
        self.logger.info(f"Output directory: {output_dir}")

        # Create output directory
        output_dir.mkdir(parents=True, exist_ok=True)

        # Initialize results storage
        all_results = []
        experiment_metrics = {}

        # Run experiments
        for run in range(num_runs):
            run_start_time = time.time()

            self.logger.info(f"Running experiment {run + 1}/{num_runs}")

            # Create progress reporter for this run
            steps = experiment_config.get('steps', 1000)
            progress = ProgressReporter(
                description=f"Run {run + 1}",
                total=steps,
                logger=self.logger
            )

            # Run simulation
            run_results = self._run_single_simulation(steps, progress)

            run_end_time = time.time()
            run_duration = run_end_time - run_start_time

            # Store run results
            run_results['run_number'] = run + 1
            run_results['duration_seconds'] = run_duration
            run_results['start_time'] = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(run_start_time))

            all_results.append(run_results)

            # Aggregate metrics
            self._aggregate_metrics(run_results, experiment_metrics)

            # Save intermediate results
            self._save_intermediate_results(output_dir, run_results, run)

        # Calculate experiment summary
        experiment_summary = self._calculate_experiment_summary(all_results, experiment_metrics)

        # Save final results
        self._save_experiment_results(output_dir, all_results, experiment_summary)

        # Generate experiment report
        self._generate_experiment_report(output_dir, experiment_summary, all_results)

        # Create visualization if requested
        if experiment_config.get('create_visualizations', True):
            self._create_experiment_visualizations(output_dir, all_results)

        self.logger.info(f"Experiment completed successfully. Results saved to: {output_dir}")

        return {
            'experiment_completed': True,
            'experiment_name': experiment_name,
            'num_runs': num_runs,
            'total_duration_seconds': sum(r['duration_seconds'] for r in all_results),
            'results_directory': str(output_dir),
            'experiment_summary': experiment_summary,
            'aggregated_metrics': experiment_metrics,
            'runs_completed': len(all_results)
        }

    def _run_single_simulation(self, steps: int, progress: ProgressReporter) -> Dict[str, Any]:
        """Run a single simulation with progress tracking."""
        self.logger.debug(f"Starting simulation with {steps} steps")

        # Initialize metrics collector
        metrics = {
            'step_times': [],
            'spike_counts': [],
            'active_neurons': [],
            'rewards': [],
            'losses': []
        }

        # Run simulation steps
        for step in range(steps):
            # Simulate brain operation
            step_start_time = time.time()

            # Generate mock metrics
            step_time = 0.01 + (step * 0.001) % 0.1  # Variable step time
            spike_count = step * 10 % 100
            active_neuron_count = 1000 * 0.1 * (1 + (step % 10) * 0.1)
            reward = 1.0 - (step * 0.01) % 1.0
            loss = 0.5 / (step + 1)  # Decreasing loss

            step_end_time = time.time()
            actual_step_time = step_end_time - step_start_time

            # Record metrics
            metrics['step_times'].append(actual_step_time)
            metrics['spike_counts'].append(spike_count)
            metrics['active_neurons'].append(active_neuron_count)
            metrics['rewards'].append(reward)
            metrics['losses'].append(loss)

            progress.update(1)

            # Generate visualization if needed
            if (step + 1) % 100 == 0:
                self.logger.debug(f"Step {step + 1}/{steps} completed")

        # Calculate run summary
        run_summary = self._calculate_run_summary(metrics)

        return {
            'metrics': metrics,
            'run_summary': run_summary
        }

    def _calculate_run_summary(self, metrics: Dict[str, List[float]]) -> Dict[str, Any]:
        """Calculate summary statistics for a single run."""
        return {
            'total_steps': len(metrics['step_times']),
            'total_spikes': sum(metrics['spike_counts']),
            'avg_step_time_ms': sum(metrics['step_times']) / len(metrics['step_times']) * 1000 if metrics['step_times'] else 0,
            'avg_spike_count': sum(metrics['spike_counts']) / len(metrics['spike_counts']) if metrics['spike_counts'] else 0,
            'avg_active_neurons': sum(metrics['active_neurons']) / len(metrics['active_neurons']) if metrics['active_neurons'] else 0,
            'avg_reward': sum(metrics['rewards']) / len(metrics['rewards']) if metrics['rewards'] else 0,
            'final_loss': metrics['losses'][-1] if metrics['losses'] else 0,
            'min_loss': min(metrics['losses']) if metrics['losses'] else 0,
            'max_loss': max(metrics['losses']) if metrics['losses'] else 0,
            'performance_score': (sum(metrics['rewards']) / len(metrics['rewards']) if metrics['rewards'] else 0) * 100
        }

    def _aggregate_metrics(self, run_results: Dict[str, Any], experiment_metrics: Dict[str, List[float]]) -> None:
        """Aggregate metrics across all runs."""
        run_summary = run_results['run_summary']

        # Aggregate performance metrics
        if 'performance_score' in run_summary:
            experiment_metrics.setdefault('performance_scores', []).append(run_summary['performance_score'])

        if 'final_loss' in run_summary:
            experiment_metrics.setdefault('final_losses', []).append(run_summary['final_loss'])

        if 'avg_reward' in run_summary:
            experiment_metrics.setdefault('avg_rewards', []).append(run_summary['avg_reward'])

    def _calculate_experiment_summary(self, all_results: List[Dict[str, Any]], experiment_metrics: Dict[str, List[float]]) -> Dict[str, Any]:
        """Calculate overall experiment summary."""
        if not all_results:
            return {}

        run_summaries = [r['run_summary'] for r in all_results]

        return {
            'num_runs': len(all_results),
            'successful_runs': len([r for r in run_summaries if r.get('performance_score', 0) > 0]),
            'avg_performance_score': sum(r.get('performance_score', 0) for r in run_summaries) / len(run_summaries) if run_summaries else 0,
            'avg_final_loss': sum(r.get('final_loss', 0) for r in run_summaries) / len(run_summaries) if run_summaries else 0,
            'avg_avg_reward': sum(r.get('avg_reward', 0) for r in run_summaries) / len(run_summaries) if run_summaries else 0,
            'total_spikes_all_runs': sum(r.get('total_spikes', 0) for r in run_summaries),
            'total_steps_all_runs': sum(r.get('total_steps', 0) for r in run_summaries),
            'experiment_efficiency': sum(r.get('performance_score', 0) for r in run_summaries) / sum(r.get('total_steps', 0) for r in run_summaries) if sum(r.get('total_steps', 0) for r in run_summaries) > 0 else 0
        }

    def _save_intermediate_results(self, output_dir: Path, run_results: Dict[str, Any], run_number: int) -> None:
        """Save intermediate results for a single run."""
        run_file = output_dir / f"run_{run_number + 1:03d}_results.json"

        try:
            with open(run_file, 'w') as f:
                json.dump(run_results, f, indent=2)
            self.logger.debug(f"Saved intermediate results to: {run_file}")
        except Exception as e:
            self.logger.error(f"Failed to save intermediate results: {e}")

    def _save_experiment_results(self, output_dir: Path, all_results: List[Dict[str, Any]], experiment_summary: Dict[str, Any]) -> None:
        """Save final experiment results."""
        results_file = output_dir / "experiment_results.json"

        try:
            with open(results_file, 'w') as f:
                json.dump({
                    'all_results': all_results,
                    'experiment_summary': experiment_summary,
                    'timestamp': time.strftime('%Y-%m-%d %H:%M:%S'),
                    'version': __version__
                }, f, indent=2)
            self.logger.debug(f"Saved experiment results to: {results_file}")
        except Exception as e:
            self.logger.error(f"Failed to save experiment results: {e}")

    def _generate_experiment_report(self, output_dir: Path, experiment_summary: Dict[str, Any], all_results: List[Dict[str, Any]]) -> None:
        """Generate comprehensive experiment report."""
        report_file = output_dir / "experiment_report.md"

        try:
            with open(report_file, 'w') as f:
                f.write(f"# NLM Experiment Report\n\n")
                f.write(f"**Generated:** {time.strftime('%Y-%m-%d %H:%M:%S')}\n")
                f.write(f"**Version:** {__version__}\n\n")

                f.write(f"## Experiment Summary\n")
                f.write(f"- Number of Runs: {experiment_summary.get('num_runs', 0)}\n")
                f.write(f"- Successful Runs: {experiment_summary.get('successful_runs', 0)}\n")
                f.write(f"- Average Performance Score: {experiment_summary.get('avg_performance_score', 0):.2f}\n")
                f.write(f"- Average Final Loss: {experiment_summary.get('avg_final_loss', 0):.4f}\n\n")

                f.write(f"## Key Metrics\n")
                f.write(f"| Metric | Value |\n")
                f.write(f"|--------|-------|\n")
                f.write(f"| Total Spikes | {experiment_summary.get('total_spikes_all_runs', 0)} |\n")
                f.write(f"| Total Steps | {experiment_summary.get('total_steps_all_runs', 0)} |\n")
                f.write(f"| Experiment Efficiency | {experiment_summary.get('experiment_efficiency', 0):.4f} |\n\n")

                f.write(f"## Run Performance Comparison\n")
                f.write(f"| Run | Performance Score | Final Loss | Duration (s) |\n")
                f.write(f"|-----|-------------------|------------|---------------|\n")
                for result in all_results:
                    run_num = result['run_number']
                    perf_score = result['run_summary'].get('performance_score', 0)
                    final_loss = result['run_summary'].get('final_loss', 0)
                    duration = result['duration_seconds']
                    f.write(f"| {run_num} | {perf_score:.2f} | {final_loss:.4f} | {duration:.1f} |\n")

            self.logger.debug(f"Generated experiment report: {report_file}")
        except Exception as e:
            self.logger.error(f"Failed to generate experiment report: {e}")

    def _create_experiment_visualizations(self, output_dir: Path, all_results: List[Dict[str, Any]]) -> None:
        """Create visualizations for experiment results."""
        self.logger.info("Creating experiment visualizations...")

        # Create performance comparison chart
        perf_chart_file = output_dir / "performance_comparison.png"
        self._create_performance_chart(all_results, perf_chart_file)

        # Create loss curve visualization
        loss_chart_file = output_dir / "loss_curves.png"
        self._create_loss_curves(all_results, loss_chart_file)

        # Create summary dashboard
        dashboard_file = output_dir / "dashboard.html"
        self._create_experiment_dashboard(all_results, dashboard_file)

    def _create_performance_chart(self, all_results: List[Dict[str, Any]], output_file: Path) -> None:
        """Create performance comparison chart."""
        self.logger.debug(f"Creating performance chart: {output_file}")
        # In real implementation, use matplotlib or similar

    def _create_loss_curves(self, all_results: List[Dict[str, Any]], output_file: Path) -> None:
        """Create loss curve visualization."""
        self.logger.debug(f"Creating loss curves: {output_file}")
        # In real implementation, use matplotlib or similar

    def _create_experiment_dashboard(self, all_results: List[Dict[str, Any]], output_file: Path) -> None:
        """Create comprehensive experiment dashboard."""
        self.logger.debug(f"Creating experiment dashboard: {output_file}")
        # In real implementation, create HTML dashboard with multiple charts
class RunExperimentCLI(NLMCLI):
    """CLI tool for running experiments with full configuration."""

    def __init__(self):
        super().__init__(
            name="run_experiment.py",
            description="Run NLM experiments with comprehensive configuration and tracking",
            version=__version__
        )

    def add_experiment_arguments(self, parser: argparse.ArgumentParser) -> None:
        """Add experiment-specific arguments."""
        exp_group = parser.add_argument_group('Experiment Configuration')

        exp_group.add_argument(
            '--config',
            type=str,
            required=True,
            help='Path to experiment configuration file (JSON)'
        )

        exp_group.add_argument(
            '--experiment-name',
            type=str,
            help='Name for the experiment (overrides config)'
        )

        exp_group.add_argument(
            '--num-runs',
            type=int,
            help='Number of experiment runs (overrides config)'
        )

        exp_group.add_argument(
            '--steps',
            type=int,
            help='Number of simulation steps per run (overrides config)'
        )

        exp_group.add_argument(
            '--output-dir',
            type=str,
            help='Output directory for results (overrides config)'
        )

        exp_group.add_argument(
            '--resume',
            action='store_true',
            help='Resume from previous experiment state'
        )

        exp_group.add_argument(
            '--dry-run',
            action='store_true',
            help='Validate configuration without running experiments'
        )

        # Subcommands
        subparsers = parser.add_subparsers(dest='command', help='Available commands')

        # Run subcommand
        run_parser = subparsers.add_parser('run', help='Run experiment(s)')
        self.add_experiment_arguments(run_parser)

        # Validate subcommand
        validate_parser = subparsers.add_parser('validate', help='Validate experiment configuration')
        validate_parser.add_argument('--config', type=str, required=True, help='Path to experiment configuration file')

    def parse_args(self, description: str, subcommands: bool = False) -> argparse.ArgumentParser:
        """Parse command line arguments."""
        self.add_experiment_arguments(self.parser)
        return self.parser

    def run_command(self, args: argparse.Namespace) -> int:
        """Execute experiment command."""
        if not args.command:
            self.logger.error("No command specified. Use 'run' or 'validate'")
            return 1

        if args.command == 'run':
            return self._run_experiment(args)
        elif args.command == 'validate':
            return self._validate_experiment(args)
        else:
            self.logger.error(f"Unknown command: {args.command}")
            return 1

    def _run_experiment(self, args: argparse.Namespace) -> int:
        """Run experiment(s) with configuration."""
        self.logger.info("Starting experiment runner")

        # Load configuration
        try:
            config = load_config(args.config)
        except Exception as e:
            self.logger.error(f"Failed to load configuration: {e}")
            return 1

        # Override configuration with command line arguments
        if args.experiment_name:
            config['experiment']['name'] = args.experiment_name

        if args.num_runs:
            config['experiment']['num_runs'] = args.num_runs

        if args.steps:
            config['experiment']['steps'] = args.steps

        if args.output_dir:
            config['experiment']['output_dir'] = args.output_dir

        # Validate configuration
        if not self._validate_experiment_config(config):
            return 1

        # Dry run mode
        if args.dry_run:
            self.logger.info("Dry run mode - validating configuration only")
            self.format_and_output({
                'dry_run': True,
                'config_loaded': True,
                'configuration': config
            }, "Dry Run Configuration")
            return 0

        # Resume mode
        if args.resume:
            self.logger.info("Resume mode - checking for previous experiment state")
            # In real implementation, load previous state

        try:
            # Create experiment runner
            runner = ExperimentRunner(config)

            # Run experiment
            results = runner.run_experiment()

            # Format and output results
            self.format_and_output(results, "Experiment Results")

            # Save results to file if specified
            if args.output_dir:
                results_file = Path(args.output_dir) / "experiment_summary.json"
                with open(results_file, 'w') as f:
                    import json
                    json.dump(results, f, indent=2)
                self.logger.info(f"Experiment summary saved to: {results_file}")

            self.logger.info("Experiment completed successfully")
            return 0

        except Exception as e:
            self.logger.error(f"Experiment failed: {e}")
            if self.verbose:
                import traceback
                self.logger.debug(traceback.format_exc())
            return 1

    def _validate_experiment_config(self, config: Dict[str, Any]) -> bool:
        """Validate experiment configuration."""
        required_fields = ['experiment', 'simulation']

        for field in required_fields:
            if field not in config:
                self.logger.error(f"Missing required configuration field: {field}")
                return False

        # Validate experiment configuration
        experiment_config = config['experiment']
        if 'name' not in experiment_config:
            experiment_config['name'] = 'Unnamed Experiment'

        if 'num_runs' not in experiment_config:
            experiment_config['num_runs'] = 1

        if 'steps' not in experiment_config:
            experiment_config['steps'] = 1000

        if 'output_dir' not in experiment_config:
            experiment_config['output_dir'] = 'experiments'

        # Validate simulation configuration
        simulation_config = config['simulation']
        if 'timestep' not in simulation_config:
            simulation_config['timestep'] = 0.01

        if 'brain' not in simulation_config:
            simulation_config['brain'] = {}

        if 'world' not in simulation_config:
            simulation_config['world'] = {}

        self.logger.info("Configuration validation passed")
        return True

    def _validate_experiment(self, args: argparse.Namespace) -> int:
        """Validate experiment configuration file."""
        self.logger.info(f"Validating experiment configuration: {args.config}")

        try:
            config = load_config(args.config)

            # Validate configuration
            if not self._validate_experiment_config(config):
                return 1

            self.logger.info("Experiment configuration validation passed")
            self.format_and_output({
                'valid': True,
                'config_path': args.config,
                'experiment_name': config['experiment']['name'],
                'num_runs': config['experiment']['num_runs'],
                'steps': config['experiment']['steps']
            }, "Configuration Validation Results")

            return 0

        except Exception as e:
            self.logger.error(f"Configuration validation failed: {e}")
            return 1

    def main(self) -> int:
        """Main entry point for run_experiment.py CLI."""
        cli = RunExperimentCLI()
        return cli.run()
if __name__ == '__main__':
    cli = RunExperimentCLI()
    sys.exit(cli.run())