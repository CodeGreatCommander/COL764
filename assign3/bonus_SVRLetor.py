from sklearn.datasets import load_svmlight_file
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA
from sklearn.svm import SVR
import os
import sys
import numpy as np
from collections import defaultdict
import yaml


np.random.seed(42)


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
        print("Usage: bonus_SVR.py [fold-directory] [output-file] [config-file]")
        sys.exit(1)

    folder_path = sys.argv[1]
    output_file = sys.argv[2]
    config_file = sys.argv[3]
    X, y, _ = load_svmlight_file(os.path.join(folder_path,'bonus.train.txt'), query_id=True)
    print("READ")
    sample_pca = np.random.choice(X.shape[0], 5000, replace=False)
    X_pca_sample = np.array(X[sample_pca].todense())
    X_pca_sample = StandardScaler().fit_transform(X_pca_sample)
    pca = PCA(n_components=100)
    pca.fit(X_pca_sample)

    sample_train = np.random.choice(X.shape[0], 30000, replace=False)
    X_train = np.array(X[sample_train].todense())
    y_train = y[sample_train]
    scaler = StandardScaler()
    X_train = scaler.fit_transform(X_train)
    X_train = pca.transform(X_train)

    print(X_train.shape)
    hyperparameters = read_hyperparameters(config_file)
    svr = SVR(
        kernel=hyperparameters['kernel'],
        C=hyperparameters['C'],
        epsilon=hyperparameters['epsilon'],
        gamma=hyperparameters['gamma']
    )
    svr.fit(X_train, y_train)

    X_train = []
    y_train = []
    X = []
    y = []

    print("Training Done")

    X_eval,y_eval, qid_eval = load_svmlight_file(os.path.join(folder_path,'bonus.test.txt'), query_id=True)
    X_eval = np.array(X_eval.todense())
    X_eval = scaler.transform(X_eval)
    X_eval = pca.transform(X_eval)

    y_pred = svr.predict(X_eval)
    doc_test = np.arange(1, len(y_pred)+1)

    write(output_file, y_pred, qid_eval, doc_test)

    