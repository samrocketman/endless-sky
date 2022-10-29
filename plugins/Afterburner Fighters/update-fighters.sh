#!/bin/bash
# Created by Sam Gleske
# Sat 29 Oct 2022 09:39:28 AM EDT
# Ubuntu 20.04.5 LTS
# Linux 5.15.0-50-generic x86_64
# GNU bash, version 5.0.17(1)-release (x86_64-pc-linux-gnu)
# grep (GNU grep) 3.4
# GNU Awk 5.0.1, API: 2.0 (GNU MPFR 4.0.2, GNU MP 6.2.0)
# dos2unix 7.4.0 (2017-10-10)

grep -rl 'category "\(Drone\|Fighter\)"' ../../data ../*/data | \
  tr '\n' '\0' | \
  xargs -0n1 -- awk '
$0 ~ /^ship/ {
  ship=$0
};
$1 == "category" && $2 ~ /Drone|Fighter/ {
    print ship"\n\tattributes\n\t\t\"rtb fuel level\" 25"
}' > data/rtb-fighters.txt

dos2unix data/rtb-fighters.txt
