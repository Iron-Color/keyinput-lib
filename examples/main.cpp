#include "ConsoleDeviceSelector.hpp"

#include <KeyInputLib/DeviceInfo.hpp>
#include <KeyInputLib/KeyInput.hpp>
#include <KeyInputLib/KeyboardDeviceFinder.hpp>

#include <chrono>
#include <iostream>
#include <linux/input.h>
#include <optional>
#include <thread>
#include <vector>

namespace
{
    /// 使用するキーボードデバイス情報を決定します。
    std::optional<DeviceInfo> ResolveKeyboardDevice()
    {
        std::vector<DeviceInfo> keyboardDevices = KeyboardDeviceFinder::FindKeyboardDevices();

        if (keyboardDevices.empty() == false)
        {
            // キーボード候補が1件なら自動採用します。
            if (keyboardDevices.size() == 1)
            {
                std::cout << "キーボードを自動検出しました: "
                          << keyboardDevices[0].displayName
                          << " -> "
                          << keyboardDevices[0].devicePath
                          << "\n";

                return keyboardDevices[0];
            }

            // 複数ある場合はサンプルアプリ側のコンソールUIで選択します。
            const int selectedIndex = ConsoleDeviceSelector::SelectDeviceIndex(keyboardDevices);
            if (selectedIndex >= 0)
            {
                return keyboardDevices[selectedIndex];
            }

            return std::nullopt;
        }

        // キーボード候補が判別できない場合は全入力デバイスから選ばせます。
        std::cout << "キーボード候補が見つからなかったため、入力デバイス全体から選択します。\n";

        std::vector<DeviceInfo> allDevices = KeyboardDeviceFinder::FindAllInputDevices();

        const int selectedIndex = ConsoleDeviceSelector::SelectDeviceIndex(allDevices);
        if (selectedIndex >= 0)
        {
            return allDevices[selectedIndex];
        }

        return std::nullopt;
    }
}

int main()
{
    try
    {
        const std::optional<DeviceInfo> selectedDevice = ResolveKeyboardDevice();

        if (selectedDevice.has_value() == false)
        {
            std::cerr << "使用するデバイスを決定できませんでした。\n";
            return 1;
        }

        KeyInput keyInput(selectedDevice.value());

        if (keyInput.HasOpenedDevice() == true)
        {
            std::cout << "使用中デバイス: "
                    << keyInput.GetOpenedDeviceDisplayText()
                    << "\n";
        }

        std::cout << "ESC キーで終了します。\n";

        while (true)
        {
            // 現在までに届いている入力イベントを取り込みます。
            keyInput.Update();

            if (keyInput.WasKeyPressed(KEY_A) == true)
            {
                std::cout << "Aキーが押されました\n";
            }

            if (keyInput.WasKeyReleased(KEY_A) == true)
            {
                std::cout << "Aキーが離されました\n";
            }

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
