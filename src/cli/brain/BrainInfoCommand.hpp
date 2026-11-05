class BrainInfoCommand : public Command {
public:
    std::string getName() const override { return "info"; }
    std::string getDescription() const override { 
        return "Display brain statistics and metadata"; 
    }
    std::string getUsage() const override { 
        return "nlm brain info [--stats] [--neurons] [--synapses]"; 
    }
    
    bool validateArgs(const std::vector<std::string>& args) override {
        // Brain info accepts optional flags
        for (const auto& arg : args) {
            if (arg == "--stats" || arg == "--neurons" || arg == "--synapses") {
                continue;
            }
            if (arg.substr(0, 2) == "--") {
                return false;
            }
        }
        return true;
    }
    
    bool execute(const std::vector<std::string>& args) override;
};