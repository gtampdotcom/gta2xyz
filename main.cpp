//gta2xyz by Sektor GTAMP.com gtamulti@gmail.com 2016
//This program fetches GTA2 player coordinates and writes them to the Epic GTA2 Map Editor camera
//Do whatever you want with this code.

#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <math.h>

using namespace std;

LPVOID framecount_ptr = (void*)0x5E8108;

int oldframecount;
int object_number;
int p1_addr;
int ped_addr;
int pedx,pedy,pedz;

float fx,fy,fz,fa;

string gta2modulename;

TCHAR gta2exe[MAX_PATH];
TCHAR editorexe[MAX_PATH];

LPVOID ped_ptr = (void*)0x5D85C0;
LPVOID pinfo_ptr = (void*)0x5EB4FC;
int camerax_addr = 0x554290;
int cameray_addr = 0x554294;
int cameraz_addr = 0x554298;
int cameraa_addr = 0x5542A4;

HANDLE findProcess(string processname);

float constrainAngle(float x)
{	
    x = fmod(x,360);
    if (x < 0)
        x += 360;
    return x;
}

int main( int argc, char* args[] )
{
	int pedcar;
	int playercount;
	bool quit = false;

	cout << "GTA2 XYZ v0.1 by Sektor GTAMP.com\n";
	cout << "-----------------------------------\n";
		
	HANDLE hGTA2 = NULL;
	HANDLE hEditor = NULL;
	
	hEditor = findProcess("MapEditor.exe");
	hGTA2 = findProcess("gta2.exe");
	
	if(hEditor == NULL || hGTA2 == NULL)
	{
		cout << "Can't find gta2.exe or MapEditor.exe process\n";
		quit = true;
	}
	
	while( !quit )
	{
		Sleep(5);
		
		int framecount,framecount_addr;
		ReadProcessMemory(hGTA2, framecount_ptr, &framecount_addr, 4, NULL);
		ReadProcessMemory(hGTA2, (LPVOID)framecount_addr, &framecount, 4, NULL);
			
		if(framecount!=oldframecount)
		{
			oldframecount = framecount;
			
			if (hEditor != NULL)
			{
				fy=256-fy; // the map editor y coordinate is the opposite to GTA2, x is the same
				WriteProcessMemory(hEditor, (LPVOID)camerax_addr, &fx, 4, NULL);
				WriteProcessMemory(hEditor, (LPVOID)cameray_addr, &fy, 4, NULL);
				WriteProcessMemory(hEditor, (LPVOID)cameraa_addr, &fa, 4, NULL);
				WriteProcessMemory(hEditor, (LPVOID)cameraz_addr, &fz, 4, NULL);
			}
			
			if (hGTA2 != NULL)
			{
				int angle[6];
				int pinfo_addr=0;
				int p1_ptr;
				int arrayofplayerptrs;
				int playercount_addr;
				int player_info;
				ReadProcessMemory(hGTA2, pinfo_ptr, &pinfo_addr, 4, NULL);	
				arrayofplayerptrs=pinfo_addr+0x4;
				ReadProcessMemory(hGTA2, (LPVOID)arrayofplayerptrs, &player_info, 4, NULL);
				p1_ptr=player_info+0xC4;
				ReadProcessMemory(hGTA2, (LPVOID)p1_ptr, &p1_addr, 4, NULL);
				
				playercount_addr=pinfo_addr+0x23;
				ReadProcessMemory(hGTA2, (LPVOID)playercount_addr, &playercount, 1, NULL);
				
				int ped_sprite_ptr;
				int ped_sprite_addr;
				int ped_angle_addr;
				int p1struct=0;
				
				object_number=0;
				
				if(playercount>6 or playercount<1)
				{
					playercount=1;
				}
				
				for(int i = 0; i <playercount; i++)
				{	
					p1struct=p1_addr;	
	      			p1struct+=object_number;
	      			int pedstruct;
	      			pedstruct=p1struct;
	      			//int ped_occupation_addr=0;
	      			//int ped_remap_addr=0;
	      			//ped_occupation_addr=p1struct+0x240;
	      			//ped_remap_addr=p1struct+0x244;
	      			ped_sprite_ptr=p1struct+360;
					
					//int pedremap_addr=pedstruct+0x244;
					//int pedremap=0;  
					//ReadProcessMemory(hGTA2, (LPVOID)pedremap_addr,&pedremap, 2, NULL);
				
					p1struct+=0x1AC;
	      			ReadProcessMemory(hGTA2, (LPVOID)p1struct,&pedx, 4, NULL);
	      			p1struct+=0x4;
	      			ReadProcessMemory(hGTA2, (LPVOID)p1struct,&pedy, 4, NULL);
	      			p1struct+=0x4;
	      			ReadProcessMemory(hGTA2, (LPVOID)p1struct,&pedz, 4, NULL);
	      			
	      			//int occupation=0;
	      			//int remap=0;	      			
					
	      			//ReadProcessMemory(hGTA2, (LPVOID)ped_occupation_addr,&occupation, 2, NULL);
	      			//ReadProcessMemory(hGTA2, (LPVOID)ped_remap_addr,&remap, 2, NULL);
					ReadProcessMemory(hGTA2, (LPVOID)ped_sprite_ptr,&ped_sprite_addr, 4, NULL);
					ped_sprite_addr+=128;
					ReadProcessMemory(hGTA2, (LPVOID)ped_sprite_addr,&ped_angle_addr, 4, NULL);
					
				 	object_number+=660;					
			
					int pedcar_addr;
					pedcar_addr=pedstruct+364;
					ReadProcessMemory(hGTA2, (LPVOID)pedcar_addr,&pedcar, 4, NULL);
					
					if(pedcar) //in vehicle
					{
						int pedcar_sprite;
						int pedcar_sprite_addr=pedcar+88;
						ReadProcessMemory(hGTA2, (LPVOID)pedcar_sprite_addr,&pedcar_sprite, 4, NULL);
						int pedcar_angle_addr=pedcar_sprite+88;
						int pedcar_angle;
						ReadProcessMemory(hGTA2, (LPVOID)pedcar_angle_addr,&angle[i], 2, NULL);
						angle[i] = angle[i]/4;
						angle[i] = 360-angle[i];
					}
					else
					{
						ReadProcessMemory(hGTA2, (LPVOID)ped_angle_addr,&angle[i], 2, NULL);
						angle[i] = angle[i]/4;
						angle[i] = 360-angle[i];
					}	
				}
				
				
				fx = (float) pedx/16384.0;
				fy = (float) pedy/16384.0;
				fz = (float) pedz/16384.0;
				fa = (float) (angle[0]+180);
				fa = (float) (constrainAngle(fa));
				
				//float fxoffset, fyoffset;
				
				//fxoffset = (float) 0.1*sin(M_PI*fa/180.0);
                //fyoffset = (float) -0.1*cos(M_PI*fa/180.0);
				
				//cout << "x offset: " << fxoffset << "\n";
				//cout << "y offset: " << fyoffset << "\n";
				cout << framecount << " x=" << fx << " y=" << fy << " z=" << fz << " a=" << fa << "\n";
				
				//fx+=fxoffset;
				//fy+=fyoffset;
				
			}
		}
	}
	
	CloseHandle(hEditor);
	CloseHandle(hGTA2);
	
	return 0;
}

HANDLE findProcess(string processname)
{			
	HANDLE hProcessSnap;	// will store a snapshot of all processes
	HANDLE hProcess = NULL;	// we will use this one for the exe process
	PROCESSENTRY32 pe32;	// stores basic info of a process, using this one to read the ProcessID from

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);	// make process snapshot

	pe32.dwSize = sizeof(PROCESSENTRY32);		// correct size

	Process32First(hProcessSnap, &pe32);	// read info about the first process into pe32
		
	do	// loop to find the process
	{
		if (strcmp(pe32.szExeFile, processname.c_str()) == 0)	// if process was found
		{
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);	// open it, assigning to the hProcess handle
			if(processname=="gta2.exe")
			{
				GetModuleFileNameEx(hProcess, NULL, gta2exe, MAX_PATH);
				//return hProcess;
			}
			
			if(processname=="MapEditor.exe")
			{
				GetModuleFileNameEx(hProcess, NULL, editorexe, MAX_PATH);
				
			}
		
  			break;	// break the loop
		}
		
	} while (Process32Next(hProcessSnap, &pe32));	// loop continued until Process32Next deliver NULL or its interrupted with the "break" above

	CloseHandle(hProcessSnap);	// close the handle	
	return hProcess;
}
