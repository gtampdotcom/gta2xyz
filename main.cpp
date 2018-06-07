//GTA2 player speed hack v0.01
//Do whatever you want with this code.

#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
//#include <math.h>

using namespace std;

class Ped {
public:
  int addr, x, y, z, a;
  //void set_values (int,int,int,int,int);
  //void set_values (int addr, int x, int y,int z, int a);
};
				
LPVOID framecount_ptr = (void*)0x5E8108;

int oldframecount;
int object_number;
int p1_addr;
int ped_addr;
int pedx,pedy,pedz,pedcount;
int pedcar_x_addr;
int pedcar_y_addr;

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

/*
float constrainAngle(float x)
{	
    x = fmod(x,360);
    if (x < 0)
        x += 360;
    return x;
}
*/

int main( int argc, char* args[] )
{
	int pedcar;
	int playercount;
	bool quit = false;

	cout << "GTA2 Player Speed Hack v0.02 by Sektor GTAMP.com\n";
	cout << "------------------------------------------------\n";
		
	HANDLE hGTA2 = NULL;

	hGTA2 = findProcess("gta2.exe");
	
	if(hGTA2 == NULL)
	{
		cout << "Can't find gta2.exe process\n";
		quit = true;
	}
	
	while( !quit )
	{
		Sleep(100);
		
		int framecount,framecount_addr;
		ReadProcessMemory(hGTA2, framecount_ptr, &framecount_addr, 4, NULL);
		ReadProcessMemory(hGTA2, (LPVOID)framecount_addr, &framecount, 4, NULL);
		
		Ped p[100];
			
		if(framecount!=oldframecount)
		{
			oldframecount = framecount;
			
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
				
				playercount=10;
				
				if(playercount>6 or playercount<1)
				{
					playercount=1;
				}
				
				for(int i = 0; i <1; i++)
				{	
					pedcount=i;
					p1struct=p1_addr;	
	      			p1struct+=object_number;
	      			int pedstruct;
	      			pedstruct=p1struct;
	      			int ped_occupation_addr=0;
	      			int ped_remap_addr=0;
	      			ped_occupation_addr=p1struct+0x240;
	      			ped_remap_addr=p1struct+0x244;
	      			ped_sprite_ptr=p1struct+360;
					
					
					//max run speed = p1struct + 496 or maybe 368
					//int pedremap_addr=pedstruct+0x244;
					int pedremap=0;  
					//WriteProcessMemory(hGTA2, (LPVOID)ped_remap_addr,&pedremap, 1, NULL);
					int ped_id_addr=p1struct+512;
					int pedid;
					ReadProcessMemory(hGTA2, (LPVOID)ped_id_addr,&pedid, 4, NULL);
					//cout << "ID: " << pedid << "\n"; 
			
					p1struct+=0x1AC;
	      			ReadProcessMemory(hGTA2, (LPVOID)p1struct,&p[pedcount].x, 4, NULL);
	      			
					//if(i>0)
					
					//x=1916928 y=2187264 z=32768 a=1080
					//p[0].x = 1916928;
					
					//WriteProcessMemory(hGTA2, (LPVOID)p1struct,&p[0].x, 4, NULL);
	      			
					  
					p1struct+=0x4;
	      			ReadProcessMemory(hGTA2, (LPVOID)p1struct,&p[pedcount].y, 4, NULL);
	      			//if(i>0)
	      			//WriteProcessMemory(hGTA2, (LPVOID)p1struct,&p[0].y, 4, NULL);
	      			//p[0].y=2187264;
	      			
	      			p1struct+=0x4;
	      			ReadProcessMemory(hGTA2, (LPVOID)p1struct,&p[pedcount].z, 4, NULL);
	      			//if(i>0)
	      			
	      			//p[0].z=65536;
					//  WriteProcessMemory(hGTA2, (LPVOID)p1struct,&p[0].z, 4, NULL);
	      			
	      			//cout << p[0].z << "\n";
	      			
	      			
	      			//p1struct+=0x1AC;
	      			//ReadProcessMemory(hGTA2, (LPVOID)p1struct,&pedx, 4, NULL);
	      			//p1struct+=0x4;
	      			//ReadProcessMemory(hGTA2, (LPVOID)p1struct,&pedy, 4, NULL);
	      			//p1struct+=0x4;
	      			//ReadProcessMemory(hGTA2, (LPVOID)p1struct,&pedz, 4, NULL);
	      			
	      			int occupation=0;
	      			//int remap=0;	      			
					int occ=0;
	      			ReadProcessMemory(hGTA2, (LPVOID)ped_occupation_addr,&occupation, 2, NULL);
	      			
	      			//if(i>0)
					//  WriteProcessMemory(hGTA2, (LPVOID)ped_occupation_addr,&occ, 2, NULL);
	      			//ReadProcessMemory(hGTA2, (LPVOID)ped_remap_addr,&remap, 2, NULL);
	      			
					ReadProcessMemory(hGTA2, (LPVOID)ped_sprite_ptr,&ped_sprite_addr, 4, NULL);
				
					//cout << "sprite addy: " << ped_sprite_addr << " \n";
					
					
					
					int ped_anim=0;
					int ped_anim_addr=0;
					
					ped_anim_addr=ped_sprite_addr+108;
					ReadProcessMemory(hGTA2, (LPVOID)ped_anim_addr,&ped_anim, 1, NULL);
					//if(ped_anim==20){
					//	ped_anim=20;
					//	WriteProcessMemory(hGTA2, (LPVOID)ped_anim_addr,&ped_anim, 1, NULL);
				
						//cout << "ped anim: " << ped_anim << " \n";	
					//}
					//ped_anim=3;
					
					
					
					//11 falling
					//20 drowning?
					
					
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
						
						pedcar_x_addr=pedcar_sprite+48;
						int carx;
						int cary;
						ReadProcessMemory(hGTA2, (LPVOID)pedcar_x_addr,&carx, 4, NULL);
					
						
						//781205 car y: 3013908
						pedcar_y_addr=pedcar_sprite+52;
						ReadProcessMemory(hGTA2, (LPVOID)pedcar_y_addr,&cary, 4, NULL);
					
						//cout << "pedcar: " << pedcar << " car x: " << carx << " car y: " << cary << "\n";
						int pedcar_angle;
						ReadProcessMemory(hGTA2, (LPVOID)pedcar_angle_addr,&p[pedcount].a, 2, NULL);
						//if(i>0)
						//WriteProcessMemory(hGTA2, (LPVOID)ped_angle_addr,&p[0].a, 2, NULL);
						angle[i] = angle[i]/4;
						angle[i] = 360-angle[i];
					}
					else //on foot
					{
						int ped_walk_addr=ped_sprite_addr+57;
						int ped_walk=12;
						int multiplayer_addr = 0x673E2C;
						int multiplayer;
						ReadProcessMemory(hGTA2, (LPVOID)multiplayer_addr,&multiplayer, 4, NULL);
						if(!multiplayer){
							WriteProcessMemory(hGTA2, (LPVOID)ped_walk_addr,&ped_walk, 4, NULL);
							//cout << "speed: " << ped_walk << "\n";
						}
						int carx;
						int cary;
						//carx=781205;
						
						//WriteProcessMemory(hGTA2, (LPVOID)pedcar_x_addr,&carx, 4, NULL);
						//cary=3013908;
						//WriteProcessMemory(hGTA2, (LPVOID)pedcar_y_addr,&cary, 4, NULL);
						
						//ReadProcessMemory(hGTA2, (LPVOID)ped_angle_addr,&angle[i], 2, NULL);
						ReadProcessMemory(hGTA2, (LPVOID)ped_angle_addr,&p[pedcount].a, 2, NULL);
						//if(i>0)
						//p[0].a=0;
						//WriteProcessMemory(hGTA2, (LPVOID)ped_angle_addr,&p[0].a, 2, NULL);
						//angle[i] = angle[i]/4;
						//angle[i] = 360-angle[i];
					}	
					
					//cout << "i:" << pedcount << "\n";
					//cout << pedcount << " x=" << p[pedcount].x << " y=" << p[pedcount].y << " z=" << p[pedcount].z << " a=" << p[pedcount].a << "\n";	
					//cout << pedcount << " x=" << p[0].x << " y=" << p[0].y << " z=" << p[0].z << " a=" << p[0].a << " 0 \n";
					
					
				}
				
				/*
				fx = (float) pedx/16384.0;
				fy = (float) pedy/16384.0;
				fz = (float) pedz/16384.0;
				fa = (float) (angle[0]+180);
				fa = (float) (constrainAngle(fa));
				
				fx-=0.5;
				fy+=0.5;
				*/
				
				
				
				//cout << framecount << " x=" << fx << " y=" << fy << " z=" << fz << " a=" << fa << "\n";		
			}
		}
		
		
	}
	
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
			}
		
  			break;	// break the loop
		}
		
	} while (Process32Next(hProcessSnap, &pe32));	// loop continued until Process32Next deliver NULL or its interrupted with the "break" above

	CloseHandle(hProcessSnap);	// close the handle	
	return hProcess;
}
