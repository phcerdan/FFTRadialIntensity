import plotly.plotly as py
import plotly.graph_objs as go

# Create random data with numpy
import numpy as np

N = 500
random_x = np.linspace(0, 1, N)
random_y = np.random.randn(N)

# Create a trace
trace = go.Scatter(
    x = random_x,
    y = random_y
)

line_x = [0.5, 0.5]
line_y = [min(random_y), max(random_y)]
a_line = go.Scatter(
        x = line_x,
        y = line_y,
        mode = "lines")

data = [trace, a_line]

plotly.offline.plot(data)
# py.iplot(data, filename='basic-line')
