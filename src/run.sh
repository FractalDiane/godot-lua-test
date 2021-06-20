#!/bin/bash

cd build
cmake -DTarget="debug" .. -Wno-dev
make
cd ..

