#include <KeyInputLib/KeyInput.hpp>

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <linux/input.h>
#include <optional>
#include <stdexcept>
#include <string>
#include <unistd.h>

KeyInput::KeyInput()
    : fd(-1),
      openedDevice(std::nullopt),
      currentKeyStates(KEY_MAX, false),
      previousKeyStates(KEY_MAX, false)
{
}

KeyInput::KeyInput(const DeviceInfo& deviceInfo)
    : fd(-1),
      openedDevice(std::nullopt),
      currentKeyStates(KEY_MAX, false),
      previousKeyStates(KEY_MAX, false)
{
    // 生成時にすぐ利用したいケース向けに、指定デバイスをそのまま開きます。
    if (OpenDevice(deviceInfo) == false)
    {
        throw std::runtime_error(
            "デバイスオープン失敗: " + deviceInfo.devicePath + " : " + std::strerror(errno));
    }
}

KeyInput::KeyInput(const std::string& devicePath)
    : fd(-1),
      openedDevice(std::nullopt),
      currentKeyStates(KEY_MAX, false),
      previousKeyStates(KEY_MAX, false)
{
    // パスのみ指定された場合は、表示名にパスをそのまま入れて保持します。
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
      openedDevice(std::move(other.openedDevice)),
      currentKeyStates(std::move(other.currentKeyStates)),
      previousKeyStates(std::move(other.previousKeyStates))
{
    // ムーブ元は未保持状態に戻します。
    other.fd = -1;
    other.openedDevice = std::nullopt;
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
    openedDevice = std::move(other.openedDevice);
    currentKeyStates = std::move(other.currentKeyStates);
    previousKeyStates = std::move(other.previousKeyStates);

    other.fd = -1;
    other.openedDevice = std::nullopt;

    return *this;
}

bool KeyInput::OpenDevice(const DeviceInfo& deviceInfo)
{
    // 既に別デバイスを開いている場合に備えて、先に閉じてから開き直します。
    CloseDevice();

    fd = open(deviceInfo.devicePath.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0)
    {
        return false;
    }

    openedDevice = deviceInfo;

    // 新しいデバイスに切り替わるため、前回までの状態は初期化します。
    ClearKeyStates();

    return true;
}

bool KeyInput::OpenDevice(const std::string& devicePath)
{
    DeviceInfo deviceInfo;
    deviceInfo.displayName = devicePath;
    deviceInfo.devicePath = devicePath;

    return OpenDevice(deviceInfo);
}

void KeyInput::CloseDevice()
{
    if (fd >= 0)
    {
        close(fd);
        fd = -1;
    }

    openedDevice = std::nullopt;

    // デバイス未接続状態に合わせてキー状態も初期化します。
    ClearKeyStates();
}

void KeyInput::ClearKeyStates()
{
    currentKeyStates.assign(KEY_MAX, false);
    previousKeyStates.assign(KEY_MAX, false);
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

bool KeyInput::HasOpenedDevice() const
{
    return openedDevice.has_value();
}

const std::optional<DeviceInfo>& KeyInput::GetOpenedDevice() const
{
    return openedDevice;
}

std::string KeyInput::GetOpenedDeviceName() const
{
    if (openedDevice.has_value() == false)
    {
        return "";
    }

    return openedDevice->displayName;
}

std::string KeyInput::GetOpenedDevicePath() const
{
    if (openedDevice.has_value() == false)
    {
        return "";
    }

    return openedDevice->devicePath;
}

std::string KeyInput::GetOpenedDeviceDisplayText() const
{
    if (openedDevice.has_value() == false)
    {
        return "";
    }

    if (openedDevice->displayName.empty() == true)
    {
        return openedDevice->devicePath;
    }

    return openedDevice->displayName + " -> " + openedDevice->devicePath;
}

bool KeyInput::IsOpenedDevice(const std::string& devicePath) const
{
    if (openedDevice.has_value() == false)
    {
        return false;
    }

    return openedDevice->devicePath == devicePath;
}

bool KeyInput::IsOpenedDevice(const DeviceInfo& deviceInfo) const
{
    if (openedDevice.has_value() == false)
    {
        return false;
    }

    return openedDevice->devicePath == deviceInfo.devicePath;
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