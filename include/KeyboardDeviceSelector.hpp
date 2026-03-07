#pragma once

#include <string>
#include <utility>
#include <vector>

/// コンソール上で使用デバイスを選択するクラスです。
class KeyboardDeviceSelector
{
public:
    /// 一覧を表示し、選択されたインデックスを返します。
    /// 選択できない場合は -1 を返します。
    static int SelectDeviceIndex(const std::vector<std::pair<std::string, std::string>>& devices);
};