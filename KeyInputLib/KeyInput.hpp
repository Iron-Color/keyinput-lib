#pragma once

#include <KeyInputLib/DeviceInfo.hpp>

#include <optional>
#include <string>
#include <vector>

/// @brief Linux の input event デバイスを読み取り、キー状態を保持するクラスです。
class KeyInput
{
public:
    /// @brief デバイス未接続の状態で初期化します。
    KeyInput();

    /// @brief 指定デバイスを開いた状態で初期化します。
    /// @param deviceInfo 開くデバイスの情報。
    /// @throws std::runtime_error デバイスを開けなかった場合。
    explicit KeyInput(const DeviceInfo& deviceInfo);

    /// @brief 指定パスのデバイスを開いた状態で初期化します。
    /// @param devicePath デバイスのパス（例: /dev/input/event0）。
    /// @throws std::runtime_error デバイスを開けなかった場合。
    explicit KeyInput(const std::string& devicePath);

    /// @brief 開いているデバイスを閉じます。
    ~KeyInput();

    // ファイルディスクリプタを単独所有したいためコピーは禁止にします。
    KeyInput(const KeyInput&) = delete;
    KeyInput& operator=(const KeyInput&) = delete;

    // 所有権の移動は許可します。
    KeyInput(KeyInput&& other) noexcept;
    KeyInput& operator=(KeyInput&& other) noexcept;

    /// @brief デバイス情報を指定してデバイスを開きます。
    /// @param deviceInfo 開くデバイスの情報。
    /// @return 成功した場合は true、失敗した場合は false。
    bool OpenDevice(const DeviceInfo& deviceInfo);

    /// @brief デバイスパスを指定してデバイスを開きます。
    /// @param devicePath デバイスのパス（例: /dev/input/event0）。
    /// @return 成功した場合は true、失敗した場合は false。
    bool OpenDevice(const std::string& devicePath);

    /// @brief 現在開いているデバイスを閉じます。
    void CloseDevice();

    /// @brief 現在保持しているキー状態を初期状態に戻します。
    void ClearKeyStates();

    /// @brief デバイスから入力イベントを読み取り、キー状態を更新します。
    /// @throws std::runtime_error デバイス読み取り中にエラーが発生した場合。
    void Update();

    /// @brief デバイスが開かれているかを返します。
    /// @return 開かれている場合は true、そうでない場合は false。
    bool IsOpen() const;

    /// @brief 現在開いているデバイス情報を保持しているかを返します。
    /// @return デバイス情報を保持している場合は true、そうでない場合は false。
    bool HasOpenedDevice() const;

    /// @brief 現在開いているデバイス情報を返します。
    /// @return デバイス情報。デバイス未接続時は std::nullopt。
    const std::optional<DeviceInfo>& GetOpenedDevice() const;

    /// @brief 現在開いているデバイスの表示名を返します。
    /// @return デバイスの表示名。デバイス未接続時は空文字。
    std::string GetOpenedDeviceName() const;

    /// @brief 現在開いているデバイスの実体パスを返します。
    /// @return デバイスの実体パス。デバイス未接続時は空文字。
    std::string GetOpenedDevicePath() const;

    /// @brief 現在開いているデバイスの表示用文字列を返します。
    /// @return "表示名 -> パス" 形式の文字列。デバイス未接続時は空文字。
    std::string GetOpenedDeviceDisplayText() const;

    /// @brief 指定パスのデバイスを現在開いているかを返します。
    /// @param devicePath 確認するデバイスのパス。
    /// @return 指定パスのデバイスを開いている場合は true、そうでない場合は false。
    bool IsOpenedDevice(const std::string& devicePath) const;

    /// @brief 指定デバイス情報のデバイスを現在開いているかを返します。
    /// @param deviceInfo 確認するデバイスの情報。
    /// @return 指定デバイスを開いている場合は true、そうでない場合は false。
    bool IsOpenedDevice(const DeviceInfo& deviceInfo) const;

    /// @brief 現在そのキーが押下中かを返します。
    /// @param keycode 確認するキーコード（linux/input.h の KEY_* 定数）。
    /// @return 押下中の場合は true、そうでない場合は false。
    bool IsKeyPressed(int keycode) const;

    /// @brief 前回 Update 時には押されておらず、今回押されたかを返します。
    /// @param keycode 確認するキーコード（linux/input.h の KEY_* 定数）。
    /// @return 今回押された場合は true、そうでない場合は false。
    bool WasKeyPressed(int keycode) const;

    /// @brief 前回 Update 時には押されており、今回離されたかを返します。
    /// @param keycode 確認するキーコード（linux/input.h の KEY_* 定数）。
    /// @return 今回離された場合は true、そうでない場合は false。
    bool WasKeyReleased(int keycode) const;

private:
    /// @brief 有効なキーコード範囲かを判定します。
    /// @param keycode 判定するキーコード。
    /// @return 有効な範囲の場合は true、そうでない場合は false。
    bool IsValidKeyCode(int keycode) const;

private:
    /// 入力デバイスのファイルディスクリプタです。
    int fd;

    /// 現在開いているデバイス情報です。
    std::optional<DeviceInfo> openedDevice;

    /// 現在フレーム時点のキー状態です。
    std::vector<bool> currentKeyStates;

    /// 1回前の Update 時点のキー状態です。
    std::vector<bool> previousKeyStates;
};
