# 新手代码--Robomaster装甲板识别
---
参考了CSDN上的代码
---
## 介绍  
本代码是Robomaster步兵视觉算法中的装甲板识别Demo实现;
---
## 1.装甲板识别目标特征分析
### 灯条的光色特征:
识别灯条在低曝光的情况下识别，在这种情况下灯条颜色更容易识别，二值化处理后灯条更为突出：
### 灯条之间的几何关系：
1.一个装甲板的左右灯条相互平行；<br>
2.筛除面积很小的轮廓（轮廓面积大于一定值)；<br>
3.轮廓的长宽比大于一定值；<br>
4.只识别目标颜色的灯条；<br>
### 装甲板的特点：
1.中间有需要识别的装甲板编号；<br>
2.两个匹配的左右灯条确定唯一一个装甲板；<br>
3.装甲板轮廓面积大于一定值；<br>
4.轮廓长宽比大于一定值；<br>
5.确定装甲板的中心位置和四个顶点，筛选出分数最高者；<br>
### 总结
由于装甲板是要追踪和打击的对象，要通过以下要素计算装甲板的分数，得分高者优先打击：<br>
1.装甲板距离视野中心的远近；<br>
2.装甲板在视野中的有效面积（正对面积）；<br>
3.装甲板打击中心的位置，角度；<br>

---
## 2.关于灯条与装甲板特征的思考：
 
1.可不可以研究三个灯条之间的关系：再定义一个距离比，第一个灯条与第二个灯条之间的距离比上第二个灯条与第三个灯条的距离比，用这个筛选条件，筛选两两匹配的装甲板的灯条；（对于距离较远的装甲板识别，对图像二值化，轮廓识别后灯条之间的角度关系，平行关系等特征变得不明显，而装甲板左右灯条距离与非装甲板的两个灯条距离的比值特征突出，容易识别）；<br>
2.为了防止装甲板中心点识别时出现再视野中两个有效装甲板中心点之间飘忽不定，可以设置视野中心位置，当装甲板离开视野中心位置时，中心打靶点就就不再识别该装甲板；<br>

### 算法详解：
I．在armor detection中建立两个<br>
II．在armor detection中，先用红蓝通道相减法筛出红/蓝色灯条；再在合适的阈值范围内对灯条轮廓进行提取；<br>
III．对灯条轮廓进行适当膨胀，筛除较小的轮廓，对轮廓进行绘制；<br>
IV．把灯条的相关数据带入循环筛选抉择，两两匹配获得装甲板信息；<br>
V．对匹配出的装甲板进行筛选，计算装甲板的得分比较出得分最高者，追踪得分最高者的中心位置；<br>
VI．在main.cpp中设置函数，计算fps；<br> 
<div align=center>
<img src="https://github.com/zgrRoot/armor-detection/new/master/final.jpg" width = "400" alt="结果图"/>
</div> 
---
 
