#!/bin/bash
mkdir -p deps
cd deps || exit

git clone https://github.com/sciplot/sciplot.git
git clone https://github.com/mariusbancila/stduuid.git
git clone https://github.com/ToruNiina/toml11.git
