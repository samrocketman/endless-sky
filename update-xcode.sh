#!/bin/bash
# Created by Sam Gleske
# Ubuntu 20.04.4 LTS
# Linux 5.13.0-52-generic x86_64
# GNU bash, version 5.0.17(1)-release (x86_64-pc-linux-gnu)
# Python 3.8.10
# DESCRIPTION
#     Update xcode project to include plugins.  This script was designed to be
#     run from Ubuntu and committed to the repository.
# Suggestion from Endless Sky Discord:
# https://discord.com/channels/251118043411775489/285540320823869441/999565417376927834

if [ ! "$(uname)" = Linux ]; then
  echo 'ERROR: this script is meant to be run from Linux.' >&2
  exit 1
fi
[ -d /tmp/.venv ] || python3 -m venv /tmp/.venv
source /tmp/.venv/bin/activate
type -P pbxproj || pip install wheel==0.37.1 docopt==0.6.2 openstep-parser==1.5.3 pbxproj==3.4.0

# Pull upstream XCode project.
#git fetch https://github.com/endless-sky/endless-sky.git master
#git checkout FETCH_HEAD EndlessSky.xcodeproj/project.pbxproj
git checkout HEAD EndlessSky.xcodeproj/project.pbxproj

# Add plugins
pbxproj folder -G --target EndlessSky EndlessSky.xcodeproj plugins

# Add plugins.txt
pbxproj file --target EndlessSky EndlessSky.xcodeproj plugins.txt
