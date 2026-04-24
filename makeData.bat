mkdir 2250150_watanabe
robocopy bin/x64/Release 2250150_watanabe *.exe
cd 2250150_watanabe
mkdir Data
cd ../
robocopy Data 2250150_watanabe\Data /E