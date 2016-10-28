#!/bin/bash

set -e
set -u
set -x

if [ ! -f private_key/private_key.pem ]; then
    echo "No private key. Cannot decrypt secrets."
    exit -1
fi

mkdir -p decrypted/

for F in `cd encrypted/ && ls`
do
  echo Decrypting... $F
  openssl rsautl -decrypt -inkey private_key/private_key.pem -in encrypted/$F -out decrypted/$F
  echo Done
done

