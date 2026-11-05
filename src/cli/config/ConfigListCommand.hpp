class ConfigListCommand : public Command {
public:
    std::string getName() const override { return "list"; }
    std::string getDescription() const override { 
        return "List all available configuration options"; 
    }
    std::string getUsage() const override { 
        return "nlm config list [format]"; 
    }
    
    bool validateArgs(const std::vector<std::string>& args) override {
        // Config list accepts optional format: list, detailed, or none
        if (args.size() > 1) return false;
        if (args.size() == 1) {
            std::string format = args[0];
            if (format != "list" && format != "detailed") return false;
        }
        return true;
    }
    
    bool execute(const std::vector<std::string>& args) override;
};