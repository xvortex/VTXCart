@echo off

del /q VTXCart.log >nul 2>&1

python VTXCart.py ..\bin\games.txt mvs --genix --patchmenu --genmame --genrom

rem aes or mvs - select system
rem --c3g        - extend C rom size to 3G
rem --genix      - generate Veriog includes
rem --patchmenu  - patch menu
rem --genmame    - generate MAME roms/hashes (for testing)
rem --genrom     - generate ROM's for Flashers.
