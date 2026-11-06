    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
    
    return 0;
}

void printHelp() {
    std::cout << R"(
NLM - Neural Learning Machine (Phase 2 Demo)

USAGE:
    nlm [OPTIONS]

OPTIONS:
    --help, -h              Show this help message
    --version               Show version information
    --info                  Show system information and capabilities
    
    Configuration Options:
        --config=FILE, --config FILE      Load configuration from file
                                           Default: configs/default.cfg
    
    Demo Options:
        --demo-mode=MODE, --demo-mode MODE  Select demo mode
                                          Available: basic, plasticity, stdp, full
                                          Default: full
    
    Simulation Control:
        --steps=N, -s N                      Number of simulation steps
                                           Default: 100
        --seed=N                             Random seed for reproducible results
                                           Default: random
        --neuron=N                           Target neuron ID for stimulation
    
    Output Options:
        --output=DIR, -o DIR                 Output directory for results
                                           Default: current directory
        --checkpoint=FILE                    Save/load checkpoint file
    
    Logging Options:
        --log=LEVEL, -l LEVEL                Logging level (DEBUG, INFO, WARNING, ERROR)
                                           Default: INFO
        --quiet                              Only show errors
        --debug                              Show debug information
    
    Preset Configurations:
        --fast                                Fast demo (100 steps, minimal logging)
        --thorough                             Thorough demo (5000 steps, maximum logging)
        --production                          Production-ready configuration
    
EXAMPLES:
    nlm --help                          Show help and exit
    nlm --demo-mode plasticity           Run plasticity demonstration
    nlm --steps 500 --log INFO          Run 500 steps with INFO logging
    nlm --config=myconfig.cfg --output results  Use custom config and output directory
    nlm --seed 12345                    Run with reproducible random seed
    nlm --neuron 42 --steps 1000        Stimulate neuron 42 for 1000 steps

For more information, visit: https://github.com/nlm-project/nlm
)";
}

void printVersion() {
    std::cout << "NLM - Neural Learning Machine v0.1.0" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Built with C++17 and pybind11" << std::endl;
    std::cout << "For documentation and examples, visit: https://github.com/nlm-project/nlm" << std::endl;
}

void printSystemInfo() {
    std::cout << "NLM System Information:" << std::endl;
    std::cout << "  Version: 0.1.0 (Phase 2)" << std::endl;
    std::cout << "  Build Type: Release" << std::endl;
    std::cout << "  C++ Standard: C++17" << std::endl;
    std::cout << "  Features:" << std::endl;
    std::cout << "    - Leaky Integrate-and-Fire (LIF) neurons" << std::endl;
    std::cout << "    - Event-driven spike propagation" << std::endl;
    std::cout << "    - STDP and Hebbian plasticity" << std::endl;
    std::cout << "    - Structural plasticity (synaptogenesis/pruning)" << std::endl;
    std::cout << "    - Neural working memory" << std::endl;
    std::cout << "    - Episodic memory system" << std::endl;
    std::cout << "    - Prediction system" << std::endl;
    std::cout << "    - Neuromodulation (dopamine, curiosity, novelty)" << std::endl;
    std::cout << "    - Concept formation" << std::endl;
    std::cout << "    - Neural planning" << std::endl;
    std::cout << "    - Developmental plasticity" << std::endl;
    std::cout << "    - Checkpoint system" << std::endl;
    std::cout << "    - Python bindings (pynlm)" << std::endl;
}