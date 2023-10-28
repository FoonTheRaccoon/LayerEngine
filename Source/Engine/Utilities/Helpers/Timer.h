#pragma once
#include "ThirdParty.h"


using namespace std::chrono_literals;

class LayerTimer
{
public:
	LayerTimer(const char* label);
	LayerTimer() : LayerTimer("") {}
	~LayerTimer();

private:
	const char* m_Label;
	std::chrono::steady_clock::time_point m_Begin;
};


#define TIMER_LOG(label) LayerTimer _MACRO_CONCAT( macroTimer, __LINE__ )(label);

