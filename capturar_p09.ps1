param(
    [string]$OutDir = "C:\Users\David\Documents\2027-1\CGICH\capturas_p09",
    [string]$Prefix = "captura",
    [int]$Count = 1,
    [int]$IntervalMs = 350,
    [int]$DelayMs = 1200
)

Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;
using System.Text;
using System.Runtime.InteropServices;
public class Win32Cap {
    [DllImport("user32.dll")] public static extern bool SetProcessDPIAware();
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr hWnd);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern IntPtr FindWindow(string lpClassName, string lpWindowName);
    [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr hWnd, out RECT lpRect);
    [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr hWnd, ref POINT lpPoint);
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumWindowsProc lpEnumFunc, IntPtr lParam);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowText(IntPtr hWnd, StringBuilder lpString, int nMaxCount);
    public delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);
    public static void HideOtherWindows(uint processId, string keepTitle) {
        EnumWindows((h, l) => {
            uint pid;
            GetWindowThreadProcessId(h, out pid);
            if (pid == processId) {
                var sb = new StringBuilder(512);
                GetWindowText(h, sb, 512);
                if (sb.ToString() != keepTitle) ShowWindow(h, 0);
            }
            return true;
        }, IntPtr.Zero);
    }
    [StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left, Top, Right, Bottom; }
    [StructLayout(LayoutKind.Sequential)] public struct POINT { public int X, Y; }
}
"@

[Win32Cap]::SetProcessDPIAware() | Out-Null

$proc = Get-Process LabCGeIHC -ErrorAction SilentlyContinue | Select-Object -First 1
if ($proc) {
    [Win32Cap]::HideOtherWindows([uint32]$proc.Id, "Practica 09: Animacion")
    Start-Sleep -Milliseconds 250
}

$h = [Win32Cap]::FindWindow($null, "Practica 09: Animacion")
if ($h -eq [IntPtr]::Zero) {
    $p = Get-Process LabCGeIHC -ErrorAction Stop |
        Where-Object { $_.MainWindowHandle -ne 0 } |
        Select-Object -First 1
    $h = $p.MainWindowHandle
}
[Win32Cap]::ShowWindow($h, 5) | Out-Null
[Win32Cap]::SetForegroundWindow($h) | Out-Null
Start-Sleep -Milliseconds 300

$rect = New-Object Win32Cap+RECT
[Win32Cap]::GetClientRect($h, [ref]$rect) | Out-Null

$pt = New-Object Win32Cap+POINT
[Win32Cap]::ClientToScreen($h, [ref]$pt) | Out-Null

$w = $rect.Right - $rect.Left
$hh = $rect.Bottom - $rect.Top
if ($w -le 0 -or $hh -le 0) { throw "No se pudo obtener el area del cliente" }

New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
$tray = [Win32Cap]::FindWindow("Shell_TrayWnd", $null)
if ($tray -ne [IntPtr]::Zero) { [Win32Cap]::ShowWindow($tray, 0) | Out-Null }
Start-Sleep -Milliseconds $DelayMs

try {
for ($i = 1; $i -le $Count; $i++) {
    $bmp = New-Object System.Drawing.Bitmap $w, $hh
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.CopyFromScreen($pt.X, $pt.Y, 0, 0, $bmp.Size)

    $file = Join-Path $OutDir ("{0}_{1:D2}.png" -f $Prefix, $i)
    $bmp.Save($file, [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose()
    $bmp.Dispose()
    Write-Output $file

    if ($i -lt $Count) { Start-Sleep -Milliseconds $IntervalMs }
}
}
finally {
    if ($tray -ne [IntPtr]::Zero) { [Win32Cap]::ShowWindow($tray, 5) | Out-Null }
}
