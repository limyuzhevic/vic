#!/usr/bin/env python3
"""
NLM Advanced Workflow Manager

This script provides a complete workflow for NLM users, including experiment
management, analysis, simulation, and reporting. It integrates all the
advanced features developed for the NLM project.
"""

import argparse
import sys
import json
import time
import os
from pathlib import Path

# Add the python directory to path
sys.path.insert(0, str(Path(__file__).parent / 'python'))

try:
    import pynlm_enhanced
    print("✓ NLM Enhanced bindings available")
except ImportError:
    print("⚠ NLM Enhanced bindings not available")
    sys.exit(1)

class NLMWorkflowManager:
    """Manage complete NLM workflows with advanced features."""
    
    def __init__(self, workspace_dir="nlm_workspace"):
        self.workspace_dir = Path(workspace_dir)
        self.workspace_dir.mkdir(exist_ok=True)
        
        # Initialize components
        self.experiment_manager = None
        self.analysis_tools = None
        self.simulation_tools = None
        
    def initialize_components(self):
        """Initialize all workflow components."""
        print(f"\n=== Initializing NLM Workflow Manager ===")
        
        # Import experiment manager
        try:
            from experiment_manager import ExperimentManager
            self.experiment_manager = ExperimentManager()
            print("✓ Experiment Manager initialized")
        except ImportError as e:
            print(f"⚠ Experiment Manager not available: {e}")
        
        # Import analysis tools
        try:
            from analysis_tools import AnalysisTools
            self.analysis_tools = AnalysisTools(str(self.workspace_dir / "analysis"))
            print("✓ Analysis Tools initialized")
        except ImportError as e:
            print(f"⚠ Analysis Tools not available: {e}")
        
        # Import simulation tools
        try:
            from nlm_cli import SimulationTools
            self.simulation_tools = SimulationTools()
            print("✓ Simulation Tools initialized")
        except ImportError as e:
            print(f"⚠ Simulation Tools not available: {e}")
    
    def create_workflow(self, name, description, parameters):
        """Create a new workflow."""
        print(f"\n=== Creating Workflow: {name} ===")
        
        workflow_id = f"workflow_{int(time.time())}_{name.replace(' ', '_')}"
        workflow_dir = self.workspace_dir / workflow_id
        workflow_dir.mkdir(exist_ok=True)
        
        workflow = {
            'id': workflow_id,
            'name': name,
            'description': description,
            'parameters': parameters,
            'created_at': time.strftime('%Y-%m-%d %H:%M:%S'),
            'status': 'initialized',
            'steps': [],
            'results': {}
        }
        
        # Save workflow
        with open(workflow_dir / 'workflow.json', 'w') as f:
            json.dump(workflow, f, indent=2)
        
        print(f"✓ Workflow created: {workflow_id}")
        print(f"  - Name: {name}")
        print(f"  - Directory: {workflow_dir}")
        
        return workflow_id
    
    def run_workflow_step(self, workflow_id, step_name, step_config):
        """Run a step in the workflow."""
        if not Path(self.workspace_dir / workflow_id).exists():
            print(f"✗ Workflow {workflow_id} not found")
            return False
        
        print(f"\n=== Running Workflow Step: {step_name} ===")
        
        # Load workflow
        workflow_file = self.workspace_dir / workflow_id / 'workflow.json'
        with open(workflow_file, 'r') as f:
            workflow = json.load(f)
        
        # Initialize brain if not already done
        if 'brain' not in workflow:
            brain = self._initialize_brain(workflow['parameters'])
            workflow['brain'] = brain
        
        brain = workflow['brain']
        
        # Run the specific step
        if step_name == 'simulation':
            steps = step_config.get('steps', 1000)
            print(f"  Running simulation: {steps} steps")
            
            brain.startPerformanceMonitoring()
            
            start_time = time.time()
            for i in range(steps):
                brain.step(i)
            end_time = time.time()
            
            brain.stopPerformanceMonitoring()
            
            workflow['results']['simulation'] = {
                'steps_completed': steps,
                'simulation_time': end_time - start_time,
                'performance_metrics': {
                    'cpu_usage': brain.getPerformanceMetrics().cpuUsage,
                    'memory_usage': brain.getPerformanceMetrics().memoryUsage,
                    'spike_count': brain.getPerformanceMetrics().spikeCount,
                    'average_firing_rate': brain.getPerformanceMetrics().averageFiringRate()
                }
            }
            
        elif step_name == 'analysis':
            output_dir = step_config.get('output_dir', str(self.workspace_dir / workflow_id / 'analysis'))
            if self.analysis_tools:
                self.analysis_tools.output_dir = output_dir
            print(f"  Running analysis: {output_dir}")
            
            # Run analysis tools
            analysis_results = {}
            
            if step_config.get('spike_analysis'):
                analysis_results['spike_analysis'] = self._run_spike_analysis(brain)
            
            if step_config.get('connectivity_analysis'):
                analysis_results['connectivity_analysis'] = self._run_connectivity_analysis(brain)
            
            if step_config.get('development_analysis'):
                analysis_results['development_analysis'] = self._run_development_analysis(brain)
            
            workflow['results']['analysis'] = analysis_results
            
        elif step_name == 'experiment':
            experiment_name = step_config.get('experiment_name', 'workflow_experiment')
            experiment_description = step_config.get('experiment_description', 'Experiment from workflow')
            
            print(f"  Creating experiment: {experiment_name}")
            
            if self.experiment_manager:
                experiment_id = self.experiment_manager.create_experiment(
                    experiment_name, experiment_description, workflow['parameters']
                )
                
                if experiment_id:
                    run_result = self.experiment_manager.run_experiment(experiment_id, step_config.get('steps', 1000))
                    workflow['results']['experiment'] = {
                        'experiment_id': experiment_id,
                        'status': 'completed' if run_result else 'failed'
                    }
            
        elif step_name == 'batch_simulation':
            batch_size = step_config.get('batch_size', 100)
            num_batches = step_config.get('num_batches', 10)
            print(f"  Running batch simulation: {num_batches} batches of {batch_size} steps")
            
            total_steps = batch_size * num_batches
            brain.startPerformanceMonitoring()
            
            start_time = time.time()
            for batch in range(num_batches):
                for i in range(batch_size):
                    step = batch * batch_size + i
                    brain.step(step)
            
            end_time = time.time()
            brain.stopPerformanceMonitoring()
            
            workflow['results']['batch_simulation'] = {
                'batches_completed': num_batches,
                'total_steps': total_steps,
                'simulation_time': end_time - start_time,
                'performance_metrics': {
                    'cpu_usage': brain.getPerformanceMetrics().cpuUsage,
                    'memory_usage': brain.getPerformanceMetrics().memoryUsage,
                    'spike_count': brain.getPerformanceMetrics().spikeCount,
                    'average_firing_rate': brain.getPerformanceMetrics().averageFiringRate()
                }
            }
        
        # Save workflow
        with open(workflow_file, 'w') as f:
            json.dump(workflow, f, indent=2)
        
        print(f"✓ Workflow step completed: {step_name}")
        return True
    
    def _initialize_brain(self, parameters):
        """Initialize brain from parameters."""
        config = pynlm_enhanced.createEnhancedConfig()
        
        # Set standard parameters
        config.set("brain.neuron_count", parameters.get('neuron_count', 100))
        config.set("brain.synapse_density", parameters.get('synapse_density', 0.1))
        config.set("plasticity.stdp.enable", parameters.get('plasticity', {}).get('stdp', True))
        config.set("plasticity.hebbian.enable", parameters.get('plasticity', {}).get('hebbian', True))
        
        # Set experiment parameters
        for key, value in parameters.get('experiment', {}).items():
            config.set(f"experiment.{key}", value)
        
        brain = pynlm_enhanced.createEnhancedBrain(config)
        brain.initialize()
        
        print(f"✓ Brain initialized: {brain.getBrain().getTotalNeuronCount()} neurons")
        return brain
    
    def _run_spike_analysis(self, brain):
        """Run spike analysis on brain."""
        if not self.analysis_tools:
            return {}
        
        return self.analysis_tools.analyze_spike_patterns(brain, 1000)
    
    def _run_connectivity_analysis(self, brain):
        """Run connectivity analysis on brain."""
        if not self.analysis_tools:
            return {}
        
        return self.analysis_tools.analyze_neural_connectivity(brain)
    
    def _run_development_analysis(self, brain):
        """Run development analysis on brain."""
        if not self.analysis_tools:
            return {}
        
        return self.analysis_tools.analyze_development(brain)
    
    def run_complete_workflow(self, workflow_id, steps=None):
        """Run complete workflow with default steps."""
        print(f"\n=== Running Complete Workflow: {workflow_id} ===")
        
        if not Path(self.workspace_dir / workflow_id).exists():
            print(f"✗ Workflow {workflow_id} not found")
            return False
        
        # Load workflow
        workflow_file = self.workspace_dir / workflow_id / 'workflow.json'
        with open(workflow_file, 'r') as f:
            workflow = json.load(f)
        
        # Initialize brain
        brain = self._initialize_brain(workflow['parameters'])
        workflow['brain'] = brain
        
        # Run default workflow steps
        default_steps = steps or workflow.get('default_steps', 1000)
        
        print(f"  Running simulation: {default_steps} steps")
        brain.startPerformanceMonitoring()
        
        start_time = time.time()
        for i in range(default_steps):
            brain.step(i)
        
        end_time = time.time()
        brain.stopPerformanceMonitoring()
        
        # Save results
        workflow['results'] = {
            'simulation': {
                'steps_completed': default_steps,
                'simulation_time': end_time - start_time,
                'performance_metrics': {
                    'cpu_usage': brain.getPerformanceMetrics().cpuUsage,
                    'memory_usage': brain.getPerformanceMetrics().memoryUsage,
                    'spike_count': brain.getPerformanceMetrics().spikeCount,
                    'average_firing_rate': brain.getPerformanceMetrics().averageFiringRate()
                }
            }
        }
        
        # Save workflow
        with open(workflow_file, 'w') as f:
            json.dump(workflow, f, indent=2)
        
        print(f"✓ Complete workflow completed")
        print(f"  - Simulation time: {end_time - start_time:.2f}s")
        print(f"  - Results saved to: {workflow_file}")
        
        return True
    
    def list_workflows(self):
        """List all workflows."""
        print(f"\n=== Workflow List ===")
        
        workflows = []
        for item in self.workspace_dir.iterdir():
            if item.is_dir() and (item / 'workflow.json').exists():
                workflows.append(item.name)
        
        if not workflows:
            print("No workflows found.")
            return
        
        for workflow_id in workflows:
            workflow_file = self.workspace_dir / workflow_id / 'workflow.json'
            with open(workflow_file, 'r') as f:
                workflow = json.load(f)
            
            print(f"\n{workflow_id}:")
            print(f"  - Name: {workflow['name']}")
            print(f"  - Description: {workflow['description']}")
            print(f"  - Status: {workflow['status']}")
            print(f"  - Created: {workflow['created_at']}")
            
            if workflow.get('results'):
                print(f"  - Results: {list(workflow['results'].keys())}")
    
    def get_workflow_details(self, workflow_id):
        """Get workflow details."""
        if not Path(self.workspace_dir / workflow_id).exists():
            print(f"✗ Workflow {workflow_id} not found")
            return None
        
        workflow_file = self.workspace_dir / workflow_id / 'workflow.json'
        with open(workflow_file, 'r') as f:
            workflow = json.load(f)
        
        print(f"\n=== Workflow Details: {workflow_id} ===")
        print(f"Name: {workflow['name']}")
        print(f"Description: {workflow['description']}")
        print(f"Status: {workflow['status']}")
        print(f"Created: {workflow['created_at']}")
        print(f"Parameters: {json.dumps(workflow['parameters'], indent=2)}")
        
        if workflow.get('results'):
            print(f"\nResults:")
            for step, result in workflow['results'].items():
                print(f"  {step}:")
                print(f"    {json.dumps(result, indent=2)}")
        
        return workflow

def main():
    """Main workflow management interface."""
    parser = argparse.ArgumentParser(
        description="NLM Advanced Workflow Manager",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Commands:
  create     Create new workflow
  run        Run workflow step
  run-all    Run complete workflow
  list       List all workflows
  get        Get workflow details

Examples:
  # Create workflow
  python3 workflow_manager.py create "neural_research" "Neural network research workflow" --neuron-count 200 --steps 1000
  
  # Run workflow step
  python3 workflow_manager.py run workflow_123456789 simulation
  
  # Run complete workflow
  python3 workflow_manager.py run-all workflow_123456789
  
  # List workflows
  python3 workflow_manager.py list
  
  # Get workflow details
  python3 workflow_manager.py get workflow_123456789
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create workflow
    create_parser = subparsers.add_parser('create', help='Create new workflow')
    create_parser.add_argument('name', help='Workflow name')
    create_parser.add_argument('description', help='Workflow description')
    create_parser.add_argument('--workspace-dir', default='nlm_workspace', help='Workspace directory')
    create_parser.add_argument('--neuron-count', type=int, default=100, help='Number of neurons')
    create_parser.add_argument('--synapse-density', type=float, default=0.1, help='Synapse density')
    create_parser.add_argument('--steps', type=int, default=1000, help='Default number of steps')
    
    # Run workflow step
    run_parser = subparsers.add_parser('run', help='Run workflow step')
    run_parser.add_argument('workflow_id', help='Workflow ID')
    run_parser.add_argument('step_name', help='Step name (simulation, analysis, experiment, batch_simulation)')
    run_parser.add_argument('--workspace-dir', default='nlm_workspace', help='Workspace directory')
    run_parser.add_argument('--steps', type=int, help='Number of steps')
    run_parser.add_argument('--batch-size', type=int, help='Batch size for batch simulation')
    run_parser.add_argument('--num-batches', type=int, help='Number of batches')
    run_parser.add_argument('--output-dir', help='Output directory')
    run_parser.add_argument('--spike-analysis', action='store_true', help='Run spike analysis')
    run_parser.add_argument('--connectivity-analysis', action='store_true', help='Run connectivity analysis')
    run_parser.add_argument('--development-analysis', action='store_true', help='Run development analysis')
    run_parser.add_argument('--experiment-name', help='Experiment name')
    run_parser.add_argument('--experiment-description', help='Experiment description')
    
    # Run complete workflow
    run_all_parser = subparsers.add_parser('run-all', help='Run complete workflow')
    run_all_parser.add_argument('workflow_id', help='Workflow ID')
    run_all_parser.add_argument('--workspace-dir', default='nlm_workspace', help='Workspace directory')
    run_all_parser.add_argument('--steps', type=int, help='Number of steps')
    
    # List workflows
    list_parser = subparsers.add_parser('list', help='List all workflows')
    list_parser.add_argument('--workspace-dir', default='nlm_workspace', help='Workspace directory')
    
    # Get workflow details
    get_parser = subparsers.add_parser('get', help='Get workflow details')
    get_parser.add_argument('workflow_id', help='Workflow ID')
    get_parser.add_argument('--workspace-dir', default='nlm_workspace', help='Workspace directory')
    
    # Parse arguments
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 1
    
    # Initialize workflow manager
    manager = NLMWorkflowManager(args.workspace_dir)
    manager.initialize_components()
    
    try:
        if args.command == 'create':
            # Create workflow
            parameters = {
                'neuron_count': args.neuron_count,
                'synapse_density': args.synapse_density,
                'experiment': {}
            }
            
            workflow_id = manager.create_workflow(args.name, args.description, parameters)
            
            if workflow_id:
                print(f"✓ Workflow created. You can now run steps with 'workflow_manager.py run {workflow_id} ...'")
                return 0
            else:
                print(f"✗ Failed to create workflow")
                return 1
                
        elif args.command == 'run':
            # Run workflow step
            step_config = {}
            
            if args.steps:
                step_config['steps'] = args.steps
            if args.batch_size:
                step_config['batch_size'] = args.batch_size
            if args.num_batches:
                step_config['num_batches'] = args.num_batches
            if args.output_dir:
                step_config['output_dir'] = args.output_dir
            if args.spike_analysis:
                step_config['spike_analysis'] = True
            if args.connectivity_analysis:
                step_config['connectivity_analysis'] = True
            if args.development_analysis:
                step_config['development_analysis'] = True
            if args.experiment_name:
                step_config['experiment_name'] = args.experiment_name
            if args.experiment_description:
                step_config['experiment_description'] = args.experiment_description
            
            success = manager.run_workflow_step(args.workflow_id, args.step_name, step_config)
            return 0 if success else 1
            
        elif args.command == 'run-all':
            # Run complete workflow
            success = manager.run_complete_workflow(args.workflow_id, args.steps)
            return 0 if success else 1
            
        elif args.command == 'list':
            # List workflows
            manager.list_workflows()
            return 0
            
        elif args.command == 'get':
            # Get workflow details
            workflow = manager.get_workflow_details(args.workflow_id)
            return 0 if workflow else 1
    
    except Exception as e:
        print(f"✗ Error: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())