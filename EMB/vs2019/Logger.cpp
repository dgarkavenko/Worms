#include "Logger.h"
#include <StructuredQueryCondition.h>

void Logger::Output(char* text)
{
	OutputDebugStringA(text);
}
