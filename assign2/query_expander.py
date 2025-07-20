from Text_Processor import Text_Processor as tp
from gensim.models import Word2Vec, KeyedVectors
from tqdm import tqdm
import random as rand
import linecache
import time

class QueryExpander:
    def __init__(self, text_processor):
        self.state_train = False
        self.state_load = False
        self.text_processor = text_processor
        self.model = None

    def train(self, filename, always_load, doc_line_no):
        self.state_train = True
        self.state_load = False
        sentences = []
        for doc in tqdm(always_load):
            line = linecache.getline(filename, doc_line_no[doc])
            sections = line.split('\t')
            sentences.append(self.text_processor.process_query(sections[2], ignore = True))
            if(sentences[-1] == []):
                sentences.pop()
            sentences.append(self.text_processor.process_query(sections[3], ignore = True))
            if(sentences[-1] == []):
                sentences.pop()
        print("Training Word2Vec Model")
        self.model = Word2Vec(sentences, vector_size=50, window=10, min_count=60, workers=8)
        self.model_vocab = set(self.model.wv.index_to_key)
        print("Trained")

    def load(self, filename, glove):
        print("Loading Model")
        self.model = KeyedVectors.load_word2vec_format(filename,no_header=glove, binary=False)
        print("Loaded")
        self.state_train = False
        self.state_load = True

    def query_expansion(self, query, lm, expansion_file, query_id):
        with open(expansion_file, 'a') as f:
            term_query = self.text_processor.process(query, train = False)
            term_query = [(x, 1) for x in term_query]
            query = self.text_processor.process_query(query, self.state_load)
            if(self.state_train):
                f.write(query_id+":")
                temp_query = []
                for word in query:
                    if word  in self.model_vocab:
                        temp_query.append(word)
                query = temp_query
                expansion = []
                if(len(query) == 0):
                    add_on = []
                else: add_on = self.model.wv.most_similar(query, topn=10)
                for word in add_on:
                    if(word[0] in self.text_processor.term_index and lm.index_count[self.text_processor.term_index[word[0]]]!=0 and word[0] not in query):
                        f.write(word[0])
                        expansion.append((self.text_processor.term_index[word[0]], word[1]))
                        if(len(expansion) == 20):
                            break
                        f.write(', ')
                f.write('\n')
                term_query.extend(expansion)
            elif(self.state_load):
                expansion = []
                if(len(query) == 0):
                    add_on = []
                else: add_on = self.model.most_similar(query, topn=30)
                f.write(query_id+":")
                for word in add_on:
                    if(word[0] in self.text_processor.term_index and lm.index_count[self.text_processor.term_index[word[0]]]!=0 and word[0] not in query):
                        f.write(word[0])
                        expansion.append((self.text_processor.term_index[word[0]], word[1]))
                        if(len(expansion) == 20):
                            break
                        f.write(', ')
                term_query.extend(expansion)
                f.write('\n')
            return term_query
    
