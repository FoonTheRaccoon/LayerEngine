 #include "Timer.h"
#include "Logger.h"


LayerTimer::LayerTimer(const char* label) 
	: m_Label(label)
{
	m_Begin = std::chrono::steady_clock::now();
}

LayerTimer::~LayerTimer()
{
	char msgBuffer[U8_MAX];
	const std::chrono::duration<float, std::milli> duration = std::chrono::steady_clock::now() - m_Begin;

	snprintf(msgBuffer, U8_MAX - 1, "Duration Of Timer \"%s\": %f ms", m_Label, duration.count());

	LOG_BENCHMARK(msgBuffer)
}
