#pragma once

#include <string>

/// @brief 入力デバイスの表示名と実体パスを表します。
struct DeviceInfo
{
    /// @brief /dev/input/by-id/ 上で見える名前です。
    std::string displayName;

    /// @brief /dev/input/eventX などの実体パスです。
    std::string devicePath;
};
