import nltk
from nltk.corpus import stopwords, wordnet
from nltk.stem import WordNetLemmatizer, PorterStemmer
import re

def simple_pos_tag(words):
    """
    A simple rule-based POS tagger for demonstration purposes.
    """
    tagged_words = []
    suffix_tags = {
        'ing': 'VBG', 'ed': 'VBD', 'es': 'VBZ', 's': 'NNS', 'ly': 'RB', 'al': 'JJ', 
        'ive': 'JJ', 'ful': 'JJ', 'ous': 'JJ', 'ble': 'JJ', 'ic': 'JJ'
    }

    for word in words:
        tag = 'NN'  # Default to noun
        for suffix, suffix_tag in suffix_tags.items():
            if word.endswith(suffix):
                tag = suffix_tag
                break
        tagged_words.append((word, tag))
    
    return tagged_words

class Text_Processor:
    def __init__(self, stopword = True, stemming = False, lemmatize = False):
        self.stopwords = stopword
        self.stemming = stemming
        self.lemmatize = lemmatize
        self.ps = PorterStemmer()
        if(stopword):
            self.stopwords_list = set(stopwords.words('english'))
        if(lemmatize):
            self.ps = WordNetLemmatizer()
        self.term_index = {}
        self.lower_case = True
        self.word2vec = set()
    
    def set_lower_case(self, lower_case):
        self.lower_case = lower_case

    def get_wordnet_pos(self, treebank_tag):
        if treebank_tag.startswith('J'):
            return wordnet.ADJ
        elif treebank_tag.startswith('V'):
            return wordnet.VERB
        elif treebank_tag.startswith('N'):
            return wordnet.NOUN
        elif treebank_tag.startswith('R'):
            return wordnet.ADV
        else:
            return wordnet.NOUN

    def get_words(self,data):
        split_pattern = r'[^a-zA-Z0-9-]'
        if(self.lower_case):
            data = data.lower()
        words = re.split(split_pattern, data)
        if(self.stemming):
            words = [self.ps.stem(word) for word in words]
        if(self.lemmatize):
            pos_tags = simple_pos_tag(words)
            words = [self.ps.lemmatize(token, self.get_wordnet_pos(pos)) for token, pos in pos_tags]
        words = [word for word in words if word]
        return words

    # def train(self, data):
    #     words = self.get_words(data)
    #     for word in words:
    #         if(self.stopwords and word in self.stopwords_list):
    #             continue
    #         if(word not in self.term_index):
    #             self.term_index[word] = len(self.term_index)

    def process(self, data, train = True):
        words = self.get_words(data)
        for word in words:
            if(word not in self.term_index):
                self.term_index[word] = len(self.term_index)
        result = []
        for word in words:
            if(self.lower_case):
                word = word.lower()
            if(self.stopwords and word in self.stopwords_list):
                continue
            if(word in self.term_index):
                result.append(self.term_index[word])
            elif train:
                self.term_index[word] = len(self.term_index)
                result.append(self.term_index[word])
        return result
    
    def process_query(self, query, load = False, ignore = False):
        words = self.get_words(query)
        result = []
        for word in words:
            if((self.stopwords and word in self.stopwords_list) or (word not in self.term_index and not load and not ignore) or (word not in self.word2vec and load and not ignore)):
                continue
            result.append(word)
        return result
    
    def add_word2vec(self, filename):
        with open(filename, 'r', encoding="UTF-8") as f:
            for line in f:
                word = line.split()[0]
                self.word2vec.add(word)