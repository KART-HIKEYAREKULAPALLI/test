#!/bin/bash

cd ~
git clone https://github.com/mz-automation/lib60870.git
cp /home/$(users)/Downloads/simple_client.c /home/$(users)/lib60870/lib60870
-C/examples/cs104_client/simple_client.c
cp /home/$(users)/Downloads/simple_server.c /home/$(users)/lib60870/lib60870
-C/examples/cs104_server/simple_server.c
cp -avr /home/$(users)/Downloads/mbedtls-2.6.0 /home/$(users)/lib60870/lib60870-C/dependencies
cd /home/$(users)/lib60870/lib60870-C
mkdir build
cd build
cmake ..
make
