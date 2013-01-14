#include "FlashRuntimeExtensions.h"

#include<Windows.h>

extern "C" __declspec(dllexport) void ExtInitializer(void** extDataToSet, FREContextInitializer* ctxInitializerToSet, FREContextFinalizer* ctxFinalizerToSet);

extern "C" __declspec(dllexport) void ExtFinalizer(void* extData);