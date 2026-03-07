#pragma once

#include <string>
#include <utility>
#include <vector>

/// キーボード候補デバイスや入力デバイスを列挙するクラスです。
class KeyboardDeviceFinder
{
public:
    /// /dev/input/by-id/ からキーボードらしいデバイスを列挙します。
    static std::vector<std::pair<std::string, std::string>> FindKeyboardDevices();

    /// /dev/input/by-id/ 配下の入力デバイスをすべて列挙します。
    static std::vector<std::pair<std::string, std::string>> FindAllInputDevices();
};