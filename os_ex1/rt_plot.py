import plotly.graph_objects as go

# Define the data for the bar graph
actions = ['addition', 'CallEmptyFunctionWithoutArgs', 'SysCall']
local_times = [0.26291, 2.09366, 481.121] # running times in seconds
vm_times = [0.7634, 19.085, 874.466]
docker_times = [0.28601, 2.05273, 480.658]

# Create the bar graph
fig = go.Figure(data=[
    go.Bar(name='Local', x=actions, y=local_times),
    go.Bar(name='VM', x=actions, y=vm_times),
    go.Bar(name='Docker', x=actions, y=docker_times)
])

# Update the layout of the graph
fig.update_layout(title='Running times of Addition/FunctionCall/SysCall in 3 environments',
                  xaxis_title='Operations',
                  yaxis_title='RT(nano seconds)',
                  yaxis_type = "log",
                  barmode='group')

# Show the graph
fig.show()