#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdint>
#include <cmath>
#include <random>
using namespace std;

uint8_t key[16];
uint8_t bestkey[16];

const uint8_t sbox[256]={
0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16};

const uint8_t hamming[256] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};

float *array;
int cmp(const void *a, const void *b){
    int ia = *(int *)a;
    int ib = *(int *)b;
    return array[ia] < array[ib] ? -1 : array[ia] > array[ib];
}
uint8_t *pt;
double *dTraces;	
uint32_t numTraces, numPoints;

int loadFiles(){
	FILE *fp = fopen("key.bin","rb");
	fread(key, 16, 1, fp);
	fclose(fp);
	
	cout << "Clave original: ";
	for(int i=0;i<16;i++)
		printf("%02x ", key[i]);
	cout << endl;
	
	fp = fopen("traces_proc.bin","rb");
	fread((void*)&numTraces, 1, sizeof(uint32_t), fp);
	fread((void*)&numPoints, 1, sizeof(uint32_t), fp);
	
	cout << numTraces << " capturas, de " << numPoints << " puntos cada una." << endl;
	
	pt = new uint8_t[numTraces*16];
	dTraces = new double[numTraces*numPoints];
			
	fread(dTraces, numTraces*numPoints, sizeof(double), fp);
	fclose(fp);
	
	fp = fopen("pt.bin","rb");
	fread(pt, numTraces*16,1, fp);
	fclose(fp);
		
	return 0;
}
void printBestKey(){
	cout << "Clave mas probable: ";
	for(int i=0;i<16;i++)
		printf("%02x ", bestkey[i]);
	cout << endl;
}
double *sumCross;
double *sumTrace;
double *sumTrace2;
double sumHyp[256*16];
double sumHyp2[256*16];

double *diffTrace0;
double *diffTrace1;
int n0[256], n1[256];

void initArrays(){
	if(!sumCross)
		sumCross = new double[256*numPoints];
	
	if(!sumTrace)
		sumTrace = new double[numPoints];
	if(!sumTrace2)
		sumTrace2 = new double[numPoints];
	
	if(!diffTrace0)
		diffTrace0 = new double[256*numPoints];
	if(!diffTrace1)
		diffTrace1 = new double[256*numPoints];
	
	for(size_t i=0;i<numPoints;i++)
		sumTrace[i] = sumTrace2[i] = 0;
	for(size_t i=0;i<numPoints*256;i++)
		sumCross[i] = 0;
	for(size_t i=0;i<numPoints*256;i++)
		diffTrace0[i] = diffTrace1[i] = 0;
	
	for(int i=0;i<256*16;i++)
		sumHyp[i] = sumHyp2[i] = 0;
}

double correlation(int time, int key, int subkey){
	double num = numTraces * sumCross[key*numPoints+time] - sumHyp[key]*sumTrace[time];
	double den = sqrt((sumHyp[key]*sumHyp[key]-numTraces * sumHyp2[key])*(sumTrace[time]*sumTrace[time]-numTraces*sumTrace2[time]));
	
	return num/den;
}

void addTrace(uint8_t *pt, double *trace, int subkey){
	
	for(size_t time=0;time<numPoints;time++){
		sumTrace[time] += trace[time];
		sumTrace2[time] += trace[time]*trace[time];
	}
	
	//for(int subkey=0;subkey<16;subkey++)
	for(int guess=0;guess<256;guess++){
		double hyp = -hamming[sbox[pt[subkey]^ guess]];
		
		int bit0 = sbox[(pt[subkey] ^ guess)] & 4;
		
		sumHyp[guess] += hyp;
		sumHyp2[guess] += hyp*hyp;
		
		for(size_t time=0;time<numPoints;time++){
			sumCross[guess*numPoints+time] += trace[time]*hyp;

			/* DPA */
			if(bit0 == 0){
				diffTrace0[guess*numPoints+time] += trace[time];
			}else{
				diffTrace1[guess*numPoints+time] += trace[time];
			}
			
		}
		
		if(bit0 == 0){
			n0[guess]++;
		}else{
			n1[guess]++;
		}
	}
	

}

void dpa(int subkey){
	FILE *fp = fopen("dpa.bin","wb");
		
	for(int guess=0;guess<256;guess++){
		
		for(size_t j=32800;j<32800+1000;j++){
			double d0 = diffTrace0[guess*numPoints+j]/n0[guess];
			double d1 = diffTrace1[guess*numPoints+j]/n1[guess];
			double dtrace = d0-d1;
			fwrite((void*)&dtrace, 1, sizeof(double), fp);
		}
	
		
	}
	fclose(fp);
}

int cpa(int minTime, unsigned int maxTime, unsigned int subkey){
	
	float cpaout[256];
	int placeMax[256];

	for(int guess=0;guess<256;guess++){
		cpaout[guess] = 0;
		
		for(size_t j=minTime;j<maxTime;j++){
			float val = correlation(j,guess, subkey);
			if(val > cpaout[guess]){
				cpaout[guess] = val;
				placeMax[guess] = j;
			}
		}
	}
	
	float maximo = 0;
	int posmaximo = -1;
	for(int j=0;j<256;j++){
		if(cpaout[j] > maximo){
			maximo = cpaout[j];
			posmaximo = j;
		}
	}
	
	int index[256];
	for(int i=0;i<256;i++){
		index[i] = i;
	}
	array = cpaout;
	qsort(index, 256, sizeof(*index), cmp);
	
	int pge = 0;
	for(int j=0;j<256;j++){
		if(index[j] == key[subkey])
			pge = 255-j;
	}

	printf("Byte %2d de clave: %02x (corr %f) en instante %d (real: %02x)\n", subkey, posmaximo, maximo, placeMax[posmaximo], key[subkey]);
	//printf("KeyEnt: %f at time %d\nPGE:%d\n", cpaout[key[subkey]], placeMax[key[subkey]], pge);
	bestkey[subkey] = posmaximo;
	
	return pge;
}

int main(){
	loadFiles();
	
	int minTime = 0;
	int maxTime = numPoints;

	ofstream out("grafico.csv");
	
	for(int subkey=0;subkey<16;subkey++){
		initArrays();
		out << subkey << ",";
		for(size_t i=0;i<numTraces;i++){
			addTrace(pt+i*16, dTraces+i*numPoints, subkey);
			int pge = cpa(minTime, maxTime, subkey);
			out << pge << ",";
		}
		out << endl;
	}
	//dpa(0);
	
	printBestKey();
	
	return 0;
}
