@echo off
cd /d %~dp0
start cmd /k "python tool/backup_tool.py"
