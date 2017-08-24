#!/bin/sh

./bin/nginx -p . -s stop

make
rm -rf ./logs/*

./bin/nginx -p .
