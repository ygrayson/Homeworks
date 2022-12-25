# Homework 2: Pacman
Welcome to the Pacman portion of Homework 2! At the end of this portion of the assignment, you will get to see your search algorithms play out in front of you as Pacman searches for food.  Below are the installation instructions.  As always, please try to figure out problems you may have before bringing them to Piazza or office hours. If you're stuck, we're here to help.  You should have no problem getting the program working on CAEN.  However, because we understand that many students will prefer to do this project outside of the CAEN labs, we provide instructions below and the support for using an Ubuntu VM created in VirtualBox to develop for this project.  You are welcome to use another system to develop your code, but support will be offered only for CAEN and the Ubuntu VM we describe below.  If your program runs on one of these two systems, it will run in our grading environment.

One final note: developing on a VM can be time consuming.  We will be exposing an autograder soon, so **if you can get the code working on your local environment, feel free to develop there and check that your code runs on the autograder when it goes live**.  The autograder tests include the tests in `test.py`, so if you find that you are passing the tests in `test.py` and failing all the autograder tests, you will know that we are not able to run your code on our grading environment, and you should get it up and running on the VM to see what went wrong.

## Ubuntu VM Installation
This process will take up to 20 minutes
Follow these [instructions](https://www.dev2qa.com/how-to-install-ubuntu-on-virtualbox-mac/) to get an Ubuntu VM running through virtualbox on your computer (note: the Ubuntu image download is ~2 GB.  If you have internet constraints at home, best to do this on MWireless).  It is recommended to download Ubuntu 18.04.3.

You can "install ubuntu" or "try it".  Either will work for this assignment.  Installing Ubuntu is helpful if you plan on using this VM long term, or would like to make [copy paste](https://apple.stackexchange.com/questions/132233/copy-and-pasting-between-host-and-vm) available from your host system to your VM. Here are a few pointers if you would like to do this:
* Step 8: choose "Install Ubuntu"
* Step 10: choose "Minimal installation" - takes far less time, and for the purposes of this assignment you won't need the other programs included.
* Step 11: choose "Erase disk and install Ubuntu".  You will see a scary warning message saying it will delete all your files.  It is referring to the virtual drive you're  creating, not the host (your computer).    

## Downloading the repo and installing dependencies
```
sudo apt install git
git clone https://github.com/abwilf/HW2_Pacman.git
cd HW2_Pacman
sudo apt-get install python3-tk
sudo apt install python3-pip
```
Note: the staff has encountered some behavior where this last step fails.  If you see an error along the lines of `Unable to locate package python3-pip`, here is a [fix](https://askubuntu.com/questions/1061486/unable-to-locate-package-python-pip-when-trying-to-install-from-fresh-18-04-in).  Make sure to use `sudo apt install python3-pip` instead of `python-pip` as the fix recommends.

Then, 
```
pip3 install future
```
Note: if this last step fails, and you may need this [fix](https://askubuntu.com/questions/1061486/unable-to-locate-package-python-pip-when-trying-to-install-from-fresh-18-04-in).

To test that all the dependencies worked, run this command to play a game of pacman.  
```
python3 pacman.py
```

Note: you should be running some version of python 3.6.  Check this with `python3 --version`, and you should see `Python 3.6.x`.

Currently, a trivial search agent, GoWestAgent, has already been implemented for you. This search agent only moves west. In some mazes, it may help Pac-Man find food, but it may not in others. Try out these 2 commands!  If this works, you can get started coding and testing.
```
python3 pacman.py --layout testMaze --pacman GoWestAgent
python3 pacman.py --layout tinyMaze --pacman GoWestAgent
```

## Credit
All credit for this portion of the homework goes to [UC Berkeley’s CS188 course](http://ai.berkeley.edu).
