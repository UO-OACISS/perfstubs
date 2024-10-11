def read_it(filename):
    for line in open(filename, 'rb'):
        yield line

def tweener():
    pass

def foo(filename):
    obj = read_it(filename)
    print('reading:',filename)
    numlines = 0
    for line in obj:
        numlines += 1
        tweener()
    print(numlines,"lines in file")

def foo2(filename):
    #with exception handling:
    obj2 = read_it(filename)
    print('reading:',filename)
    numlines = 0
    while True:
        try:
            line = next(obj2)
        except StopIteration:
            break
        numlines += 1
    print(numlines,"lines in file")

def infinite_sequence():
    num = 0
    while True:
        yield num
        num += 1

def is_palindrome(num):
    # Skip single-digit inputs
    if num // 10 == 0:
        return False
    temp = num
    reversed_num = 0

    while temp != 0:
        reversed_num = (reversed_num * 10) + (temp % 10)
        temp = temp // 10

    if num == reversed_num:
        return num
    else:
        return False

def foo3():
    for i in infinite_sequence():
        pal = is_palindrome(i)
        if pal:
            print(i, end=' ')
        if i > 10:
            break
    print("")

def foo4():
    # create a generator object
    nums_squared_gc = (num**2 for num in range(5))
    for n in nums_squared_gc:
        print(n, end=' ')
    print("")

def infinite_palindromes():
    num = 0
    while True:
        if is_palindrome(num):
            i = (yield num)
            if i is not None:
                num = i
        num += 1

# coroutine example with throw
def foo5_inner():
    pal_gen = infinite_palindromes()
    for i in pal_gen:
        digits = len(str(i))
        if digits == 5:
            pal_gen.throw(ValueError("We don't like large palindromes"))
        pal_gen.send(10 ** (digits))

def foo5():
    try:
        foo5_inner()
    except Exception as e:
        pass


# coroutine example with close
def foo6_inner():
    pal_gen = infinite_palindromes()
    for i in pal_gen:
        digits = len(str(i))
        if digits == 5:
            pal_gen.close()
        pal_gen.send(10 ** (digits))

def foo6():
    try:
        foo6_inner()
    except Exception as e:
        pass

if __name__ == '__main__':
    filename = 'yield.py'
    foo(filename)
    foo2(filename)
    foo3()
    foo4()
    foo5()
    foo6()