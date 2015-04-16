#!/bin/bash

command -v clib >/dev/null 2>&1 || { git clone https://github.com/clibs/clib.git /tmp/clib && cd /tmp/clib && make && make install;  }

clib install hjanuschka/buffer -o src/deps
#clib install clibs/list -o src 
