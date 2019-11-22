#include "hdevice.h"

#ifdef _WIN32
#include <windows.h>
#define NO_DSHOW_STRSAFE
#include <dshow.h>

static std::vector<HDevice> getDevicesList(REFGUID category){
    std::vector<HDevice> ret;
    //HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    HRESULT hr = S_OK;
    if (SUCCEEDED(hr)) {
        ICreateDevEnum *pDevEnum = NULL;
        HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
            CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));
        if (SUCCEEDED(hr) && pDevEnum) {
            IEnumMoniker *pEnum = NULL;
            hr = pDevEnum->CreateClassEnumerator(category, &pEnum, 0);
            if (hr == S_FALSE || pEnum == NULL) {
                pDevEnum->Release();
                return ret;
            }

            IMoniker *pMoniker = NULL;
            while (pEnum->Next(1, &pMoniker, NULL) == S_OK && pMoniker) {
                IPropertyBag *pPropBag = NULL;
                HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
                if (FAILED(hr) || pPropBag == NULL) {
                    pMoniker->Release();
                    continue;
                }

                VARIANT var;
                VariantInit(&var);

                // Get description or friendly name.
                hr = pPropBag->Read(L"Description", &var, 0);
                if (FAILED(hr)) {
                    hr = pPropBag->Read(L"FriendlyName", &var, 0);
                }
                if (SUCCEEDED(hr)) {
                    HDevice dev;
                    sprintf(dev.name, "%S", var.bstrVal);
                    ret.push_back(dev);
                    //printf("%S\n", var.bstrVal);
                    VariantClear(&var);
                }

//                hr = pPropBag->Write(L"FriendlyName", &var);

//                // WaveInID applies only to audio capture devices.
//                hr = pPropBag->Read(L"WaveInID", &var, 0);
//                if (SUCCEEDED(hr)) {
//                    printf("WaveIn ID: %d\n", var.lVal);
//                    VariantClear(&var);
//                }

//                hr = pPropBag->Read(L"DevicePath", &var, 0);
//                if (SUCCEEDED(hr)) {
//                    // The device path is not intended for display.
//                    printf("Device path: %S\n", var.bstrVal);
//                    VariantClear(&var);
//                }

                pPropBag->Release();
                pMoniker->Release();
            }

            pEnum->Release();
            pDevEnum->Release();
        }

        //CoUninitialize();
    }

    return ret;
}

std::vector<HDevice> getVideoDevices() {
    return getDevicesList(CLSID_VideoInputDeviceCategory);
}

std::vector<HDevice> getAudioDevices() {
    return getDevicesList(CLSID_AudioInputDeviceCategory);
}
#else
std::vector<HDevice> getVideoDevices() {
    return std::vector<HDevice>();
}

std::vector<HDevice> getAudioDevices() {
    return std::vector<HDevice>();
}
#endif
