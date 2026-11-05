class ConfigValidateCommand : public Command {
public:
    std::string getName() const override { return "validate"; }
    std::string getDescription() const override { 
        return "Validate current configuration"; 
    }
    std::string getUsage() const override { 
        return "nlm config validate"; 
    }
    
    bool validateArgs(const std::vector<std::string>& args) override {
        return args.empty();
    }
    
    bool execute(const std::vector<std::string>& args) override;
};