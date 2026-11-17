import os
import glob

for root, dirs, files in os.walk("src"):
    for file in files:
        if file.endswith('.hpp') or file.endswith('.h'):
            base = os.path.splitext(file)[0]
            cpp_file = os.path.join(root, base + '.cpp')
            if not os.path.exists(cpp_file):
                # Check if this is a template header (contains template<typename> or template<class>)
                with open(os.path.join(root, file), 'r') as f:
                    content = f.read()
                    if 'template' in content:
                        print(f"{root}/ {base}: Template header (may have explicit instantiations)")
                    else:
                        print(f"{root}/ {base}: NO CPP FILE")