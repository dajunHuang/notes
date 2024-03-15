---
title: UNIX 数据处理汇总
date: 2023/09/10
---
# [Shell 正则](https://www.gnu.org/software/bash/manual/html_node/Shell-Expansions.html)
单引号`‘`包起来的内容都是字面值

双引号`“`包起来的内容中除了`$`, `\\``, `\`都是字面值

`*`、`?`、`[]`的用法和正则表达式类似。

`^`变成了否的意思。

没有`.`。

`[:upper:]`。

`$#`: 脚本参数个数。	

`$@`、`$*`: 脚本所有参数

`${#var}`: 变量长度。

`${varname:offset:length}`: 字符串的一部分，可以从末尾开始。

`((var=3+3))`: 整数计算

`$?`：最近返回值。

`$$`：当前shell的pid

`$!`：最近加入后台的进程的pid

`$0`：shell script 名字

`$N`：第N个参数的名字

`$_`: 上一条命令的最后一个参数

```
bash$ echo a{d,c,b}e
ade ace abe
$(command)
`command`
$(( expression ))
```

# [find](https://www.man7.org/linux/man-pages/man1/find.1.html)
## 位置选项
`-P`：忽略符号链接

`-L`：访问符号链接

`-depth level`：先访问文件夹里的内容，再访问文件夹本身，默认先访问本身

`-maxdepth level`：设置最大深度，起始点为0

`-mindepth level`：设置最小深度，起始点为0

`-mount`：不访问其它文件系统的文件

## 筛选选项
```
此处用于比较的数字n可以是+n(比n大)或-n(比n小)
```
`-amin n`：最近访问时间到现在过了多少分钟和n比较

`-atime n`：最近访问时间到现在过了多少个24h和n比较

`-anewer reference`：是否比`reference`文件更新

```
ctime、mtime和上面三个同理，是状态修改的时间，数据修改时间（第三个是newer）
```

`-empty`：文件为空

`-fstype type`：文件系统是`type`

`-name pattern`：匹配文件名，pattern是一个字符串[shell pattern](https://www.gnu.org/software/findutils/manual/html_node/find_html/Shell-Pattern-Matching.html)
，例如`find -name '*macs'`

`-regex pattern`：匹配文件名，pattern是一个正则表达式字符串

`-path pattern`：匹配路径名，pattern是一个shell pattern字符串

```
对应地，有大小写不敏感的iname、ipath、iregex
```
`-readable`：当前用户和有权限读

`-samefile name`：和文件名name是一个inode

`-size n[cwbkMG]`：文件大小和n比较，b：512-byte block，c：bytes，w：2 bytes words，k：kB，M：mb，G:GB

`-type [dfpl]`：文件类型d：目录，f：文件，p：管道，l：连接

## 执行操作
`--delete`：删除文件或路径

`--exec command ;`：为找到的每个文件执行命令，如`find . -type f -exec file {} \;`

`--exec command +`：把找到的文件名拼一起再执行命令，如`find . -type f -exec echo {} +`
```
还有一些打印的操作见手册
```

# regex

[在线 ReGex](https://www.regex101.com)
```
\d	Any Digit
\D	Any Non-digit character
.	Any Character
\.	Period
[abc]	Only a, b, or c
[^abc]	Not a, b, nor c
[a-z]	Characters a to z
[0-9]	Numbers 0 to 9
\w	Any Alphanumeric character
\W	Any Non-alphanumeric character
{m}	m Repetitions
{m,} at least m Repetitions
{,n} at most n Repetitions
{m,n}	m to n Repetitions
*	Zero or more repetitions
+	One or more repetitions
?	Optional character
\s	Any Whitespace
\S	Any Non-whitespace character
^…$	Starts and ends
(…)	Capture Group
(a(bc))	Capture Sub-group
(.*)	Capture all
(abc|def)	Matches abc or def
[[:upper:]]
```
[more about grep regex](https://www.gnu.org/software/grep/manual/grep.html#Character-Classes-and-Bracket-Expressions-1)

# [vim find & substitute](https://vim.fandom.com/wiki/Search_and_replace)

替换指令格式:`:s/foo/bar/g`，把当前行的所有foo替换为bar

## `s`可替换为
`%s`：整个文档替换

`5,12s`：5-12行替换

`.,$s`：当前行到文档末替换

`.,+2s`：下两行替换

## `foo`和`bar`需要注意

`.`,`*`,`\`,`[`,`^`,`$`首先使用特殊用法

`+`,`?`,`|`,`&`,`{`,`(`,`)`首先使用字面意义

`\0`是匹配到的整个字符串，`\1`是匹配到的第一个参数捕获列表

用`\zs`和`\ze`指定替换范围，例如`:s/Copyright \zs2007\ze All Rights Reserved/2008/`替换数字

## `g`可替换为

`null`：只匹配当前行

`c`：替换前确认

`i`：大小写敏感

`I`：大小写不敏感

[sort](https://man7.org/linux/man-pages/man1/sort.1.html)

# [grep、egrep](https://www.gnu.org/software/grep/manual/grep.html)
```
Basic special character:    .*^$\[
extended special character: .*^$\[ +{|()
grep = grep -G、egrep = grep -E、fgrep = grep -F
```

`-i`：忽略大小写

`-v`：反向匹配

`-c`：仅计数

`--max-count=num`：最多匹配num行

`-o`：只输出匹配的结果

`-q`：不打印，匹配到则返回0

`-l`：仅打印文件名

`-b`：打印次行的字节偏移

`-H`：打印文件名

`-n`：打印行号

`-r dir`：搜索文件夹dir中所有文件的内容

`--exclude`、`--exclude-from`、`--exclude-dir`：忽略文件或文件夹

# [sed](https://www.gnu.org/software/sed/manual/html_node/index.html)

`-e script`、`-f script-file`：指定（文件或字符串）模板输入

`-i`：对文件原地替换，不输出

`-r`、`-E`：使用扩展Regex表达式

```
Basic special character:    .*^$\[]
extended special character: .*^$\[] ?+{}|()

sed -e 's/regexp/replacement/[flags]' input.txt > output.txt
```
[`s`命令](https://www.gnu.org/software/sed/manual/html_node/The-_0022s_0022-Command.html#The-_0022s_0022-Command)


# [awk](https://www.gnu.org/software/gawk/manual/html_node/index.html)
```
BEGIN { rows = 0 }
$1 == 1 && $2 ~ /^c[^ ]*e$/ { rows += $1 }
END { print rows }
```
```
awk 'program' input-file1 input-file2 …
awk -f program-file input-file1 input-file2 …
```
`-f program-file`：指定脚本

`-F ':'`：制定分割符，分割得到`$1`、`$2`、`$3`...

$NF代表最后一个字段

[简要教程from阮一峰](https://www.ruanyifeng.com/blog/2018/11/awk.html)

# Makefile

TODO
