#pragma once

#ifdef MMMENGINE_EXPORTS
#define MMMENGINE_API __declspec(dllexport)
#else
#define MMMENGINE_API __declspec(dllimport)
#endif