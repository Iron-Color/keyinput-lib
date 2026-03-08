#include <KeyInputLib/KeyboardDeviceFinder.hpp>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace
{
    /// ファイル名にキーボードを示す文字列が含まれているかを判定します。
    bool ContainsKeyboardKeyword(const std::string& deviceName)
    {
        std::string lowerName = deviceName;

        // 大文字小文字の違いを吸収するため、小文字化して判定します。
        std::transform(
            lowerName.begin(),
            lowerName.end(),
            lowerName.begin(),
            [](unsigned char character)
            {
                return static_cast<char>(std::tolower(character));
            });

        if (lowerName.find("kbd") != std::string::npos)
        {
            return true;
        }

        if (lowerName.find("keyboard") != std::string::npos)
        {
            return true;
        }

        return false;
    }

    /// /dev/input/by-id/ を列挙し、条件に合うデバイス一覧を返します。
    std::vector<DeviceInfo> EnumerateDevices(bool keyboardOnly)
    {
        std::vector<DeviceInfo> devices;
        const fs::path inputByIdPath = "/dev/input/by-id/";

        // 入力デバイス一覧が存在しない環境では空で返します。
        if (fs::exists(inputByIdPath) == false)
        {
            return devices;
        }

        if (fs::is_directory(inputByIdPath) == false)
        {
            return devices;
        }

        for (const auto& entry : fs::directory_iterator(inputByIdPath))
        {
            const std::string fileName = entry.path().filename().string();

            // キーボードのみ欲しい場合は候補外を除外します。
            if (keyboardOnly == true && ContainsKeyboardKeyword(fileName) == false)
            {
                continue;
            }

            try
            {
                DeviceInfo deviceInfo;
                deviceInfo.displayName = fileName;
                deviceInfo.devicePath = fs::canonical(entry.path()).string();

                devices.push_back(deviceInfo);
            }
            catch (const fs::filesystem_error&)
            {
                // 解決できないリンクやアクセスできない項目は無視します。
            }
        }

        return devices;
    }
}

std::vector<DeviceInfo> KeyboardDeviceFinder::FindKeyboardDevices()
{
    return EnumerateDevices(true);
}

std::vector<DeviceInfo> KeyboardDeviceFinder::FindAllInputDevices()
{
    return EnumerateDevices(false);
}
