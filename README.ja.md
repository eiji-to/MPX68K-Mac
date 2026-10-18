# MPX68K Mac用ローカルビルド

X68000エミュレーター [YosAwed/MPX68K](https://github.com/YosAwed/MPX68K) を土台にした、Apple Silicon Mac向けのビルドです。元のエミュレーション機能を使用し、ROM選択の日本語案内とアプリ内の使い方を追加しました。

## 起動

1. `build/dist/MPX68K.app` を開きます。
2. 最初の選択画面で `IPLROM.DAT` と `CGROM.DAT` を選択します。Commandキーで複数選択できます。選択したROMは「書類/MPX68K」へコピーされます。
3. 起動ディスクを選択します。フロッピーの場合は通常 FDD 0 を選びます。
4. FDDメニューの `Open Drive 0…` / `Open Drive 1…` でディスクを交換できます。`.dim` / `.xdf` / `.2hd` / `.d88` / `.hdm` を扱えます。HDDはHDDメニューから選択します。

ROMとディスクイメージは同梱していません。使用できるファイルを別途用意してください。OSやゲームの互換性確認には、それらのファイルが必要です。

## 操作

- 再起動：System → Reset System
- 画面回転：Command + R
- X68000マウス切り替え：Shift + Command + M
- ジョイパッド：方向キー / WASD、A＝Space / J、B＝Z / K
- 使い方・データフォルダ：Help → MPX68Kの使い方…

ROM・SRAM（本体設定）の保存先は `~/Documents/MPX68K/` です。HDDイメージは書き換えられる場合があるため、手元の原本のコピーを使用してください。

## 再ビルド

XcodeをインストールしたApple Silicon Macで、リポジトリ直下から実行します。

```sh
./tools/build-local.sh Debug
./tools/build-local.sh Release
make -C tests/core
```

初回のReleaseビルドではCPUコアの最適化に時間がかかります。

ReleaseスクリプトはアプリとZIPを `build/dist/` に作成し、ローカル利用向けのアドホック署名を付けて署名を検証します。Developer ID署名・Appleの公証は行っていません。Intel版はこのスクリプトの対象外です。

## 出典とライセンス

土台：MPX68K 4.6 (build 940)、元コミット `0498eae`。CPU・音源などの各構成要素の出典と条件は [LICENSE](LICENSE)、[ATTRIBUTION.md](ATTRIBUTION.md)、[LICENSES](LICENSES) とソース内の著作権表示を参照してください。バイナリを配布する場合は、対応するソースも提供し、各構成要素の条件に従ってください。元のREADMEは [README.md](README.md) に残しています。

## IPL ROM 1.0のディスクなし起動

12MBメモリでIPL ROM 1.0を使うと、未使用のROM起動先をRAM内のアドレスと誤認して停止する既知の問題があります。本ビルドは該当する命令列とダミー起動先（または未初期化設定）が一致する場合だけ、メモリ上のIPLイメージで不要な呼び出しをスキップします。実際の拡張ROM起動先や別の命令列は保持します。ディスク上のROM・SRAMファイルをパッチする処理はありません。

新しいCGROMで、ディスク待ちの継続と自作診断フロッピーからのプログラム実行を確認済みです。Human68kや市販ゲーム、HDD、音声は別途確認が必要です。詳細は [BUILD-NOTES.md](BUILD-NOTES.md) を参照してください。
