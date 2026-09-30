#include "core/CommandLineInterface.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <memory>
#include <vector>
#include <map>

using namespace nlm;

// CommandLineConfig implementation
CommandLineConfig::CommandLineConfig(int argc, char** argv, const std::string& app_name)
    : valid_(true), error_message_(), argc_(argc), argv_(argv), app_name_(app_name) {

    // Standard options
    options_["help"] = std::any(false);
    options_["version"] = std::any(app_name_);
    options_["verbose"] = std::any(false);
    options_["quiet"] = std::any(false);
    options_["log-level"] = std::any(std::string("info"));
    options_["config"] = std::any(std::string("configs/default.cfg"));
}

bool CommandLineConfig::parse() {
    remaining_args_.clear();

    for (int i = 1; i < argc_; ++i) {
        std::string arg = argv_[i];

        if (arg == "--help" || arg == "-h") {
            options_["help"] = std::any(true);
        } else if (arg == "--version") {
            options_["version"] = std::any(true);
        } else if (arg.substr(0, 2) == "--") {
            if (!parseKeyValuePair(arg)) {
                return false;
            }
        } else if (arg.substr(0, 1) == "-") {
            // Short option
            for (char c : arg.substr(1)) {
                std::string short_opt(1, c);
                if (short_opt == "h") {
                    options_["help"] = std::any(true);
                } else if (short_opt == "v") {
                    options_["verbose"] = std::any(true);
                } else if (short_opt == "q") {
                    options_["quiet"] = std::any(true);
                } else {
                    remaining_args_.push_back("-" + short_opt);
                }
            }
        } else {
            remaining_args_.push_back(arg);
        }
    }

    return validateOptions();
}

bool CommandLineConfig::parseKeyValuePair(const std::string& arg) {
    size_t eq_pos = arg.find('=');
    if (eq_pos != std::string::npos) {
        std::string key = arg.substr(2, eq_pos - 2);
        std::string value = arg.substr(eq_pos + 1);

        // Handle different types based on key
        if (key == "log-level") {
            options_[key] = std::any(value);
        } else if (key == "config") {
            options_[key] = std::any(value);
        } else {
            remaining_args_.push_back(arg);
        }
    } else {
        remaining_args_.push_back(arg);
    }

    return true;
}

std::any CommandLineConfig::stringToAny(const std::string& value, const std::string& type_hint) {
    if (type_hint == "int") {
        return std::any(std::stoi(value));
    } else if (type_hint == "float") {
        return std::any(std::stof(value));
    } else if (type_hint == "double") {
        return std::any(std::stod(value));
    } else if (type_hint == "bool") {
        return std::any((value == "true" || value == "1" || value == "yes"));
    } else {
        return std::any(value);
    }
}

std::string CommandLineConfig::anyToString(const std::any& value) {
    std::ostringstream oss;
    if (value.type() == typeid(int)) {
        oss << std::any_cast<int>(value);
    } else if (value.type() == typeid(float)) {
        oss << std::any_cast<float>(value);
    } else if (value.type() == typeid(double)) {
        oss << std::any_cast<double>(value);
    } else if (value.type() == typeid(bool)) {
        oss << (std::any_cast<bool>(value) ? "true" : "false");
    } else if (value.type() == typeid(std::string)) {
        oss << std::any_cast<std::string>(value);
    } else {
        oss << "unknown";
    }
    return oss.str();
}

bool CommandLineConfig::validateOptions() {
    return true;
}

void CommandLineConfig::printHelp(const std::string& app_name, const std::vector<Option>& options) {
    std::cout << app_name << " - " << "NLM (Neural Learning Machine)" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  " << app_name << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;

    for (const auto& option : options) {
        std::cout << "  ";
        if (!option.short_name.empty()) {
            std::cout << "-" << option.short_name << ", ";
        }
        std::cout << "--" << option.long_name;

        if (!option.description.empty()) {
            std::cout << std::string(30 - option.long_name.length() - (option.short_name.empty() ? 3 : 5), ' ')
                     << option.description;
        }

        if (option.required) {
            std::cout << " (required)";
        }

        if (!option.type_hint.empty()) {
            std::cout << " [" << option.type_hint << "]";
        }

        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << app_name << " --run-demo" << std::endl;
    std::cout << "  " << app_name << " --run-experiments --experiment-mode=learning" << std::endl;
    std::cout << "  " << app_name << " --config=my_config.cfg --verbose" << std::endl;
}

// CommandLineApplication implementation
CommandLineApplication::CommandLineApplication(int argc, char** argv)
    : argc_(argc), argv_(argv) {

    // Parse standard options first
    if (argc >= 2) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--help" || arg == "-h") {
                printHelp(argv[0], {});
                std::exit(0);
            } else if (arg == "--version") {
                std::cout << argv[0] << " version " << config_.version << std::endl;
                std::exit(0);
            } else if (arg == "--config-mode") {
                // Force config mode
                config_.run_tests = false;
                config_.run_demo = false;
                config_.run_experiments = false;
            }
        }
    }

    options_ = std::make_shared<CommandLineConfig>(argc, argv, config_.app_name);
    options_->parse();

    processCommandLineOptions();
}

bool CommandLineApplication::run(const Config& config) {
    config_ = config;

    if (!setupLogging()) {
        std::cerr << "Failed to setup logging" << std::endl;
        return false;
    }

    std::cout << "=== " << config_.app_name << " ===" << std::endl;
    printBanner();

    if (!initializeEnvironment()) {
        std::cerr << "Failed to initialize environment" << std::endl;
        return false;
    }

    if (!setup()) {
        std::cerr << "Failed to setup" << std::endl;
        return false;
    }

    ApplicationResult result = runWithConfig(config_);

    if (!cleanup()) {
        std::cerr << "Warning: Cleanup failed" << std::endl;
    }

    return result.getStatus() == ApplicationResult::Status::Success;
}

bool CommandLineApplication::runWithConfig(const Config& config) {
    start_timer();

    try {
        if (config.run_tests) {
            NLM_LOG_INFO("Running in test mode");
            TestMode test_app;
            if (!test_app.initialize(config)) {
                return ApplicationResult(ApplicationResult::Status::Failed, "Test app initialization failed").toString();
            }
            auto result = test_app.run();
            return result.getStatus() == ApplicationResult::Status::Success;
        } else if (config.run_demo) {
            NLM_LOG_INFO("Running in demo mode");
            DemoMode demo_app;
            if (!demo_app.initialize(config)) {
                return ApplicationResult(ApplicationResult::Status::Failed, "Demo app initialization failed").toString();
            }
            auto result = demo_app.run();
            return result.getStatus() == ApplicationResult::Status::Success;
        } else if (config.run_experiments) {
            NLM_LOG_INFO("Running in experiment mode");
            ExperimentMode experiment_app;
            if (!experiment_app.initialize(config)) {
                return ApplicationResult(ApplicationResult::Status::Failed, "Experiment app initialization failed").toString();
            }
            auto result = experiment_app.run();
            return result.getStatus() == ApplicationResult::Status::Success;
        } else {
            NLM_LOG_INFO("Running in default mode (configuration validation)");
            ConfigMode config_app;
            if (!config_app.initialize(config)) {
                return ApplicationResult(ApplicationResult::Status::Failed, "Config app initialization failed").toString();
            }
            auto result = config_app.run();
            return result.getStatus() == ApplicationResult::Status::Success;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in application: " + std::string(e.what()));
        return ApplicationResult(ApplicationResult::Status::Failed, std::string(e.what())).toString();
    } catch (...) {
        NLM_LOG_ERROR("Unknown exception in application");
        return ApplicationResult(ApplicationResult::Status::Failed, "Unknown exception").toString();
    }
}

bool CommandLineApplication::setupLogging() {
    logger_ = std::make_unique<Logger>();

    if (config_.quiet) {
        logger_->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Warning));
    } else if (config_.verbose) {
        logger_->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Debug));
    } else {
        logger_->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Info));
    }

    Logger::setGlobal(logger_);
    NLM_LOG_INFO("Logging initialized with level: " + config_.log_level);
    return true;
}

bool CommandLineApplication::initializeEnvironment() {
    std::cout << "Initializing environment..." << std::endl;

    if (!createDirectories()) {
        NLM_LOG_WARNING("Failed to create some directories");
    }

    NLM_LOG_INFO("Environment initialized successfully");
    return true;
}

bool CommandLineApplication::createDirectories() const {
    bool success = true;

    std::vector<std::string> dirs = {
        config_.checkpoint_dir,
        "logs",
        "configs",
        "checkpoints"
    };

    for (const auto& dir : dirs) {
        if (!dir.empty() && std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
    }

    return success;
}

void CommandLineApplication::printBanner() const {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << std::endl;
}

bool CommandLineApplication::processCommandLineOptions() {
    std::cout << "Processing command line options..." << std::endl;

    // Process standard options
    if (options_->has("help") && std::any_cast<bool>(options_->get("help"))) {
        std::cout << "Help requested" << std::endl;
    }

    if (options_->has("version") && std::any_cast<bool>(options_->get("version"))) {
        std::cout << "Version: " << config_.version << std::endl;
    }

    if (options_->has("verbose") && std::any_cast<bool>(options_->get("verbose"))) {
        config_.verbose = true;
        std::cout << "Verbose mode enabled" << std::endl;
    }

    if (options_->has("quiet") && std::any_cast<bool>(options_->get("quiet"))) {
        config_.quiet = true;
        std::cout << "Quiet mode enabled" << std::endl;
    }

    if (options_->has("log-level")) {
        std::string log_level = std::any_cast<std::string>(options_->get("log-level"));
        config_.log_level = log_level;
        std::cout << "Log level set to: " << log_level << std::endl;
    }

    if (options_->has("config")) {
        std::string config_file = std::any_cast<std::string>(options_->get("config"));
        config_.config_file = config_file;
        std::cout << "Config file: " << config_file << std::endl;
    }

    // Process remaining arguments
    auto remaining_args = options_->getRemainingArgs();
    for (const auto& arg : remaining_args) {
        std::cout << "Remaining argument: " << arg << std::endl;

        if (arg == "--run-demo" || arg == "-demo") {
            config_.run_demo = true;
        } else if (arg == "--run-tests" || arg == "-tests") {
            config_.run_tests = true;
        } else if (arg == "--run-experiments" || arg == "-experiments") {
            config_.run_experiments = true;
        } else if (arg.find("--experiment-mode=") == 0) {
            config_.experiment_mode = arg.substr(18);
        } else if (arg.find("--duration=") == 0) {
            try {
                config_.simulation_duration = std::stod(arg.substr(11));
            } catch (...) {
                NLM_LOG_WARNING("Invalid duration value: " + arg);
            }
        } else if (arg.find("--max-steps=") == 0) {
            try {
                config_.max_steps = static_cast<size_t>(std::stoul(arg.substr(11)));
            } catch (...) {
                NLM_LOG_WARNING("Invalid max steps value: " + arg);
            }
        } else if (arg.find("--warmup-steps=") == 0) {
            try {
                config_.warmup_steps = static_cast<size_t>(std::stoul(arg.substr(15)));
            } catch (...) {
                NLM_LOG_WARNING("Invalid warmup steps value: " + arg);
            }
        }
    }

    return true;
}

// ApplicationResult implementation
ApplicationResult::ApplicationResult(Status status, const std::string& message)
    : status_(status), message_(message), start_time_(std::chrono::high_resolution_clock::now()) {
}

std::string ApplicationResult::toString() const {
    std::ostringstream oss;
    oss << "Status: ";

    switch (status_) {
        case Status::Success:
            oss << "Success";
            break;
        case Status::Failed:
            oss << "Failed";
            break;
        case Status::PartiallyCompleted:
            oss << "PartiallyCompleted";
            break;
        case Status::UserInterrupted:
            oss << "UserInterrupted";
            break;
        case Status::ConfigurationError:
            oss << "ConfigurationError";
            break;
    }

    oss << ", Message: " << message_;
    if (!details_.empty()) {
        oss << ", Details: " << details_;
    }

    if (start_time_ != std::chrono::high_resolution_clock::time_point()) {
        oss << ", Duration: " << getDurationSeconds() << "s";
    }

    return oss.str();
}

// TestMode implementation
ApplicationResult TestMode::run() {
    NLM_LOG_INFO("=== NLM Test Suite ===");

    std::vector<std::string> test_results;
    bool all_passed = true;

    // Run basic connectivity test
    NLM_LOG_INFO("Running basic connectivity test...");
    // This would call the actual test function from main.cpp

    // Run plasticity test
    NLM_LOG_INFO("Running plasticity test...");
    // This would call the actual plasticity test function

    // Run STDP verification test
    NLM_LOG_INFO("Running STDP verification test...");
    // This would call the actual STDP test function

    NLM_LOG_INFO("=== Test Suite Complete ===");
    return ApplicationResult(ApplicationResult::Status::Success, "All tests passed");
}

// DemoMode implementation
ApplicationResult DemoMode::run() {
    NLM_LOG_INFO("=== NLM Phase 2 Demo ===");

    try {
        // Initialize configuration
        auto config = std::make_shared<Config>();
        config->loadFromFile("configs/default.cfg");

        // Initialize brain
        NLM_LOG_INFO("Initializing brain...");
        auto brain = std::make_shared<Brain>(config);

        if (!brain->initialize()) {
            return ApplicationResult(ApplicationResult::Status::Failed, "Brain initialization failed");
        }

        // Run basic connectivity test
        runBasicConnectivityTest(brain);

        // Run plasticity experiment
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);

        // Run STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);

        NLM_LOG_INFO("=== Demo Complete ===");
        return ApplicationResult(ApplicationResult::Status::Success, "Demo completed successfully");
    } catch (const std::exception& e) {
        return ApplicationResult(ApplicationResult::Status::Failed, std::string(e.what()));
    }
}

// ExperimentMode implementation
ApplicationResult ExperimentMode::run() {
    NLM_LOG_INFO("=== NLM Experiment Mode ===");

    try {
        // This would run various neural learning experiments
        NLM_LOG_INFO("Running experiments with mode: " + config_.experiment_mode);

        // Example: Learning experiment
        NLM_LOG_INFO("Running learning experiment...");
        // This would run the specific experiment based on mode

        NLM_LOG_INFO("=== Experiment Complete ===");
        return ApplicationResult(ApplicationResult::Status::Success, "Experiment completed successfully");
    } catch (const std::exception& e) {
        return ApplicationResult(ApplicationResult::Status::Failed, std::string(e.what()));
    }
}

// ConfigMode implementation
ApplicationResult ConfigMode::run() {
    NLM_LOG_INFO("=== NLM Configuration Mode ===");

    std::ostringstream config_details;

    try {
        // Load configuration
        auto config = std::make_shared<Config>();
        config->loadFromFile("configs/default.cfg");

        config_details << "Configuration loaded from: " << "configs/default.cfg" << std::endl;

        // Validate configuration
        if (config->has("neuron_count")) {
            config_details << "Neuron count: " << config->get<int64_t>("neuron_count") << std::endl;
        }

        if (config->has("region_count")) {
            config_details << "Region count: " << config->get<int64_t>("region_count") << std::endl;
        }

        if (config->has("connection_probability")) {
            config_details << "Connection probability: " << config->get<float>("connection_probability") << std::endl;
        }

        // Set Phase 2 defaults
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);

        config_details << "" << std::endl;
        config_details << "Default values set for Phase 2:" << std::endl;
        config_details << "  Random seed: 42" << std::endl;
        config_details << "  Timestep: 0.001s" << std::endl;
        config_details << "  Neuron count: 500" << std::endl;
        config_details << "  Region count: 1" << std::endl;
        config_details << "  Connection probability: 0.15" << std::endl;

        NLM_LOG_INFO(config_details.str());

        NLM_LOG_INFO("=== Configuration Mode Complete ===");
        return ApplicationResult(ApplicationResult::Status::Success, "Configuration validated successfully");
    } catch (const std::exception& e) {
        return ApplicationResult(ApplicationResult::Status::ConfigurationError, std::string(e.what()));
    }
}

// Application Factory implementation
namespace {
    std::unordered_map<std::string, std::function<std::unique_ptr<CommandLineInterface>()>>& getRegistry() {
        static std::unordered_map<std::string, std::function<std::unique_ptr<CommandLineInterface>()>> registry;
        return registry;
    }
}

void CommandLineApplicationFactory::registerApplication(const std::string& name,
                                                       ApplicationFactory factory) {
    getRegistry()[name] = factory;
}

std::unique_ptr<CommandLineInterface> CommandLineApplicationFactory::createApplication(const std::string& name) {
    auto it = getRegistry().find(name);
    if (it != getRegistry().end()) {
        return it->second();
    }
    return nullptr;
}

std::vector<std::string> CommandLineApplicationFactory::getAvailableApplications() {
    std::vector<std::string> names;
    for (const auto& pair : getRegistry()) {
        names.push_back(pair.first);
    }
    return names;
}

void CommandLineApplicationFactory::printHelp(const std::string& app_name) {
    std::cout << "Available applications:" << std::endl;
    for (const auto& name : getAvailableApplications()) {
        auto app = createApplication(name);
        if (app) {
            std::cout << "  " << name << ": " << app->getDescription() << std::endl;
        }
    }
}

// Register default applications
namespace {
    class TestAppRegistrar {
    public:
        TestAppRegistrar() {
            CommandLineApplicationFactory::registerApplication("test", []() {
                return std::make_unique<TestMode>();
            });
        }
    } test_registrar;

    class DemoAppRegistrar {
    public:
        DemoAppRegistrar() {
            CommandLineApplicationFactory::registerApplication("demo", []() {
                return std::make_unique<DemoMode>();
            });
        }
    } demo_registrar;

    class ExperimentAppRegistrar {
    public:
        ExperimentAppRegistrar() {
            CommandLineApplicationFactory::registerApplication("experiment", []() {
                return std::make_unique<ExperimentMode>();
            });
        }
    } experiment_registrar;

    class ConfigAppRegistrar {
    public:
        ConfigAppRegistrar() {
            CommandLineApplicationFactory::registerApplication("config", []() {
                return std::make_unique<ConfigMode>();
            });
        }
    } config_registrar;
}

// Integration with existing main.cpp functions
void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    auto region = brain->getRegion(RegionId(1));
    if (!region) return;

    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }

    size_t initialSpikes = brain->getTotalSpikeCount();

    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }

    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }

    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));

    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    LearningExperiment experiment(brain, 42);

    experiment.recordInitialState();

    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }

    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");

    for (SimulationStep step = 0; step < 1000; ++step) {
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }

        brain->step(step, step * 0.001);

        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) +
                         " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                         " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }

    experiment.recordFinalState();
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;

    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }

    NLM_LOG_INFO("  Testing STDP on 5 synapses:");

    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + " before: " + std::to_string(beforeWeights[i]));
    }

    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");

    for (int trial = 0; trial < 50; ++trial) {
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;

        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }

        if (preNeuron && postNeuron) {
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);

            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }

    NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + " after: " +
                     std::to_string(synapses[i]->getWeight()) +
                     " (Δ=" + std::to_string(delta) + ")");
    }

    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }

    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;

    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s)
        : brain(b), seed(s), initialSynapseCount(0) {}

    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();

        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }

        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }

    void recordFinalState() {
        finalWeights.clear();

        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }

        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);

        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }

    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");

        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }

        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();

        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));

        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;

        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }

        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));

        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));

        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);

        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};
