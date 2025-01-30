#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/fmt.h"


// Logging class - This is essentially a wrapper for spdlog include
class Log
{
public:
	enum class Level {
		Info,
		Warn,
		Error,
		Trace
	};

	struct LogMessage {
		Level level;
		std::string message;
	};


	static Log& Get() {
		static Log instance;
		return instance;
	}
	static void Init();
	inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_AppLogger; }

	template<typename... Args>
	void Push(Level level, const std::string& fmt, Args&&... args) {
		// std::lock_guard<std::mutex> lock(m_LogMutex); // For THread safety. Only single threaded so no need.
		m_Logs.push_back({ level, fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...) });
	}
	const std::vector<LogMessage>& GetLogs() const { return m_Logs; }
	void ClearLogs() { m_Logs.clear(); }


private:
	Log() = default;
	~Log() = default;

	static std::shared_ptr<spdlog::logger> s_AppLogger;

private:
	std::vector<LogMessage> m_Logs;
};


