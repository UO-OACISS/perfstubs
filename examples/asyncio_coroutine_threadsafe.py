import asyncio
import threading
import time


async def coro_func():
    return await asyncio.sleep(3, 42)


def another_thread(_loop):
    coro = coro_func()  # is local thread coroutine which we would like to run in another thread

    # _loop is a loop which was created in another thread

    future = asyncio.run_coroutine_threadsafe(coro, _loop)
    print(f"{threading.current_thread().name}: {future.result()}")
    time.sleep(15)
    print(f"{threading.current_thread().name} is Finished")


if __name__ == '__main__':
    loop = asyncio.get_event_loop()
    main_th_cor = asyncio.sleep(10)
    # main_th_cor  is used to make loop busy with something until another_thread will not send coroutine to it
    print("START MAIN")
    x = threading.Thread(target=another_thread, args=(loop, ), name="Some_Thread")
    x.start()
    time.sleep(1)
    loop.run_until_complete(main_th_cor)
    print("FINISH MAIN")