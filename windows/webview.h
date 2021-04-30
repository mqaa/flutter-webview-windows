#pragma once

#include <WebView2.h>
#include <wil/com.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <wrl.h>

#include <functional>

class WebviewHost;

enum class WebviewLoadingState {
  None,
  Loading,
  NavigationCompleted
};

enum class WebviewPointerButton {
  None,
  Primary,
  Secondary,
  Tertiary
};

class Webview {
 public:
  friend class WebviewHost;

  typedef std::function<void(const std::string&)> UrlChangedCallback;
  typedef std::function<void(WebviewLoadingState)> LoadingStateChangedCallback;
  typedef std::function<void(size_t width, size_t height)>
      SurfaceSizeChangedCallback;

  winrt::agile_ref<winrt::Windows::UI::Composition::Visual> const surface() {
    return surface_;
  }

  void SetSurfaceSize(size_t width, size_t height);

  void SetCursorPos(double x, double y);

  void SetPointerButtonState(WebviewPointerButton button, bool isDown);

  void SetScrollDelta(double delta_x, double delta_y);
  void Navigate(const std::string& url);
  bool ClearCookies();

  void OnUrlChanged(UrlChangedCallback callback) {
    url_changed_callback_ = std::move(callback);
  }

  void OnLoadingStateChanged(LoadingStateChangedCallback callback) {
    loading_state_changed_callback_ = std::move(callback);
  }

  void OnSurfaceSizeChanged(SurfaceSizeChangedCallback callback) {
    surface_size_changed_callback_ = std::move(callback);
  }

 private:
  wil::com_ptr<ICoreWebView2CompositionController> composition_controller_;
  wil::com_ptr<ICoreWebView2Controller3> webview_controller_;
  wil::com_ptr<ICoreWebView2> webview_;
  POINT last_cursor_pos_ = {0, 0};

  winrt::agile_ref<winrt::Windows::UI::Composition::Visual> surface_{nullptr};

  winrt::agile_ref<winrt::Windows::UI::Core::CoreDispatcher> dispatcher_{
      nullptr};

  winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget window_target_{
      nullptr};

  WebviewHost* host_;

  EventRegistrationToken source_changed_token_{};
  EventRegistrationToken content_loading_token_{};
  EventRegistrationToken navigation_completed_token_{};

  UrlChangedCallback url_changed_callback_;
  LoadingStateChangedCallback loading_state_changed_callback_;
  SurfaceSizeChangedCallback surface_size_changed_callback_;

  Webview(
      wil::com_ptr<ICoreWebView2CompositionController> composition_controller,
      WebviewHost* host, std::optional<HWND> hwnd);

  void RegisterEventHandlers();
};