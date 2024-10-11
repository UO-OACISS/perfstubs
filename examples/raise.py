def baz():
    print("baz")
    raise Exception("an exception happened")

def bar():
    print("bar")
    try:
        baz()
    except Exception as e:
        print("Caught exception")

def foo():
    print("foo")
    bar()

class B(Exception):
    pass

class C(B):
    pass

class D(C):
    pass

def foo2():
    for cls in [B, C, D]:
        try:
            raise cls()
        except D:
            print("D")
        except C:
            print("C")
        except B:
            print("B")

def inner():
    raise Exception("throwing this?")

def outer():
    inner()

if __name__ == '__main__':
   foo()
   foo2()
   outer()