#!/usr/bin/env python3

import re
import os

INCLUDE_DIR = "../../lib/include"

OUTPUT_HPP = "Fixbrot/Fixbrot.h"

RE_INCLUDE = re.compile(r'#include\s+"(.+)"')

included_files = set()

def include_file(inc_path):
    path = os.path.join(INCLUDE_DIR, inc_path)
    
    if path in included_files:
        return
    included_files.add(path)

    with open(path, "r") as f:
        lines = f.readlines()
        for line in lines:
            match = RE_INCLUDE.search(line)
            if match:
                out_f.write(f"// {line}\n")
                inc_path = match.group(1)
                line = include_file(inc_path)
            else:
                out_f.write(line)

with open(OUTPUT_HPP, "w") as out_f:
    out_f.write("#ifndef FIXBROT_H\n")
    out_f.write("#define FIXBROT_H\n")
    out_f.write("\n")
    
    include_file(os.path.join("fixbrot/fixbrot.hpp"))

    out_f.write("\n")
    out_f.write("#endif\n")
