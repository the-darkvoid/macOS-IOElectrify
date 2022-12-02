[![Typing SVG](https://readme-typing-svg.herokuapp.com?font=Fira+Code&pause=1000&width=900&height=70&lines=Welcome+%F0%9F%9B%B8+to+Alienware+%F0%9F%91%BD+Hackintosh+%F0%9F%8D%8E+%E5%A4%96%E6%98%9F%E4%BA%BA+%F0%9F%92%BB+%E9%BB%91%E8%8B%B9%E6%9E%9C+%F0%9F%96%A5+by+RockJesus.cn)](https://rockjesus.cn)

[![qq](https://img.shields.io/badge/Blog-RockJesus-silver.svg?logo=Blogger)](https://rockjesus.cn)

[![qq](https://cdn.jsdelivr.net/gh/rockjesus/img/img/17r4.gif)](https://rockjesus.cn)
 [![Anurag's GitHub stats](https://github-readme-stats.vercel.app/api?username=rockjesus&theme=radical&show_icons=true&bg_color=end,000000,cccccc&hide_border)](https://rockjesus.cn)
 
[![Hardware](https://img.shields.io/badge/Hardware-Alienware-silver.svg?logo=Hackaday)](https://alienware.com) [![Platform](https://img.shields.io/badge/platform-MacOS13~10-red.svg?logo=apple)](https://developer.apple.com/macos) [![Platform](https://img.shields.io/badge/platform-Windows11-blue.svg?logo=windows)](https://www.microsoft.com/en-us/windows/)
[![b](https://img.shields.io/badge/Bootloader-Opencore-silver.svg?logo=OpenCollective)](https://github.com/acidanthera/OpenCorePkg)
[![b](https://img.shields.io/badge/Bootloader-Clover-green.svg?logo=4chan)](https://github.com/CloverHackyColor/CloverBootloader)
[![b](https://img.shields.io/badge/Credits-Acidanthera-purple.svg?logo=Academia)](https://github.com/acidanthera)
[![b](https://img.shields.io/badge/Credits-TonymacX86-green.svg?logo=Tesla)](https://tonymacx86.com)
[![b](https://img.shields.io/badge/Credits-PCbeta-red.svg?logo=PyCharm)](https://bbs.pcbeta.com)
[![b](https://img.shields.io/badge/Credits-黑果小兵-yellow.svg?logo=Babel)](https://blog.daliansky.net/)
[![qq](https://img.shields.io/badge/外星人黑苹果-个人博客Blog-silver.svg?logo=Blogger)](https://rockjesus.cn)
[![qq](https://img.shields.io/badge/外星人黑苹果-QQ群-purple.svg?logo=TencentQQ)](https://gcore.jsdelivr.net/gh/rockjesus/rockjesus/img/qq.png)
[![qq](https://img.shields.io/badge/外星人黑苹果-微信公众号-silver.svg?logo=WeChat)](https://gcore.jsdelivr.net/gh/rockjesus/rockjesus/img/gzh.jpg)
[![qq](https://img.shields.io/badge/social-tiktok-blue.svg?logo=tiktok)](https://gcore.jsdelivr.net/gh/rockjesus/rockjesus/img/dou.png)
[![qq](https://img.shields.io/badge/social-微信视频号-green.svg?logo=AirPlayVideo)](https://gcore.jsdelivr.net/gh/rockjesus/rockjesus/img/sph.jpg)
[![qq](https://img.shields.io/badge/social-电影公众号-red.svg?logo=Aparat)](https://gcore.jsdelivr.net/gh/rockjesus/rockjesus/img/dy.jpg)
 [![zfb](https://img.shields.io/badge/打赏-支付宝-blue.svg?logo=alipay)](https://gcore.jsdelivr.net/gh/rockjesus/rockjesus/img/zfb.png)
 [![wx](https://img.shields.io/badge/打赏-微信-green.svg?logo=wechat)](https://gcore.jsdelivr.net/gh/rockjesus/rockjesus/img/wx.png)
 [![zsm](https://img.shields.io/badge/打赏-赞赏码-yellow.svg?logo=wechat)](https://gcore.jsdelivr.net/gh/rockjesus/rockjesus/img/zsm.png)
 [![qq](https://img.shields.io/badge/黑苹果-远程安装-purple.svg?logo=Atom)](https://gcore.jsdelivr.net/gh/rockjesus/rockjesus/img/az.jpg)
 [![Gitter](https://img.shields.io/badge/Chatroom-Gitter-9cf.svg?logo=gitter)](https://gitter.im/Alienware-hackintosh/community)
[![stars](https://img.shields.io/github/stars/rockjesus.svg?logo=ApacheSpark)](https://github.com/RockJesus)
[![followers](https://img.shields.io/github/followers/RockJesus.svg?logo=SouthwestAirlines&color=purple)](https://github.com/RockJesus)
 [![download-badge](https://img.shields.io/github/downloads/RockJesus/Alienware-Opencore-Theme/total.svg?logo=DocuSign "Download status")](https://github.com/RockJesus/Alienware-Hackintosh "Download status")
 [![visitors](https://visitor-badge.glitch.me/badge?page_id=rockjesus.visitor-badge&left_color=green&right_color=red)](https://rockjesus.cn)
# Introduction

IOElectrify is a macOS kext which enables always-on power to Intel Thunderbolt hardware.

## In Depth

macOS expects the Intel Thunderbolt controller to be always powered on.
This opposed to the Windows implementation where Thunderbolt devices are configured through the BIOS.

Most ACPI implementations on Windows machines expose a Thunderbolt `force-power` method.

This method is published using the [Windows Management Instrumentation ACPI interface](https://msdn.microsoft.com/en-us/library/windows/hardware/dn614028(v=vs.85).aspx).

macOS does not support Windows Management Instrumentation out of the box, this is where IOElectrify comes in.

IOElectrify attaches to the `WTBT` ACPI object and determines the method with [guid](https://patchwork.kernel.org/patch/9942739/) `86ccfd48-205e-4a77-9c48-2021cbede341`.

For both the awake and sleep events, IOElectrify will respectively activate and de-activate the Intel Thunderbolt `force-power` setting.

## Configuration

IOElectrify attaches to ACPI identity `PNP0C14` with an `_UID` of `TBFP` by default.
This can be modified in the `Info.plist` as required.

## Tested

* Dell XPS 9360 - Alpine Ridge 2C `8086:1716`

## Credits

* [goodwin_c](https://www.tonymacx86.com/threads/usb-c-hotplug-not-working.223534/page-5#post-1580114): Thunderbolt force-power method
* [IOWMIFamily by dolnor](https://github.com/Dolnor/IOWMIFamily): macOS WMI interface
