clear all; clc; close all;
addpath("..\..\MATLAB\lib_function\");
Dataset = readmatrix('Valid_pulse_20rad.out', 'NumHeaderLines', 1, 'FileType', 'text');

data_Time          = Dataset(:,1);
data_Time_length   = length(data_Time);
data_Time_uniform  = linspace(data_Time(1), data_Time(end), data_Time_length)';

data_Wcmd          = Dataset(:,2);
data_Wgyro         = Dataset(:,5);

s           = tf('s');
Ts          = 1/ 200 ;

Km          = 11.8676/ 12.6778     ;   % [rad/(sec*V)]
Time_const  = 1 / 12.6778            ;   % [sec]
Gm          = Km/(Time_const*s + 1);

Hs          =c2d(Gm, Ts, 'tustin');

Wc                  = 20.0 * 2 * pi;
fft_sample_freq     = 200.0;

LPF_Wgyro                       = LPF_1D(data_Time_uniform, data_Wgyro, Wc);
[freq_axis_real, FFT_Wgyro]     = FFT_1D(data_Time_uniform, LPF_Wgyro, fft_sample_freq);


% -------------------------------------------------------------------------
% Simulink
% -------------------------------------------------------------------------
Tsample            = 0.005;
simin_Wgyro        = [data_Time_uniform LPF_Wgyro];
simin_Wcmd         = [data_Time_uniform data_Wcmd];

out = sim("Motor_model.slx");

simout_Wgyro        = out.simout;
simout_time         = out.time;
[sim_freq_axis, FFT_Wgyro_sim]       = FFT_1D(simout_time, simout_Wgyro, fft_sample_freq);




figure(1); hold on;
plot(data_Time_uniform, data_Wcmd, 'r-', LineWidth=2, DisplayName="Step [rad/s]");
plot(data_Time_uniform, LPF_Wgyro, 'b-', LineWidth=2, DisplayName="\omega_{real} [rad/s]");
plot(out.time, out.simout, 'g-', LineWidth=2, DisplayName="\omega_{simul} [rad/s]");
title("Motor Pulse response Mag: 20 [rad/s]");
xlabel("Time [sec]");
ylabel("\omega [rad/s]")
legend;
hold off; grid on;

figure(2); hold on;
plot(freq_axis_real, FFT_Wgyro, 'r-', LineWidth=2, DisplayName="\omega_{real} dB");
plot(sim_freq_axis, FFT_Wgyro_sim, 'b-', LineWidth=2, DisplayName="\omega_{sim} dB");
title("Motor Pulse response Mag: 20 [rad/s]");
xlabel("freq [Hz]");
ylabel("\Magnitude dB")
legend;
hold off; grid on;
