/*
	Mini Biblioteca para conectarse con Osciloscopio Rigol DS1052E
	2016 - Gonzalo Ávila Alterach - http://gzalo.com
	Puede tener bugs :P
*/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <fcntl.h>
#include <stdint.h>
#include <visa.h>
#include <windows.h>
using namespace std;

void checkErr(unsigned int st){
	if(st == VI_SUCCESS){
		cout << "Success" << endl;
	}else if(st == VI_SUCCESS_TERM_CHAR){
		cout << "VI_SUCCESS_TERM_CHAR" << endl;
	}else if(st == VI_SUCCESS_MAX_CNT){
		cout << "VI_SUCCESS_MAX_CNT" << endl;
	}else if(st == VI_ERROR_INV_OBJECT){
		cout << "VI_ERROR_INV_OBJECT" << endl;
	}else if(st == VI_ERROR_NSUP_OPER){
		cout << "VI_ERROR_NSUP_OPER" << endl;
	}else if(st == VI_ERROR_RSRC_LOCKED){
		cout << "VI_ERROR_RSRC_LOCKED" << endl;
	}else if(st == VI_ERROR_TMO){
		cout << "VI_ERROR_TMO" << endl;
	}else if(st == VI_ERROR_RAW_WR_PROT_VIOL){
		cout << "VI_ERROR_RAW_WR_PROT_VIOL" << endl;
	}else if(st == VI_ERROR_RAW_RD_PROT_VIOL){
		cout << "VI_ERROR_RAW_RD_PROT_VIOL" << endl;
	}else if(st == VI_ERROR_OUTP_PROT_VIOL){
		cout << "VI_ERROR_OUTP_PROT_VIOL" << endl;
	}else if(st == VI_ERROR_BERR){
		cout << "VI_ERROR_BERR" << endl;
	}else if(st == VI_ERROR_INV_SETUP){
		cout << "VI_ERROR_INV_SETUP" << endl;
	}else if(st == VI_ERROR_NCIC){
		cout << "VI_ERROR_NCIC" << endl;
	}else if(st == VI_ERROR_NLISTENERS){
		cout << "VI_ERROR_NLISTENERS" << endl;
	}else if(st == VI_ERROR_ASRL_PARITY){
		cout << "VI_ERROR_ASRL_PARITY" << endl;
	}else if(st == VI_ERROR_ASRL_FRAMING){
		cout << "VI_ERROR_ASRL_FRAMING" << endl;
	}else if(st == VI_ERROR_ASRL_OVERRUN){
		cout << "VI_ERROR_ASRL_OVERRUN" << endl;
	}else if(st == VI_ERROR_IO){
		cout << "VI_ERROR_IO" << endl;
	}else if(st == VI_ERROR_CONN_LOST){
		cout << "VI_ERROR_CONN_LOST" << endl;
	}
}

void usleep(__int64 usec){ 
    HANDLE timer; 
    LARGE_INTEGER ft; 

    ft.QuadPart = -(10*usec); // Convert to 100 nanosecond interval, negative value indicates relative time

    timer = CreateWaitableTimer(NULL, TRUE, NULL); 
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0); 
    WaitForSingleObject(timer, INFINITE); 
    CloseHandle(timer); 
}

class osc{
	public:
		osc();
		~osc();
		
		int init();
		int end();
		int getID();
		int setTrigger();
		int run();
		int setAcq();		
		int getMemDepth();
		int getStatus();
		int setTimescale();		
		int waitNonStop();
		uint8_t *getData();

	private:
		int oscFD;
		char buffer[4096];
		string status;
		
		ViSession rmSession, scopeSession;
		ViFindList resourceList;
		ViUInt32 numResources;
		
		uint8_t dataBuff[1024*1024+32];
		
};
osc::osc(){
}
osc::~osc(){
}
int osc::init(){
	//oscFD=open("/dev/usbtmc0",O_RDWR);
	//if(oscFD==-1) return -1;

 /*SetPriorityClass (
                    GetCurrentProcess(),
                    HIGH_PRIORITY_CLASS
                     );	*/
	char usbResource[VI_FIND_BUFLEN];
	// Open session with the resource manager
	viOpenDefaultRM(&rmSession);
	// Find USB resources
	viFindRsrc(rmSession, (ViString)"USB?*", &resourceList, &numResources, usbResource);
	if(numResources == 0){
		cerr << "No USB Resources" << endl;
		return -1;
	}

	// Open session to the resource
	cout << usbResource << endl;
	
	//char nuevo[] = "USB0::0x1AB1::0x0588::DS1ED124413060::RAW";
	
	viOpen(rmSession, usbResource, VI_NULL, VI_NULL, &scopeSession);
	viSetAttribute(scopeSession, VI_ATTR_TMO_VALUE, 10000);
	//viFlush(scopeSession, VI_READ_BUF_DISCARD|VI_WRITE_BUF_DISCARD);
	//viSetBuf(scopeSession, 48, 1024*1024*2);
	return 0;
}

int osc::end(){
	//close(oscFD);
	
	/*ViUInt32 len;
	do{
		viRead(scopeSession, (ViBuf)buffer, 4096, &len);
	}while(len != 0);*/
	
	// Close the session to the resource
	viClose(scopeSession);

	// Close the session to the resource manager
	viClose(rmSession);
	return 0;
}

int osc::getStatus(){
	//int retval=write(oscFD,":TRIG:STAT?\n",12);
	int retval = viWrite(scopeSession, (ViBuf)":TRIG:STAT?\n", 12, VI_NULL);
	if(retval==-1) return -1;
	
	/*int recp=read(oscFD,buffer,4096);
	buffer[recp] = 0;
	
	cout << "Status: " << buffer << endl;
	
	status = buffer;*/
	
	// Read the response
	ViUInt32 recp;
	viRead(scopeSession, (ViBuf)buffer, 512, &recp);
	buffer[recp] = 0;
	
	cout << "Status: " << buffer << endl;
	status = buffer;
	return 0;
}

int osc::getID(){
	
	// Send a command to the resource
	viWrite(scopeSession, (ViBuf)"*IDN?\n", 6, VI_NULL);

	// Read the response
	ViUInt32 len;
	viRead(scopeSession, (ViBuf)buffer, 512, &len);

	buffer[len] = 0;
	// Print the ID string
	printf("Resource ID: %s\n", buffer);
	
	return 0;
}

int osc::setTrigger(){
	//int retval=write(oscFD,":TRIG:EDGE:SWE SING\n",20);
	int retval = viWrite(scopeSession, (ViBuf)":TRIG:EDGE:SWE SING\n", 20, VI_NULL);
	if(retval==-1) return -1;
	
	/*run();
	
	do{
		usleep(100*1000);
		getStatus();
	}while(status != "STOP");*/
	
	return 0;
}
int osc::waitNonStop(){

	do{
		usleep(100*100);
		getStatus();
	}while(status != "STOP");
	
	return 0;
}

int osc::run(){
	//int retval=write(oscFD,":RUN\n",5);
	int retval = viWrite(scopeSession, (ViBuf)":RUN\n", 5, VI_NULL);
	
	if(retval==-1) return -1;
	return 0;
}
int osc::setAcq(){
	int retval = viWrite(scopeSession, (ViBuf)":ACQ:TYPE NORMAL\n", 17, VI_NULL);
	if(retval==-1) return -1;
	retval = viWrite(scopeSession, (ViBuf)":ACQ:MODE RTI\n", 14, VI_NULL);
	if(retval==-1) return -1;
	retval = viWrite(scopeSession, (ViBuf)":ACQ:MEMD LONG\n", 15, VI_NULL);
	if(retval==-1) return -1;
	
	return 0;
}
int osc::setTimescale(){
	int retval;
	
	retval = viWrite(scopeSession, (ViBuf)":TIM:MODE MAIN\n", 15, VI_NULL);
	if(retval==-1) return -1;
	retval = viWrite(scopeSession, (ViBuf)":TIM:SCAL 5e-9\n", 15, VI_NULL);
	usleep(1000*1000);
	if(retval==-1) return -1;
	retval = viWrite(scopeSession, (ViBuf)":TIM:OFFS 0.001048576\n", 22, VI_NULL); //8.192e-6 en maxima con 16k,  0.000524288 con 1M (1ch) 
	if(retval==-1) return -1;
	
	/*retval=write(oscFD,":TIM:MODE MAIN\n",15);	
	if(retval==-1) return -1;
	
	retval=write(oscFD,":TIM:SCAL 0.002\n",16);	
	if(retval==-1) return -1;	
	usleep(1000*1000);
	retval=write(oscFD,":TIM:OFFS 0.04\n",15);	
	if(retval==-1) return -1;*/
	
//	usleep(500*1000);
	
	retval = viWrite(scopeSession, (ViBuf)":TIM:OFFS?\n", 11, VI_NULL);
	if(retval==-1) return -1;
	
	ViUInt32 len;
	viRead(scopeSession, (ViBuf)buffer, 512, &len);
	buffer[len] = 0;
	// Print the ID string
	printf("Trigger offset: %s\n", buffer);
	 
	 //--------------
	 
	retval = viWrite(scopeSession, (ViBuf)":TIM:SCAL?\n", 11, VI_NULL);
	if(retval==-1) return -1;
	
	viRead(scopeSession, (ViBuf)buffer, 512, &len);
	buffer[len] = 0;
	// Print the ID string
	printf("Timescale: %s\n", buffer);
	
	/*retval=write(oscFD,":TIM:OFFS?\n",11);	
	if(retval==-1) return -1;
	int recp=read(oscFD,buffer,4096);
	buffer[recp] = 0;
	cout << "Trigger Offset: " << buffer << endl;
	
	retval=write(oscFD,":TIM:SCAL?\n",11);	
	if(retval==-1) return -1;
	recp=read(oscFD,buffer,4096);
	buffer[recp] = 0;
	cout << "Timescale: " << buffer << endl;*/
	
	return 0;
}

int osc::getMemDepth(){
	int retval = viWrite(scopeSession, (ViBuf)":WAV:POIN:MODE RAW\n", 19, VI_NULL);
	if(retval==-1) return -1;
	/*int retval=write(oscFD,":WAV:POIN:MODE MAX\n",21);	
	if(retval==-1) return -1;*/
	
	/*retval=write(oscFD,":ACQ:SAMP? CHAN1\n",17);	
	if(retval==-1) return -1;*/
	
	retval = viWrite(scopeSession, (ViBuf)":ACQ:SAMP? CHAN1\n", 17, VI_NULL);
	if(retval==-1) return -1;
	
	ViUInt32 len;
	viRead(scopeSession, (ViBuf)buffer, 4096, &len);
	buffer[len] = 0;
	
	//int recp=read(oscFD,buffer,4096);
	//buffer[recp] = 0;
	stringstream ss(buffer);
	int sampleRate; ss >> sampleRate;	
	cout << "Sample rate: " << sampleRate << endl;
	//viFlush(scopeSession, VI_READ_BUF_DISCARD|VI_WRITE_BUF_DISCARD);
}
uint8_t *osc::getData(){
	int retval = viWrite(scopeSession, (ViBuf)":WAV:DATA? CHAN1\n", 17, VI_NULL); //:WAV:DATA? CHAN1
	if(retval==-1) return NULL;
	
	ViUInt32 len;
	int st;
    st = viRead(scopeSession, (ViBuf)dataBuff, 1024*1024+32, &len);
	checkErr(st);
	
	if(st==-1) {
		cerr << "Error reading" << endl;
		return NULL;
	}
	cout << "Read: " << len << endl;
	
	return dataBuff+32;
}
