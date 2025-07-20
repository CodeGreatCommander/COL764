# Learning to Rank with Regression Models

This project implements Learning to Rank (LTR) using three machine learning regression models: Support Vector Regressor (SVR), Gradient Boosting Regressor (GBR), and Multi-Layer Perceptron (MLP). Evaluation metrics focus on nDCG, a standard ranking metric. The models were tested on the TD2003 and TD2004 datasets.

## Problem Overview

The assignment explores machine learning models for LTR, focusing on training models to predict relevance scores for query-document pairs, with particular attention to computational efficiency and ranking performance.

## Objectives

- Implement and evaluate three regression models for ranking.
- Optimize performance through hyperparameter tuning.
- Conduct comparative analysis of model behaviors.

## Methodology

LTR is formulated as a pointwise ranking problem, with query-document pairs represented as feature vectors. The goal is to learn a function that maps these vectors to relevance scores closely aligned with ground truth.

### Models

1. **Support Vector Regressor (SVR)**
   - Objective: Minimize deviation from targets within an epsilon margin.
   - Uses RBF kernel to capture non-linear patterns.
   - Hyperparameters: C, epsilon, gamma.

2. **Gradient Boosting Regressor (GBR)**
   - Objective: Minimize Mean Squared Error through an additive tree-based approach.
   - Fits regression trees to gradient of loss function at each stage.
   - Hyperparameters: estimators, learning rate, max depth.

3. **Multi-Layer Perceptron (MLP)**
   - Objective: Learn non-linear transformations through neural network layers.
   - Uses ReLU activation and L2 regularization.
   - Hyperparameters: hidden layer sizes, activation, alpha, learning rate init.

## Hyperparameters

For each model, hyperparameters were selected to maximize performance on each dataset while ensuring computational efficiency.

- **SVR (TD2003):** Kernel: RBF, C: 0.0602, epsilon: 0.0114, gamma: 0.0107
- **GBR (TD2003 & TD2004):** Estimators: 154, Learning Rate: 0.0792, Max Depth: 6
- **MLP (TD2003):** Hidden Layers: (100,100), Activation: ReLU, Alpha: 0.040

## Time Optimization

Strategies were employed to improve runtime:

- **Hyperparameter Tuning**: Limiting SVR's C parameter and epsilon, constraining GBR tree depths, and reducing MLP layer sizes.
- **Dataset Subsampling**: Using smaller representative subsets during hyperparameter tuning to reduce training time without compromising quality.

## Bonus Task: PCA and Standardization

To improve speed and performance:

- **Principal Component Analysis (PCA)** reduced feature dimensionality to 100 components.
- **Standardization** normalized feature values for models sensitive to scale, improving convergence and model generalization.

## Running the Code
To run the different models, this it the way:
```bash
python3 <model>.py <dataset fold directory> <output-file> <config-file>
```

here are example:

```bash
python3 SVRLetor.py ../dataset/TD2004/Fold1 output-file td2004-svr.yaml
python3 GBDT.py ../dataset/TD2004/Fold1 output-file td2004-gbp.yaml
python3 MLP.py ../dataset/TD2003/Fold1 output-file td2003-mlp.yaml
```

## Conclusion

This project demonstrates the application of regression models in LTR and highlights techniques to balance computational efficiency with performance. The MLP model exhibited the best performance, while SVR was the fastest to compute.

