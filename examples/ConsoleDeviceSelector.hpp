#pragma once

#include <KeyInputLib/DeviceInfo.hpp>

#include <vector>

/// コンソール上で使用デバイスを選択する補助クラスです。
class ConsoleDeviceSelector
{
public:
    /// 一覧を表示し、選択されたインデックスを返します。
    /// 選択できない場合は -1 を返します。
    static int SelectDeviceIndex(const std::vector<DeviceInfo>& devices);
};
