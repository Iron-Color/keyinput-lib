#pragma once

#include <KeyInputLib/DeviceInfo.hpp>

#include <vector>

/// キーボード候補デバイスや入力デバイスを列挙するクラスです。
class KeyboardDeviceFinder
{
public:
    /// /dev/input/by-id/ からキーボードらしいデバイスを列挙します。
    static std::vector<DeviceInfo> FindKeyboardDevices();

    /// /dev/input/by-id/ 配下の入力デバイスをすべて列挙します。
    static std::vector<DeviceInfo> FindAllInputDevices();
};
