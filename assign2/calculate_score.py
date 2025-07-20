from math import log2

qrels_filename = '../dataset/qrels.tsv'
qrels = {}

#load qrels
with open(qrels_filename, 'r', encoding='UTF-8') as f:
    for line_number,line in enumerate(f):
        if(line_number == 0):
            continue
        sections = line.split('\t')
        if sections[0] not in qrels:
            qrels[sections[0]] = {}
        qrels[sections[0]][sections[1]] = int(sections[2])

def get_score(query_id, data, qrels):
    score = 0
    scores = []
    rel_scores = []
    for i in range(len(data)):
        if data[i] in qrels[query_id]:
            score += qrels[query_id][data[i]]/log2(i+2)
            rel_scores.append(qrels[query_id][data[i]])
        else:
            rel_scores.append(0)
        scores.append(score)
    rel_scores.sort(reverse=True)
    max_score = 0
    for i in range(len(rel_scores)):
        max_score += rel_scores[i]/log2(i+2)
        if(max_score  > 0):
            scores[i]/=max_score
    return scores

data = {}
with open("../output/output.txt", 'r', encoding='UTF-8') as f:
    for line in f:
        sections = line.split('\t')
        if sections[0] not in data:
            data[sections[0]] = []
        data[sections[0]].append(sections[2])

m1,m2,m3,m4 = 0,0,0,0
for dat in data:
    scores = get_score(dat, data[dat], qrels)
    print(scores[4],scores[9],scores[49])
    m1 += scores[4]
    m2 += scores[9]
    m3 += scores[49]
    m4 += scores[99]
print(m1/len(data),m2/len(data),m3/len(data),m4/len(data))