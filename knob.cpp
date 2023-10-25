#include "knob.h"

#include <utility>
#include <ostream>
#include <optional>

#include <atlbase.h>
#include <atlwin.h>

#include "error_handling.h"

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
                ::FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK, nullptr, m_gle, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, std::size (err) - 1, nullptr);
                os << "0x" << std::hex << m_gle << std::dec << " - \"" << err << '"'; 
            }

        private:
            const DWORD m_gle;
        };

        throw GleError {::GetLastError(), msg, std::move (location)};
    }
}

template <class CRTP>
class HotkeyWindow : public CWindowImpl<HotkeyWindow<CRTP>, CWindow, CWinTraits<WS_POPUP>>
{
public:
    BEGIN_MSG_MAP(Impl)
        MESSAGE_HANDLER(WM_HOTKEY, OnHotKey)
    END_MSG_MAP()

    HotkeyWindow()
    {
        this->Create (nullptr);
    }
    
    ~HotkeyWindow()
    {
        // WTL needs this
        if (GetHwnd())
            this->DestroyWindow();
    }

public:
    auto GetHwnd() const
    {
        return this->m_hWnd;
    }

private:
    LRESULT OnHotKey (UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/) 
    {
        static_cast<CRTP*>(this)->OnHotKey (static_cast<size_t>(wParam));
        return 0;
    }
};

enum class HandlerId
{
    Push = 0,
    SpinLeft,
    SpinRight,

    Last
};

class Knob::Impl : public HotkeyWindow<Impl>
{
public:
    void SetHandler (Handler handler, UINT vk, HandlerId id)
    {
        m_handlers[static_cast<size_t>(id)].emplace (std::move (handler), vk, static_cast<size_t> (id), GetHwnd());
    }

    void OnHotKey (size_t id) noexcept
    {   
        if (id < std::size (m_handlers) && m_handlers[id])
            m_handlers[id]->Trigger();
    }

private:
    struct HotKey
    {
    public:
        explicit HotKey (Handler handler, UINT vk, size_t id, HWND hWnd):
            m_handler {std::move (handler)}, m_id (id), m_hWnd {hWnd}
        {
            ThrowGleErrorIfFalse (::RegisterHotKey (m_hWnd, m_id, 0, vk), "Hotkey registretion");
        }

        ~HotKey()
        {
            ::UnregisterHotKey (m_hWnd, m_id);
        }

    public:
        void Trigger() noexcept
        {
            IgnoreException (m_handler);
        }

    private:
        const Handler m_handler;
        const size_t  m_id;
        const HWND    m_hWnd;
    };

    std::optional<HotKey> m_handlers[static_cast<size_t>(HandlerId::Last)];
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
    m_impl->SetHandler (std::move (handler), VK_F20, HandlerId::Push);
}

void Knob::SetSpinLeftHandler (Handler handler)
{
    m_impl->SetHandler (std::move (handler), VK_F18, HandlerId::SpinLeft);
}

void Knob::SetSpinRightHandler (Handler handler)
{
    m_impl->SetHandler (std::move (handler), VK_F19, HandlerId::SpinRight);
}