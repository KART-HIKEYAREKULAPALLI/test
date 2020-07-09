#include<stdio.h>


#include "si/iec60870-104_client.h"


int main(){

 
struct IEC60870_104Settings set={
    .ipaddr="127.0.0.1"
};



iec60870104Read(set,80);
 
 
return 0;
}

