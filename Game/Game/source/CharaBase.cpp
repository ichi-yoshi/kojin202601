#include "CharaBase.h"

const CharaStatus& CharaBase::GetBaseStatus() const
{
	return _baseStatus;
}

void CharaBase::SetBaseStatus(const CharaStatus& status)
{
	_baseStatus = status;
}