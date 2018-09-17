#include "hdevice.h"

#ifdef _WIN32
#include <windows.h>
#define NO_DSHOW_STRSAFE
#include <dshow.h>

vector<HDevice> getDevicesList(REFGUID category){
    vector<HDevice> ret;
    //HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    HRESULT hr = S_OK;
    if (SUCCEEDED(hr)){
        IEnumMoniker *pEnum;
        ICreateDevEnum *pDevEnum;
        HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
            CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));

        if (SUCCEEDED(hr))
        {
            // Create an enumerator for the category.
            hr = pDevEnum->CreateClassEnumerator(category, &pEnum, 0);
            if (hr == S_FALSE)
            {
                hr = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
            }
            pDevEnum->Release();

            IMoniker *pMoniker = NULL;
            while (pEnum->Next(1, &pMoniker, NULL) == S_OK){
                IPropertyBag *pPropBag;
                HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
                if (FAILED(hr))
                {
                    pMoniker->Release();
                    continue;
                }

                VARIANT var;
                VariantInit(&var);

                // Get description or friendly name.
                hr = pPropBag->Read(L"Description", &var, 0);
                if (FAILED(hr))
                {
                    hr = pPropBag->Read(L"FriendlyName", &var, 0);
                }
                if (SUCCEEDED(hr))
                {
                    HDevice dev;
                    sprintf(dev.name, "%S", var.bstrVal);
                    ret.push_back(dev);
                    //printf("%S\n", var.bstrVal);
                    VariantClear(&var);
                }

//                hr = pPropBag->Write(L"FriendlyName", &var);

//                // WaveInID applies only to audio capture devices.
//                hr = pPropBag->Read(L"WaveInID", &var, 0);
//                if (SUCCEEDED(hr))
//                {
//                    printf("WaveIn ID: %d\n", var.lVal);
//                    VariantClear(&var);
//                }

//                hr = pPropBag->Read(L"DevicePath", &var, 0);
//                if (SUCCEEDED(hr))
//                {
//                    // The device path is not intended for display.
//                    printf("Device path: %S\n", var.bstrVal);
//                    VariantClear(&var);
//                }

                pPropBag->Release();
                pMoniker->Release();
            }
        }

        //CoUninitialize();
    }

    return ret;
}

vector<HDevice> getVideoDevices(){
    return getDevicesList(CLSID_VideoInputDeviceCategory);
}

vector<HDevice> getAudioDevices(){
    return getDevicesList(CLSID_AudioInputDeviceCategory);
}
#else
vector<HDevice> getVideoDevices(){
    return vector<HDevice>();
}

vector<HDevice> getAudioDevices(){
    return vector<HDevice>();
}
#endif
