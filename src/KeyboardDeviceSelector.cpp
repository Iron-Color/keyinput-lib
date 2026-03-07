#include "KeyboardDeviceSelector.hpp"

#include <iostream>
#include <limits>
#include <utility>
#include <vector>

int KeyboardDeviceSelector::SelectDeviceIndex(const std::vector<std::pair<std::string, std::string>>& devices)
{
    if (devices.empty())
    {
        return -1;
    }

    // 候補一覧を表示して、利用者に選択してもらいます。
    std::cout << "利用可能な入力デバイス一覧\n";

    for (size_t index = 0; index < devices.size(); ++index)
    {
        std::cout << index << ": " << devices[index].first << " -> " << devices[index].second << "\n";
    }

    int selectedIndex = -1;

    while (true)
    {
        std::cout << "使用する番号を入力してください: ";
        std::cin >> selectedIndex;

        // 数値以外が入力された場合は入力状態を回復して再試行します。
        if (std::cin.fail() == true)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "数値を入力してください。\n";
            continue;
        }

        // 範囲外の番号は受け付けません。
        if (selectedIndex < 0 || selectedIndex >= static_cast<int>(devices.size()))
        {
            std::cout << "範囲外です。再入力してください。\n";
            continue;
        }

        return selectedIndex;
    }
}