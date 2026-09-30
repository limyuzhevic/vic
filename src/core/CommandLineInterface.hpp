#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <any>
#include <typeinfo>
#include <stdexcept>
#include <sstream>
#include "core/Logger/Logger.hpp"

namespace nlm {

class CommandLineConfig {
public:
    struct Option {
        std::string long_name;
        std::string short_name;
        std::string description;
        std::any default_value;
        bool required;
        std::string type_hint;
    };

    CommandLineConfig(int argc, char** argv, const std::string& app_name);
    ~CommandLineConfig() = default;

    bool parse();
    bool isValid() const { return valid_; }
    std::string getError() const { return error_message_; }

    template<typename T>
    T get(const std::string& key) const {
        if (!has(key)) {
            throw std::runtime_error("Missing required option: " + key);
        }
        return std::any_cast<T>(options_[key]);
    }

    template<typename T>
    T get(const std::string& key, const T& default_value) const {
        if (!has(key)) {
            return default_value;
        }
        return std::any_cast<T>(options_[key]);
    }

    bool has(const std::string& key) const {
        return options_.find(key) != options_.end();
    }

    std::vector<std::string> getRemainingArgs() const { return remaining_args_; }

    static void printHelp(const std::string& app_name, const std::vector<Option>& options);

private:
    bool valid_;
    std::string error_message_;
    int argc_;
    char** argv_;
    std::string app_name_;
    std::unordered_map<std::string, std::any> options_;
    std::vector<std::string> remaining_args_;

    static std::any stringToAny(const std::string& value, const std::string& type_hint);
    static std::string anyToString(const std::any& value);
    bool parseArgument(const std::string& arg);
    bool parseKeyValuePair(const std::string& arg);
    bool validateOptions();
};

class CommandLineApplication {
public:
    struct Config {
        std::string app_name = "nlm";
        std::string version = "0.1.0";
        std::string description = "NLM (Neural Learning Machine) - Experimental AI Brain";
        bool verbose = false;
        bool quiet = false;
        std::string log_level = "info";
        std::string config_file = "configs/default.cfg";
        bool run_tests = false;
        bool run_demo = false;
        bool run_experiments = false;
        std::string experiment_mode = "all";
        std::string output_format = "text";
        std::string output_file = "";
        double simulation_duration = 10.0;
        size_t max_steps = 10000;
        size_t warmup_steps = 1000;
        bool enable_gui = false;
        std::string gui_backend = "terminal";
        bool enable_performance_monitoring = false;
        size_t checkpoint_interval = 1000;
        std::string checkpoint_dir = "./checkpoints";
    };

    CommandLineApplication(int argc, char** argv);
    ~CommandLineApplication() = default;

    bool run() { return run(config_); }
    bool run(const Config& config);

    static std::unique_ptr<CommandLineApplication> create(int argc, char** argv) {
        return std::make_unique<CommandLineApplication>(argc, argv);
    }

    const Config& getConfig() const { return config_; }
    std::shared_ptr<CommandLineConfig> getOptions() const { return options_; }

protected:
    virtual bool setup() { return true; }
    virtual bool execute() { return true; }
    virtual bool cleanup() { return true; }

    bool runWithConfig(const Config& config);

private:
    int argc_;
    char** argv_;
    Config config_;
    std::shared_ptr<CommandLineConfig> options_;
    std::unique_ptr<class ILogger> logger_;

    bool setupLogging();
    bool initializeEnvironment();
    bool createDirectories() const;
    void printBanner() const;
    bool processCommandLineOptions();
};

class ApplicationResult {
public:
    enum class Status {
        Success,
        Failed,
        PartiallyCompleted,
        UserInterrupted,
        ConfigurationError
    };

    ApplicationResult(Status status = Status::Success, const std::string& message = "")
        : status_(status), message_(message) {}

    Status getStatus() const { return status_; }
    const std::string& getMessage() const { return message_; }
    const std::string& getDetails() const { return details_; }
    const std::chrono::high_resolution_clock::time_point& getStartTime() const { return start_time_; }
    const std::chrono::high_resolution_clock::time_point& getEndTime() const { return end_time_; }
    double getDurationSeconds() const;

    void setDetails(const std::string& details) { details_ = details; }
    void setStartTime(const std::chrono::high_resolution_clock::time_point& time) { start_time_ = time; }
    void setEndTime(const std::chrono::high_resolution_clock::time_point& time) { end_time_ = time; }

    std::string toString() const;

private:
    Status status_;
    std::string message_;
    std::string details_;
    std::chrono::high_resolution_clock::time_point start_time_;
    std::chrono::high_resolution_clock::time_point end_time_;
};

class CommandLineInterface {
public:
    CommandLineInterface() = default;
    virtual ~CommandLineInterface() = default;

    virtual bool initialize(const CommandLineApplication::Config& config) = 0;
    virtual ApplicationResult run() = 0;
    virtual bool cleanup() = 0;

    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool isEnabled() const = 0;
};

class CommandLineApplicationFactory {
public:
    using ApplicationFactory = std::function<std::unique_ptr<CommandLineInterface>()>;

    static void registerApplication(const std::string& name, ApplicationFactory factory);
    static std::unique_ptr<CommandLineInterface> createApplication(const std::string& name);
    static std::vector<std::string> getAvailableApplications();
    static void printHelp(const std::string& app_name);

private:
    static std::unordered_map<std::string, ApplicationFactory>& getRegistry();
};

class TestMode : public CommandLineInterface {
public:
    bool initialize(const CommandLineApplication::Config& config) override {
        config_ = config;
        return true;
    }

    ApplicationResult run() override;
    bool cleanup() override { return true; }

    std::string getName() const override { return "test"; }
    std::string getDescription() const override {
        return "Run unit and integration tests";
    }
    bool isEnabled() const override { return config_.run_tests; }

private:
    CommandLineApplication::Config config_;
};

class DemoMode : public CommandLineInterface {
public:
    bool initialize(const CommandLineApplication::Config& config) override {
        config_ = config;
        return true;
    }

    ApplicationResult run() override;
    bool cleanup() override { return true; }

    std::string getName() const override { return "demo"; }
    std::string getDescription() const override {
        return "Run demonstration simulations";
    }
    bool isEnabled() const override { return config_.run_demo; }

private:
    CommandLineApplication::Config config_;
};

class ExperimentMode : public CommandLineInterface {
public:
    bool initialize(const CommandLineApplication::Config& config) override {
        config_ = config;
        return true;
    }

    ApplicationResult run() override;
    bool cleanup() override { return true; }

    std::string getName() const override { return "experiment"; }
    std::string getDescription() const override {
        return "Run experiments with neural learning";
    }
    bool isEnabled() const override { return config_.run_experiments; }

private:
    CommandLineApplication::Config config_;
};

class ConfigMode : public CommandLineInterface {
public:
    bool initialize(const CommandLineApplication::Config& config) override {
        config_ = config;
        return true;
    }

    ApplicationResult run() override;
    bool cleanup() override { return true; }

    std::string getName() const override { return "config"; }
    std::string getDescription() const override {
        return "Configure and validate configuration";
    }
    bool isEnabled() const override { return true; } // Always available for config validation

private:
    CommandLineApplication::Config config_;
};

} // namespace nlm
