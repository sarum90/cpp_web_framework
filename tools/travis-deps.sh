#!/bin/bash

docker pull quay.io/travisci/travis-ruby:latest
docker tag quay.io/travisci/travis-ruby:latest travis:default
