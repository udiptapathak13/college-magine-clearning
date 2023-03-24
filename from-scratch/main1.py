from ctypes import *

Dataframe = CDLL('./lib/libdframe.so')
Label = CDLL('./lib/liblabel.so')
Dataset = CDLL('./lib/libdset.so')

df = Dataframe.readCSV(c_char_p(bytes('drug.csv', 'ascii')))

Label.labelEncoder(c_void_p(df), c_char_p(bytes('Sex', 'ascii')))

ds = Dataset.setTargetFeature(c_void_p(df), c_char_p(bytes('Drug', 'ascii')))
Dataset.trainTestSplit(c_void_p(ds), c_double(0.8))
