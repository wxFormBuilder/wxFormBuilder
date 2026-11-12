# Usage: powershell -NoProfile -ExecutionPolicy Bypass -File dump.ps1 -files "a.png,b.png" -out dump.txt
# Prints each PNG at native size as a character grid ('.' = transparent) plus palette (RRGGBBAA).
param([string]$files, [string]$out)
Add-Type -AssemblyName System.Drawing
$chars = '123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz@#$%&*+=?<>!^~:;'
$sb = [System.Text.StringBuilder]::new()
foreach ($f in $files.Split(',')) {
  $b = [System.Drawing.Bitmap]::new($f.Trim())
  $pal = [ordered]@{}; $rows = @()
  for ($y=0; $y -lt $b.Height; $y++) {
    $line = ''
    for ($x=0; $x -lt $b.Width; $x++) {
      $c = $b.GetPixel($x, $y)
      if ($c.A -lt 16) { $line += '.'; continue }
      $hex = '{0:x2}{1:x2}{2:x2}{3:x2}' -f $c.R,$c.G,$c.B,$c.A
      if (-not $pal.Contains($hex)) { $pal[$hex] = if ($pal.Count -lt $chars.Length) { $chars[$pal.Count] } else { '"' } }
      $line += $pal[$hex]
    }
    $rows += $line
  }
  [void]$sb.AppendLine("== $f ($($b.Width)x$($b.Height))")
  $i=0; foreach ($k in $pal.Keys) { [void]$sb.Append("$($pal[$k])=$k "); $i++; if ($i % 8 -eq 0) { [void]$sb.AppendLine() } }
  [void]$sb.AppendLine(); [void]$sb.AppendLine('   ' + (-join (0..($b.Width-1) | ForEach-Object { $_ % 10 })))
  $r=0; foreach ($l in $rows) { [void]$sb.AppendLine(('{0,2} ' -f $r) + $l); $r++ }
  $b.Dispose()
}
[IO.File]::WriteAllText($out, $sb.ToString())
