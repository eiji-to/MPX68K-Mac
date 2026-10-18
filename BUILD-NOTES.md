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
