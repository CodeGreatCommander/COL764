#include "utils.h"
#include <iostream>

bool PairComparator::operator()(const std::pair<size_t, std::pair<size_t, size_t>>& lhs,
                const std::pair<size_t, std::pair<size_t, size_t>>& rhs) const {
    if (lhs.first != rhs.first)  return lhs.first < rhs.first;
    else if(lhs.second.first != rhs.second.first) return lhs.second.first < rhs.second.first;
    else return lhs.second.second < rhs.second.second;
}

bool PairComparatorDouble::operator()(const std::pair<double, std::pair<size_t, size_t>>& lhs,
                const std::pair<double, std::pair<size_t, size_t>>& rhs) const {
    if (lhs.first != rhs.first)  return lhs.first < rhs.first;
    else if(lhs.second.first != rhs.second.first) return lhs.second.first < rhs.second.first;
    else return lhs.second.second < rhs.second.second;
}

    trie::trie():is_end(false){}
    void trie::insert(const string& word, size_t index){
        trie* node = this;
        for(char c: word){
            if(node->children.find(c)==node->children.end()){
                node->children[c] = new trie();
            }
            node = node->children[c];
        }
        node->is_end = true;
        node->index = index;
    }
    vector<size_t> trie::tokenise(const string& word,bool wordpiece){
        string temp="";
        for(const char& c:word){
            if(this->children.find(c)!=this->children.end()){
                temp+=c;
            }
        }
        vector<size_t> tokens;
        trie* node = nullptr;
        size_t l =0;
        while(l!=temp.size()){
            size_t e = l, sav = l, sav_id = 0;
            if(node==nullptr||!wordpiece)
            node = this;
            else
            node = this->children['@']->children['@'];
            while(e<temp.size()&&node->children.find(temp[e])!=node->children.end()){
                node = node->children[temp[e]];
                if(node->is_end){
                    sav = e;
                    sav_id = node->index;
                }
                e++;
            }
            tokens.push_back(sav_id);
            l = sav+1;
        }
        return tokens;   
    }

    size_t trie::search(const string& word){
        trie* node = this;
        for(char c: word){
            if(node->children.find(c)==node->children.end()){
                return -1;
            }
            node = node->children[c];
        }
        if(node->is_end)
        return node->index;
        return -1;
    }

    trie::~trie(){
        for(auto& child: children){
            delete child.second;
        }
    }