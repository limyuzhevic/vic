class BrainLoadCommand : public Command {
public:
    std::string getName() const override { return "load"; }
    std::string getDescription() const override { 
        return "Load brain state from checkpoint"; 
    }
    std::string getUsage() const override { 
        return "nlm brain load <path>"; 
    }
    
    bool validateArgs(const std::vector<std::string>& args) override {
        return args.size() == 1;
    }
    
    bool execute(const std::vector<std::string>& args) override;
};