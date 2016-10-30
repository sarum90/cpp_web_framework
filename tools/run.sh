#!/bin/bash

docker run -v `pwd`:/repo travis:default /repo/tools/run_in_container.sh

