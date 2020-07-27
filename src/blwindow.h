#pragma once

#include "pch.h"

#include <QQuickWindow>
#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>

class BLWindow : public QQuickWindow
{
    Q_OBJECT

public:

    explicit BLWindow(QWindow *parent = nullptr);

protected:

    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

signals:

private:

    void setStyle();
    LRESULT hit_test(POINT cursor) const;
    void set_shadow(bool enabled);
    bool composition_enabled();
    bool maximized(HWND hwnd);
    void adjust_maximized_client_rect(HWND window, RECT &rect);

    static constexpr DWORD aero_borderless  =
            WS_THICKFRAME | WS_CAPTION | WS_SYSMENU |
            WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
    // WS_POPUP

    static constexpr DWORD ex_style = WS_EX_APPWINDOW;
    //

    HWND m_hwnd;
};

