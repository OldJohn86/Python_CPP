import LexcialAnalyzer as lex
line='printf("\n", i)'
tokens = lex.ScanLine(line, 0)
for token in tokens:
    print(token)
