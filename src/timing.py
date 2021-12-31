import time

class Timer:
    def __init__(self):
        self.timer_start = time.time()
    def TimerEnd(self, msg, logger):
        timer_end = time.time()
        time_elapsed_in_ms = round((timer_end - self.timer_start) * 1000, 2)
        logger.Log("Total time for {} = ".format(msg) + str( time_elapsed_in_ms ) + " ms" )
