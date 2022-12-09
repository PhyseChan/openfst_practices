#!/bin/bash

fst_folder=fsts
img_folder=imgs

mkdir -p $fst_folder
mkdir -p $img_folder

run_fst=./digit
fst_file=$fst_folder/digit.fst
fst_pic=$img_folder/digit.png
fst_det=$fst_folder/det.fst
det_pic=$img_folder/det.png
fst_min=$fst_folder/min.fst
min_pic=$img_folder/min.png
fst_res=$fst_folder/res.fst
res_pic=$img_folder/res.png



if [ -z "$run_fst" ]; then
    echo "Usage: $0 <run_fst>"
    exit 1
fi

if [ ! -x "$run_fst" ]; then
    echo "Error: $run_fst is not executable"
    exit 1
fi

# Run the fst_file
echo "Running $run_fst"
$run_fst


# draw the fst_file
fstdraw $fst_file  | dot -Tpng > $fst_pic
convert $fst_pic -rotate 90 $fst_pic

fstdraw $fst_det | dot -Tpng > $det_pic
convert $det_pic -rotate 90 $det_pic

fstdraw $fst_min | dot -Tpng > $min_pic
convert $min_pic -rotate 90 $min_pic

fstdraw $fst_res | dot -Tpng > $res_pic
convert $res_pic -rotate 90 $res_pic


