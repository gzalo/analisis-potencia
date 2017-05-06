#include <windows.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
using namespace std;
#include "rigol.h"

//Puerto Serie
HANDLE hSerial;

int serialInit(){
	int i;
	for(i=2;i<16;i++){
		char filename[32];
		sprintf(filename, "COM%d",i);
		hSerial = CreateFile(filename,GENERIC_READ | GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		
		if(hSerial!=INVALID_HANDLE_VALUE){
			break;
		}
	}
	if(i==15) return 1;
	
	DCB dcbSerialParams = {0};
	dcbSerialParams.DCBlength=sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams)) {
		cerr << "Error obteniendo parametros." << endl;
		return 1;
	}
	dcbSerialParams.BaudRate=CBR_115200;
	dcbSerialParams.ByteSize=8;
	dcbSerialParams.StopBits=ONESTOPBIT;
	dcbSerialParams.Parity=NOPARITY;
	if(!SetCommState(hSerial, &dcbSerialParams)){
		cerr << "Error estableciendo parametros." << endl;
		return 1;
	}
	
	COMMTIMEOUTS timeouts={0};
	timeouts.ReadIntervalTimeout=10000;
	timeouts.ReadTotalTimeoutConstant=10000;
	timeouts.ReadTotalTimeoutMultiplier=10000;
	if(!SetCommTimeouts(hSerial, &timeouts)){
		cerr << "Error estableciendo timeout." << endl;
		return 1;
	}
	
	return 0;
}

void print16Hexa(const uint8_t *bff){
	for(int i=0;i<16;i++)
		printf("%x ", bff[i]);
	printf("\n");
}

int main(int argc, char **args){
	srand(time(NULL));
	if(serialInit() != 0){
		return 1;
	}
	
	osc O;
	O.init();
	O.getID();
		
	O.setAcq();
	O.setTimescale();
	O.setTrigger();
	
	LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks
    double elapsedTime;
    QueryPerformanceFrequency(&frequency);

	uint8_t bufferRx[17], bufferTx[17];

	const uint8_t key[17] = "1234estaesunapru";
	FILE *fp = fopen("key.bin","wb");
	fwrite(key, 16, 1, fp);
	fclose(fp);
	
	FILE *fp_pt = fopen("pt.bin","ab");
	FILE *fp_tr = fopen("traces.bin","ab");
	
	for(int i=0;i<450;i++){
	
		DWORD dwBytesSent, dwBytesRecv;
		
		bufferTx[0] = 0;
		for(int j=1;j<17;j++)
			bufferTx[j] = rand()%256;	
		
		fwrite(bufferTx+1, 16, 1, fp_pt);
		
		O.run();
		Sleep(1000);
		WriteFile(hSerial, bufferTx, 17, &dwBytesSent, NULL);
		FlushFileBuffers(hSerial);
		O.waitNonStop();
		Sleep(100);
		O.getMemDepth();
						
						QueryPerformanceCounter(&t1);		
		fwrite(O.getData(), 1024*1024-16, 1, fp_tr);
		QueryPerformanceCounter(&t2);

    	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
		cout << elapsedTime << " ms (transfer).\n";
						
		#if 0			
		if(dwBytesSent != 17){
			cerr << "Escritura en puerto serie no se completo." << endl;
			return 1;
		}
		
		ReadFile(hSerial, bufferRx, 17, &dwBytesRecv, NULL);
		
		if(memcmp(bufferTx, bufferRx, 17) != 0){
			cerr << "error sincronismo" << endl;
			return 1;
		}
		
		if(dwBytesRecv != 17){
			cerr << "Lectura de puerto serie dio distinto a 17: " << dwBytesRecv << endl;
			return 1;
		}
		
		print16Hexa(bufferTx);
		print16Hexa(bufferRx);
		#endif
		/*Sleep(100);*/
		
	}
	O.end();			
	
	fclose(fp_pt);	
	fclose(fp_tr);	

	return 0;
}
	