# 程序笔记
- [程序笔记](#%E7%A8%8B%E5%BA%8F%E7%AC%94%E8%AE%B0)
    - [图像(Image)](#%E5%9B%BE%E5%83%8Fimage)
        - [square_detection](#squaredetection)
        - [background_subtraction（背景减除）](#backgroundsubtraction%E8%83%8C%E6%99%AF%E5%87%8F%E9%99%A4)
            - [KNN](#knn)
        - [image_enhance(图像增强)](#imageenhance%E5%9B%BE%E5%83%8F%E5%A2%9E%E5%BC%BA)
            - [histogram_equalization](#histogramequalization)
    - [语音(Speech)](#%E8%AF%AD%E9%9F%B3speech)
        - [plot_waveform&spectrogram（python绘制.wav文件的波形图和语谱图）](#plotwaveformspectrogrampython%E7%BB%98%E5%88%B6wav%E6%96%87%E4%BB%B6%E7%9A%84%E6%B3%A2%E5%BD%A2%E5%9B%BE%E5%92%8C%E8%AF%AD%E8%B0%B1%E5%9B%BE)
        - [CRF_ChineseWordSegmentation(中文分词)](#crfchinesewordsegmentation%E4%B8%AD%E6%96%87%E5%88%86%E8%AF%8D)
            - [关于CRF++模板](#%E5%85%B3%E4%BA%8Ecrf%E6%A8%A1%E6%9D%BF)
    - [遇到的问题(Problems)](#%E9%81%87%E5%88%B0%E7%9A%84%E9%97%AE%E9%A2%98problems)
        - [About python](#about-python)
            - [if \_\_name\_\_=='\_\_main\_\_'](#if-namemain)
            - [文件路径问题](#%E6%96%87%E4%BB%B6%E8%B7%AF%E5%BE%84%E9%97%AE%E9%A2%98)
        - [About OpenCV](#about-opencv)
            - [OpenCV与Matplotlib读取彩色图像模式不同](#opencv%E4%B8%8Ematplotlib%E8%AF%BB%E5%8F%96%E5%BD%A9%E8%89%B2%E5%9B%BE%E5%83%8F%E6%A8%A1%E5%BC%8F%E4%B8%8D%E5%90%8C)
            - [开启摄像头（打开视频）后没有图像](#%E5%BC%80%E5%90%AF%E6%91%84%E5%83%8F%E5%A4%B4%E6%89%93%E5%BC%80%E8%A7%86%E9%A2%91%E5%90%8E%E6%B2%A1%E6%9C%89%E5%9B%BE%E5%83%8F)
            - [RGB2GRAY OR BGR2GRAY](#rgb2gray-or-bgr2gray)
## 图像(Image)

### square_detection

*任务要求：给定两个视频，视频中有一个黄色的矩形面板，要求将其框出来，并输出结果*

整体思路就是使用HSV模型利用颜色的不同将目标物体的与周围环境区分开（适用于目标颜色与周围环境颜色差距较大时），然后再将其二值化后将目标物体框出。

主要的过程方法在程序[square_detection.py](https://github.com/XinzheTing/Courses/blob/master/DigitalImageProcessing/square_detection/square_detection.py)中注释已经写的很清楚了，这里主要说一下在框出目标物体时所使用的三种方法（这三种方法在处理data1.avi时效果相同，主要区别是在处理data2.avi时，因为data2.avi考虑了目标物体的旋转和倾斜）：

1. 固定矩形（我暂且这么称呼它）

    在这种方法中，我使用了boundingRect函数，它的作用是计算轮廓的垂直边界最小矩形，**矩形是与图像上下边界平行的**。这个函数返回来矩形左上角顶点坐标和举行的长和宽，依据这些信息我们就可以画出一个矩形。但是这种方法没有考虑旋转，在目标与图像上下边界平行时没有问题，但是当目标发生了旋转，这时框出的矩形包含了很多除目标物体以外的区域，如下图绿色矩形

    <div align=center>

    ![result1](square_detection/result1.png)

    </div>

2. 外接矩形
  
   这种方法使用minAreaRect函数，它是计算轮廓的外接矩形，弥补了固定矩形方法中在目标物体发生旋转时的不足。见上图红色矩形。

3. 近似轮廓

   以上两种方法在目标物体发生倾斜时，仍有一些不足。进一步学习了解到，findContours所找到的轮廓直接输出的话最贴近目标物体的形状，不论物体如何变化（远近，旋转，倾斜）都可以很好的描绘出，但是findContours所找到的轮廓是一些列散列的点。使用轮廓近似函数approxPolyDP可以选择其中的部分点在使用polylines将这些点连起来，通过改变取点的间隔就可以控制轮廓近似的程度。结果见下图蓝色矩形。

   <div align=center>

   ![result2](square_detection/result2.png)

   </div>

> 参考文章：
>
> [python opencv检测目标颜色](https://blog.csdn.net/Lingdongtianxia/article/details/75194950)
> 
> [Python图像处理（7）：利用轮廓分块处理](https://blog.csdn.net/lights_joy/article/details/46368197)
> 
> [[opencv]Opencv（Python） 教程-轮廓（2）轮廓特征求取](https://blog.csdn.net/zj360202/article/details/79170265)
> 
> [OpenCV-Python 中文教程15——OpenCV 中的轮廓](https://blog.csdn.net/zichen_ziqi/article/details/80912133)

### background_subtraction（背景减除）

#### KNN

### image_enhance(图像增强)

#### histogram_equalization

在现实的拍摄过程中，比如说视频监控领域，由于其图像的灰度分布集中在较窄的范围内，这就导致了图像的细节不够清晰。为什么不清晰呢，因为灰度分布较窄时，那么，在计算对比度的时候，对比度就很小，所以就不清晰。为了使得图像变得清晰，那么就需要使得灰度值的差别变大，为了使得灰度值的差别变大，就意味着灰度分布就变的较宽，使得灰度值分布变得均匀，在某个灰度级区间内，像素的个数分布大致相同，这样才能使得图像的对比度增强，细节变得清晰可见。

>参考文章
>
> [python数字图像处理（9）：直方图与均衡化](https://blog.csdn.net/denny2015/article/details/50581784)

## 语音(Speech)

### plot_waveform&spectrogram（python绘制.wav文件的波形图和语谱图）

> 参考文章
> 
> [语音信号处理之时域分析-音色及其Python实现](http://ibillxia.github.io/blog/2013/05/18/audio-signal-processing-time-domain-timbre-python-realization/)

### CRF_ChineseWordSegmentation(中文分词)

*本次实验内容是在linux上完成，需要提前安装CRF++（Windows上如果安装CRF++应该也能够运行）。*

#### 关于CRF++模板

CRF中有两种模板U-gram和B-gram。

U-gram：一元模板(unigram template),表示只与当前位置对应的标签相关的特征。

B-gram：二元模板(bigram template),表示前一个位置和当前位置对应的标签相关的特征。

以下面的模板为例：
```
# Unigram  
U00:%x[-2,0]  
U01:%x[-1,0]  
U02:%x[0,0]  
U03:%x[1,0]  
U04:%x[2,0]  
U05:%x[-2,0]/%x[-1,0]/%x[0,0]  
U06:%x[-1,0]/%x[0,0]/%x[1,0]  
U07:%x[0,0]/%x[1,0]/%x[2,0]  
U08:%x[-1,0]/%x[0,0]  
U09:%x[0,0]/%x[1,0]  
   
# Bigram  
B
```
假如我们训练的语料句子是：我是中国人（下标：-2,-1,0,1,2），我们考虑的当前位置为：“中”

U0--U4 特征模板：表示某个位置与当前位置的信息之间的关系，比如说 U00，就是指的 “我” 和“中”之间的联系

U5--U7 特征模板：表示某三个位置与当前位置的信息之间的关系，比如说 U05，就是指的 “我”、“是”、“中” 和“中”之间的联系

U8--U9 特征模板：表示某两个位置与当前位置的信息之间的关系，比如说 U08，就是指的 “是”、“中” 和“中”之间的联系

一般使用 Unigram 就足够了。若使用 Bigram，也使用最简单的模板。它会带来效率低下的效果。

**根据第一篇参考文章及[对应的文件夹下的程序](https://github.com/XinzheTing/Courses/tree/master/MultimediaProcessing/CRF_ChineseWordSegmentation) 就可以轻松的做出基于CRF的中文分词。**
> 参考文章
>
>[中文分词入门之字标注法4](http://www.52nlp.cn/%E4%B8%AD%E6%96%87%E5%88%86%E8%AF%8D%E5%85%A5%E9%97%A8%E4%B9%8B%E5%AD%97%E6%A0%87%E6%B3%A8%E6%B3%954)
>[CRF 算法的特征模板总结](https://blog.csdn.net/sihailongwang/article/details/75340491)
>

## 遇到的问题(Problems)

### About python

#### if \_\_name\_\_=='\_\_main\_\_'

这个是用来区分当前源文件是作为主程序运行还是作为模块被导入其他文件
由于每个python模块（python文件）都包含内置的变量\_\_name\_\_，当运行模块被执行的时候，\_\_name\_\_等于文件名（包含了后缀 .py）。如果import到其他模块中，则\_\_name\_\_等于模块名称（不包含后缀 .py）。而“\_\_main\_\_”等于当前执行文件的名称（包含了后缀 .py）。所以当模块被直接执行时，\_\_name\_\_ == '\_\_main\_\_'结果为真；而当模块被import到其他模块中时，\_\_name\_\_ == '\_\_main\_\_'结果为假，就是不调用对应的方法。

#### 文件路径问题

在使用VS Code编程中发现，当打开的文件夹包含多个子文件夹，而运行的python程序恰好在某一子文件夹内，那么在打开文件时就不能直接输入文件名（即使程序与要打开的文件在同一文件夹下），不知道为什么不能默认先搜索一下程序所在的文件夹。

**解决办法：** 将要打开的文件与程序放到同一文件夹下，然后使用下面两行代码来获取文件的绝对路径。
```python
module_path = os.path.dirname(__file__)
filein = module_path+'/文件名.扩展名'
```


### About OpenCV

#### OpenCV与Matplotlib读取彩色图像模式不同

彩色图像使用OpenCV加载时是BGR模式。但是Matplotlib是RGB模式。所以彩色图像如果已经被OpenCV读取，那它将不会被Matplotlib正确显示。
在python中有一种很简单的转换方式 ```img1 = img[:,:,::-1]```

#### 开启摄像头（打开视频）后没有图像

在做背景减除时发现，运行程序摄像头开启了去不显示图像（我的笔记本电脑在摄像头开启时，摄像头旁边的一个小灯会亮），开以为是电脑问题或是权限不够，然后在网上各种找相关的问题。后来发现是因为程序中缺少一句话：
```python
if cv2.waitKey(1) & 0xFF == ord('q'):
        break
```
那么这句话是什么意思呢？这句话的有一个函数``` cv2.waitKey ```主要作用是等待1ms的键盘输入然后返回输入符号的ASCII码，如果没有输入则返回-1。后面的``` &0xFF ```是取返回值的低八位，主要是针对64位操作系统有时返回值不止8位，等号后面的``` ord('q') ```是计算q键的ASCII码，所以这句话是说“按q键退出”。

不要小瞧了这简单的一段程序，就是因为少了这一句，while循环在飞速的执行，窗口刷新速度太快，导致窗口没有画面显示，而有了这段程序while循环在执行时就会有1ms的暂停，就是这1ms使得图像得以正常显示。无论是从摄像头中读取画面还是从视频中读取，都需要这样一个停顿来时图像正常显示，但是停顿时间不太一样，摄像头由于需要实时性，所以1ms就好（``` cv2.waitKey ```中传参数0则为无限等待直到键盘有输入），但是对于视频，需要其以正常速度播放的话一般设置在25~30ms（不同的等待时间就相当于改变视频的播放速度）

#### RGB2GRAY OR BGR2GRAY

在前面我们提到了OpenCV和matplotlib在加载彩色图像时使用的模式是不同的，OpenCV使用的是BGR模式，所以在将彩色图像转换为灰度图时需要使用``` cv2.cvtColor(img,cv2.COLOR_BGR2GRAY) ```而不是``` cv2.cvtColor(img,cv2.COLOR_RGB2GRAY) ```.

奇怪的是OpenCV对于这两种转化都支持，但是转换结果会有所不同。我在做直方图均衡化的实验时发现了这个问题。

我首先把一幅图像读进来，然后将其转化为灰度图，在我尝试[将灰度图恢复成彩色图像](#灰度图转换为RGB图像(GRAY2RGB))时我发现恢复后的图像与原图在色彩上有较大差别,其原因就是在灰度化时色彩模式选错了。之后我特意尝试了``` cv2.cvtColor(img,cv2.COLOR_BGR2GRAY) ```和``` cv2.cvtColor(img,cv2.COLOR_RGB2GRAY) ```这两种方式在对由OpenCV加载进来的图像(BGR模式)进行灰度化处理后有什么不同，结果如下图：

<div align=center>

![BGR2GRAY && RGB2GRAY](image_enhance/BGR2GRAY&RGB2GRAY.png)

</div>

乍一看感觉这两幅图没什么不一样，但是如果仔细看的话就会发现左图(BGR2GRAY)的颜色比右图(RGB2GRAY)的颜色要浅一些，但要是没有对比的话还真是很难看出来。