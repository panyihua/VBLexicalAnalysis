#include <iostream>
#include <iomanip>
using namespace std;
#include "fsm.h"
#include <Windows.h>

const char *DoorEventStr[] = 
{
	"EVENT_UNKOWN",
	"EVENT_OPEN_DOOR",
	"EVENT_CLOSE_DOOR",	
	"EVENT_LOCK_DOOR",
	"EVENT_UNLOCK_DOOR"
};

void StateOpened::Handle(Context* pContext, DoorEvent event)
{
	switch(event)
	{
	case EVENT_CLOSE_DOOR:
		pContext->ChangeState(new StateClosed);
		break;

	default:
		break;
	}
}

void StateClosed::Handle(Context* pContext, DoorEvent event)
{
	switch(event)
	{
	case EVENT_LOCK_DOOR:
		pContext->ChangeState(new StateLocked);
		break;

	case EVENT_OPEN_DOOR:
		pContext->ChangeState(new StateOpened);
		break;

	default:
		break;
	}
}

void StateLocked::Handle(Context* pContext, DoorEvent event)
{
	switch(event)
	{
	case EVENT_UNLOCK_DOOR:
		pContext->ChangeState(new StateUnlocked);
		break;

	default:
		break;
	}
}


void StateUnlocked::Handle(Context* pContext, DoorEvent event)
{
	switch(event)
	{
	case EVENT_OPEN_DOOR:
		pContext->ChangeState(new StateOpened);
		break;

	case EVENT_LOCK_DOOR:
		pContext->ChangeState(new StateLocked);
		break;

	default:
		break;
	}
}	


int main(int argc, char* argv[])
{
	DoorEvent event = EVENT_UNKOWN;  

	Context *pContext = new Context(new StateOpened);
	
	int index = 0;
	while(index < 10)
	{  
		switch(index%4+1)
		{  
			case 1: event = EVENT_OPEN_DOOR; break;  
			case 2: event = EVENT_CLOSE_DOOR; break;  
			case 3: event = EVENT_LOCK_DOOR; break;  
			case 4: event = EVENT_UNLOCK_DOOR; break;  
			default: break;  
		}

		index++;  

		string lastStat = pContext->getStateStr();

		pContext->Request(event);

		cout<<setiosflags(ios::left) 
			<<setw(15)<<lastStat
			<<" - "
			<<setw(20)<<DoorEventStr[event]
			<<" - "
			<<setw(15)<<pContext->getStateStr()<<endl;

		Sleep(1);
	}

	return 0;
}