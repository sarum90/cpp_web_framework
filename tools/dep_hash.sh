#!/bin/bash

( (find third_party/ | sort -d -f); echo "__MANIFEST DONE__"; cat `find third_party/ -type f | sort -d -f`) | openssl dgst -sha256 -hex | cut -d' ' -f2
