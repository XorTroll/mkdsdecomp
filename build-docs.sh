#!/bin/bash

rm -rf docs
mkdir -p docs/save-editor

cd tools/save-editor
make
cd ../..
cp -r tools/save-editor/out/* docs/
