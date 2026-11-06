Comprehensive CLI argument parsing for NLM demo applications

This file provides an enhanced command-line argument parsing system that builds upon the
existing Config class, offering better user experience, validation, and common argument
templates for all NLM demo applications.

Features:
- Rich argument types and validation
- Subcommand support for organizing demo types
- Common argument templates for different categories
- Advanced help generation with colored output
- Integration with existing Config class
- Error handling and validation
- Persistent argument state management

Usage:
1. Define arguments using CLIArgument definitions
2. Add argument templates for common categories (brain, world, simulation, etc.)
3. Parse command line with CLIParser
4. Access parsed values through the Config class interface

This framework is designed to make all NLM demo applications have consistent and
powerful command-line interfaces while maintaining backward compatibility with
existing Config::loadFromArgs functionality.