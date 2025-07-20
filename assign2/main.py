
#-------------------Imports-------------------#
from Text_Processor import Text_Processor as tp
from langmodel import LangModel, GlobLangModel
from loader import load
from query_expander import QueryExpander

import random as rand
import linecache
from math import log2
from tqdm import tqdm
import sys

rand.seed(0)

#-------------------Constants-------------------#
global_model_extra = 50


#-------------------Global Variables-------------------#
language_models = {}
text_processor = tp()
doc_line_no = {}
doc_line_no_inverse = {}
always_load = set()


#-------------------Functions-------------------#

def train_text_processor(filename):
    with open(filename, 'r',encoding='UTF-8') as f:
        for line_no,line in tqdm(enumerate(f)):
            sections = line.split('\t')
            doc_line_no[sections[0]] = line_no+1
            doc_line_no_inverse[line_no+1] = sections[0]
#             if(sections[0] in always_load or rand.random() < text_processor_learning_ratio):
#                 text_processor.train(sections[2])
#                 text_processor.train(sections[3])


def get_docs_models(docs, filename, global_language_model):
    for doc in docs:
        if doc in language_models:
            continue
        language_models[doc] = LangModel()
        line = linecache.getline(filename, doc_line_no[doc])
        sections = line.split('\t')
        global_language_model.train(text_processor.process(sections[2]))
        global_language_model.train(text_processor.process(sections[3]))
        language_models[doc].train(text_processor.process(sections[2]))
        language_models[doc].train(text_processor.process(sections[3]))
    
    random_lines = rand.sample(range(1, len(doc_line_no)), global_model_extra)

    for doc in random_lines:
        line = linecache.getline(filename, doc)
        sections = line.split('\t')
        global_language_model.train(text_processor.process(sections[2]))
        global_language_model.train(text_processor.process(sections[3]))
    

def generate_results(query_id, query, query_docs, output_filename, global_language_model):
    global_score = global_language_model.get_prob(query)
    doc_scores = {}
    for doc in query_docs[query_id]:
        doc_scores[doc] = language_models[doc].get_prob(query, global_score)
    data = sorted(doc_scores.items(), key=lambda x: x[1], reverse=True)
    with open(output_filename, 'a') as f:
        for i in range(len(data)):
            f.write(query_id + '\t'+ 'Q0' + '\t' + data[i][0] + '\t' + str(i+1) + '\t' + str(data[i][1]) + '\trunid1\n')
    return data


if __name__ == "__main__":
    queries_filename, top_100_docs_filename, data_filename, output_filename, expansion_filename = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5]

    load_word2vec = sys.argv[6]=='True'
    lower_case = True
    if(load_word2vec):
        word2vec_path = sys.argv[7]
        glove = sys.argv[8]=='True'
        if(not glove):lower_case = False
        
    if(not load_word2vec):
        embedding = sys.argv[7]=='True'
    else:
        embedding = False

    text_processor.set_lower_case(lower_case)
    query_docs, queries = load(top_100_docs_filename, queries_filename)

    for lis in query_docs.values():
        for doc in lis:
            always_load.add(doc)

    train_text_processor(data_filename)
    qe = QueryExpander(text_processor)
    if(load_word2vec):
        qe.load(word2vec_path, glove)
        text_processor.add_word2vec(word2vec_path)
    elif(embedding):
        qe.train(data_filename, always_load, doc_line_no)
    for query_id, query in tqdm(queries):
        global_language_model = GlobLangModel()
        get_docs_models(query_docs[query_id], data_filename, global_language_model)
        query_list = qe.query_expansion(query, global_language_model, expansion_filename, query_id)
        data = generate_results(query_id, query_list, query_docs, output_filename, global_language_model)

            
