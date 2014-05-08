cvtres /out:swrap1.res swrap.res
dllwrap -s -o swrapd.dll *.obj swrap_private.res ..\bass.lib ..\mss32.lib -def ../swrap.def
pause
