    @echo off
for %%f in (".\Data\Excel\*.xlsx") do (
    echo %%~nxf
    "./Tools/ConvertExcelToJson.exe" %%~nf
)
pause