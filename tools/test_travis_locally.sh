#!/bin/bash

docker run --rm -v `pwd`:/repo travis:default /repo/tools/run_travis_in_container.sh

