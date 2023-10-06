@echo off

del /q VTXCart.log >nul 2>&1

VTXCart.exe games.txt MVS GenIX PatchMenu GenMAME GenROM

rem AES or MVS - select system
rem c3g        - extend C rom size to 3G
rem GenIX      - generate Veriog includes
rem PatchMenu  - patch menu
rem GenMAME    - generate MAME roms/hashes (for testing)
rem GenROM     - generate ROM's for Flashers.
