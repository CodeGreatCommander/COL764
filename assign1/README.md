# Project README

## Compilation and Execution

To compile all files, run:

```bash
source build.sh
```

## Generating the Vocabulary

To generate the vocabulary, use:
```bash
source dictcons.sh [coll-path] {0|1|2}
```
Example:
```bash
source dictcons.sh /home/rohan/col764/assign1/data 1
```

## Generating the Posting List and Indexed File
To generate the posting list and indexed file, use:

```bash
source invidx.sh [coll-path] [indexfile] {0|1|2}
```

Example:
```bash
source invidx.sh /home/rohan/col764/assign1/data invertedindex 1
```

## Querying
To perform a TF-IDF search, use:
```bash
source tf_idf_search.sh [queryfile] [resultfile] [indexfile] [dictfile]
```

Example:
```bash
source tf_idf_search.sh /home/rohan/col764/assign1/data/cord19-trec_covid-queries result invertedindex.idx invertedindex.dict
```


Feel free to adjust paths or filenames as necessary!
