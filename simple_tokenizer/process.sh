#!/bin/bash

fst_folder=fsts
img_folder=imgs

mkdir -p $fst_folder
mkdir -p $img_folder

run_fst=./example
tokenization_fst=tokenization
combination_fst=word_combination
tokenization_and_combination_fst=tokenization_and_combination
result_fst=result

# fst files
tokenization_fst_file=$fst_folder/$tokenization_fst.fst
combination_fst_file=$fst_folder/$combination_fst.fst
tokenization_and_combination_fst_file=$fst_folder/$tokenization_and_combination_fst.fst
result_fst_file=$fst_folder/$result_fst.fst

# image files
tokenization_fst_img_file=$img_folder/$tokenization_fst.png
combination_fst_img_file=$img_folder/$combination_fst.png
tokenization_and_combination_fst_img_file=$img_folder/$tokenization_and_combination_fst.png
result_fst_img_file=$img_folder/$result_fst.png


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
fstdraw $tokenization_fst_file  | dot -Tpng -Gdpi=300 > $tokenization_fst_img_file
convert $tokenization_fst_img_file -rotate 90 $tokenization_fst_img_file

fstdraw $combination_fst_file | dot -Tpng -Gdpi=300 > $combination_fst_img_file
convert $combination_fst_img_file -rotate 90 $combination_fst_img_file

fstdraw $tokenization_and_combination_fst_file | dot -Tpng -Gdpi=300 > $tokenization_and_combination_fst_img_file
convert $tokenization_and_combination_fst_img_file -rotate 90 $tokenization_and_combination_fst_img_file

fstdraw $result_fst_file | dot -Tpng -Gdpi=300 > $result_fst_img_file
convert $result_fst_img_file -rotate 90 $result_fst_img_file


