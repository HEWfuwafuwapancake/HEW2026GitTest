# HEW2026 チーム制作 — Claude向けのプロジェクトメモ

DirectX 11（C++ / Visual Studio 2022）で作るゲームのチーム制作リポジトリ。
GitHub の Organization 上の public リポジトリで、git-flow（main / develop / feature / release / hotfix）で運用する。

## 進め方のルール
- ユーザーへの返答は日本語。ユーザーに示すコマンドは **PowerShell** 形式（コードブロックは `powershell`）。
- `.ps1` に日本語を書くときは **BOM付きUTF-8** で保存する（Windows PowerShell 5.1 対策）。

## git のルール（必ず守る）
- **`main` と `develop` には直接コミット・push しない。** 作業は必ず `feature/<作業名>` で行う（GitHub の Rulesets でも禁止されている）。
- git 操作はできるだけ `tools/` のスクリプトを使う：
  - 作業開始：`.\tools\start-work.ps1 <作業名>`（英小文字・数字・ハイフン）
  - 保存：`.\tools\save-work.ps1 "やったこと"`
  - レビュー依頼：`.\tools\submit-pr.ps1`
  - develop の最新を取ってくるだけ（作業は始めない）：`.\tools\fetch-latest.ps1`
- `git push --force`、`git reset --hard`、ブランチやタグの削除は、ユーザーに確認してから。
- develop / main へのマージ、release / hotfix、タグ付けは **管理者だけ**（`tools/admin/`）。ユーザーが管理者かどうか分からなければ聞く。
- 画像・モデル・音声は Git LFS（`.gitattributes`）。新しい種類の素材ファイルを入れるときは `.gitattributes` に追加する。

## ドキュメント
- `docs/team/01_管理者向け_環境構築.md` / `02_メンバー向け_環境構築.md` / `03_ブランチ運用ルール.md` / `04_コマンド一覧.md`
- スクリプトの動きを変えたり、新しいスクリプトを足したりしたら、これらのドキュメント（特に `04_コマンド一覧.md`）も合わせて直す。
