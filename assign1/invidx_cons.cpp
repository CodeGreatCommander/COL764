#include "bpe.h"
#include "wordpiece.h"

using namespace std;

string endtoken = "<\\w>";
vector<string> headers;
bool delimiters[128]= {0};

unordered_map<string,uint32_t> word_count;
unordered_map<size_t, size_t> doc_last_used;
// vector<size_t> doc_tot;

uint32_t doc_count = 0;

void json_ise(const string& line, size_t doc_id){

    string word = "";
    bool state = false;

    for(size_t i=0, l = line.length(); i<l; i++){

        char c = tolower(line[i]);
        if(c == '\"'&&i!=0&&line[i-1]!='\\'&&state){
            if(word.length()!=0){
                word_count[word]++;
            }
            word.clear();
            state = false;
        }
        else if(c==' ' || (state&&delimiters[c])){
            if(word.length()==0)continue;
            if(state){
                word_count[word]++;
            }
            else{
                for(string& header: headers){
                    if(word==header){
                        state = true;
                        i++;
                        break;
                    }
                }
            }
            word.clear();
        }
        else if(c=='\\'){
            if(line[i+1]=='u')i+=5;
        }
        else{
            word+=c;
        }
    }
}


vector<string> json_tokenise(const string& line){
    vector<string> tokens;
    string word = "";
    bool state = false;

    for(size_t i=0, l = line.length(); i<l; i++){

        char c = tolower(line[i]);
        if(c == '\"'&&i!=0&&line[i-1]!='\\'&&state){
            tokens.push_back(word);
            word.clear();
            state = false;
        }
        else if(c==' ' || (state&&delimiters[c])){
            if(word.length()==0)continue;
            if(state){
                tokens.push_back(word);
            }
            else{
                for(string& header: headers){
                    if(word==header){
                        state = true;
                        i++;
                        break;
                    }
                }
            }
            word.clear();
        }
        else if(c=='\\'){
            if(line[i+1]=='u')i+=5;
        }
        else{
            word+=c;
        }
    }
    return tokens;
}


int main(int argc, char*argv[]){
    string file_path = argv[1], output_filename = argv[2], option = argv[3];
    file_path+="/cord19-trec_covid-docs";
    cout<<file_path<<endl;
    cout<<output_filename<<endl;
    cout<<option<<endl;

    headers = {"title", "abstract"};
    for(string& header: headers){
        header = "\""+header+"\":";
    }


    ifstream file(file_path);
    if(!file.is_open()){
        cout<<"Error opening file"<<endl;
        return 1;
    }

    string line;
    char delim[] = {' ','\n','\t',',','.',':',';','\"','\''};
    for(char c: delim){
        delimiters[c] = true;
    }
    
    for(size_t doc_id = 1;getline(file, line); doc_id++) {
        if(line!="")doc_count++;
    }
    file.clear();
    file.seekg(0);

    for(size_t doc_id = 1,start = doc_count - doc_count/2;getline(file, line); doc_id++) {
        if(doc_id>=start)
        json_ise(line,doc_id);
    }

    auto write = [](ofstream& file, size_t num,uint32_t &c){
        if(num==0){
            uint8_t t = 128;
            file.write(reinterpret_cast<const char*>(&t), sizeof(t));
            c++;
            return;
        }
        while (num > 127) {
            uint8_t t = num & 127;
            num >>= 7;
            file.write(reinterpret_cast<const char*>(&t), sizeof(t));
            c++;
        }
        if (num) {
            num |= 1 << 7;
            file.write(reinterpret_cast<const char*>(&num), sizeof(uint8_t));
            c++;
        }
    };
    headers.push_back("{\"doc_id\":");
    ofstream dict(output_filename+".dict"), inv_idx(output_filename+".idx", ios::binary);
    dict<<option<<endl;
    if(option=="0"){
        vector<string> vocab;
        unordered_map<string,size_t> vocab_index;
        for(auto& word: word_count){
            vocab_index[word.first] = vocab.size();
            vocab.push_back(word.first);
        }
        file.clear();
        file.seekg(0);
        vector<vector<pair<size_t,size_t>>> doc_tokens(vocab.size());
        vector<string> document_id;
        vector<size_t> doc_tot;
        for(size_t i =0;getline(file, line);i++) {
            vector<string> pretokens = json_tokenise(line);
            document_id.push_back(pretokens[0]);
            doc_tot.push_back(pretokens.size());
            bool flag = false;
            for(const string& token:pretokens){
                if(!flag){flag = true;continue;}
                if(vocab_index.find(token)!=vocab_index.end()){
                    size_t index = vocab_index[token];
                    if(doc_tokens[index].size()==0||doc_tokens[index].back().first!=i){
                        doc_tokens[index].push_back({i,1});
                    }
                    else{
                        doc_tokens[index].back().second++;
                    }
                }
            }
        }
        file.close();

        uint32_t c=0;
        vector<float> total_weight(doc_tot.size());
        for(size_t i = 0;i<doc_tokens.size();i++){
            size_t l = 0;
            uint32_t in_c = c;
            for(auto& doc: doc_tokens[i]){
                write(inv_idx,doc.first-l,c);
                write(inv_idx,doc.second,c);
                float tf = 1+log2(doc.second);
                float idf = log2(1.0+1.0*doc_count/doc_tokens[i].size());
                total_weight[doc.first]+=tf*tf*idf*idf;
                l = doc.first;
            }
            dict<<vocab[i]<<" "<<in_c<<" "<<doc_tokens[i].size()<<endl;
        }
        dict<<"__doc__"<<endl;
        for(size_t i = 0;i<document_id.size();i++){
            dict<<document_id[i]<<" "<<sqrt(total_weight[i])<<endl;
        }
    }
    else if(option=="1"){
        BPE bpe(word_count);
        bpe.train(20000);
        vector<vector<pair<size_t,size_t>>> doc_tokens(bpe.vocab.size());
        file.clear();
        file.seekg(0);
        trie* tri= new trie();
        for(size_t i=0;i<bpe.vocab.size();i++){
            tri->insert(bpe.vocab[i],i);
        }
        vector<size_t> doc_tot;
        cout<<"Tokenising"<<endl;

        vector<string> document_id;
        for(size_t i =0;getline(file, line);i++) {
            vector<string> pretokens = json_tokenise(line);
            if(i%1000==0)
            cout<<"\r"<<i<<" "<<flush;
            document_id.push_back(pretokens[0]);  
            doc_tot.push_back(0);
            bool flag = false;          
            for(const string& pretoken:pretokens){  
                if(!flag){flag = true;continue;}
                if(bpe.word_index.find(pretoken)==bpe.word_index.end()){
                    vector<size_t> tokens = tri->tokenise(pretoken,false);
                    doc_tot.back()+=tokens.size();
                    for(size_t token:tokens){
                        if(doc_tokens[token].size()==0){
                            doc_tokens[token].push_back({i,1});
                        }
                        else{
                            if(doc_tokens[token].back().first==i){
                                doc_tokens[token].back().second++;
                            }
                            else{
                                doc_tokens[token].push_back({i,1});
                            }
                        }
                    }
                }
                else{
                    doc_tot.back()+=bpe.word_decom[bpe.word_index[pretoken]].first.size();
                    for(size_t token:bpe.word_decom[bpe.word_index[pretoken]].first){
                        if(!doc_tokens[token].size()){
                            doc_tokens[token].push_back({i,1});
                        }
                        else{
                            if(doc_tokens[token].back().first==i){
                                doc_tokens[token].back().second++;
                            }
                            else{
                                doc_tokens[token].push_back({i,1});
                            }
                        }
                    }
                }
            }
        }
        delete tri;
        file.close();
        cout<<"Writing Postings list"<<endl;
        uint32_t c=0;
        vector<float> total_weight(document_id.size());
        for(size_t i = 0;i<doc_tokens.size();i++){
            size_t l = 0;
            uint32_t in_c = c;
            for(auto& doc: doc_tokens[i]){
                write(inv_idx,doc.first-l,c);
                write(inv_idx,doc.second,c);
                float tf = 1+log2(doc.second);
                float idf = log2(1.0+1.0*doc_count/doc_tokens[i].size());
                total_weight[doc.first]+=tf*tf*idf*idf;
                l = doc.first;
            }
            dict<<bpe.vocab[i]<<" "<<in_c<<" "<<doc_tokens[i].size()<<endl;
        }
        dict<<"__doc__"<<endl;
        for(size_t i = 0;i<document_id.size();i++){
            dict<<document_id[i]<<" "<<sqrt(total_weight[i])<<endl;
        }

    }
    else if(option=="2"){
        WordPiece wp(word_count);
        wp.train(20000);
        vector<vector<pair<size_t,size_t>>> doc_tokens(wp.vocab.size());
        file.clear();
        file.seekg(0);
        trie* tri= new trie();
        for(size_t i=0;i<wp.vocab.size();i++){
            tri->insert(wp.vocab[i],i);
        }
        vector<size_t> doc_tot;
        cout<<"Tokenising"<<endl;

        vector<string> document_id;
        for(size_t i =0;getline(file, line);i++) {
            vector<string> pretokens = json_tokenise(line);
            if(i%1000==0)
            cout<<"\r"<<i<<" "<<flush;
            document_id.push_back(pretokens[0]);  
            doc_tot.push_back(0);
            bool flag = false;          
            for(string& pretoken:pretokens){  
                if(!flag){flag = true;continue;}
                if(wp.word_index.find(pretoken)==wp.word_index.end()){
                    vector<size_t> tokens = tri->tokenise(pretoken, true);
                    doc_tot.back()+=tokens.size();
                    for(size_t token:tokens){
                        if(doc_tokens[token].size()==0){
                            doc_tokens[token].push_back({i,1});
                        }
                        else{
                            if(doc_tokens[token].back().first==i){
                                doc_tokens[token].back().second++;
                            }
                            else{
                                doc_tokens[token].push_back({i,1});
                            }
                        }
                    }
                }
                else{
                    doc_tot.back()+=wp.word_decom[wp.word_index[pretoken]].first.size();
                    for(size_t token:wp.word_decom[wp.word_index[pretoken]].first){
                        if(!doc_tokens[token].size()){
                            doc_tokens[token].push_back({i,1});
                        }
                        else{
                            if(doc_tokens[token].back().first==i){
                                doc_tokens[token].back().second++;
                            }
                            else{
                                doc_tokens[token].push_back({i,1});
                            }
                        }
                    }
                }
            }
        }
        delete tri;
        file.close();
        cout<<"Writing Postings list"<<endl;
        uint32_t c=0;
        vector<float> total_weight(document_id.size());
        for(size_t i = 0;i<doc_tokens.size();i++){
            size_t l = 0;
            uint32_t in_c = c;
            for(auto& doc: doc_tokens[i]){
                write(inv_idx,doc.first-l,c);
                write(inv_idx,doc.second,c);
                float tf = 1+log2(doc.second);
                float idf = log2(1.0+1.0*doc_count/doc_tokens[i].size());
                total_weight[doc.first]+=tf*tf*idf*idf;
                l = doc.first;
            }
            dict<<wp.vocab[i]<<" "<<in_c<<" "<<doc_tokens[i].size()<<endl;
        }
        dict<<"__doc__"<<endl;
        for(size_t i = 0;i<document_id.size();i++){
            dict<<document_id[i]<<" "<<sqrt(total_weight[i])<<endl;
        }
    }
    cout<<"Done"<<endl;
    return 0;
}