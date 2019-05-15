from anytree import Node, PostOrderIter
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

input_txt = "producer0.txt"  # 产生式文件
with open(input_txt) as f:
    lines = f.readlines()

queue = []
code_queue = []
root = Node("program")

queue.append(root)

for line in lines[:-1]:
    left_right = line.split('->')
    left = left_right[0]
    right = left_right[1].split(' ')[:-1]
    left_root = queue.pop()

    for right_item in right:
        # print('left: ', left_root.name)
        node = Node(right_item, parent=left_root)
        code_queue.append(node)

    code_queue.reverse()

    for item in code_queue:
        if left == 'ID' or left == 'NUM':
            break
        if item.name in nonterminal_table:
            queue.append(item)

    code_queue.clear()

for node in PostOrderIter(root):
    if node.name in nonterminal_table:
        continue
    else:
        if node.name == 'empty':
            continue
    code_queue.append(node.name)

in_code = 'precode.txt' # 预处理后的文件
with open(in_code, 'r')as in_f:
    line = in_f.read()
read_list = line.split(' ')
if read_list[:-1] == code_queue:  # 比较重新还原的文件与预处理后的文件是否一致
    print(True)
else:
    print(False)
