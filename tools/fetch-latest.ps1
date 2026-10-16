# develop に新しくマージされたものを取ってくるスクリプト（作業は始めない）
# 使い方: .\tools\fetch-latest.ps1
#   develop にいるとき          … 手元の develop を GitHub の最新にする（フォルダの中身も最新になる）
#   フィーチャーブランチにいるとき … 手元の develop だけ最新にする（今の作業には混ぜない。混ぜるときは start-work.ps1）

. "$PSScriptRoot\common.ps1"
Set-Location $RepoRoot
$cfg = Get-TeamConfig
$dev = $cfg.developBranch

Write-Step "GitHub の最新を取得しています"
$before = git rev-parse --verify -q "refs/heads/$dev"
git fetch --prune origin
if ($LASTEXITCODE -ne 0) { Stop-Team "GitHub に接続できませんでした。" "ネットワークと gh auth status を確認してください" }
$after = git rev-parse "origin/$dev"

$current = Get-CurrentBranch
if (-not (Test-LocalBranch $dev)) {
    # 手元に develop がない（クローン直後に別のブランチにいる など）→ 作るだけ
    Git-Run branch --track $dev "origin/$dev"
} elseif ($before -eq $after) {
    Write-Ok "$dev は最新です。"
} elseif (-not (Test-Contains "origin/$dev" $dev)) {
    Stop-Team "手元の $dev に、GitHub にないコミットがあります（develop で直接コミットした可能性）。" "管理者に相談してください"
} elseif ($current -eq $dev) {
    # develop にいる → フォルダの中身ごと最新にする
    git merge --ff-only "origin/$dev"
    if ($LASTEXITCODE -ne 0) {
        Stop-Team "保存していない変更が、新しく入ったファイルとぶつかっています。" ".\tools\start-work.ps1 <作業名> で変更をフィーチャーブランチに移してから、もう一度実行してください"
    }
} else {
    # 別のブランチにいる → 手元の develop の印だけ進める（今の作業ファイルは変わらない）
    Git-Run update-ref "refs/heads/$dev" "origin/$dev" $before
}

# 新しく入ったものを表示する
if ($before -and $before -ne $after) {
    Write-Host ""
    Write-Host "=== $dev に新しく入ったもの ===" -ForegroundColor Cyan
    $merges = @(git log "$before..$after" --merges --first-parent --reverse --format="%H")
    $others = git log "$before..$after" --no-merges --reverse --format="  %h %s（%an）"
    if ($merges.Count -gt 0) {
        foreach ($h in $merges) {
            # "Merge pull request #3 from org/feature/xxx" ＋ 本文1行目（PRのタイトル）→ "#3 タイトル（feature/xxx）"
            $subject = git log -1 --format="%s" $h
            $title = git log -1 --format="%b" $h | Where-Object { $_ } | Select-Object -First 1
            if ($subject -match '^Merge pull request (#\d+) from [^/]+/(\S+)$') { Write-Host "  $($Matches[1]) $title（$($Matches[2])）" }
            else { Write-Host "  $subject" }
        }
        Write-Host "  （コミット $(@($others).Count) 件）" -ForegroundColor DarkGray
    } else {
        $others | ForEach-Object { Write-Host $_ }
    }
}

# マージ済みで GitHub から消えたフィーチャーブランチ（手元にだけ残っているもの）
$gone = git for-each-ref --format="%(refname:short)%09%(upstream:track)" refs/heads/feature | Where-Object { $_ -match "\t\[gone\]$" } | ForEach-Object { ($_ -split "`t")[0] }
if ($gone) {
    Write-Host ""
    Write-Host "マージ済みで、手元にだけ残っているブランチ（消してOK）:" -ForegroundColor DarkGray
    foreach ($g in $gone) { Write-Host "  git branch -d $g" -ForegroundColor DarkGray }
}

Write-Host ""
if ($current -eq $dev) {
    Write-Ok "フォルダの中身は $dev の最新です。Visual Studio で「再読み込み」を聞かれたら「すべて再読み込み」を押してください。"
    Write-Host "  作業を始めるとき: .\tools\start-work.ps1 <作業名>"
} else {
    Write-Ok "手元の $dev を最新にしました（今の $current の中身は変えていません）。"
    if (-not (Test-Contains HEAD "origin/$dev")) { Write-Host "  今の作業に develop の最新を取り込むとき: .\tools\start-work.ps1" }
}
