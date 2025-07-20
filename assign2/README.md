# Document Reranking

**Course**: COL764  
**Assignment Number**: Assignment 2  
**Student Name**: Rohan Chaturvedi  
**Entry Number**: 2022MT11262  

---

## Table of Contents
1. [Assignment Overview](#assignment-overview)
2. [Structure of the Submission](#structure-of-the-submission)
3. [Dependencies](#dependencies)
4. [How to Run the Code](#how-to-run-the-code)
5. [Results](#results)
6. [Approach/Methodology](#approach-methodology)

---

### Assignment Overview

This assignment involves reranking the top 100 documents for 24 queries from the TREC benchmark using pseudo-relevance feedback and query expansion techniques. The task is divided into three main components:

1. **Base LM Ranking**: Implement a language model-based ranking method with Dirichlet smoothing to rerank the top 100 documents for each query.

2. **Local Word2Vec Reranking**: Train a Word2Vec model on the top-k pseudo-relevant documents, expand the query using the nearest terms, and rerank the documents.

3. **Generic Word2Vec and GloVe Reranking**: Use pre-trained Word2Vec and GloVe embeddings to find top-m nearest words for query expansion and reranking.

Evaluation is based on nDCG@{5, 10, 50}, and results should follow the TREC eval format.

---

## Structure of the Submission

The submission folder contains the following files, organized to clearly separate each component of the solution:

- **`main.py`**: Acts as the central control script for orchestrating the entire document reranking process.
  
- **`langmodel.py`**: Implements classes for the language modeling retrieval approach, including the base language model with Dirichlet smoothing.

- **`loader.py`**: Responsible for reading the input files such as queries and the top 100 document list. This module handles the parsing of TSV files.

- **`query_expander.py`**: Contains functionality to train and load Word2Vec models and perform query expansion by selecting relevant terms based on embeddings.

- **`Text_Processor.py`**: Provides text processing utilities, including stop-word removal, lemmatization, and stemming for query and document preprocessing.

- **`calculate_score.py`**: Computes and evaluates the performance metrics (nDCG@5, nDCG@10, nDCG@50) for the retrieval results.

- **`glove-gen_rerank.sh`**: Shell script for reranking documents based on the pre-trained GloVe model.

- **`w2v-gen_rerank.sh`**: Shell script for reranking documents using the pre-trained Word2Vec model.

- **`w2v-local_rerank.sh`**: Shell script for reranking documents based on a locally trained Word2Vec model using the top-k pseudo-relevant documents. 

Each component is modularized to facilitate efficient execution and to maintain clarity of the solution structure.

---

## Dependencies
- **Required Libraries/Packages**: The code is implemented in Python and requires the following libraries. Please include the version numbers to ensure compatibility.

```bash
nltk
gensim
numpy
nltk.popular()
```

- **Installation**: The dependencies can be installed using the following command:

```bash
pip install nltk gensim numpy

nltk.download('popular')
```

---

## How to Run the Code
- **Instructions**: Provide clear and concise instructions on how to run your code. Include any necessary command-line arguments or specific file inputs.

Example:
```bash
# To run the local Word2Vec reranking script:
bash w2v-local_rerank.sh  [query-file] [top-100-file] [collection-file] [output-file]
[expansions-file]

# To run the generic Word2Vec reranking script:
bash w2v-gen_rerank.sh [query-file] [top-100-file] [collection-file] [w2v-embeddingsfile] [output-file] [expansions-file]

# To run the generic GloVe reranking script:
bash glove-gen_rerank.sh [query-file] [top-100-file] [collection-file] [glove-embeddingsfile] [output-file] [expansions-file]

```

---

## Results
- **Output**: The results would be generated in the TREC eval format, including the nDCG scores for the reranked documents and also the query expansions, at locations specified by the output file path. To generate nDCG scores, the `trec_eval` tool can be used.

---

### Approach/Methodology

**Methodology**: In this document reranking task, I explored three models—Local Word2Vec, Generic Word2Vec, and Generic GloVe models—to enhance the retrieval accuracy of the top-100 documents retrieved for each query using query expansion techniques. The primary objective was to utilize pseudo-relevance feedback and word embeddings to introduce semantically related terms into the query, thereby improving ranking results in terms of precision and recall.

For all tasks, I employed Kullback-Leibler (KL) divergence for reranking the documents based on the expanded queries. The expanded queries were designed to represent a richer semantic space through embedding models, leading to an improved ranking of documents closer in meaning to the query. 

1. **Task 0: Language Model Retrieval (Baseline)**  
   The first task involved implementing a baseline language modeling-based retrieval using Dirichlet smoothing, with the purpose of ranking documents before applying any reranking techniques. The model calculates the likelihood of generating a query from a document and applies Dirichlet smoothing to balance between the document-specific term frequencies and the overall collection frequencies. The hyperparameter tuning process for the smoothing parameter ($\mu$) was conducted based on nDCG scores.

2. **Task 1: Local Word2Vec Query Expansion and Reranking**  
   The local Word2Vec model was trained on the top-$k$ documents retrieved for each query. I used the SkipGram architecture to learn word embeddings from this local pseudo-relevance feedback set. Then, for each query, I found the nearest words to the query terms using the trained embeddings and expanded the query accordingly. After expansion, documents were reranked using KL divergence, which measured how well a document’s language model matched the expanded query’s model.

3. **Task 2: Generic Word2Vec and GloVe Reranking**  
   In this task, I leveraged pre-trained word embeddings from two well-known models—Google News Word2Vec and GloVe (Wikipedia + Gigaword corpus). Similar to Task 1, the queries were expanded using the nearest neighbors of query terms in the pre-trained embeddings’ vector space. Again, KL divergence was used to rank documents based on how well their language models matched the expanded query.

**Challenges**:  
Several challenges were encountered during the implementation and optimization of the models:

1. **Performance Trade-offs with Stopword Removal**: Initially, stopword removal, stemming, and lemmatization were experimented with during preprocessing. However, empirical results showed that while stopword removal improved precision, lemmatization and stemming did not yield significant gains, and in some cases, reduced performance. As a result, I opted for stopword removal only, excluding stemming and lemmatization from the final pipeline to focus on higher-impact terms.

2. **Local Word2Vec Model Training**: Training the local Word2Vec model was computationally intensive due to the large document set and the requirement for training on each query’s top-$k$ documents. Additionally, finding the optimal set of hyperparameters, such as vector size, window size, and minimum word frequency, required extensive experimentation. I settled on a vector size of 50, a window size of 10, and a minimum word frequency of 60 after evaluating their impact on reranking quality.

3. **Handling Vocabulary Mismatches in Pre-trained Models**: For Tasks 2 (Word2Vec and GloVe), a common challenge was that many query terms were not present in the pre-trained embeddings' vocabularies, leading to issues in query expansion. To mitigate this, I ensured that only terms present in both the query and document vocabulary were used for reranking. This limited the expansion but helped maintain relevance.

4. **Balancing Query Expansion and Original Terms**: Expanding the query with too many terms could dilute the relevance of the original query terms, negatively affecting the ranking. I experimented with different numbers of expansion terms (10, 20, 30, etc.) and found that expanding the query with 20 additional terms struck the best balance between semantic enrichment and relevance.

5. **Computational Efficiency**: Given the size of the TREC dataset and the complexity of reranking large document sets, computational efficiency was a concern. I optimized my code to parallelize parts of the Word2Vec training and KL divergence computation using multi-threading, significantly reducing the overall runtime.

6. **Tuning Dirichlet Smoothing Parameter**: For Task 0 (baseline language model), tuning the smoothing parameter ($\mu$) was critical. I ran multiple experiments, testing values of $\mu$ ranging from 50 to 2000. I observed that a value of $\mu = 500$ provided the best trade-off between precision and recall, resulting in the highest average nDCG scores.

---

Through careful evaluation of the results, I was able to determine that query expansion via word embeddings significantly improves retrieval performance. Both the locally trained Word2Vec model and pre-trained embeddings yielded notable improvements over the baseline language model in terms of nDCG metrics, demonstrating the value of semantic enrichment for document reranking.

