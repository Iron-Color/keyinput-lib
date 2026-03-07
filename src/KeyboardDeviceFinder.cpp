#include "KeyboardDeviceFinder.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <string>
#include <utility>
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
    std::vector<std::pair<std::string, std::string>> EnumerateDevices(bool keyboardOnly)
    {
        std::vector<std::pair<std::string, std::string>> devices;
        const fs::path inputByIdPath = "/dev/input/by-id/";

        // 入力デバイスのシンボリックリンク置き場が無い環境では空で返します。
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
                // by-id はシンボリックリンクなので、実体パスへ解決して保持します。
                const std::string realPath = fs::canonical(entry.path()).string();
                devices.emplace_back(fileName, realPath);
            }
            catch (const fs::filesystem_error&)
            {
                // 解決できないリンクやアクセスできない項目は無視します。
            }
        }

        return devices;
    }
}

std::vector<std::pair<std::string, std::string>> KeyboardDeviceFinder::FindKeyboardDevices()
{
    return EnumerateDevices(true);
}

std::vector<std::pair<std::string, std::string>> KeyboardDeviceFinder::FindAllInputDevices()
{
    return EnumerateDevices(false);
}