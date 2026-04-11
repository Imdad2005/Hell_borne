Add-Type -AssemblyName System.Drawing

$width = 1280
$height = 720
$outputPath = "D:/Hell_borne/Hell_borne/assets/sprites/background/background_sheet.png"

$bitmap = New-Object System.Drawing.Bitmap $width, $height
$graphics = [System.Drawing.Graphics]::FromImage($bitmap)
$graphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::None
$graphics.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::NearestNeighbor
$graphics.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::Half

function Fill-PolygonPoints {
    param(
        [System.Drawing.Graphics]$Graphics,
        [System.Drawing.Brush]$Brush,
        [int[]]$Points
    )

    $pointArray = New-Object System.Drawing.Point[] ($Points.Length / 2)
    for ($index = 0; $index -lt $Points.Length; $index += 2) {
        $pointArray[$index / 2] = New-Object System.Drawing.Point $Points[$index], $Points[$index + 1]
    }

    $Graphics.FillPolygon($Brush, $pointArray)
}

$graphics.Clear([System.Drawing.Color]::FromArgb(255, 8, 10, 24))

$skyBrush = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 24, 14, 34))
$graphics.FillRectangle($skyBrush, 0, 0, $width, 250)
$skyBrush.Dispose()

$horizonBrush = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 55, 16, 20))
$graphics.FillRectangle($horizonBrush, 0, 250, $width, 140)
$horizonBrush.Dispose()

$moonBrush = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 214, 176, 104))
$graphics.FillEllipse($moonBrush, 970, 85, 150, 150)
$moonBrush.Dispose()

 $starBrush = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 224, 232, 255))
for ($i = 0; $i -lt 80; $i++) {
    $sx = Get-Random -Minimum 16 -Maximum 1240
    $sy = Get-Random -Minimum 16 -Maximum 210
    $ss = Get-Random -Minimum 1 -Maximum 3
    $graphics.FillRectangle($starBrush, $sx, $sy, $ss, $ss)
}
$starBrush.Dispose()

$groundDark = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 16, 12, 16))
$groundMid = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 40, 28, 34))
$pathBrush = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 30, 26, 32))
$archDark = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 22, 18, 20))
$archEdge = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 58, 42, 36))
$rockBrush = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 68, 56, 72))
$grassBrush = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 74, 34, 20))

Fill-PolygonPoints $graphics $groundDark @(0, 470, 140, 430, 280, 460, 400, 500, 560, 480, 760, 505, 940, 470, 1100, 500, 1280, 455, 1280, 720, 0, 720)
Fill-PolygonPoints $graphics $groundMid @(0, 510, 150, 470, 280, 490, 400, 530, 560, 510, 760, 535, 940, 500, 1100, 530, 1280, 490, 1280, 720, 0, 720)
Fill-PolygonPoints $graphics $pathBrush @(0, 560, 170, 510, 350, 520, 520, 540, 700, 550, 920, 560, 1100, 600, 1280, 560, 1280, 720, 0, 720)

$graphics.FillEllipse($archDark, 0, 145, 360, 500)
$graphics.FillEllipse($archDark, 470, 55, 340, 585)
$graphics.FillEllipse($archDark, 880, 95, 460, 500)

$graphics.FillEllipse($archEdge, 18, 165, 330, 450)
$graphics.FillEllipse($archEdge, 495, 78, 290, 525)
$graphics.FillEllipse($archEdge, 905, 118, 410, 440)

$graphics.FillEllipse($rockBrush, 90, 190, 70, 220)
$graphics.FillEllipse($rockBrush, 340, 160, 95, 250)
$graphics.FillEllipse($rockBrush, 595, 150, 70, 260)
$graphics.FillEllipse($rockBrush, 1015, 170, 85, 240)

$pillarBrush = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 30, 24, 30))
$graphics.FillRectangle($pillarBrush, 245, 110, 34, 500)
$graphics.FillRectangle($pillarBrush, 660, 92, 28, 548)
$graphics.FillRectangle($pillarBrush, 1085, 120, 30, 520)
$pillarBrush.Dispose()

$graphics.FillRectangle($grassBrush, 310, 620, 28, 18)
$graphics.FillRectangle($grassBrush, 520, 612, 30, 20)
$graphics.FillRectangle($grassBrush, 735, 618, 28, 19)

$torchBrush = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 255, 188, 92))
$emberBrush = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(255, 255, 112, 20))
for ($i = 0; $i -lt 10; $i++) {
    $tx = Get-Random -Minimum 40 -Maximum 1210
    $ty = Get-Random -Minimum 470 -Maximum 690
    $graphics.FillRectangle($torchBrush, $tx, $ty, 6, 6)
    $graphics.FillRectangle($emberBrush, $tx + (Get-Random -Minimum -2 -Maximum 3), $ty - (Get-Random -Minimum 6 -Maximum 18), 2, 2)
}
$torchBrush.Dispose()
$emberBrush.Dispose()

$graphics.Dispose()
$bitmap.Save($outputPath, [System.Drawing.Imaging.ImageFormat]::Png)
$bitmap.Dispose()

$check = [System.Drawing.Image]::FromFile($outputPath)
Write-Output ("Generated background sheet: {0}x{1} -> {2}" -f $check.Width, $check.Height, $outputPath)
$check.Dispose()
