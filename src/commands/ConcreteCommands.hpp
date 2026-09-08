// BrainRunnerCommand - executes all default tests (replaces hardcoded execution)
class BrainRunnerCommand : public Command {
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    
public:
    BrainRunnerCommand(std::shared_ptr<Config> config) : config_(config) {}
    
    void execute() override {
        // Initialize brain
        brain_ = std::make_shared<Brain>(config_);
        if (!brain_->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        brain_->logStatus();
        
        // Run connectivity test
        runBasicConnectivityTest(brain_);
        
        // Reset and run plasticity experiment
        brain_->reset();
        brain_->initialize();
        runPlasticityExperiment(brain_);
        
        // Reset and run STDP verification
        brain_->reset();
        brain_->initialize();
        runStdpVerification(brain_);
        
        // Final status
        brain_->logStatus();
    }
    
    std::string getName() const override {
        return "brainrunner";
    }
    
    std::string getDescription() const override {
        return "Run all default tests: connectivity, plasticity, and STDP verification";
    }
};

// ConnectivityTestCommand
class ConnectivityTestCommand : public Command {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    ConnectivityTestCommand(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    void execute() override {
        runBasicConnectivityTest(brain_);
    }
    
    std::string getName() const override {
        return "connectivity";
    }
    
    std::string getDescription() const override {
        return "Run basic neural connectivity test";
    }
};

// PlasticityExperimentCommand
class PlasticityExperimentCommand : public Command {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    PlasticityExperimentCommand(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    void execute() override {
        runPlasticityExperiment(brain_);
    }
    
    std::string getName() const override {
        return "plasticity";
    }
    
    std::string getDescription() const override {
        return "Run plasticity learning experiment";
    }
};

// StdpVerificationCommand
class StdpVerificationCommand : public Command {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    StdpVerificationCommand(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    void execute() override {
        runStdpVerification(brain_);
    }
    
    std::string getName() const override {
        return "stdp";
    }
    
    std::string getDescription() const override {
        return "Verify STDP plasticity rule works";
    }
};

// ExportCommand
class ExportCommand : public Command {
private:
    std::string filename_;
    
public:
    ExportCommand(const std::string& filename) : filename_(filename) {}
    
    void execute() override {
        // TODO: Implement actual export functionality
        std::cout << "Exporting simulation data to: " << filename_ << std::endl;
        std::cout << "  (Export functionality to be implemented)" << std::endl;
    }
    
    std::string getName() const override {
        return "export";
    }
    
    std::string getDescription() const override {
        return "Export simulation data to file";
    }
};

// ImportCommand
class ImportCommand : public Command {
private:
    std::string filename_;
    
public:
    ImportCommand(const std::string& filename) : filename_(filename) {}
    
    void execute() override {
        // TODO: Implement actual import functionality
        std::cout << "Importing simulation data from: " << filename_ << std::endl;
        std::cout << "  (Import functionality to be implemented)" << std::endl;
    }
    
    std::string getName() const override {
        return "import";
    }
    
    std::string getDescription() const override {
        return "Import simulation data from file";
    }
};

// ConfigCommand
class ConfigCommand : public Command {
public:
    ConfigCommand() = default;
    
    void execute() override {
        std::cout << "Current configuration:" << std::endl;
        auto config = std::make_shared<Config>();
        std::cout << "  (Configuration display to be implemented)" << std::endl;
    }
    
    std::string getName() const override {
        return "config";
    }
    
    std::string getDescription() const override {
        return "Display or modify configuration";
    }
};

// HelpCommand
class HelpCommand : public Command {
private:
    CommandRegistry* registry_;
    
public:
    HelpCommand(CommandRegistry* registry) : registry_(registry) {}
    
    void execute() override {
        std::cout << "Available commands:" << std::endl;
        std::cout << std::endl;
        
        auto commandNames = registry_->getCommandNames();
        for (const auto& name : commandNames) {
            std::cout << "  " << name << " - TODO: get description" << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Usage: ./nlm [options] <command> [arguments]" << std::endl;
        std::cout << "  --help, -h: Show help" << std::endl;
        std::cout << "  --version, -v: Show version" << std::endl;
        std::cout << "  --config <file>: Configuration file (default: configs/default.cfg)" << std::endl;
    }
    
    std::string getName() const override {
        return "help";
    }
    
    std::string getDescription() const override {
        return "Show help information about available commands";
    }
};