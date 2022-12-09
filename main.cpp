//gta2xyz by Sektor GTAMP.com gtamulti@gmail.com 2022
//This program fetches GTA2 player coordinates and writes them to the Epic GTA2 Map Editor camera
//Do whatever you want with this code.
//This code is horrible, it's just a quick experiment. I haven't even tried to clean it up.

#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <math.h>

using namespace std;

LPVOID framecount_ptr = (void*)0x5E8108;

const int gta2base_addr = 0x3F0000;
const int mapeditorbase_addr = 0x400000;

int object_ptr = gta2base_addr+0x1E8750;
int savedgame_first_object_ptr = gta2base_addr+0x275784;
int first_object_ptr = gta2base_addr+0x275788; // = ptr + c = ptr + 2c = first object

int oldframecount;
int object_number;
int p1_addr;
int ped_addr;
int pedx,pedy,pedz;

float fx,fy,fz,fa,ex,ey,ez;

string gta2modulename;

TCHAR gta2exe[MAX_PATH];
TCHAR editorexe[MAX_PATH];

LPVOID ped_ptr = (void*)0x5D85C0;
LPVOID pinfo_ptr = (void*)0x5EB4FC;

//MapEditor camera data
byte cameraData[32] = {
	0x70, 0x2E, 0x9D, 0x42, 0xA0, 0xFC, 0x93, 0x41, 0x00, 0x00, 0x80, 0x40,
	0xCF, 0xBD, 0x88, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD8, 0x42,
	0x00, 0x00, 0x34, 0x42, 0x00, 0x00, 0x00, 0x00
};


byte cameraData2[32] = {
	0x70, 0x2E, 0x9D, 0x42, 0xA0, 0xFC, 0x93, 0x41, 0x00, 0x00, 0x80, 0x40,
	0xCF, 0xBD, 0x88, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD8, 0x42,
	0x00, 0x00, 0x34, 0x42, 0x00, 0x00, 0x00, 0x00
};

int camerax_addr = 0x554290;
int cameray_addr = 0x554294;
int cameraz_addr = 0x554298;
int cameraa_addr = 0x5542A4;
int editor_script_preview = 0x5342DA;
int editor_yellow_cursor = 0x534627;
int editor_show_menu = 0x534712;
int editor_p1_xyz = mapeditorbase_addr + 0x14AACC;
int editor_p1_xyz_addr;
//int editor_p1_x;
int editor_p1_x_addr;
int editor_p1_y_addr;
int editor_p1_z_addr;
int editor_p1_angle_addr;
int editor_kf_xyz = mapeditorbase_addr + 0x154D10;
int editor_kf_xyz_addr;
int editor_kf_x_addr;
int editor_kf_y_addr;
int editor_kf_z_addr;
int editor_angle;

int firstrun;

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

	cout << "GTA2 XYZ v0.2 by Sektor GTAMP.com\n";
	cout << "-----------------------------------\n";
		
	HANDLE hGTA2 = NULL;
	HANDLE hEditor = NULL;
	
	
	/*
	if(hEditor == NULL || hGTA2 == NULL)
	{
		cout << "Can't find gta2.exe or MapEditor.exe process\n";
		quit = true;
	}
	*/
	cout << "Waiting for gta2.exe and MapEditor.exe\n";
	
	
	while( !quit )
	{
		Sleep(5);
		
		
		//while(hEditor == NULL || hGTA2 == NULL)
		{
			
		
			//quit = true;
		}
		
		
		hEditor = findProcess("MapEditor.exe");
		hGTA2 = findProcess("gta2.exe");
		
		int framecount,framecount_addr;
		ReadProcessMemory(hGTA2, framecount_ptr, &framecount_addr, 4, NULL);
		ReadProcessMemory(hGTA2, (LPVOID)framecount_addr, &framecount, 4, NULL);
			
		if(framecount!=oldframecount)
		{
			oldframecount = framecount;
			
			if (hEditor != NULL)
			{
				fy=256-fy; // the map editor y coordinate is the opposite to GTA2, x is the same
				
				if(firstrun==0)
				{
					firstrun=1;
					WriteProcessMemory(hEditor, (LPVOID)camerax_addr, cameraData, 32, NULL); // updating this too often causes some flicking
				}
					
				
				WriteProcessMemory(hEditor, (LPVOID)editor_script_preview, "\x00", 1, NULL); // disable script preview
				WriteProcessMemory(hEditor, (LPVOID)editor_yellow_cursor, "\x00", 1, NULL); // disable yellow cursor (switches from script mode to map mode)
				WriteProcessMemory(hEditor, (LPVOID)editor_show_menu, "\x00", 1, NULL); // disable menu and status bar
				
				
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
				
				// OBJECTS //
				// OBJECTS //
				// OBJECTS //
				// OBJECTS //
				// OBJECTS //
				// OBJECTS //
				
				int first_object_ptr2 = 0;
				int first_object_addr = 0;
				int first_object_value = 0;
				int savedgame_first_object_ptr2 = 0;
				int objectspace=100000;
				unsigned char tokenmemspace[objectspace];
				
				// first object location is different in saved games and new games
				// there must be a better way to get the address for the first object but using different ptrs for new or save seems to work
				ReadProcessMemory(hGTA2, (LPVOID)savedgame_first_object_ptr, &savedgame_first_object_ptr2, 4, NULL);
				savedgame_first_object_ptr2+=52;
				ReadProcessMemory(hGTA2, (LPVOID)savedgame_first_object_ptr2, &first_object_ptr2, 4, NULL);
				
				if(first_object_ptr2>0) // true if saved game
				{
					first_object_ptr2+=44;
				}
				else
				{	
					// it's probably a new game
					ReadProcessMemory(hGTA2, (LPVOID)first_object_ptr, &first_object_ptr2, 4, NULL);
					first_object_ptr2+=0xC;
				}
				
				ReadProcessMemory(hGTA2, (LPVOID)first_object_ptr2, &first_object_addr, 4, NULL);
				
				//cout << "first object addr: " << first_object_addr << "\n";
				
				
				int projectiles = 0;
				
				if(first_object_addr>0) //only read object memory if the game has started and isn't just on the menu
				{
					first_object_addr+=24;	
					ReadProcessMemory(hGTA2, (LPVOID)first_object_addr, &tokenmemspace[0], objectspace, NULL);
					int token_addr = 0;
					int tokenx = 0;
					int tokeny = 0;
					int object_z = 0;
					int tokenx_addr = 0;
					int tokeny_addr = 0;
					int object_z_addr = 0;
					
					float fTokenX;
					float fTokenY;
					float fTokenZ;
					
					int first_token_addr;
					
					int realfirstobject_addr=first_object_addr;
					
					int invulcount = 0;
					int ddcount = 0;
					int frcount = 0;
					int inviscount = 0;
					int tokencount = 0;
					int kfcount = 0;
					
					
					for(int i = 0; i <objectspace; i=i+44)
					{
						int object_num = 0xFFFF; 
						int object_addr;
						memcpy ( &object_num, &tokenmemspace[i], 2 ); //copy 2 bytes to get object number	
	
						//ignore object 122 (traffic lights)
						
						// Find invul by searching EA 00 00 00 00 00 00 00 01
						// You can find gta2 tokens/icons by searching for hex string 0A0100000000000002
						// 0A 01 00 00 00 00 00 00 02
						
						switch(object_num)
						{
							/*
							case 234:
							++invulcount;
							break;
						
							case 235:
							++ddcount;
							break;
							
							case 236:
							++frcount;
							break;
							
							case 239:
							++inviscount;
							break;
							
							case 266:
							++tokencount;
							break;
							
							case 286:
							++kfcount;
							break;
							*/
							
							case 128: case 138: case 182: case 183: case 254: case 265:
								++projectiles;
								realfirstobject_addr=first_object_addr+i-20;
								ReadProcessMemory(hGTA2, (LPVOID)realfirstobject_addr, &token_addr, 4, NULL);
								tokenx_addr=token_addr+20;
								tokeny_addr=token_addr+24;
								object_z_addr=token_addr+28;
								ReadProcessMemory(hGTA2, (LPVOID)tokenx_addr, &tokenx, 4, NULL);
								ReadProcessMemory(hGTA2, (LPVOID)tokeny_addr, &tokeny, 4, NULL);			
								ReadProcessMemory(hGTA2, (LPVOID)object_z_addr, &object_z, 4, NULL);			
								fTokenX =(float) tokenx/16384.0;
								fTokenY =(float) tokeny/16384.0;
								cout << "rocket x: " << fTokenX << " y: " << fTokenY << " z: " << object_z << "\n";
								//if(i==0)
								{
									//writing kf/rocket coords
									ReadProcessMemory(hEditor, (LPVOID)editor_kf_xyz,&editor_kf_xyz_addr, 4, NULL);
								
									editor_kf_x_addr = editor_kf_xyz_addr + 0xE9C;
									
									WriteProcessMemory(hEditor, (LPVOID)editor_kf_x_addr,&fTokenX, 4, NULL);
									
									editor_kf_y_addr = editor_kf_xyz_addr + 0xEA4;
									//fy=256-fy; // the map editor y coordinate is the opposite to GTA2, x is the same
									fTokenY=256-fTokenY; // the map editor y coordinate is the opposite to GTA2, x is the same
									//ey=256-fy; // the map editor y coordinate is the opposite to GTA2, x is the same
									
									WriteProcessMemory(hEditor, (LPVOID)editor_kf_y_addr,&fTokenY, 4, NULL);
									//WriteProcessMemory(hEditor, (LPVOID)editor_p1_y_addr,&ey, 4, NULL);
									
									
									editor_kf_z_addr = editor_kf_xyz_addr + 0xEAC;
									fTokenZ = fz-0.5;
									
									WriteProcessMemory(hEditor, (LPVOID)editor_kf_z_addr,&fTokenZ, 4, NULL);
									
								}
								break;
						}
					}
				}
				
				
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
						
						//cout << "angle raw: " << angle[i] << "\n";
						
						angle[i] = angle[i]/4;
						angle[i] = 360-angle[i];
					}
					
					
					fx = (float) pedx/16384.0;
					fy = (float) pedy/16384.0;
					fz = (float) pedz/16384.0;
					fa = (float) (angle[i]+180);
					fa = (float) (constrainAngle(fa));
					//fa = angle[i];
					
					//float fxoffset, fyoffset;
					
					//fxoffset = (float) 0.1*sin(M_PI*fa/180.0);
	                //fyoffset = (float) -0.1*cos(M_PI*fa/180.0);
					
					//cout << "x offset: " << fxoffset << "\n";
					//cout << "y offset: " << fyoffset << "\n";
				

								
							
							//printf("object %d %.02f %.02f %d\n",object_num,fTokenX,fTokenY,realfirstobject_addr);	
							//if(!gObjectTexture[object_num].getHeight())
							//{
							//	printf("object %d %.02f %.02f %d\n",object_num,fTokenX,fTokenY,tokenx_addr);	
						//		gObjectTexture[object_num].loadFromFile( "images\\arrow.png" );
						//	}
								
								
							//int object_angle;
							//ReadProcessMemory(hGTA2, (LPVOID)token_addr, &object_angle, 4, NULL);
							//if(p1angle>0)
						//	{
								//printf("object %d %d %d %d\n",token_addr,object_angle,p1angle,object_z);	
								//i=100000;
						//	}
						
					//	}	
					//}	
					
					if(i==0)
					{
					
					//writing player coords
					ReadProcessMemory(hEditor, (LPVOID)editor_p1_xyz,&editor_p1_xyz_addr, 4, NULL);
				
					editor_p1_x_addr = editor_p1_xyz_addr + 0x1C;
					WriteProcessMemory(hEditor, (LPVOID)editor_p1_x_addr,&fx, 4, NULL);
					
					editor_p1_y_addr = editor_p1_xyz_addr + 0x24;
					//fy=256-fy; // the map editor y coordinate is the opposite to GTA2, x is the same
					ey=256-fy; // the map editor y coordinate is the opposite to GTA2, x is the same
					
					WriteProcessMemory(hEditor, (LPVOID)editor_p1_y_addr,&ey, 4, NULL);
					//WriteProcessMemory(hEditor, (LPVOID)editor_p1_y_addr,&ey, 4, NULL);
					
					
					editor_p1_z_addr = editor_p1_xyz_addr + 0x2C;
					
					
					ez=fz-0.5;
					WriteProcessMemory(hEditor, (LPVOID)editor_p1_z_addr,&ez, 4, NULL);
					}
					
					//I give up on editor angle 
					//editor_p1_angle_addr = editor_p1_xyz_addr + 0x55;
					//ReadProcessMemory(hEditor, (LPVOID)editor_p1_angle_addr,&editor_angle, 3, NULL);
					//editor_angle = constrainAngle(editor_angle);
					//cout << "editor angle: " << editor_angle << "\n";
					
					
					
					/*
					//reading
					ReadProcessMemory(hEditor, (LPVOID)editor_p1_xyz,&editor_p1_xyz_addr, 4, NULL);
					editor_p1_x_addr = editor_p1_xyz_addr + 0x1C;
					
					ReadProcessMemory(hEditor, (LPVOID)editor_p1_x_addr,&ex, 4, NULL);
					
					editor_p1_y_addr = editor_p1_xyz_addr + 0x24;
					ReadProcessMemory(hEditor, (LPVOID)editor_p1_y_addr,&ey, 4, NULL);
					
					editor_p1_z_addr = editor_p1_xyz_addr + 0x2C;
					ReadProcessMemory(hEditor, (LPVOID)editor_p1_z_addr,&ez, 4, NULL);
					*/
					
					
					cout << framecount << " player= " << i << " x=" << fx << " y=" << fy << " z=" << fz << " a=" << fa << " editor x=" << ex << " editor y=" << ey << " editor z=" << ez << "\n";
				}	
				
				
				//if there are no projectiles found then hide the last one by moving it to z position 0
					if(projectiles==0)
					{
						editor_kf_z_addr = editor_kf_xyz_addr + 0xEAC;								
						WriteProcessMemory(hEditor, (LPVOID)editor_kf_z_addr,"\x00", 4, NULL);
					}	
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
