// Phase2CLI.cpp - Implementation of Phase 2 command line interface
#include "core/Phase2CLI.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {
namespace phase2 {

struct Phase2CLI::Impl {
    std::vector<std::pair<std::string, std::function<void(const std::string&)>>> customOptions;
};

Phase2CLI::Phase2CLI() : pImpl(std::make_unique<Impl>()) {
    initializeDefaultOptions();
    registerTestOptions();
}

Phase2CLI::~Phase2CLI() = default;

void Phase2CLI::initializeDefaultOptions() {
    // General options already initialized in constructor
}

void Phase2CLI::registerTestOptions() {
    // Register test-related options
    registerOption("--test", "Comma-separated list of tests to run", 
                   [this](const std::string& value) {
                       parseTestList(value);
                   });
    
    registerOption("--skip-test", "Comma-separated list of tests to skip",
                   [this](const std::string& value) {
                       parseSkipList(value);
                   });
}

bool Phase2CLI::parseArgs(int argc, char** argv) {
    errorMessage_.clear();
    
    // Parse general options first
    parseGeneralOptions(argc, argv);
    
    // Parse test options
    parseTestOptions(argc, argv);
    
    // Parse performance options
    parsePerformanceOptions(argc, argv);
    
    // Parse simulation options
    parseSimulationOptions(argc, argv);
    
    // Parse plasticity options
    parsePlasticityOptions(argc, argv);
    
    // Parse file options
    parseFileOptions(argc, argv);
    
    // Validate options
    if (!validateOptions()) {
        return false;
    }
    
    return true;
}

void Phase2CLI::parseGeneralOptions(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            options_.help = true;
        } else if (arg == "--version") {
            options_.version = true;
        } else if (arg.substr(0, 7) == "--config=") {
            options_.configFile = arg.substr(8);
        } else if (arg.substr(0, 8) == "--output=") {
            options_.outputFile = arg.substr(9);
        } else if (arg.substr(0, 11) == "--checkpoint=") {
            options_.checkpointFile = arg.substr(12);
        } else if (arg.substr(0, 10) == "--log-level=") {
            options_.logLevel = arg.substr(10);
        } else if (arg.substr(0, 11) == "--log-file=") {
            options_.logFile = arg.substr(12);
        } else if (arg == "--no-colors") {
            options_.enableColors = false;
        } else if (arg == "--list-tests") {
            options_.listTests = true;
        }
    }
}

void Phase2CLI::parseTestOptions(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 6) == "--test") {
            if (arg.find('=') != std::string::npos) {
                parseTestList(arg.substr(7));
            } else if (i + 1 < argc) {
                parseTestList(argv[++i]);
            }
        } else if (arg.substr(0, 10) == "--skip-test") {
            if (arg.find('=') != std::string::npos) {
                parseSkipList(arg.substr(11));
            } else if (i + 1 < argc) {
                parseSkipList(argv[++i]);
            }
        }
    }
}

void Phase2CLI::parseTestList(const std::string& testList) {
    options_.runTests.clear();
    size_t pos = 0;
    while ((pos = testList.find(',')) != std::string::npos) {
        std::string test = testList.substr(0, pos);
        options_.runTests.push_back(test);
        testList.erase(0, pos + 1);
    }
    options_.runTests.push_back(testList);
}

void Phase2CLI::parseSkipList(const std::string& skipList) {
    options_.skipTests.clear();
    size_t pos = 0;
    while ((pos = skipList.find(',')) != std::string::npos) {
        std::string test = skipList.substr(0, pos);
        options_.skipTests.push_back(test);
        skipList.erase(0, pos + 1);
    }
    options_.skipTests.push_back(skipList);
}

void Phase2CLI::parsePerformanceOptions(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--benchmark") {
            options_.enableBenchmark = true;
        } else if (arg == "--verbose") {
            options_.enableVerbose = true;
        } else if (arg == "--profile") {
            options_.enableProfiling = true;
        } else if (arg.substr(0, 17) == "--benchmark-iterations") {
            if (i + 1 < argc) {
                options_.benchmarkIterations = std::stoul(argv[++i]);
            }
        }
    }
}

void Phase2CLI::parseSimulationOptions(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 16) == "--neuron-count=") {
            options_.neuronCount = std::stoull(arg.substr(17));
        } else if (arg.substr(0, 16) == "--region-count=") {
            options_.regionCount = std::stoull(arg.substr(17));
        } else if (arg.substr(0, 25) == "--connection-probability=") {
            options_.connectionProbability = std::stod(arg.substr(26));
        } else if (arg.substr(0, 16) == "--timestep=") {
            options_.timestep = std::stod(arg.substr(10));
        } else if (arg.substr(0, 11) == "--seed=") {
            options_.randomSeed = std::stoull(arg.substr(7));
        }
    }
}

void Phase2CLI::parsePlasticityOptions(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 16) == "--stdp-ltp-weight=") {
            options_.stdpLtpWeight = std::stof(arg.substr(19));
        } else if (arg.substr(0, 17) == "--stdp-ltd-weight=") {
            options_.stdpLtdWeight = std::stof(arg.substr(19));
        } else if (arg.substr(0, 12) == "--stdp-tau=") {
            options_.stdpTau = std::stof(arg.substr(12));
        } else if (arg.substr(0, 21) == "--synaptogenesis-rate=") {
            options_.synaptogenesisRate = std::stof(arg.substr(23));
        } else if (arg.substr(0, 17) == "--pruning-rate=") {
            options_.pruningRate = std::stof(arg.substr(15));
        }
    }
}

void Phase2CLI::parseFileOptions(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 16) == "--plasticity-steps=") {
            options_.plasticitySteps = std::stoul(arg.substr(18));
        } else if (arg.substr(0, 25) == "--connectivity-test-steps=") {
            options_.connectivityTestSteps = std::stoul(arg.substr(28));
        } else if (arg.substr(0, 22) == "--stdp-test-trials=") {
            options_.stdpTestTrials = std::stoul(arg.substr(24));
        } else if (arg == "--save-checkpoint") {
            options_.saveCheckpoint = true;
        } else if (arg == "--load-checkpoint") {
            options_.loadCheckpoint = true;
        } else if (arg == "--save-results") {
            options_.saveResults = true;
        } else if (arg == "--load-results") {
            options_.loadResults = true;
        }
    }
}

bool Phase2CLI::validateOptions() const {
    if (options_.connectionProbability < 0.0 || options_.connectionProbability > 1.0) {
        errorMessage_ = "Connection probability must be between 0 and 1";
        return false;
    }
    
    if (options_.neuronCount == 0) {
        errorMessage_ = "Neuron count must be positive";
        return false;
    }
    
    if (options_.plasticitySteps == 0) {
        errorMessage_ = "Plasticity steps must be positive";
        return false;
    }
    
    if (options_.benchmarkIterations == 0) {
        errorMessage_ = "Benchmark iterations must be positive";
        return false;
    }
    
    return true;
}

void Phase2CLI::printHelp() const {
    std::cout << "=== NLM Phase 2 Help ===" << std::endl;
    std::cout << "Phase 2 Neural Learning Machine Tests" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --version                 Show version information" << std::endl;
    std::cout << "  --config <file>           Load configuration from file" << std::endl;
    std::cout << "  --output <file>           Save results to file" << std::endl;
    std::cout << "  --checkpoint <file>       Checkpoint file path" << std::endl;
    std::cout << "  --test <name>[,<name>...]  Run specific tests" << std::endl;
    std::cout << "  --skip-test <name>[,<name>...] Skip specific tests" << std::endl;
    std::cout << "  --benchmark               Run performance benchmarks" << std::endl;
    std::cout << "  --verbose                 Enable verbose output" << std::endl;
    std::cout << "  --profile                 Enable profiling" << std::endl;
    std::cout << "  --neuron-count <count>    Number of neurons" << std::endl;
    std::cout << "  --region-count <count>    Number of regions" << std::endl;
    std::cout << "  --connection-probability <prob> Connection probability" << std::endl;
    std::cout << "  --timestep <timestep>      Simulation timestep" << std::endl;
    std::cout << "  --seed <seed>             Random seed" << std::endl;
    std::cout << "  --plasticity-steps <steps> Number of plasticity steps" << std::endl;
    std::cout << "  --connectivity-test-steps <steps> Steps for connectivity test" << std::endl;
    std::cout << "  --stdp-test-trials <trials> STDP test trials" << std::endl;
    std::cout << "  --stdp-ltp-weight <weight> STDP LTP weight" << std::endl;
    std::cout << "  --stdp-ltd-weight <weight> STDP LTD weight" << std::endl;
    std::cout << "  --stdp-tau <tau>          STDP time constant" << std::endl;
    std::cout << "  --synaptogenesis-rate <rate> Synaptogenesis rate" << std::endl;
    std::cout << "  --pruning-rate <rate>     Pruning rate" << std::endl;
    std::cout << "  --save-checkpoint          Save checkpoint" << std::endl;
    std::cout << "  --load-checkpoint          Load checkpoint" << std::endl;
    std::cout << "  --save-results             Save test results" << std::endl;
    std::cout << "  --load-results             Load test results" << std::endl;
    std::cout << std::endl;
    std::cout << "Available tests:" << std::endl;
    std::vector<std::string> testNames;
    auto& framework = Phase2TestFramework::getInstance();
    for (const auto& test : framework.getAllTests()) {
        testNames.push_back(test->getName());
        std::cout << "  " << test->getName() << ": " << test->getDescription() << std::endl;
    }
    std::cout << std::endl;
    std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
}

void Phase2CLI::printVersion() const {
    std::cout << "NLM Phase 2 Version 0.1.0" << std::endl;
}

void Phase2CLI::printAvailableTests() const {
    std::cout << "Available tests:" << std::endl;
    auto& framework = Phase2TestFramework::getInstance();
    for (const auto& test : framework.getAllTests()) {
        std::cout << "  " << test->getName() << ": " << test->getDescription() << std::endl;
    }
}

void Phase2CLI::registerOption(const std::string& name, const std::string& description,
                              std::function<void(const std::string&)> callback) {
    pImpl->customOptions.push_back({name, callback});
}

Phase2TestConfig Phase2CLI::toTestConfig() const {
    Phase2TestConfig config;
    
    // Convert CLI options to test config
    config.randomSeed = options_.randomSeed;
    config.simulationTimestep = options_.timestep;
    config.neuronCount = options_.neuronCount;
    config.regionCount = options_.regionCount;
    config.connectionProbability = options_.connectionProbability;
    config.std_pLtpWeight = options_.stdpLtpWeight;
    config.std_pLtdWeight = options_.stdpLtdWeight;
    config.std_pTau = options_.stdpTau;
    config.synaptogenesisRate = options_.synaptogenesisRate;
    config.pruningRate = options_.pruningRate;
    config.plasticitySteps = options_.plasticitySteps;
    config.connectivityTestSteps = options_.connectivityTestSteps;
    config.stdpTestTrials = options_.stdpTestTrials;
    config.verbose = options_.enableVerbose;
    config.enableBenchmark = options_.enableBenchmark;
    config.enableProfiling = options_.enableProfiling;
    
    // Set test selection
    if (!options_.runTests.empty()) {
        config.runTests = options_.runTests;
    }
    
    if (!options_.skipTests.empty()) {
        config.skipTests = options_.skipTests;
    }
    
    return config;
}

Phase2CLI& Phase2CLI::getInstance() {
    static std::shared_ptr<Phase2CLI> instance = std::make_shared<Phase2CLI>();
    return *instance;
}

void Phase2CLI::setInstance(std::shared_ptr<Phase2CLI> cli) {
    std::shared_ptr<Phase2CLI>& instance = 
        const_cast<std::shared_ptr<Phase2CLI>&>(
            reinterpret_cast<const std::shared_ptr<Phase2CLI>&>(getInstance()));
    instance = cli;
}

} // namespace phase2
} // namespace nlm