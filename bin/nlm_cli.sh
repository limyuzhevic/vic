#!/usr/bin/env bash
# Command-line interface for NLM (Neural Learning Machine)
# Unified interface for running different phases and configurations

NLM_USAGE="
Usage: nlm [OPTIONS] [ARGS...]

NLM (Neural Learning Machine) - Experimental Artificial Brain Simulator

Phases Available:
  phase2     Run Phase 2: Real neural computation (default)
  phase3     Run Phase 3: World interaction experiments  
  phase4     Run Phase 4: Advanced integration experiments
  phase6     Run Phase 6: Final integration tests (default)
  demo       Run phase6 demo with interactive visualization

Basic Options:
  --help, -h          Show this help message
  --version, -v       Show version information
  --config FILE, -c    Load configuration from FILE
  --neurons N, -n     Number of neurons (default: 1000)
  --regions R, -r      Number of brain regions (default: 1)
  --steps S, -s        Number of simulation steps (default: 1000)
  --timestep T         Simulation timestep in seconds (default: 0.001)
  --seed SEED          Random seed (default: 42)
  --verbose, -V        Enable verbose logging
  --quiet, -q          Quiet mode (errors only)
  --log-level L        Log level: DEBUG, INFO, WARNING, ERROR

Phase 2 Specific Options:
  --plasticity, -p      Enable plasticity experiments
  --stdp, -t           Enable STDP learning
  --hebbian, -H        Enable Hebbian learning
  --structural, -S     Enable structural plasticity

Phase 3 Specific Options:
  --world W, -w        World type: simple, complex, arena
  --vision V, -V       Vision range (0.0-1.0)

Phase 4 Specific Options:
  --memory M, -m       Memory type: working, episodic, semantic, associative
  --cognition C, -c    Cognition type: planner, concepts, attention

Output Options:
  --output DIR, -o      Output directory for results and checkpoints
  --save-checkpoints    Enable automatic checkpoint saving
  --load-checkpoint FILE Load checkpoint from FILE
  --results FILE, -R    Save results to FILE (JSON/CSV)

Advanced Options:
  --gpu, -g            Use GPU acceleration if available
  --parallel, -p       Enable parallel processing
  --threads N          Number of threads (default: auto)
  --memory-limit M     Memory limit in MB
  --timeout T          Timeout in seconds (0 = unlimited)
  --no-ui              Run without UI (batch mode)
  --profile, -P        Enable profiling

Example Commands:
  nlm                          # Run phase6 demo with default settings
  nlm phase3 --steps 5000 --world complex
  nlm --config my_config.cfg --neurons 5000 --regions 3
  nlm phase4 --memory episodic --cognition planner
  nlm --output ./results --save-checkpoints --results results.json
  nlm phase2 --plasticity --stdp --hebbian
  nlm demo --world arena --vision 0.8

For more information, visit: https://github.com/nlm/neural-learning-machine
"