#pragma once

#include "extern/expected/include/tl/expected.hpp"
#include <string>

namespace common
{
  template <typename T>
  using expected = tl::expected<T, std::string>;

  using unexpected = tl::unexpected<std::string>;

  template <typename F>
  auto expect(F&& func) noexcept -> expected<std::invoke_result_t<F>>
  {
    try
    {
        return std::invoke(std::forward<F>(func));
    }
    catch (std::exception& ex)
    {
        return unexpected(ex.what());
    }
  }
}