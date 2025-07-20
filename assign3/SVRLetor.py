import sys
import numpy as np
import yaml
from sklearn.svm import SVR
from sklearn.preprocessing import StandardScaler
from collections import defaultdict
import time

def parse_line(line):
    parts = line.split()
    label = int(parts[0])
    qid = int(parts[1].split(':')[1])
    doc_id = int(parts[-1])
    features = list(map(lambda x: float(x.split(':')[1]), parts[2:-3]))
    return label, qid, doc_id, features

def read_file(file):
    X, y, qid, doc = [], [], [], []
    with open(file, 'r') as f:
        for line in f:
            label, qi, doc_id, features = parse_line(line)
            y.append(label)
            qid.append(qi)
            doc.append(doc_id)
            X.append(features)
    return np.array(X), np.array(y), qid, doc


def write(output_file, y_pred, qid_test, doc_test):

    query_groups = defaultdict(list)
    for i in range(len(y_pred)):
        query_groups[qid_test[i]].append((i, y_pred[i], doc_test[i]))

    with open(output_file, 'w') as f:
        for qid in query_groups:
            sorted_predictions = sorted(query_groups[qid], key=lambda x: x[1], reverse=True)
            for rank, (i, score, doc_id) in enumerate(sorted_predictions):
                f.write(f"{qid} 0 {doc_id} {score}\n")

def read_hyperparameters(yaml_file):
    with open(yaml_file, 'r') as file:
        hyperparameters = yaml.safe_load(file)
    return hyperparameters


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: svr.py [fold-directory] [output-file] [config-file]")
        sys.exit(1)
    
    fold_directory = sys.argv[1]
    output_file = sys.argv[2]
    config_file = sys.argv[3]
    
    print(f"Fold Directory: {fold_directory}")
    print(f"Output File: {output_file}")

    X_train, y_train, qid_train, _ = read_file(f"{fold_directory}/trainingset.txt")
    X_test, _, qid_test, doc_test = read_file(f"{fold_directory}/testset.txt")
    
    scaler = StandardScaler()
    X_train = scaler.fit_transform(X_train)
    X_test = scaler.transform(X_test)

    hyperparameters = read_hyperparameters(config_file)
    svr = SVR(
        kernel=hyperparameters['kernel'],
        C=hyperparameters['C'],
        epsilon=hyperparameters['epsilon'],
        gamma=hyperparameters['gamma']
    )
    if X_train.shape[0] > 33000:
        sample_indices = np.random.choice(X_train.shape[0], 33000, replace=False)
        X_train_s = X_train[sample_indices]
        y_train_s = y_train[sample_indices]
    else:
        X_train_s = X_train
        y_train_s = y_train
    print(X_train.shape, y_train.shape)
    start = time.time()
    svr.fit(X_train_s, y_train_s)
    end = time.time()
    train_time = end - start
    print("Training Done")

    start = time.time()
    y_pred = svr.predict(X_test)
    write(output_file, y_pred, qid_test, doc_test)
    end = time.time()
    print("train time:",train_time,"\ntest time",end - start)