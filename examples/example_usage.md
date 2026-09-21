// Example 1: Basic usage with help
// nlm --help

// Example 2: Run specific test suite
// nlm --run-test=2 --neuron-count=1000 --region-count=2 --max-steps=5000

// Example 3: Load custom configuration and validate
// nlm --config=myconfig.cfg --validate

// Example 4: Enable verbose logging and run with custom parameters
// nlm --neuron-count=2000 --region-count=4 --timestep=0.0005 --enable-feature=visualization

// Example 5: Analysis and benchmarking
// nlm --analyze --neuron-count=500 --region-count=1

// Example 6: Configuration sections
// nlm --config-sections config/base.cfg config/development.cfg

// Example 7: Performance profiling
// nlm --profile --neuron-count=1000 --max-steps=10000

// Example 8: Result export
// nlm --save-results=experiment_results.json --run-test=2

// Example 9: Validation only
// nlm --validate

// Example 10: Build information
// nlm --build-info

// Example 11: Version information
// nlm --version

// Example 12: Compare with reference
// nlm --compare=reference_results.json --run-test=1

// Example 13: Load previous results
// nlm --load-results=previous_experiment.json

// Example 14: Debug mode
// nlm --debug --neuron-count=100 --region-count=1

// Example 15: Feature toggling
// nlm --enable-feature=advanced_plasticity --disable-feature=structural_plasticity
