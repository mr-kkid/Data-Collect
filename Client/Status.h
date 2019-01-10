#pragma once
class Status
{
protected:
	bool flag;
public:
	Status(){
	}
	~Status();
	virtual void SetStatus(bool status) = 0;
	virtual bool GetStatus() = 0;
};

