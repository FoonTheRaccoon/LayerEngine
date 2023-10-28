#include "StringHelper.h"


u64 StringHelpers::GetCstrSize(const char* str)
{
	u64 index = 0;
	while (str[index] != '\0')
	{
		index++;
	}
	return index;
}