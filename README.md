
### 简介
此项目中实现了一些C++的类。这些类有的非常实用，有的实现了一些巧妙的功能。  
项目中的类多数彼此独立，可以单独使用。

### 详细说明

#### ScopeGuard
ScopeGuard用来保证在退出作用域时执行指定的操作。  
在定义ScopeGuard对象时，需要传入一个`std::function<void(void)>`对象。当程序流程退出ScopeGuard对象所在的作用域时，执行这个function对象。

如果你不想在退出时调用function对象，可以显式调用dismiss方法。dismiss方法被调用后，ScopeGuard对象在作用域退出时不会执行任何操作。

如果你无论如何都会执行构造时传入的function对象，那么可以使用SCOPE_EXIT宏。这个宏自动为你定义一个ScopeGuard对象。

使用示例：
```c++
#include "scope_guard.h"

FILE* open_zip_file(const char* path)
{
  FILE* fp = fopen(path);
  if (0 == fp)
    return false;
  ScopeGuard fp_guard([fp](){fclose(fp); });

  //check if this is a zip file.
  if (!success)
    return 0;

  //this is a zip file, we will return it.
  fp_guard.dismiss();
  return fp;
}
```
SCOPE_EXIT宏使用示例：
```c++
bool is_zip_file(const char* path)
{
  FILE* fp = fopen(path);
  if (0 == fp)
    return false;
  SCOPE_EXIT([fp](){fclose(fp); });

  //check if this is a zip file.
  if (!success)
    return true;
  else
    return false;
}
```
