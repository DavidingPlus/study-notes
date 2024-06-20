#!/usr/bin/env python3
import sys

# 从标准输入过来的数据
for line in sys.stdin:
    # 将首位的空格去掉
line = line.strip()
    # 将这一行文本切分成单词（按空格）
words = line.split()
    # 读一个单词写出一个<单词,1>
    for word in words:
        print("%s\t%s" % (word, 1)
