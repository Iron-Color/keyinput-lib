#include "KeyInput.hpp"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <linux/input.h>
#include <stdexcept>
#include <string>
#include <unistd.h>

KeyInput::KeyInput()
    : fd(-1),
      currentKeyStates(KEY_MAX, false),
      previousKeyStates(KEY_MAX, false)
{
}

KeyInput::KeyInput(const std::string& devicePath)
    : fd(-1),
      currentKeyStates(KEY_MAX, false),
      previousKeyStates(KEY_MAX, false)
{
    // 生成時にすぐ利用したいケース向けに、指定パスをそのまま開きます。
    if (OpenDevice(devicePath) == false)
    {
        throw std::runtime_error(
            "デバイスオープン失敗: " + devicePath + " : " + std::strerror(errno));
    }
}

KeyInput::~KeyInput()
{
    CloseDevice();
}

KeyInput::KeyInput(KeyInput&& other) noexcept
    : fd(other.fd),
      currentKeyStates(std::move(other.currentKeyStates)),
      previousKeyStates(std::move(other.previousKeyStates))
{
    // ムーブ元はデバイス未保持状態に戻します。
    other.fd = -1;
}

KeyInput& KeyInput::operator=(KeyInput&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    // 既存のデバイスを先に閉じてから所有権を受け取ります。
    CloseDevice();

    fd = other.fd;
    currentKeyStates = std::move(other.currentKeyStates);
    previousKeyStates = std::move(other.previousKeyStates);

    other.fd = -1;

    return *this;
}

bool KeyInput::OpenDevice(const std::string& devicePath)
{
    // 既に別デバイスを開いている場合に備えて、先に閉じてから開き直します。
    CloseDevice();

    fd = open(devicePath.c_str(), O_RDONLY | O_NONBLOCK);
    return fd >= 0;
}

void KeyInput::CloseDevice()
{
    if (fd >= 0)
    {
        close(fd);
        fd = -1;
    }
}

void KeyInput::Update()
{
    if (fd < 0)
    {
        return;
    }

    // 前回状態を退避しておくことで、押した瞬間・離した瞬間を判定できます。
    previousKeyStates = currentKeyStates;

    input_event inputEvent;
    ssize_t readSize = 0;

    // 非ブロッキングで読める分だけイベントを読み切ります。
    while ((readSize = read(fd, &inputEvent, sizeof(inputEvent))) > 0)
    {
        // キーイベントのみを対象にして状態を更新します。
        if (inputEvent.type == EV_KEY && inputEvent.code < KEY_MAX)
        {
            currentKeyStates[inputEvent.code] = (inputEvent.value != 0);
        }
    }

    // EAGAIN は「今はこれ以上読めない」だけなので正常扱いです。
    if (readSize < 0 && errno != EAGAIN)
    {
        throw std::runtime_error(std::string("read error: ") + std::strerror(errno));
    }
}

bool KeyInput::IsOpen() const
{
    return fd >= 0;
}

bool KeyInput::IsKeyPressed(int keycode) const
{
    if (IsValidKeyCode(keycode) == false)
    {
        return false;
    }

    return currentKeyStates[keycode];
}

bool KeyInput::WasKeyPressed(int keycode) const
{
    if (IsValidKeyCode(keycode) == false)
    {
        return false;
    }

    // 前回は押されておらず、今回押されていれば押下開始です。
    if (previousKeyStates[keycode] == false && currentKeyStates[keycode] == true)
    {
        return true;
    }

    return false;
}

bool KeyInput::WasKeyReleased(int keycode) const
{
    if (IsValidKeyCode(keycode) == false)
    {
        return false;
    }

    // 前回は押されていて、今回離されていれば解放です。
    if (previousKeyStates[keycode] == true && currentKeyStates[keycode] == false)
    {
        return true;
    }

    return false;
}

bool KeyInput::IsValidKeyCode(int keycode) const
{
    if (keycode < 0)
    {
        return false;
    }

    if (keycode >= static_cast<int>(currentKeyStates.size()))
    {
        return false;
    }

    return true;
}