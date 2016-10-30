#!/bin/bash

#docker run --rm -v `pwd`:/repo travis:default /repo/tools/run_in_container.sh

docker run --rm -it -v `pwd`:/repo travis:default /bin/bash

