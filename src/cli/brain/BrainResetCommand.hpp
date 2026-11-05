class BrainResetCommand : public Command {
public:
    std::string getName() const override { return "reset"; }
    std::string getDescription() const override { 
        return "Reset brain to initial state"; 
    }
    std::string getUsage() const override { 
        return "nlm brain reset [--preserve-config]"; 
    }
    
    bool validateArgs(const std::vector<std::string>& args) override {
        for (const auto& arg : args) {
            if (arg != "--preserve-config") return false;
        }
        return true;
    }
    
    bool execute(const std::vector<std::string>& args) override;
};