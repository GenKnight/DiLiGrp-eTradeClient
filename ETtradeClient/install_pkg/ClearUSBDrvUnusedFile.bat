@echo off

set file_1=samcoins.dll
set file_2=sdt_s_drv_x64.cat
set file_3=USBDrv.sys
set file_4=USBDrvCo.inf

if exist E:\%file_1% (del E:\%file_1%)
if exist E:\%file_2% (del E:\%file_2%)
if exist E:\%file_3% (del E:\%file_3%)
if exist E:\%file_4% (del E:\%file_4%)

if exist C:\%file_1% (del C:\%file_1%)
if exist C:\%file_2% (del C:\%file_2%)
if exist C:\%file_3% (del C:\%file_3%)
if exist C:\%file_4% (del C:\%file_4%)