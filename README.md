数据类型
==========

整数
----------

int8,  byte  - 1字节，有符号整数

int16, short - 2字节，有符号整数

int32, int   - 4字节，有符号整数

int64, long  - 8字节，有符号整数


uint8,  ubyte  - 1字节，无符号整数

uint16, ushort - 

uint32, uint   - 

uint64, ulong  -

字符串
----------

字符串头部始终包含一个无符号整数类型的长度信息，空字符串的长度为0。

string8, string  - 长度信息占用1个字节

string16         - 长度信息占用2个字节

string32         - 长度信息占用4个字节

string64         - 长度信息占用8个字节

列表
----------

列表头部包含一个无符号整数类型的列表长度信息，空列表的长度为0。

list8, list  - 长度信息占用1个字节

list16       - 长度信息占用2个字节

list32       - 长度信息占用4个字节

list64       - 长度信息占用8个字节

列表后面必须紧跟列表元素的描述信息。

示例 1:

    items : list
    {
        id : int,
        name : string
    }

枚举
----------

与C语言不一样，枚举声明必须有值和值类型

示例 2:

    enum color : byte
    {
        red = 1, green = 2, blue = 3
    }

文档格式
==========

包和定义
----------

示例 3：

    pkg player = 1
    {
      enum login_state
      { 
        INIT    = 0,
        FAILED  = 1,
        SUCCEED = 2
      }

      def login_params = 0
      {
        user_name  : string,
        hash_code  : string,
        login_time : int
      }

      def login_result = 1
      {
        state : enum<login_state>
      }
    }

包可以嵌套定义，目的是更灵活的组织代码，比如登录接口有上下行数据，
按示例3那样分开声明没有问题，但是显得分散不利于阅读。可以尝试像示例4一样组织代码。

示例 4:

    pkg player = 1
    {
      pkg login
      {
        enum state
        { 
          INIT    = 0, 
          FAILED  = 1,
          SUCCEED = 2
        }

        def in = 0
        {
          user_name  : string,
          hash_code  : string,
          login_time : int
        }

        def out = 1
        {
          state : enum<state>
        }
      }
    }

具体的组织方式应该根据自身项目特点决定，并没有绝对的好坏。但是建议每个项目
只按一种规则书写BS代码，这是从工程化角度出发。

注释
----------

BS支持单行注释和多行注释，单行注释使用 // 起始，到行尾结束

多行注释使用 /* 起始，*/ 结束，多行注释不支持嵌套

