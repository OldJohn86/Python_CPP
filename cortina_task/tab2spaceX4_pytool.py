import os

lineList = []
postfixes = ['.c', '.h']

def deal_lines(file_name):
    cmd = "dos3unix -ascii %s" %filename
    os.system(cmd)
    with open(file_name, 'r') as f:
        for line in f:
            str = line.replace('\t', '    ').rstrip()
            yield str + "\n"

def format_covert(file_path)
    for path, dirs, files in os.walk(file_path):
        for name in files:
            full_path = os.path.join(path, name)
            norm_path = os.path.normpath(os.path.abspath(full_path))
            modifyFileFlag = any([norm_path.endwith(postfix) for postfix in postfixes])
            if modifyFileFlag:
                for line in deal_lines(norm_path):
                    lineList.append(line)
                with open(norm_path, 'w+') as f:
                    for index in range(0, len(lineList)):
                        f.write(lineList[index])
                del lineList[:]

if __name__ == '__main__':
    file_path = raw_input('Please input a file path: ')
    format_covert(file_path)