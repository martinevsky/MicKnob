#include "mic.h"

#include <atlbase.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

class CoInit
{
public:
    CoInit()
    {
        ATLENSURE_SUCCEEDED(::CoInitialize(nullptr));
    }

    ~CoInit()
    {
        ::CoUninitialize();
    }
};

class Mic::Impl : CoInit
{
public:
    Impl()
    {
        // Create the device enumerator.
        CComPtr<IMMDeviceEnumerator> deviceEnumerator;
        ATLENSURE_SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (void**)&deviceEnumerator));

        // Get the default audio endpoint.
        CComPtr<IMMDevice> defaultDevice;
        ATLENSURE_SUCCEEDED(deviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &defaultDevice));

        // Activate the endpoint volume interface.
        ATLENSURE_SUCCEEDED(defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (void**)&m_endpointVolume));
    }

public:
    void ToggleMute()
    {
        BOOL mute;
        ATLENSURE_SUCCEEDED(m_endpointVolume->GetMute(&mute));
        ATLENSURE_SUCCEEDED(m_endpointVolume->SetMute(!mute, nullptr));
    }

    void IncVol()
    {
        ChangeVol(.02);
    }

    void DecVol()
    {
        ChangeVol(-.02);
    }

private:
    void ChangeVol(float delta)
    {
        float currentVolume;
        ATLENSURE_SUCCEEDED(m_endpointVolume->GetMasterVolumeLevelScalar(&currentVolume));
        ATLENSURE_SUCCEEDED(m_endpointVolume->SetMasterVolumeLevelScalar(currentVolume + delta, nullptr));
    }

private:
    CComPtr<IAudioEndpointVolume> m_endpointVolume;
};

Mic::Mic():
    m_impl {std::make_unique<Impl>()}
{
}

Mic::~Mic() = default;

void Mic::ToggleMute()
{
    m_impl->ToggleMute();
}

void Mic::IncVol()
{
    m_impl->IncVol();
}

void Mic::DecVol()
{
    m_impl->DecVol();
}