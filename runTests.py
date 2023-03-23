#!/usr/bin/env python3

import os

cwd = os.getcwd()

config_dir = "./CSE331-S2023-Lab2/confs"
trace_dir = "./CSE331-S2023-Lab2/traces"
output_dir = "./output/"

# Get list of config files
config_files = os.listdir(config_dir)

# Get list of trace files
trace_files = os.listdir(trace_dir)

# Create output directory
if(not os.path.exists(f"{cwd}/{output_dir}")):
    os.mkdir(f"{cwd}/{output_dir}")
for config_file in config_files:
    if(not os.path.exists(f"{cwd}/{output_dir}/{config_file}")):
        os.mkdir(f"{cwd}/{output_dir}/{config_file}")

# Loop over every combination of config and trace files
for config_file in config_files:
    for trace_file in trace_files:
        # Construct command to execute
        command = f'./cachesim {config_dir}/{config_file} {trace_dir}/{trace_file}'

        # Execute command
        os.system(command)
        if(os.path.exists(f"{trace_dir}/{trace_file}.out")):
            os.rename(f"{trace_dir}/{trace_file}.out", f"{output_dir}/{config_file}/{trace_file}.out")
