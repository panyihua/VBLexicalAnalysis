#pragma  once
#include <string>
using namespace std;

enum DoorEvent
{
	EVENT_UNKOWN		   =   0,
	EVENT_OPEN_DOOR        =   1,
	EVENT_CLOSE_DOOR       =   2,	
	EVENT_LOCK_DOOR        =   3,
	EVENT_UNLOCK_DOOR      =   4
};

class Context;

class State
{
public:
	State(string stateStr="STATE_UNKOWN"):m_stateStr(stateStr){}
	virtual ~State(){}
	string getStateStr(){return m_stateStr;}

	virtual void Handle(Context* pContext, DoorEvent event) = 0;

protected:
	string m_stateStr;
};

class  StateOpened : public State
{
public:
	StateOpened() : State("STATE_OPENED"){}

	void Handle(Context* pContext, DoorEvent event);
};

class StateClosed: public State
{
public:
	StateClosed():State("STATE_CLOSED"){}

	void Handle(Context* pContext, DoorEvent event);
};

class StateLocked: public State
{
public:	
	StateLocked() : State("STATE_LOCKED"){}

	void Handle(Context* pContext, DoorEvent event);
};

class StateUnlocked: public State
{
public:
	StateUnlocked() : State("STATE_UNLOCKED"){}

	void Handle(Context* pContext, DoorEvent event);
};

class Context
{
public:
	Context(State* pState){m_state = pState;}
	~Context(){}
	void Request(DoorEvent event){m_state->Handle(this, event);}
	void ChangeState(State *pState){m_state = pState;}
	string getStateStr(void){return m_state->getStateStr();}

private:
	State *m_state;
};