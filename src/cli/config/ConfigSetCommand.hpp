class ConfigSetCommand : public Command {
public:
    std::string getName() const override { return "set"; }
    std::string getDescription() const override { 
        return "Set configuration value"; 
    }
    std::string getUsage() const override { 
        return "nlm config set <key> <value>"; 
    }
    
    bool validateArgs(const std::vector<std::string>& args) override {
        return args.size() >= 2;
    }
    
    bool execute(const std::vector<std::string>& args) override;
};