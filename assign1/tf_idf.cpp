#include "utils.h"

using namespace std;

string endtoken = "<\\w>";

vector<string> headers;
bool delimiters[128]= {0};

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


struct term{
    string word;
    uint32_t start_byte, doc_freq;
};

struct document{
    string id;
    float comp_weight;
};

void update_tfidf(const term& t,const float qtf, float& total, ifstream& file, unordered_map<uint32_t, float>& score,const vector<document>& docs){
    file.clear();
    file.seekg(t.start_byte);
    float idf = log2(1.0+192509.0/t.doc_freq);
    total += idf*idf*qtf*qtf;
    auto read = [](std::ifstream& file) -> uint32_t {
        uint32_t num = 0;
        uint32_t c = 0;
        uint8_t byte = 0;
        do {
            file.read(reinterpret_cast<char*>(&byte), 1);
            num |= (byte & 127) << (7 * c);
            c++;
        } while (!(byte & 128));
        return num;
    };
    uint32_t sum = 0;
    for(uint32_t i=t.doc_freq;i>0;i--){
        uint32_t doc_id = read(file)+sum;
        uint32_t doc_freq = read(file);
        sum = doc_id;
        float tf = 1+log2(doc_freq);
        score[doc_id] += tf*idf*idf*qtf/docs[doc_id].comp_weight;
        if(doc_id>192509){
            cout<<"Error"<<endl;
            exit(1);
        }
    }
}


int main(int argc, char*argv[]){
    string queryfile = argv[1], resultfile = argv[2], indexfile = argv[3], dictfile = argv[4];

    cout<<queryfile<<endl<<resultfile<<endl<<indexfile<<endl<<dictfile<<endl;

    headers = {"title","description"};
    for(string& header: headers){
        header = "\""+header+"\":";
    }

    char delim[] = {' ','\n','\t',',','.',':',';','\"','\''};
    for(char c: delim){
        delimiters[c] = true;
    }

    ifstream file(dictfile);
    string option;
    file>>option;
    trie tri;

    if(!file.is_open()){
        cout<<"Error opening file"<<endl;
        return 1;
    }
    string line;
    vector<term> terms;
    vector<document> docs;
    bool state = false;
    for(size_t i =0;getline(file, line);i++){
        istringstream ss(line);
        string first;
        ss>>first;
        if(first=="__doc__"){
            state = true;
        }
        else if(state){
            document d;
            d.id = first;
            ss>>d.comp_weight;
            docs.push_back(d);
        }
        else{
            term t;
            t.word = first;
            ss>>t.start_byte>>t.doc_freq;
            terms.push_back(t);
            tri.insert(t.word,i);
        }
    }
    cout<<"Dictionary loaded"<<endl;
    file = ifstream(queryfile);
    ifstream pol(indexfile);
    if(!file.is_open()){
        cout<<"Error opening file"<<endl;
        return 1;
    }
    ofstream ofile(resultfile);
    for(size_t qid =1;getline(file, line);++qid){
        vector<string> ptokens = json_tokenise(line);
        unordered_map<uint32_t, size_t> query_count;
        unordered_map<uint32_t, float> score;
        if(option=="0")
        for(string& ptoken: ptokens){
            size_t pos = tri.search(ptoken);
            if(pos!=-1){
                query_count[pos]++;
            }
        }
        else if(option=="1")
        for(string& ptoken: ptokens){
            vector<size_t> tokens = tri.tokenise(ptoken,false);
            for(size_t& token: tokens){
                query_count[token]++;
            }
        }
        else if(option=="2")
        for(string& ptoken: ptokens){
            vector<size_t> tokens = tri.tokenise(ptoken,true);
            for(size_t& token: tokens){
                query_count[token]++;
            }
        }
        float total = 0;
        for(auto& [token, count]: query_count){
            float qtf = 1+log2(count);
            update_tfidf(terms[token], qtf, total, pol, score, docs);
        }

        vector<pair<uint32_t, float>> score_vector(score.begin(), score.end());
        sort(score_vector.begin(), score_vector.end(), [](const pair<uint32_t, float>& a, const pair<uint32_t, float>& b) {
            return b.second < a.second;
        });
        total = sqrt(total);
        for (size_t i = 0; i < min(score_vector.size(), size_t(100)); ++i) {
            ofile<<qid<<"\t"<<0<<"\t"<<docs[score_vector[i].first].id<<"\t"<<score_vector[i].second/total<<endl;
        }
    }
    ofile.close();
    cout<<"TF-IDF calculated"<<endl;

    return 0;

}