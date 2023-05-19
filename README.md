# 下载cmake
 https://cmake.org/download/
# 下载libevent 
https://libevent.org/  这里使用的libevent-2.1.12-stable

# 编译libevent
编译器使用vs2017

使用cmake编译，会报错，如下：

![image](https://github.com/neilyoguo/windows-libevent/blob/main/image/2.png)

在windows上不需要openssl，勾选掉重新构建即可

![image](https://github.com/neilyoguo/windows-libevent/blob/main/image/1.png)

通过vs2017 构建出来3个lib库和dll库
```
event.lib
event_core.lib
event_extra.lib
event.dll
event_core.dll
event_extra.dll
```
## 使用libevent库 
tcp实战 见[testlibevent](https://github.com/neilyoguo/windows-libevent/tree/main/testlibevent)

