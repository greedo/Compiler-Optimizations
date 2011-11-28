#!/bin/sh
# allows multiple tests to be run

# copy over the asm to spim
cp ./program.asm ../spim/program.asm

# then run
cd ../spim
cat defs.asm >> program.asm
./spim.linux -count -file program.asm
