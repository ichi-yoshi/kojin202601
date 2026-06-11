#pragma once

struct CharaStatus
{
	double hp = 0.0;				//HP
	double attack = 0.0;			//攻撃
	double defense = 0.0;			//防御

	double hpPercent = 0.0;			//HP%
	double attackPercent = 0.0;		//攻撃%
	double defensePercent = 0.0;	//防御%

	double critRate = 0.0;			//会心率
	double critDamage = 0.0;		//会心ダメージ
	double speed = 0.0;				//速度
	double luck = 0.0;				//運値
	double poop = 0.0;				//Poop
};

class CharaBase
{
public:
	const CharaStatus& GetBaseStatus() const;
	void SetBaseStatus(const CharaStatus& status);

private:
	CharaStatus _baseStatus;
};
