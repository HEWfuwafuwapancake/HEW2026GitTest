# 作業を保存して GitHub に送るスクリプト（こまめに実行してOK）
# 使い方: .\tools\save-work.ps1 "やったこと"
param([Parameter(Position = 0)][string]$Message = "")

. "$PSScriptRoot\common.ps1"
Set-Location $RepoRoot
$cfg = Get-TeamConfig

$current = Get-CurrentBranch
if (-not $current) { Stop-Team "ブランチにいません（detached HEAD）。" "管理者に相談してください" }
if ($current -eq $cfg.mainBranch -or $current -eq $cfg.developBranch) {
    Stop-Team "$current には直接保存できません。" ".\tools\start-work.ps1 <作業名> を実行すると、今の変更ごとフィーチャーブランチに移れます"
}
if ($Message -eq "") { $Message = "作業の保存 " + (Get-Date -Format "yyyy-MM-dd HH:mm") }

git add -A

# 50MB を超えるファイルが LFS 以外で入ろうとしていないか（GitHub は 100MB 超を拒否する）
$big = @()
foreach ($f in (git diff --cached --name-only --diff-filter=AM)) {
    if (-not (Test-Path -LiteralPath $f)) { continue }
    $size = (Get-Item -LiteralPath $f).Length
    $isLfs = (git check-attr filter -- $f) -match "filter: lfs$"
    if ($size -gt 50MB -and -not $isLfs) { $big += "$f ($([math]::Round($size / 1MB)) MB)" }
}
if ($big.Count -gt 0) {
    git reset -q
    Stop-Team "大きいファイルが LFS の対象になっていません: $($big -join ', ')" "管理者に .gitattributes への追加を頼んでください"
}

git diff --cached --quiet
if ($LASTEXITCODE -ne 0) {
    Git-Run commit -q -m $Message
    Write-Ok "コミットしました: $Message"
} else {
    Write-Host "新しい変更はありません。"
}

Write-Step "GitHub に送っています"
git push -u origin $current
if ($LASTEXITCODE -ne 0) {
    Stop-Team "GitHub に送れませんでした。" "別のPCで同じブランチを更新していれば .\tools\start-work.ps1 で取り込んでから、もう一度実行してください"
}
Write-Ok "$current を GitHub に保存しました。"
