"""
EECS 445 - Introduction to Machine Learning
Fall 2019 - Project 2
Autoencoder
    Constructs a pytorch model for a neural autoencoder
    Autoencoder usage: from model.autoencoder import Autoencoder
    Autoencoder classifier usage:
        from model.autoencoder import AutoencoderClassifier
    Naive method usage: from model.autoencoder import NaiveRecon
"""
import torch
import torch.nn as nn
import torch.nn.functional as F
from math import sqrt

class Autoencoder(nn.Module):
    def __init__(self, repr_dim):
        super().__init__()
        self.repr_dim = repr_dim

        ## DONE: define each layer
        self.pool = nn.AvgPool2d(kernel_size=2, stride=2)
        self.fc1_encode = nn.Linear(in_features=768, out_features=128)
        self.fc2_encode = nn.Linear(in_features=128, out_features=self.repr_dim)
        self.fc3_decode = nn.Linear(in_features=self.repr_dim, out_features=20736)
        ##

        self.deconv = nn.ConvTranspose2d(repr_dim, 3, 5, stride=2, padding=2)
        self.init_weights()

    def init_weights(self):
        # DONE: initialize the parameters for
        #       [self.fc1, self.fc2, self.fc3, self.deconv]
        # initialize fully connected layers
        for layers in [self.fc1, self.fc2, self.fc3]:
            N_in = layers.weight.size(1)
            nn.init.normal_(layers.weight, 0.0, 0.1 / sqrt(N_in))
            nn.init.constant_(layers.bias, 0.01)

        # initialize deconvolutional layer
        nn.init.normal_(self.deconv.weight, 0.0, 0.01)
        nn.init.constant_(self.deconv.weight, 0.0)
        

    def forward(self, x):
        """
        forward propagation function
        returns the learned data representation encoded, and final output decoded
        """
        encoded = self.encoder(x)
        decoded = self.decoder(encoded)

        #baseline error evaluation (assuming all reconstrction simply returns 0)
        #baseline = torch.zeros_like(decoded)
        return encoded, decoded

    def encoder(self, x):
        # DONE: encoder
        N, C, H, W = x.shape
        
        x = self.pool(x) #pooling layer
        x = x.view(N, -1) #resize, -1 is inferring the size, which will flat out the input for fully connected layer

        x = F.elu(self.fc1_encode(x)) #fully connected layer 1
        encoded = F.elu(self.fc2_encode(x)) #fully connected layer 2
        #
        return encoded

    def decoder(self, encoded):
        # DONE: decoder
        z = F.elu(self.fc3_decode(encoded)) #fully connected layer 3

        #DEBUG
        #print("The shape after layer 3 is", z.shape)

        decoded = self._grow_and_crop(z)
        decoded = _normalize(decoded)
        return decoded

    def _grow_and_crop(self, x, input_width=18, crop_size=32, scale=2):
        """
        this function execute both the deconvolutional and crop layer in feed forward process
        """
        decoded = x.view(-1, self.repr_dim, input_width, input_width)
        decoded = self.deconv(decoded)

        magnified_length = input_width * scale
        crop_offset = (magnified_length - crop_size) // 2
        L, R = crop_offset, (magnified_length-crop_offset)
        decoded = decoded[:, :, L:R, L:R]
        return decoded

class AutoencoderClassifier(nn.Module):
    # skip connections
    def __init__(self, repr_dim, d_out, n_neurons=32):
        super().__init__()
        self.repr_dim = repr_dim

        # DONE: define each layer
        self.pool = nn.AvgPool2d(kernel_size=2, stride=2)
        self.fc1_encode = nn.Linear(in_features=768, out_features=128)
        self.fc2_encode = nn.Linear(in_features=128, out_features=repr_dim)
        #

        self.fc_1 = nn.Linear(repr_dim, n_neurons)
        self.fc_2 = nn.Linear(n_neurons, n_neurons)
        self.fc_3 = nn.Linear(n_neurons, n_neurons)
        self.fc_last = nn.Linear(n_neurons, d_out)

    def forward(self, x):
        """
        forward propagate first through the encoder for a representation
        then propagate the fully connected layers for the classification
        """
        # learn representation by encoder
        encoded = self.encoder(x)

        # learn classifier by fully connected layers
        z1 = F.elu(self.fc_1(encoded))
        z2 = F.elu(self.fc_2(z1))
        z3 = F.elu(self.fc_3(z2))
        z = F.elu(self.fc_last(z1 + z3))
        return z

    def encoder(self, x):
        # DONE: encoder
        N, C, H, W = x.shape

        x = self.pool(x) #pooling layer
        x = x.view(N, -1) #resize, -1 is inferring the size, which will flat out the input for fully connected layer

        x = F.elu(self.fc1_encode(x)) #fully connected layer 1
        encoded = F.elu(self.fc2_encode(x)) #fully connected layer 2
        #

        return encoded

class NaiveRecon(nn.Module):
    def __init__(self, scale):
        super().__init__()
        self.scale = scale

    def forward(self, x):
        compressed = F.avg_pool2d(x, self.scale, stride=self.scale)
        grow = F.interpolate(compressed, size=(32, 32),
            mode='bilinear', align_corners=False)
        reconstructed = _normalize(grow)
        return compressed, reconstructed

def _normalize(x):
    """
    Per-image channelwise normalization
    """
    mean = x.mean(2, True).mean(3, True).mean(0, True)
    std = torch.sqrt((x - mean).pow(2).mean(2, True).mean(3, True).mean(0, True))
    z = (x - mean) / std
    return z
