import numpy

def load(top_100_docs_filename, queries_filename):

    query_docs = {}

    with open(top_100_docs_filename, 'r', encoding='UTF-8') as f:
        for i, line in enumerate(f):
            if i == 0:
                continue  # Skip the first line
            sections = line.split('\t')
            if sections[0] not in query_docs:
                query_docs[sections[0]] = []
            query_docs[sections[0]].append(sections[1])

    queries = []
    with open(queries_filename, 'r', encoding='UTF-8') as f:
        for line_number, line in enumerate(f):
            if line_number == 0:
                continue
            sections = line.split('\t')
            queries.append((sections[0], sections[1]))
    
    return query_docs, queries
            