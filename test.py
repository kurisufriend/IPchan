#!/usr/bin/env python3
from os import system
from sys import argv

flags = []
if len(argv) <= 1:
    flags.extend(["pm", "m", "r", "t"])
else:
    flags.extend(argv[1:])

if "pm" in flags:
    system("premake5 gmake2")
if "m" in flags:
    #system("rm -r obj/*")
    system("rm ./bin/Debug/IPchan")
    system("make config=debug")
if "r" in flags:
    print("running~~~")
    system("./bin/Debug/IPchan ./sample")
if "d" in flags:
    system("gdb ./bin/Debug/IPchan")
if "pr" in flags:
    system("valgrind --tool=callgrind bin/Debug/IPchan ./sample/")
    system("kcachegrind ./callgrind*")