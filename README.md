## 上位机与下位机进行串口通信
>上位机采用pyside与pyserial  
下位机当然是用c写，不过编译器我用的sdcc，在一些方面与keil编译器不同  

**主要功能**  
上位机发送数据到下位机，下位机根据功能位做出不同的反应  

+ 文件烧写  
>文件烧写是一个脚本,[链接地址](https://github.com/laborer/stcflash)  
注:烧写bin文件的时候会报错,但实际上以及烧写成功  
hex文件不会报错  

+ 查询设置地址
+ 查询设置量程上下限
+ 查询设置报警上下限
+ 查询报警状态
+ 查询温度  
>*这个没有成功*  

+ LCD液晶显示  
>显示一些信息及温度  

+ 数据保存  
>采用postgresql保存数据，虽然有些大材小用  

### 上位机界面
![上位机](https://raw.githubusercontent.com/honmaple/upper/master/upper.png)

写的很丑,见谅


