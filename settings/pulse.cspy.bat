@REM This batch file has been generated by the IAR Embedded Workbench
@REM C-SPY Debugger, as an aid to preparing a command line for running
@REM the cspybat command line utility using the appropriate settings.
@REM
@REM Note that this file is generated every time a new debug session
@REM is initialized, so you may want to move or rename the file before
@REM making changes.
@REM
@REM You can launch cspybat by typing the name of this batch file followed
@REM by the name of the debug file (usually an ELF/DWARF or UBROF file).
@REM
@REM Read about available command line parameters in the C-SPY Debugging
@REM Guide. Hints about additional command line parameters that may be
@REM useful in specific cases:
@REM   --download_only   Downloads a code image without starting a debug
@REM                     session afterwards.
@REM   --silent          Omits the sign-on message.
@REM   --timeout         Limits the maximum allowed execution time.
@REM 


"C:\iar\common\bin\cspybat" "C:\iar\avr\bin\avrproc.dll" "C:\iar\avr\bin\avrJtagIce-mkII.dll"  %1 --plugin "C:\iar\avr\bin\avrlibsupportbat.dll" --backend -B "--cpu=m8" "--enhanced_core" "-p" "C:\iar\avr\Config\iom8.ddf" "--eeprom_size" "512" "-d" "jtagIceMkII" "--drv_dragon" "--drv_communication_log" "cspycomm.log" "--jtagice_clock=100000" "--jtagice_leave_timers_running" "--jtagice_do_hardware_reset" "--drv_communication=USB" 


