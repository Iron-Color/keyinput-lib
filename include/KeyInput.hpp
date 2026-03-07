#pragma once

#include <string>
#include <vector>

/// Linux の input event デバイスを読み取り、キー状態を保持するクラスです。
class KeyInput
{
public:
    /// デバイス未接続の状態で初期化します。
    KeyInput();

    /// 指定デバイスを開いた状態で初期化します。
    explicit KeyInput(const std::string& devicePath);

    /// 開いているデバイスを閉じます。
    ~KeyInput();

    // ファイルディスクリプタを単独所有したいためコピーは禁止にします。
    KeyInput(const KeyInput&) = delete;
    KeyInput& operator=(const KeyInput&) = delete;

    // 所有権の移動は許可します。
    KeyInput(KeyInput&& other) noexcept;
    KeyInput& operator=(KeyInput&& other) noexcept;

    /// デバイスを開きます。
    /// 開けた場合は true、失敗時は false を返します。
    bool OpenDevice(const std::string& devicePath);

    /// 現在開いているデバイスを閉じます。
    void CloseDevice();

    /// デバイスから入力イベントを読み取り、キー状態を更新します。
    void Update();

    /// デバイスが開かれているかを返します。
    bool IsOpen() const;

    /// 現在そのキーが押下中かを返します。
    bool IsKeyPressed(int keycode) const;

    /// 前回 Update 時には押されておらず、今回押されたかを返します。
    bool WasKeyPressed(int keycode) const;

    /// 前回 Update 時には押されており、今回離されたかを返します。
    bool WasKeyReleased(int keycode) const;

private:
    /// 有効なキーコード範囲かを判定します。
    bool IsValidKeyCode(int keycode) const;

private:
    /// 入力デバイスのファイルディスクリプタです。
    int fd;

    /// 現在フレーム時点のキー状態です。
    std::vector<bool> currentKeyStates;

    /// 1 回前の Update 時点のキー状態です。
    std::vector<bool> previousKeyStates;
};