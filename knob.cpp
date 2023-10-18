#include "knob.h"

#include <utility>
#include <ostream>

#include <atlbase.h>
#include <atlwin.h>

#include "error_handling.h"

enum class HandlerId
{
    Push = 0,
    SpinLeft,
    SpinRight,

    Last
};

void ThrowGleErrorIfFalse (bool val, const char* msg, std::source_location location = std::source_location::current())
{
    if (!val) [[unlikelly]]
    {
        struct GleError final : TraceableException
        {
            explicit GleError (DWORD gle, const char* msg, std::source_location location):
                TraceableException {msg, std::move (location)},
                m_gle {gle}
            {
            }

        protected:
            // TraceableException
            virtual void Format (std::ostream& os) const override
            {
                char err[256] = {};
                const auto errSize = ::FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM, nullptr, m_gle, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, std::size (err) - 1, nullptr);
                if (errSize > 2)
                    err[errSize - 2] = '\0'; // Clear '\n' at the end
                os << "0x" << std::hex << m_gle << std::dec << " - \"" << err << '"'; 
            }

        private:
            const DWORD m_gle;
        };

        throw GleError {::GetLastError(), msg, std::move (location)};
    }
}

class Knob::Impl : public CWindowImpl<Impl, CWindow, CWinTraits<WS_POPUP>> 
{
public:
    BEGIN_MSG_MAP(Impl)
        MESSAGE_HANDLER(WM_HOTKEY, OnHotKey)
    END_MSG_MAP()

    Impl() 
    {
        Create (nullptr);

        try
        {
            ThrowGleErrorIfFalse (::RegisterHotKey(m_hWnd, static_cast<size_t>(HandlerId::Push),      0, VK_F20), "Hotkey 1 registaretion");
            ThrowGleErrorIfFalse (::RegisterHotKey(m_hWnd, static_cast<size_t>(HandlerId::SpinLeft),  0, VK_F18), "Hotkey 2 registaretion");
            ThrowGleErrorIfFalse (::RegisterHotKey(m_hWnd, static_cast<size_t>(HandlerId::SpinRight), 0, VK_F19), "Hotkey 3 registaretion");
        }
        catch(...)
        {
            // WTL needs this
            DestroyWindow();
            throw;
        }
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
        while (const auto getMessageRes = ::GetMessage(&msg, nullptr, 0, 0)) 
        {
            ThrowGleErrorIfFalse (getMessageRes != -1, "GetMessage");
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
            IgnoreException (m_handlers[id]);
        return 0;
    }

private:
    Knob::Handler m_handlers[static_cast<size_t>(HandlerId::Last)];    
};

Knob::Knob()
try
    :m_impl {std::make_unique<Impl>()}
{
}
catch (const std::bad_alloc&)
{
    std::throw_with_nested (std::runtime_error {"Bad alloc while Knob::Impl construction"});
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