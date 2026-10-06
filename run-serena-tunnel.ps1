$ErrorActionPreference = "Stop"

# ============================================================
# OpenAI Tunnel + Serena MCP
# Project: PowerMeter_PZEM004
# ============================================================

$TunnelDir    = "C:\OpenAI-Tunnel"
$TunnelClient = Join-Path $TunnelDir "tunnel-client.exe"

$TunnelProfile = "serena"
$Project = "D:\@Project\PowerMeter_PZEM004"

# ------------------------------------------------------------
# Display
# ------------------------------------------------------------

Write-Host ""
Write-Host "============================================" -ForegroundColor Cyan
Write-Host " OpenAI Tunnel + Serena MCP" -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan
Write-Host ""

Write-Host "Profile : $TunnelProfile" -ForegroundColor White
Write-Host "Project : $Project" -ForegroundColor White
Write-Host "Tunnel  : $TunnelClient" -ForegroundColor White
Write-Host ""

# ------------------------------------------------------------
# Check Tunnel Client
# ------------------------------------------------------------

Write-Host "[1/4] Checking tunnel-client.exe..." -ForegroundColor Yellow

if (-not (Test-Path -LiteralPath $TunnelClient)) {
    Write-Host ""
    Write-Host "ERROR: tunnel-client.exe not found." -ForegroundColor Red
    Write-Host $TunnelClient -ForegroundColor Red
    Write-Host ""
    exit 1
}

Write-Host "OK: tunnel-client.exe found." -ForegroundColor Green
Write-Host ""

# ------------------------------------------------------------
# Check Project
# ------------------------------------------------------------

Write-Host "[2/4] Checking project..." -ForegroundColor Yellow

if (-not (Test-Path -LiteralPath $Project)) {
    Write-Host ""
    Write-Host "ERROR: Project directory not found." -ForegroundColor Red
    Write-Host $Project -ForegroundColor Red
    Write-Host ""
    exit 1
}

$SerenaDir = Join-Path $Project ".serena"
$ProjectConfig = Join-Path $SerenaDir "project.yml"
$LocalConfig = Join-Path $SerenaDir "project.local.yml"

Write-Host "OK: Project found." -ForegroundColor Green
Write-Host "Serena : $SerenaDir" -ForegroundColor Gray

if (Test-Path -LiteralPath $ProjectConfig) {
    Write-Host "OK: project.yml found." -ForegroundColor Green
}
else {
    Write-Host "WARNING: project.yml not found." -ForegroundColor Yellow
}

if (Test-Path -LiteralPath $LocalConfig) {
    Write-Host "OK: project.local.yml found." -ForegroundColor Green
}
else {
    Write-Host "WARNING: project.local.yml not found." -ForegroundColor Yellow
}

Write-Host ""

# ------------------------------------------------------------
# Change to Tunnel directory
# ------------------------------------------------------------

Write-Host "[3/4] Checking Serena tunnel profile..." -ForegroundColor Yellow

Set-Location -LiteralPath $TunnelDir

& $TunnelClient doctor --profile $TunnelProfile --explain

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "ERROR: Serena tunnel profile check failed." -ForegroundColor Red
    Write-Host ""
    exit $LASTEXITCODE
}

Write-Host ""
Write-Host "OK: Serena tunnel profile is ready." -ForegroundColor Green
Write-Host ""

# ------------------------------------------------------------
# Start Tunnel + Serena
# ------------------------------------------------------------

Write-Host "[4/4] Starting OpenAI Tunnel + Serena MCP..." -ForegroundColor Green
Write-Host ""
Write-Host "Project:" -ForegroundColor Cyan
Write-Host "  $Project" -ForegroundColor White
Write-Host ""
Write-Host "Serena configuration:" -ForegroundColor Cyan
Write-Host "  $LocalConfig" -ForegroundColor White
Write-Host ""
Write-Host "Expected clangd:" -ForegroundColor Cyan
Write-Host "  C:\Program Files\LLVM\bin\clangd.exe" -ForegroundColor White
Write-Host ""
Write-Host "Keep this terminal open while using Serena." -ForegroundColor Yellow
Write-Host ""
Write-Host "Press Ctrl+C to stop the Tunnel + Serena MCP." -ForegroundColor Yellow
Write-Host ""

& $TunnelClient run --profile $TunnelProfile

Write-Host ""
Write-Host "============================================" -ForegroundColor Yellow
Write-Host " Tunnel + Serena stopped." -ForegroundColor Yellow
Write-Host "============================================" -ForegroundColor Yellow
Write-Host ""

Pause
