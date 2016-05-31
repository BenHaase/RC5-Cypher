#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <string>

void rcset(unsigned char *);
unsigned int * rce(unsigned int, unsigned int, unsigned int *, int);
unsigned int * rcd(unsigned int, unsigned int, unsigned int *, int);
void packChars(unsigned int *, unsigned char *);
unsigned int rrot(unsigned int, unsigned int);
unsigned int lrot(unsigned int, unsigned int);
void printab(unsigned int *, int);
int ceil(int, int);
int max(int, int);

using namespace std;
int main(int argc, char* argv[])
{
	string t = argv[1];
	unsigned char k[8];
	int s = 0;
	for (int i = 0; i < 8; i++){
		if (t[i] > '9')
			k[i] = ((tolower(k[i]) - 'a') + 10) << 4;
		else
			k[i] = (t[s] - '0') << 4;
		s++;
		if (t[s] > '9')
			k[i] |= tolower(t[s]) - 'a' + 10;
		else
			k[i] |= t[s] - '0';
		s++;
	}
	rcset(k);
	return 0;
}

void rcset(unsigned char k[8])
{
	string ibuff;
	unsigned char buf[4]; //char array to split up input
	unsigned int aa = 0; //"A" lower 32bits of 64bit word
	unsigned int bb = 0; //"B" upper 32bits of 64bit word
	unsigned int p = 0xb7e15163; // P
	unsigned int q = 0x9E3779B9; // Q
	int r = 12; // number of rounds
	int b = 8; // size of key
	int u = 4; // number of bytes per word
	int t = 26; // (2 * r) + 2;
	int cc = ceil(b, u); // "c" 2
	unsigned int s[26]; //S array

	// S array prep
	for (int i = 0; i < t; i++)s[i] = 0;
	s[0] = p;
	for (int i = 1; i < t; i++){ s[i] = s[i - 1] + q; }

	// L array prep
	unsigned int l[2];
	l[0] = 0;
	l[cc - 1] = 0;
	for (int i = b - 1; i > -1; i--){
		l[i / u] = (l[i / u] << 8) + k[i];
	}

	// Combining L and S
	int z, j;
	unsigned int c, d;
	z = j = 0;
	c = d = 0;
	for (int h = 0; h < (3*t); h++){
		c = s[z] = lrot((s[z] + (c + d)), 3);
		z = (z + 1) % t;
		d = l[j] = lrot((l[j] + (c + d)), (c + d));
		j = (j + 1) % cc;
	}

	cout << "Enter data: ";
	getline(cin, ibuff);
	cout << endl;
    int esz = ceil(ibuff.length(), 4);
	unsigned int * ab;
	unsigned int * abio = new unsigned int[esz];
	int v = 0;
	for (int i = 0; i < esz; i++){ //do for whole input
		for (int h = 0; h < 4; h++){ //take 4 chars at a time
			if ((h + v) < ibuff.length()) buf[h] = ibuff[h + v];
			else buf[h] = 0x00;
		}
		v += 4;
		if ((v % 8) == 0){ //when aa and bb are filled
			packChars(&bb, buf);
			ab = rce(aa, bb, s, r);
			abio[2 * ((v / 8) - 1)] = ab[0];
			abio[2 * ((v / 8) - 1) + 1] = ab[1];
			aa = 0;
			bb = 0;
			delete[] ab;
		}
		else{ packChars(&aa, buf); }
	}
	cout << "Encryptions: " << endl;
	printab(abio, esz);
	for (int i = 0; i < esz; i+=2){
		ab = rcd(abio[i], abio[i + 1], s, r);
		abio[i] = ab[0];
		abio[i + 1] = ab[1];
		delete[] ab;
	}
	cout << endl;
	cout << "Decryptions:" << endl;
	printab(abio, esz);
	delete[] abio;
}

//encrypt function
unsigned int * rce(unsigned int a, unsigned int b, unsigned int * s, int r){
	a = a + s[0];
	b = b + s[1];
	for (int i = 1; i <= r; i++)
	{
		a = lrot((a ^ b), b) + s[(2 * i)];
		b = lrot((b ^ a), a) + s[(2 * i) + 1];
	}
	unsigned int * ab = new unsigned int[2];
	ab[0] = a;
	ab[1] = b;
	return ab;
}

//decrypt function
unsigned int * rcd(unsigned int a, unsigned int b, unsigned int * s, int r){
	for (int i = r; i > 0; i--){
		b = rrot((b - s[2 * i + 1]), a) ^ a;
		a = rrot((a - s[2 * i]), b) ^ b;
	}
	b = b - s[1];
	a = a - s[0];
	unsigned int * ab = new unsigned int[2];
	ab[0] = a;
	ab[1] = b;
	return ab;
}

//function to pack ints from char array
void packChars(unsigned int * a, unsigned char * b){
	*a = 0;
	for (int i = 3; i > -1; i--){
		(*a) = ((*a) << 8) + b[i];
	}
}

//left rotation function
unsigned int lrot(unsigned int a, unsigned int b){
	return ((((a) << (b&(32-1)))) | ((a) >> (32 - (b&(32-1)))));
}

//right rotation function
unsigned int rrot(unsigned int a, unsigned int b){
	return ((((a) >> (b&(32 - 1)))) | ((a) << (32 - (b&(32 - 1)))));
}

//print encryptions and decryptions
void printab(unsigned int * ab, int size){
	for (int i = 0; i < size; i += 2){
		cout << "Block " << (i / 2) << " = ";
		printf("%08x", ab[i]);
		cout << " ";
		printf("%08x", ab[i + 1]);
		cout << endl;
	}
}

//ceiling function
int ceil(int b, int u)
{
	if (b%u > 0) return (b / u) + 1;
	else return b / u;
}

//max function
int max(int t, int c){
	if (t > c) return t;
	else return c;
}
