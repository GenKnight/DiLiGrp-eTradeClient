@rem 制作颁发者证书文件
makecert -sky signature -n "CN=地利集团, E=*.diligrp.com, O=diligrp, OU=diligrp, C=China, S=Beijing" -r -sv ETradeClient_issuer.pvk ETradeClient_issuer.cer

@rem 用颁发者证书文件生成使用者证书文件
makecert -n "CN=ETradeClient" -iv ETradeClient_issuer.pvk -ic ETradeClient_issuer.cer -sv ETradeClient.pvk ETradeClient.cer

@rem 将cer文件转换为spc文件
cert2spc.exe ETradeClient.cer  ETradeClient.spc
cert2spc.exe ETradeClient_issuer.cer  ETradeClient_issuer.spc

@rem 用spc文件与pvk文件生成pfx文件，pfx文件是cer证书文件的封装，用户使用时需要输入密码，最终我们使用pfx文件进行证书安装
pvk2pfx.exe -po "5AYpz4g0KT" -pvk ETradeClient.pvk -spc ETradeClient.spc -pfx ETradeClient.pfx
pvk2pfx.exe -po "5AYpz4g0KT" -pvk ETradeClient_issuer.pvk -spc ETradeClient_issuer.spc -pfx ETradeClient_issuer.pfx