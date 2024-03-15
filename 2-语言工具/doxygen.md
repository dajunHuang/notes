---
title: Doxygen使用方法
date: 2022/2/4
---
# Doxygen 介绍
&#8195;&#8195;Doxygen是一个程序的文件产生工具，可将程序中的特定注释转换成为说明文件。
&#8195;&#8195;Doxygen是一种开源跨平台的，以类似JavaDoc风格描述的文档系统，完全支持C、C++、Java、Objective-C和IDL语言，部分支持PHP、C#。注释的语法与Qt-Doc、KDoc和JavaDoc兼容。Doxygen可以从一套归档源文件开始，生成HTML格式的在线类浏览器，或离线的LATEX、RTF参考手册。

# Doxygen 安装

&#8195;&#8195;前置要求：Cmake、Flex、Bison等。

&#8195;&#8195;安装指令：
```
sudo apt install cmake/flex/bison

git clone https://github.com/doxygen/doxygen.git
cd doxygen
mkdir build
cd build
cmake -G "Unix Makefiles" ..
make

make install
```
&#8195;&#8195;安装完成后使用`doxygen -g`指令生成配置文件，使用`doxygen Doxygen`编译生成文档。

# Doxygen 注释格式

## 文件注释

&#8195;&#8195;在代码文件头部写上这段注释。可以看到可以标注一些文本名称、作者、邮件、版本、日期、介绍、以及版本详细记录。

```
/**
  * @file     	sensor.c
  * @author   	JonesLee
  * @email   	Jones_Lee3@163.com
  * @version	V4.01
  * @date    	07-DEC-2017
  * @license  	GNU General Public License (GPL)  
  * @brief   	Universal Synchronous/Asynchronous Receiver/Transmitter 
  * @detail		detail
  * @attention
  *  This file is part of OST.                                                  \n                                                                  
  *  This program is free software; you can redistribute it and/or modify 		\n     
  *  it under the terms of the GNU General Public License version 3 as 		    \n   
  *  published by the Free Software Foundation.                               	\n 
  *  You should have received a copy of the GNU General Public License   		\n      
  *  along with OST. If not, see <http://www.gnu.org/licenses/>.       			\n  
  *  Unless required by applicable law or agreed to in writing, software       	\n
  *  distributed under the License is distributed on an "AS IS" BASIS,         	\n
  *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  	\n
  *  See the License for the specific language governing permissions and     	\n  
  *  limitations under the License.   											\n
  *   																			\n
  * @htmlonly 
  * <span style="font-weight: bold">History</span> 
  * @endhtmlonly 
  * Version|Auther|Date|Describe
  * ------|----|------|-------- 
  * V3.3|Jones Lee|07-DEC-2017|Create File
  * <h2><center>&copy;COPYRIGHT 2017 WELLCASA All Rights Reserved.</center></h2>
  */  
```

## **类和成员注释**

&#8195;&#8195;如果对文件、结构体、联合体、类或者枚举的成员进行文档注释的话,并且要在成员中间添加注释,而这些注释往往都是在每个成员后面。为此,可以使用在注释段中使用'<'标识。

```
/**
* @class ASBase
* @brief Functions definitions are at the end of ASResource.cpp
* @author CsOH_DAYY
* @note
* detailed description
*/
class ASBase : protected ASResource
{
private:
	int baseFileType;      /**< a value from enum FileType */

protected:
    /** @brief A constructor. */
	ASBase() : baseFileType(C_TYPE) { }

```

## 函数注释

```
/**
		* @brief		can send the message
		* @param[in]	canx : The Number of CAN
		* @param[in]	id : the can id	
		* @param[in]	p : the data will be sent
		* @param[in]	size : the data size
		* @param[in]	is_check_send_time : is need check out the time out
		* @note	Notice that the size of the size is smaller than the size of the buffer.		
		* @return		
		*	+1 Send successfully \n
		*	-1 input parameter error \n
		*	-2 canx initialize error \n
		*	-3 canx time out error \n
		* @par Sample
		* @code
		*	u8 p[8] = {0};
		*	res_ res = 0; 
		* 	res = can_send_msg(CAN1,1,p,0x11,8,1);
		* @endcode
		*/							
	extern s32 can_send_msg(const CAN_TypeDef * canx,
							const u32 id,
							const u8 *p,
							const u8 size,
							const u8 is_check_send_time);	
```

## 枚举注释

```text
typedef enum
{
    false = 0,  /**< FALSE 0  */
    true = 1    /**< TRUE  1  */
}bool;
```

# Doxyfile

[下载链接(v9s7)](https://pan.baidu.com/s/1WKnz_tptP2DP8a7P539ZlA?pwd=v9s7)

**参考：[百度百科](https://baike.baidu.com/item/Doxygen/1366536?fr=aladdin)、[Doxygen快速入门]([Doxygen快速入门 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/100223113))**