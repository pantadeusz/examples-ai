#  cat btc_investing_com.csv | sed 's/","/;/g' | sed 's/"//g' | sed 's/,//g' | sed 's/K/*1000/g' | sed 's/%/*0.01/g' | sed 's/M/*1000*1000/g' | sed 's/B/*1000*1000*1000/g' | sed 's/Change.*/Change/g'  > btc_investing_com_c.csv
import pandas as pd
import numpy as np
import logging
import tensorflow as tf
tf.get_logger().setLevel(logging.CRITICAL)
import datetime
from tensorflow.keras.models import Sequential, load_model
from tensorflow.keras.layers import LSTM, Dense, Dropout
import math

def compute_val(s):
    if str(s) == '<NA>': return 0.0
    return eval(str(s))

def get_historical_data(csvname):
    data = pd.read_csv(csvname, sep=";")
#    Date;Price;Open;High;Low;Vol.;Change
#    03/24/2024;67211.9;64036.5;67587.8;63812.9;65.59*1000;4.96*0.01

    data["Date"] = pd.to_datetime(data["Date"], format='%m/%d/%Y').map(pd.Timestamp.timestamp)
    data["Vol."] = data["Vol."].astype('string').apply(compute_val)
    data["Change"] = data["Change"].astype('string').apply(compute_val)
    data = data.sort_values("Date",ascending=True)
    columns_to_use = ['Date','Price','Open','High','Low','Vol.','Change']
    data = data[columns_to_use].values
    return data, columns_to_use

def preprocess_data_rows_for_input(input_data):
    day = 24*60*60
    year = year = (365.2425)*day
    input_data = np.array(input_data,dtype=float)
    def transformed_row(row):
        return np.concatenate(([np.sin(row[0] * (2 * np.pi / year)), np.cos(row[0] * (2 * np.pi / year))], 
                               np.log10(np.fmax(row[1:5],np.ones(row[1:5].shape)*0.001))))
    ret = [ transformed_row(row) for row in input_data]
    ret = np.array(ret)
    return ret

def preprocess_inputs_to_data_rows(input_data):
    input_data = np.array(input_data,dtype=float)
    def transformed_row(row):
        return [ 10.0**i for i in row[2:6] ]
    ret = [ transformed_row(row) for row in input_data]
    ret = np.array(ret)
    return ret

def generate_x_y(data_rows, sequence_length=14, shuffle=True):
    sequence_length = 14
    X, y = [], []

    for i in range(len(data_rows) - sequence_length):
        X.append(data_rows[i:i + sequence_length])
        y.append(data_rows[i + sequence_length][2])  # Predicting price

    if shuffle:
        seq = np.random.permutation(len(X))
        return np.array(X)[seq], np.array(y)[seq]
    else:
        return np.array(X), np.array(y)


def predict(model, X_test, y_test):
    predictions = model.predict(X_test, verbose=0)

    X_test_original =      np.array([ [ 10.0 ** x for x in y[2:] ] for y in X_test[0] ])
    y_test_original =      [ 10.0 ** x for x in y_test ]
    predictions_original = [ 10.0 ** x for x in predictions ]

    print("Prediction      :", predictions_original, np.array(predictions_original).shape)
    print("True values     :", y_test_original, np.array(y_test_original).shape)
    print("X test last     :", X_test_original[-1])
    print("Decision: ", 
          float(predictions_original[0][0]), 
          float(X_test_original[-1][0]), 
          "BUY" if float(predictions_original[0][0]) > float(X_test_original[-1][0]) else "SELL" )


input_data, columns_to_use = get_historical_data("btc_investing_com_c.csv")
data_rows = preprocess_data_rows_for_input(input_data)


# 3. Build the LSTM model
model = Sequential([
    LSTM(64, activation='relu'), #, input_shape=(None, data_rows.shape[1])), # variable sequence length
    Dense(256, activation='relu'),  # Predicting a single value
    Dense(256),  # Predicting a single value
    Dense(1)  # Predicting a single value
])


optimizer = tf.keras.optimizers.Adam(learning_rate=0.0005)
model.compile(optimizer=optimizer, loss='mse')

try:
    model = load_model("prediction.keras")
except:
    print("training model from scratch")


# 4. Train the model
X, y = generate_x_y(data_rows, 60, shuffle=False)
print(X.shape)
history = model.fit(X, y, epochs=10, batch_size=8, validation_split=0.1, verbose=1, shuffle=True)

model.save("prediction.keras")

#predict(model,X_test,y_test)
print("PREDICTION FOR " , data_rows[-1], int(input_data[-1][0]),datetime.datetime.fromtimestamp(int(input_data[-1][0])).strftime('%c'))
predict(model,np.array([data_rows[-14:]]),np.array([0.0001]))
