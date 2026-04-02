#pragma once

#include <string>

/// 入力デバイスの表示名と実体パスを表します。
struct DeviceInfo
{
    /// /dev/input/by-id/ 上で見える名前です。
    std::string displayName;

    /// /dev/input/eventX などの実体パスです。
    std::string devicePath;
};
