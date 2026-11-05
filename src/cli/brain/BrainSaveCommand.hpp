class BrainSaveCommand : public Command {
public:
    std::string getName() const override { return "save"; }
    std::string getDescription() const override { 
        return "Save brain state to checkpoint"; 
    }
    std::string getUsage() const override { 
        return "nlm brain save <path>"; 
    }
    
    bool validateArgs(const std::vector<std::string>& args) override {
        return args.size() == 1;
    }
    
    bool execute(const std::vector<std::string>& args) override;
};