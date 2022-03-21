% AERO4701 - Satellite Simulation
% Author: Group C
clear;
clc;
close all

start_time = datetime(2020,02,22,22,22,22);
stop_time = start_time + days(1);
sample_time = 60;

sc = satelliteScenario(start_time, stop_time, sample_time);

sat = satellite(sc,"nice.tle");