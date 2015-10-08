#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "test.h"
#include <klee/klee.h>

double func_a(double n){
	return n+1;
}

double func_b(double n){
	return n+0.5;
}

double func_c(double n){
	return n;
}

int main(int argc, char *argv[]){
	double m;
	

	int i;


	m = func_a(m);
	m = func_a(m);
	m = func_b(m);
	m = func_b(m);
	m = func_c(m);
	m = func_c(m);

//	assert(m!=10);
	klee_make_symbolic(&i, sizeof(i), "i");
	klee_assert(i != 7);

	return 0;
}
