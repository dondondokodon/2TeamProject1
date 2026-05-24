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
		openGoal = 1 << 0,     
		IsGoal = 1 << 1,  

		rotateLaser = 1 << 2,  
		rotateMirror = 1 << 3, 

		StageBGM = 1 << 4,
		LaserRotateSE = 1 << 5,
		MirrorRotateSE = 1 << 6,
		ResultBGM = 1 << 7,
		GoalSE = 1 << 8,
		GoalOpenSE = 1 << 9,
		TitleBGM=1<<10,
		reflectMirror=1<<11,
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

