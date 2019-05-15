from graphviz import Graph
nonterminal_table = ["program",
                     "declaration-list",
                     "additive-expression",
                     "additive-expression-1",
                     "addop",
                     "arg-list",
                     "arg-list-1",
                     "args",
                     "compound-stmt",
                     "declaration",
                     "declaration-1",
                     "declaration-list-1",
                     "expression",
                     "expression-1",
                     "expression-2",
                     "expression-stmt",
                     "factor",
                     "factor-1",
                     "fun-declaration",
                     "iteration-stmt",
                     "local-declarations",
                     "local-declarations-1",
                     "mulop",
                     "param",
                     "param-1",
                     "param-list",
                     "param-list-1",
                     "params",
                     "params-1",
                     "relop",
                     "return-stmt",
                     "return-stmt-1",
                     "selection-stmt",
                     "selection-stmt-1",
                     "simple-expression",
                     "simple-expression-1",
                     "statement",
                     "statement-list",
                     "statement-list-1",
                     "term",
                     "term-1",
                     "type-specifier",
                     "var-1",
                     "var-declaration",
                     "var-declaration-1",
                     "ID","NUM"]

input_txt = "producer2.txt"
with open(input_txt) as f:  # 读取推导的产生式
    lines = f.readlines()

queue = []
temp = []

dot = Graph(comment='The Syntax Analysis Tree')

root = dot.node('program', 'program')
queue.append('program')

for index, line in enumerate(lines[:-1]):
    left_right = line.split('->')  # 产生式的左部
    left = left_right[0]
    right = left_right[1].split(' ')[:-1]	# 产生式的右部
    left_root = queue.pop()  # 产生式的左部所在结点出栈
    for right_item in right:
        right_new_name = right_item + str(index)
        dot.node(right_new_name, right_item)  # 画点
        dot.edge(left_root, right_new_name)  # 画边
        temp.append((right_new_name,right_item))

    temp.reverse()

    for str_name in temp:
        if left == 'ID' or left == 'NUM':
            break
        if str_name[1] in nonterminal_table:  # 判断是否在非终结符表中，或是否为NUM、ID
            queue.append(str_name[0])  # 满足条件才入栈成为父节点
    temp.clear()
dot.render('graphtest.gv', view=True) # 输出PDF