#include "bpe.h"

BPE::BPE(unordered_map<string,uint32_t> word_count):word_count(word_count){
    bool check[128] = {0};
    for(auto& t: word_count){
        word_index[t.first] = word_index.size();
        words.push_back(t.first);
        word_decom.push_back({list<size_t>(),t.second});
        char prev = -1;
        for(const char& c: t.first){
            if(!check[c]){
                check[c] = true;
                vocab.push_back(string(1,c));
                vocab_index[string(1,c)] = vocab.size()-(size_t)1;
            }
            string ch = string(1,c);
            word_decom.back().first.push_back(vocab_index[ch]);
            if(prev!=-1){
                pair<size_t,size_t> p = {vocab_index[string(1,prev)],vocab_index[ch]};
                if(comb_count.find(p)!=comb_count.end()&&comb_count[p]>0)inv_comb_count.erase({comb_count[p],p});
                comb_count[p]+=t.second;
                inv_comb_count.insert({comb_count[p],p});
                comb_words[p].insert(words.size()-1);
            }
            prev = c;
        }
    }
}


void BPE::train(uint32_t ncomb){
    
    for(uint32_t i=1; i<=ncomb; i++){
        if(i%100==1){
            cout<<"\r"<<i<<"/"<<ncomb<<flush;
        }
        auto it = inv_comb_count.rbegin();
        size_t max_count = it->first;
        pair<size_t,size_t> max_comb= it->second;
        if(max_count==0)break;
        rules.push_back({max_comb,vocab.size()});
        vocab.push_back(vocab[max_comb.first]+vocab[max_comb.second]);
        vocab_index[vocab.back()] = vocab.size()-(size_t)1;
        inv_comb_count.erase({max_count,max_comb});
        comb_count[max_comb] = 0;
        comb_count.erase(max_comb);
        

        for(const size_t& w: comb_words[max_comb]){
            list<size_t>& l = word_decom[w].first;
            auto it = l.begin();
            auto it2 = l.begin();
            it2++;
            while(it2!=l.end()){
                if(*it==max_comb.first&&*it2==max_comb.second){
                    if(it!=l.begin()){
                        it--;
                        it2--;
                        if(inv_comb_count.find({comb_count[{*it,*it2}],{*it,*it2}})!=inv_comb_count.end()){
                            inv_comb_count.erase({comb_count[{*it,*it2}],{*it,*it2}});
                        }
                        comb_count[{*it,*it2}]-=min(word_decom[w].second,comb_count[{*it,*it2}]);
                        if(comb_count[{*it,*it2}]>0)inv_comb_count.insert({comb_count[{*it,*it2}],{*it,*it2}});
                        if(inv_comb_count.find({comb_count[{*it,vocab.size()-1}],{*it,vocab.size()-1}})!=inv_comb_count.end())inv_comb_count.erase({comb_count[{*it,vocab.size()-1}],{*it,vocab.size()-1}});
                        comb_count[{*it,vocab.size()-1}]+=word_decom[w].second;
                        if(comb_count[{*it,vocab.size()-1}]>0)inv_comb_count.insert({comb_count[{*it,vocab.size()-1}],{*it,vocab.size()-1}});
                        comb_words[{*it,vocab.size()-1}].insert(w);
                        it++;
                        it2++;
                    }
                    it2++;
                    if(it2!=l.end()){
                        it++;

                        if(inv_comb_count.find({comb_count[{*it,*it2}],{*it,*it2}})!=inv_comb_count.end())inv_comb_count.erase({comb_count[{*it,*it2}],{*it,*it2}});
                        comb_count[{*it,*it2}]-=min(word_decom[w].second,comb_count[{*it,*it2}]);
                        if(comb_count[{*it,*it2}]>0)inv_comb_count.insert({comb_count[{*it,*it2}],{*it,*it2}});
                        if(inv_comb_count.find({comb_count[{vocab.size()-1,*it2}],{vocab.size()-1,*it2}})!=inv_comb_count.end())inv_comb_count.erase({comb_count[{vocab.size()-1,*it2}],{vocab.size()-1,*it2}});
                        comb_count[{vocab.size()-1,*it2}]+=word_decom[w].second;
                        if(comb_count[{vocab.size()-1,*it2}]>0)inv_comb_count.insert({comb_count[{vocab.size()-1,*it2}],{vocab.size()-1,*it2}});
                        comb_words[{vocab.size()-1,*it2}].insert(w);
                        it--;
                    }
                    it2--;
                    it = l.erase(it);
                    it = l.erase(it);
                    if(it!=l.begin()){
                        it = l.insert(it,vocab.size()-1);
                        it2 = it;
                        it--;
                    }
                    else{
                        l.push_front(vocab.size()-1);
                        it = l.begin();
                        it2 = l.begin();
                        it2++;
                        if(it2==l.end())break;
                    }
                }
                it++;
                it2++;
            }
        }
        comb_words.erase(max_comb);
    }
    inv_comb_count.clear();
    comb_count.clear();
    comb_words.clear();
}


void BPE::tokenise(const string& word){
    if(word_index.find(word)!=word_index.end()){
        return;
    }
    unordered_map<size_t,bool> used;
    word_index[word] = word_index.size();
    words.push_back(word);
    word_decom.push_back({list<size_t>(),1});
    list<size_t>& l = word_decom.back().first;
    for(const char& c: word){
        string ch = string(1,c);
        l.push_back(vocab_index[ch]);
        used[vocab_index[ch]] = true;
    }
    for(auto& rule: rules){
        if(!used[rule.first.first] || !used[rule.first.second])continue;
        auto it = l.begin();
        auto it2 = l.begin();
        it2++;
        while(it2!=l.end()){
            if(*it==rule.first.first&&*it2==rule.first.second){
                it = l.erase(it);
                it = l.erase(it);
                it = l.insert(it,rule.second);
                it2 = it;
                it--;
                used[rule.second] = true;
            }
            it++;
            it2++;
        }
    }
}