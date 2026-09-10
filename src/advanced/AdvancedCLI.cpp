#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class AdvancedCommandLineInterface {
public:
    AdvancedCommandLineInterface() = default;
    
    // Parse advanced command-line options
    bool parseAdvancedOptions(const vector<string>& args, 
                              unordered_map<string, string>& configOptions,
                              unordered_map<string, string>& systemOptions) {
        bool success = true;
        
        for (size_t i = 0; i < args.size(); ++i) {
            string arg = args[i];
            
            if (arg == "--memory-capacity") {
                if (i + 1 < args.size()) {
                    configOptions["brain.working_memory.capacity"] = args[++i];
                } else {
                    cerr << "Error: --memory-capacity requires a value" << endl;
                    success = false;
                }
            }
            else if (arg == "--memory-decay") {
                if (i + 1 < args.size()) {
                    configOptions["brain.working_memory.decay_rate"] = args[++i];
                } else {
                    cerr << "Error: --memory-decay requires a value" << endl;
                    success = false;
                }
            }
            else if (arg == "--prediction-model") {
                if (i + 1 < args.size()) {
                    systemOptions["prediction.model"] = args[++i];
                } else {
                    cerr << "Error: --prediction-model requires a value" << endl;
                    success = false;
                }
            }
            else if (arg == "--attention-strength") {
                if (i + 1 < args.size()) {
                    systemOptions["attention.inhibition_strength"] = args[++i];
                } else {
                    cerr << "Error: --attention-strength requires a value" << endl;
                    success = false;
                }
            }
            else if (arg == "--simulation-speed") {
                if (i + 1 < args.size()) {
                    systemOptions["simulation.speed_factor"] = args[++i];
                } else {
                    cerr << "Error: --simulation-speed requires a value" << endl;
                    success = false;
                }
            }
            else if (arg == "--checkpoint-interval") {
                if (i + 1 < args.size()) {
                    systemOptions["checkpoint.interval"] = args[++i];
                } else {
                    cerr << "Error: --checkpoint-interval requires a value" << endl;
                    success = false;
                }
            }
            else if (arg == "--replay-enabled") {
                systemOptions["memory.replay_enabled"] = "true";
            }
            else if (arg == "--development-stage") {
                if (i + 1 < args.size()) {
                    systemOptions["development.stage"] = args[++i];
                } else {
                    cerr << "Error: --development-stage requires a value" << endl;
                    success = false;
                }
            }
            else if (arg == "--novelty-threshold") {
                if (i + 1 < args.size()) {
                    systemOptions["novelty.threshold"] = args[++i];
                } else {
                    cerr << "Error: --novelty-threshold requires a value" << endl;
                    success = false;
                }
            }
            else if (arg == "--curiosity-rate") {
                if (i + 1 < args.size()) {
                    systemOptions["curiosity.exploration_rate"] = args[++i];
                } else {
                    cerr << "Error: --curiosity-rate requires a value" << endl;
                    success = false;
                }
            }
        }
        
        return success;
    }
    
    // Generate advanced usage documentation
    void printAdvancedUsage() {
        cout << "\n=== Advanced Command-Line Options ===" << endl;
        cout << "\nMemory System Options:" << endl;
        cout << "  --memory-capacity <int>        Set working memory capacity (default: 100)" << endl;
        cout << "  --memory-decay <float>         Set memory decay rate (0.0-1.0, default: 0.01)" << endl;
        cout << "  --replay-enabled               Enable memory replay during simulation" << endl;
        
        cout << "\nPrediction System Options:" << endl;
        cout << "  --prediction-model <string>    Prediction model type (default: neural)" << endl;
        
        cout << "\nAttention System Options:" << endl;
        cout << "  --attention-strength <float>   Attention inhibition strength (default: 0.5)" << endl;
        
        cout << "\nDevelopment Options:" << endl;
        cout << "  --development-stage <int>      Development stage (0-4, default: 0)" << endl;
        
        cout << "\nNeuromodulation Options:" << endl;
        cout << "  --novelty-threshold <float>    Novelty detection threshold (default: 0.1)" << endl;
        cout << "  --curiosity-rate <float>       Curiosity exploration rate (default: 0.5)" << endl;
        
        cout << "\nSystem Options:" << endl;
        cout << "  --simulation-speed <float>     Simulation speed factor (default: 1.0)" << endl;
        cout << "  --checkpoint-interval <int>    Checkpoint save interval (default: 1000)" << endl;
        
        cout << "\nExample Usage:" << endl;
        cout << "  ./nlm --memory-capacity 500 --memory-decay 0.05 --replay-enabled" << endl;
        cout << "  ./nlm --development-stage 2 --novelty-threshold 0.2 --curiosity-rate 0.8" << endl;
        cout << "  ./nlm --prediction-model bayesian --checkpoint-interval 5000" << endl;
    }
};