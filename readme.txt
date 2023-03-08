词法分析DFA的构建:
    regular2minDFA.cpp  文件是正则表达式转化最小化DFA的源代码，相应的可执行文件为regular2minDFA.exe
    该文件的输入是 regular2minDFA-input.txt ，在regular2minDFA-input中书写词法的正则表达式规则，格式为一行token名，一行正则表达式
    正则表达式的写法规则： [x-y]表示x到y的所有ascill字符，()括号表示优先级，*表示闭包运算，?表示可以没有前方的一段字符。
    该文件输出为 analysis-dfaMap-input.txt,文件中的内容是所有的token名和DFA图，用于分析文件的 lexical_analysis 的输入
词法分析:
    lexical_analysis.cpp 文件是词法分析的源代码，相应的可执行文件为 lexical_analysis.exe
    调用时需要一个命令行参数来指定要编译的文件全名 形如"lexical_analysis test.cpp"
    该文件的输入是 analysis-dfaMap-input.txt ，读入生成好的DFA图，用来进行词法分析
    该文件输出为 grammar_token.txt,文件中的内容是去掉了词法错误的词之后的token序列，token序列的格式是一行一个，输入的词 token名 
    还有一个输出是 lexical_errors.txt，该文件内容是源代码错误信息的存储，展示给用户
语法分析表的构建：
    grammar2parsetable.cpp 文件是正则表达式转化最小化DFA的源代码，相应的可执行文件为 grammar2parsetable.exe
    该文件的输入是 grammar_rules.txt ，在grammar_rules.txt文件中书写语法的正则文法规则
    产生式的写法规则：a =  b c,a,b,c为终结符号或非终结符号    
    该文件输出为 grammar_parse_table.txt,文件中的内容是分析表，用于语义分析文件 grammar_semantic_intermediatecode 的读入 
语法分析，语义分析，中间代码的生成、优化:
    grammar_semantic_intermediatecode.cpp 文件是语法分析，语义分析，中间代码的生成、优化的源代码，相应的可执行文件为grammar_semantic_intermediatecode.exe
    该文件的输入有两个，1.grammar_parse_table.txt,文件中的内容是分析表，用来进行分析表
			2.grammar_token.txt，是词法分析生成的token序列
    文件的输出也有两个，1.quaternion_file.txt 存放传给目标代码生成的格式的四元式
			2.show_quaternion.txt 存放比较直观的四元式，展示给用户

目标代码生成：
    mips_generation.cpp 文件是目标代码生成的源代码，相应的可执行文件为mips_generation.exe
    该文件的输入是 quaternion_file.txt ,是上一步生成的四元式
    该文件的输出是 mips.txt ，是最终的目标代码文件，内容是可以进行编译执行的mips源代码
用户使用界面：
    用python语言做了简单的命令行控制程序，方便用户使用。
    compiler.py 文件是用户界面的源文件，相应的可执行文件为 compiler.exe
    该文件没有输入输出，用户根据提示操作即可。
    值得注意的生成新的词法语法分析表一定要先把对应的规则文件进行更改，并保证文件在当前目录下。