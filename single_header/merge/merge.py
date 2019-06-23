import subprocess
import re

main_path = '../../graph_constraint_solver/main.cpp'
output_path = 'out.hpp'
single_header_path = 'single_header.cpp'

params = 'pcpp -o {} -D GRAPH_CONSTRAINT_SOLVER_SINGLE_HEADER {}'.format(output_path, main_path).split()
subprocess.run(params)

single_header_libs = [
    ['nlohmann/json.hpp', '/home/danilov/prog/nlohmann/nlohmann/json.hpp'],
]

with open(output_path, 'r') as f:
    lines = f.readlines()

with open(output_path, 'w') as f:
    for line in lines:
        good = True
        for include, path in single_header_libs:
            if re.match('#include.*<{}>'.format(include), line):
                good = False
        if good:
            f.write(line)   

merge_list = [path for include, path in single_header_libs] + [output_path]

with open(single_header_path, 'w') as single_file:
    for name in merge_list:
        with open(name, 'r') as current_file:
            single_file.write(current_file.read())
