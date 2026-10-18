# ローカルビルドの検証記録

検証日：2026-10-18（日本時間）

- 土台：YosAwed/MPX68K、コミット `0498eaecc2060a617a0aaa7c4cd205e403027f95`（4.6 / build 940）。
- 環境：Apple Silicon、macOS 27.0.1、Xcode 27.1（27A9275）。
- Debug：ビルド成功。Releaseとは別のDerivedDataで検証。
- Release：`./tools/build-local.sh Release` が成功。
- 既存コアテスト：`make -C tests/core` の7実行ファイルがすべて成功。AddressSanitizer / UndefinedBehaviorSanitizerを有効にして実行。
- 署名：アドホック署名。`codesign --verify --deep --strict --verbose=2` が成功。
- 配布ZIP：`unzip -t` が成功。
- 起動：Debug版と完成Release版の起動完了、メインウィンドウ、日本語のROM選択ウィンドウをプロセス情報とウィンドウ一覧で確認。完成版は日付フォルダ直下の `MPX68K.app` から起動。
- UI操作の自動検証はアクセシビリティ権限がないため未実施。使い方メニューのクリック、ROM取り込み、FDD/HDD読み込み、入力、画面回転の実操作は未検証。
- ROM・起動ディスクは未提供のため、X68000のOS起動・ゲーム・音声・互換性は未検証。ROMやディスクイメージをリポジトリへ追加していない。
- Developer ID署名・公証・Intel版ビルドは対象外。

ビルド中にXcodeからiOSシミュレーター関連の警告が出たが、macOSのDebug/Releaseビルドは成功した。

生ログはGit管理対象外の `build/verification/` に保存。

## ROMを配置した後の追加確認（2026-10-18）

- IPLROM.DAT：131,072バイト、MD5 `7fd4caabac1d9169e289f0f7bbf71d8e`。[PX68k公式資料](https://docs.libretro.com/library/px68k/#bios)の値と一致。
- CGROM.DAT：786,376バイト。標準の786,432バイトより56バイト短い。MD5 `df0226b29c1931299462746086c2df3c` は、公式資料の `cb0a5cfcf7247a7eab74bb2716260269` と不一致。別フォントの可能性もあるが、短いサイズと合わせて正常なファイルでの再確認が必要。
- 完成版はROM選択を要求せず、起動ディスクの選択画面へ進んだ。CGROMの読み込み処理は最大サイズ以下を許容するため、サイズが短くてもエラーにはならない。
- 診断スナップショットでフレームカウンターが増加することを確認したが、CPUのPCは `0x000154` で固定され、単命令実行でも進まなかった。Debug版でも同じ状態を再現。フレーム更新をCPUの正常動作とは判定していない。
- FDD/HDDは未マウント。OS・ゲーム・音声の動作確認はできていない。
- CGROMの不足がCPU停止の直接原因かは未確定。まず正常なCGROMで再確認し、それでも停止する場合はアプリ側を調査する。
- ROMファイルには変更を加えていない。ROMおよびSRAMはGitHubへ追加していない。

追加のローカル診断ログと画面画像は `build/verification/romcheck/` に保存。
