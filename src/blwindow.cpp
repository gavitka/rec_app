#include <QApplication>

#include "blwindow.h"

BLWindow::BLWindow(QWindow *parent) :
    QQuickWindow(parent)
{
    setFlags(Qt::FramelessWindowHint);
    m_hwnd = reinterpret_cast<HWND>(winId());
    setStyle();

    //connect(this, &BLWindow::beforeRendering, this, &BLWindow::setStyle);
}

bool BLWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType);

    auto msg = static_cast<MSG *>(message);

    switch (msg->message)
    {
    case WM_NCCREATE: {
            auto userdata = reinterpret_cast<CREATESTRUCTW*>(msg->lParam)->lpCreateParams;
            // store window instance pointer in window user data
            ::SetWindowLongPtrW(msg->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(userdata));
        }
    case WM_ERASEBKGND: {
            auto brush = CreateSolidBrush(RGB(48, 48, 48));
            SetClassLongPtr(msg->hwnd, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(brush));
        }

    case WM_NCCALCSIZE: {
            if (msg->wParam == TRUE) {
                auto& params = *reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
                adjust_maximized_client_rect(msg->hwnd, params.rgrc[0]);
            }
            *result = 0;
            return true;
        }

    case WM_NCHITTEST: {
            *result = 0;
            const LONG border_width = 4; //in pixels
            RECT winrect;
            GetWindowRect(reinterpret_cast<HWND>(winId()), &winrect);

            long x = GET_X_LPARAM(msg->lParam);
            long y = GET_Y_LPARAM(msg->lParam);

            auto resizeWidth = minimumWidth() != maximumWidth();
            auto resizeHeight = minimumHeight() != maximumHeight();

            if (resizeWidth) {
                //left border
                if (x >= winrect.left && x < winrect.left + border_width) {
                    *result = HTLEFT;
                }
                //right border
                if (x < winrect.right && x >= winrect.right - border_width) {
                    *result = HTRIGHT;
                }
            }
            if (resizeHeight) {
                //bottom border
                if (y < winrect.bottom && y >= winrect.bottom - border_width) {
                    *result = HTBOTTOM;
                }
                //top border
                if (y >= winrect.top && y < winrect.top + border_width) {
                    *result = HTTOP;
                }
            }
            if (resizeWidth && resizeHeight) {
                //bottom left corner
                if (x >= winrect.left && x < winrect.left + border_width &&
                    y < winrect.bottom && y >= winrect.bottom - border_width) {
                    *result = HTBOTTOMLEFT;
                }
                //bottom right corner
                if (x < winrect.right && x >= winrect.right - border_width &&
                    y < winrect.bottom && y >= winrect.bottom - border_width) {
                    *result = HTBOTTOMRIGHT;
                }
                //top left corner
                if (x >= winrect.left && x < winrect.left + border_width &&
                    y >= winrect.top && y < winrect.top + border_width) {
                    *result = HTTOPLEFT;
                }
                //top right corner
                if (x < winrect.right && x >= winrect.right - border_width &&
                    y >= winrect.top && y < winrect.top + border_width) {
                    *result = HTTOPRIGHT;
                }
            }

            if(x >= winrect.right - 46 * 3 && y <= winrect.top + 30) {
                *result = HTCLIENT; // TODO: Fix
            }

            if (*result != 0)
                return true;

            if (y >= winrect.top && y < winrect.top + 20) {
                *result = HTCAPTION;
                return true;
            }
            break;

    //        auto action = QApplication::widgetAt(QCursor::pos());
    //        if (action == title_bar_widget_) {
    //            *result = HTCAPTION;
    //            return true;
    //        }
    //        break;
        }

    case WM_GETMINMAXINFO: {
            MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(msg->lParam);

            if (maximized(msg->hwnd)) {

                RECT window_rect;

                if (!GetWindowRect(msg->hwnd, &window_rect)) {
                    return false;

                }

                HMONITOR monitor = MonitorFromRect(&window_rect, MONITOR_DEFAULTTONULL);
                if (!monitor) {
                    return false;
                }

                MONITORINFO monitor_info = { 0 };
                monitor_info.cbSize = sizeof(monitor_info);
                GetMonitorInfo(monitor, &monitor_info);

                RECT work_area = monitor_info.rcWork;
                RECT monitor_rect = monitor_info.rcMonitor;

                mmi->ptMaxPosition.x = abs(work_area.left - monitor_rect.left);
                mmi->ptMaxPosition.y = abs(work_area.top - monitor_rect.top);

                mmi->ptMaxSize.x = abs(work_area.right - work_area.left);
                mmi->ptMaxSize.y = abs(work_area.bottom - work_area.top);
                mmi->ptMaxTrackSize.x = mmi->ptMaxSize.x;
                mmi->ptMaxTrackSize.y = mmi->ptMaxSize.y;

                *result = 1;
                return true;
            }
        }

    case WM_NCACTIVATE: {
            if (!composition_enabled()) {
                // Prevents window frame reappearing on window activation
                // in "basic" theme, where no aero shadow is present.
                *result = 1;
                return true;
            }
            break;
        }

    case WM_SIZE: {
            RECT winrect;
            GetClientRect(msg->hwnd, &winrect);
            WINDOWPLACEMENT wp;
            wp.length = sizeof(WINDOWPLACEMENT);
            GetWindowPlacement(msg->hwnd, &wp);
            if (this) {
                if (wp.showCmd == SW_MAXIMIZE) {
                    ::SetWindowPos(reinterpret_cast<HWND>(winId()), Q_NULLPTR, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
                }
            }
        }

    // But whaa??  default: QQuickWindow::nativeEvent(eventType, message, result);
    } // switch

    return QQuickWindow::nativeEvent(eventType, message, result);
}

void BLWindow::setStyle()
{
    ::SetWindowLongPtrW(m_hwnd, GWL_STYLE, static_cast<LONG>(aero_borderless));
    ::SetWindowLongPtrW(m_hwnd, GWL_EXSTYLE, static_cast<LONG>(ex_style));
    set_shadow(true);
    ::SetWindowPos(reinterpret_cast<HWND>(winId()), nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
}

LRESULT BLWindow::hit_test(POINT cursor) const
{
    const POINT border{
        ::GetSystemMetrics(SM_CXFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER),
        ::GetSystemMetrics(SM_CYFRAME) + ::GetSystemMetrics(SM_CXPADDEDBORDER)
    };
    RECT window;
    if (!::GetWindowRect(m_hwnd, &window)) {
        return HTNOWHERE;
    }

    enum region_mask {
        client = 0b0000,
        left   = 0b0001,
        right  = 0b0010,
        top    = 0b0100,
        bottom = 0b1000,
    };

    const auto result =
        left    * (cursor.x <  (window.left   + border.x)) |
        right   * (cursor.x >= (window.right  - border.x)) |
        top     * (cursor.y <  (window.top    + border.y)) |
        bottom  * (cursor.y >= (window.bottom - border.y));

    switch (result) {
        case left          : return HTLEFT;
        case right         : return HTRIGHT;
        case top           : return HTTOP;
        case bottom        : return HTBOTTOM;
        case top | left    : return HTTOPLEFT;
        case top | right   : return HTTOPRIGHT;
        case bottom | left : return HTBOTTOMLEFT;
        case bottom | right: return HTBOTTOMRIGHT;
        case client        : return HTCLIENT; // HTCAPTION
        default            : return HTNOWHERE;
    }
}

void BLWindow::set_shadow(bool enabled)
{
    if (composition_enabled()) {
        static const MARGINS shadow_state[2]{ { 0,0,0,0 },{ 1,1,1,1 } };
        ::DwmExtendFrameIntoClientArea(m_hwnd, &shadow_state[enabled]);
    }
}

bool BLWindow::composition_enabled()
{
    BOOL composition_enabled = FALSE;
    bool success = ::DwmIsCompositionEnabled(&composition_enabled) == S_OK;
    return composition_enabled && success;
}

bool BLWindow::maximized(HWND hwnd)
{
    WINDOWPLACEMENT placement;
    if (!::GetWindowPlacement(hwnd, &placement)) {
        return false;
    }

    return placement.showCmd == SW_MAXIMIZE;
}

void BLWindow:: adjust_maximized_client_rect(HWND window, RECT& rect)
{
    if (!maximized(window)) {
        return;
    }

    auto monitor = ::MonitorFromWindow(window, MONITOR_DEFAULTTONULL);
    if (!monitor) {
        return;
    }

    MONITORINFO monitor_info{};
    monitor_info.cbSize = sizeof(monitor_info);
    if (!::GetMonitorInfoW(monitor, &monitor_info)) {
        return;
    }

    // when maximized, make the client area fill just the monitor (without task bar) rect,
    // not the whole window rect which extends beyond the monitor.
    rect = monitor_info.rcWork;
}
