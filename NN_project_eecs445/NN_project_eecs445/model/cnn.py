'''
EECS 445 - Introduction to Machine Learning
Fall 2019 - Project 2
CNN
    Constructs a pytorch model for a convolutional neural network
    Usage: from model.cnn import CNN
'''
import torch
import torch.nn as nn
import torch.nn.functional as F
from math import sqrt

class CNN(nn.Module):
    def __init__(self):
        """
        initialize the architecture of the Convolutional Neural Network
        input dimension 32*32*3
        3 convolutional layers
        3 fully connected layers
        5 output labels
        """
        super().__init__()

        # DONE: define each layer
        self.conv1 = nn.Conv2d(in_channels=3, out_channels=16, kernel_size=5, stride=2, padding=2)
        self.conv2 = nn.Conv2d(in_channels=16, out_channels=64, kernel_size=5, stride=2, padding=2)
        self.conv3 = nn.Conv2d(in_channels=64, out_channels=32, kernel_size=5, stride=2, padding=2)
        self.fc1 = nn.Linear(in_features=4*4*32, out_features=64)
        self.fc2 = nn.Linear(in_features=64, out_features=32)
        self.fc3 = nn.Linear(in_features=32, out_features=5)
        #

        self.init_weights()

    def init_weights(self):
        """
        initialize weights using normal distribution with different std, which depends on the layer
        """
        for conv in [self.conv1, self.conv2, self.conv3]:
            C_in = conv.weight.size(1)
            nn.init.normal_(conv.weight, 0.0, 1 / sqrt(5*5*C_in))
            nn.init.constant_(conv.bias, 0.0)

        # DONE: initialize the parameters for [self.fc1, self.fc2, self.fc3]
        for fully_connect in [self.fc1, self.fc2, self.fc3]:
            N_in = fully_connect.weight.size(1) #UNSURE: is this input dimension?
            nn.init.normal_(fully_connect.weight, 0.0, 1 / sqrt(N_in))
            nn.init.constant_(fully_connect.bias, 0.0)
        #

    def forward(self, x):
        """
        forward propagation from an input x to the output after applying Neural Network
        x - input Tensor, N*C*H*W 4-dimensional input Tensor data
        return z - torch.Tensor object with dimension 5
        """
        N, C, H, W = x.shape

        # DONE: forward pass
        # 3 convolutional layers
        x = F.relu(self.conv1(x))
        x = F.relu(self.conv2(x))
        x = F.relu(self.conv3(x))

        # 2 fully connected layers
        x = x.view(N, -1)
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        z = self.fc3(x)
        #

        return z
