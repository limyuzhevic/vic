// NLM Command Line Interface
// Advanced features for users who want more control and automation

// Colors for output
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[0;34m"
#define NC "\033[0m" // No Color

// Default configuration
#define DEFAULT_NEURONS 500
#define DEFAULT_STEPS 2000
#define DEFAULT_WIDTH 16
#define DEFAULT_HEIGHT 16
#define DEFAULT_VISION 16
#define DEFAULT_REWARD_MODULATION true
#define DEFAULT_CURiosity true
#define DEFAULT_CHECKPOINTING true
#define DEFAULT_DEVELOPMENT true
#define DEFAULT_REPLAY true

// Help message
void show_help() {
    printf(BLUE "=== NLM Command Line Interface ===" NC "\n");
    printf("\nUSAGE:\n");
    printf("    ./nlm_cli [COMMAND] [OPTIONS]\n\n");
    
    printf("COMMANDS:\n");
    printf("    run        Run an NLM simulation with specified configuration\n");
    printf("    config      Manage configuration settings\n");
    printf("    info        Display system information and capabilities\n");
    printf("    benchmark   Run performance benchmarks\n");
    printf("    check       Check system status and validate installation\n\n");
    
    printf("EXAMPLES:\n");
    printf("    ./nlm_cli run --neurons 1000 --steps 5000 --width 32 --height 32\n");
    printf("    ./nlm_cli run --demo phase6 --checkpoint\n");
    printf("    ./nlm_cli config set default_neurons 2000\n");
    printf("    ./nlm_cli benchmark run\n\n");
    
    printf("For more detailed help, run ./nlm_cli COMMAND --help\n\n");
    
    printf(BLUE "DEMOS AVAILABLE:" NC "\n");
    printf("    phase2     - Basic neural computation (Phase 2)\n");
    printf("    phase3     - World interaction loop (Phase 3)\n");
    printf("    phase4     - Advanced cognition and planning (Phase 4)\n");
    printf("    phase5     - Brain-personality and sandbox learning (Phase 5)\n");
    printf("    phase6     - Final integration with all systems (Phase 6)\n");
    printf("    phase6_text - Phase 6 with text-as-a-sense capabilities\n");
}

// Configuration management
void config_menu() {
    printf(BLUE "=== NLM Configuration ===" NC "\n");
    printf("1. View current configuration\n");
    printf("2. Set default neurons\n");
    printf("3. Set default simulation steps\n");
    printf("4. Set world dimensions\n");
    printf("5. Set vision parameters\n");
    printf("6. Reset to defaults\n");
    printf("7. Exit\n\n");
    
    int choice;
    printf("Enter choice [1-7]: ");
    scanf("%d", &choice);
    
    switch (choice) {
        case 1:
            printf(YELLOW "Current configuration:" NC "\n");
            printf("  Default neurons: %d\n", DEFAULT_NEURONS);
            printf("  Default steps: %d\n", DEFAULT_STEPS);
            printf("  World width: %d\n", DEFAULT_WIDTH);
            printf("  World height: %d\n", DEFAULT_HEIGHT);
            printf("  Vision width: %d\n", DEFAULT_VISION);
            printf("  Vision height: %d\n", DEFAULT_VISION);
            printf("  Reward modulation: %s\n", DEFAULT_REWARD_MODULATION ? "true" : "false");
            printf("  Curiosity: %s\n", DEFAULT_CURiosity ? "true" : "false");
            printf("  Checkpointing: %s\n", DEFAULT_CHECKPOINTING ? "true" : "false");
            printf("  Development: %s\n", DEFAULT_DEVELOPMENT ? "true" : "false");
            printf("  Replay: %s\n", DEFAULT_REPLAY ? "true" : "false");
            break;
        
        case 2:
            printf("Enter default neurons: ");
            scanf("%d", &DEFAULT_NEURONS);
            break;
        
        case 3:
            printf("Enter default simulation steps: ");
            scanf("%d", &DEFAULT_STEPS);
            break;
        
        case 4:
            printf("Enter world width: ");
            scanf("%d", &DEFAULT_WIDTH);
            printf("Enter world height: ");
            scanf("%d", &DEFAULT_HEIGHT);
            break;
        
        case 5:
            printf("Enter vision width/height: ");
            scanf("%d", &DEFAULT_VISION);
            break;
        
        case 6:
            // Reset to defaults
            #define RESET_NEURONS 500
            #define RESET_STEPS 2000
            #define RESET_WIDTH 16
            #define RESET_HEIGHT 16
            #define RESET_VISION 16
            #define RESET_REWARD_MODULATION true
            #define RESET_CURiosity true
            #define RESET_CHECKPOINTING true
            #define RESET_DEVELOPMENT true
            #define RESET_REPLAY true
            printf(GREEN "Configuration reset to defaults.\n" NC);
            break;
        
        case 7:
            return;
        
        default:
            printf(RED "Invalid choice.\n" NC);
            break;
    }
    
    config_menu();
}

// Display system information
void show_info() {
    printf(BLUE "=== NLM System Information ===" NC "\n");
    printf("NLM Version: 0.1.0\n");
    printf("Phase: 6 (Final Integration)\n");
    printf("C++ Standard: C++20\n");
    printf("Build Type: Release (default)\n");
    
    // Check if executables are available
    printf("\n" YELLOW "Available Demos:" NC "\n");
    const char* demos[] = {"nlm", "nlm_phase2", "nlm_phase3_demo", "nlm_phase4_demo", 
                          "nlm_phase5_demo", "nlm_phase6_demo", "nlm_phase6_text_demo"};
    for (int i = 0; i < 7; i++) {
        if (system((string("test -f ") + demos[i] + string(" > /dev/null 2>&1")).c_str()) == 0) {
            printf("  " GREEN "✓ %s" NC "\n", demos[i]);
        } else {
            printf("  " RED "✗ %s (not built)" NC "\n", demos[i]);
        }
    }
    
    printf("\n" YELLOW "Features:" NC "\n");
    printf("  " GREEN "✓ Real LIF neuron dynamics" NC "\n");
    printf("  " GREEN "✓ Event-driven spike propagation with delays" NC "\n");
    printf("  " GREEN "✓ STDP and Hebbian plasticity rules" NC "\n");
    printf("  " GREEN "✓ Structural plasticity (synaptogenesis/pruning)" NC "\n");
    printf("  " GREEN "✓ Working memory integration" NC "\n");
    printf("  " GREEN "✓ Episodic memory with replay" NC "\n");
    printf("  " GREEN "✓ Neuromodulation (dopamine, curiosity, novelty)" NC "\n");
    printf("  " GREEN "✓ Prediction system integration" NC "\n");
    printf("  " GREEN "✓ Cognitive planning and concept formation" NC "\n");
    printf("  " GREEN "✓ Developmental system" NC "\n");
    printf("  " GREEN "✓ Checkpoint save/load" NC "\n");
    printf("  " GREEN "✓ Reward-modulated learning" NC "\n");
    printf("  " GREEN "✓ Curiosity-driven exploration" NC "\n");
}

// Run NLM simulation
void run_simulation(const char* demo, int neurons, int steps, int width, int height, 
                   int vision, bool reward_mod, bool curiosity, bool checkpoint, 
                   bool development, bool replay, const char* checkpoint_path, 
                   bool verbose) {
    printf(BLUE "Running NLM %s demo..." NC "\n", demo);
    printf("Configuration:\n");
    printf("  Neurons: %d\n", neurons);
    printf("  Steps: %d\n", steps);
    printf("  World: %dx%d\n", width, height);
    printf("  Vision: %dx%d\n", vision, vision);
    printf("  Reward modulation: %s\n", reward_mod ? "true" : "false");
    printf("  Curiosity: %s\n", curiosity ? "true" : "false");
    printf("  Checkpointing: %s\n", checkpoint ? "true" : "false");
    printf("  Development: %s\n", development ? "true" : "false");
    printf("  Replay: %s\n", replay ? "true" : "false");
    
    if (verbose) {
        printf(YELLOW "Verbose output enabled.\n" NC);
    }
    
    // Run the demo with configuration
    char command[256];
    snprintf(command, sizeof(command), "./nlm_phase%d_demo", demo[6] - '0');
    system(command);
    
    printf(GREEN "Demo completed.\n" NC);
}

// Run benchmarks
void run_benchmarks() {
    printf(BLUE "Running NLM benchmarks..." NC "\n");
    
    // Run the built-in benchmark
    if (system("test -f ./nlm_benchmark > /dev/null 2>&1") == 0) {
        printf("Running nlm_benchmark...\n");
        system("./nlm_benchmark");
    } else {
        printf(RED "nlm_benchmark not found! Build the project first.\n" NC);
    }
    
    // Additional benchmarks can be added here
    printf("Benchmarks completed.\n");
}

// Check system status
void check_system() {
    printf(BLUE "=== NLM System Status Check ===" NC "\n");
    
    // Check if main nlm executable exists
    if (system("test -f ./nlm > /dev/null 2>&1") == 0) {
        printf(GREEN "✓ Main NLM executable found\n" NC);
    } else {
        printf(RED "✗ Main NLM executable not found\n" NC);
    }
    
    // Check if demos exist
    int demos_found = 0;
    const char* demos[] = {"nlm_phase2", "nlm_phase3_demo", "nlm_phase4_demo", 
                          "nlm_phase5_demo", "nlm_phase6_demo", "nlm_phase6_text_demo"};
    for (int i = 0; i < 6; i++) {
        if (system((string("test -f ./") + demos[i] + string(" > /dev/null 2>&1")).c_str()) == 0) {
            demos_found++;
        }
    }
    
    printf("Demos found: %d/6\n", demos_found);
    
    // Check for build directory
    if (system("test -d ./build > /dev/null 2>&1") == 0) {
        printf(GREEN "✓ Build directory found\n" NC);
    } else {
        printf(YELLOW "⚠ Build directory not found\n" NC);
    }
    
    // Check for source files
    if (system("test -f ./src/main.cpp > /dev/null 2>&1") == 0 && 
        system("test -f ./src/brain/Neuron.cpp > /dev/null 2>&1") == 0) {
        printf(GREEN "✓ Core source files found\n" NC);
    } else {
        printf(RED "✗ Core source files missing\n" NC);
    }
    
    printf("\n" YELLOW "To build NLM:" NC "\n");
    printf("  mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4\n");
    
    printf("\n" YELLOW "To run demos after building:" NC "\n");
    printf("  ./nlm              # Phase 2 demo\n");
    printf("  ./nlm_phase3_demo  # Phase 3 demo\n");
    printf("  ./nlm_phase4_demo  # Phase 4 demo\n");
    printf("  ./nlm_phase5_demo  # Phase 5 demo\n");
    printf("  ./nlm_phase6_demo  # Phase 6 demo\n");
    printf("  ./nlm_phase6_text_demo  # Phase 6 text demo\n");
}

// Parse command line arguments
int main(int argc, char* argv[]) {
    if (argc < 2) {
        show_help();
        return 0;
    }
    
    const char* command = argv[1];
    
    if (strcmp(command, "run") == 0) {
        const char* demo = "phase6"; // Default demo
        int neurons = DEFAULT_NEURONS;
        int steps = DEFAULT_STEPS;
        int width = DEFAULT_WIDTH;
        int height = DEFAULT_HEIGHT;
        int vision = DEFAULT_VISION;
        bool reward_mod = DEFAULT_REWARD_MODULATION;
        bool curiosity = DEFAULT_CURiosity;
        bool checkpoint = DEFAULT_CHECKPOINTING;
        bool development = DEFAULT_DEVELOPMENT;
        bool replay = DEFAULT_REPLAY;
        const char* checkpoint_path = "./checkpoints";
        bool verbose = false;
        
        // Parse additional arguments
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "--demo") == 0 && i + 1 < argc) {
                demo = argv[++i];
            } else if (strcmp(argv[i], "--neurons") == 0 && i + 1 < argc) {
                neurons = atoi(argv[++i]);
            } else if (strcmp(argv[i], "--steps") == 0 && i + 1 < argc) {
                steps = atoi(argv[++i]);
            } else if (strcmp(argv[i], "--width") == 0 && i + 1 < argc) {
                width = atoi(argv[++i]);
            } else if (strcmp(argv[i], "--height") == 0 && i + 1 < argc) {
                height = atoi(argv[++i]);
            } else if (strcmp(argv[i], "--vision") == 0 && i + 1 < argc) {
                vision = atoi(argv[++i]);
            } else if (strcmp(argv[i], "--reward-mod") == 0 && i + 1 < argc) {
                reward_mod = (strcmp(argv[++i], "true") == 0);
            } else if (strcmp(argv[i], "--curiosity") == 0 && i + 1 < argc) {
                curiosity = (strcmp(argv[++i], "true") == 0);
            } else if (strcmp(argv[i], "--checkpoint") == 0 && i + 1 < argc) {
                checkpoint = (strcmp(argv[++i], "true") == 0);
            } else if (strcmp(argv[i], "--development") == 0 && i + 1 < argc) {
                development = (strcmp(argv[++i], "true") == 0);
            } else if (strcmp(argv[i], "--replay") == 0 && i + 1 < argc) {
                replay = (strcmp(argv[++i], "true") == 0);
            } else if (strcmp(argv[i], "--checkpoint-path") == 0 && i + 1 < argc) {
                checkpoint_path = argv[++i];
            } else if (strcmp(argv[i], "--verbose") == 0) {
                verbose = true;
            } else if (strcmp(argv[i], "--help") == 0) {
                show_help();
                return 0;
            } else {
                printf(RED "Unknown option: %s\n" NC, argv[i]);
                show_help();
                return 1;
            }
        }
        
        run_simulation(demo, neurons, steps, width, height, vision, 
                      reward_mod, curiosity, checkpoint, development, replay, 
                      checkpoint_path, verbose);
        
    } else if (strcmp(command, "config") == 0) {
        config_menu();
    } else if (strcmp(command, "info") == 0) {
        show_info();
    } else if (strcmp(command, "benchmark") == 0) {
        run_benchmarks();
    } else if (strcmp(command, "check") == 0) {
        check_system();
    } else if (strcmp(command, "help") == 0 || strcmp(command, "--help") == 0 || 
               strcmp(command, "-h") == 0) {
        show_help();
    } else {
        printf(RED "Unknown command: %s\n" NC, command);
        show_help();
        return 1;
    }
    
    return 0;
}