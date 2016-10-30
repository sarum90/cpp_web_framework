#!/bin/bash

( (find third_party/ | sort); echo "__MANIFEST DONE__"; cat `find third_party/ -type f | sort`) | openssl dgst -sha256 -hex | cut -d' ' -f2
