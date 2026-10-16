// Automated documentation generation for NLM
// 
// This file contains tools and processes for automatically generating
// comprehensive documentation from the NLM codebase.
// 
// Key features:
// 1. Automatic API documentation generation
// 2. Code example extraction and formatting
// 3. Documentation consistency checking
// 4. Cross-referencing between documents
// 5. Template-based documentation generation
// 6. Integration with existing documentation
// 7. Version-aware documentation updates
// 8. Performance documentation generation
// 
// Generated documentation includes:
// - API reference documentation
// - Code example gallery
// - Tutorial documentation
// - Best practices guide
// - Installation and setup instructions
// - Configuration reference
// - Performance benchmarks
// - Changelog documentation
// 
// Generation tools:
// 1. Doxygen integration for C++ API documentation
// 2. Python docstring extraction for Python bindings
// 3. Code example parser for example gallery
// 4. Documentation template engine
// 5. Cross-referencing tool
// 6. Validation and consistency checker
// 
// Usage:
//   ./docgen.sh generate-all      # Generate all documentation
//   ./docgen.sh generate-api      # Generate API documentation
//   ./docgen.sh generate-examples # Generate example gallery
//   ./docgen.sh validate-docs     # Validate documentation consistency
//   ./docgen.sh check-updates     # Check for documentation updates
// 
// Configuration:
// - Documentation templates in docs/templates/
// - Example sources in examples/
// - API source in src/
// - Generated output in docs/generated/
// 
// This tool can be extended to support:
// - Markdown-to-HTML conversion
// - Static site generation
// - Documentation hosting integration
// - Version control integration
// - Continuous integration hooks
// 
// Example script for documentation generation:
// ```bash
// #!/bin/bash
// set -e
// 
// SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
// DOCS_DIR="$SCRIPT_DIR/.."
// GENERATED_DIR="$DOCS_DIR/generated"
// 
// case "$1" in
//     generate-all)
//         echo "Generating all documentation..."
//         generate-api "$DOCS_DIR"
//         generate-examples "$DOCS_DIR"
//         generate-tutorials "$DOCS_DIR"
//         generate-best-practices "$DOCS_DIR"
//         copy-static "$DOCS_DIR"
//         echo "Documentation generation complete!"
//         ;;
//     
//     generate-api)
//         echo "Generating API documentation..."
//         doxygen "$DOCS_DIR/Doxyfile" || {
//             echo "Doxygen generation failed!"
//             exit 1
//         }
//         ;;
//     
//     generate-examples)
//         echo "Generating example gallery..."
//         python3 "$SCRIPT_DIR/generate_examples.py" "$DOCS_DIR"
//         ;;
//     
//     generate-tutorials)
//         echo "Generating tutorials..."
//         python3 "$SCRIPT_DIR/generate_tutorials.py" "$DOCS_DIR"
//         ;;
//     
//     generate-best-practices)
//         echo "Generating best practices..."
//         python3 "$SCRIPT_DIR/generate_best_practices.py" "$DOCS_DIR"
//         ;;
//     
//     validate-docs)
//         echo "Validating documentation..."
//         python3 "$SCRIPT_DIR/validate_docs.py" "$DOCS_DIR"
//         ;;
//     
//     check-updates)
//         echo "Checking for documentation updates..."
//         python3 "$SCRIPT_DIR/check_updates.py" "$DOCS_DIR"
//         ;;
//     
//     *)
//         echo "Usage: $0 {generate-all|generate-api|generate-examples|generate-tutorials|generate-best-practices|validate-docs|check-updates}"
//         exit 1
//         ;;
// esac
// ```
