#pragma once

#ifdef NOVA_MACOS

#import <Cocoa/Cocoa.h>
#include <nova/platform/window_driver.h>

#include <unordered_map>

namespace Nova {

	class CocoaWindowDriver final : public WindowDriver {
	  public:
		CocoaWindowDriver();
		~CocoaWindowDriver() override;

		WindowAPI get_api() const override;
		std::string get_api_name() const override;

		void poll_events() override;
		void beep() override;

		u32 get_window_count() const override;
		WindowID create_window(const std::string& title, u32 width, u32 height) override;
		void destroy_window(WindowID window) override;

		void set_window_title(WindowID window, const std::string& title) override;
		void set_window_size(WindowID window, u32 width, u32 height) override;
		void set_window_position(WindowID window, i32 x, i32 y) override;

		const char* get_surface_extension() const override;
		SurfaceID create_surface(WindowID window, RenderDriver* p_driver) override;

	  private:
		std::unordered_map<NSWindow*, WindowID> m_windows;

		NSApplication* m_app = nullptr;

		void _on_window_close(NSWindow* window);
	};

} // namespace Nova

#endif // NOVA_MACOS
