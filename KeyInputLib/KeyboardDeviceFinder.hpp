#pragma once

#include <KeyInputLib/DeviceInfo.hpp>

#include <vector>

/// @brief キーボード候補デバイスや入力デバイスを列挙するユーティリティです。
namespace KeyboardDeviceFinder
{
    /// @brief /dev/input/by-id/ からキーボードらしいデバイスを列挙します。
    /// @return キーボードと判定されたデバイスの一覧。該当なしや列挙失敗時は空のリスト。
    std::vector<DeviceInfo> FindKeyboardDevices();

    /// @brief /dev/input/by-id/ 配下の入力デバイスをすべて列挙します。
    /// @return 入力デバイスの一覧。列挙失敗時は空のリスト。
    std::vector<DeviceInfo> FindAllInputDevices();
}
