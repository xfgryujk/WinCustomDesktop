WinCustomDesktop
=========

自绘桌面框架，用你的代码绘制桌面，支持插件开发。目前自带的插件有*播放视频*和*遮罩桌面*

### 播放*Bad Apple*

![VideoDesktop](https://github.com/xfgryujk/WinCustomDesktop/blob/master/.wiki/image/snapshot_VideoDesktop.gif)

### 遮罩桌面
![MaskDesktop](https://github.com/xfgryujk/WinCustomDesktop/blob/master/.wiki/image/snapshot_MaskDesktop.gif)

### 资源占用

经过优化，占用的计算资源很少，以下为播放*Bad Apple*时占用的资源

![MaskDesktop](https://github.com/xfgryujk/WinCustomDesktop/blob/master/.wiki/image/snapshot_CPU.png)


支持的平台
---------

以下是已通过测试的平台，其他平台未测试，不代表不支持

* 64位Windows 10
* 64位Windows 7
* 32位Windows 7
* Windows XP


使用方法
---------

1. 使用VS2013生成
2. 把插件放在`CustomDesktop.dll`所在目录下的`Plugin`目录（如x64\Release\Plugin）（其实已经把生成目录设在这里了）
3. 复制`Data`文件夹到插件所在目录
4. 运行`Inject.exe`
5. 如果运行失败，提示“从服务器返回了一个参照”，在**文件属性-兼容性**里选择**以管理员身份运行此程序**


插件开发
---------

1. 下载源码
2. 把插件模板导入VS2013：  
   把`Plugin\CustomDesktopPluginTemplate.zip`放到你的VS2013模板目录下，你可以在**工具-选项-项目和解决方案-用户项目模板位置**看到你的模板目录
3. 在解决方案中新建项目，模板选择**CustomDesktop插件**，位置选择**Plugin**目录，否则无法找到CommonProperty.props
