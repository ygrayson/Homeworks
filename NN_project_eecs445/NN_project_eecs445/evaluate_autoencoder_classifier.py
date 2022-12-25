"""
EECS 445 - Introduction to Machine Learning
Fall 2019 - Project 2
Evaluate Autoencoder Classifier
    Runs classification on an autoencoder classifier to get the per-class performance on the
    validation data.
    Usage: python evaluate_autoencoder_classifier.py
"""
import torch
import numpy as np
import utils
from dataset import get_train_val_test_loaders
from model.autoencoder import AutoencoderClassifier
from train_common import *
from utils import config

def get_data_by_label(dataset):
    """
    extract from dataset by the labels
    return data, partitioned by each label
    """

    data = []
    for i, (X, y) in enumerate(dataset):
        # c as different class of labels
        for c in range(config('autoencoder.classifier.num_classes')):
            # X dataset of class c
            batch = X[(y == c).nonzero().squeeze(1)]
            if len(data) <= c:
                data.append(batch)
            else:
                data[c] = torch.cat((data[c], batch))

    return data

def evaluate_autoencoder_classifier(dataset, get_semantic_label, model, criterion):
    """
    Runs inference on an autoencoder classifier model to evaluate the mse loss on the
    validation sets. Reports per-class performance to terminal.
    """
    num_classes = config('autoencoder.classifier.num_classes')
    batch_size = config('autoencoder.classifier.batch_size')
    performance = np.zeros(num_classes)
    overall_correct = 0
    overall_sample_num = 0
    for c in range(num_classes):
        # get the specific class of dataset and its prediction
        X = dataset[c]
        y_predict = model(X)
        y_true = torch.full_like(y_predict, c)

        # per class performance, c is the correct label
        correct = (y_predict == y_true).sum().item()
        batch_accracy = correct / len(y_true)
        performance[c] = batch_accracy

        overall_sample_num = overall_sample_num + X.shape[0]
        overall_correct = overall_correct + correct

    #overall performance
    overall_performance = overall_correct / overall_sample_num
    print('Overall performance: {} mean squared error'
            .format(overall_performance))
    for c, p in enumerate(performance):
        print('Class {}: {} mean squared error'
            .format(get_semantic_label(c), p))


def main():
    # data loaders
    _, va_loader, _, get_semantic_label = get_train_val_test_loaders(num_classes=config('autoencoder.classifier.num_classes'))
    #dataset and label are partitioned according to label class
    dataset = get_data_by_label(va_loader)

    model = AutoencoderClassifier(config('autoencoder.ae_repr_dim'), config('autoencoder.classifier.num_classes'))
    criterion = torch.nn.MSELoss()

    # Attempts to restore the latest checkpoint if exists
    print('Loading Autoencoder Classifier...')
    model, start_epoch, _ = restore_checkpoint(model,
        config('autoencoder.classifier.checkpoint'))

    # Evaluate model
    evaluate_autoencoder_classifier(dataset, get_semantic_label, model, criterion)


if __name__ == '__main__':
    main()
