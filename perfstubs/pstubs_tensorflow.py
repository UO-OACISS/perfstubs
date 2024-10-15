#! /usr/bin/env python

"""Python interface for PerfStubs profiling of TensorFlow."""

import perfstubs
import pstubs_common as ps

try:
    import tensorflow.keras as keras
    class PstubsTensorFlowCallbacks(keras.callbacks.Callback):

        def __init__(self):
            perfstubs.initialize()
            self.train_stack = []
            self.epoch_stack = []
            self.test_stack = []
            self.predict_stack = []
            self.train_batch_stack = []
            self.test_batch_stack = []
            self.predict_batch_stack = []

        def __del__(self):
            perfstubs.finalize()

        def on_train_begin(self, logs=None):
            x = "Training"
            perfstubs.start(x,"",0)
            self.train_stack.append(x)

        def on_train_end(self, logs=None):
            x = self.train_stack.pop()
            perfstubs.stop(x,"",0)

        def on_epoch_begin(self, epoch, logs=None):
            x = "Epoch {}".format(epoch)
            perfstubs.start(x,"",0)
            self.epoch_stack.append(x)

        def on_epoch_end(self, epoch, logs=None):
            x = self.epoch_stack.pop()
            perfstubs.stop(x,"",0)

        def on_test_begin(self, logs=None):
            x = "Test"
            perfstubs.start(x)
            self.test_stack.append(x)

        def on_test_end(self, logs=None):
            x = self.test_stack.pop()
            perfstubs.stop(x,"",0)

        def on_predict_begin(self, logs=None):
            x = "Predict"
            perfstubs.start(x,"",0)
            self.predict_stack.append(x)

        def on_predict_end(self, logs=None):
            x = self.predict_stack.pop()
            perfstubs.stop(x,"",0)

        def on_train_batch_begin(self, batch, logs=None):
            x = "Training Batch {}".format(batch)
            perfstubs.start(x,"",0)
            self.train_batch_stack.append(x)

        def on_train_batch_end(self, batch, logs=None):
            x = self.train_batch_stack.pop()
            perfstubs.stop(x,"",0)

        def on_test_batch_begin(self, batch, logs=None):
            x = "Testing Batch {}".format(batch)
            perfstubs.start(x,"",0)
            self.test_batch_stack.append(x)

        def on_test_batch_end(self, batch, logs=None):
            x = self.test_batch_stack.pop()
            perfstubs.stop(x,"",0)

        def on_predict_batch_begin(self, batch, logs=None):
            x = "Predict Batch {}".format(batch)
            perfstubs.start(x,"",0)
            self.predict_batch_stack.append(x)

        def on_predict_batch_end(self, batch, logs=None):
            x = self.predict_batch_stack.pop()
            perfstubs.stop(x,"",0)
except Exception:
    # Do nothing if TensorFlow isn't available
    pass

