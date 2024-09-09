#!/bin/bash

NAME="StomaImagePack"

sudo rm -rf /usr/include/$NAME
sudo mkdir /usr/include/$NAME
sudo cp Include/* /usr/include/$NAME/
