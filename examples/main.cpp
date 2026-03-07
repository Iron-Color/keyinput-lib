#include "KeyInput.hpp"
#include "KeyboardDeviceFinder.hpp"
#include "KeyboardDeviceSelector.hpp"

#include <chrono>
#include <iostream>
#include <linux/input.h>
#include <thread>
#include <utility>
#include <vector>

namespace
{
    /// 使用するキーボードデバイスの実体パスを決定します。
    std::string ResolveKeyboardDevicePath()
    {
        std::vector<std::pair<std::string, std::string>> keyboardDevices =
            KeyboardDeviceFinder::FindKeyboardDevices();

        if (keyboardDevices.empty() == false)
        {
            // キーボード候補が 1 件なら自動採用します。
            if (keyboardDevices.size() == 1)
            {
                std::cout << "キーボードを自動検出しました: "
                          << keyboardDevices[0].first
                          << " -> "
                          << keyboardDevices[0].second
                          << "\n";

                return keyboardDevices[0].second;
            }

            // 複数ある場合は利用者に選んでもらいます。
            const int selectedIndex = KeyboardDeviceSelector::SelectDeviceIndex(keyboardDevices);
            if (selectedIndex >= 0)
            {
                return keyboardDevices[selectedIndex].second;
            }

            return "";
        }

        // キーボード候補が判別できない場合は全入力デバイスから選ばせます。
        std::cout << "キーボード候補が見つからなかったため、入力デバイス全体から選択します。\n";

        std::vector<std::pair<std::string, std::string>> allDevices =
            KeyboardDeviceFinder::FindAllInputDevices();

        const int selectedIndex = KeyboardDeviceSelector::SelectDeviceIndex(allDevices);
        if (selectedIndex >= 0)
        {
            return allDevices[selectedIndex].second;
        }

        return "";
    }
}

int main()
{
    try
    {
        const std::string devicePath = ResolveKeyboardDevicePath();

        if (devicePath.empty() == true)
        {
            std::cerr << "使用するデバイスを決定できませんでした。\n";
            return 1;
        }

        KeyInput keyInput(devicePath);

        std::cout << "ESC キーで終了します。\n";

        while (true)
        {
            // 現在までに届いている入力イベントを取り込みます。
            keyInput.Update();

            // 押した瞬間だけ反応したい場合は WasKeyPressed を使います。
            if (keyInput.WasKeyPressed(KEY_A) == true)
            {
                std::cout << "Aキーが押されました\n";
            }

            // 離した瞬間だけ反応したい場合は WasKeyReleased を使います。
            if (keyInput.WasKeyReleased(KEY_A) == true)
            {
                std::cout << "Aキーが離されました\n";
            }

            // 押している間ずっと判定したい場合は IsKeyPressed を使います。
            if (keyInput.IsKeyPressed(KEY_LEFTSHIFT) == true)
            {
                std::cout << "Shift押下中\n";
            }

            if (keyInput.WasKeyPressed(KEY_ESC) == true)
            {
                std::cout << "終了します。\n";
                break;
            }

            // CPU を無駄に使いすぎないよう少し待ちます。
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    catch (const std::exception& exception)
    {
        std::cerr << "エラー: " << exception.what() << "\n";
        return 1;
    }

    return 0;
}