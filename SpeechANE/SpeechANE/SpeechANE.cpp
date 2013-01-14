#include "SpeechANE.h"
#include <windows.h>
#include <sapi.h>
#include <stdio.h>
#include <string.h>
#include <atlbase.h>
#include "sphelper.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;
using namespace System;

FREContext thisCtx;

public ref class SpeechListener
{
	public:
			inline HRESULT BlockForResult(ISpRecoContext * pRecoCtxt, ISpRecoResult ** ppResult)
			{
				HRESULT hr = S_OK;
				CSpEvent event;

				while (SUCCEEDED(hr) && SUCCEEDED(hr = event.GetFrom(pRecoCtxt)) && hr == S_FALSE)
				{
					hr = pRecoCtxt->WaitForNotifyEvent(INFINITE);
				}

				*ppResult = event.RecoResult();
				if (*ppResult)
				{
					(*ppResult)->AddRef();
				}

				return hr;
			}

			const WCHAR * StopWord()
			{
				const WCHAR * pchStop;
    
				//Determine Language (LANGID) of Local Machine
				LANGID LangId = ::SpGetUserDefaultUILanguage();

				//Defines "Stop Word" to end console.
				switch (LangId)
				{
					case MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT):
						pchStop = L"\x7d42\x4e86\\\x30b7\x30e5\x30fc\x30ea\x30e7\x30fc/\x3057\x3085\x3046\x308a\x3087\x3046";;
						break;

					default:
						pchStop = L"Stop";
						break;
				}

				return pchStop;
			}
            
			void main()
			{
				HRESULT hr = E_FAIL;

				::CoUninitialize();
				if (SUCCEEDED(hr = ::CoInitialize(NULL)))
				{
					{
						CComPtr<ISpRecoContext> cpRecoCtxt;
						CComPtr<ISpRecoGrammar> cpGrammar;
						CComPtr<ISpVoice> cpVoice;
						hr = cpRecoCtxt.CoCreateInstance(CLSID_SpSharedRecoContext);
						if(SUCCEEDED(hr))
						{
							hr = cpRecoCtxt->GetVoice(&cpVoice);
						}
           
						if (cpRecoCtxt && cpVoice &&
							SUCCEEDED(hr = cpRecoCtxt->SetNotifyWin32Event()) &&
							SUCCEEDED(hr = cpRecoCtxt->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION))) &&
							SUCCEEDED(hr = cpRecoCtxt->SetAudioOptions(SPAO_RETAIN_AUDIO, NULL, NULL)) &&
							SUCCEEDED(hr = cpRecoCtxt->CreateGrammar(0, &cpGrammar)) &&
							SUCCEEDED(hr = cpGrammar->LoadDictation(NULL, SPLO_STATIC)) &&
							SUCCEEDED(hr = cpGrammar->SetDictationState(SPRS_ACTIVE)))
						{
							const WCHAR * const pchStop = StopWord();
							CComPtr<ISpRecoResult> cpResult;

							printf( "Speech Recognition Console \n", (LPSTR)CW2A(pchStop));
							printf( "Listening...\n");


							while (SUCCEEDED(hr = BlockForResult(cpRecoCtxt, &cpResult)))
							{
								cpGrammar->SetDictationState( SPRS_INACTIVE );

								CSpDynamicString dstrText;

								if (SUCCEEDED(cpResult->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, TRUE, &dstrText, NULL)))
								{
									//printf("Original Output: %s \n", (LPSTR)CW2A(dstrText));
						
									BSTR bstr;
									dstrText.CopyToBSTR(&bstr);
									wstring output(bstr, SysStringLen(bstr));
									char* thing = new char[wcslen(output.c_str()) + 1];
									wcstombs(thing, output.c_str(), wcslen(output.c_str()-1));

									string outputString = thing;

									cout << "Output:" << outputString << endl;

									FREDispatchStatusEventAsync(thisCtx, (const uint8_t *)outputString.c_str(),(const uint8_t *)"Speech Phrase");

									cpResult.Release();
								}

								if (_wcsicmp(dstrText, pchStop) == 0)
								{
									break;
								}
                    
								cpGrammar->SetDictationState( SPRS_ACTIVE );
							} 
						}
					}
					
					::CoUninitialize();
				}
				//return hr;
			}

	
};

FREObject startSpeech(FREContext ctx, void* funcDava, uint32_t argc, FREObject argv[])
{
	FREObject result;
	uint32_t returnValue = 0;

	SpeechListener^ speechListener = gcnew SpeechListener();
	System::Threading::Thread^ speechListenerThread = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(speechListener, &SpeechListener::main));
	speechListenerThread->IsBackground = false;
	speechListenerThread->Start();

	if(speechListenerThread->ThreadState == System::Threading::ThreadState::Running)
	{
		returnValue = 1;
	}

	FRENewObjectFromBool(returnValue, &result);
	return result;
}

FREObject isSupported(FREContext ctx, void* funcDava, uint32_t argc, FREObject argv[])
{
	FREObject result;

	uint32_t isSupportedInThisOS = 1;
	FRENewObjectFromBool(isSupportedInThisOS, &result);

	return result;
}

void ContextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx, uint32_t* numFunctionsToTest, const FRENamedFunction** functionsToSet)
{
	thisCtx = ctx;

	*numFunctionsToTest = 2;
	FRENamedFunction* func = (FRENamedFunction*)malloc(sizeof(FRENamedFunction) * (*numFunctionsToTest));

	func[0].name = (const uint8_t*)"isSupported";
	func[0].functionData = NULL;
	func[0].function = &isSupported;

	func[1].name = (const uint8_t*)"startSpeech";
	func[1].functionData = NULL;
	func[1].function = &startSpeech;

	*functionsToSet = func;
}
void ContextFinalizer(FREContext ctx)
{
	return;
}

void ExtInitializer(void** extDataToSet, FREContextInitializer* ctxInitializerToSet, FREContextFinalizer* ctxFinalizerToSet)
{
	*extDataToSet = NULL;
	*ctxInitializerToSet = &ContextInitializer;
	*ctxFinalizerToSet = &ContextFinalizer;
}

void ExtFinalizer(void* extData)
{
	return;
}
