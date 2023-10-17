#include "knob.h"

#include <utility>

#include <atlbase.h>
#include <atlwin.h>

enum class HandlerId
{
    Push = 0,
    SpinLeft,
    SpinRight,

    Last
};

class Knob::Impl : public CWindowImpl<Impl, CWindow, CWinTraits<WS_POPUP>> 
{
public:
    BEGIN_MSG_MAP(Impl)
        MESSAGE_HANDLER(WM_HOTKEY, OnHotKey)
    END_MSG_MAP()

    Impl() 
    {
        Create (nullptr);

        ::RegisterHotKey(m_hWnd, static_cast<size_t>(HandlerId::Push),      0, VK_F20);
        ::RegisterHotKey(m_hWnd, static_cast<size_t>(HandlerId::SpinLeft),  0, VK_F18);
        ::RegisterHotKey(m_hWnd, static_cast<size_t>(HandlerId::SpinRight), 0, VK_F19);
    }

    ~Impl() 
    {
        ::UnregisterHotKey(m_hWnd, static_cast<size_t>(HandlerId::Push));
        ::UnregisterHotKey(m_hWnd, static_cast<size_t>(HandlerId::SpinLeft));
        ::UnregisterHotKey(m_hWnd, static_cast<size_t>(HandlerId::SpinRight));
    }

public:
    void Loop()
    {
        // Run message loop
        MSG msg;
        while (::GetMessage(&msg, NULL, 0, 0)) 
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }

    void SetHandler (HandlerId id, Handler handler)
    {
        m_handlers[static_cast<size_t>(id)] = std::move (handler);
    }   

private:
    LRESULT OnHotKey (UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/) 
    {
        const auto id = static_cast<size_t>(wParam);
        if (id < static_cast<size_t>(HandlerId::Last))
            m_handlers[id]();
        return 0;
    }

private:
    Knob::Handler m_handlers[static_cast<size_t>(HandlerId::Last)];    
};

Knob::Knob():
    m_impl {std::make_unique<Impl>()}
{
}

Knob::~Knob() = default;

void Knob::SetPushHandler (Handler handler)
{
    m_impl->SetHandler (HandlerId::Push, std::move (handler));
}

void Knob::SetSpinLeftHandler (Handler handler)
{
    m_impl->SetHandler (HandlerId::SpinLeft, std::move (handler));
}

void Knob::SetSpinRightHandler (Handler handler)
{
    m_impl->SetHandler (HandlerId::SpinRight, std::move (handler));
}

void Knob::Loop()
{
    m_impl->Loop();
}