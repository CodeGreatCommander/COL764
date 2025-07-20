#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 6 ]; then
    echo "Usage: $0 [query-file] [top-100-file] [collection-file] [w2v-embeddingsfile] [output-file] [expansions-file]"
    exit 1
fi

# Assign positional parameters to variables
query_file=$1
top_100_file=$2
collection_file=$3
w2v_embeddingsfile=$4
output_file=$5
expansions_file=$6

rm -f $output_file
rm -f $expansions_file
python main.py $query_file $top_100_file $collection_file $output_file $expansions_file True $w2v_embeddingsfile False

# Exit the script
exit 0