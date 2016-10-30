#!/bin/bash

docker run --rm -it -v `pwd`:/repo travis:default /bin/bash #/repo/tools/run_in_container.sh

