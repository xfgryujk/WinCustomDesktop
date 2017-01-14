WinCustomDesktop
=========

自绘桌面框架，用你的代码绘制桌面，支持插件开发

目前自带的插件有播放视频、桌面浏览器、遮罩桌面和WIMC

想法来自DreamScene，在Wallpaper Engine出来很早之前就做过类似的了，最近想完善它

### 播放*Bad Apple*

![VideoDesktop](https://github.com/xfgryujk/WinCustomDesktop/blob/master/.wiki/image/snapshot_VideoDesktop.gif)

### 桌面浏览器

![DesktopBrowser](https://github.com/xfgryujk/WinCustomDesktop/blob/master/.wiki/image/snapshot_DesktopBrowser.png)

### 遮罩桌面

![MaskDesktop](https://github.com/xfgryujk/WinCustomDesktop/blob/master/.wiki/image/snapshot_MaskDesktop.gif)

### WIMC

WIMC = Where the fuck Is My Cursor

![WIMC](https://github.com/xfgryujk/WinCustomDesktop/blob/master/.wiki/image/snapshot_WIMC.gif)

### 资源占用

经过优化，占用的计算资源很少，以下为播放*Bad Apple*时占用的资源

![CPU](https://github.com/xfgryujk/WinCustomDesktop/blob/master/.wiki/image/snapshot_CPU.png)


支持的平台
---------

以下是已通过测试的平台，其他平台未测试，不代表不支持

* 64位Windows 10
* 64位Windows 7
* 32位Windows 7
* Windows XP


搭建开发环境
---------

1. 使用VS2013或以上版本
2. 运行`SetupEnvironment.bat`


插件开发
---------

1. 下载源码
2. 把插件模板导入VS2013：  
   把`Plugin\CustomDesktopPluginTemplate.zip`放到你的VS2013模板目录下，你可以在**工具-选项-项目和解决方案-用户项目模板位置**看到你的模板目录
3. 在解决方案中新建项目，模板选择**CustomDesktop插件**，位置选择**Plugin**目录，否则无法找到CommonProperty.props
