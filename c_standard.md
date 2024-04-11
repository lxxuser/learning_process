##### gnu关键词

```
alias:      设置函数别名。
aligned:    设置函数对齐方式。
always_inline/gnu_inline: 
函数是否是内联函数。
constructor/destructor:
主函数执行之前、之后执行的函数。
format:
指定变参函数的格式输入字符串所在函数位置以及对应格式输出的位置。
noreturn:
指定这个函数没有返回值。
请注意，这里的没有返回值，并不是返回值是void。而是像_exit/exit/abord那样
执行完函数之后进程就结束的函数。
weak：指定函数属性为弱属性，而不是全局属性，一旦全局函数名称和指定的函数名称
命名有冲突，使用全局函数名称。


#include <stdio.h>

void before() __attribute__((constructor));
void after() __attribute__((destructor));

void before() {
    printf("this is function %s\n",__func__);
    return;
}

void after(){
    printf("this is function %s\n",__func__);
    return;
}

int main(){
    printf("this is function %s\n",__func__);
    return 0;
}

// 输出结果
// this is function before
// this is function main
// this is function after
```

##### 二进制求1的位数

```
#include <stdio.h>
int func(int x)
{
     int countx = 0;
     while(x)
     {
      countx++;
      x = x&(x-1);
     }
	 return countx;
}
int main()
{
     printf("%d\n",func(9999));
     return 0;
}
```

