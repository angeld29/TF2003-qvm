#!/bin/bash
#:cex system('./pvs.sh')
cd _cmake/qvm/
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=On .
pvs-studio-analyzer analyze -j2 -o pvs.log -e ../../tools/ -e ../../_cmake/
plog-converter -a  GA:1,2 -t errorfile pvs.log
cd ../..
