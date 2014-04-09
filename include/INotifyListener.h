#pragma once

class INotifyListener 
{	
public:
	
	enum INotifyEventMask {CREATE = 0, DELETE = 2, MODIFY = 4;}
	
	INotifyListener(const std::string&	folder);
	void setListeningEventType(INotifyEventMask mask);
};
