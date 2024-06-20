#!/usr/bin/env python3

from operator import itemgetter
import sys

current_word = None
current_count = 0
word = None

# 从标准输入过来的数据
for line in sys.stdin:
    # 去除左右空格
line = line.strip()
         # 按照tab键进行切分，得到word和次数1
    word, count = line.split('\t', 1)

    # 得到的1是一个字符串，需要类型转化
    try:
        count = int(count)
    except ValueError:
        continue

    # 如果本次读取的单词和上一次一样，对次数加1
    if current_word == word:
        current_count += count
    else:
        if current_word:
            # 输出统计结果
            print ("%s\t%s" % (current_word, current_count))
        current_count = count
        current_word = word

# do not forget to output the last word if needed!
if current_word == word:
    print ("%s\t%s" % (current_word, current_count))
