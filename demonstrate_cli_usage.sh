#!/bin/bash

cat << 'USAGE'
==================================================
NLM Phase 2 CLI Usage Examples
==================================================

BASIC USAGE:
    ./nlm                                    # Run all tests with defaults

TEST MODES:
    ./nlm --test-mode connectivity           # Run only connectivity tests
    ./nlm --test-mode plasticity             # Run only plasticity tests  
    ./nlm --test-mode stdp                   # Run only STDP tests
    ./nlm --test-mode all                    # Run all tests (default)
    ./nlm --test-mode quick                  # Run quick connectivity test only

CONFIGURATION:
    ./nlm --config configs/custom.cfg        # Load custom configuration file
    ./nlm -config my_config.cfg              # Short form

VERBOSITY CONTROL:
    ./nlm --verbose                          # Enable verbose output
    ./nlm --quiet                            # Suppress output
    ./nlm --log-level DEBUG                  # Enable debug logging
    ./nlm --log-level INFO                   # Info logging (default)
    ./nlm --log-level WARNING                # Warning and above
    ./nlm --log-level ERROR                  # Error and above

OUTPUT FORMAT:
    ./nlm --format text                      # Text output (default)
    ./nlm --format json                      # JSON output
    ./nlm --format csv                       # CSV output

TEST SELECTION:
    ./nlm --run-tests connectivity,plasticity      # Run specific tests
    ./nlm --skip-tests stdp                        # Skip STDP tests

SIMULATION PARAMETERS:
    ./nlm --steps 5000                           # Set max simulation steps
    ./nlm --timestep 0.01                        # Set simulation timestep

HELP AND USAGE:
    ./nlm --help                                # Display all options
    ./nlm --usage                               # Show usage examples
    ./nlm --examples                            # Show practical examples

COMBINED EXAMPLES:
    ./nlm --test-mode all --format json --verbose --steps 2000
    ./nlm --config configs/prod.cfg --run-tests connectivity,plasticity,PLOTERYA --log-level INFO

==================================================
USAGE

