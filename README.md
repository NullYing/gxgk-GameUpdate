#游戏更新器
<p>    
仅广科校园网可高速更新
游戏更新神器，通过内网下载游戏更新，速度突破电信带宽限制
若使用人数多，可解决每次LOL或其他游戏更新导致的校园网崩溃
LOL提供从3.1.5.1(忘记最后抓的包是什么版本了)到最新版本的更新包，之后或可能提供最新版安装包
不过由于内网带宽出口不过，最大同时使用人数有所限制

由于网络升级，游戏更新网络拥堵已经很少，存在意义并不大了
</p>
**使用环境**
	客户端 Windows：VS2015
	服务端 Linux
	使用的库：curl
**不足之处**
	1.没有分析腾讯游戏的tcp包，只是用二进制模仿发包，每隔一段时间要重新抓包，不然接收到的数量有点大
	2.不玩游戏，某些未知情况下，需要N.exe的更新包，然而不知道需要此包条件，无法准确的判断是否下载
	3.MFC界面，并不友好
	4.只分析了英雄联盟和地下城与勇士的更新
**注意事项**
如果使用该源码，请准守开源协议，并请勿修改原作者名称
[GPL开源协议](https://github.com/NullYing/GameUpdate/raw/master/LICENSE)
##界面预览
该源码客户端界面<br />
![客户端界面](https://raw.githubusercontent.com/NullYing/GameUpdate/master/images/GameUpdate.png)
#莞香广科
官网：[http://www.gxgk.cc](http://www.gxgk.cc)<br />
		莞香广科是学生自建的互联网创新团队。团队成立于2012年5月，以校园论坛起步，逐渐发展了校园网资源共享站、媒体中心、微信助手等项目。

#游戏更新器声明
<p>
	/**************************************************************************************<br />
	<br />
	Copyleft © 2015 NULLYING<br />
	Author:白月秋见心<br />
	http://www.lastfighting.com/<br />
	Email: ourweijiang@gmail.com<br />
	<br />
	关于“游戏更新器”的声明：<br />
	1.游戏更新功能的实现均是通过黑盒分析得来，并未通过任何不正当方法获得。<br />
	2.不对游戏更新补丁负责，包括补丁包普遍安装出错，游戏启动出错。<br />
	3.本软件仅供研究学习之用，未经本人允许，不得用于商业用途<br />
	4.本软件在发布前均通过一般性应用测试，但不保证任何情况下对机器无害，由于未知的使用环境或不当的使用对计算机造成的损害，责任由使用者全部承担。<br />
	6.本软件属开源软件，如果需要修改本软件源码以进行二次发布，也请继续公开源代码。<br />
    如不同意该声明请不要使用该软件，谢谢合作。<br />
	**************************************************************************************/<br />
</p>
