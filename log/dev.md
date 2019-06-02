

开发环境

gcc bison flex


编程语言的语法处理过程一般会经历: 词法分析、语法分析、语义分析、生成代码几个阶段

1、词法分析
    源代码转换为token.

    比如源代码:

    if( flag == 1){
        printf("1\n");

    }else{
        printf("not 1\n");
    }

    经过词法分析会转换为:

    [ if ] [ ( ] [ flag ] [ == ][ 1 ][ ) ][ } ]
        [ printf ][ ( ][ "1" ][ ) ][ ; ] 
    [ } ]
    [ else ]
    [ { ] 
        [ printf ][ ( ][ "not 1" ][ ) ][ ; ]
    [ } ]

    其中每个[] 表示一个token.


2、语法分析

    从token构建parse tree分析树的过程.分析树也称为“syntax tree”语法树,或者“abstract syntax tree”
    AST抽象语法树.

    [ if statement ]- expression ->[ == ]
    [ if statement ]- then ->[ then statement block ]{label: block;}
    [ if statement ]- else ->[ else statement block ]{label: block;}

    [ == ]- left ->[ flag ]
    [ == ]- right ->[ 1 ]

    [ then statement block ]{label: block;}- states ->[ printf ]
    [ printf ]- args ->[ "1" ]

    [ else statement block ]{label: block;}- states ->[ printf0 ]{label:printf;}
    [ printf0 ]- args ->[ "not 1" ]

=============

 
+---------+  then   +--------------+  else    +-------+  states   +--------+  args   +---------+
|  block  | <------ | if statement | -------> | block | --------> | printf | ------> | "not 1" |
+---------+         +--------------+          +-------+           +--------+         +---------+
  |                   |
  | states            | expression
  v                   v
+---------+         +--------------+  right   +-------+
| printf  |         |      ==      | -------> |   1   |
+---------+         +--------------+          +-------+
  |                   |
  | args              | left
  v                   v
+---------+         +--------------+
|   "1"   |         |     flag     |
+---------+         +--------------+


3、语义分析
4、生成代码


|
|



