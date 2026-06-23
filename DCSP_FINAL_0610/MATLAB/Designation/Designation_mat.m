% subject: Digital control and signal Processing
% title: Designation
clc; clear all; close all;

%==========================================================================
% Read Dataset
%==========================================================================
addpath("lib_function\");
scaler_rad2deg   = 180.0/pi ; 

Tstep           = 0.001;    %[sec] for looking 0.005 [sec] sampling effect
Tsample         = 0.005;    %[sec] 
Tf              = 10;        %[sec]
time            = 0:Tstep:Tf;
target_angle    = 80.0; 

%==========================================================================
% Read Dataset
%==========================================================================
filename        = '..\..\Data\Designation_data\Designation_Step_80.out'; 
Dataset         = readmatrix(filename, 'FileType', 'text', 'NumHeaderLines', 1);

data_Time_raw        = data(1:end, 1);
data_time_length     = length(data_Time_raw);
data_time_uniform    = linspace(data_Time_raw(1), data_Time_raw(end), data_time_length)';

data_Angle_deg      = Dataset(:, 4); % Actual Angle [deg]
data_Wgyro_deg_sec  = Dataset(:, 5);

%==========================================================================
% Motor parameter
%==========================================================================
Km          = 10.7936 / 10.7132      ;   % [rad/(sec*V)]
Time_const  = 1 / 10.7132            ;   % [sec]
Gm          = Km / (Time_const*s + 1); % Motor bandwidth 2.7723 [Hz]



%==========================================================================
% Simin Values
%==========================================================================
simin_Angle         = [data_time_uniform data_Angle_deg];
simin_Wgyro         = [data_time_uniform data_Wgyro_deg_sec];
simin_step          = [data_time_uniform, target_angle*ones(data_time_length)];


data_Time          = Dataset(:,1);
data_Wgyro         = Dataset(:,4);
data_disturbance   = Dataset(:,5);


target_deg      = 80.0; 
target_90       = target_deg * 0.9;