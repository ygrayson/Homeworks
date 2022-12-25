"""
EECS 445 - Introduction to Machine Learning
Fall 2019 - Project 2
Posters Dataset
    Class wrapper for interfacing with the dataset of movie poster images
"""
import os
import numpy as np
import pandas as pd
import torch
from matplotlib import pyplot as plt
from imageio import imread
from PIL import Image
from torch.utils.data import Dataset, DataLoader
from utils import config

def get_train_val_test_loaders(num_classes):
    tr, va, te, _ = get_train_val_dataset(num_classes=num_classes)

    batch_size = config('cnn.batch_size')
    tr_loader = DataLoader(tr, batch_size=batch_size, shuffle=True)
    va_loader = DataLoader(va, batch_size=batch_size, shuffle=False)
    te_loader = DataLoader(te, batch_size=batch_size, shuffle=False)

    return tr_loader, va_loader, te_loader, tr.get_semantic_label

def get_train_val_dataset(num_classes=10):
    """
    get_train_val_dataset()
    get dataset from the file input
    return tr, va, te, standardizer
    tr, va, te are PosterDataset class that contains training, validation and testing dataset respectively
    """
    tr = PostersDataset('train', num_classes) #training data
    va = PostersDataset('val', num_classes) #validation data
    te = PostersDataset('test', num_classes) #testing data

    # Resize
    tr.X = resize(tr.X)
    va.X = resize(va.X)
    te.X = resize(te.X)

    # Standardize
    standardizer = ImageStandardizer() #standardizer sotres mean and variance info
    standardizer.fit(tr.X)
    tr.X = standardizer.transform(tr.X)
    va.X = standardizer.transform(va.X)
    te.X = standardizer.transform(te.X)

    # Transpose the dimensions from (N,H,W,C) to (N,C,H,W)
    tr.X = tr.X.transpose(0,3,1,2)
    va.X = va.X.transpose(0,3,1,2)
    te.X = te.X.transpose(0,3,1,2)

    return tr, va, te, standardizer

def resize(X):
    """
    Resizes the data partition X to the size specified in the config file.
    Uses bicubic interpolation for resizing.
    X - multi-dimensional numpy array, each X[i] is the ith poster input

    Returns:
        the resized images as a numpy array.
    """

    # DONE: Complete this function
    image_dim = config('image_dim')

    resized = np.empty((len(X), image_dim, image_dim, 3))
    for idx, img_array in enumerate(X):
        original_im = Image.fromarray(img_array)
        small_im = original_im.resize((image_dim, image_dim), resample=Image.BICUBIC)
        # augment into the new resized numpy array
        resized[idx] = np.array(small_im)

    #DEBUG
    #print("The shape of resized", resized.shape)
    return resized


class ImageStandardizer(object):
    """
    Channel-wise standardization for batch of images to mean 0 and variance 1.
    The mean and standard deviation parameters are computed in `fit(X)` and
    applied using `transform(X)`.

    X has shape (N, image_height, image_width, color_channel), always takein and not member variable
    """
    def __init__(self):
        super().__init__()
        self.image_mean = None
        self.image_std = None

    def fit(self, X):
        # DONE: Complete this function
        self.image_mean = X.mean(axis=(0,1,2))

        # find RGB std for all poster image
        variance = np.var(X, axis=(0,1,2))
        self.image_std = variance ** 0.5

        #DEBUG
        #print("The RGB means are", self.image_mean)
        #print("The RGB stds are", self.image_std)


    def transform(self, X):
        """
        input X - the whole dataset(training/validation/testing)
        transform the dataset according to self.mean and self.std. Normalize the whole dataset
        return normalized_X, same dimension as X but after normalization
        """
        normalized_X = np.zeros_like(X)
        # DONE: Complete this function
        # OPTIMIZE: this can be optimized using numpy array build-in functions
        for idx in range(len(X)):
            for i in range(len(X[0])):
                for j in range(len(X[0][0])):
                    for k in range(3):
                        # normalize each channel (RGB) with regarding to its mean and std
                        normalized_X[idx][i][j][k] = (X[idx][i][j][k] - self.image_mean[k]) / self.image_std[k]

        return normalized_X

class PostersDataset(Dataset):

    def __init__(self, partition, num_classes=10):
        """
        Reads in the necessary data from disk.
        """
        super().__init__()

        if partition not in ['train', 'val', 'test']:
            raise ValueError('Partition {} does not exist'.format(partition))

        np.random.seed(0)
        self.partition = partition
        self.num_classes = num_classes

        # Load in all the data we need from disk
        self.metadata = pd.read_csv(config('csv_file'))
        self.X, self.y = self._load_data()

        self.semantic_labels = dict(zip(
            self.metadata['numeric_label'],
            self.metadata['semantic_label']
        ))

    def __len__(self):
        return len(self.X)

    def __getitem__(self, idx):
        return torch.from_numpy(self.X[idx]).float(), torch.tensor(self.y[idx]).long()

    def _load_data(self):
        """
        Loads a single data partition from file.
        """
        print("loading %s..." % self.partition)

        if self.partition == 'test':
            if self.num_classes == 5:
                df = self.metadata[self.metadata.partition == self.partition]
            elif self.num_classes == 10:
                df = self.metadata[self.metadata.partition.isin([self.partition, ' '])]
            else:
                raise ValueError('Unsupported test partition: num_classes must be 5 or 10')
        else:
            df = self.metadata[
                (self.metadata.numeric_label < self.num_classes) &
                (self.metadata.partition == self.partition)
            ]

        X, y = [], []
        for i, row in df.iterrows():
            label = row['numeric_label']
            image = imread(os.path.join(config('image_path'), row['filename']), pilmode='RGB')
            X.append(image)
            y.append(row['numeric_label'])

        return np.array(X), np.array(y)

    def get_semantic_label(self, numeric_label):
        """
        Returns the string representation of the numeric class label (e.g.,
        the numberic label 1 maps to the semantic label 'miniature_poodle').
        """
        return self.semantic_labels[numeric_label]

if __name__ == '__main__':
    ## Future note: check scipy imread and imresize
    import warnings
    warnings.filterwarnings("ignore", category=DeprecationWarning)

    np.set_printoptions(precision=3)
    tr, va, te, standardizer = get_train_val_dataset()
    print("Train:\t", len(tr.X))
    print("Val:\t", len(va.X))
    print("Test:\t", len(te.X))
    print("Mean:", standardizer.image_mean)
    print("Std: ", standardizer.image_std)
