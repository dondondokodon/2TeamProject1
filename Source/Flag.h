#pragma once

class Flag
{
public:
	~Flag()=default;

	static Flag& Instance()
	{
		static Flag ins;
		return ins;
	}

	enum eventName
	{
		openGoal = 0x01,
		IsGoal   = 0x02,
	};

	void SetFlag(eventName name,bool q)
	{
		if (q)
			flag |= name;
		else
			flag &= ~name;
	}

	void ClearFlag()
	{
		flag = 0;
	}

	bool getFlag(eventName name)
	{
		return flag & name;
	}

private:
	int flag = 0;

	Flag(){}
};

