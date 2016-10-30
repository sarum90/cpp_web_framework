#!/bin/bash

set -e
set -u

if [ ! -f private_key/private_key.pem ]; then
    echo "No private key. Cannot decrypt secrets."
    exit -1
fi

mkdir -p decrypted/

for F in `cd encrypted/ && ls | grep -v '_aeskey_'`
do
  echo Decrypting... $F
  openssl rsautl -decrypt -inkey private_key/private_key.pem -in encrypted/$F._aeskey_ -out decrypted/$F._aeskey_
  openssl enc -d -aes-256-cbc -out decrypted/$F -in encrypted/$F -pass file:decrypted/$F._aeskey_
  echo Done
done

