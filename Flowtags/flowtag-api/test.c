#include<stdio.h>
#include <arpa/inet.h>

int main(){
  printf("%d\n",htonl(inet_addr("10.0.0.1")));
}
