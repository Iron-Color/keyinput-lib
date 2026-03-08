# KeyInputLib

Linux / Raspberry Pi 環境で `input event` デバイスを読み取り、  
キーボード入力状態を扱うための C++17 ライブラリです。

`/dev/input/by-id/` からキーボード候補を列挙し、選択したデバイスのキー入力を取得できます。  
現在は Linux の `input_event` を利用しており、主に Raspberry Pi 上での利用を想定しています。

## Features

- Linux の `input event` デバイスに対応
- `/dev/input/by-id/` からキーボード候補を列挙可能
- `DeviceInfo` で表示名と実体パスを一括管理
- 押下中 / 押した瞬間 / 離した瞬間 を判定可能
- 現在開いているデバイス情報を取得可能
- `ClearKeyStates()` による状態リセットに対応
- `examples/` にコンソール用のサンプルアプリを同梱
- CMake によるビルド / install / `find_package` に対応

## Supported Environment

# KeyInputLib

軽量な C++17 ライブラリで、Linux（主に Raspberry Pi）上の `input event` デバイスからキーボード入力を取得・管理します。

主な用途: /dev/input/by-id/ にある候補からキーボードを選択し、キーの押下状態や押した／離した瞬間を扱うユーティリティを提供します。

---

## 特長

- Linux の `input event` デバイスに対応
- `/dev/input/by-id/` からキーボード候補を列挙
- `DeviceInfo` で表示名と実体パスを管理
- 押下中（pressing）、押した瞬間（pressed）、離した瞬間（released）の判定が可能
- 現在開いているデバイス情報の取得、状態リセットに対応
- CMake によるビルド / install / `find_package` に対応

---

## 動作環境

- Linux（Raspberry Pi を想定）
- C++17 以上
- `linux/input.h` が利用可能であること

※ 入力デバイスへ直接アクセスするため、実行に適切な権限が必要です（sudo 実行等）。

---

## クイックスタート

ビルド手順:

```bash
mkdir build
cd build
cmake ..
make
```

実行（権限が必要な場合）:

```bash
sudo ./keyinput_example
```

インストール:

```bash
mkdir build
cd build
cmake ..
make
sudo make install
```

インストール先の例:

- ライブラリ: `/usr/local/lib`
- ヘッダ: `/usr/local/include/KeyInputLib`
- CMake パッケージ: `/usr/local/lib/cmake/KeyInputLib`

---

## 使い方（簡単なサンプル）

デバイスを列挙して最初の候補を開き、ループで状態を更新する例です。

```cpp
#include <KeyInputLib/DeviceInfo.hpp>
#include <KeyInputLib/KeyInput.hpp>
#include <KeyInputLib/KeyboardDeviceFinder.hpp>

#include <chrono>
#include <iostream>
#include <linux/input.h>
#include <thread>
#include <vector>

int main()
{
    auto devices = KeyboardDeviceFinder::FindKeyboardDevices();
    if (devices.empty()) return 1;

    KeyInput keyInput(devices[0]);
    while (true) {
        keyInput.Update();
        if (keyInput.WasKeyPressed(KEY_ESC)) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return 0;
}
```

---

## 公開 API（概要）

- `DeviceInfo` — デバイスの表示名と実体パスを保持（[include/DeviceInfo.hpp](include/DeviceInfo.hpp#L1)）
- `KeyboardDeviceFinder` — 入力デバイス列挙用ユーティリティ（[include/KeyboardDeviceFinder.hpp](include/KeyboardDeviceFinder.hpp#L1)）
- `KeyInput` — 指定デバイスを開いてキー状態を管理（[include/KeyInput.hpp](include/KeyInput.hpp#L1)）

各クラスの主なメソッドはソース内のヘッダを参照してください。

---

## 例（examples）

`examples/` 配下にコンソール向けのデバイス選択とサンプル実装があります。実行例は上記の通りです。

---

## 注意点

- 本ライブラリは Linux 固有です。
- `/dev/input/eventX` に直接アクセスするため、実行権限や device ノードのアクセス許可に注意してください。
- `KeyInput::Update()` は定期的に呼び出して状態を更新する必要があります。

---

## 今後の改善案

- イベントキュー方式の API
- 複数デバイスの同時監視対応
- 共有ライブラリ（.so）のビルドオプション
- 自動化テストの追加

---

## ライセンス

MIT License（詳細は [License](License#L1) を参照）

---

作成者: Iron_color
