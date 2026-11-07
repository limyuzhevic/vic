=== NLM Experiment Manager ===

An advanced experiment management and automation tool for NLM Phase 6 simulations with parameter sweeps, parallel execution, and result analysis.

Usage:
  nlm_experiment [options]

Options:
  --experiment-file FILE     Experiment configuration file (JSON/YAML)
  --parallel EXECS          Number of parallel experiments (default: 1)
  --parameter-sweep         Enable parameter sweep mode
  --save-results DIR        Directory to save experiment results
  --results-format FORMAT   Result format: json, csv, parquet (default: json)
  --validate-config         Validate experiment configuration
  --dry-run                 Print experiment plan without execution
  --resume                   Resume from last checkpoint
  --auto-config             Auto-configure experiment based on available resources
  --timeout TIMEOUT         Experiment timeout in seconds (default: 3600)
  --continue-on-failure     Continue running experiments even if some fail
  --progress-report INTERVAL Report progress every INTERVAL seconds (default: 30)
  --export-summary FILE     Export experiment summary to file
  --import-results FILE     Import previous experiment results
  --analyze                 Perform automated analysis of results
  --generate-report         Generate comprehensive experiment report
  --help                    Show this help message

Examples:
  nlm_experiment --experiment-file my_experiment.json
  nlm_experiment --parameter-sweep --parallel 4
  nlm_experiment --save-results results/ --analyze
