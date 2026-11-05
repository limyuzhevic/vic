class ConfigGetCommand : public Command {
public:
    std::string getName() const override { return "get"; }
    std::string getDescription() const override { 
        return "Get specific configuration value"; 
    }
    std::string getUsage() const override { 
        return "nlm config get <key>"; 
    }
    
    bool validateArgs(const std::vector<std::string>& args) override {
        return args.size() == 1;
    }
    
    bool execute(const std::vector<std::string>& args) override;
};