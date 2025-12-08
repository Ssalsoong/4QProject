#pragma once
#include "fast_delegate/MultiCastDelegate.h"

using namespace SA;

namespace MMMEngine
{	
	template<typename T>
	using Delegate = SA::multicast_delegate<T>;

    template<typename... Args>
    using Action = SA::multicast_delegate<void(Args...)>;

    template<typename T, typename... Args>
    using Func = SA::multicast_delegate<T(Args...)>;
}