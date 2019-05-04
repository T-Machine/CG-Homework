#pragma once
#ifndef _TOOL_
#define _TOOL_

void swap(int &a, int &b) {
	int temp = a;
	a = b;
	b = temp;
}

int max(int a, int b) {
	return a > b ? a : b;
}

int min(int a, int b) {
	return a < b ? a : b;
}


#endif
