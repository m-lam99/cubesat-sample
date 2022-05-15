from db.db import DB
from dash import Dash, html, dcc
from dash.dependencies import Input, Output
import plotly.express as px
import pandas as pd

app = Dash(__name__)

px.set_mapbox_access_token(
    'pk.eyJ1IjoiYWRhbWhhbW1vIiwiYSI6ImNqemM3YWN4cTAzMGwzY3FtMTJwNWsxMmMifQ.--bZnHXt1i_ajrFnpdlwOQ')

WOD_OPTIONS = {  # display name: (db column, y-axis, ylim)
    "Mode": ("mode", "", (0, 1)),
    "Battery Voltage": ("batteryVoltage", "field", (3, 15.75)),
    "Battery Current": ("batteryCurrent", "A", (-1, 1.008)),
    "3V3 Current": ("3V3Current", "A", (0, 6.375)),
    "5V Current": ("5VCurrent", "A", (0, 6.375)),
    "Comms Temperature": ("commTemperature", "C", (-15, 48.75)),
    "EPS Temperature": ("epsTemperature", "C", (-15, 48.75)),
    "Battery Temperature": ("batteryTemperature", "C", (-15, 48.75))
}
# I know there's probs an easier way to do this but i sack
# we probs wont even get marks for this
app.layout = html.Div([
    html.H1("NICE"),
    html.Div([
        html.Div([dcc.Input(type="datetime-local", step="1", id="starttime"), dcc.Input(type="datetime-local",
                                                                                        step="1", id="endtime")], style={'display': 'flex', 'flex-direction': 'row', "margin-bottom": "10px"}),
        dcc.Graph(id='map')
    ], style={"margin-bottom": "10px"}),
    html.Div([
        html.Div([
            dcc.Dropdown(list(WOD_OPTIONS.keys()),
                         "Battery Voltage", id='topleft-input'),
            dcc.Graph(id='topleft-chart', style={'height': '35vh'})
        ], className="chart", style={'width': '100%'}),
        html.Div([
            dcc.Dropdown(list(WOD_OPTIONS.keys()),
                         "Battery Current", id='topright-input'),
            dcc.Graph(id='topright-chart', style={'height': '35vh'})
        ], className="chart", style={'width': '100%'})
    ], className="row", style={'display': 'flex', 'flex-direction': 'row', 'fontSize': '16px'}),
    html.Div([
        html.Div([
            dcc.Dropdown(list(WOD_OPTIONS.keys()),
                         "5V Current", id='bottomleft-input'),
            dcc.Graph(id='bottomleft-chart', style={'height': '35vh'})
        ], className="chart", style={'width': '100%'}),
        html.Div([
            dcc.Dropdown(list(WOD_OPTIONS.keys()),
                         "Battery Temperature", id='bottomright-input'),
            dcc.Graph(id='bottomright-chart', style={'height': '35vh'})
        ], className="chart", style={'width': '100%'})
    ], className="row", style={'display': 'flex', 'flex-direction': 'row', 'fontSize': '16px'})
])


# app.css.append_css({'external_url': 'ihatecss.css'})

def new_graph(field, start_time, end_time):
    data_info = WOD_OPTIONS[field]
    db = DB()
    data = db.get_wod(data_info[0], start_time, end_time)
    xlabel = "timestamp"
    ylabel = f"{field} ({data_info[1]})"
    # https://community.plotly.com/t/valueerror-invalid-value-in-basedatatypes-py/55993/6
    # i swear to god
    try:
        fig = px.line(pd.DataFrame(
            data, columns=[xlabel, ylabel]), x=xlabel, y=ylabel)
    except Exception:
        fig = px.line(pd.DataFrame(
            data, columns=[xlabel, ylabel]), x=xlabel, y=ylabel)
    fig.update_layout(font={'size': 10}, margin={
                      'l': 0, 'r': 0, 't': 0, 'b': 0}, yaxis_range=data_info[2])
    return fig


@app.callback(
    Output("topleft-chart", "figure"),
    Input("topleft-input", "value"),
    Input("starttime", "value"),
    Input("endtime", "value")
)
def update_graph_topleft(field, start_time, end_time):
    return new_graph(field, start_time, end_time)


@app.callback(
    Output("topright-chart", "figure"),
    Input("topright-input", "value"),
    Input("starttime", "value"),
    Input("endtime", "value")
)
def update_graph_topright(field, start_time, end_time):
    return new_graph(field, start_time, end_time)


@app.callback(
    Output("bottomleft-chart", "figure"),
    Input("bottomleft-input", "value"),
    Input("starttime", "value"),
    Input("endtime", "value")
)
def update_graph_bottomleft(field, start_time, end_time):
    return new_graph(field, start_time, end_time)


@app.callback(
    Output("bottomright-chart", "figure"),
    Input("bottomright-input", "value"),
    Input("starttime", "value"),
    Input("endtime", "value")
)
def update_graph_bottomright(field, start_time, end_time):
    return new_graph(field, start_time, end_time)


@app.callback(
    Output("map", "figure"),
    Input("starttime", "value"),
    Input("endtime", "value")
)
def update_map(start_time, end_time):
    db = DB()
    data = db.get_science(start_time, end_time)
    try:
        fig = px.scatter_mapbox(data, lat="latitude", lon="longitude", color="sensor reading",
                                hover_name="timestamp", size="altitude", zoom=1, opacity=0.05, mapbox_style='satellite', size_max=10)
    except Exception:
        fig = px.scatter_mapbox(data, lat="latitude", lon="longitude", color="sensor reading",
                                hover_name="timestamp", size="altitude", zoom=1, opacity=0.05, mapbox_style='satellite', size_max=10)
    fig.update_layout(margin={'l': 0, 'r': 0, 't': 0, 'b': 0})

    return fig


def run():
    app.run_server(debug=True, dev_tools_props_check=False)
