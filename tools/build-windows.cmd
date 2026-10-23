@rem Builds the Windows installer: double click in Explorer or "tools\build-windows" in a console
@powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build-windows.ps1" %*
@pause
