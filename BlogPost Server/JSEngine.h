#pragma once

//#define STATIC_JS_API 1 // Compiles with static mode when build options were static
#include <jsapi.h>
#include <js/Initialization.h>

#pragma comment(lib, "mozjs-60.lib")

namespace JSEngine
{
	bool BasicInit()
	{
		return JS_Init();
	}

	void BasicDestroy() noexcept
	{
		JS_ShutDown();
	}
}