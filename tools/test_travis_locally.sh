#!/bin/bash

docker run --rm -it -v `pwd`:/repo travis:default /repo/tools/run_travis_in_container.sh

