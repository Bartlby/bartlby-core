////////////////////////////////////////////////////////////////////// 
// NT Service Stub Code (For XYROOT )
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include <winbase.h>
#include <winsvc.h>

SERVICE_STATUS          serviceStatus; 
SERVICE_STATUS_HANDLE   hServiceStatusHandle; 

////////////////////////////////////////////////////////////////////// 
//
// Configuration Data and Tables
//

BOOL KillService(char* pName) 
{ 
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		printf("OpenSCManager failed, error code = %X\n", GetLastError());
	}
	else
	{
		SC_HANDLE schService = OpenService( schSCManager, pName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			printf("OpenService failed, error code = %X\n", GetLastError()); 
		}
		else
		{
			SERVICE_STATUS status;
			if(ControlService(schService,SERVICE_CONTROL_STOP,&status))
			{
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager); 
				return TRUE;
			}
			else
			{
				printf("ControlService failed, error code = %X\n", GetLastError()); 
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager); 
	}
	return FALSE;
}

BOOL RunService(char* pName, int nArg, char** pArg) 
{ 
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		printf("OpenSCManager failed, error code = %X\n", GetLastError());
	}
	else
	{
		SC_HANDLE schService = OpenService( schSCManager, pName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			printf("OpenService failed, error code = %X\n", GetLastError()); 
		}
		else
		{
			if(StartService(schService,nArg,(const char**)pArg))
			{
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager); 
				return TRUE;
			}
			else
			{
				printf("StartService failed, error code = %X\n", GetLastError()); 
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager); 
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////// 
//
// Uninstall
//
VOID UnInstall(char* pName)
{
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		printf("OpenSCManager failed, error code = %X\n", GetLastError());
	}
	else
	{
		SC_HANDLE schService = OpenService( schSCManager, pName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			printf("OpenService failed, error code = %X\n", GetLastError()); 
		}
		else
		{
			if(!DeleteService(schService)) 
				printf("Failed to delete service %s\n", pName); 
			else printf("Service %s removed\n",pName); 
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager);	
	}
}

////////////////////////////////////////////////////////////////////// 
//
// Install
//
VOID Install(char* pPath, char* pName, char* pLogon, char* pPassword) 
{  
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_CREATE_SERVICE); 
	if (schSCManager==0) 
	{
		printf("OpenSCManager failed, error code = %X\n", GetLastError());
	}
	else
	{
		SC_HANDLE schService = CreateService
		( 
			schSCManager,	/* SCManager database      */ 
			pName,			/* name of service         */ 
			pName,			/* service name to display */ 
			SERVICE_ALL_ACCESS,        /* desired access          */ 
			pLogon?(SERVICE_WIN32_OWN_PROCESS):(SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS) , /* service type            */ 
			SERVICE_AUTO_START,      /* start type              */ 
			SERVICE_ERROR_NORMAL,      /* error control type      */ 
			pPath,			/* service's binary        */ 
			NULL,                      /* no load ordering group  */ 
			NULL,                      /* no tag identifier       */ 
			NULL,                      /* no dependencies         */ 
			pLogon,                      /* LocalSystem account     */ 
			pPassword				/* no password             */
		);                      
		if (schService==0) 
		{
			printf("Failed to create service %s, error code = %X\n", pName, GetLastError()); 
			printf("%s, %s\n", pLogon, pPassword);
		}
		else
		{
			printf("Service %s installed\n", pName);
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager);
	}	
}

////////////////////////////////////////////////////////////////////// 
//
// Standard C Main
//
void main(int argc, char *argv[] )
{
	if(argc==3&&_stricmp("-u",argv[1])==0)
	{
		UnInstall(argv[2]);
	}
	else if(argc>=4&&_stricmp("-i",argv[1])==0)
	{			
		Install(argv[3], argv[2], argc==6?argv[4]:NULL, argc==6?argv[5]:NULL);
	}
	else if(argc==3&&_stricmp("-k",argv[1])==0)
	{
		if(KillService(argv[2]))
		{
			printf("Killed service %s.\n", argv[2]); 
		}
		else
		{
			printf("Failed to kill service %s.\n", argv[2]); 
		}
	}
	else if(argc>=3&&_stricmp("-r",argv[1])==0)
	{
		if(RunService(argv[2], argc>3?(argc-3):0,argc>3?(&(argv[3])):NULL))
		{
			printf("Ran service %s.\n", argv[2]); 
		}
		else
		{
			printf("Failed to run service %s.\n", argv[2]); 
		}
	}
	else printf("Usage: ServiceInstaller -[iukr] ServiceName\n");
}
