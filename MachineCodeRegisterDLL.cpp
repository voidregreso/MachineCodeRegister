#include "stdafx.h"

#include <fstream>
#include <cstring>
#include <string>
using namespace std;

char *cpu_id()
{
    int s1, s2;
    char CPUID_1[1024], CPUID_2[1024];
    __asm
    {
        mov eax, 01h
        xor edx, edx
        cpuid
        mov s1, edx
        mov s2, eax
    }
   	sprintf(CPUID_1,"%08X%08X", s1, s2);
    __asm
    {
        mov eax, 03h
        xor ecx, ecx
        xor edx, edx
        cpuid
        mov s1, edx
        mov s2, ecx
    }
    sprintf(CPUID_2,"%08X%08X", s1, s2);
	strcat(CPUID_1, CPUID_2);
	return CPUID_1;
}

char *disk_id()
{
    try
    {
        char id[1024];
        char Name[MAX_PATH];
        DWORD serno;
        DWORD length;
        DWORD FileFlag;
        char FileName[MAX_PATH];
        BOOL Ret;
        Ret = GetVolumeInformation("C:\\", Name, MAX_PATH, &serno, &length, &FileFlag, FileName, MAX_PATH);
        if (Ret) sprintf(id, "%08X", serno);
        return id;
    }
    catch(...)
    {
        return "";
    }
}

extern "C" _declspec(dllexport) char *  __stdcall computer_id()
{
    char cid[10240];
    strcpy(cid,cpu_id());
    strcat(cid,disk_id());
    return cid;
}

string genKey(char mc[]){
	string tmp;
	for(int i=0;i<(int)strlen(mc);i++){
		tmp += mc[i];
		if((i+1)%5==0 && (i+1)!=strlen(mc)){
			tmp += "-";
		}
	}
	return tmp;
}

extern "C" _declspec(dllexport) char * __stdcall genSerial(char mc[]){
	int tmp = 0,g = 1,delta = 0;
	char fk[10240];
	string str = genKey(mc);
	sprintf(fk,"%s",str.c_str());
	for(int i = 0;i < (int)strlen(fk); i++){
		if((fk[i]>='A'&&fk[i]<='Z')){
			if(fk[i]<='P') delta = delta + ((((delta + 1) << 3) + strlen(fk)) ^ i) % 10;
			else {
				delta = delta + ((delta+2) << (i+4) ^ fk[i]) % 10;
				delta = delta - (delta ^ (int)strstr(fk,"a")) % 10;
			}
			if(delta >= 0) fk[i]+=delta;
			else fk[i]-=abs(delta);
			delta = 0;
			if(!(fk[i]>='A'&&fk[i]<='Z') && !(fk[i]>='0'&&fk[i]<='9') && !(fk[i]>='a'&&fk[i]<='z'))
				fk[i] = 'R';
		}
		if((fk[i]>='0'&&fk[i]<='9')){
			tmp = (int)fk[i]-48;
			if(tmp >= 2) tmp = (tmp ^ (tmp-2) << i) % 10;
			if(tmp == 1) tmp = (tmp << strlen(fk) / 9) % 7;
			if(tmp == 0){ 
				if(g>=10) g = 2;
				tmp = (((tmp + g) ^ fk[i]) << i * strlen(fk)) % 9;
				g++;
			}
			fk[i] = tmp + 48;
			if(!(fk[i]>='A'&&fk[i]<='Z') && !(fk[i]>='0'&&fk[i]<='9') && !(fk[i]>='a'&&fk[i]<='z'))
				fk[i] = '5';
		}
	}
	return fk;
}

extern "C" _declspec(dllexport) bool  __stdcall writeRegcodeToTextFile(char strfile[],char regcode[]){
	ofstream out(strfile);
	if(out.is_open()){
		out << regcode;
		out.close();
		return true;
	} else return false;
}
