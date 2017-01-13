
#include "fmt/format.h"

/// Default TC string format function.
template<typename Format, typename... Args>
inline std::string StringFormat(Format&& fmt, Args&&... args)
{
	return fmt::sprintf(std::forward<Format>(fmt), std::forward<Args>(args)...);
}

/// Returns true if the given char pointer is null.
inline bool IsFormatEmptyOrNull(const char* fmt)
{
	return fmt == nullptr;
}

/// Returns true if the given std::string is empty.
inline bool IsFormatEmptyOrNull(std::string const& fmt)
{
	return fmt.empty();
}