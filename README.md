# Project 项目文档
### 一、项目概述：
本项目是一个综合性的编程项目，涉及多个领域的功能实现，包括但不仅限于：

- 人工智能
- 物理模拟（待完成）
- 计算机模拟
- 算法

等。 使用主要使用C++进行开发，借助 Red Panda C++ IDE 进行开发。
### 二、项目结构
1. **项目配置文件**
   1. CppProperties.json：C++ 项目的属性配置文件，定义了编译环境、包含路径、宏定义等信息。
2. **基础头文件**
   1. Project_private.h：包含项目的版本定义和一些基本信息，该文件会被 Red Panda C++ 覆盖，不建议手动编辑。
   2. Configuration.h：定义了一些常量和变量，如微增量、微误差、重力常数等。
3. **主要文件**
    这些文件均是``Project``的子文件加下的子文件
### 三、环境配置
   1. C++ 环境  
      这个项目使用 MSVC x64 作为 C++ 编译环境，相关配置可以在 CppProperties.json 文件中查看，作者并不保证在其他编译器中可以正常编译。
### 四、编译和运行
   1. C++ 项目  
      确保已经安装了 MinGW 或者其他 C++ 编译器。
      使用编译器打开项目文件，进行编译和运行。
   2. 特别注意：  
      ``Project/Algorithm/NumberTheory/BigInt``中的文件在使用之前记得编译。
### 分点介绍
	- __Future__ : 这里是未来准备实现的部分，不建议使用（或根本不能使用）
	- Actuator : 这里是执行模拟器，你可以通过自定义解析&执行函数实现CPU的功能
	- Algorithm : 这里是常见算法实现，Oier可以参考/批判
	- ArtificialIntelligence : 这里是人工智能的实现，通过提供数据集便可以开始训练（别忘了实例化）
	- EquationAndFunction : 请忽略Function，这里不是Function的定义，这里提供了解方程/组的方法
	- Expression : 项目的源头，一个表达式类的实现，暂不支持化简
	- Geometry : 几何大类，目前只实现了计算几何，主类在[CoordinateSystem](Project/Geometry/CoordinateSystem/CoordinateSystem.h)中，未来还会继续开发，有兴趣可以看看其他的定义
	- Logging : 日志记录
	- Matrix : ~~不要问我为什么不放在Geometry中~~，矩阵的计算（但在）[Project/ArtificialIntelligence/ArtificialIntelligence/Model/Public.h](Project/ArtificialIntelligence/ArtificialIntelligence/Model/Public.h)中也有实现
	- Server : 一个服务器的实现，通过自定义解析&显示函数实现类似服务器的功能
	- 其他文件 : 均有RedPanda-DevCpp自动生成，可以选择删除
### 六、版本信息
项目当前版本为 1.0.0.0
### 八、许可证
本项目采用 [MIT 许可证](LICENSE) 进行许可。
### 鸣谢（部分）
   1. 感谢[io-wiki](https://oi-wiki.org/)为[Project/Algorithm/](Project/Algorithm)贡献的部分代码
   2. 感谢RedPanda-DevCpp提供的编辑器支持（虽然生成了许多不必要文件）
   3. 感谢Github提供的代码托管
   4. 许多AI与朋友的宝贵建议
