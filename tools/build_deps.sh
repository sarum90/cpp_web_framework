#!/bin/bash

docker run --rm -v `pwd`:/repo travis:default /repo/tools/run_in_container.sh

