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

## 正常なCGROMでの再検証と修正（2026-10-18）

- 新CGROM.DAT：786,432バイト、MD5 `cb0a5cfcf7247a7eab74bb2716260269`。IPLROM.DATと合わせ、PX68k公式資料の値と一致。
- 修正前は正常なCGROMでも同じCPU停止を再現したため、前回のCGROM不足はCPU停止の直接原因ではなかった。
- 一時的なDebug命令追跡で、IPLの `$FF0236` から始まるROM起動処理が、SRAM `$ED000C` のダミーハンドル `$BFFFFC` を参照して `$000000` へ飛び、最後にCPUがfault状態になることを特定した。
- この挙動は [STUDIO KAMADAのROM 1.0 / 12MBに関する技術資料](https://www.stdkmd.net/bugsx68k/) と一致する。不要なROM起動呼び出しを、既知の命令列とダミー／未初期化ハンドルの場合に限ってメモリ上でNOPへ置換する互換処理を追加した。カスタムROM起動先では元の命令に戻す。ROMファイルは変更しない。
- 一時的なCPUトレースの変更は撤去済み。CPUコア自体の変更はない。
- 回帰テストを含む既存コアテスト8種類がASan/UBSan付きですべて成功。別ROM・カスタム起動先・未初期化SRAM・ミラー・再適用・復元・範囲外も検証した。
- Debug/Releaseビルド成功。完成Release版は正常なROM内アドレスで単命令実行が進み、従来の `$000154` 停止を再現しなかった。
- 自作の1,261,568バイトのXDF診断ディスクでFDD起動を確認。IPLがロードしたコードのPCは `$00202A`、コードがRAM `$003000` に書いたマーカーは `$5836384B`。OS・ゲーム・第三者のブートコードは診断ディスクに含まない。確認後はFDDから取り出した。
- OS・市販ゲーム・HDD・音声・コントローラーの互換性は引き続き未検証。
- 修正版を日付フォルダのアプリとZIPへ反映。元のアプリはローカルの `build/verification/BeforeBootFix.app` に保存。

追加ログと自作診断ディスクはGit管理対象外の `build/verification/rom-retest/` に保存。
