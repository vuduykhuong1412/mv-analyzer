#!/usr/bin/python
# Filename : putallinone.py
# Usage : python putallinone.py [xxxxx], 其中xxxxx是你希望的输出文件名前缀
import glob
import sys

# 如果给出了输出文件名前缀，则用之，否则使用'output'做为文件名
if len(sys.argv) < 2:
	outfilename='output'
else:
	outfilename=sys.argv[1]

outfile = file(outfilename+".mv",'w')

# 写入文件头
outfile.write('<?xml version="1.0" encoding="UTF-8"?>\n')
outfile.write('<file name="'+outfilename+'.cif">\n')

# 挨个合并文件，按数字顺序进行
for frame in glob.glob('frame*.xml'):
	print frame+' ...',
	fp = file(frame)
	outfile.write(fp.read())
	fp.close()
	print 'done'

# 写入文件尾
outfile.write('</file>\n')
outfile.close()

# 输出完成信息
print 'concatenate into '+outfilename+'.mv, finished!'

