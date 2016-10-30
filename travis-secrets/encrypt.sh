#!/bin/bash

set -e
set -u

mkdir -p encrypted

for F in `cd decrypted/ && ls | grep -v '_aeskey_'`
do
  echo Encrypting... $F
  openssl rand 245 > decrypted/$F._aeskey_
  openssl rsautl -encrypt -inkey public_key.pem -pubin -in decrypted/$F._aeskey_ -out encrypted/$F._aeskey_
  openssl enc -aes-256-cbc -in decrypted/$F -out encrypted/$F -pass file:decrypted/$F._aeskey_
  echo Done
done
