#!/usr/bin/sh

python3 nanopb/generator/nanopb_generator.py simple.proto
mv simple.pb.* ./src

protoc -I=. --python_out=. simple.proto
