import os
import re

def main():
    base_dir = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_4dd67c99-0b18-44fb-9894-04e4482c8e0c/src"
    
    print("=== NLM Include Path Analysis ===")
    print(f"Directory: {base_dir}")
    print()
    
    # Count all .cpp and .hpp files
    cpp_files = []
    hpp_files = []
    for root, dirs, files in os.walk(base_dir):
        for file in files:
            if file.endswith('.cpp'):
                cpp_files.append(os.path.join(root, file))
            elif file.endswith('.hpp'):
                hpp_files.append(os.path.join(root, file))
    
    total_files = len(cpp_files) + len(hpp_files)
    
    print("=== FILE COUNT ===")
    print(f"Total .cpp files: {len(cpp_files)}")
    print(f"Total .hpp files: {len(hpp_files)}")
    print(f"Total source files: {total_files}")
    print()
    
    # Find files with '../' relative includes
    print("=== FILES WITH '../' RELATIVE INCLUDES ===")
    
    affected_files = []
    for file_path in cpp_files + hpp_files:
        try:
            with open(file_path, 'r') as f:
                content = f.read()
                if re.search(r'#include.*\.\.\/[^\"\]]*\.hpp\"', content):
                    affected_files.append(file_path)
        except Exception as e:
            pass
    
    if affected_files:
        print(f"Number of affected files: {len(affected_files)}")
        print()
        print("Affected files:")
        for f in affected_files:
            print(f)
        print()
        
        print("=== EXAMPLE INCLUDES FROM AFFECTED FILES ===")
        include_counts = {}
        for file_path in affected_files:
            try:
                with open(file_path, 'r') as f:
                    content = f.read()
                    includes = re.findall(r'#include.*\.\.\/[^\"\]]*\.hpp\"', content)
                    for inc in includes:
                        include_counts[inc] = include_counts.get(inc, 0) + 1
            except Exception:
                pass
        
        print(f"\nTotal unique '../' include patterns: {len(include_counts)}")
        for inc, count in sorted(include_counts.items(), key=lambda x: x[1], reverse=True):
            print(f"{count:3d} {inc}")
    else:
        print("No files found with '../' relative includes")
    
    # Additional analysis: all include patterns
    print("\n=== ALL UNIQUE INCLUDE PATTERNS ===")
    all_includes = set()
    for file_path in cpp_files + hpp_files:
        try:
            with open(file_path, 'r') as f:
                content = f.read()
                includes = re.findall(r'#include\s*[<"]([^">]*)[>"]', content)
                all_includes.update(includes)
        except Exception:
            pass
    
    print(f"Total unique include patterns: {len(all_includes)}")
    for inc in sorted(all_includes):
        print(f"  {inc}")

if __name__ == "__main__":
    main()