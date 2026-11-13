nmcli - Neural Machine Learning CLI Commands

Available commands:

1. nlm_cli --help                    Show this help message

2. nlm_cli create <name>              Create a new neural brain with default configuration

3. nlm_cli load <checkpoint>          Load a brain from a checkpoint file

4. nlm_cli save <checkpoint>          Save current brain state to a checkpoint

5. nlm_cli simulate <steps> [options]  Run brain simulation for specified steps

6. nlm_cli analyze [metrics]          Analyze brain performance and metrics

7. nlm_cli config [key] [value]       Get or set configuration parameters

8. nlm_cli memory <type> [action]     Manage brain memory systems

9. nlm_cli predict <input>            Generate prediction from current brain state

10. nlm_cli train <data> [epochs]     Train brain on provided data

Usage examples:
  nlm_cli create mybrain
  nlm_cli simulate 1000 --world --verbose
  nlm_cli analyze --spikes --memory
  nlm_cli config neuron_count 2000
  nlm_cli memory working retrieve
  nlm_cli predict "visual_input_data"
  nlm_cli train "training_data" --epochs 100

Global options:
  --help, -h                Show help message
  --verbose, -v             Enable verbose output
  --quiet, -q              Quiet mode (minimal output)
  --config <file>           Load configuration from file
  --output <file>           Save output to file
  --thread <count>          Number唯一的线程数
  --gpu                       Use GPU acceleration (if available)

Sub-command specific options:
  create:
    --neurons <count>        Number of neurons (default: 1000)
    --regions <count>         Number of brain regions (default: 1)
    --connections <float>    Connection probability (default: 0.1)
    --development             Enable developmental plasticity
    -- neuromodulation         Enable neuromodulation systems

  simulate:
    --duration <float>        Simulation duration in seconds (default: 1.0)
    -- timestep <float>       Simulation timestep (default: 0.001)
    --with-world              Run with world environment
    --replay                  Enable memory replay
    --consolidation            Enable memory consolidation

  analyze:
    --spikes                  Analyze spike patterns
    --memory                  Analyze memory systems
    --prediction              Analyze prediction accuracy
    --cognition               Analyze cognitive processes
    --performance              Performance metrics
    --developmental           Developmental progress

  memory:
    working -- Retrieve working memory content
    episodic -- Retrieve episodic memories
    associative -- Retrieve associative patterns
    clear -- Clear all memory systems
    export -- Export memory data to file
    import -- Import memory data from file

  config:
    get -- Get configuration value
    set -- Set configuration value
    list -- List all configuration options
    validate -- Validate configuration

  train:
    --epochs <count>          Number of training epochs (default: 100)
    --batch <size>             Batch size for training (default: 32)
    --learning-rate <float>    Learning rate (default: 0.01)
    --mode <string>           Training mode: "reinforcement", "supervised", "unsupervised"
    --save-best                Save best model during training
    --checkpoint <file>        Save checkpoint during training

  predict:
    --type <string>            Prediction type: "next-state", "action", "reward"
    --context <data>           Context for prediction
    --confidence               Include confidence in prediction

Error Codes:
  0 = Success
  1 = Invalid arguments
  2 = Configuration error
  3 = Simulation error
  4 = Memory error
  5 = Memory error
  6 = General error
  7 = Shutdown requested

Exit Codes:
  0 = Normal completion
  1 = Invalid arguments
  2 = Configuration error
  3 = Simulation error
  4 = Memory error
  5 = Memory error
  6 = General error
  7 = Shutdown requested

Version:
  NLM CLI 2.0 (Phase 6 Integration)
  Released: 2026-11-13
  Compatible with: NLM v0.1.�וי