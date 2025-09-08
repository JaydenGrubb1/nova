#ifdef NOVA_MACOS

#import "platform/macos/window_driver.h"
#import <nova/core/debug.h>

#ifdef NOVA_VULKAN
#import <vulkan/vulkan.h>
#import "drivers/vulkan/render_driver.h"
#endif

using namespace Nova;

namespace {
    class CocoaDelegate : public NSObject<NSApplicationDelegate, NSWindowDelegate> {
    public:
        CocoaWindowDriver* driver;

        CocoaDelegate(CocoaWindowDriver* d) : driver(d) {}

        - (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
            return YES;
        }

        - (void)windowWillClose:(NSNotification*)notification {
            NSWindow* window = (NSWindow*)notification.object;
            driver->_on_window_close(window);
        }
    };
}

CocoaWindowDriver::CocoaWindowDriver() {
	NOVA_AUTO_TRACE();

	[NSApplication sharedApplication];
	m_app = NSApp;

	static CocoaDelegate* delegate = [[CocoaDelegate alloc] init];
	delegate->driver = this;
	[m_app setDelegate:delegate];
}

CocoaWindowDriver::~CocoaWindowDriver() {
	NOVA_AUTO_TRACE();
	m_windows.clear();
}

WindowAPI CocoaWindowDriver::get_api() const {
	return WindowAPI::MACOS;
}

std::string CocoaWindowDriver::get_api_name() const {
	return "Cocoa";
}

void CocoaWindowDriver::poll_events() {
	NSEvent* event;
	while ((event = [m_app nextEventMatchingMask:NSEventMaskAny
	                                   untilDate:[NSDate distantPast]
	                                      inMode:NSDefaultRunLoopMode
	                                     dequeue:YES])) {
		[m_app sendEvent:event];
	}
	[m_app updateWindows];
}

void CocoaWindowDriver::beep() {
	NSBeep();
}

u32 CocoaWindowDriver::get_window_count() const {
	return static_cast<u32>(m_windows.size());
}

WindowID CocoaWindowDriver::create_window(const std::string& title, u32 width, u32 height) {
	NOVA_AUTO_TRACE();

	NSRect frame = NSMakeRect(100, 100, width, height);
	NSWindowStyleMask style = NSWindowStyleMaskTitled |
	                          NSWindowStyleMaskClosable |
	                          NSWindowStyleMaskResizable;

	NSWindow* ns_window = [[NSWindow alloc] initWithContentRect:frame
	                                                  styleMask:style
	                                                    backing:NSBackingStoreBuffered
	                                                      defer:NO];
	[ns_window setTitle:[NSString stringWithUTF8String:title.c_str()]];
	[ns_window setDelegate:(id<NSWindowDelegate>)m_app.delegate];
	[ns_window makeKeyAndOrderFront:nil];

	Window* win = new Window();
	win->width = width;
	win->height = height;
	win->handle = ns_window;

	m_windows[ns_window] = win;
	return win;
}

void CocoaWindowDriver::destroy_window(WindowID p_window) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_window);

	NSWindow* ns_window = static_cast<NSWindow*>(p_window->handle);
	auto it = m_windows.find(ns_window);
	if (it != m_windows.end()) {
		[ns_window close];
		m_windows.erase(it);
		delete p_window;
	}
}

void CocoaWindowDriver::set_window_title(WindowID p_window, const std::string& title) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_window);

	NSWindow* ns_window = static_cast<NSWindow*>(p_window->handle);
	[ns_window setTitle:[NSString stringWithUTF8String:title.c_str()]];
}

void CocoaWindowDriver::set_window_size(WindowID p_window, u32 width, u32 height) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_window);

	NSWindow* ns_window = static_cast<NSWindow*>(p_window->handle);
	NSRect frame = [ns_window frame];
	frame.size = NSMakeSize(width, height);
	[ns_window setFrame:frame display:YES];
}

void CocoaWindowDriver::set_window_position(WindowID p_window, i32 x, i32 y) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_window);

	NSWindow* ns_window = static_cast<NSWindow*>(p_window->handle);
	NSRect frame = [ns_window frame];
	frame.origin = NSMakePoint(x, y);
	[ns_window setFrameOrigin:frame.origin];
}

const char* CocoaWindowDriver::get_surface_extension() const {
#ifdef NOVA_VULKAN
	return VK_MVK_MACOS_SURFACE_EXTENSION_NAME;
#else
	return nullptr;
#endif
}

SurfaceID CocoaWindowDriver::create_surface(WindowID p_window, RenderDriver* p_driver) {
	NOVA_AUTO_TRACE();
	NOVA_ASSERT(p_window);
	NOVA_ASSERT(p_driver);
	NOVA_ASSERT(p_driver->get_api() == RenderAPI::VULKAN);

#ifdef NOVA_VULKAN
	NSWindow* ns_window = static_cast<NSWindow*>(p_window->handle);

	VkMacOSSurfaceCreateInfoMVK createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
	createInfo.pView = (__bridge void*)[ns_window contentView];

	auto* vkrd = static_cast<VulkanRenderDriver*>(p_driver);
	Surface* surface = new Surface();

	if (vkCreateMacOSSurfaceMVK(vkrd->get_instance(), &createInfo,
	                            vkrd->get_allocator(VK_OBJECT_TYPE_SURFACE_KHR),
	                            &surface->handle) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create macOS Vulkan surface");
	}
	return surface;
#else
	return nullptr;
#endif
}

void CocoaWindowDriver::_on_window_close(NSWindow* window) {
	auto it = m_windows.find(window);
	if (it != m_windows.end()) {
		WindowID win = it->second;
		NOVA_DEBUG("Window event: CLOSED");
		delete win;
		m_windows.erase(it);
	}
	if (m_windows.empty()) {
		[m_app terminate:nil];
	}
}

#endif // NOVA_MACOS
