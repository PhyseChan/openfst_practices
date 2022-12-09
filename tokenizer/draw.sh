#!/bin/bash

fst_name=$1
png_name=$2

if [ -z "$fst_name" ]; then
    echo "Usage: $0 <fst_name> <png_name>"
    exit 1
fi

fstdraw fsts/$fst_name.fst | dot -Tpng -Gdpi=800 > imgs/$png_name.png
convert imgs/$png_name.png -rotate 90 imgs/$png_name.png

