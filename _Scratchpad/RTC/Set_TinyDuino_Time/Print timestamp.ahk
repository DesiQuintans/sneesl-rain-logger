;
; Title
; Author:		Desi Quintans <me@desiquintans.com>
; Website:		http://www.desiquintans.com
;
; Script Function:
;	
;

#NoEnv
SetWorkingDir %A_ScriptDir%
#SingleInstance Force
SetTitleMatchMode, 3

^`::
	FormatTime, Time,, yyyyMMdd HH:mm:ss
	SendInput %Time%{Enter}
return
