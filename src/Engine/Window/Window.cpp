//
//	Created by rtryan98 on 21. Oct. 2020
//	Edited by MarcasRealAccount on 31. Oct. 2020
//

#include "Engine/Window/Window.h"
#include "Engine/Utility/Logger.h"

#include "Engine/Events/EventHandler.h"
#include "Engine/Events/KeyboardEvent.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/WindowEvent.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace gp1::window
{
	Window::Window(const WindowData& p_WindowData)
	    : m_WindowData { p_WindowData }, m_Logger { "Window" } {}

	void Window::Init()
	{
		if (!glfwInit())
		{
			m_Logger.Log(Severity::Error, "Failed to initialize GLFW!");
		}

		glfwDefaultWindowHints();
		for (auto& windowHint : m_WindowHints)
			glfwWindowHint(windowHint.first, windowHint.second);

		GLFWmonitor*       monitor { glfwGetPrimaryMonitor() };
		const GLFWvidmode* mode { glfwGetVideoMode(monitor) };

		switch (m_WindowData.Mode)
		{
		case WindowMode::FULLSCREEN:
			m_NativeHandle = glfwCreateWindow(mode->width, mode->height, m_WindowData.Title.c_str(), monitor, nullptr);
			break;
		case WindowMode::BORDERLESS:
			// while following the official guide, this is not the completely desired effect of a borderless window
			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
			m_NativeHandle = glfwCreateWindow(mode->width, mode->height, m_WindowData.Title.c_str(), monitor, nullptr);
			break;
		default:
			m_NativeHandle = glfwCreateWindow(m_WindowData.Width, m_WindowData.Height, m_WindowData.Title.c_str(), nullptr, nullptr);
			break;
		}

		glfwSetWindowUserPointer(m_NativeHandle, &m_WindowData);

		glfwSetWindowSizeCallback(m_NativeHandle, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			data.Width       = width;
			data.Height      = height;
			events::window::WindowResizeEvent event { width, height };
			events::EventHandler::PushEvent(event);
		});

		glfwSetFramebufferSizeCallback(m_NativeHandle, [](GLFWwindow* window, int width, int height) {
			WindowData& data       = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			data.FramebufferWidth  = width;
			data.FramebufferHeight = height;
		});

		glfwSetKeyCallback(m_NativeHandle, [](GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
			[[maybe_unused]] WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			switch (action)
			{
			case GLFW_PRESS:
			{
				events::keyboard::KeyPressedEvent event { key, false };
				events::EventHandler::PushEvent(event);
				break;
			}
			case GLFW_RELEASE:
			{
				events::keyboard::KeyReleasedEvent event { key };
				events::EventHandler::PushEvent(event);
				break;
			}
			case GLFW_REPEAT:
			{
				events::keyboard::KeyPressedEvent event { key, true };
				events::EventHandler::PushEvent(event);
				break;
			}
			default:
			{
				break;
			}
			}
		});

		glfwSetMouseButtonCallback(m_NativeHandle, [](GLFWwindow* window, int button, int action, [[maybe_unused]] int mods) {
			[[maybe_unused]] WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			switch (action)
			{
			case GLFW_PRESS:
			{
				events::mouse::MouseButtonPressedEvent event { button };
				events::EventHandler::PushEvent(event);
				break;
			}
			case GLFW_RELEASE:
			{
				events::mouse::MouseButtonReleasedEvent event { button };
				events::EventHandler::PushEvent(event);
				break;
			}
			default:
			{
				break;
			}
			}
		});

		glfwSetCursorPosCallback(m_NativeHandle, [](GLFWwindow* window, double xPos, double yPos) {
			[[maybe_unused]] WindowData&   data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			events::mouse::MouseMovedEvent event { xPos, yPos };
			events::EventHandler::PushEvent(event);
		});

		glfwSetScrollCallback(m_NativeHandle, [](GLFWwindow* window, double x, double y) {
			[[maybe_unused]] WindowData&    data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			events::mouse::MouseScrollEvent event { x, y };
			events::EventHandler::PushEvent(event);
		});

		m_Logger.Log(Severity::Trace, "Window was created successfully.");
	}

	void Window::DeInit()
	{
		glfwDestroyWindow(m_NativeHandle);
		glfwTerminate();
		m_Logger.Log(Severity::Trace, "Window terminated.");
	}

	void Window::OnUpdate()
	{
		glfwPollEvents();
	}

	void Window::SetVSync(const bool p_VSync)
	{
		m_WindowData.VSync = p_VSync;
		if (p_VSync)
		{
			glfwSwapInterval(1);
		}
		else
		{
			glfwSwapInterval(0);
		}
	}

	void Window::SetWidth(const int p_Width)
	{
		m_WindowData.Width = p_Width;
		glfwSetWindowSize(m_NativeHandle, m_WindowData.Width, m_WindowData.Height);
	}

	void Window::SetHeight(const int p_Height)
	{
		m_WindowData.Height = p_Height;
		glfwSetWindowSize(m_NativeHandle, m_WindowData.Width, m_WindowData.Height);
	}

	void Window::SetSize(const int p_Width, const int p_Height)
	{
		m_WindowData.Width  = p_Width;
		m_WindowData.Height = p_Height;
		glfwSetWindowSize(m_NativeHandle, m_WindowData.Width, m_WindowData.Height);
	}

	void Window::SetTitle(const std::string& p_Title)
	{
		m_WindowData.Title = p_Title;
		glfwSetWindowTitle(m_NativeHandle, m_WindowData.Title.c_str());
	}

	int Window::GetInputMode(int mode)
	{
		return glfwGetInputMode(m_NativeHandle, mode);
	}

	void Window::SetInputMode(int mode, int value)
	{
		glfwSetInputMode(m_NativeHandle, mode, value);
	}

	const WindowData& Window::GetWindowData() const
	{
		return m_WindowData;
	}

	bool Window::IsCloseRequested() const
	{
		return glfwWindowShouldClose(m_NativeHandle);
	}

	void Window::DefaultWindowHints()
	{
		m_WindowHints.clear();
	}

	void Window::SetWindowHint(int hint, int value)
	{
		m_WindowHints.insert_or_assign(hint, value);
	}

	GLFWwindow* Window::GetNativeHandle() const
	{
		return m_NativeHandle;
	}
} // namespace gp1::window
