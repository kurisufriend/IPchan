#!/usr/bin/env python3
from os import system
from sys import argv
from secrets import token_hex
from json import loads, dumps

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
    with open("./config.json", "r+") as f:
        cfg = loads(f.read())
        cfg["captcha_secret"] = token_hex(1024)
        f.seek(0)
        f.write(dumps(cfg, indent = 4))
if "r" in flags:
    print("running~~~")
    system("./bin/Debug/IPchan ./sample")
if "d" in flags:
    system("gdb ./bin/Debug/IPchan")
if "pr" in flags:
    system("valgrind --tool=callgrind bin/Debug/IPchan ./sample/")
    system("kcachegrind ./callgrind*")