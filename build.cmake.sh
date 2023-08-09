#!/bin/bash

mkdir -p build
pushd build
if [[ "$OSTYPE" == "linux-gnu"* ]]; then

cmake .. 

elif [[ "$OSTYPE" == "darwin"* ]]; then

cmake .. 

elif [[ "$OSTYPE" == "msys" ]]; then

cmake .. --fresh -DOPENSSL_ROOT_DIR="F:/nostr_client_relay/ext/openssl-3.0.5"
cmake --build .

fi

sleep 3
cmake --build .

popd
exit