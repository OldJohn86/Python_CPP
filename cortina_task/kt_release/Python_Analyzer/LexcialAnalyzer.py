# -*- coding: utf-8 -*-
import re

reserved = {
'if'      :'IF', 
'then'    :'THEN',
'else'    :'ELSE',
'while'   :'WHILE',
'break'   :'BREAK',
'continue':'CONTINUE',
'for'     :'FOR',
'double'  :'DOUBLE',
'int'     :'INT',
'float'   :'FLOAT',
'long'    :'LONG',
'short'   :'SHORT',
'bool'    :'BOOL',
'switch'  :'SWITCH',
'case'    :'CASE',
'return'  :'RETURN',
'void'    :'VOID',
'unsigned':'UNSIGNED',
'enum'    :'ENUM',
'register':'REGISTER',
'typdef'  :'TYPEDEF',
'char'    :'CHAR',
'extern'  :'EXTERN',
'union'   :'UNION',
'const'   :'CONST',
'signed'  :'SIGNED',
'default' :'DEFAULT',
'goto'    :'GOTO',
'sizeof'  :'SIZEOF',
'volatile':'VOLATILE',
'static'  :'STATIC',
'auto'    :'AUTO',
'struct'  :'STRUCT'
}#保留字

type = [
    'Seperator', 'Operator', 'Indetifier', 'String', 'Inum', 'Fnum'
]#类别

regexs = [
    '\{|\}|\[|\]|\(|\)|~|,|;|\.|\?|\:'#界符
    ,'\+|\+\+|-|--|\+=|-=|\*|\*=|%|%=|->|\||\||\|=|/|/=|>|<|>=|<=|=|==|!=|!|&'#操作符
    ,'[a-zA-Z_][a-zA-Z_0-9]*'#标识符
    ,'\".+?\"'#字符串
    ,'\d+'#整数
    ,'-?\d+\.\d+?'#浮点数
]#匹配使用的正则表达式

def DeNote(data): #预处理，去除注释
    temp = re.findall('//.*?\n', data, flags=re.DOTALL)
    if(len(temp) > 0):
        data = data.replace(temp[0], "")
    temp = re.findall('/\*.*?\*/', data, flags=re.DOTALL)
    if(len(temp) > 0):
        data = data.replace(temp[0], "")
    return data

def Scan(line): #经行一次扫描， 返回得到的token以及剩余的字符串
    max = ''
    TargetRegex = regexs[0]
    subindex = 0
    match = False
    for regex in regexs:
        result =re.findall(regex, line, flags=re.DOTALL)
        if(len(result) > 0):
            result = result[0]
            index = line.find(result)
            if(index != 0):
                continue
            else:
                if(len(result) > len(max)):
                    match = True
                    max = result
                    TargetRegex = regex
    if(match == False): #出错处理
        print("不认识的字符: " + line[0])
        return {"data":line[0], "regex":"null", "remain":line[1:]}
    else:
        return {"data":max, "regex":TargetRegex, "remain":line[subindex+len(max):]}

def ScanLine(line, currentline): #对一行进行重复扫描,获得一组token
    tokens = []
    temp = line.strip().strip('\t')
    index = 0
    while True:
        if(temp == ""):
            break
        temp = Scan(temp)
        if(temp['regex'] != "null"):
            token = {}
            token['type'] = type[regexs.index(temp['regex'])]
            if(reserved.get(temp['data']) is not None):
                token['type'] = reserved[temp['data']]
            token['value'] = temp['data']
            token['row']   = (currentline + 1) / 2
            token['colum'] = index
            index = index + len(token['value'])
            if(token['type'] == "Inum"):
                token['value'] = int(token['value'])
            if(token['type'] == "Fnum"):
                token['value'] = float(token['value'])
            tokens.append(token)
        index = index + len(temp['data'])
        temp = temp['remain'].strip().strip('\t')
        if(temp == ""):
            return tokens
    return tokens

def main():
    currentline = 1
    data = DeNote(open('../testCodes/epon.c', 'r').read())
    lines = re.findall('.*?(?=\n)', data)
    tokens = []
    for line in lines:
        temptokens = ScanLine(line, currentline)
        for token in temptokens:
            tokens.append(token)
        currentline = currentline + 1
    for token in tokens:
        print(token)
    return

if __name__ == "__main__":
    main()



