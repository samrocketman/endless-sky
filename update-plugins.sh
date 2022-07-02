#!/bin/bash
# Created by Sam Gleske
# Sat Jul  2 07:46:07 EDT 2022
# Pop!_OS 18.04 LTS
# Linux 5.4.0-113-generic x86_64
# GNU bash, version 4.4.20(1)-release (x86_64-pc-linux-gnu)
# git version 2.17.1

git submodule foreach /bin/bash -exc 'git checkout main || git checkout master'
git submodule foreach git pull
