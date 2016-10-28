#!/bin/bash

set -e
set -u

for F in $(cd decrypted/ && ls)
do
  echo Encrypting... $F
  openssl rsautl -encrypt -inkey public_key.pem -pubin -in decrypted/$F -out encrypted/$F
  echo Done
done
