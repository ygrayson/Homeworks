'''
EECS 445 - Introduction to Machine Learning
Fall 2019 - Project 2
Train Autoencoder
    Trains an autoencoder to learn a sparse representation of image data
    Periodically outputs training information, and saves model checkpoints
    Usage: python train_autoencoder.py
'''
import torch
import numpy as np
import utils
import matplotlib.pyplot as plt
from dataset import get_train_val_test_loaders
from model.autoencoder import Autoencoder, NaiveRecon
from train_common import *
from utils import config

def _train_epoch(data_loader, model, criterion, optimizer):
    """
    Train the `model` for one epoch of data from `data_loader`
    Use `optimizer` to optimize the specified `criterion`
    """
    # DONE: complete the training step
    for i, (X, y) in enumerate(data_loader):
        # clear parameter gradients
        optimizer.zero_grad()

        # forward + backward + optimize
        output = model(X)
        #print("type of output[0] is", type(output[0]))
        #print("type of output[1] is", type(output[1]))
        #print("The size of output[0] is", output[0].shape)
        #print("The size of X is", X.shape)
        loss = criterion(output[1], X)
        loss.backward()
        optimizer.step()
        #
    

def _evaluate_epoch(axes, tr_loader, val_loader, model, criterion, epoch, stats):
    """
    Evaluates the `model` on the train and validation set.
    """
    running_loss = []
    for X, y in tr_loader:
        with torch.no_grad():
            _, recon = model(X)
            running_loss.append(criterion(recon, X))
    tr_loss = np.mean(running_loss)
    running_loss = []
    for X, y in val_loader:
        with torch.no_grad():
            _, recon = model(X)
            running_loss.append(criterion(recon, X))
    val_loss = np.mean(running_loss)
    stats.append([val_loss, tr_loss])
    utils.log_ae_training(epoch, stats)
    utils.update_ae_training_plot(axes, epoch, stats)

def main():
    # data loaders
    tr_loader, va_loader, te_loader, _ = get_train_val_test_loaders(
        num_classes=config('autoencoder.num_classes'))

    # Model
    model = Autoencoder(config('autoencoder.ae_repr_dim'))

    # DONE: define loss function, and optimizer
    criterion = torch.nn.MSELoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=config('autoencoder.learning_rate'))
    #

    # Attempts to restore the latest checkpoint if exists
    print('Loading autoencoder...')
    model, start_epoch, stats = restore_checkpoint(model,
        config('autoencoder.checkpoint'))

    fig, axes = utils.make_ae_training_plot()

    # Loop over the entire dataset multiple times
    for epoch in range(start_epoch, config('autoencoder.num_epochs')):
        # Evaluate model
        _evaluate_epoch(axes, tr_loader, va_loader, model, criterion, epoch+1,
            stats)

        # Train model
        _train_epoch(tr_loader, model, criterion, optimizer) #7200 datapoints in training set
        _train_epoch(te_loader, model, criterion, optimizer) #1000 datapoints in testing set, total 8200 training

        # Save model parameters
        save_checkpoint(model, epoch+1, config('autoencoder.checkpoint'), stats)

    print('Finished Training')

    # Save figure and keep plot open
    utils.save_ae_training_plot(fig)
    utils.hold_training_plot()

if __name__ == '__main__':
    main()
